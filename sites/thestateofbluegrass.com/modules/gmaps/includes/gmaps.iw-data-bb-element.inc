<?php
// $Id: gmaps.iw-data-bb-element.inc,v 1.1.2.6 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * gmaps_iw_data_bb element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_iw_data_bb_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    foreach(array('maximized', 'style', 'collapsible', 'collapsed', 'tabs_nav', 'tabs_state') as $child) {
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
 * Expands a gmaps_iw_data_bb.
 */
function _gmaps_iw_data_bb_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);
  
  $element['maximized'] = array('#type' => 'select',
    '#title' => t('Open in maximized state'),
    '#default_value' => $element['#value']['maximized'],
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
  
  $element['#pre_render'][] = '_gmaps_iw_data_bb_pre_render';

  return $element;
}

function _gmaps_iw_data_bb_validate($element, &$form_state) {
  $value = &$element['#value'];

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
      else if ($value[$child] == GMAPS_INHERIT_VALUE) {
        unset($value[$child]);
      }
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_iw_data_bb_pre_render($element) {
  $class = 'form-gmaps-iw-data-bb';
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
 * Format a gmaps_iw_data_bb element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_iw_data_bb($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-iw-data-bb-collapsible');
}
