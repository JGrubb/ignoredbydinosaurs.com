<?php
// $Id: gmaps.marker-tracker-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_marker_tracker element processors and validators
 */

/**
 * Set GMAPS_INHERIT_VALUE where necessary.
 */
function _gmaps_marker_tracker_value($element, $edit = FALSE) {
  if ($edit === FALSE && !$element['#required']) {
    $value = isset($element['#default_value']) ? $element['#default_value'] : array();
    $value['enabled'] = isset($value['enabled']) ? $value['enabled'] : GMAPS_INHERIT_VALUE;
    $value['quick_pan'] = isset($value['quick_pan']) ? $value['quick_pan'] : GMAPS_INHERIT_VALUE;
    return $value;
  }
}

/**
 * Expands a gmaps_marker_tracker.
 */
function _gmaps_marker_tracker_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $inherit = $element['#required'] ? array() : array(GMAPS_INHERIT_VALUE => $element['#inherit_label']);

  $element['enabled'] = array('#type' => 'select',
    '#title' => t('Enable tracker'),
    '#default_value' => $element['#value']['enabled'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['scale'] = array('#type' => 'textfield',
    '#title' => t('Icon scale'),
    '#default_value' => $element['#value']['scale'],
    '#required' => $element['#required'],
    '#size' => 6,
    '#maxlength' => 4,
    '#description' => t('A number between 0 (no icon) and 1.'),
    '#disabled' => $element['#disabled'],
  );
  $element['padding'] = array('#type' => 'textfield',
    '#title' => t('Padding'),
    '#default_value' => $element['#value']['padding'],
    '#required' => $element['#required'],
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => 'px',
    '#description' => t('The number of pixels between the arrow and the edge of the map.'),
    '#disabled' => $element['#disabled'],
  );
  $element['color'] = array('#type' => 'textfield',
    '#title' => t('Arrow color'),
    '#default_value' => $element['#value']['color'],
    '#required' => $element['#required'],
    '#size' => 10,
    '#disabled' => $element['#disabled'],
  );
  $element['weight'] = array('#type' => 'textfield',
    '#title' => t('Weight'),
    '#default_value' => $element['#value']['weight'],
    '#required' => $element['#required'],
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => 'px',
    '#description' => t('Thickness of the lines that make up the arrows.'),
    '#disabled' => $element['#disabled'],
  );
  $element['length'] = array('#type' => 'textfield',
    '#title' => t('Arrow length'),
    '#default_value' => $element['#value']['length'],
    '#required' => $element['#required'],
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => 'px',
    '#disabled' => $element['#disabled'],
  );
  $element['opacity'] = array('#type' => 'textfield',
    '#title' => t('Opacity'),
    '#default_value' => $element['#value']['opacity'],
    '#required' => $element['#required'],
    '#size' => 6,
    '#maxlength' => 4,
    '#description' => t('A number between 0 and 1.'),
    '#disabled' => $element['#disabled'],
  );
  $element['update_event'] = array('#type' => 'textfield',
    '#title' => t('Update event'),
    '#default_value' => $element['#value']['update_event'],
    '#required' => $element['#required'],
    '#description' => t('This parameter changes the GMap2 event name that triggers the arrows to update.'),
    '#disabled' => $element['#disabled'],
  );
  $element['quick_pan'] = array('#type' => 'select',
    '#title' => t('Enable quick pan'),
    '#default_value' => $element['#value']['quick_pan'],
    '#options' => $inherit + array(0 => t('Disabled'), 1 => t('Enabled')),
    '#disabled' => $element['#disabled'],
  );
  $element['pan_event'] = array('#type' => 'textfield',
    '#title' => t('Pan event'),
    '#default_value' => $element['#value']['pan_event'],
    '#required' => $element['#required'],
    '#description' => t('This parameter specifies a GMarker event name that triggers a quick pan to the tracked marker.'),
    '#disabled' => $element['#disabled'],
  );
  
  $element['#pre_render'][] = '_gmaps_marker_tracker_pre_render';

  return $element;
}

function _gmaps_marker_tracker_validate($element, &$form_state) {
  $value = &$element['#value'];

  if (!empty($value['scale']) && ($value['scale'] < 0 || $value['scale'] > 1)) {
    form_error($element['scale'], t('The marker tracker scale must be between zero and one.'));
  }
  if (!empty($value['opacity']) && ($value['opacity'] < 0 || $value['opacity'] > 1)) {
    form_error($element['opacity'], t('The marker tracker opacity must be between zero and one.'));
  }
  if (!empty($value['padding']) && (intval($value['padding']) != $value['padding'] || $value['padding'] < 0)) {
    form_error($element['padding'], t('The marker tracker padding must be an integer greater than or equal to zero.'));
  }
  if (!empty($value['weight']) && (intval($value['weight']) != $value['weight'] || $value['weight'] <= 0)) {
    form_error($element['weight'], t('The marker tracker weight must be an integer greater than zero.'));
  }
  if (!empty($value['length']) && (intval($value['length']) != $value['length'] || $value['length'] <= 0)) {
    form_error($element['length'], t('The marker tracker length must be an integer greater than zero.'));
  }
  
  if (!form_get_errors() && !$element['#required']) {
    foreach(element_children($element) as $child) {
      if (in_array($child, array('enabled', 'quick_pan'))) {
        if ($value[$child] == GMAPS_INHERIT_VALUE) {
          unset($value[$child]);
        }
      }
      else if (empty($value[$child])) {
          unset($value[$child]);
      }
    }
    form_set_value($element, $value, $form_state);
  }
}

function _gmaps_marker_tracker_pre_render($element) {
  $class = 'form-gmaps-marker-tracker';
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
 * Format a gmaps_marker_tracker element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string.
 *
 * @ingroup themeable
 */
function theme_gmaps_marker_tracker($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-marker-tracker-collapsible');
}
