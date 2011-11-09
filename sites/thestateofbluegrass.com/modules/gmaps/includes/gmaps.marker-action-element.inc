<?php
// $Id: gmaps.marker-action-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_marker_action element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_marker_action_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    foreach(array('hover', 'click', 'dblclick', 'rightclick') as $child) {
      if (!isset($value[$child])) {
        $value[$child] = GMAPS_INHERIT_VALUE;
      }
    }
    return $value;
  }
}

/**
 * Expands a gmaps_marker_action.
 */
function _gmaps_marker_action_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  $actions = $inherit + array('' => t('Disabled'), 'link' => t('Link'), 'content' => t('Content'), 'blowup' => t('Blowup map'), 'svu' => t('Street view'));
  $element['hover'] = array('#type' => 'select',
    '#title' => t('Mouse over action'),
    '#default_value' => $element['#value']['hover'],
    '#options' => $actions,
    '#disabled' => $element['#disabled'],
  );
  $element['click'] = array('#type' => 'select',
    '#title' => t('Click action'),
    '#default_value' => $element['#value']['click'],
    '#options' => $actions,
    '#disabled' => $element['#disabled'],
  );
  $element['dblclick'] = array('#type' => 'select',
    '#title' => t('Double click action'),
    '#default_value' => $element['#value']['dblclick'],
    '#options' => $actions,
    '#disabled' => $element['#disabled'],
  );
  $element['rightclick'] = array('#type' => 'select',
    '#title' => t('Right click action'),
    '#default_value' => $element['#value']['rightclick'],
    '#options' => $actions,
    '#disabled' => $element['#disabled'],
  );
  
  $element['#pre_render'][] = '_gmaps_marker_action_pre_render';

  return $element;
}

function _gmaps_marker_action_validate($element, &$form_state) {
  $value = &$element['#value'];
  if (!$element['#required']) {
    foreach(element_children($element) as $child) {
      if ($value[$child] == GMAPS_INHERIT_VALUE) {
        unset($value[$child]);
      }
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_marker_action_pre_render($element) {
  $class = 'form-gmaps-marker-action';
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
 * Format a gmaps_marker_action element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_marker_action($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-marker-action-collapsible');
}
