<?php
// $Id: gmaps_content_handler_field.inc,v 1.1.2.3 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * The subclass adds basic field and formatter info,
 * for field-specific subclasses to use if they need to.
 *
 * Fields could extend this class if they want field and formatter handling
 * but don't want the multiple value grouping options created by
 * content_handler_field_multiple.
 */
class gmaps_content_handler_field extends views_handler_field_node {
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

    // Override views_handler_field_node's default label
    $options['label'] = '';
    $options['label_type'] = 'widget';
    $options['format'] = 'default';

    $field = $this->get_content_field();
    
    $gmaps = &$options['gmaps']['formatter'];
      
    $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
    $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');
    
    if (empty($gmaps)) {
      $gmaps = array(
        'map_links' => array(
          'google_find' => 0,
          'google_directions_from' => 0,
          'google_directions_to' => 0,
          'yahoo_find' => 0,
          'yahoo_directions_from' => 0,
          'yahoo_directions_to' => 0,
          'mapquest_find' => 0,
        ),
      );
      
      if ($address && $point) {
        $gmaps['show'] = 'anp';
      }
      
      if ($address) {
        $gmaps['address'] = array(
          'user_settings' => TRUE,
          'country' => 'name',
          'state' => '',
          'flag' => '',
          'taxonomy_links' => 0,
        );
        $gmaps['map_links']['mapquest_directions_from'] = 0;
        $gmaps['map_links']['mapquest_directions_to'] = 0;
      }
      
      if ($point) {
        $gmaps['point'] = array(
          'user_settings' => TRUE,
          'format' => 'dd',
          'elevation' => 1,
          'dd_prec' => 6,
          'dms_prec' => 0,
          'cart_prec' => 2,
          'elevation_prec' => 0,
        );
        $gmaps['map'] = array(
          'map' => gmaps_map_get_mid_for_default('field_display_'. ($field['gmaps']['map_type'] ? $field['gmaps']['map_type'] : GMAPS_MAP_EARTH)),
          'static_map' => gmaps_static_map_get_mid_for_default('field_display'),
          'cache' => 0,
          'content' => 'default',
        );
      }
    }
  }

  /**
   * Provide formatter option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['link_to_node']['#description'] .= ' '. t('Allowed only with plain text formatter.');
    
    $field = $this->get_content_field();
    $options = $this->options;

    $form['label_type'] = array(
      '#title' => t('Label'),
      '#type' => 'radios',
      '#options' => array(
        'none' => t('None'),
        'widget' => t('Widget label (@label)', array('@label' => $field['widget']['label'])),
        'custom' => t('Custom'),
      ),
      '#default_value' => $options['label_type'],
      '#weight' => 2,
    );
    $form['label'] = array(
      '#title' => t('Custom label'),
      '#type' => 'textfield',
      '#default_value' => $options['label'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('radio:options[label_type]' => array('custom')),
      '#weight' => 3,
     );

    $field_types = _content_field_types();
    $formatters = array();
    if (is_array($field_types[$field['type']]['formatters'])) {
      foreach ($field_types[$field['type']]['formatters'] as $name => $info) {
        $formatters[$name] = $info['label'];
      }
    }
    $form['format'] = array(
      '#title' => t('Format'),
      '#type' => 'select',
      '#options' => $formatters,
      '#required' => TRUE,
      '#default_value' => $options['format'],
      '#weight' => 4,
    );

    $form['gmaps'] = array('#type' => 'fieldset',
      '#title' => t('GMaps settings'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($options['gmaps']),
      '#tree' => TRUE,
      '#weight' => 5,
    );
    
    $widget = $field['widget'];
    $widget['gmaps']['formatter'] = $options['gmaps']['formatter'];
    
    $form['gmaps'] += _gmaps_call_func('gmaps_content', 'admin', '_gmaps_content_widget_settings_form_formatter', array($widget));
    $form['gmaps']['formatter']['#type'] = 'markup';
    
    if (isset($form['gmaps']['formatter']['map'])) {
      $map_form = &$form['gmaps']['formatter']['map'];
      $map_type = $field['gmaps']['map_type'];
      
      $map_form['cache'] = array('#type' => 'value', '#value' => 0);
      
      $map_form['map']['#options'] = gmaps_map_get_options($map_type);
      
      if ($map_type == GMAPS_MAP_EARTH) {
        $map_form['static_map']['#options'] = gmaps_static_map_get_options();
      }
      else {
        $map_form['static_map'] = array('#type' => 'value', '#value' => 0);
      }
    }
  }


  /**
   * Make sure some value is stored as a label.
   *
   * Don't use t(), since Views' views_handler_field already has
   * $this->options['label'] marked as a translatable field.
   *
   * @see http://drupal.org/node/285470
   */
  function options_submit($form, &$form_state) {
    switch ($form_state['values']['options']['label_type']) {
      case 'none':
        $form_state['values']['options']['label'] = '';
        break;
      case 'widget':
        $field = $this->get_content_field();
        $form_state['values']['options']['label'] = $field['widget']['label'];
        break;
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

  /**
   * Provide text for the administrative summary
   */
  function admin_summary() {
    // Display the formatter name.
    $field = $this->get_content_field();
    $field_types = _content_field_types();
    if (isset($field_types[$field['type']]['formatters'][$this->options['format']])) {
      return t($field_types[$field['type']]['formatters'][$this->options['format']]['label']);
    }
  }

  function click_sortable() {
    $field = $this->get_content_field();
    return !empty($this->definition['click sortable']) && $field['type'] != 'gmaps_point';
  }

  /**
   * Called to determine what to tell the clicksorter.
   */
  function click_sort($order) {
    foreach(_gmaps_address_get_parts() as $part => $label) {
      $this->query->add_orderby($this->table_alias, NULL, $order, $this->aliases[$part]);
    }
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
    if (!$field) {
      return '';
    }
    $options = $this->options;

    $item = $this->_build_item_from_values($values, $field);
    if (!$item) {
      return '';
    }
    
    // Build a pseudo-node from the retrieved values.
    $node = drupal_clone($values);
    $node->type = $values->{$this->aliases['type']};
    $node->nid = $values->{$this->aliases['nid']};
    $node->vid = $values->{$this->aliases['vid']};
    $node->uid = $values->{$this->aliases['uid']};
    // Some formatters need to behave differently depending on the build_mode
    // (for instance: preview), so we provide one.
    $node->build_mode = NODE_BUILD_NORMAL;

    $item['#delta'] = $field['multiple'] ?  $values->{$this->aliases['delta']} : 0;
    $node->{$field['field_name']}[$item['#delta']] = $item;

    // Render items.
    $formatter_name = $options['format'];
    if ($formatter = _content_get_formatter($formatter_name, $field['type'])) {
      $field['gmaps'] = array_merge($field['gmaps'], $options['gmaps']);
      $field['widget']['gmaps'] = array_merge($field['widget']['gmaps'], $options['gmaps']);
      if (content_handle('formatter', 'multiple values', $formatter) == CONTENT_HANDLE_CORE) {
        // Single-value formatter.
        $output = gmaps_content_format($field, $item, $formatter_name, $node);
      }
      else {
        // Multiple values formatter - we actually have only one value to display.
        $output = gmaps_content_format($field, array($item), $formatter_name, $node);
      }
      return $this->render_link($output, $values);
    }
    return '';
  }
  
  function render_link($data, $values) {
    if ($this->options['format'] == 'plain') {
      return parent::render_link($data, $values);
    }
    else {
      return $data;
    }
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
    
    return array('value' => $item);
  }

}