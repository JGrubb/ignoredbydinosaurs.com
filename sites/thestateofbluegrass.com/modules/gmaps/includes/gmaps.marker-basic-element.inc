<?php
// $Id: gmaps.marker-basic-element.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_marker_basic element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_marker_basic_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    foreach(array('icon', 'protected_icon', 'info_window') as $child) {
      if (!isset($value[$child])) {
        $value[$child] = GMAPS_INHERIT_VALUE;
      }
    }
    return $value;
  }
}

/**
 * Expands a gmaps_marker_basic.
 */
function _gmaps_marker_basic_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  
  $icons = gmaps_get_icons();
  
  $element['icon'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Icon'),
    '#default_value' => $element['#value']['icon'],
    '#icons' => $inherit + array(GMAPS_ICON_API_DEFAULT => t('API default')) + $icons,
    '#disabled' => $element['#disabled'],
  );
  $element['protected_icon'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Protected icon'),
    '#default_value' => $element['#value']['protected_icon'],
    '#icons' => $inherit + array(GMAPS_ICON_API_DEFAULT => t('API default')) + $icons,
    '#disabled' => $element['#disabled'],
  );
  $element['info_window'] = array('#type' => 'select',
    '#title' => t('Info window'),
    '#default_value' => $element['#value']['info_window'],
    '#options' => $inherit + gmaps_map_get_iw_options(),
    '#disabled' => $element['#disabled'],
  );
  
  $element['#pre_render'][] = '_gmaps_marker_basic_pre_render';

  return $element;
}

function _gmaps_marker_basic_validate($element, &$form_state) {
  $value = &$element['#value'];

  if (!form_get_errors() && !$element['#required']) {
    foreach(element_children($element) as $child) {
      if ($value[$child] == GMAPS_INHERIT_VALUE) {
        if ($child == 'info_window') {
          unset($value[$child]);
        }
        else {
          $value[$child] = GMAPS_ICON_MAP_DEFAULT;
        }
      }
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_marker_basic_pre_render($element) {
  $class = 'form-gmaps-marker-basic';
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
 * Format a gmaps_marker_basic element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_marker_basic($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-marker-basic-collapsible');
}
