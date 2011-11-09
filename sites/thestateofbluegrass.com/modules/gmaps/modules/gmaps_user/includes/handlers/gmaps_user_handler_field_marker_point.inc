<?php
// $Id: gmaps_user_handler_field_marker_point.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

/**
 * @file
 * Point option for user markers.
 */
class gmaps_user_handler_field_marker_point extends views_handler_field implements gmaps_views_map_overlay_option_marker_point, gmaps_views_static_map_overlay_option_marker_point {
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
    return $this->definition['map_type'];
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
    $point = new stdClass();
    
    foreach($this->definition['additional fields'] as $alias => $info) {
      $value = $values->{$this->aliases[$alias]};
      $point->{$info['field']} = ($info['serialize'] && !empty($value)) ? unserialize($value) : $value;
    }
    $point->uid = $values->uid;
    
    if (gmaps_point_empty($point)) {
      return NULL;
    }
    
    return $point;
  }

}