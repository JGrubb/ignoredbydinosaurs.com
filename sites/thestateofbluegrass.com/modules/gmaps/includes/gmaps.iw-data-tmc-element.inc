<?php
// $Id: gmaps.iw-data-tmc-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_iw_data_tmc element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_iw_data_tmc_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    $value['nocloseonclick'] = isset($value['nocloseonclick']) ? $value['nocloseonclick'] : GMAPS_INHERIT_VALUE;
    $value['maximized'] = isset($value['maximized']) ? $value['maximized'] : GMAPS_INHERIT_VALUE;
    return $value;
  }
}

/**
 * Expands a gmaps_iw_data_tmc.
 */
function _gmaps_iw_data_tmc_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  
  $element['maxwidth'] = array('#type' => 'textfield',
    '#title' => t('Maximum width'),
    '#default_value' => $element['#value']['maxwidth'],
    '#required' => $element['#required'],
    '#maxlength' => 4,
    '#size' => 6,
    '#field_suffix' => 'px',
    '#description' => t('Use 0 (zero) for unlimited.'),
    '#disabled' => $element['#disabled'],
  );
  $element['nocloseonclick'] = array('#type' => 'select',
    '#title' => t('Do not close on map click'),
    '#default_value' => $element['#value']['nocloseonclick'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['offset'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Offset'),
    '#default_value' => $element['#value']['offset'],
    '#required' => $element['#required'],
    '#return_value_keys' => 'xy',
    '#description' => t('Specifies a number of pixels in the up (x) and right (y) direction to move the infowindow away from the given GLatLng.'),
    '#disabled' => $element['#disabled'],
  );
  $element['maximized'] = array('#type' => 'select',
    '#title' => t('Open in maximized state'),
    '#default_value' => $element['#value']['maximized'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['class'] = array('#type' => 'textfield',
    '#title' => t('CSS class'),
    '#default_value' => $element['#value']['class'],
    '#disabled' => $element['#disabled'],
  );

  $element['#pre_render'][] = '_gmaps_iw_data_tmc_pre_render';

  return $element;
}

function _gmaps_iw_data_tmc_validate($element, &$form_state) {
  $value = &$element['#value'];
  
  if (!empty($value['maxwidth']) && (intval($value['maxwidth']) != $value['maxwidth'] || $value['maxwidth'] < 0)) {
    form_error($element['maxwidth'], t('The maximum width of the API default info window must be an integer greater than or equal to zero.'));
  }

  if (!form_get_errors() && !$element['#required']) {
    if (empty($value['maxwidth'])) {
      unset($value['maxwidth']);
    }
    if ($value['nocloseonclick'] == GMAPS_INHERIT_VALUE) {
      unset($value['nocloseonclick']);
    }
    if (empty($value['offset'])) {
      unset($value['offset']);
    }
    if ($value['maximized'] == GMAPS_INHERIT_VALUE) {
      unset($value['maximized']);
    }
    if (empty($value['class'])) {
      unset($value['class']);
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_iw_data_tmc_pre_render($element) {
  $class = 'form-gmaps-iw-data-tmc';
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
 * Format a gmaps_iw_data_tmc element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_iw_data_tmc($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-iw-data-tmc-collapsible');
}
