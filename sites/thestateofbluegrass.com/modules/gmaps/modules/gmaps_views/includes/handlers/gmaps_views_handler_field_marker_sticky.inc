<?php
// $Id: gmaps_views_handler_field_marker_sticky.inc,v 1.1.2.1 2009/12/03 12:29:46 xmarket Exp $

/**
 * @file
 * Sticky option for interactive and static markers.
 */
class gmaps_views_handler_field_marker_sticky extends views_handler_field_boolean implements gmaps_views_map_overlay_option_marker, gmaps_views_static_map_overlay_option_marker {
  function admin_summary() {
    return '';
  }
  
  function set_overlay_option(&$gmi, &$overlay, $values) {
    $overlay['sticky'] = $values->{$this->field_alias};
  }
  
  function set_static_overlay_option(&$smi, &$overlay, $values) {
    $this->set_overlay_option($smi, $overlay, $values);
  }
}