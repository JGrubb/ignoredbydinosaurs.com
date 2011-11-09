<?php
// $Id: theme.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Theme preprocess functions for gmaps_term's themeables.
 */

/**
 * Theme function for 'default' gmaps_term formatter
 */
function theme_gmaps_term_formatter_default($data, $data_type, $options) {
  global $user;
  
  if (empty($options['formatter']) || empty($data)) {
    return;
  }

  $backup = $user->gmaps_settings['address']['taxonomy_links'];
  $user->gmaps_settings['address']['taxonomy_links'] = FALSE;
  
  $output = '';
  if ($data_type == 'gmaps_address') {
    if (!gmaps_address_empty($data->address)) {
      $output = gmaps_address_render($data->address);
    }
  }
  else if ($data_type == 'gmaps_point') {
    if (!gmaps_point_empty($data->point)) {
      $output = gmaps_coords_render($data->point);
    }
  }
  else if ($data_type == 'gmaps_anp') {
    if (!gmaps_anp_empty($data)) {
      $a = $p = '';
      if ($options['show'] != 'point') {
        $a = gmaps_address_render($data->address);
        if ($options['show'] == 'address') {
          $output = $a;
        }
      }
      if ($options['show'] != 'address') {
        $p = gmaps_coords_render($data->point);
        if ($options['show'] == 'point') {
          $output = $p;
        }
      }
      if ($options['show'] == 'anp') {
        $output = $a .' '. $p;
      }
      else if ($options['show'] == 'pna') {
        $output = $p .' '. $a;
      }
    }
  }
  
  if ($output && ($user->gmaps_settings['address']['formatter'] == 'themed' || $user->gmaps_settings['coordinate']['formatter'] == 'themed')) {
    if ($links = gmaps_anp_map_links($data, $options['map_links'])) {
      $output .= theme('item_list', $links);
    }
  }
  
  $user->gmaps_settings['address']['taxonomy_links'] = $backup;
  
  return $output;
}

/**
 * Theme function for 'plain' gmaps_term formatter
 */
function theme_gmaps_term_formatter_plain($data, $data_type, $options) {
  global $user;
  
  if (empty($options['formatter']) || empty($data)) {
    return;
  }

  $backup = $user->gmaps_settings['address']['taxonomy_links'];
  $user->gmaps_settings['address']['taxonomy_links'] = FALSE;
  
  $output = '';
  if ($data_type == 'gmaps_address') {
    if (!gmaps_address_empty($data->address)) {
      $s = $options['address'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['address'];
      }
      $output = gmaps_address_format_plain($data->address, $s['country'], $s['state']);
    }
  }
  else if ($data_type == 'gmaps_point') {
    if (!gmaps_point_empty($data->point)) {
      $s = $options['point'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['coordinate'];
      }
      $output = gmaps_coords_format_plain($data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
    }
  }
  else if ($data_type == 'gmaps_anp') {
    if (!gmaps_anp_empty($data)) {
      $a = $p = '';
      if ($options['show'] != 'point') {
        $s = $options['address'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['address'];
        }
        $a = gmaps_address_format_plain($data->address, $s['country'], $s['state']);
        if ($options['show'] == 'address') {
          $output = $a;
        }
      }
      if ($options['show'] != 'address') {
        $s = $options['point'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['coordinate'];
        }
        $p = gmaps_coords_format_plain($data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
        if ($options['show'] == 'point') {
          $output = $p;
        }
      }
      if ($options['show'] == 'anp') {
        $output = $a .' '. $p;
      }
      else if ($options['show'] == 'pna') {
        $output = $p .' '. $a;
      }
    }
  }
  
  $user->gmaps_settings['address']['taxonomy_links'] = $backup;
  
  return $output;
}

/**
 * Theme function for 'themed' gmaps_term formatter
 */
function theme_gmaps_term_formatter_themed($data, $data_type, $options) {
  global $user;
  
  if (empty($options['formatter']) || empty($data)) {
    return;
  }

  $backup = $user->gmaps_settings['address']['taxonomy_links'];
  $user->gmaps_settings['address']['taxonomy_links'] = FALSE;
  
  $output = '';
  if ($data_type == 'gmaps_address') {
    if (!gmaps_address_empty($data->address)) {
      $s = $options['address'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['address'];
      }
      $output = gmaps_address_format($data->address, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);
    }
  }
  else if ($data_type == 'gmaps_point') {
    if (!gmaps_point_empty($data->point)) {
      $s = $options['point'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['coordinate'];
      }
      $output = gmaps_coords_format($data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
    }
  }
  else if ($data_type == 'gmaps_anp') {
    if (!gmaps_anp_empty($data)) {
      $a = $p = '';
      if ($options['show'] != 'point') {
        $s = $options['address'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['address'];
        }
        $a = gmaps_address_format($data->address, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);
        if ($options['show'] == 'address') {
          $output = $a;
        }
      }
      if ($options['show'] != 'address') {
        $s = $options['point'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['coordinate'];
        }
        $p = gmaps_coords_format($data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
        if ($options['show'] == 'point') {
          $output = $p;
        }
      }
      if ($options['show'] == 'anp') {
        $output = $a .' '. $p;
      }
      else if ($options['show'] == 'pna') {
        $output = $p .' '. $a;
      }
    }
  }
  
  if ($output && ($links = gmaps_anp_map_links($data, $options['map_links']))) {
    $output .= theme('item_list', $links);
  }
  
  $user->gmaps_settings['address']['taxonomy_links'] = $backup;
  
  return $output;
}

/**
 * Theme function for 'map' gmaps_term formatter
 */
function theme_gmaps_term_formatter_map($data, $data_type, $options) {
  if (!in_array($data_type, array('gmaps_point', 'gmaps_anp')) || empty($options['map']['map']) || empty($data)) {
    return '';
  }
  
  return gmaps_map_item_render($options['map']['map'], '_gmaps_term_geo_data_gmi', array($data, $data_type, $options), array(), 'term-data-'. $data->tid);
}

/**
 * Theme function for 'static_map' gmaps_user formatter
 */
function theme_gmaps_term_formatter_static_map($data, $data_type, $options) {
  if (empty($data) || empty($data->point) || $data->point->map_type != GMAPS_MAP_EARTH && !in_array($data_type, array('gmaps_point', 'gmaps_anp')) || empty($options['map']['static_map'])) {
    return '';
  }
  
  return gmaps_static_map_item_render($options['map']['static_map'], '_gmaps_term_geo_data_smi', array($data, $data_type, $options), array(), 'term-data-'. $data->tid);
}

function _gmaps_term_geo_data_gmi(&$gmi, $data, $data_type, $options) {
  $gmi->overlays['marker'] = array();
  
  $term = taxonomy_get_term($data->tid);
  
  $marker = array(
    'point' => $data->point,
    'title' => check_plain($term->name),
  );

  $content_options = $options;
  $content_options['formatter'] = $options['map']['content'];
  if ($content = gmaps_term_geo_data_render($data, $data_type, $content_options)) {
    $marker['content'] = array('min' => $content);
  }
  
  if ($mo_data = gmaps_term_mo_data_load($data->tid)) {
    if ($mo_data->icon != GMAPS_ICON_MAP_DEFAULT) {
      $marker['basic']['icon'] = $mo_data->icon;
    }
    
    if ($mo_data->protected_icon != GMAPS_ICON_MAP_DEFAULT) {
      $marker['basic']['protected_icon'] = $mo_data->protected_icon;
    }
  }
  $gmi->overlays['marker']['term_'. $data->tid] = $marker;
  
  $gmi->attributes['class'] = 'term-data-gmi term-data-gmi-'. $data->point->map_type;
}

function _gmaps_term_geo_data_smi(&$smi, $data, $data_type, $options) {
  $smi->overlays['marker'] = array();
  
  $term = taxonomy_get_term($data->tid);
  
  $marker = array(
    'point' => $data->point,
    'title' => check_plain($term->name),
  );

  $content_options = $options;
  $content_options['formatter'] = $options['map']['content'];
  if ($content = gmaps_term_geo_data_render($data, $data_type, $content_options)) {
    $marker['content'] = array('min' => $content);
  }
  
  if ($mo_data = gmaps_term_mo_data_load($data->tid)) {
    $icon = $mo_data->static_icon;
    if (!empty($icon)) {
      $marker['style'] = $icon['style'];
      $marker['style_sticky'] = $icon['style_sticky'];
    }
    
    $icon = $mo_data->protected_static_icon;
    if (!empty($icon)) {
      $marker['protected'] = $icon['style'];
      $marker['protected_sticky'] = $icon['style_sticky'];
    }
  }
  $smi->overlays['marker']['term_'. $data->tid] = $marker;
  
  $smi->attributes['class'] = 'term-data-smi term-data-smi-'. $data->point->map_type;
}
