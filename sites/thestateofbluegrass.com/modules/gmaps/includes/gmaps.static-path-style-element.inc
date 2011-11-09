<?php
// $Id: gmaps.static-path-style-element.inc,v 1.1.2.4 2010/01/29 13:21:02 xmarket Exp $

/**
 * @file
 * gmaps_static_path_style element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_static_path_style_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    return $value;
  }
}

/**
 * Expands a gmaps_static_path_style.
 */
function _gmaps_static_path_style_expand($element) {
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $element['color'] = array('#type' => 'textfield',
    '#title' => t('Color'),
    '#default_value' => $element['#value']['color'],
    '#required' => $element['#required'],
    '#maxlength' => 6,
    '#size' => 10,
    '#description' => t("Hexadecimal RGB color. Proposed value is 'ff0000'."),
    '#disabled' => $element['#disabled'],
  );
  $element['opacity'] = array('#type' => 'textfield',
    '#title' => t('Opacity'),
    '#default_value' => $element['#value']['opacity'],
    '#required' => $element['#required'],
    '#maxlength' => 2,
    '#size' => 5,
    '#description' => t("Hexadecimal value in range 00 - FF. Proposed value is '7f' in normal and 'ff' in sticky context."),
    '#disabled' => $element['#disabled'],
  );
  $element['weight'] = array('#type' => 'textfield',
    '#title' => t('Weight'),
    '#default_value' => $element['#value']['weight'],
    '#required' => $element['#required'],
    '#maxlength' => 3,
    '#size' => 5,
    '#field_suffix' => 'px',
    '#disabled' => $element['#disabled'],
    '#description' => t("Proposed value is '5' in normal and '10' in sticky context."),
  );
    
  $element['#pre_render'][] = '_gmaps_static_path_style_pre_render';

  return $element;
}

function _gmaps_static_path_style_validate($element, &$form_state) {
  $value = &$element['#value'];
  
  if (($element['#required'] || $value['color'] !== '') && !preg_match('|[0-9A-Fa-f]{6,6}|', $value['color'])) {
    form_error($element['color'], t('The color of the path must be given as a hexadecimal RGB value without a hashmark.'));
  }
  if (($element['#required'] || $value['opacity'] !== '') && !preg_match('|[0-9A-Fa-f]{2,2}|', $value['opacity'])) {
    form_error($element['opacity'], t('The opacity of the path must be given as a hexadecimal value without a hashmark.'));
  }
  if (($element['#required'] || $value['weight'] !== '') && (intval($value['weight']) != $value['weight'] || $value['weight'] <= 0)) {
    form_error($element['weight'], t('The weight of the path must be an integer greater than zero.'));
  }
  
  if (!form_get_errors() && !$element['#required']) {
    if (empty($value['color'])) {
      unset($value['color']);
    }
    if (empty($value['opacity'])) {
      unset($value['opacity']);
    }
    if (empty($value['weight'])) {
      unset($value['weight']);
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_static_path_style_pre_render($element) {
  $class = 'form-gmaps-static-path-style';
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
 * Format a gmaps_static_path_style element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_static_path_style($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-static-path-style-collapsible');
}
