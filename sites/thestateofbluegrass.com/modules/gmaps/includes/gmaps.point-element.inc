<?php
// $Id: gmaps.point-element.inc,v 1.1.2.15 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * gmaps_point element processors and validators
 */

/**
 * Convert object #default_value to array if necessary.
 */
function _gmaps_point_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    $value = array();
    if (isset($element['#default_value']) && !gmaps_point_empty((object)$element['#default_value'])) {
      $value = !is_array($element['#default_value']) ? (array)$element['#default_value'] : $element['#default_value'];
      
      if ($element['#enable_bounds']) {
        if (!empty($value['bounds'])) {
          $b = !is_array($value['bounds']) ? (array)$value['bounds'] : $value['bounds'];
          $b['northeast'] = !is_object($b['northeast']) ? (object)$b['northeast'] : $b['northeast'];
          $b['southwest'] = !is_object($b['southwest']) ? (object)$b['southwest'] : $b['southwest'];
          if (gmaps_bounds_empty($b)) {
            $b = NULL;
          }
        }
        if (!$b) {
          $b = array(
            'northeast' => array(
              'latitude' => $value['latitude'] + GMAPS_MAP_DEFAULT_BOUNDS_SPAN,
              'longitude' => $value['longitude'] + GMAPS_MAP_DEFAULT_BOUNDS_SPAN,
              'elevation' => 0,
              'map_type' => $element['#map_type'],
            ),
            'southwest' => array(
              'latitude' => $value['latitude'] - GMAPS_MAP_DEFAULT_BOUNDS_SPAN,
              'longitude' => $value['longitude'] - GMAPS_MAP_DEFAULT_BOUNDS_SPAN,
              'elevation' => 0,
              'map_type' => $element['#map_type'],
            ),
          );
        }
        else {
          $b['northeast'] = (array)$b['northeast'];
          $b['southwest'] = (array)$b['southwest'];
        }
        $value['bounds'] = $b;
      }
      
      if ($element['#map_type'] == GMAPS_MAP_EARTH && $element['#enable_pov']) {
        if (!empty($value['pov'])) {
          $p = !is_array($value['pov']) ? (array)$value['pov'] : $value['pov'];
          if (gmaps_pov_empty($p)) {
            $p = NULL;
          }
        }
        if (!$p) {
          $p = array(
            'yaw' => 0,
            'pitch' => 0,
            'zoom' => 0,
          );
        }
        $value['pov'] = $p;
      }
    }
    else if ($element['#required']) {
      $value = array(
        'latitude' => 0,
        'longitude' => 0,
        'map_type' => $element['#map_type'],
      );
    }
    return $value;
  }
  else if (!empty($edit)) {
    $edit += $edit['tabs']['coords'];
    if (array_key_exists('bounds', $edit['tabs']) && !empty($edit['tabs']['bounds'])) {
      $edit['bounds'] = $edit['tabs']['bounds'];
      $edit['bounds']['northeast'] = _gmaps_point_value(array(), $edit['bounds']['northeast']);
      $edit['bounds']['southwest'] = _gmaps_point_value(array(), $edit['bounds']['southwest']);
    }
    if (array_key_exists('pov', $edit['tabs']) && !empty($edit['tabs']['pov'])) {
      $edit['pov'] = $edit['tabs']['pov'];
    }
    if (array_key_exists('search', $edit) && !empty($edit['search'])) {
      $edit['search'] = $edit['search']['query'];
    }
    unset($edit['tabs']);
    return $edit;
  }
}

/**
 * Expands a gmaps_point.
 */
function _gmaps_point_expand($element) {
  global $user;
  static $labels = NULL;

  if (!isset($element['#map_type']) || !($type = gmaps_map_get_types('type', $element['#map_type']))) {
    drupal_set_message(t('Missing or invalid map type (%type).', array('%type' => $type ? $type->name : 'null')), 'error gmaps-point-error');
    $element = array('#type' => 'markup', '#value' => theme('status_messages', 'error gmaps-point-error'), '#parents' => array());
    return $element;
  }

  if (is_null($labels)) {
    $labels = array(
      'coords' => t('Coordinates'),
      'latitude' => t('Latitude'),
      'longitude' => t('Longitude'),
      'elevation' => t('Elevation'),
      'privacy' => t('Privacy'),
      'bounds' => t('Bounds'),
      'pov' => t('Point of view'),
      'pov_enabled' => t('Enabled'),
      'pov_yaw' => t('Yaw'),
      'pov_pitch' => t('Pitch'),
      'pov_zoom' => t('Zoom'),
    );
  }

  $element['#tree'] = TRUE;

  if (!isset($element['#disabled'])) {
    $element['#disabled'] = FALSE;
  }

  if (!isset($element['#required'])) {
    $element['#required'] = FALSE;
  }
  
  $element['#collapsed'] = isset($element['#collapsed']) ? $element['#collapsed'] : !gmaps_point_empty((object)$element['#value']);

  //geocoder
  $geocoder = ($type->type == GMAPS_MAP_EARTH) && $element['#enable_geocoder'] && user_access('access geocoder service');
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
  
  if ($element['#enable_tabs'] && ($element['#enable_bounds'] || ($type->type == GMAPS_MAP_EARTH && $element['#enable_pov']))) {
    $element['tabs'] = array('#type' => 'gmaps_tabs',
      '#id' => $element['#id'],
      '#collapsible' => FALSE,
      '#embedded_item' => TRUE,
    );
    $element['tabs']['coords'] = array('#type' => 'gmaps_tabs_panel',
      '#id' => 'coords',
      '#title' => $labels['coords'],
      '#attributes' => array('class' => 'form-gmaps-point-coordinates'),
    );
  }
  else if (!$element['#enable_tabs']) {
    $element['tabs'] = array('coords' => array('#type' => 'item',
      '#title' => $labels['coords'],
      '#attributes' => array('class' => 'form-gmaps-point-coordinates'),
    ));
  }
  else {
    $element['tabs'] = array('coords' => array());
  }
  //coords
  $size = !empty($element['#size']) ? $element['#size'] : 17;
  $element['tabs']['coords']['latitude'] =  array('#type' => 'textfield',
    '#title' => $labels['latitude'],
    '#default_value' => $element['#value']['latitude'],
    '#required' => $element['#required'],
    '#disabled' => $element['#disabled'],
    '#field_suffix' => '&deg;',
    '#size' => $size,
    '#maxlength' => 20,
    '#attributes' => array('class' => 'form-gmaps-point-latitude', 'title' => $labels['latitude'], 'alt' => $labels['latitude']),
  );
  $element['tabs']['coords']['longitude'] = $element['tabs']['coords']['latitude'];
  $element['tabs']['coords']['longitude']['#title'] = $labels['longitude'];
  $element['tabs']['coords']['longitude']['#default_value'] = $element['#value']['longitude'];
  $element['tabs']['coords']['longitude']['#attributes'] = array('class' => 'form-gmaps-point-longitude', 'title' => $labels['longitude'], 'alt' => $labels['longitude']);

  if ($type->{'3D_enabled'} && $element['#enable_3D']) {
    $element['tabs']['coords']['elevation'] = array('#type' => 'textfield',
      '#title' => $labels['elevation'],
      '#default_value' => isset($element['#value']['elevation']) ? $element['#value']['elevation'] : 0,
      '#disabled' => $element['#disabled'],
      '#required' => $element['#required'],
      '#field_suffix' => GMAPS_DEFAULT_LENGTH_UNIT_ABBR,
      '#size' => $size,
      '#maxlength' => 20,
      '#attributes' => array('class' => 'form-gmaps-point-elevation', 'title' => $labels['elevation'], 'alt' => $labels['elevation']),
    );
  }
  else {
    $element['tabs']['coords']['elevation'] = array('#type' => 'hidden', '#value' => 0);
  }

  //privacy
  if (($function = $type->coords['callbacks']['line_end']) && $element['#enable_privacy']) {
    $element['tabs']['coords']['privacy'] = array('#type' => $user->gmaps_settings['length']['input'],
      '#title' => $labels['privacy'],
      '#default_value' => $element['#value']['privacy'],
      '#size' => $size,
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-point-privacy', 'title' => $labels['privacy'], 'alt' => $labels['privacy']),
      '#description' => '#disabled',
    );
  }

  //bounds
  if ($element['#enable_bounds']) {
    $bounds = &$element['#value']['bounds'];
    if ($element['#enable_tabs']) {
      $element['tabs']['bounds'] = array('#type' => 'gmaps_tabs_panel',
        '#id' => 'bounds',
        '#title' => $labels['bounds'],
        '#attributes' => array('class' => 'form-gmaps-point-bounds'),
      );
    }
    else {
      $element['tabs']['bounds'] = array('#type' => 'item',
        '#title' => $labels['bounds'],
        '#attributes' => array('class' => 'form-gmaps-point-bounds'),
      );
    }
    $element['tabs']['bounds']['northeast'] = array('#type' => 'gmaps_point',
      '#title' => t('Northeast bound'),
      '#default_value' => $bounds['northeast'],
      '#required' => $element['#required'],
      '#disabled' => $element['#disabled'],
      '#size' => $size,
      '#map_type' => $element['#map_type'],
      '#enable_geocoder' => FALSE,
      '#enable_bounds' => FALSE,
      '#enable_privacy' => FALSE,
      '#enable_3D' => FALSE,
      '#use_map' => 0,
      '#input_disabled' => $element['#input_disabled'],
      '#embedded_item' => TRUE,
      '#collapsible' => FALSE,
      '#attributes' => array('class' => 'form-gmaps-point-bounds-northeast'),
    );
    $element['tabs']['bounds']['southwest'] = array('#type' => 'gmaps_point',
      '#title' => t('Southwest bound'),
      '#default_value' => $bounds['southwest'],
      '#required' => $element['#required'],
      '#disabled' => $element['#disabled'],
      '#size' => $size,
      '#map_type' => $element['#map_type'],
      '#enable_geocoder' => FALSE,
      '#enable_bounds' => FALSE,
      '#enable_privacy' => FALSE,
      '#enable_3D' => FALSE,
      '#use_map' => 0,
      '#input_disabled' => $element['#input_disabled'],
      '#embedded_item' => TRUE,
      '#collapsible' => FALSE,
      '#attributes' => array('class' => 'form-gmaps-point-bounds-southwest'),
    );
  }
  
  if ($type->type == GMAPS_MAP_EARTH && $element['#enable_pov']) {
    $pov = &$element['#value']['pov'];
    if ($element['#enable_tabs']) {
      $element['tabs']['pov'] = array('#type' => 'gmaps_tabs_panel',
        '#id' => 'pov',
        '#title' => $labels['pov'],
        '#attributes' => array('class' => 'form-gmaps-point-pov', 'title' => $labels['pov'], 'alt' => $labels['pov']),
      );
    }
    else {
      $element['tabs']['pov'] = array('#type' => 'item',
        '#title' => $labels['pov'],
        '#attributes' => array('class' => 'form-gmaps-point-pov', 'title' => $labels['pov'], 'alt' => $labels['pov']),
      );
    }
    $element['tabs']['pov']['has_svu'] = array('#type' => 'hidden',
      '#default_value' => $pov['has_svu'],
      '#attributes' => array('class' => 'form-gmaps-point-pov-has-svu'),
    );
    $element['tabs']['pov']['enabled'] = array('#type' => 'checkbox',
      '#title' => $labels['pov_enabled'],
      '#default_value' => $pov['enabled'],
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-point-pov-enabled', 'title' => $labels['pov_enabled'], 'alt' => $labels['pov_enabled']),
    );
    $element['tabs']['pov']['yaw'] = array('#type' => 'textfield',
      '#title' => $labels['pov_yaw'],
      '#default_value' => $pov['yaw'],
      '#required' => $element['#required'],
      '#size' => 6,
      '#maxlength' => 6,
      '#field_suffix' => '&deg;',
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-point-pov-yaw', 'title' => $labels['pov_yaw'], 'alt' => $labels['pov_yaw']),
    );
    $element['tabs']['pov']['pitch'] = array('#type' => 'textfield',
      '#title' => $labels['pov_pitch'],
      '#default_value' => $pov['pitch'],
      '#required' => $element['#required'],
      '#size' => 6,
      '#maxlength' => 6,
      '#field_suffix' => '&deg;',
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-point-pov-pitch', 'title' => $labels['pov_pitch'], 'alt' => $labels['pov_pitch']),
    );
    $element['tabs']['pov']['zoom'] = array('#type' => 'select',
      '#title' => $labels['pov_zoom'],
      '#default_value' => $pov['zoom'],
      '#options' => array(0,1,2,3),
      '#disabled' => $element['#disabled'],
      '#attributes' => array('class' => 'form-gmaps-point-pov-zoom', 'title' => $labels['pov_zoom'], 'alt' => $labels['pov_zoom']),
    );
  }
  
  //map
  if ($element['#use_map']) {
    $element['map'] = array('#type' => 'markup', '#value' => '');
  }

  $element['#pre_render'][] = '_gmaps_point_pre_render';
  
  return $element;
}

function _gmaps_point_validate($element, &$form_state) {
  $value = &$element['#value'];
  if (!gmaps_point_empty((object)$value)) {
    $number = _gmaps_number_parse($value['latitude']);
    if ($number === FALSE) {
      form_error($element['tabs']['coords']['latitude'], t('The specified number is invalid.'));
    }
    else if ($number < -90 || $number > 90) {
      form_error($element['tabs']['coords']['latitude'], t('The latitude must be a number between -90 and 90.'));
    }
    $number = _gmaps_number_parse($value['longitude']);
    if ($number === FALSE) {
      form_error($element['tabs']['coords']['longitude'], t('The specified number is invalid.'));
    }
    else if ($number < -180 || $number > 180) {
      form_error($element['tabs']['coords']['longitude'], t('The longitude must be a number between -180 and 180.'));
    }
    $number = _gmaps_number_parse($value['elevation']);
    if ($number === FALSE) {
      form_error($element['tabs']['coords']['elevation'], t('The specified number is invalid.'));
    }
  
    if (array_key_exists('pov', $value) && !empty($value['pov'])) {
      $pov = &$value['pov'];
      if (!empty($pov['yaw']) && ($pov['yaw'] < 0 || $pov['yaw'] > 360)) {
        form_error($element['tabs']['pov']['yaw'], t('%title yaw must be an integer between 0&deg; and 360&deg;.', array('%title' => $element['#title'])));
      }
      if (!empty($pov['pitch']) && ($pov['pitch'] < -90 || $pov['pitch'] > 90)) {
        form_error($element['tabs']['pov']['pitch'], t('%title pitch must be an integer between -90&deg; and 90&deg;.', array('%title' => $element['#title'])));
      }
    }
    _gmaps_point_set_return_value($value, $element['#map_type']);
  }
  else {
    if ($element['#required']) {
      $value = (object)array(
        'latitude' => 0,
        'longitude' => 0,
        'map_type' => $element['#map_type'],
      );
    }
    else {
      $value = NULL;
    }
  }

  form_set_value($element, $value, $form_state);
}

function _gmaps_point_set_return_value(&$value, $map_type) {
  if (array_key_exists('bounds', $value)) {
    _gmaps_point_set_return_value($value['bounds']['northeast'], $map_type);
    _gmaps_point_set_return_value($value['bounds']['southwest'], $map_type);
    if (gmaps_bounds_empty($value['bounds'])) {
      $value['bounds'] = array();
    }
  }
  if (array_key_exists('pov', $value) && gmaps_pov_empty($value['pov'])) {
    $value['pov'] = array();
  }
  if (array_key_exists('privacy', $value)) {
    $number = _gmaps_number_parse($value['privacy']['value']);
    if ($number !== FALSE) {
      $value['privacy']['value'] = $number;
    }
    else {
      unset($value['privacy']);
    }
  }
  $value['map_type'] = $map_type;
  $value = (object)$value;
}

function _gmaps_point_pre_render($element) {
  static $js_added = FALSE;
  
  foreach(array('latitude', 'longitude', 'elevation') as $part) {
    $element['tabs']['coords'][$part]['#prefix'] = '<div class="gmaps-point-part-'. $part .'">';
    $element['tabs']['coords'][$part]['#suffix'] = '</div>';
  }
  if (!$element['#enable_3D']) {
    $element['tabs']['coords']['latitude']['#prefix'] = '<div class="gmaps-point-2d-wrapper">'. $element['tabs']['coords']['latitude']['#prefix'];
    $element['tabs']['coords']['longitude']['#suffix'] .= '<div class="gmaps-point-clear-2d-wrapper"></div></div>';
  }
  $element['tabs']['coords']['latitude']['#prefix'] = '<div class="gmaps-point-parts-wrapper">'. $element['tabs']['coords']['latitude']['#prefix'];
  $element['tabs']['coords']['elevation']['#suffix'] .= '</div>';

  if ($element['#use_map']) {
    $map = $element['#use_map'];
    
    if ($map && is_bool($map)) {
      $map = gmaps_map_get_mid_for_default('input_'. $element['#map_type']);
    }
    if (is_numeric($map)) {
      $map = gmaps_map_load($map);
    }
    $map = is_object($map) ? $map : FALSE;
    
    if ($map) {
      $element['map']['#prefix'] = '<div class="gmaps-point-gmi">';
      $element['map']['#value'] = gmaps_map_item_render($map, '_gmaps_point_gmi', array($element), array(), $element['#id']);
      $element['map']['#suffix'] = '</div>';
    }
    else {
      unset($element['map']);
    }
  }

  $class = 'form-gmaps-point';
  if ($element['#embedded_item']) {
    $class .= ' gmaps-point-embedded';
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
    drupal_add_js(drupal_get_path('module', 'gmaps') .'/misc/gmaps-point-element.js');
    drupal_add_js(array(
      'gmaps' => array(
        'elevation' => variable_get('gmaps_elevation_service', ''),
      ),
    ), 'setting');
    $js_added = TRUE;
  }

  $input = TRUE;
  if (!user_access('edit gmaps point input fields')) {
    if (array_key_exists('#inherited_input', $element)) {
      $input = $element['#inherited_input'];
    }
    else {
      $input = !$map || !$element['#input_disabled'];
    }
  }
  drupal_add_js(array(
    'gmaps' => array(
      'point' => array(
        $element['#id'] => array(
          'input' => $input,
        ),
      ),
    ),
  ), 'setting');
  
  if (!empty($element['tabs']['bounds'])) {
    $element['tabs']['bounds']['northeast']['#inherited_input'] = $input;
    $element['tabs']['bounds']['southwest']['#inherited_input'] = $input;
  }
  
  return $element;
}

function _gmaps_point_gmi(&$gmi, $element) {
  $value = &$element['#value'];
  
  $gmi->throttle = 0;
  $gmi->embedded = TRUE;
  
  $base = &$gmi->base;
  $base->map_data['progressbar']['enabled'] = 0;
  $base->map_data['toc_map']['enabled'] = 0;
  
  if (isset($base->map_data['svu'])) {
    $base->map_data['svu']['enabled'] = ($base->type == GMAPS_MAP_EARTH && $element['#enable_pov']) ? 1 : 0;
    if ($base->map_data['svu']['enabled']) {
      $base->map_data['svu']['show'] = 0;
      $base->map_data['svu']['marker'] = 0;
      $base->map_data['svu']['hide'] = 0;
      $base->map_data['svu']['icon']['type'] = GMAPS_MAP_SVU_ICON_TYPE_STATIC;
      $base->map_data['svu']['icon']['n'] = $base->ov_data['marker']['icon'];
    }
  }
    
  $base->methods = drupal_map_assoc(array('auto_center_zoom', 'dragging', 'resize'));
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
  $base->ov_data['marker']['options']['autopan'] = 1;
  
  $base->ov_data['marker']['labeled']['enabled'] = 0;
  $base->ov_data['marker']['protected_labeled']['enabled'] = 0;
  
  $base->ov_data['marker']['action']['hover'] = '';
  $base->ov_data['marker']['action']['click'] = 'content';
  $base->ov_data['marker']['action']['dblclick'] = '';
  $base->ov_data['marker']['action']['rightclick'] = '';
  
  $base->ov_data['marker']['tracker']['enabled'] = 1;
  $base->ov_data['marker']['route']['enabled'] = 0;
  $base->ov_data['marker']['manager']['manager'] = '';
  
  $empty = gmaps_point_empty((object)$element['#value']);
  $point = !$empty ? (object)$element['#value'] : $gmi->base->center;
  $point->map_type = !empty($point->map_type) ? $point->map_type : $element['#map_type'];
  
  if (isset($element['tabs']['pov']) && !isset($point->pov)) {
    $point->pov = array('yaw' => 0, 'pitch' => 0, 'zoom' => 0);
  }
  if (empty($point->bounds)) {
    unset($point->bounds);
  }
  else {
    $point->bounds['northeast'] = (object)$point->bounds['northeast'];
    $point->bounds['southwest'] = (object)$point->bounds['southwest'];
  }
  $gmi->overlays['marker']['input'] = array(
    'point' => $point,
    'skip_privacy' => TRUE,
    'options' => array(
      'clickable' => 1,
      'draggable' => 1,
      'hide' => $empty ? 1 : 0,
    ),
  );
  if ($element['#enable_bounds']) {
    $empty = $empty || empty($element['#value']['bounds']);
    $p = NULL;
    if (!$empty) {
      $p = (object)$element['#value']['bounds']['northeast'];
      $p->map_type = !empty($p->map_type) ? $p->map_type : $element['#map_type'];
    }
    if ($empty || gmaps_point_empty($p)) {
      $p = gmaps_extend(new stdClass(), $point);
      unset($p->bounds);
      $p->latitude += 1;
      $p->longitude += 1;
    }
    $gmi->overlays['marker']['ne_bound'] = array(
      'point' => $p,
      'basic' => array(
        'icon' => gmaps_icon_get_icid_for_default('bound_ne'),
      ),
      'skip_privacy' => TRUE,
      'options' => array(
        'clickable' => 1,
        'draggable' => 1,
        'hide' => ($empty || gmaps_point_empty((object)$element['#value']['bounds']['northeast'])) ? 1 : 0,
      ),
      'tracker' => array(
        'color' => '#0000ff',
      ),
    );

    $p = NULL;
    if (!$empty) {
      $p = (object)$element['#value']['bounds']['southwest'];
      $p->map_type = !empty($p->map_type) ? $p->map_type : $element['#map_type'];
    }
    if ($empty || gmaps_point_empty($p)) {
      $p = gmaps_extend(new stdClass(), $point);
      unset($p->bounds);
      $p->latitude -= 1;
      $p->longitude -= 1;
    }
    $gmi->overlays['marker']['sw_bound'] = array(
      'point' => $p,
      'basic' => array(
        'icon' => gmaps_icon_get_icid_for_default('bound_sw'),
      ),
      'skip_privacy' => TRUE,
      'options' => array(
        'clickable' => 1,
        'draggable' => 1,
        'hide' => ($empty || gmaps_point_empty((object)$element['#value']['bounds']['southwest'])) ? 1 : 0,
      ),
      'tracker' => array(
        'color' => '#0000ff',
      ),
    );
  }
  
  $gmi->attributes['class'] = 'input-gmi input-gmi-point input-gmi-point-'. $element['#map_type'];
}



/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_point element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing the gmaps_point element.
 *
 * @ingroup themeable
 */
function theme_gmaps_point($element) {
  return theme('gmaps_collapsible_element', $element, 'form-gmaps-point-collapsible');
}
