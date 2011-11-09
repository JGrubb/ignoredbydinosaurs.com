<?php
// $Id: gmaps_views_geo_handler_field_marker_point.inc,v 1.1.2.1 2009/12/12 08:43:48 xmarket Exp $

/**
 * @file
 * Point option for markers.
 */
class gmaps_views_geo_handler_field_marker_point extends views_handler_field implements gmaps_views_map_overlay_option_marker_point, gmaps_views_static_map_overlay_option_marker_point {
  function option_definition() {
    $options = parent::option_definition();
    $options['exclude'] = array('default' => FALSE);
    
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['exclude'] = array('#type' => 'value', '#value' => FALSE);
  }
  
  function admin_summary() {
    return '';
  }
  
  function click_sortable() {
    FALSE;
  }

  function render($values) {
    return '';
  }
  
  function get_map_type() {
    return GMAPS_MAP_EARTH;
  }
  
  function set_overlay_option(&$gmi, &$overlay, $values) {
    $point = $this->_build_point_from_values($values);
    if (!$point) {
      return;
    }
    
    $overlay['point'] = $point;
  }
  
  function set_static_overlay_option(&$smi, &$overlay, $values) {
    $this->set_overlay_option($smi, $overlay, $values);
  }
  
  private function _build_point_from_values($values) {
    static $backend;
    $wkt = $values->{$this->field_alias};
    
    if (is_null($backend)) {
      $backend = geo('backend_type');
      if (!$backend) {
        $backend = FALSE;
      }
    }
    if (!$backend) {
      return;
    }
    
    $point = new stdClass();
    
    switch ($backend) {
      case 'mysql_spatial':
        if (preg_match('|point\s*\(\s*(\d+[0-9.]*)\s+(\d+[0-9.]*)\)|i', $wkt, $matches)) {
          $point->latitude = $matches[2];
          $point->longitude = $matches[1];
        }
        break;
      case 'postgis':
        if (preg_match('|\(\s*(\d+[0-9.]*)\s*,\s*(\d+[0-9.]*)\)|', $wkt, $matches)) {
          $point->latitude = $matches[2];
          $point->longitude = $matches[1];
        }
        break;
    }
    
    $point->elevation = 0;
    $point->map_type = GMAPS_MAP_EARTH;
    
    if (gmaps_point_empty($point)) {
      return NULL;
    }
    
    return $point;
  }

}