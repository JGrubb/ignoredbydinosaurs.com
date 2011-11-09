<?php
// $Id: gmaps.distancefield-element.inc,v 1.1.2.5 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_distancefield element processors and validators
 */

/**
 * Converts array #default_value to string if necessary.
 */
function _gmaps_distancefield_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    return isset($element['#default_value']) ? (is_string($element['#default_value']) ? $element['#default_value'] : gmaps_length_format_plain($element['#default_value'], -1)) : '';
  }
}

/**
 * Expands a gmaps_distancefield.
 */
function _gmaps_distancefield_expand($element) {
  $element['#pre_render'][] = '_gmaps_distancefield_pre_render';
  return $element;
}

/**
 * Validates the gmaps_distancefield.
 */
function _gmaps_distancefield_validate($element, &$form_state) {
  if (!empty($element['#value'])) {
    $length = gmaps_length_parse($element['#value']);
    if ($length === FALSE) {
      form_error($element, t('The specified length is invalid.'));
    }
    else if ($length['value'] < 0 && !$element['#enable_negative']) {
      form_error($element, t('Negative values are not allowed.'));
    }
    else {
      $element['#value'] = $length;
      form_set_value($element, $element['#value'], $form_state);
    }
  }
}

function _gmaps_distancefield_pre_render($element) {
  static $help = NULL;
  static $title_help = NULL;
  $class = 'form-gmaps-distancefield';

  if (is_null($help)) {
    if (module_exists('advanced_help')) {
      $help = FALSE;
      $title_help = theme('gmaps_element_help_topic', 'gmaps', 'length-formatting');
    }
    else {
      $help = t('At most <strong>one</strong> space allowed between value and unit.');
      $fm_help = _gmaps_call_func('gmaps', 'advanced-help', '_gmaps_get_number_formatting_help');
      $help .= empty($fm_help) ? '' : ' '. $fm_help;
      $title_help = FALSE;
    }
  }

  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }

  if (isset($element['#description'])) {
    $element['#description'] = ($element['#description'] == '#disabled') ? '' : $element['#description'] . ($help ? "<p>$help</p>" : '');
  }
  else if ($help) {
    $element['#description'] = $help;
  }
  if ($title_help) {
    $element['#title'] = $title_help . $element['#title'];
  }
  return $element;
}




/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_distancefield element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used:  same as for textfield
 * @return
 *   A themed HTML string representing gmaps_distancefield as a textfield.
 *
 * @ingroup themeable
 */
function theme_gmaps_distancefield($element) {
  return theme('textfield', $element);
}
