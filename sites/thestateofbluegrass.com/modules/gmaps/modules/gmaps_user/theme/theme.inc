<?php
// $Id: theme.inc,v 1.1.2.3 2010/04/01 07:42:11 xmarket Exp $

/**
 * @file
 * Theme preprocess functions for gmaps_user's themeables.
 */

/**
 * Theme function for 'default' gmaps_user formatter
 */
function theme_gmaps_user_formatter_default($account, $geo = NULL) {
  global $user;
  
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  if (empty($geo['type']) || empty($geo['formatter']['formatter']) || empty($geo_data)) {
    return;
  }

  $backup = $user->gmaps_settings['address']['taxonomy_links'];
  $user->gmaps_settings['address']['taxonomy_links'] = FALSE;
  
  $output = '';
  if ($geo['type'] == 'gmaps_address') {
    if (!gmaps_address_empty($geo_data->address)) {
      $output = gmaps_address_render($geo_data->address);
    }
  }
  else if ($geo['type'] == 'gmaps_point') {
    if (!gmaps_point_empty($geo_data->point)) {
      $output = gmaps_coords_render($geo_data->point);
    }
  }
  else if ($geo['type'] == 'gmaps_anp') {
    if (!gmaps_anp_empty($geo_data)) {
      $fmt = $geo['formatter'];
      $a = $p = '';
      if ($fmt['show'] != 'point') {
        $a = gmaps_address_render($geo_data->address);
        if ($fmt['show'] == 'address') {
          $output = $a;
        }
      }
      if ($fmt['show'] != 'address') {
        $p = gmaps_coords_render($geo_data->point);
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
  }
  
  if ($output && ($user->gmaps_settings['address']['formatter'] == 'themed' || $user->gmaps_settings['coordinate']['formatter'] == 'themed')) {
    if ($links = gmaps_anp_map_links($geo_data, $geo['formatter']['map_links'])) {
      $output .= theme('item_list', $links);
    }
  }
  
  $user->gmaps_settings['address']['taxonomy_links'] = $backup;
  
  return $output;
}

/**
 * Theme function for 'plain' gmaps_user formatter
 */
function theme_gmaps_user_formatter_plain($account, $geo = NULL) {
  global $user;
  
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  if (empty($geo['type']) || empty($geo['formatter']['formatter']) || empty($geo_data)) {
    return;
  }

  $backup = $user->gmaps_settings['address']['taxonomy_links'];
  $user->gmaps_settings['address']['taxonomy_links'] = FALSE;
  
  $output = '';
  if ($geo['type'] == 'gmaps_address') {
    if (!gmaps_address_empty($geo_data->address)) {
      $s = $geo['formatter']['address'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['address'];
      }
      $output = gmaps_address_format_plain($geo_data->address, $s['country'], $s['state']);
    }
  }
  else if ($geo['type'] == 'gmaps_point') {
    if (!gmaps_point_empty($geo_data->point)) {
      $s = $geo['formatter']['point'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['coordinate'];
      }
      $output = gmaps_coords_format_plain($geo_data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
    }
  }
  else if ($geo['type'] == 'gmaps_anp') {
    if (!gmaps_anp_empty($geo_data)) {
      $fmt = $geo['formatter'];
      $a = $p = '';
      if ($fmt['show'] != 'point') {
        $s = $fmt['address'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['address'];
        }
        $a = gmaps_address_format_plain($geo_data->address, $s['country'], $s['state']);
        if ($fmt['show'] == 'address') {
          $output = $a;
        }
      }
      if ($fmt['show'] != 'address') {
        $s = $fmt['point'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['coordinate'];
        }
        $p = gmaps_coords_format_plain($geo_data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
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
  }
  
  $user->gmaps_settings['address']['taxonomy_links'] = $backup;
  
  return $output;
}

/**
 * Theme function for 'themed' gmaps_user formatter
 */
function theme_gmaps_user_formatter_themed($account, $geo = NULL) {
  global $user;
  
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  if (empty($geo['type']) || empty($geo['formatter']['formatter']) || empty($geo_data)) {
    return;
  }

  $backup = $user->gmaps_settings['address']['taxonomy_links'];
  $user->gmaps_settings['address']['taxonomy_links'] = FALSE;
  
  $output = '';
  if ($geo['type'] == 'gmaps_address') {
    if (!gmaps_address_empty($geo_data->address)) {
      $s = $geo['formatter']['address'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['address'];
      }
      $output = gmaps_address_format($geo_data->address, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);
    }
  }
  else if ($geo['type'] == 'gmaps_point') {
    if (!gmaps_point_empty($geo_data->point)) {
      $s = $geo['formatter']['point'];
      if ($s['user_settings']) {
        $s = $user->gmaps_settings['coordinate'];
      }
      $output = gmaps_coords_format($geo_data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
    }
  }
  else if ($geo['type'] == 'gmaps_anp') {
    if (!gmaps_anp_empty($geo_data)) {
      $fmt = $geo['formatter'];
      $a = $p = '';
      if ($fmt['show'] != 'point') {
        $s = $fmt['address'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['address'];
        }
        $a = gmaps_address_format($geo_data->address, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);
        if ($fmt['show'] == 'address') {
          $output = $a;
        }
      }
      if ($fmt['show'] != 'address') {
        $s = $fmt['point'];
        if ($s['user_settings']) {
          $s = $user->gmaps_settings['coordinate'];
        }
        $p = gmaps_coords_format($geo_data->point, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
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
  }
  
  if ($output && ($links = gmaps_anp_map_links($geo_data, $geo['formatter']['map_links']))) {
    $output .= theme('item_list', $links);
  }
  
  $user->gmaps_settings['address']['taxonomy_links'] = $backup;
  
  return $output;
}

/**
 * Theme function for 'map' gmaps_user formatter
 */
function theme_gmaps_user_formatter_map($account, $geo = NULL) {
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  if (!in_array($geo['type'], array('gmaps_point', 'gmaps_anp')) || empty($geo['formatter']['map']['map']) || empty($geo_data)) {
    return '';
  }
  
  $cache = array();
  if ($geo['formatter']['map']['cache']) {
    $expires = $geo['formatter']['map']['cache_expires'];
    $cache = array(
      'cid' => $account->uid,
      'expires' => empty($expires) ? CACHE_PERMANENT : ($expires < 0 ? CACHE_TEMPORARY : (time() + $expires)),
      'table' => GMAPS_USER_CACHE_TABLE_MAP,
      'user_mask' => GMAPS_CACHE_NO_USER,
    );
  }
  
  return gmaps_map_item_render($geo['formatter']['map']['map'], '_gmaps_user_profile_gmi', array($account, $geo), $cache, 'user-profile-'. $account->uid);
}

/**
 * Theme function for 'static_map' gmaps_user formatter
 */
function theme_gmaps_user_formatter_static_map($account, $geo = NULL) {
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  if ($geo['options']['map_type'] != GMAPS_MAP_EARTH && !in_array($geo['type'], array('gmaps_point', 'gmaps_anp')) || empty($geo['formatter']['map']['static_map']) || empty($geo_data)) {
    return '';
  }
  
  $cache = array();
  if ($geo['formatter']['map']['cache']) {
    $expires = $geo['formatter']['map']['cache_expires'];
    $cache = array(
      'cid' => $account->uid,
      'expires' => empty($expires) ? CACHE_PERMANENT : ($expires < 0 ? CACHE_TEMPORARY : (time() + $expires)),
      'table' => GMAPS_USER_CACHE_TABLE_STATIC_MAP,
      'user_mask' => GMAPS_CACHE_NO_USER,
    );
  }
  
  return gmaps_static_map_item_render($geo['formatter']['map']['static_map'], '_gmaps_user_profile_smi', array($account, $geo), $cache, 'user-profile-'. $account->uid);
}

function _gmaps_user_profile_gmi(&$gmi, $account, $geo = NULL) {
  $gmi->overlays['marker'] = array();
  
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  $marker = array(
    'point' => $geo_data->point,
    'title' => check_plain($account->name),
  );

  $content_geo = $geo;
  $content_geo['formatter']['formatter'] = $geo['formatter']['map']['content'];
  if ($content = gmaps_user_profile_format($account, $content_geo)) {
    $marker['content'] = array('min' => $content);
  }
  
  if ($mo_data = $account->gmaps['mo']) {
    if ($mo_data->icon != GMAPS_ICON_MAP_DEFAULT) {
      $marker['basic']['icon'] = $mo_data->icon;
    }
    
    if ($mo_data->protected_icon != GMAPS_ICON_MAP_DEFAULT) {
      $marker['basic']['protected_icon'] = $mo_data->protected_icon;
    }
  }
  $gmi->overlays['marker']['profile_'. $account->uid] = $marker;
  
  $gmi->attributes['class'] = 'user-profile-gmi user-profile-gmi-earth';
}

function _gmaps_user_profile_smi(&$smi, $account, $geo = NULL) {
  $smi->overlays['marker'] = array();
  
  if (empty($geo)) {
    $geo = variable_get('gmaps_user_geo_data', array());
  }
  $geo_data = $account->gmaps['geo'];
  
  $marker = array(
    'point' => $geo_data->point,
    'title' => check_plain($account->name),
  );

  $content_geo = $geo;
  $content_geo['formatter']['formatter'] = $geo['formatter']['map']['content'];
  if ($content = gmaps_user_profile_format($account, $content_geo)) {
    $marker['content'] = array('min' => $content);
  }
  
  if ($mo_data = $account->gmaps['mo']) {
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
  $smi->overlays['marker']['profile_'. $account->uid] = $marker;
  
  $smi->attributes['class'] = 'user-profile-smi user-profile-smi-earth';
}
