<?php
// $Id: gmaps_views_geo.views.inc,v 1.1.2.1 2009/12/12 08:43:48 xmarket Exp $

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_views_geo_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_views_geo') . '/includes/handlers',
    ),
    'handlers' => array(
      'gmaps_views_geo_handler_field_marker_point' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data_alter().
 */
function gmaps_views_geo_views_data_alter(&$data) {
  $fields = content_fields();
  foreach($fields as $field_name => $field) {
    if ($field['type'] == 'geo') {
      $table = content_views_tablename($field);
      $db_field = $field_name . '_wkt';
      //marker point option
      $data[$table]['gmaps_marker_point'] = array(
        'group' => t('Marker option'),
        'title' => t('Point option - @label (!name)', array('@label' => t($field['widget']['label']), '!name' => $field['field_name'])),
        'title short' => t('Point option - @label', array('@label' => t($field['widget']['label']))),
        'help' =>  t('Interactive and static marker option.'),
        'field' => array(
          'field' => $db_field,
          'handler' => 'gmaps_views_geo_handler_field_marker_point',
          'click sortable' => FALSE,
        ),
      );
    }
  }
}
