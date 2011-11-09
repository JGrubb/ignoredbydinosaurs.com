<?php
// $Id: gmaps_views_handler_field_map_center.inc,v 1.1.2.1 2010/03/22 14:41:55 xmarket Exp $

class gmaps_views_handler_field_map_center extends views_handler_field implements gmaps_views_map_option, gmaps_views_static_map_option {
  private static $geoip;
  private static $refloc;

  function option_definition() {
    $options = parent::option_definition();
    $options['exclude'] = array('default' => FALSE);
    
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['exclude'] = array('#type' => 'value', '#value' => FALSE);
    $form['label']['#access'] = FALSE;
    $form['alter']['#access'] = FALSE;
    $form['empty']['#access'] = FALSE;
    $form['empty_zero']['#access'] = FALSE;
    $form['hide_empty']['#access'] = FALSE;
  }
  
  protected function _get_location() {
    if ($this->options['field'] == 'gmaps_map_center_refloc') {
      if (is_null(self::$refloc)) {
        self::$refloc = gmaps_get_reference_location();
        if (empty(self::$refloc)) {
          self::$refloc = FALSE;
        }
      }
      return self::$refloc;
    }
    else if ($this->options['field'] == 'gmaps_map_center_geoip') {
      if (is_null(self::$geoip)) {
        self::$geoip = gmaps_get_geoip_location();
        if (empty(self::$geoip)) {
          self::$geoip = FALSE;
        }
      }
      return self::$geoip;
    }
    else {
      return $this->view->gmaps_viewloc;
    }
  }
  
  function click_sortable() {
    return FALSE;
  }

  function click_sort($order) {
  }

  function admin_summary() {
  }
  
  function query() {
    // do nothing -- to override the parent query.
  }

  function render($values) {
    return;
  }
  
  function set_map_option(&$gmi) {
    $loc = $this->_get_location();
    if (!$loc || gmaps_point_empty($loc->point)) {
      return;
    }
    
    if ($gmi->base->type == $loc->point->map_type) {
      $gmi->base->center = $loc->point;
    }
  }
  
  function set_static_map_option(&$smi) {
    $this->set_map_option($smi);
  }
}