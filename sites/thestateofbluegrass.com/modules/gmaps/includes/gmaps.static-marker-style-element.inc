<?php
// $Id: gmaps.static-marker-style-element.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_static_marker_style element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_static_marker_style_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    $value['size'] = isset($value['size']) ? $value['size'] : GMAPS_INHERIT_VALUE;
    $value['color'] = isset($value['color']) ? $value['color'] : GMAPS_INHERIT_VALUE;
    return $value;
  }
}

/**
 * Expands a gmaps_static_marker_style.
 */
function _gmaps_static_marker_style_expand($element) {
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  $element['size'] = array('#type' => 'select',
    '#title' => t('Size'),
    '#default_value' => $element['#value']['size'],
    '#options' => $inherit + _gmaps_static_marker_sizes(),
    '#disabled' => $element['#disabled'],
  );
  $element['color'] = array('#type' => 'select',
    '#title' => t('Color'),
    '#default_value' => $element['#value']['color'],
    '#options' => $inherit + _gmaps_static_marker_colors(),
    '#disabled' => $element['#disabled'],
  );
    
  $element['#pre_render'][] = '_gmaps_static_marker_style_pre_render';

  return $element;
}

function _gmaps_static_marker_style_validate($element, &$form_state) {
  $value = &$element['#value'];
  if (!$element['#required']) {
    if ($value['size'] == GMAPS_INHERIT_VALUE) {
      unset($value['size']);
    }
    if ($value['color'] == GMAPS_INHERIT_VALUE) {
      unset($value['color']);
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_static_marker_style_pre_render($element) {
  $class = 'form-gmaps-static-marker-style';
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
 * Format a gmaps_static_marker_style element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_static_marker_style($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-static-marker-style-collapsible');
}
