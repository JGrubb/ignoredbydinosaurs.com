<?php
// $Id: gmaps_views_location.views.inc,v 1.1.2.1 2009/12/12 08:43:48 xmarket Exp $

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_views_location_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_views_location') . '/includes/handlers',
    ),
    'handlers' => array(
      'gmaps_views_location_handler_field_marker_point' => array(
        'parent' => 'location_views_handler_field_coordinates',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function gmaps_views_location_views_data_alter(&$data) {
  //marker point option
  $data['location']['gmaps_marker_point'] = array(
    'group' => t('Marker option'),
    'title' => t('Location - Point option'),
    'title short' => t('Point option'),
    'help' =>  t('Interactive and static marker option.'),
    'field' => array(
      'field' => 'latitude',
      'handler' => 'gmaps_views_location_handler_field_marker_point',
      'click sortable' => FALSE,
    ),
  );
}
