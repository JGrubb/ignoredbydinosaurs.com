<?php
// $Id: gmaps_term_handler_field.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Address, Point and AnP field handler for Terms.
 */
class gmaps_term_handler_field extends views_handler_field_taxonomy {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = array('table' => 'term_data', 'field' => 'vid');
    $this->additional_fields['tid'] = array('table' => 'term_data', 'field' => 'tid');
  }
  
  function options(&$options) {
    parent::options($options);

    $gmaps = &$options['gmaps'];
      
    $data_type = $this->definition['gmaps_data_type'];
    
    $address = ($data_type == 'gmaps_address' || $data_type == 'gmaps_anp');
    $point = ($data_type == 'gmaps_point' || $data_type == 'gmaps_anp');
    
    if (empty($gmaps)) {
      $gmaps = array(
        'formatter' => 'hidden',
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
        $map_type = $address ? GMAPS_MAP_EARTH : $this->definition['gmaps_map_type'];
        $gmaps['map'] = array(
          'map' => gmaps_map_get_mid_for_default('term_display_'. $map_type),
          'static_map' => ($map_type == GMAPS_MAP_EARTH) ? gmaps_static_map_get_mid_for_default('term_display') : 0,
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

    $form['link_to_taxonomy']['#description'] .= ' '. t('Allowed only with plain text formatter.');
    
    $data_type = $this->definition['gmaps_data_type'];
    $address = ($data_type == 'gmaps_address' || $data_type == 'gmaps_anp');
    $map_type = $address ? GMAPS_MAP_EARTH : $this->definition['gmaps_map_type'];
    
    $form['gmaps'] = _gmaps_call_func('gmaps_term', 'views-admin', '_gmaps_term_handler_field_options_form', array($this->options['gmaps'], $map_type));
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
    $formatters = _gmaps_call_func('gmaps_term', 'views-admin', '_gmaps_term_geo_data_formatters');
    // Display the formatter name.
    if (isset($formatters[$this->options['gmaps']['formatter']])) {
      return $formatters[$this->options['gmaps']['formatter']];
    }
  }

  function click_sortable() {
    return !empty($this->definition['click sortable']) && $this->definition['gmaps_data_type'] != 'gmaps_point';
  }

  /**
   * Called to determine what to tell the clicksorter.
   */
  function click_sort($order) {
    $data_type = $this->definition['gmaps_data_type'];
    
    $a = ($data_type == 'gmaps_address' || $data_type == 'gmaps_anp');
    $p = ($data_type == 'gmaps_point' || $data_type == 'gmaps_anp');
    $nanp = !($a && $p);
    
    if ($a) {
      foreach($this->definition['additional fields'] as $alias => $info) {
        if ($nanp || strpos($alias, 'a_') === 0) {
          $this->query->add_orderby($this->table_alias, NULL, $order, $this->aliases[$alias]);
        }
      }
    }
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
    if ($this->options['gmaps']['formatter'] == 'hidden') {
      return;
    }
    
    $item = $this->_build_item_from_values($values);
    if (!$item) {
      return;
    }
    
    return $this->render_link(gmaps_term_geo_data_render($item, $this->definition['gmaps_data_type'], $this->options['gmaps']), $values);
  }
  
  function render_link($data, $values) {
    if ($this->options['gmaps']['formatter'] == 'plain') {
      return parent::render_link($data, $values);
    }
    else {
      return $data;
    }
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