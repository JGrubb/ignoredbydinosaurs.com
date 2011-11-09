<?php
// $Id: gmaps.anp-element.inc,v 1.1.2.11 2010/05/06 15:15:07 xmarket Exp $

/**
 * @file
 * gmaps_anp element processors and validators
 */

/**
 * Convert object #default_value to array if necessary.
 */
function _gmaps_anp_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    if (isset($element['#default_value'])) {
      $value = (array)$element['#default_value'];
      $value['address'] = (array)$value['address'];
      $value['point'] = (array)$value['point'];
      return $value;
    }
    return  array();
  }
  else if (!empty($edit)) {
    $edit['address'] = form_type_gmaps_address_value(array(), $edit['address']);
    $edit['point'] = form_type_gmaps_point_value(array(), $edit['point']);
    if (isset($edit['search'])) {
      $edit['search'] = $edit['search']['query'];
    }
    return $edit;
  }
}

/**
 * Expands a gmaps_address.
 */
function _gmaps_anp_expand($element) {
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

  $has_value = !gmaps_anp_empty((object)array('address' => (object)$element['#value']['address'], 'point' => (object)$element['#value']['point']));
  $element['#collapsed'] = isset($element['#collapsed']) ? $element['#collapsed'] : $has_value;
  $element['#tabbed_parts'] = isset($element['#tabbed_parts']) ? $element['#tabbed_parts'] : $has_value;
  
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
  
  $geocoder = (!$map || $map->type == GMAPS_MAP_EARTH) && $element['#enable_geocoder'] && user_access('access geocoder service');
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

  $element['address'] = array('#type' => 'gmaps_address',
    '#default_value' => $element['#value']['address'],
    '#enable_geocoder' => FALSE,
    '#enable_privacy' => $element['#enable_privacy'],
    '#allowed_countries' => $element['#allowed_countries'],
    '#required' => $element['#required'],
    '#disabled' => $element['#disabled'],
    '#size' => $element['#size'],
    '#embedded_item' => TRUE,
    '#collapsible' => FALSE,
    '#attributes' => array('class' => 'form-gmaps-anp-address'),
  );

  $element['point'] = array('#type' => 'gmaps_point',
    '#default_value' => $element['#value']['point'],
    '#map_type' => GMAPS_MAP_EARTH,
    '#enable_geocoder' => FALSE,
    '#enable_privacy' => $element['#enable_privacy'],
    '#enable_3D' => $element['#enable_3D'],
    '#enable_bounds' => $element['#enable_bounds'],
    '#enable_pov' => $element['#enable_pov'],
    '#required' => $element['#required'],
    '#disabled' => $element['#disabled'],
    '#input_disabled' => ($element['#required'] && $element['#input_disabled']),
    '#use_map' => $element['#use_map'],
    '#embedded_item' => TRUE,
    '#collapsible' => FALSE,
    '#attributes' => array('class' => 'form-gmaps-anp-point'),
  );

  $element['#pre_render'][] = '_gmaps_anp_pre_render';

  return $element;
}

function _gmaps_anp_validate($element, &$form_state) {
  //always return object
  $value = &$element['#value'];
  $value['address'] = (object)$value['address'];
  _gmaps_point_set_return_value($value['point'], $element['point']['#map_type']);
  if (!$element['#required']) {
    if (gmaps_address_empty($value['address']) && !gmaps_point_empty($value['point'])) {
      form_error($element, t('Please, fill the address for the field !name, too, or clear the latitude and longitude fields.', array('!name' => $element['#title'])));
    }
    else if (!gmaps_address_empty($value['address']) && gmaps_point_empty($value['point'])) {
      form_error($element, t('Please, fill the coordinates for the field !name, too, or clear the address fields.', array('!name' => $element['#title'])));
    }
  }
  $value = (object)$value;
  form_set_value($element, $value, $form_state);
}

function _gmaps_anp_pre_render($element) {
  static $js_added = FALSE;
  $class = 'form-gmaps-anp';
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }

  if (!$js_added) {
    drupal_add_js('misc/jquery.form.js');
    gmaps_add_js();
    drupal_add_js(drupal_get_path('module', 'gmaps') .'/misc/gmaps-anp-element.js');
    $js_added = TRUE;
  }
  
  //if an error occured on the address part, than the point map initialization fails
  if ((!form_get_errors() || !$element['#use_map']) && $element['#tabbed_parts']) {
    $tabs = array('#type' => 'gmaps_tabs',
      '#id' => $element['#id'] .'-part-tabs',
      '#attributes' => array('class' => 'gmaps-anp-part-tabs'),
    );
    $tabs['point'] = array('#type' => 'gmaps_tabs_panel',
      '#title' => t('Point'),
      '#id' => 'point',
      'value' => $element['point'],
    );
    unset($element['point']);
    $tabs['address'] = array('#type' => 'gmaps_tabs_panel',
      '#title' => t('Address'),
      '#id' => 'address',
      'value' => $element['address'],
    );
    unset($element['address']);
    $element['tabs'] = $tabs;
  }
  
  return $element;
}




/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_anp element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing the gmaps_anp element.
 *
 * @ingroup themeable
 */
function theme_gmaps_anp($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-anp-collapsible');
}
