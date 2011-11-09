<?php
// $Id: gmaps_cmo.views.inc,v 1.1.2.1 2010/02/18 16:36:22 xmarket Exp $

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_cmo_views_handlers() {
  $handlers = array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_cmo') . '/includes/handlers',
    ),
    'handlers' => array(

      // field handlers
      'gmaps_cmo_handler_marker_icon' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
  
  return $handlers;
}
