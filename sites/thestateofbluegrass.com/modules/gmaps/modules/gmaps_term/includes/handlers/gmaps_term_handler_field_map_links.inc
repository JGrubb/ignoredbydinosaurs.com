<?php
// $Id: gmaps_term_handler_field_map_links.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Map link handler.
 */
class gmaps_term_handler_field_map_links extends views_handler_field_taxonomy {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = array('table' => 'term_data', 'field' => 'vid');
    $this->additional_fields['tid'] = array('table' => 'term_data', 'field' => 'tid');
  }
  
  function options(&$options) {
    parent::options($options);

    $data_type = $this->definition['gmaps_data_type'];
    
    $address = ($data_type == 'gmaps_address' || $data_type == 'gmaps_anp');
        
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

    $form['link_to_taxonomy'] = array('#type' => 'value', '#value' => FALSE);
    
    $data_type = $this->definition['gmaps_data_type'];
    $address = ($data_type == 'gmaps_address' || $data_type == 'gmaps_anp');
    $map_type = $address ? GMAPS_MAP_EARTH : $this->definition['gmaps_map_type'];
    
    $form['gmaps'] = array('#type' => 'fieldset',
      '#title' => t('GMaps settings'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($this->options['gmaps']),
      '#tree' => TRUE,
      '#weight' => 5,
    );
    
    $options = array('map_links' => $this->options['gmaps']);
    $formatter_form = _gmaps_call_func('gmaps_term', 'views-admin', '_gmaps_term_handler_field_options_form', array($options, $map_type));
    $form['gmaps'] += $formatter_form['map_links'];
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
    
    if ($this->definition['gmaps_data_type'] == 'gmaps_anp' && !array_key_exists('gtanp_'. $this->table_alias, $this->view->query->where)) {
      $a = $this->definition['gmaps_address_table'];
      $p = $this->table_alias;
      $sql = "($p.tid IS NULL OR ($p.tid = $a.tid))";
      $this->view->query->add_where('gtanp_'. $this->table_alias, $sql);
    }
  }

  function render($values) {
    $item = $this->_build_item_from_values($values);
    if (!$item) {
      return '';
    }
    
    if ($links = gmaps_anp_map_links($item, $this->options['gmaps'])) {
      return theme('item_list', $links);
    }
    
    return '';
  }
  
  private function _build_item_from_values($values) {
    $data_type = $this->definition['gmaps_data_type'];
    
    $a = ($data_type == 'gmaps_address' || $data_type == 'gmaps_anp');
    $p = ($data_type == 'gmaps_point' || $data_type == 'gmaps_anp');
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
      if ($nanp && gmaps_address_empty($address)) {
        return NULL;
      }
      $address->tid = $values->{$this->aliases['tid']};
      $address->uid = 0;
      $item->address = $address;
      if (!$p) {
        $item->tid = $item->address->tid;
        $item->uid = $item->address->uid;
        $item->search = $item->address->search;
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
      if ($nanp && gmaps_point_empty($point)) {
        return NULL;
      }
      $point->tid = $values->{$this->aliases['tid']};
      $point->uid = 0;
      $item->point = $point;
      $item->tid = $item->point->tid;
      $item->uid = $item->point->uid;
      $item->search = $item->point->search;
    }
    
    if (!$nanp && gmaps_anp_empty($item)) {
      return NULL;
    }
    
    return $item;
  }

}