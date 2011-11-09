<?php
// $Id: gmaps_views_geotaxonomy.views.inc,v 1.1.2.1 2009/12/12 08:43:48 xmarket Exp $

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_views_geotaxonomy_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_views_geotaxonomy') . '/includes/handlers',
    ),
    'handlers' => array(
      'gmaps_views_geotaxonomy_handler_field_marker_point' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function gmaps_views_geotaxonomy_views_data_alter(&$data) {
  //marker point option
  $data['term_geo']['gmaps_marker_point'] = array(
    'group' => t('Marker option'),
    'title' => t('Geo Taxonomy - Point option'),
    'title short' => t('Point option'),
    'help' =>  t('Interactive and static marker option.'),
    'field' => array(
      'field' => 'lat',
      'handler' => 'gmaps_views_geotaxonomy_handler_field_marker_point',
      'click sortable' => FALSE,
    ),
  );
}
