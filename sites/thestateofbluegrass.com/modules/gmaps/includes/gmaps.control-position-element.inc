<?php
// $Id: gmaps.control-position-element.inc,v 1.1.2.1 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * gmaps_control_position element processors and validators
 */

/**
 * Value callback for gmaps_control_position element.
 */
function _gmaps_control_position_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    return !empty($element['#default_value']) ? $element['#default_value'] : array('anchor' => GMAPS_CONTROL_POSITION_DEFAULT, 'offest' => '');
  }
  return $edit;
}

/**
 * Expands a gmaps_control_position element.
 */
function _gmaps_control_position_expand($element) {
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $element['anchor'] = array('#type' => 'select',
    '#title' => t('Anchored to corner'),
    '#default_value' => $element['#value']['anchor'],
    '#options' => array(
      GMAPS_CONTROL_POSITION_DEFAULT => t('Default'),
      GMAPS_CONTROL_POSITION_TOP_LEFT => t('Top-left'),
      GMAPS_CONTROL_POSITION_TOP_RIGHT => t('Top-right'),
      GMAPS_CONTROL_POSITION_BOTTOM_LEFT => t('Bottom-left'),
      GMAPS_CONTROL_POSITION_BOTTOM_RIGHT => t('Bottom-right'),
    ),
    '#disabled' => $element['#disabled'],
  );
  $element['offset'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Offset'),
    '#default_value' => $element['#value']['offset'],
    '#disabled' => $element['#disabled'],
    '#enable_negative' => FALSE,
    '#return_value_keys' => 'wh',
    '#description' => t('Leave empty for default offset.'),
  );
    
  $element['#pre_render'][] = '_gmaps_control_position_pre_render';
  
  return $element;
}

/**
 * Validates a gmaps_control_position element.
 */
function _gmaps_control_position_validate($element, &$form_state) {
  $value = &$element['#value'];
  $value['offset'] = gmaps_pixels_parse($value['offset'], $element['offset']['#return_value_keys']);
  $element['#value'] = $value;
  form_set_value($element, $element['#value'], $form_state);
}

function _gmaps_control_position_pre_render($element) {
  $class = 'form-gmaps-control-position';
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
 * Format a gmaps_control_position element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing a gmaps_control_position element.
 *
 * @ingroup themeable
 */
function theme_gmaps_control_position($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-control-position-collapsible');
}
