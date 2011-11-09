<?php
// $Id: gmaps_content_handler_field_map_links.inc,v 1.1.2.2 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * The subclass adds basic field and formatter info,
 * for field-specific subclasses to use if they need to.
 *
 * Fields could extend this class if they want field and formatter handling
 * but don't want the multiple value grouping options created by
 * content_handler_field_multiple.
 */
class gmaps_content_handler_field_map_links extends views_handler_field_node {
  protected $content_field;

  function construct() {
    parent::construct();
  }

  function init(&$view, $options) {
    parent::init($view, $options);
    $field = $this->get_content_field();
    
    if ($view->base_table == 'node_revisions') {
      $this->additional_fields['vid'] = array('table' => 'node_revisions', 'field' => 'vid');
      $this->additional_fields['uid'] = array('table' => 'node_revisions', 'field' => 'uid');
    }
    else {
      $this->additional_fields['vid'] = array('table' => 'node', 'field' => 'vid');
      $this->additional_fields['uid'] = array('table' => 'node', 'field' => 'uid');
    }
    
    if ($field['multiple']) {
      $this->additional_fields['delta'] = 'delta';
    }
    // Make sure we grab enough information to build a pseudo-node with enough
    // credentials at render-time.
    $this->additional_fields['type'] = array('table' => 'node', 'field' => 'type');
    $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
  }

  protected function get_content_field() {
    if (is_null($this->content_field)) {
      $this->content_field = content_fields($this->definition['content_field_name']);
    }
    return $this->content_field;
  }
  
  function options(&$options) {
    parent::options($options);

    $field = $this->get_content_field();
    
    $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
    
    $options['gmaps'] = array(
      'google_find' => 1,
      'google_directions_from' => 1,
      'google_directions_to' => 1,
      'yahoo_find' => 0,
      'yahoo_directions_from' => 0,
      'yahoo_directions_to' => 0,
      'mapquest_find' => 0,
    );

    if ($address) {
      $options['gmaps']['mapquest_directions_from'] = 0;
      $options['gmaps']['mapquest_directions_to'] = 0;
    }
  }

  /**
   * Provide formatter option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['link_to_node'] = array('#type' => 'value', '#value' => FALSE);
    
    $field = $this->get_content_field();
    $options = $this->options;

    $form['gmaps'] = array('#type' => 'fieldset',
      '#title' => t('GMaps settings'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($options['gmaps']),
      '#tree' => TRUE,
      '#weight' => 5,
    );
    
    $widget = $field['widget'];
    $widget['gmaps']['formatter']['map_links'] = $options['gmaps'];
    
    $formatter_form = _gmaps_call_func('gmaps_content', 'admin', '_gmaps_content_widget_settings_form_formatter', array($widget));
    $form['gmaps'] += $formatter_form['formatter']['map_links'];
  }

  function options_validate($form, &$form_state) {
    if (!array_filter($form_state['values']['options']['gmaps'])) {
      form_set_error('options][gmaps', t('You should select at least one map link to render.'));
    }
  }
  
  /**
   * Return DIV or SPAN based upon the field's element type.
   */
  function element_type() {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    
    return 'div';
  }

  function click_sortable() {
    return FALSE;
  }
  
  /**
   * Called to determine what to tell the clicksorter.
   */
  function click_sort($order) {
  }

  function query() {
    parent::query();
    
    $field = $this->get_content_field();
    if ($field['type'] == 'gmaps_anp' && !array_key_exists('gcanp_'. $this->table_alias, $this->view->query->where)) {
      $a = $this->definition['address_table_alias'];
      $p = $this->table_alias;
      $delta_sql = $field['multiple'] ? '' : " AND $p.delta = 0";
      $sql = "($p.vid IS NULL OR ($p.vid = $a.vid AND $p.field_name = $a.field_name AND $p.delta = $a.delta AND $p.field_name = '%s'$delta_sql))";
      $this->view->query->add_where('gcanp_'. $this->table_alias, $sql, $field['field_name']);
    }
  }

  function render($values) {
    // We're down to a single node here, so we can retrieve the actual field
    // definition for the node type being considered.
    $field = $this->get_content_field();
    $field = content_fields($field['field_name'], $values->{$this->aliases['type']});
    $options = $this->options;

    $item = $this->_build_item_from_values($values, $field);
    if (!$item) {
      return '';
    }
    
    if ($links = gmaps_anp_map_links($item, $options['gmaps'])) {
      return theme('item_list', $links);
    }
    
    return '';
  }
  
  private function _build_item_from_values($values, $field) {
    $a = in_array($field['type'], array('gmaps_address', 'gmaps_anp'));
    $p = in_array($field['type'], array('gmaps_point', 'gmaps_anp'));
    $nanp = !($a && $p);
    $item = new stdClass();
    
    if ($a) {
      $address = new stdClass();
      foreach($this->definition['additional fields'] as $alias => $info) {
        if ($nanp || strpos($alias, 'a_') === 0) {
          $value = $values->{$this->aliases[$alias]};
          $address->{$info['field']} = ($info['serialize'] && !empty($value)) ? unserialize($value) : $value;
        }
      }
      $address->uid = $values->{$this->aliases['uid']};
      $item->address = $address;
      if ($nanp && gmaps_address_empty($address)) {
        return NULL;
      }
    }
    
    if ($p) {
      $point = new stdClass();
      foreach($this->definition['additional fields'] as $alias => $info) {
        if ($nanp || strpos($alias, 'p_') === 0) {
          $value = $values->{$this->aliases[$alias]};
          $point->{$info['field']} = ($info['serialize'] && !empty($value)) ? unserialize($value) : $value;
        }
      }
      $point->uid = $values->{$this->aliases['uid']};
      $item->point = $point;
      if ($nanp && gmaps_point_empty($point)) {
        return NULL;
      }
    }
    
    if (!$nanp) {
      $item->uid = $item->point->uid;
      $item->search = $item->point->search;
      if (gmaps_anp_empty($item)) {
        return NULL;
      }
    }
    
    return $item;
  }

}