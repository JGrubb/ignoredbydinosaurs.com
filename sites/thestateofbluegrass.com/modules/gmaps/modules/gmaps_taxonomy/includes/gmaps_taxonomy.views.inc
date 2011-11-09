<?php
// $Id: gmaps_taxonomy.views.inc,v 1.1.2.1 2009/11/23 13:23:48 xmarket Exp $

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_taxonomy_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_taxonomy') . '/includes/handlers',
    ),
    'handlers' => array(

      // argument handlers
      'gmaps_taxonomy_handler_argument_location_term' => array(
        'parent' => 'views_handler_argument',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_plugins
 */
function gmaps_taxonomy_views_plugins() {
  return array(
    'module' => 'gmaps_taxonomy', // This just tells our themes are elsewhere.
    'argument validator' => array(
      'gmaps_location_term' => array(
        'title' => t('GMaps location term'),
        'handler' => 'gmaps_taxonomy_plugin_argument_validate_location_term',
        'path' => drupal_get_path('module', 'gmaps_taxonomy') . '/includes/handlers',
        'type' => 'gmaps location term',
      ),
    ),
    'cache' => array(
      'gmaps_taxonomy' => array(
        'title' => t('GMaps Taxonomy'),
        'help' => t('GMaps Taxonomy page cache based caching.'),
        'handler' => 'gmaps_taxonomy_plugin_cache',
        'path' => drupal_get_path('module', 'gmaps_taxonomy') . '/includes/handlers',
      ),
    ),
  );
}

function gmaps_taxonomy_break_phrase($str, $filter = NULL) {
  if (!$filter) {
    $filter = new stdClass();
  }
  
  if (!empty($filter->value) && !empty($filter->operator)) {
    return $filter;
  }
  
  // Set up defaults:
  if (!isset($filter->value)) {
    $filter->value = array();
  }

  if (!isset($filter->operator)) {
    $filter->operator = 'or';
  }

  $terms = gmaps_taxonomy_location_terms_parse($str);
  if (!$terms) {
    return $filter;
  }
  
  $filter->value = $terms['locations'];
  $filter->operator = $terms['operator'];
  if ($terms['continent']) {
    $filter->continent = $terms['continent'];
  }

  return $filter;
}

