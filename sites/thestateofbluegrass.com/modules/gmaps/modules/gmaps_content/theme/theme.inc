<?php
// $Id: theme.inc,v 1.1.2.7 2010/02/26 14:13:03 xmarket Exp $

/**
 * @file
 * Theme preprocess functions for gmaps_content's themeables.
 */

/**
 * Theme function for 'default' gmaps_content formatter
 */
function theme_gmaps_content_formatter_default($element) {
  global $user;
  
  if (isset($element['#field'])) {
    $field = $element['#field'];
  }
  else {
    $field = content_fields($element['#field_name'], $element['#type_name']);
  }
  $item = $element['#item'];

  $output = '';
  if ($field['type'] == 'gmaps_distance') {
    $s = $field['widget']['gmaps'];
    if ($s['user_settings']) {
      $s = $user->gmaps_settings['length'];
    }
    if (!is_object($s['unit'])) {
      $s['unit'] = gmaps_length_unit_load($s['unit']);
    }
    return gmaps_length_render($item, $s);
  }
  else if ($field['type'] == 'gmaps_address') {
    $output = gmaps_address_render($item['value']->address);
  }
  else if ($field['type'] == 'gmaps_point') {
    $output = gmaps_coords_render($item['value']->point);
  }
  else if ($field['type'] == 'gmaps_anp') {
    $fmt = $field['widget']['gmaps']['formatter'];
    $a = $p = '';
    if ($fmt['show'] != 'point') {
      $a = gmaps_address_render($item['value']->address);
      if ($fmt['show'] == 'address') {
        $output = $a;
      }
    }
    if ($fmt['show'] != 'address') {
      $p = gmaps_coords_render($item['value']->point);
      if ($fmt['show'] == 'point') {
        $output = $p;
      }
    }
    if ($fmt['show'] == 'anp') {
      $output = $a .' '. $p;
    }
    else if ($fmt['show'] == 'pna') {
      $output = $p .' '. $a;
    }
  }
  
  if ($output && ($user->gmaps_settings['address']['formatter'] == 'themed' || $user->gmaps_settings['coordinate']['formatter'] == 'themed')) {
    if ($links = gmaps_anp_map_links($item['value'], $field['widget']['gmaps']['formatter']['map_links'])) {
      $output .= theme('item_list', $links);
    }
  }
  
  return $output;
}

/**
 * Theme function for 'plain' gmaps_content formatter
 */
function theme_gmaps_content_formatter_plain($element) {
  global $user;
  
  if (isset($element['#field'])) {
    $field = $element['#field'];
  }
  else {
    $field = content_fields($element['#field_name'], $element['#type_name']);
  }
  $item = $element['#item'];

  if ($field['type'] == 'gmaps_distance') {
    $s = $field['widget']['gmaps'];
    return gmaps_length_format_plain($item, $s['user_settings'] ? $user->gmaps_settings['length']['precision'] : $s['precision']);
  }
  else if ($field['type'] == 'gmaps_address') {
    $s = $field['widget']['gmaps']['formatter']['address'];
    if ($s['user_settings']) {
      $s = $user->gmaps_settings['address'];
    }
    return gmaps_address_format_plain($item['value']->address, $s['country'], $s['state']);
  }
  else if ($field['type'] == 'gmaps_point') {
    $s = $field['widget']['gmaps']['formatter']['point'];
    if ($s['user_settings']) {
      $s = $user->gmaps_settings['coordinate'];
    }
    return gmaps_coords_format_plain($item['value']->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
  }
  else if ($field['type'] == 'gmaps_anp') {
    $fmt = $field['widget']['gmaps']['formatter'];
    $a = $p = '';
    if ($fmt['show'] != 'point') {
      $s = $fmt['address'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['address'];
      }
      $a = gmaps_address_format_plain($item['value']->address, $s['country'], $s['state']);
      if ($fmt['show'] == 'address') {
        return $a;
      }
    }
    if ($fmt['show'] != 'address') {
      $s = $fmt['point'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['coordinate'];
      }
      $p = gmaps_coords_format_plain($item['value']->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
      if ($fmt['show'] == 'point') {
        return $p;
      }
    }
    if ($fmt['show'] == 'anp') {
      return $a .' '. $p;
    }
    else if ($fmt['show'] == 'pna') {
      return $p .' '. $a;
    }
  }
}

/**
 * Theme function for 'themed' gmaps_content formatter
 */
function theme_gmaps_content_formatter_themed($element) {
  global $user;
  
  if (isset($element['#field'])) {
    $field = $element['#field'];
  }
  else {
    $field = content_fields($element['#field_name'], $element['#type_name']);
  }
  $item = $element['#item'];

  $output = '';
  if ($field['type'] == 'gmaps_distance') {
    $s = $field['widget']['gmaps'];
    if (true || $s['user_settings']) {
      $s = $user->gmaps_settings['length'];
    }
    return gmaps_length_format($item, $s['precision'], $s['multipart'], $s['optmode']);
  }
  else if ($field['type'] == 'gmaps_address') {
    $s = $field['widget']['gmaps']['formatter']['address'];
    if ($s['user_settings']) {
      $s = $user->gmaps_settings['address'];
    }
    $output = gmaps_address_format($item['value']->address, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);
  }
  else if ($field['type'] == 'gmaps_point') {
    $s = $field['widget']['gmaps']['formatter']['point'];
    if ($s['user_settings']) {
      $s = $user->gmaps_settings['coordinate'];
    }
    $output = gmaps_coords_format($item['value']->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
  }
  else if ($field['type'] == 'gmaps_anp') {
    $fmt = $field['widget']['gmaps']['formatter'];
    $a = $p = '';
    if ($fmt['show'] != 'point') {
      $s = $fmt['address'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['address'];
      }
      $a = gmaps_address_format($item['value']->address, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);
      if ($fmt['show'] == 'address') {
        $output = $a;
      }
    }
    if ($fmt['show'] != 'address') {
      $s = $fmt['point'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['coordinate'];
      }
      $p = gmaps_coords_format($item['value']->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
      if ($fmt['show'] == 'point') {
        $output = $p;
      }
    }
    if ($fmt['show'] == 'anp') {
      $output = $a .' '. $p;
    }
    else if ($fmt['show'] == 'pna') {
      $output = $p .' '. $a;
    }
  }
  
  if ($output && ($links = gmaps_anp_map_links($item['value'], $field['widget']['gmaps']['formatter']['map_links']))) {
    $output .= theme('item_list', $links);
  }
  
  return $output;
}

/**
 * Theme function for 'map' gmaps_content formatter
 */
function theme_gmaps_content_formatter_map($element) {
  if (isset($element['#field'])) {
    $field = $element['#field'];
  }
  else {
    $field = content_fields($element['#field_name'], $element['#type_name']);
  }
    
  if (!in_array($field['type'], array('gmaps_point', 'gmaps_anp')) || empty($field['widget']['gmaps']['formatter']['map']['map']) || !count(element_children($element))) {
    return '';
  }
  
  $cache = array();
  if ($field['widget']['gmaps']['formatter']['map']['cache']) {
    $expires = $field['widget']['gmaps']['formatter']['map']['cache_expires'];
    $cache = array(
      'cid' => gmaps_content_field_map_cid($element['#node'], $field),
      'expires' => empty($expires) ? CACHE_PERMANENT : ($expires < 0 ? CACHE_TEMPORARY : (time() + $expires)),
      'table' => GMAPS_CONTENT_CACHE_TABLE_MAP,
      'user_mask' => _gmaps_content_map_cache_user_mask($field),
    );
  }
  
  return gmaps_map_item_render($field['widget']['gmaps']['formatter']['map']['map'], '_gmaps_content_field_gmi', array($element, $field), $cache, 'content-field-'. $element['#node']->vid .'-'. $field['field_name']);
}

/**
 * Theme function for 'static_map' gmaps_content formatter
 */
function theme_gmaps_content_formatter_static_map($element) {
  if (isset($element['#field'])) {
    $field = $element['#field'];
  }
  else {
    $field = content_fields($element['#field_name'], $element['#type_name']);
  }
    
  if ($field['gmaps']['map_type'] != GMAPS_MAP_EARTH && !in_array($field['type'], array('gmaps_point', 'gmaps_anp')) || empty($field['widget']['gmaps']['formatter']['map']['static_map']) || !count(element_children($element))) {
    return '';
  }
  
  $cache = array();
  if ($field['widget']['gmaps']['formatter']['map']['cache']) {
    $expires = $field['widget']['gmaps']['formatter']['map']['cache_expires'];
    $cache = array(
      'cid' => gmaps_content_field_map_cid($element['#node'], $field),
      'expires' => empty($expires) ? CACHE_PERMANENT : ($expires < 0 ? CACHE_TEMPORARY : (time() + $expires)),
      'table' => GMAPS_CONTENT_CACHE_TABLE_STATIC_MAP,
      'user_mask' => _gmaps_content_map_cache_user_mask($field),
    );
  }
  
  return gmaps_static_map_item_render($field['widget']['gmaps']['formatter']['map']['static_map'], '_gmaps_content_field_smi', array($element, $field), $cache, 'content-field-'. $element['#node']->vid .'-'. $field['field_name']);
}

function _gmaps_content_map_cache_user_mask($field) {
  $content = $field['widget']['gmaps']['formatter']['map']['content'];
  if ($field['gmaps']['enable_privacy'] || ($content != 'hidden' && variable_get('gmaps_enable_user_settings', 0))) {
    return GMAPS_CACHE_PER_USER;
  }
  
  return GMAPS_CACHE_NO_USER;
}

function _gmaps_content_field_gmi(&$gmi, $element, $field) {
  $gmi->overlays['marker'] = array();
  
  foreach(element_children($element) as $delta) {
    $item = $element[$delta]['#item'];
    
    if (!is_array($item)) {
      $item = array('value' => $item);
    }
    
    $marker = array(
      'point' => $item['value']->point,
      'sticky' => $element['#node']->sticky,
      'title' => check_plain(t($field['widget']['label'])),
    );

    if ($content = content_format($field, $item, $field['widget']['gmaps']['formatter']['map']['content'], $element['#node'])) {
      $marker['content'] = array('min' => $content);
    }
    
    $gmi->overlays['marker'][$field['field_name'] .'_'. $delta] = $marker;
  }
  
  $classes = array(
    '',
    $field['type'] == 'gmaps_anp' ? GMAPS_MAP_EARTH : strtr($field['gmaps']['map_type'], '_', '-'),
    'type-'. strtr($field['type'], '_', '-'),
    strtr($field['field_name'], '_', '-'),
  );
  $gmi->attributes['class'] = 'content-field-gmi '. implode(' content-field-gmi-', $classes);
}

function _gmaps_content_field_smi(&$smi, $element, $field) {
  $smi->overlays['marker'] = array();
  
  foreach(element_children($element) as $delta) {
    $item = $element[$delta]['#item'];
    
    if (!is_array($item)) {
      $item = array('value' => $item);
    }
    
    $marker = array(
      'point' => $item['value']->point,
      'sticky' => $element['#node']->sticky,
      'title' => check_plain(t($field['widget']['label'])),
    );

    if ($content = content_format($field, $item, $field['widget']['gmaps']['formatter']['map']['content'], $element['#node'])) {
      $marker['content'] = array('min' => $content);
    }
    
    $smi->overlays['marker'][$field['field_name'] .'_'. $delta] = $marker;
  }
  
  $classes = array(
    '',
    $field['type'] == 'gmaps_anp' ? GMAPS_MAP_EARTH : strtr($field['gmaps']['map_type'], '_', '-'),
    'type-'. strtr($field['type'], '_', '-'),
    strtr($field['field_name'], '_', '-'),
  );
  $smi->attributes['class'] = 'content-field-smi '. implode(' content-field-smi-', $classes);
}
