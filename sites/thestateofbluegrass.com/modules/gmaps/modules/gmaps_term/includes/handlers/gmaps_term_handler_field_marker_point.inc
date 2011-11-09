<?php
// $Id: gmaps_term_handler_field_marker_point.inc,v 1.1.2.1 2010/03/18 10:52:17 xmarket Exp $

/**
 * @file
 * Point option for map markers.
 */
class gmaps_term_handler_field_marker_point extends views_handler_field implements gmaps_views_map_overlay_option_marker_point, gmaps_views_static_map_overlay_option_marker_point {
  protected $content_field;

  function construct() {
    parent::construct();
  }

  function init(&$view, $options) {
    parent::init($view, $options);
  }

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

  function query() {
    parent::query();
    
    if (!array_key_exists('gt_map_type_'. $this->table_alias, $this->view->query->where)) {
      $p = $this->table_alias;
      $sql = "($p.map_type = '%s')";
      $this->view->query->add_where('gt_map_type_'. $this->table_alias, $sql, $this->definition['map_type']);
    }
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
    
    if (gmaps_point_empty($point)) {
      return NULL;
    }
    
    return $point;
  }

}