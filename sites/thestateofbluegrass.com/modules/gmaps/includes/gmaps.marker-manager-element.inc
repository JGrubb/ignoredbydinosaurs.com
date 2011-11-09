<?php
// $Id: gmaps.marker-manager-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_marker_manager element processors and validators
 */

/**
 * Set defaults if necessary.
 */
function _gmaps_marker_manager_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array(
      'skip_route' => 0,
      'skip_manager' => 0,
      'minzoom' => 0,
      'maxzoom' => -1,
    );
    return $value;
  }
}

/**
 * Expands a gmaps_marker_manager.
 */
function _gmaps_marker_manager_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $info = gmaps_map_get_info($element['#map_type']);
  $zoom_levels = gmaps_map_get_zoom_levels($info);
  
  $element['skip_route'] = array('#type' => 'checkbox',
    '#title' => t('Skip this marker from route'),
    '#default_value' => $element['#value']['skip_route'],
    '#disabled' => $element['#disabled'],
  );
  $element['skip_manager'] = array('#type' => 'checkbox',
    '#title' => t('Do not manage this marker'),
    '#default_value' => $element['#value']['skip_manager'],
    '#disabled' => $element['#disabled'],
  );
  $element['minzoom'] = array('#type' => 'select',
    '#title' => t('Minimum zoom level'),
    '#default_value' => $element['#value']['minzoom'],
    '#options' => $zoom_levels,
    '#disabled' => $element['#disabled'],
  );
  $element['maxzoom'] = array('#type' => 'select',
    '#title' => t('Maximum zoom level'),
    '#default_value' => $element['#value']['maxzoom'],
    '#options' => array(-1 => t('Auto')) + $zoom_levels,
    '#disabled' => $element['#disabled'],
  );
  
  $element['#pre_render'][] = '_gmaps_marker_manager_pre_render';

  return $element;
}

function _gmaps_marker_manager_validate($element, &$form_state) {
  $value = &$element['#value'];
  
  if ($value['maxzoom'] >= 0 && $value['maxzoom'] < $value['minzoom']) {
    form_error($element['maxzoom'], t('Maximum zoom must be greater than or equal to minimum zoom.'));
  }
}

function _gmaps_marker_manager_pre_render($element) {
  $class = 'form-gmaps-marker-manager';
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }
  
  return $element;
}





/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_marker_manager element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_marker_manager($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-marker-manager-collapsible');
}
