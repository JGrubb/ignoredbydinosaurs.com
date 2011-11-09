<?php
// $Id: gmaps.icon-select-element.inc,v 1.1.2.5 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_icon_select element processors and validators
 */

function _gmaps_icon_select_value($form, $edit = FALSE) {
  if ($edit !== FALSE) {
    if (isset($form['#multiple']) && $form['#multiple']) {
      return (is_array($edit)) ? drupal_map_assoc($edit) : array();
    }
    else {
      return $edit;
    }
  }
}

function _gmaps_icon_select_expand($element) {
  $element['#pre_render'][] = '_gmaps_icon_select_pre_render';
  return $element;
}

/**
 * Validates a gmaps_distance element.
 */
function _gmaps_icon_select_validate($element, &$form_state) {
  if (isset($element['#value'])) {
    $t = get_t();
    $options = _gmaps_icon_select_options_flatten($element['#icons']);
    if (is_array($element['#value'])) {
      $value = $element['#value'];
      foreach ($value as $v) {
        if (!isset($options[$v])) {
          form_error($element, $t('An illegal choice has been detected. Please contact the site administrator.'));
          watchdog('form', 'Illegal choice %choice in !name element.', array('%choice' => $v, '!name' => empty($element['#title']) ? $element['#parents'][0] : $element['#title']), WATCHDOG_ERROR);
        }
      }
    }
    elseif (!isset($options[$element['#value']])) {
      form_error($element, $t('An illegal choice has been detected. Please contact the site administrator.'));
      watchdog('form', 'Illegal choice %choice in %name element.', array('%choice' => $element['#value'], '%name' => empty($element['#title']) ? $element['#parents'][0] : $element['#title']), WATCHDOG_ERROR);
    }
  }
}

function _gmaps_icon_select_pre_render($element) {
  global $gmaps_throttle;
  static $js_added = FALSE;
  static $default_max = NULL;

  if (is_null($default_max)) {
    $settings = gmaps_get_settings('gmaps_icon_settings');
    $default_max = $settings['inline_preview_size'];
  }
  
  $element['#preview_max_size'] = gmaps_pixels_parse(isset($element['#preview_max_size']) ? $element['#preview_max_size'] : $default_max);
  $max = &$element['#preview_max_size'];
  
  if (isset($element['#ahah'])) {
    if (isset($element['#ahah']['path']) && !isset($element['#ahah']['event'])) {
      $element['#ahah']['event'] = 'change';
    }
    $element = form_expand_ahah($element);
  }
  if (!$gmaps_throttle && $max[0] > 0 && $max[1] > 0 && !$js_added) {
    drupal_add_js('misc/jquery.form.js');
    drupal_add_js(drupal_get_path('module', 'gmaps') .'/misc/gmaps-icon-select.js');
    $js_added = TRUE;
  }
  return $element;
}

function _gmaps_icon_select_options_flatten($array, $reset = TRUE) {
  static $return;

  if ($reset) {
    $return = array();
  }

  foreach ($array as $key => $value) {
    if (is_array($value)) {
      form_options_flatten($value, FALSE);
    }
    else {
      $return[$key] = 1;
    }
  }

  return $return;
}

function _gmaps_icon_select_options($element, $choices = NULL) {
  global $gmaps_throttle;
  $max = $element['#preview_max_size'];
  //skip preview if throttle is active
  if ($gmaps_throttle) {
    $max = array(0,0);
  }

  if (!isset($choices)) {
    $choices = $element['#icons'];
  }
  // array_key_exists() accommodates the rare event where $element['#value'] is NULL.
  // isset() fails in this situation.
  $value_valid = isset($element['#value']) || array_key_exists('#value', $element);
  $value_is_array = is_array($element['#value']);
  $options = '';
  $preview = array();
  foreach ($choices as $key => $choice) {
    if (is_array($choice)) {
      $options .= '<optgroup label="'. $key .'">';
      $options .= _gmaps_icon_select_options($element, $choice);
      $options .= '</optgroup>';
    }
    else if (is_string($choice)) {
      $key = (string)$key;
      if ($value_valid && (!$value_is_array && (string)$element['#value'] === $key || ($value_is_array && in_array($key, $element['#value'])))) {
        $selected = ' selected="selected"';
      }
      else {
        $selected = '';
      }
      $options .= '<option value="'. check_plain($key) .'"'. $selected .'>'. check_plain($choice) .'</option>';
    }
    else {
      $key = (string)$choice->icid;
      if ($value_valid && (!$value_is_array && (string)$element['#value'] === $key || ($value_is_array && in_array($key, $element['#value'])))) {
        $selected = ' selected="selected"';
      }
      else {
        $selected = '';
      }
      $options .= '<option value="'. check_plain($key) .'"'. $selected .'>'. check_plain(t($choice->name)) .'</option>';

      if ($max[0] > 0 && $max[1] > 0) {
        $size = is_string($choice->icon_size) ? unserialize($choice->icon_size) : $choice->icon_size;
        $size = gmaps_pixels_parse($size, 'wh');
        if ($max[0] < $size['width']) {
          $size['height'] = round(($max[0] / $size['width']) * $size['height']);
          $size['width'] = $max[0];
        }
        if ($max[1] < $size['height']) {
          $size['width'] = round(($max[1] / $size['height']) * $size['width']);
          $size['height'] = $max[1];
        }
        $preview[$choice->icid] = array(
          'width' => $size['width'],
          'height' => $size['height'],
          'src' => file_create_url($choice->icon),
        );
      }
    }
  }

  if (!empty($preview)) {
    drupal_add_js(array(
      'gmaps' => array(
        'iconSelect' => array(
          $element['#id'] => $preview
    ))), 'setting');
  }

  return $options;
}





/**************
 * Themeables *
 **************/

/**
 * Format a dropdown menu or scrolling selection box
 * from an associative array of icon objects.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used: same as for the select element, except '#options', which replaced by '#icons',
 *     and there is an extra '#preview_max_size'.
 * @return
 *   A themed HTML string representing the form element.
 *
 * @ingroup themeable
 *
 * It is possible to group options together; to do this, change the format of
 * $options to an associative array in which the keys are group labels, and the
 * values are associative arrays in the normal #icons format.
 */
function theme_gmaps_icon_select($element) {
  $select = '';
  _form_set_class($element, array('form-select', 'form-gmaps-icon-select'));

  $size = $element['#size'] ? ' size="'. $element['#size'] .'"' : '';
  $multiple = $element['#multiple'];

  $value = '<div class="gis-preview-wrapper">';
  $value .= '<select name="'. $element['#name'] .''. ($multiple ? '[]' : '') .'"'. ($multiple ? ' multiple="multiple" ' : '') . drupal_attributes($element['#attributes']) .' id="'. $element['#id'] .'" '. $size .'>'. _gmaps_icon_select_options($element) .'</select>';
  $value .= theme('image', '', '', '', array('class' => 'gis-preview', 'style' => 'display: none;'), FALSE);
  $value .= '</div>';

  return theme('form_element', $element, $value);
}
