<?php
// $Id: gmaps.pixels-element.inc,v 1.1.2.5 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_pixels element processors and validators
 */

/**
 * Value callback for gmaps_pixels element.
 */
function _gmaps_pixels_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    return isset($element['#default_value']) ? gmaps_pixels_render($element['#default_value']) : '';
  }
}

/**
 * Expands a gmaps_distance element.
 */
function _gmaps_pixels_expand($element) {
  $element['#pre_render'][] = '_gmaps_pixels_pre_render';
  return $element;
}

/**
 * Validates a gmaps_pixels element.
 */
function _gmaps_pixels_validate($element, &$form_state) {
  if (!empty($element['#value']) || $element['#value'] == '0') {
    $pixels = gmaps_pixels_parse($element['#value']);
    if ($pixels === FALSE) {
      form_error($element, t('Invalid pixel format.'));
    }
    else if (!$element['#enable_negative'] && ($pixels[0] < 0 || $pixels[1] < 0)) {
      form_error($element, t('Negative values are not allowed.'));
    }
    else if (isset($element['#min_pixels']) && ($min = gmaps_pixels_parse($element['#min_pixels'])) && ($pixels[0] < $min[0] || $pixels[1] < $min[1])) {
      form_error($element, t('Too small values. Minimum values are @min.', array('@min' => gmaps_pixels_render($min, FALSE))));
    }
    else if (isset($element['#max_pixels']) && ($max = gmaps_pixels_parse($element['#max_pixels'])) && ($pixels[0] > $max[0] || $pixels[1] > $max[1])) {
      form_error($element, t('Too large values. Maximum values are @max.', array('@max' => gmaps_pixels_render($max, FALSE))));
    }
    else {
      //if #default_value is an array or object, pass back the new value using the same keys
      if (isset($element['#return_value_keys'])) {
        $pixels = gmaps_pixels_parse($pixels, $element['#return_value_keys']);
      }
      //pass back the new value
      $element['#value'] = $pixels;
      form_set_value($element, $element['#value'], $form_state);
    }
  }
}

function _gmaps_pixels_pre_render($element) {
  static $help = NULL;
  static $title_help = NULL;
  $class = 'form-gmaps-pixels';

  if (is_null($help)) {
    if (module_exists('advanced_help')) {
      $help = FALSE;
      $title_help = theme('gmaps_element_help_topic', 'gmaps', 'pixel-formatting');
    }
    else {
      $help = t('A pair of pixels values in the format of 123x456.');
      $title_help = FALSE;
    }
  }

  //a few hardcoded properties
  $element['#maxlength'] =  11;
  $element['#size'] = 19;
  $element['#field_suffix'] = 'px';

  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }

  if (isset($element['#description'])) {
    $element['#description'] = ($element['#description'] == 'disabled') ? '' : $element['#description'] .(!empty($help) ? "<p>$help</p>" : '');
  }
  else if (!empty($help)) {
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
 * Format a gmaps_pixels element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used:  same as for textfield
 * @return
 *   A themed HTML string representing gmaps_pixels as a textfield.
 *
 * @ingroup themeable
 */
function theme_gmaps_pixels($element) {
  return theme('textfield', $element);
}
