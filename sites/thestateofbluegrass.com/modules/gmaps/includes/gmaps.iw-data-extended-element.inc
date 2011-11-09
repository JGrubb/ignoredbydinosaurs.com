<?php
// $Id: gmaps.iw-data-extended-element.inc,v 1.1.2.8 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * gmaps_iw_data_extended element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_iw_data_extended_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    foreach(array('nocloseonclick', 'style', 'collapsible', 'collapsed', 'tabs_nav', 'tabs_state') as $child) {
      if (!isset($value[$child])) {
        $value[$child] = GMAPS_INHERIT_VALUE;
      }
    }
    if (!isset($value['grid'])) {
      $value['grid'] = array();
    }
    if (!isset($value['grid']['alignment'])) {
      $value['grid']['alignment'] = GMAPS_INHERIT_VALUE;
    }
    return $value;
  }
}

/**
 * Expands a gmaps_iw_data_extended.
 */
function _gmaps_iw_data_extended_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  
  $element['class'] = array('#type' => 'textfield',
    '#title' => t('CSS class'),
    '#default_value' => $element['#value']['class'],
    '#disabled' => $element['#disabled'],
  );
  $element['beak_offset'] = array('#type' => 'textfield',
    '#title' => t('Beak offset'),
    '#default_value' => $element['#value']['beak_offset'],
    '#required' => $element['#required'],
    '#maxlength' => 4,
    '#size' => 6,
    '#field_suffix' => 'px',
    '#description' => t('The number of pixels to move the beak up.'),
    '#disabled' => $element['#disabled'],
  );
  $element['padding'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Padding'),
    '#default_value' => $element['#value']['padding'],
    '#required' => $element['#required'],
    '#return_value_keys' => 'xy',
    '#disabled' => $element['#disabled'],
  );
  $element['max_panning'] = array('#type' => 'textfield',
    '#title' => t('Maximum panning'),
    '#default_value' => $element['#value']['max_panning'],
    '#required' => $element['#required'],
    '#maxlength' => 4,
    '#size' => 6,
    '#field_suffix' => 'px',
    '#description' => t('The maximum panning distance when the marker is not in screen. Default is 500.'),
    '#disabled' => $element['#disabled'],
  );
  $element['nocloseonclick'] = array('#type' => 'select',
    '#title' => t('Do not close on map click'),
    '#default_value' => $element['#value']['nocloseonclick'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['style'] = array('#type' => 'select',
    '#title' => t('Render tabs as'),
    '#default_value' => $element['#value']['style'],
    '#options' => $inherit + _gmaps_map_tab_styles(),
    '#disabled' => $element['#disabled'],
  );
  $element['collapsible'] = array('#type' => 'select',
    '#title' => t('Collapsible'),
    '#default_value' => $element['#value']['collapsible'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['collapsed'] = array('#type' => 'select',
    '#title' => t('Collapsed'),
    '#default_value' => $element['#value']['collapsed'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['tabs_nav'] = array('#type' => 'select',
    '#title' => t('Tabs navigation position'),
    '#default_value' => $element['#value']['tabs_nav'],
    '#options' => $inherit + array(
      GMAPS_TABS_NAV_TOP => t('Top'),
      GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
      GMAPS_TABS_NAV_LEFT => t('Left'),
      GMAPS_TABS_NAV_RIGHT => t('Right'),
    ),
    '#disabled' => $element['#disabled'],
  );
  $element['tabs_state'] = array('#type' => 'select',
    '#title' => t('Show tabs in state'),
    '#default_value' => $element['#value']['tabs_state'],
    '#options' => $inherit + array('min' => t('Minimized'), 'max' => t('Maximized')),
    '#disabled' => $element['#disabled'],
  );
  $element['grid'] = array('#type' => 'fieldset',
    '#title' => t('Grid settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $element['grid']['columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of columns'),
    '#default_value' => $element['#value']['grid']['columns'],
    '#required' => $element['#required'],
    '#disabled' => $element['#disabled'],
  );
  $element['grid']['alignment'] = array(
    '#type' => 'radios',
    '#title' => t('Alignment'),
    '#options' => $inherit + array(GMAPS_GRID_ALIGN_HOR => t('Horizontal'), GMAPS_GRID_ALIGN_VER => t('Vertical')),
    '#default_value' => $element['#value']['grid']['alignment'],
    '#description' => t('Horizontal alignment will place items starting in the upper left and moving right. Vertical alignment will place items starting in the upper left and moving down.'),
    '#disabled' => $element['#disabled'],
  );
  
  $element['#pre_render'][] = '_gmaps_iw_data_extended_pre_render';

  return $element;
}

function _gmaps_iw_data_extended_validate($element, &$form_state) {
  $value = &$element['#value'];
  
  if (intval($value['beak_offset']) != $value['beak_offset']) {
    form_set_error($element['beak_offset'], t('The beak offset of the extended info window must be an integer.'));
  }
  if (intval($value['max_panning']) != $value['max_panning']) {
    form_set_error($element['max_panning'], t('The maximum panning of the extended info window must be an integer.'));
  }
  
  
  $grid = $value['grid'];
  if (intval($grid['columns']) != $grid['columns'] || $grid['columns'] < 1) {
    form_error($element['grid']['columns'], t('Grid columns must be an integer greater than or equal to one.'));
  }
  
  if (!$element['#required']) {
    foreach(element_children($element) as $child) {
      if ($child == 'grid') {
        if (empty($value['grid']['columns'])) {
          unset($value['grid']['columns']);
        }
        if ($value['grid']['alignment'] == GMAPS_INHERIT_VALUE) {
          unset($value['grid']['alignment']);
        }
        if (empty($value['grid'])) {
          unset($value['grid']);
        }
      }
      else if (in_array($child, array('class', 'beak_offset', 'padding', 'max_panning'))) {
        if (empty($value[$child])) {
          unset($value[$child]);
        }
      }
      else if ($value[$child] == GMAPS_INHERIT_VALUE) {
        unset($value[$child]);
      }
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_iw_data_extended_pre_render($element) {
  $class = 'form-gmaps-iw-data-extended';
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
 * Format a gmaps_iw_data_extended element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_iw_data_extended($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-iw-data-extended-collapsible');
}
