<?php
// $Id: gmaps.distance-element.inc,v 1.1.2.4 2009/08/20 06:15:10 xmarket Exp $

/**
 * @file
 * gmaps_distance element processors and validators
 */

/**
 * Converts string #default_value to array if necessary.
 */
function _gmaps_distance_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    return isset($element['#default_value']) ? (is_array($element['#default_value']) ? $element['#default_value'] : gmaps_length_parse($element['#default_value'], -1)) : '';
  }
}

/**
 * Expands a gmaps_distance.
 */
function _gmaps_distance_expand($element) {
  if (!isset($element['#default_value']) || !is_array($element['#default_value'])) {
    $element['#default_value'] =  array();
  }
  $element['#tree'] = TRUE;
  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $element['value'] = array('#type' => 'textfield',
    '#default_value' => _gmaps_number_format($element['#default_value']['value'], -1),
    '#disabled' => $element['#disabled'],
    '#required' => $element['#required'],
    '#attributes' => array('class' => 'form-gmaps-distance-value'),
  );
  //to be compatible with distancefield decrease the size by an average select width
  if (isset($element['#size'])) {
    $element['value']['#size'] = $element['#size'] - 10;
  }

  $element['unit'] = array('#type' => 'select',
    '#default_value' => $element['#default_value']['unit'],
    '#options' => gmaps_length_get_unit_options(),
    '#disabled' => $element['#disabled'],
    '#attributes' => array('class' => 'form-gmaps-distance-unit'),
  );

  $element['#pre_render'][] = '_gmaps_distance_pre_render';

  return $element;
}

/**
 * Validates a gmaps_distance element.
 */
function _gmaps_distance_validate($element, &$form_state) {
  if (!gmaps_length_empty($element['#value'])) {
    $number = _gmaps_number_parse($element['#value']['value']);
    if ($number === FALSE) {
      form_error($element['value'], t('The specified number is invalid.'));
    }
    else if ($number < 0 && !$element['#enable_negative']) {
      form_error($element['value'], t('Negative values are not allowed.'));
    }
    else {
      //pass back the parsed number
      $element['#value']['value'] = $number;
      form_set_value($element, $element['#value'], $form_state);
    }
  }
}

function _gmaps_distance_pre_render($element) {
  static $help = NULL;
  static $title_help = NULL;

  if (is_null($help)) {
    if (module_exists('advanced_help')) {
      $help = FALSE;
      $title_help = theme('gmaps_element_help_topic', 'gmaps', 'number-formatting');
    }
    else {
      $help = _gmaps_call_func('gmaps', 'advanced-help', '_gmaps_get_number_formatting_help');
      $title_help = FALSE;
    }
  }

  if (isset($element['#description'])) {
    $element['#description'] = ($element['#description'] == '#disabled') ? '' : $element['#description'] .(!empty($help) ? "<p>$help</p>" : '');
  }
  else if (!empty($help)) {
    $element['#description'] = $help;
  }
  if ($title_help) {
    $element['#title'] = $title_help . $element['#title'];
  }

  //ahah support
  //event overwrites must come in an array, keyed by the part names
  if (isset($element['#ahah'])) {
    //custom event
    if (isset($element['#ahah']['event'])) {
      $ahah = $element['#ahah'];
      $event = $ahah['event'];
      //custom for value part
      if (isset($event['value'])) {
        //not disabled
        if (!empty($event['value'])) {
          $element['value']['#ahah'] = array_merge($ahah, array('event' => $event['value']));
        }
      }
      //default for value part
      else {
        $element['value']['#ahah'] = $ahah;
        unset($element['value']['#ahah']['event']);
      }
      //custom for unit part
      if (isset($event['unit'])) {
        //not disabled
        if (!empty($event['unit'])) {
          $element['unit']['#ahah'] = array_merge($ahah, array('event' => $event['unit']));
        }
      }
      //default for unit part
      else {
        $element['unit']['#ahah'] = $ahah;
        unset($element['unit']['#ahah']['event']);
      }
    }
    //default event for both field
    else {
      $element['value']['#ahah'] = $element['unit']['#ahah'] = $element['#ahah'];
    }
    $element = form_expand_ahah($element);
  }

  return $element;
}





/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_distance element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing gmaps_distance as a textfield and a select element.
 *
 * @ingroup themeable
 */
function theme_gmaps_distance($element) {
  _form_set_class($element, array('form-gmaps-distance'));
  return theme('form_element', $element, '<div id="'. $element['#id'] .'"'. drupal_attributes($element['#attributes']) .'>'. $element['#children'] .'</div>');
}
