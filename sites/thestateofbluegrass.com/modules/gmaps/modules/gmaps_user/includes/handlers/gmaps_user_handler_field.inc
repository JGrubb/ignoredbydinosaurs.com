<?php
// $Id: gmaps_user_handler_field.inc,v 1.1.2.2 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Address, Point and AnP field handler for Users.
 */
class gmaps_user_handler_field extends views_handler_field_user {
  function options(&$options) {
    parent::options($options);

    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    $gmaps = &$options['gmaps'];
      
    $address = ($geo['type'] == 'gmaps_address' || $geo['type'] == 'gmaps_anp');
    $point = ($geo['type'] == 'gmaps_point' || $geo['type'] == 'gmaps_anp');
    
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
          'profile_links' => 0,
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
          'map' => gmaps_map_get_mid_for_default('user_display_'. $geo['options']['map_type']),
          'static_map' => gmaps_static_map_get_mid_for_default('user_display'),
          'cache' => 0,
          'cache_expires' => '',
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

    $form['link_to_user']['#description'] .= ' '. t('Allowed only with plain text formatter.');
    
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    $geo['formatter'] = $this->options['gmaps'];
    
    $form['gmaps'] = _gmaps_call_func('gmaps_user', 'admin', '_gmaps_user_geo_data_formatter_form', array($geo, TRUE));
    $form['gmaps']['#title'] = t('GMaps settings');
    
    if (isset($form['gmaps']['map'])) {
      $map_form = &$form['gmaps']['map'];
      $map_type = $geo['options']['map_type'];
      
      $map_form['cache'] = array('#type' => 'value', '#value' => 0);
      $map_form['cache_expires'] = array('#type' => 'value', '#value' => '');
      
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
    $formatters = _gmaps_call_func('gmaps_user', 'admin', '_gmaps_user_geo_data_formatters');
    // Display the formatter name.
    if (isset($formatters[$this->options['gmaps']['formatter']])) {
      return $formatters[$this->options['gmaps']['formatter']];
    }
  }

  function click_sortable() {
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return FALSE;
    }
    
    return !empty($this->definition['click sortable']) && $geo['type'] != 'gmaps_point';
  }

  /**
   * Called to determine what to tell the clicksorter.
   */
  function click_sort($order) {
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    $a = in_array($geo['type'], array('gmaps_address', 'gmaps_anp'));
    $p = in_array($geo['type'], array('gmaps_point', 'gmaps_anp'));
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
    
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    if ($geo['type'] == 'gmaps_anp' && !array_key_exists('guanp_'. $this->table_alias, $this->view->query->where)) {
      $a = $this->definition['address_table_alias'];
      $p = $this->table_alias;
      $sql = "($p.uid IS NULL OR ($p.uid = $a.uid))";
      $this->view->query->add_where('guanp_'. $this->table_alias, $sql);
    }
  }

  function render($values) {
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type']) || $this->options['gmaps']['formatter'] == 'hidden') {
      return;
    }
    
    $item = $this->_build_item_from_values($values, $geo);
    if (!$item) {
      return;
    }
    
    $account = (object)array('gmaps' => array('geo' => $item));
    $geo['formatter'] = $this->options['gmaps'];
    
    return $this->render_link(gmaps_user_profile_format($account, $geo), $values);
  }
  
  function render_link($data, $values) {
    if ($this->options['gmaps']['formatter'] == 'plain') {
      return parent::render_link($data, $values);
    }
    else {
      return $data;
    }
  }
  
  private function _build_item_from_values($values, $geo) {
    $a = in_array($geo['type'], array('gmaps_address', 'gmaps_anp'));
    $p = in_array($geo['type'], array('gmaps_point', 'gmaps_anp'));
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