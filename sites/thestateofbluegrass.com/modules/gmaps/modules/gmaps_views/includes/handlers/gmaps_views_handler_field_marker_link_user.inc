<?php
// $Id: gmaps_views_handler_field_marker_link_user.inc,v 1.1.2.2 2010/02/26 14:13:03 xmarket Exp $

/**
 * @file
 * Link option for interactive user markers.
 */
class gmaps_views_handler_field_marker_link_user extends views_handler_field_user implements gmaps_views_map_overlay_option_marker {
  function admin_summary() {
    return '';
  }
  
  function set_overlay_option(&$gmi, &$overlay, $values) {
    $overlay['link'] = url('user/'. $values->{$this->field_alias});
  }
}