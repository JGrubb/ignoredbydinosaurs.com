<?php
// $Id: gmaps.static-marker-protected-element.inc,v 1.1.2.4 2010/01/29 13:21:02 xmarket Exp $

/**
 * @file
 * gmaps_static_marker_protected element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_static_marker_protected_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    return $value;
  }
}

/**
 * Expands a gmaps_static_marker_protected.
 */
function _gmaps_static_marker_protected_expand($element) {
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $element['quality'] = array('#type' => 'textfield',
    '#title' => t('Quality'),
    '#default_value' => $element['#value']['quality'],
    '#required' => $element['#required'],
    '#maxlength' => 3,
    '#size' => 5,
    '#description' => t("Numer of points to make up the circle. Proposed value is '16'."),
    '#disabled' => $element['#disabled'],
  );
  $element['style'] = array('#type' => 'gmaps_static_path_style',
    '#default_value' => $element['#value']['style'],
    '#required' => $element['#required'],
    '#collapsible' => FALSE,
    '#disabled' => $element['#disabled'],
  );
    
  $element['#pre_render'][] = '_gmaps_static_marker_protected_pre_render';

  return $element;
}

function _gmaps_static_marker_protected_validate($element, &$form_state) {
  $value = &$element['#value'];
  
  if (($element['#required'] || $value['quality'] !== '') && (intval($value['quality']) != $value['quality'] || $value['quality'] < 5 || $value['quality'] > 360)) {
    form_error($element['quality'], t('The quality of the circle must be an integer between 5 and 360.'));
  }
  
  if (!form_get_errors() && !$element['#required']) {
    if (empty($value['quality'])) {
      unset($value['quality']);
    }
    if (empty($value['style']['color'])) {
      unset($value['style']['color']);
    }
    if (empty($value['style']['opacity'])) {
      unset($value['style']['opacity']);
    }
    if (empty($value['style']['weight'])) {
      unset($value['style']['weight']);
    }
    if (empty($value['style'])) {
      unset($value['style']);
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_static_marker_protected_pre_render($element) {
  $class = 'form-gmaps-static-marker-protected';
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
 * Format a gmaps_static_marker_protected element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_static_marker_protected($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-static-marker-protected-collapsible');
}
