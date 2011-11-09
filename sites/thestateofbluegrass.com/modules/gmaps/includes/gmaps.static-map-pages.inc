<?php
// $Id: gmaps.static-map-pages.inc,v 1.1.2.10 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Static map renderer functions.
 */

function _gmaps_static_map_item_render($base, $callback = NULL, $args = array(), $cache = array(), $id = NULL) {
  global $gmaps_throttle, $theme, $user;
  $current_throttle = $gmaps_throttle;
  
  $key = _gmaps_get_api_key();
  if (!$key) {
    drupal_set_message(t('No Google Maps API key available.'), 'error');
    return '';
  }
  
  if (is_object($base)) {
    $base_clone = (object)gmaps_extend(array(), $base);
  }
  else {
    $base_clone = gmaps_static_map_load($base);
    if (!$base_clone) {
      //return this message in place of the map to let the user know which map is broken,
      //when multiple maps exists on the same page.
      drupal_set_message(t('Invalid map ID.'), 'error gmaps-static-map-error');
      return theme('status_messages', 'error gmaps-static-map-error');
    }
  }
  
  $smi_id = 'smi-'. (empty($id) ? ($callback ? $callback : 'gmaps') : $id);
  $smi_id = _gmaps_static_map_item_clean_id($smi_id);
  
  if ($user->uid != 1 && variable_get('gmaps_static_map_cache', 1)) {
    if (empty($theme)) {
      init_theme();
    }
    
    //set default cache table and user mask if not set
    $cache += array('table' => GMAPS_CACHE_TABLE_STATIC_MAP, 'user_mask' => GMAPS_CACHE_PER_ROLE);
    
    $cid_parts = array(empty($cache['cid']) ? md5($smi_id . (empty($args) ? '' : '-'. serialize($args))) : $cache['cid']);
    $cid_parts[] = "b.$base_clone->mid";
    $cid_parts[] = "t.$theme";
    if (module_exists('locale')) {
      global $language;
      $cid_parts[] = "l.$language->language";
    }
    
    //force per user if there are any node access related modules
    if ($cache['user_mask'] == GMAPS_CACHE_PER_ROLE && count(module_implements('node_grants'))) {
      $cache['user_mask'] = GMAPS_CACHE_PER_USER;
    }
    if ($cache['user_mask'] == GMAPS_CACHE_PER_ROLE) {
      $cid_parts[] = "r.". implode(',', array_keys($user->roles));
    }
    else if ($cache['user_mask'] == GMAPS_CACHE_PER_USER) {
      $cid_parts[] = "u$user->uid.";
    }
    
    $smi_cid = implode(':', $cid_parts);
  }
  
  $se_agent = gmaps_agent_is_search_engine();
    
  //track se agent on all maps
  if ($se_agent && variable_get('gmaps_se_track_access', 1)) {
    watchdog('gmaps se access', 'The robot %robot has been accessed the map %map', array('%robot' => $_SERVER['HTTP_USER_AGENT'], '%map' => $base_clone->name .($callback ? ' ('. $callback .')' : '')), WATCHDOG_INFO);
  }

  if ($se_agent && $base_clone->se_support) {
    $current_throttle = 0;
  }
  //skip one or two db_query if possible
  else if ($user->uid != 1 && variable_get('gmaps_static_map_cache', 1)) {
    if (!empty($cache['cid']) && ($cached = cache_get($smi_cid, $cache['table']))) {
      if ($cached->expire == CACHE_PERMANENT || $cached->expire > time()) {
        $smi = $cached->data;
        $smi->cached = TRUE;
      }
    }
    if (!$smi && $current_throttle && $base_clone->throttle_support && ($cached = cache_get($smi_cid .':_throttle_', $cache['table']))) {
      if ($cached->expire == CACHE_PERMANENT || $cached->expire > time()) {
        $smi = $cached->data;
        $smi->cached = TRUE;
      }
    }
  }
  
  if (!isset($smi)) {
    $smi = (object)array(
      'id' => $smi_id,
      'base' => $base_clone,
      'throttle' => $base_clone->throttle_support && $current_throttle,
      'se_agent' => ($se_agent && $base_clone->se_support),
      'cached' => FALSE,
      'cachable' => FALSE,
      'cache' => $smi_cid ? $cache : array(),
      'overlays' => array(),
      'content' => array(),
      'after_process' => array(),
      'attributes' => array(),
      'url' => '',
      'headers' => array(),
      'view' => '',
    );
    if ($smi_cid) {
      $smi->cid = $smi_cid .($smi->throttle ? ':_throttle_' : '');
    }
    
    //not admin user, not se agent, global switch on
    if ($smi->cid && $user->uid != 1 && !$se_agent && variable_get('gmaps_static_map_cache', 1)) {
      //throttle cache
      if ($smi->throttle) {
        if ($smi->base->throttle_lifetime) {
          $smi->cachable = TRUE;
        }
      }
      //regular cache
      else if (!empty($cache['cid']) && isset($cache['expires'])) {
        $smi->cachable = TRUE;
      }
    }
    
    _gmaps_static_map_item_process($smi, $callback, $args);
  }
  
  if ($smi) {
    if (!empty($smi->after_process)) {
      foreach($smi->after_process as $function) {
        if (function_exists($function)) {
          $function($smi);
        }
      }
    }
    
    if ($smi->cached) {
      if (!$se_agent) {
        gmaps_restore_headers($smi->headers);
      }
    }
    else {
      //never cache se visits, because they visit 'everything',
      //which, in turn, can result in killer large cache tables.
      if (!$se_agent && $smi->cid && $smi->cachable && variable_get('gmaps_static_map_cache', 1)) {
        $smi->headers = gmaps_gather_headers();
      }
      
      $smi->view = theme('gmaps_static_map_item', $smi);
      
      if (!$se_agent && $smi->cid && $smi->cachable && variable_get('gmaps_static_map_cache', 1)) {
        $smi->headers = gmaps_gather_headers($smi->headers);
    
        cache_set($smi->cid, $smi, $cache['table'], $smi->throttle ? (time() + $smi->base->throttle_lifetime) : $cache['expires']);
      }
    }
    
    return $smi->view;
  }
  
  return '';
}

function _gmaps_static_map_item_clean_id($id) {
  static $seen_ids = array();

  $id = str_replace(array('][', '_', ' '), '-', $id);

  if (isset($seen_ids[$id])) {
    $id = $id .'-'. $seen_ids[$id]++;
  }
  else {
    $seen_ids[$id] = 1;
  }

  return $id;
}

/**
 * Set throttle restriction, add content and process it.
 * 
 * @param obejct $map
 * @param function $callback
 * @param array $args
 *  Arguments for $callback. Will be prepended with the $map.
 * 
 * @return null
 */
function _gmaps_static_map_item_process(&$smi, $callback, $args) {
  if ($smi->throttle || $smi->se_agent) {
    if ($smi->se_agent) {
      $smi->base->content['ajax'] = 0;
      $smi->base->content['style'] = 'box';
      $smi->base->content['content']['style'] = 'box';
    }
    if ($smi->throttle && $smi->base->content['style'] == 'fieldset') {
      $smi->base->content['style'] = 'box';
    }
    if ($smi->throttle && $smi->base->content['content']['style'] == 'fieldset') {
      $smi->base->content['content']['style'] = 'box';
    }
  }
  if (!in_array($smi->base->content['style'], array('accordion', 'tabs'))) {
    $smi->base->content['ajax'] = 0;
  }

  if ($callback) {
    array_unshift($args, $smi);
    call_user_func_array($callback, $args);
  }
  _gmaps_static_map_item_process_ov_marker($smi);
  _gmaps_static_map_item_process_ov_path($smi);
  
  if (!empty($smi->base->content['ahah']['message'])) {
    $smi->base->content['ahah']['message'] = t($smi->base->content['ahah']['message']);
  }

  drupal_alter('gmaps_static_map_item', $smi);
  
  //url
  _gmaps_static_map_item_build_url($smi);

  _gmaps_static_map_item_cache_image($smi);
}

function _gmaps_static_map_item_cache_image(&$smi) {
  if (empty($smi->cid) || empty($smi->url) || !variable_get('gmaps_static_map_cache', 1) || !variable_get('gmaps_static_map_cache_image', 1)) {
    return;
  }
  $dir = gmaps_get_path('smcache') .'/'. strtr($smi->cache['table'], '_', '-');
  if (!file_check_directory($dir, FILE_CREATE_DIRECTORY)) {
    return;
  }
  $formats = _gmaps_static_map_formats();
  $expires = empty($smi->cache['expires']) ? CACHE_PERMANENT : ($smi->cache['expires'] < 0 ? GMAPS_CACHE_SM_IMAGE_TEMPORARY : $smi->cache['expires']);
  $file = $dir .'/'. preg_replace(GMAPS_SM_CID_TO_FILE, '-', $smi->cid) .'-'. $expires .'.'. $formats[$smi->base->format]['ext'];
  if ($file = _gmaps_static_map_item_download_image($smi->url, $file)) {
    $smi->file_path = $file;
  }
  else {
    //when blocked by us or really broken, try to download next time.
    $smi->cacheable = FALSE;
  }
}

function _gmaps_static_map_item_download_image($url, $dest) {
  global $user;
  
  $now = time();
  $last_day = variable_get('gmaps_sm_api_last_access_day', 0);
  $last_access = variable_get('gmaps_sm_api_last_access', 0);
  $limit = 0;
  if (($now - $last_day) > 86400) {
    variable_set('gmaps_sm_api_last_access_day', $now);
    variable_set('gmaps_sm_api_daily_count', 0);
  }
  else {
    $limit = variable_get('gmaps_sm_api_daily_count', 0);
  }
  //block accessing the service too much times a day
  if ($limit >= GMAPS_SM_API_ACCESS_LIMIT) {
    return FALSE;
  }
  //block accessing the service too often
  else if (($now - $last_access) <= GMAPS_SM_API_ACCESS_DELAY) {
    return FALSE;
  }
  // hopefully we can download valid maps, not 'googlometers', now.
  else {
    variable_set('gmaps_sm_api_daily_count', $limit + 1);
    variable_set('gmaps_sm_api_last_access', $now);
    $file = NULL;
    $result = drupal_http_request($url);
    switch ($result->code) {
      case 304:
      case 301:
      case 200:
      case 302:
      case 307:
        $file = file_save_data($result->data, $dest, FILE_EXISTS_REPLACE);
        break;
      default:
        watchdog('gmaps sm api', 'The static map service seems to be broken, due to "%error". Request url: !url', array('%error' => $result->code .' '. $result->error, '!url' => $url), WATCHDOG_ERROR);
        if ($user->uid) {
          drupal_set_message(t('The static map service seems to be broken, because of error "%error".', array('%error' => $result->code .' '. $result->error)), 'error');
        }
    }
  }
  return $file;
}

function _gmaps_static_map_item_build_url(&$smi) {
  $url = 'http://maps.google.com/staticmap?key='. _gmaps_get_api_key() .
    '&size='. $smi->base->size['width'] .'x'. $smi->base->size['height'] .
    '&maptype='. $smi->base->maptype .'&format='. $smi->base->format .
    ($smi->base->frame ? '&frame=true' : '') .'&sensor=false';
    
  $settings = gmaps_get_settings('gmaps_api_settings');
  if ($settings['site_language']) {
    global $language;
    $url .= '&hl='. $language->language;
  }
  
  $markers = $paths = array();
  if (!empty($smi->overlays['path'])) {
    foreach($smi->overlays['path'] as $id => $path) {
      $style = 'rgba:0x'. $path->style['color'] . $path->style['opacity'] .',weight:'. $path->style['weight'];
      $p = '';
      foreach($path->path->points as $point) {
        $p .= '|'. gmaps_coords_format_dd_query($point);
      }
      $paths[] = $style . $p;
    }
    $url .= '&path='. implode('&path=', $paths);
  }
  if (!empty($smi->overlays['marker'])) {
    foreach($smi->overlays['marker'] as $id => $marker) {
      $style = '';
      if ($marker->style['size'] != 'normal') {
        $style .= $marker->style['size'];
      }
      if ($marker->style['color'] != 'default') {
        $style .= $marker->style['color'];
      }
      if (!empty($marker->label) || $marker->label === '0') {
        $style .= $marker->label;
      }
      $markers[] = gmaps_coords_format_dd_query($marker->point) . ($style ? ','. $style : '');
    }
    $url .= '&markers='. implode('|', $markers);
  }
  
  if (!count($paths) && count($markers) <= 1) {
    if (count($markers)) {
      $p = array_keys($smi->overlays['marker']);
      $p = $smi->overlays['marker'][$p[0]]->point;
    }
    else {
      $p = $smi->base->center;
    }
    $url .= '&center='. gmaps_coords_format_dd_query($p);
    
    if ($smi->base->zoom >= 0) {
      $url .= '&zoom='. $smi->base->zoom;
    }
    else {
      $p = isset($p->bounds) ? $p : $smi->base->center;
      $url .= '&span='. gmaps_coords_format_dd_query(gmaps_coords_bounds_span($p->bounds));
    }
  }
  else if (count($paths) == 1 && !count($markers)) {
    $p = array_keys($smi->overlays['path']);
    $p = $smi->overlays['path'][$p[0]]->path;
    
    $bounds = !empty($p->bounds) ? $p->bounds : gmaps_coords_path_bounds($p->points);
    $url .= '&center='. gmaps_coords_format_dd_query(gmaps_coords_bounds_center($bounds));
    
    if ($smi->base->zoom >= 0) {
      $url .= '&zoom='. $smi->base->zoom;
    }
    else {
      $url .= '&span='. gmaps_coords_format_dd_query(gmaps_coords_bounds_span($bounds));
    }
  }
  
  $smi->url = $url;
}

function _gmaps_static_map_item_process_ov_marker(&$smi) {
  static $first_centered = array();
  
  if (isset($smi->overlays['marker']) && !empty($smi->overlays['marker'])) {
    if ($smi->base->ov_data['marker']['route']['enabled']) {
      $route = array(
        'path' => (object)array('points' => array()),
        'style' => $smi->base->ov_data['marker']['route']['style'],
      );
    }
    foreach($smi->overlays['marker'] as $id => $marker) {
      if (isset($marker['point']) && $marker['point']->map_type == GMAPS_MAP_EARTH && !isset($first_centered[$smi->id])) {
        $first_centered[$smi->id] = TRUE;
        if ($smi->base->fix_viewport && $smi->base->ov_data['marker']['center_first']) {
          $smi->base->center = $marker['point'];
        }
      }
      
      if (!isset($marker['point']) || $marker['point']->map_type != GMAPS_MAP_EARTH || ($smi->base->fix_viewport && !gmaps_coords_bounds_contains($smi->base->center->bounds, $marker['point']))) {
        unset($smi->overlays['marker'][$id]);
        continue;
      }
      
      $marker['id'] = $id;
      $marker = _gmaps_static_marker_prepare($smi, $marker);
      
      if ($marker->point->protected) {
        unset($smi->overlays['marker'][$id]);
        $path = array(
          'path' => gmaps_coords_get_circle($marker->point, $marker->point->privacy, $marker->protected['quality']),
          'style' => $marker->protected['style'],
        );
        if (!isset($smi->overlays['path'])) {
          $smi->overlays['path'] = array();
        }
        $smi->overlays['path']['protected-marker-'. $id] = $path;
      }
      else {
        $smi->overlays['marker'][$id] = $marker;
      }
      
      //route
      if ($smi->base->ov_data['marker']['route']['enabled'] && (!isset($marker->skip_route) || !$marker->skip_route)) {
        $route['path']->points[] = $marker->point;
      }
    }
    if (count($route['path']->points) > 1) {
      $smi->overlays['path']['marker-route'] = $route;
    }
  }
}

function _gmaps_static_marker_get_default(&$smi) {
  static $default, $smi_id;
  
  if (is_null($default) || $smi->id != $smi_id) {
    $base = $smi->base->ov_data['marker'];
  
    $default = array(
      'style' => $base['style'],
      'protected' => $base['protected'],
    );
    $default['style_sticky'] = gmaps_extend($default['style'], $base['style_sticky']);
    $default['protected_sticky'] = gmaps_extend($default['protected'], $base['protected_sticky']);
  }
  
  return $default;
}

function _gmaps_static_marker_prepare(&$smi, $marker) {
  static $sizes, $colors;
  
  if (is_null($sizes)) {
    $sizes = _gmaps_static_marker_sizes();
    $colors = _gmaps_static_marker_colors();
  }
  
  $base = &$smi->base->ov_data['marker'];
  
  $default = _gmaps_static_marker_get_default($smi);
  
  $marker = (object)gmaps_extend($default, $marker);
  
  if (!isset($marker->skip_privacy) || !$marker->skip_privacy) {
    gmaps_coords_apply_privacy($marker->point);
  }
  
  if ($marker->sticky) {
    if ($marker->point->protected) {
      $marker->protected = $marker->protected_sticky;
    }
    else {
      $marker->style = $marker->style_sticky;
    }
  }
  unset($marker->style_sticky);
  
  if (!empty($marker->title)) {
    $marker->title = check_plain($marker->title);
  }
  
  if (!$smi->throttle && !$smi->se_agent && $base['labels']) {
    if ($base['labels'] == 'static' && isset($marker->label)) {
      $marker->label = strtolower(check_plain(substr($marker->label, 0, 1)));
    }
    else if ($base['labels'] == 'numeric') {
      $marker->label = _gmaps_static_marker_label_numeric($smi);
    }
    else if ($base['labels'] == 'letter') {
      $marker->label = _gmaps_static_marker_label_letter($smi);
    }
    else if ($base['labels'] == 'both') {
      $marker->label = _gmaps_static_marker_label_alphanum($smi);
    }
    if (!in_array($marker->style['size'], array('mid', 'normal'))) {
      unset($marker->label);
    }
  }
  else {
    unset($marker->label);
  }
  
  if (gmaps_overlay_has_content($marker->content)) {
    $id = 'marker-'. $marker->id;
    $content = array(
      'title' => '',
      'content' => $marker->content,
    );
    
    if ($marker->title) {
      $content['title'] = $marker->title;
    }
    else if ($marker->content['title']) {
      $content['title'] = $marker->content['title'];
    }
    $content['content']['title'] = NULL;
    
    if ($smi->base->content['icon'] && !$marker->point->protected) {
      $content['icon'] = theme('gmaps_static_map_marker_icon', $smi, $marker, $sizes[$marker->style['size']], $colors[$marker->style['color']], $marker->label ? strtoupper($marker->label) : NULL);
    }
    
    if ($marker->content['ajax']) {
      $content['ahah'] = $marker->content['ajax'];
    }
    $smi->content[$id] = $content;
  }
  
  return $marker;
}

function _gmaps_static_marker_label_numeric(&$smi) {
  static $i, $smi_id;
  
  if (is_null($i) || $i >= 10 || $smi->id != $smi_id) {
    $i = 1;
    $smi_id = $smi->id;
  }
  
  return $i++;
}

function _gmaps_static_marker_label_letter(&$smi) {
  static $i, $smi_id;
  
  if (is_null($i) || $i == 'aa' || $smi->id != $smi_id) {
    $i = 'a';
    $smi_id = $smi->id;
  }
  
  return $i++;
}

function _gmaps_static_marker_label_alphanum(&$smi) {
  static $i, $labels, $smi_id;
  
  if (is_null($labels) || $smi->id != $smi_id) {
    $i = 0;
    $labels = array_merge(array_map('chr', range(97, 122)), array_map('chr', range(48, 57)));
    $smi_id = $smi->id;
  }
  
  if ($i >= count($labels)) {
    $i = 0;
  }
  
  return $labels[$i++];
}

function _gmaps_static_map_item_process_ov_path(&$smi) {
  if (isset($smi->overlays['path']) && !empty($smi->overlays['path'])) {
    foreach($smi->overlays['path'] as $id => $path) {
      if (!isset($path['path']->points) || empty($path['path']->points)) {
        unset($smi->overlays['path'][$id]);
        continue;
      }
      if ($smi->base->fix_viewport) {
        foreach($path['path']->points as $point) {
          if (!gmaps_coords_bounds_contains($smi->base->center->bounds, $point)) {
            unset($smi->overlays['path'][$id]);
            continue 2;
          }
        }
      }
      
      $path['id'] = $id;
      $path = _gmaps_static_path_prepare($smi, $path);

      $smi->overlays['path'][$id] = $path;
    }
  }
}

function _gmaps_static_path_get_default(&$smi) {
  static $default, $smi_id;
  
  if (is_null($default) || $smi->id != $smi_id) {
    $base = $smi->base->ov_data['path'];
  
    $default = array(
      'style' => $base['style'],
    );
    $default['style_sticky'] = gmaps_extend($default['style'], $base['style_sticky']);
  }
  
  return $default;
}

function _gmaps_static_path_prepare(&$smi, $path) {
  $default = _gmaps_static_path_get_default($smi);
  
  $path = (object)gmaps_extend($default, $path);
  
  if ($path->sticky) {
    $path->style = $path->style_sticky;
  }
  unset($path->style_sticky);
  
  if (!empty($path->title)) {
    $path->title = check_plain($path->title);
  }
  
  if (gmaps_overlay_has_content($path->content)) {
    $id = 'path-'. $path->id;
    $content = array(
      'title' => '',
      'content' => $path->content,
    );
    
    if ($path->title) {
      $content['title'] = $path->title;
    }
    else if ($path->content['title']) {
      $content['title'] = $path->content['title'];
    }
    $content['content']['title'] = NULL;
    
    if ($path->content['ajax']) {
      $content['ahah'] = $path->content['ajax'];
    }
    $smi->content[$id] = $content;
  }
  
  return $path;
}

