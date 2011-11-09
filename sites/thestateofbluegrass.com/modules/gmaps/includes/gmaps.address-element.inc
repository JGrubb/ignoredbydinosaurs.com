<?php
// $Id: gmaps.address-element.inc,v 1.1.2.10 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * gmaps_address element processors and validators
 */

/**
 * Convert object #default_value to array if necessary.
 */
function _gmaps_address_value($element, $edit = FALSE) {
  static $included = FALSE;

  if (!$included) {
    module_load_include('inc', 'gmaps', 'includes/gmaps.address');
    $included = TRUE;
  }

  if ($edit === FALSE) {
    return isset($element['#default_value']) ? (is_object($element['#default_value']) ? (array)$element['#default_value'] : $element['#default_value']) : array();
  }
  else if (!empty($edit)) {
    if (isset($edit['search'])) {
      $edit['search'] = $edit['search']['query'];
    }
    return $edit;
  }
}

/**
 * Expands a gmaps_address.
 */
function _gmaps_address_expand($element) {
  $parts = _gmaps_address_get_part_labels();
  $country_label = $parts['country'];
  unset($parts['country']);

  //country select
  $countries = _gmaps_get_countries('names');
  $country_options = $element['#required'] ? array() : array('' => t('Unknown'));

  if ($countries) {
    if (is_array($element['#allowed_countries']) && !empty($element['#allowed_countries'])) {
      foreach($element['#allowed_countries'] as $code) {
        if (isset($countries[$code])) {
          $country_options[$code] = $countries[$code];
        }
      }
    } else {
      $country_options += $countries;
    }
  }
  else {
    drupal_set_message(t('Missing geonames country database. Run cron to build it.'), 'error gmaps-address-error');
    $element = array('#type' => 'markup', '#value' => theme('status_messages', 'error gmaps-address-error'), '#parents' => array());
    return $element;
  }

  //in case whan #allowed_contries not in global allowed countries list.
  if (empty($country_options) || (count($country_options) == 1 && isset($country_options['']))) {
    drupal_set_message(t('No countries allowed'), 'gmaps-address-error error');
    $element = array('#type' => 'markup', '#value' => theme('status_messages', 'gmaps-address-error error'), '#parents' => array());
    return $element;
  }
  
  if (!isset($country_options[$element['#value']['country']])) {
    $country_options[$element['#value']['country']] = t('Disabled');
  }

  $element['#tree'] = TRUE;

  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  if (!isset($element['#required'])) {
    $element['#required'] = 0;
  }
  else if ($element['#required'] === TRUE) {
    $element['#required'] = 3;
  }

  $element['#collapsed'] = isset($element['#collapsed']) ? $element['#collapsed'] : !gmaps_address_empty((object)$element['#value']);

  //geocoder
  if ($element['#use_map']) {
    $map = $element['#use_map'];
    
    if ($map && is_bool($map)) {
      $map = gmaps_map_get_mid_for_default('input_'. GMAPS_MAP_EARTH);
    }
    if (is_numeric($map)) {
      $map = gmaps_map_load($map);
    }
    $element['#use_map'] = (is_object($map) && $map->type == GMAPS_MAP_EARTH) ? $map : FALSE;
  }

  $geocoder = $element['#enable_geocoder'] && user_access('access geocoder service');
  if ($geocoder) {
    $query = '';
    if ($element['#search_value']) {
      $query = $element['#search_value'];
    }
    else if (!empty($element['#value'])) {
      if (isset($element['#value']['search'])) {
        $query = $element['#value']['search'];
      }
    }
    $element['search'] = array('#type' => 'gmaps_geocoder',
      '#default_value' => $query,
      '#size' => 23,
    );
  }

  $element['country'] = array('#type' => 'select',
    '#title' => $country_label,
    '#default_value' => $element['#value']['country'],
    '#options' => $country_options,
    '#disabled' => $element['#disabled'],
    '#attributes' => array('class' => 'form-gmaps-address-country', 'title' => $country_label, 'alt' => $country_label),
  );

  //address parts
  $size = !empty($element['#size']) ? $element['#size'] : 30;
  $size = ($geocoder && $element['#use_map']) ? min(30, $size) : $size;
  foreach ($parts as $part => $title) {
    $element[$part] = array('#type' => 'textfield',
      '#title' => $title,
      '#default_value' => $element['#value'][$part],
      '#size' => $size,
      '#maxlength' => GMAPS_ADDRESS_PART_MAXLENGTH,
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-address-'. $part, 'title' => $title, 'alt' => $title),
    );
  }
  $element['postalcode']['#size'] = min(max(floor($size / 2), 10), $size);
  $element['postalcode']['#maxlength'] = 16;
  $element['thoroughfare']['#maxlength'] = 255;
  
  if ($element['#required']) {
    $element['country']['#required'] = TRUE;
    $element['locality']['#required'] = ($element['#required'] >= 2);
    $element['thoroughfare']['#required'] = ($element['#required'] >= 3);
  }

  //map
  if ($geocoder && $element['#use_map']) {
    $element['map'] = array('#type' => 'markup', '#value' => '');
  }

  //privacy
  if ($element['#enable_privacy']) {
    $element['privacy'] = array('#type' => 'select',
      '#title' => t('Privacy'),
      '#default_value' => $element['#value']['privacy'],
      '#options' => _gmaps_address_privacy_levels(),
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-address-privacy', 'title' => t('Privacy'), 'alt' => t('Privacy')),
    );
  }

  $element['#pre_render'][] = '_gmaps_address_pre_render';
  $element['#theme']= 'gmaps_address_element';

  return $element;
}

function _gmaps_address_validate($element, &$form_state) {
  $value = &$element['#value'];

  if (!empty($value['country'])) {
    $countries = _gmaps_get_countries('names');
    $country_options = array();
  
    if ($countries) {
      if (is_array($element['#allowed_countries']) && !empty($element['#allowed_countries'])) {
        foreach($element['#allowed_countries'] as $code) {
          if (isset($countries[$code])) {
            $country_options[$code] = $countries[$code];
          }
        }
      } else {
        $country_options += $countries;
      }
    }
    if (!isset($country_options[$value['country']])) {
      form_error($element['country'], t('This country is not allowed.'));
    }
  }
    
  if (!empty($value['locality']) && empty($value['country'])) {
    form_error($element['country'], t('You need to set the country also.'));
  }
  if (!empty($value['deplocality']) && empty($value['locality'])) {
    form_error($element['locality'], t('You need to set the city also.'));
  }
  if (!empty($value['postalcode']) && empty($value['locality'])) {
    form_error($element['locality'], t('You need to set the city also.'));
  }
  if (!empty($value['adminarea']) && empty($value['country'])) {
    form_error($element['country'], t('You need to set the country also.'));
  }
  if (!empty($value['subadminarea']) && empty($value['adminarea'])) {
    form_error($element['adminarea'], t('You need to set the adninistrative area also.'));
  }
  
  //always return object
  $value = (object)$value;
  form_set_value($element, $value, $form_state);
}

function _gmaps_address_pre_render($element) {
  static $js_added = FALSE;
  
  $parts = _gmaps_address_get_parts();
  unset($parts['country']);

  foreach($parts as $part => $label) {
    if (!isset($first_part)) {
      $first_part = $part;
    }
    $element[$part]['#prefix'] = '<div class="gmaps-address-part-'. $part .'">';
    $element[$part]['#suffix'] = '</div>';
  }
  $element[$first_part]['#prefix'] = '<div class="gmaps-address-parts-wrapper">'. $element[$first_part]['#prefix'];
  $element[$part]['#suffix'] .= '</div>';

  if (isset($element['map'])) {
    $element[$first_part]['#prefix'] = '<div class="gmaps-address-map-wrapper">'. $element[$first_part]['#prefix'];
    $element['map']['#prefix'] = '<div class="gmaps-address-gmi">';
    $element['map']['#suffix'] = '</div><div class="gmaps-address-clear-map-wrapper"></div></div>';

    $element['map']['#value'] = gmaps_map_item_render($element['#use_map'], '_gmaps_address_gmi', array($element['#value']), array(), $element['#id']);
  }

  $class = 'form-gmaps-address';
  if ($element['#embedded_item']) {
    $class .= ' gmaps-address-embedded';
  }
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }

  if (!$js_added) {
    drupal_add_js('misc/jquery.form.js');
    gmaps_add_js();
    drupal_add_js(drupal_get_path('module', 'gmaps') .'/misc/gmaps-address-element.js');
    drupal_add_js(array(
      'gmaps' => array(
        'address' => array(
          'parts' => array_keys($parts),
          'accessAllParts' => user_access('access all address parts'),
        ),
      ),
    ), 'setting');
    $js_added = TRUE;
  }

  return $element;
}

function _gmaps_address_gmi(&$gmi, $value) {
  $gmi->throttle = 0;
  $gmi->embedded = TRUE;
  
  $base = &$gmi->base;
  $base->size = gmaps_pixels_parse('0x0', 'wh');
  $base->map_data['progressbar']['enabled'] = 0;
  $base->map_data['toc_map']['enabled'] = 0;
  
  if (isset($base->map_data['svu'])) {
    $base->map_data['svu']['enabled'] = 0;
  }
    
  $base->methods = drupal_map_assoc(array('auto_center_zoom', 'dragging'));
  $base->controls['maptype']['control'] = 'menumaptype';
  $base->controls['navigation']['control'] = 'smallmap';
  
  foreach($base->controls as $ct => $ct_data) {
    if (!in_array($ct, array('maptype', 'navigation'))) {
      $base->controls[$ct]['control'] = '';
    }
  }
  
  $base->control_data['menumaptype']['shortnames'] = 1;
  
  $base->ov_data['marker']['misc']['labels'] = '';
  
  $base->ov_data['marker']['options']['clickable'] = 1;
  $base->ov_data['marker']['options']['draggable'] = 1;
  $base->ov_data['marker']['options']['hide'] = 0;
  
  $base->ov_data['marker']['labeled']['enabled'] = 0;
  $base->ov_data['marker']['protected_labeled']['enabled'] = 0;
  
  $base->ov_data['marker']['action']['hover'] = '';
  $base->ov_data['marker']['action']['click'] = 'content';
  $base->ov_data['marker']['action']['dblclick'] = '';
  $base->ov_data['marker']['action']['rightclick'] = '';
  
  $base->ov_data['marker']['tracker']['enabled'] = 1;
  $base->ov_data['marker']['route']['enabled'] = 0;
  $base->ov_data['marker']['manager']['manager'] = '';
  
  $gmi->overlays['marker']['input'] = array(
    'point' => $gmi->base->center,
    'options' => array(
      'clickable' => 1,
      'draggable' => 1,
      'hide' => 1,
    ),
  );

  $gmi->attributes['class'] = 'input-gmi input-gmi-address';
}


/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_address element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing the gmaps_address element.
 *
 * @ingroup themeable
 */
function theme_gmaps_address($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-address-collapsible');
}
