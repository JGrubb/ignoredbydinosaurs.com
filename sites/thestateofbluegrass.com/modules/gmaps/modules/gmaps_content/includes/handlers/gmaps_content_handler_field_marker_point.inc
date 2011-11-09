<?php
// $Id: gmaps_content_handler_field_marker_point.inc,v 1.1.2.2 2009/12/12 08:43:48 xmarket Exp $

/**
 * @file
 * Point option for map markers.
 */
class gmaps_content_handler_field_marker_point extends views_handler_field implements gmaps_views_map_overlay_option_marker_point, gmaps_views_static_map_overlay_option_marker_point {
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
    $field = $this->get_content_field();
    $field = content_fields($field['field_name'], $values->{$this->aliases['type']});
    if (!$field) {
      return;
    }
    $options = $this->options;

    $point = $this->_build_point_from_values($values, $field);
    if (!$point) {
      return;
    }
    
    $overlay['point'] = $point;
  }
  
  function set_static_overlay_option(&$smi, &$overlay, $values) {
    $this->set_overlay_option($smi, $overlay, $values);
  }
  
  private function _build_point_from_values($values, $field) {
    $point = new stdClass();
    
    foreach($this->definition['additional fields'] as $alias => $info) {
      $value = $values->{$this->aliases[$alias]};
      $point->{$info['field']} = ($info['serialize'] && !empty($value)) ? unserialize($value) : $value;
    }
    $point->uid = $values->{$this->aliases['uid']};
    
    if (gmaps_point_empty($point)) {
      return NULL;
    }
    
    return $point;
  }

}