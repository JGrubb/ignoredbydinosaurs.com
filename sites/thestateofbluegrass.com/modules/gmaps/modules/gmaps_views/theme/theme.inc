<?php
// $Id: theme.inc,v 1.1.2.1 2009/12/03 12:29:46 xmarket Exp $

/**
 * @file
 * Theme and theme preprocess functions for gmaps_views's themeables.
 */

/**
 * Theme function for GMaps Accordion and GMaps Tabs style plugins.
 */
function theme_gmaps_views_view_element($view, $options, $rows, $title) {
  if ($view->gmaps_element) {
    return drupal_render($view->gmaps_element);
  }
  return '';
}

/**
 * Theme function for GMaps Accordion and GMaps Tabs row style plugins.
 */
function theme_gmaps_views_row_element($view, $options, $row, $field_alias) {
  if ($view->gmaps_element) {
    return drupal_render($view->gmaps_element);
  }
  return '';
}

/**
 * Preprocess theme function to print a single accordion/tab panel from a piece of row, with fields
 */
function template_preprocess_gmaps_views_view_fields(&$vars) {
  $vars['fields'] = $vars['view']->gmaps_fields ? $vars['view']->gmaps_fields : array();
}

/**
 * Preprocess theme function to print a map item
 */
function template_preprocess_gmaps_views_view_map(&$vars) {
  $vars['gmaps_map_item'] = $vars['view']->gmaps_map_item ? $vars['view']->gmaps_map_item : '';
}

/**
 * Preprocess theme function to print a static map item
 */
function template_preprocess_gmaps_views_view_static_map(&$vars) {
  $vars['gmaps_static_map_item'] = $vars['view']->gmaps_static_map_item ? $vars['view']->gmaps_static_map_item : '';
}

/**
 * Template helper for theme_views_view_row_node
 */
function template_preprocess_gmaps_views_view_row_marker_node(&$vars) {
  $options = $vars['options'];

  // Make sure the variables are defined.
  $vars['node'] = '';
  $vars['comments'] = '';

  $nid = $vars['row']->{$vars['field_alias']};
  if (!is_numeric($nid)) {
    return;
  }

  $node = node_load($nid);

  if (empty($node)) {
    return;
  }

  $node->gmaps = (object)array('view' => $vars['view'], 'options' => $opts);
  $node->build_mode = ($options['build_mode'] == 'teaser' || $options['build_mode'] == 'full') ? NODE_BUILD_NORMAL : $options['build_mode'];
  $vars['node'] = node_view($node, $options['build_mode'] == 'teaser', FALSE, $options['links']);

  if (!empty($options['comments']) && function_exists('comment_render')) {
    $vars['comments'] = comment_render($node);
  }
}

