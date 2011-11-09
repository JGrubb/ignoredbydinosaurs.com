<?php
// $Id: gmaps.marker-options-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_marker_options element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_marker_options_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    foreach(array('clickable', 'draggable', 'hide', 'autopan', 'dcmove', 'bouncy') as $child) {
      if (!isset($value[$child])) {
        $value[$child] = GMAPS_INHERIT_VALUE;
      }
    }
    return $value;
  }
}

/**
 * Expands a gmaps_marker_options.
 */
function _gmaps_marker_options_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  
  $element['clickable'] = array('#type' => 'select',
    '#title' => t('Clickable'),
    '#default_value' => $element['#value']['clickable'],
    '#options' => $inherit + array(-1 => t('Auto'), 0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
    '#description' => t('When set to auto, then a marker is clickable only, if it is not draggable and has any action.'),
  );
  $element['draggable'] = array('#type' => 'select',
    '#title' => t('Draggable'),
    '#default_value' => $element['#value']['draggable'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['hide'] = array('#type' => 'select',
    '#title' => t('Hide'),
    '#default_value' => $element['#value']['hide'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['autopan'] = array('#type' => 'select',
    '#title' => t('Auto pan'),
    '#default_value' => $element['#value']['autopan'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['dcmove'] = array('#type' => 'select',
    '#title' => t('Drag cross move'),
    '#default_value' => $element['#value']['dcmove'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
    '#description' => t('When dragging markers normally, the marker floats up and away from the cursor. Setting this value to true  keeps the marker underneath the cursor, and moves the cross downwards instead.'),
  );
  $element['bouncy'] = array('#type' => 'select',
    '#title' => t('Bouncy'),
    '#default_value' => $element['#value']['bouncy'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
    '#description' => t('Toggles whether or not the marker should bounce up and down after it finishes dragging.'),
  );
  $element['gravity'] = array('#type' => 'textfield',
    '#title' => t('Bounce gravity'),
    '#default_value' => $element['#value']['gravity'],
    '#required' => $element['#required'],
    '#disabled' => $element['#disabled'],
    '#maxlength' => 3,
    '#size' => 5,
    '#description' => t('When finishing dragging, this number is used to define the acceleration rate of the marker during the bounce down to earth.'),
  );
  
  $element['#pre_render'][] = '_gmaps_marker_options_pre_render';

  return $element;
}

function _gmaps_marker_options_validate($element, &$form_state) {
  $value = &$element['#value'];

  if (!empty($value['gravity']) && (intval($value['gravity']) != $value['gravity'] || $value['gravity'] < 0)) {
    form_error($element['gravity'], t('The marker bounce gravity must be an integer greater than or equal to zero.'));
  }

  if (!form_get_errors() && !$element['#required']) {
    foreach(element_children($element) as $child) {
      if ($child != 'gravity' && $value[$child] == GMAPS_INHERIT_VALUE) {
        unset($value[$child]);
      }
    }
    if (empty($value['gravity'])) {
      unset($value['gravity']);
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_marker_options_pre_render($element) {
  $class = 'form-gmaps-marker-options';
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
 * Format a gmaps_marker_options element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_marker_options($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-marker-options-collapsible');
}
