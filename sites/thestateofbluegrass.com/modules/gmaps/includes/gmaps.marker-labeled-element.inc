<?php
// $Id: gmaps.marker-labeled-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_marker_labeled (Labeled marker options) element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_marker_labeled_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    $value['enabled'] = isset($value['enabled']) ? $value['enabled'] : GMAPS_INHERIT_VALUE;
    return $value;
  }
}

/**
 * Expands a gmaps_marker_labeled.
 */
function _gmaps_marker_labeled_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  $element['enabled'] = array('#type' => 'select',
    '#title' => t('Enable labeled marker'),
    '#default_value' => $element['#value']['enabled'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['class'] = array('#type' => 'textfield',
    '#title' => t('Label CSS class'),
    '#default_value' => $element['#value']['class'],
    '#required' => $element['#required'],
    '#disabled' => $element['#disabled'],
  );
  $element['offset'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Offset'),
    '#default_value' => $element['#value']['offset'],
    '#required' => $element['#required'],
    '#enable_negative' => TRUE,
    '#return_value_keys' => 'xy',
    '#description' => t('The x- and y-distance between the marker\'s anchor and the upper-left corner of the text div.'),
    '#disabled' => $element['#disabled'],
  );
  
  $element['#pre_render'][] = '_gmaps_marker_labeled_pre_render';

  return $element;
}

function _gmaps_marker_labeled_validate($element, &$form_state) {
  $value = &$element['#value'];
  if (!$element['#required']) {
    if ($value['enabled'] == GMAPS_INHERIT_VALUE) {
      unset($value['enabled']);
    }
    if (empty($value['class'])) {
      unset($value['class']);
    }
    if (empty($value['offset'])) {
      unset($value['offset']);
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_marker_labeled_pre_render($element) {
  $class = 'form-gmaps-marker-labeled';
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
 * Format a gmaps_marker_labeled element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_marker_labeled($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-marker-labeled-collapsible');
}
