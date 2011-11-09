<?php
// $Id: gmaps.geocoder-element.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_distancefield element processors and validators
 */

/**
 * Convert array $_POST to string.
 */
function _gmaps_geocoder_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    $value = array();
    if (isset($element['#default_value'])) {
      if (is_array($element['#default_value'])) {
        $value = $element['#default_value'];
      }
      else {
        $settings = gmaps_get_settings('gmaps_api_settings');
        $value = array(
          'query' => $element['#default_value'],
          'domain' => $settings['domain'],
        );
      }
    }
  }
  else {
    if (is_array($edit)) {
      $value = $edit;
    }
    else {
      $settings = gmaps_get_settings('gmaps_api_settings');
      $value = array(
        'query' => $edit,
        'domain' => $settings['domain'],
      );
    }
  }
  return $value;
}

/**
 * Expands a gmaps_geocoder.
 */
function _gmaps_geocoder_expand($element) {
  $element['#tree'] = TRUE;

  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  $element['query'] = array('#type' => 'textfield',
    '#default_value' => $element['#value']['query'],
    '#size' => isset($element['#size']) ? $element['#size'] : 25,
    '#disabled' => $element['#disabled'],
    '#required' => $element['#required'],
    '#attributes' => array('class' => 'form-gmaps-geocoder-query'),
  );
  $element['domain'] = array('#type' => 'select',
    '#default_value' => $element['#value']['domain'],
    '#options' => _gmaps_get_maps_api_domain_options(),
    '#disabled' => $element['#disabled'],
    '#attributes' => array('class' => 'form-gmaps-geocoder-domain'),
  );
  $element['search'] = array('#type' => 'button_button',
    '#value' => t('Search'),
    '#disabled' => $element['#disabled'],
    '#attributes' => array('class' => 'form-gmaps-geocoder-search'),
  );

  $element['#pre_render'][] = '_gmaps_geocoder_pre_render';

  return $element;
}

function _gmaps_geocoder_validate($element, &$form_state) {
  if (!form_get_errors()) {
    $element['#value'] = $element['#value']['query'];
    form_set_value($element, $element['#value'], $form_state);
  }
}

function _gmaps_geocoder_pre_render($element) {
  static $js_added = FALSE;

  $element['viewport'] = array(
    '#type' => 'markup',
    '#value' => '<div class="gmaps-geocoder-options"></div>',
  );
  $class = 'form-gmaps-geocoder';
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }

  if (!$element['#disabled'] && !$js_added) {
    drupal_add_js('misc/jquery.form.js');
    gmaps_add_js();
    drupal_add_js(drupal_get_path('module', 'gmaps') .'/misc/gmaps-geocoder-element.js');

    drupal_add_js(array(
      'gmaps' => array(
        'geocoder' => array(
          'accuracyLevels' => _gmaps_geocoder_accuracy_levels(),
        ),
      ),
    ), 'setting');
    $js_added = TRUE;
  }
  return $element;
}

function _gmaps_geocoder_accuracy_levels() {
  return array(t('Unknown'),
    t('Country'),
    t('Region'),
    t('Sub-region'),
    t('Town'),
    t('Zip code'),
    t('Street'),
    t('Intersection'),
    t('Address'),
    t('Premise'),
  );
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
function theme_gmaps_geocoder($element) {
  $value = '<div id="'. $element['#id'] .'"'. drupal_attributes($element['#attributes']) .'>';
  $value .= '<div class="gmaps-geocoder-popup"></div>';
  $value .= $element['#children'];
  $value .= '</div>';
  return theme('form_element', $element, $value);
}
