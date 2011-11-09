<?php
// $Id: gmaps_views_handler_field_marker_link_node.inc,v 1.1.2.2 2010/02/26 14:13:03 xmarket Exp $

/**
 * @file
 * Link option for interactive content markers.
 */
class gmaps_views_handler_field_marker_link_node extends views_handler_field_node implements gmaps_views_map_overlay_option_marker {
  function admin_summary() {
    return '';
  }
  
  function set_overlay_option(&$gmi, &$overlay, $values) {
    $overlay['link'] = url('node/'. $values->{$this->field_alias});
  }
}