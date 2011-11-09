<?php
// $Id: gmaps.map-pages.inc,v 1.1.2.14 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Map renderer functions.
 */

/**
 * Invoke the given operation on all page hook.
 * 
 * @param string $op
 *  'pre_process': for throttle restrictions
 *  'process': for overlay hooks
 *  'post_process: for non-overlay hooks
 *  'clean': unset unnecessary properties on the gmi object before it added to Drupal.settings
 *  
 *  All ops will be called on all hooks, the above descriptions are just suggested operations.
 *  
 * @param object $map
 * 
 * @return null
 */
function gmaps_map_page_invoke_all($op, &$gmi) {
  static $included = array();
  static $hook_maps = array();
  
  if (!isset($hook_maps[$gmi->base->type])) {
    $info = gmaps_map_get_info($gmi->base->type);
    foreach(_gmaps_map_get_hook_map() as $h => $hook_info) {
      $hook_maps[$gmi->base->type][$h] = $info->$hook_info['info'];
    }
  }
  $hook_info = $hook_maps[$gmi->base->type];

  if ($hook_info) {
    foreach($hook_info as $hook => $types) {
      foreach($types as $type) {
        if (isset($type->page_file) && !isset($included[$type->page_file])) {
          module_load_include('inc', $type->module, $type->page_file);
          $included[$type->page_file] = TRUE;
        }
      
        $function = $type->module .'_gmaps_map_page_'. $hook .'_'. $type->type;
        if (function_exists($function)) {
          $function($op, $gmi);
        }
      }
    }
  }
}

function _gmaps_map_item_render($base, $callback = NULL, $args = array(), $cache = array(), $id = NULL) {
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
    $base_clone = gmaps_map_load($base);
    if (!$base_clone) {
      //return this message in place of the map to let the user know which map is broken,
      //when multiple maps exists on the same page.
      drupal_set_message(t('Invalid map ID.'), 'error gmaps-map-error');
      return theme('status_messages', 'error gmaps-map-error');
    }
  }
  
  //set element id
  $gmi_id = 'gmi-'. (empty($id) ? ($callback ? $callback : 'gmaps') : $id);
  $gmi_id = _gmaps_map_item_clean_id($gmi_id);
  
  if ($user->uid != 1 && variable_get('gmaps_map_cache', 1)) {
    if (empty($theme)) {
      init_theme();
    }
    
    //set default cache table and user mask if not set
    $cache += array('table' => GMAPS_CACHE_TABLE_MAP, 'user_mask' => GMAPS_CACHE_PER_ROLE);
    
    //create cache id
    $cid_parts = array(empty($cache['cid']) ? md5($gmi_id . (empty($args) ? '' : '-'. serialize($args))) : $cache['cid']);
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
    
    $gmi_cid = implode(':', $cid_parts);
  }
  
  $se_agent = gmaps_agent_is_search_engine();
  
  //track se agent on all maps
  if ($se_agent && variable_get('gmaps_se_track_access', 1)) {
    watchdog('gmaps se access', 'The robot %robot has been accessed the map %map', array('%robot' => $_SERVER['HTTP_USER_AGENT'], '%map' => $base_clone->name .($callback ? ' ('. $callback .')' : '')), WATCHDOG_INFO);
  }

  if ($se_agent && $base_clone->map_data['se_support']) {
    $current_throttle = 0;
  }
  //skip one or two db_query if possible
  else if ($user->uid != 1 && variable_get('gmaps_map_cache', 1)) {
    if (!empty($cache['cid']) && ($cached = cache_get($gmi_cid, $cache['table']))) {
      if ($cached->expire == CACHE_PERMANENT || $cached->expire > time()) {
        $gmi = $cached->data;
        $gmi->cached = TRUE;
      }
    }
    if (!isset($gmi) && $current_throttle && $base_clone->throttle_support && ($cached = cache_get($gmi_cid .':_throttle_', $cache['table']))) {
      if ($cached->expire == CACHE_PERMANENT || $cached->expire > time()) {
        $gmi = $cached->data;
        $gmi->cached = TRUE;
      }
    }
  }
  
  if (!isset($gmi)) {
    $gmi = (object)array(
      'id' => $gmi_id,
      'base' => $base_clone,
      'throttle' => $base_clone->throttle_support && $current_throttle,
      'embedded' => FALSE,
      'cached' => FALSE,
      'cachable' => FALSE,
      'cache' => $gmi_cid ? $cache : array(),
      'se_agent' => ($se_agent && $base_clone->map_data['se_support']),
      'icons' => array(),
      'info_windows' => array(),
      'js_files' => array(
        'api' => array('api' => 1),
      ),
      'overlays' => array(),
      'content' => array(),
      'after_process' => array(),
      'attributes' => array(),
      'headers' => array(),
      'view' => '',
    );
    if ($gmi_cid) {
      $gmi->cid = $gmi_cid .($gmi->throttle ? ':_throttle_' : '');
    }
    
    //not admin user, not se agent, global switch on
    if ($gmi->cid && $user->uid != 1 && !$se_agent && variable_get('gmaps_map_cache', 1)) {
      //throttle cache
      if ($gmi->throttle) {
        if ($gmi->base->throttle_lifetime) {
          $gmi->cachable = TRUE;
        }
      }
      //regular cache
      else if (!empty($cache['cid']) && isset($cache['expires'])) {
        $gmi->cachable = TRUE;
      }
    }
    _gmaps_map_item_process($gmi, $callback, $args);
  }
  
  if ($gmi) {
    if (!$se_agent) {
      _gmaps_map_add_js($gmi);
    }

    if (!empty($gmi->after_process)) {
      foreach($gmi->after_process as $function) {
        if (function_exists($function)) {
          $function($gmi);
        }
      }
    }
    
    if ($gmi->cached) {
      if (!$se_agent) {
        gmaps_restore_headers($gmi->headers);
      }
    }
    else {
      //never cache se visits, because they visit 'everything',
      //which, in turn, can result in killer large cache tables.
      if (!$se_agent && $gmi->cid && $gmi->cachable && variable_get('gmaps_map_cache', 1)) {
        $gmi->headers = gmaps_gather_headers();
      }
      
      $gmi->view = theme('gmaps_map_item', $gmi);
      
      if (!$se_agent && $gmi->cid && $gmi->cachable && variable_get('gmaps_map_cache', 1)) {
        $gmi->headers = gmaps_gather_headers($gmi->headers);
        
        cache_set($gmi->cid, $gmi, $cache['table'], $gmi->throttle ? (time() + $gmi->base->throttle_lifetime) : $cache['expires']);
      }
    }
    
    return $gmi->view;
  }
  
  return '';
}

function _gmaps_map_item_clean_id($id) {
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
function _gmaps_map_item_process(&$gmi, $callback, $args) {
  if ($gmi->throttle || $gmi->se_agent) {
    if ($gmi->se_agent) {
      $gmi->base->map_data['toc_map']['enabled'] = 1;
      $gmi->base->map_data['toc_map']['style'] = 'box';
      $gmi->base->map_data['toc_map']['content']['style'] = 'box';
    }
    if ($gmi->throttle && $gmi->base->map_data['toc_map']['style'] == 'fieldset') {
      $gmi->base->map_data['toc_map']['style'] = 'box';
    }
    //disable progressbar, set street view icon to static
    $gmi->base->map_data['progressbar']['enabled'] = 0;
    if (isset($gmi->base->map_data['svu']) && $gmi->base->map_data['svu']['enabled']) {
      if ($gmi->se_agent) {
        $gmi->base->map_data['svu']['enabled'] = 0;
      }
      else {
        $gmi->base->map_data['svu']['icon']['type'] = GMAPS_MAP_SVU_ICON_TYPE_STATIC;
      }
    }
    
    $info = gmaps_map_get_info($gmi->base->type);
    foreach($info->control_types as $ct) {
      if ($control = $gmi->base->controls[$ct->type]['control']) {
        if ($gmi->se_agent) {
          $gmi->base->controls[$ct->type]['control'] = '';
        }
        else {
          $control = $info->controls[$control];
          if ($control->api_version === FALSE) {
            $gmi->base->controls[$ct->type]['control'] = '';
          }
          else if ($control->api_version !== TRUE) {
            $gmi->base->controls[$ct->type]['control'] = $control->api_version;
          }
        }
      }
    }
  }
  //mainly to set throttle restrictions
  gmaps_map_page_invoke_all('pre_process', $gmi);
  

  if ($callback) {
    array_unshift($args, &$gmi);
    call_user_func_array($callback, $args);
  }
  //mainly for overlays to process content
  gmaps_map_page_invoke_all('process', $gmi);
  
  if (!empty($gmi->info_windows) && !isset($gmi->base->methods['infowindow'])) {
    $gmi->base->methods['infowindow'] = 'infowindow';
  }
  
  if ($gmi->base->map_data['progressbar']['enabled']) {
    $gmi->js_files['api']['progressbar'] = 1;
    $gmi->base->map_data['progressbar']['loadstring'] = t($gmi->base->map_data['progressbar']['loadstring']);
  }

  if ($gmi->base->map_data['toc_map']['enabled'] && !empty($gmi->base->map_data['toc_map']['ahah']['message'])) {
    $gmi->base->map_data['toc_map']['ahah']['message'] = t($gmi->base->map_data['toc_map']['ahah']['message']);
  }

  //register throbber icon
  $gmi->icons[$gmi->base->map_data['throbber']] = $gmi->base->map_data['throbber'];

  if (isset($gmi->base->map_data['svu']) && $gmi->base->map_data['svu']['enabled']) {
    //register svu icons
    if ($gmi->base->map_data['svu']['icon']['type'] == GMAPS_MAP_SVU_ICON_TYPE_STATIC) {
      $gmi->icons[$gmi->base->map_data['svu']['icon']['n']] = $gmi->base->map_data['svu']['icon']['n'];
    }
    else {
      foreach($gmi->base->map_data['svu']['icon'] as $key => $value) {
        if ($key != 'type') {
          $gmi->icons[$value] = $value;
        }
      }
    }
    if ($gmi->base->map_data['svu']['track']) {
      $gmi->js_files['ov_marker']['markertracker'] = 1;
    }
    if ($gmi->base->map_data['svu']['blowup']) {
      $gmi->info_windows['google'] = 1;
    }
  }
  //mainly for non-overlay hooks to add themselves to the map or translate strings, etc.
  gmaps_map_page_invoke_all('post_process', $gmi);
  
  drupal_alter('gmaps_map_item', $gmi);
}

/**
 * Remove unnecessary fields.
 * 
 * @param $gmi
 * @return null
 */
function _gmaps_map_item_clean(&$gmi) {
  //item clean-ups
  unset($gmi->content, $gmi->js_files, $gmi->after_process, $gmi->attributes, $gmi->icons, $gmi->throttle, $gmi->embedded, $gmi->se_agent, $gmi->toc_ahah);
  unset($gmi->cached, $gmi->cachable, $gmi->cache, $gmi->headers, $gmi->view);
  
  if (empty($gmi->icons)) {
    unset($gmi->icons);
  }
  //disable infowindow method if no info windows registered
  if (empty($gmi->info_windows)) {
    if ($index = array_search('infowindow', $gmi->base->methods)) {
      unset($gmi->base->methods[$index]);
    }
    unset($gmi->base->iw_data, $gmi->info_windows);
  }
  if (empty($gmi->overlays)) {
    unset($gmi->base->ov_data, $gmi->overlays);
  }
  
  //base clean-ups
  $base = &$gmi->base;
  
  unset($base->throttle);
  if (!$base->map_data['progressbar']['enabled']) {
    unset($base->map_data['progressbar']);
  }
  if (!$base->map_data['toc_map']['enabled']) {
    unset($base->map_data['toc_map']);
  }
  if (!$base->map_data['svu']['enabled']) {
    unset($base->map_data['svu']);
  }
  
  gmaps_map_page_invoke_all('clean', $gmi);
  
  $info = gmaps_map_get_info($base->type);
  //methods
  if (empty($base->methods)) {
    unset($base->method_data);
  }
  else {
    foreach($info->methods as $method) {
      if (!in_array($method->type, $base->methods)) {
        unset($base->method_data[$method->type]);
      }
    }
    if (empty($base->method_data)) {
      unset($base->method_data);
    }
  }

  //infow windows, only when not removed completely
  if (isset($gmi->info_windows)) {
    foreach($info->info_windows as $iw) {
      if (!isset($gmi->info_windows[$iw->type])) {
        unset($base->iw_data[$iw->type]);
      }
    }
    if (empty($base->iw_data)) {
      unset($base->iw_data);
    }
  }
  
  //controls
  foreach($base->controls as $ct => $ct_data) {
    foreach($info->control_types[$ct]->controls as $control) {
      if ($control != $ct_data['control']) {
        unset($base->control_data[$control]);
      }
    }
    if (empty($ct_data['control'])) {
      unset($base->controls[$ct]);
    }
  }
  if (empty($base->control_data)) {
    unset($base->control_data);
  }
  if (empty($base->controls)) {
    unset($base->controls);
  }
  
  //overlays, if any
  if (isset($gmi->overlays)) {
    foreach($info->overlays as $ov) {
      if (!isset($gmi->overlays[$ov->type])) {
        unset($base->ov_data[$ov->type]);
      }
    }
    if (empty($base->ov_data)) {
      unset($base->ov_data);
    }
  }
  unset($base->default_id, $base->modified, $base->locked, $base->name, $base->throttle_lifetime, $base->throttle_support,
    $base->map_data['all_files'], $base->map_data['se_support']);
}

function _gmaps_map_add_js($gmi) {
  global $gmaps_map_all_js_added;
  static $added_files = array();
  static $added_maps = array();
  
  if ($gmi->base->map_data['all_files']) {
    gmaps_map_add_all_js();
  }
  else if (!$gmaps_map_all_js_added) {
    _gmaps_add_maps_api();
    $info = gmaps_map_get_js_info();
    $new_files = array();
    foreach($gmi->js_files as $hook => $files) {
      foreach($files as $file => $value) {
        $hook_key = $hook .'_'. $file;
        if (!isset($added_files[$hook_key]) && !empty($info[$hook][$file])) {
          $new_files[] = $info[$hook][$file];
          $added_files[$hook_key] = 1;
        }
      }
    }
    if (!empty($new_files)) {
      gmaps_add_js($new_files);
    }
  }
  
  if (!isset($added_maps[$gmi->id])) {
    $clone = gmaps_extend(new stdCLass(), $gmi);
    //clean-up non-used settings
    _gmaps_map_item_clean($clone);
    drupal_add_js(array(
      'gmaps' => array(
        'map' => array(
          $gmi->id => $clone,
        ),
      ),
    ), 'setting');
    $added_maps[$gmi->id] = TRUE;
  }
  
  if (!empty($gmi->icons)) {
    _gmaps_icon_add_icons($gmi->icons);
  }
}

function _gmaps_icon_add_icons($icons) {
  static $added_icons = array();
  
  $new_types = array();
  foreach($icons as $icon) {
    $icid = is_object($icon) ? $icon->icid : $icon;
    if (!isset($added_icons[$icid])) {
      $icon = is_object($icon) ? $icon : gmaps_icon_load($icid);
      if ($icon) {
        $new_types[$icon->type] = 1;
        $added_icons[$icid] = 1;
        foreach(_gmaps_icon_get_file_fields() as $key) {
          if (!empty($icon->$key)) {
            $icon->$key = file_create_url($icon->$key);
          }
        }
        drupal_add_js(array(
          'gmaps' => array(
            'icon' => array(
              'icon_'. $icid => $icon,
            ),
          ),
        ), 'setting');
      }
    }
  }
  
  if (!empty($new_types)) {
    _gmaps_icon_add_js(array_keys($new_types));
  }
}

function _gmaps_icon_add_js($types) {
  static $added_types = array();
  
  $info = gmaps_icon_get_js_info();
  $new_files = array();
  
  if (!isset($added_files['api'])) {
    $new_files = $info['api'];
    $added_files['api'] = 1;
  }
  
  foreach($types as $type) {
    if (!isset($added_types[$type]) && isset($info[$type])) {
      $new_files = array_merge($new_files, $info[$type]);
      $added_types[$type] = 1;
    }
  }
  
  if (!empty($new_files)) {
    gmaps_add_js($new_files);
  }
}




/************************
 * Hook implementations *
 ************************/

function gmaps_gmaps_map_page_method_layers($op, &$gmi) {
  global $language;
  if ($op == 'process') {
    if (isset($gmi->base->methods['layers'])) {
      $ld = &$gmi->base->method_data['layers'];
      $layers = array();
      if ($ld['panoramio']) {
        $layers['panoramio'] = 'com.panoramio.all';
      }
      if ($ld['youtube']) {
        $layers['youtube'] = 'com.youtube.all';
      }
      if ($ld['webcams']) {
        $layers['webcams'] = 'com.google.webcams';
      }
      if ($wp = $ld['wikipedia']) {
        $wp_languages = _gmaps_get_wikipedia_layer_languages();
        if ($wp == 1 && isset($wp_languages[$language->language])) {
          $layers['wikipedia_'. $language->language] = 'org.wikipedia.'. $language->language;
        }
        else {
          foreach(language_list() as $code -> $info) {
            if (isset($wp_languages[$code])) {
              $layers['wikipedia_'. $code] = 'org.wikipedia.'. $code;
            }
          }
        }
        if ($ld['wikipedia_en'] && !isset($layers['wikipedia_en'])) {
          $layers['wikipedia_en'] = 'org.wikipedia.en';
        }
      }
      if (!empty($layers)) {
        $ld['layers'] = $layers;
        $gmi->info_windows['google'] = 1;
      }
    }
  }
}

function gmaps_gmaps_map_page_method_traffic($op, &$gmi) {
  if ($op == 'process') {
    if (isset($gmi->base->methods['traffic'])) {
      $gmi->info_windows['google'] = 1;
    }
  }
}

function gmaps_gmaps_map_page_method_keydragzoom($op, &$gmi) {
  if ($op == 'post_process') {
    if (isset($gmi->base->methods['keydragzoom'])) {
      $gmi->js_files['method_keydragzoom']['keydragzoom'] = 1;
    }
  }
}

function gmaps_gmaps_map_page_iw_bb($op, &$gmi) {
  if ($op == 'post_process'&& isset($gmi->info_windows['bb'])) {
    $gmi->after_process[] = '_gmaps_map_iw_after_process';
  }
}

function gmaps_gmaps_map_page_iw_extended($op, &$gmi) {
  if ($op == 'post_process'&& isset($gmi->info_windows['extended'])) {
    $gmi->js_files['iw_extended']['extinfowindow'] = 1;
    $gmi->after_process[] = '_gmaps_map_iw_after_process';
  }
}

function gmaps_gmaps_map_page_iw_tmc($op, &$gmi) {
  if ($op == 'post_process'&& isset($gmi->info_windows['tmc'])) {
    $gmi->js_files['iw_tmc']['tabbedmaxcontent'] = 1;
  }
}

function gmaps_gmaps_map_page_control_extmaptype($op, &$gmi) {
  if ($op == 'post_process') {
    if ($gmi->base->controls['maptype']['control'] == 'extmaptype') {
      $gmi->js_files['control_extmaptype']['extmaptypecontrol'] = 1;
      
      if ($gmi->base->control_data['extmaptype']['maptype'] != 'extmaptype') {
        $control_data = $gmi->base->control_data[$gmi->base->control_data['extmaptype']['maptype']];
        if (!empty($control_data)) {
          $gmi->base->control_data['extmaptype']['control_data'] = $control_data;
        }
      }
    }
  }
}

function gmaps_gmaps_map_page_control_extnav($op, &$gmi) {
  if ($op == 'post_process') {
    if ($gmi->base->controls['navigation']['control'] == 'extnav') {
      $title = &$gmi->base->control_data['extnav']['title'];
      $title['east'] = t($title['east']);
      $title['west'] = t($title['west']);
      $title['north'] = t($title['north']);
      $title['south'] = t($title['south']);
      $title['return'] = t($title['return']);
      $title['zoomin'] = t($title['zoomin']);
      $title['zoomout'] = t($title['zoomout']);
      
      $gmi->js_files['control_extnav']['extlargemapcontrol'] = 1;
    }
  }
}

function gmaps_gmaps_map_page_control_extdragzoom($op, &$gmi) {
  if ($op == 'post_process') {
    if ($gmi->base->controls['dragzoom']['control'] == 'extdragzoom') {
      $other = &$gmi->base->control_data['extdragzoom']['other'];
      list($other['button_html'], $other['zooming_html'], $other['back_html']) = preg_replace_callback(
        '|\{(.*?)\}|',
        create_function('$matches', 'return empty($matches[1]) ? "" : t($matches[1]);'),
        array($other['button_html'], $other['zooming_html'], $other['back_html'])
      );
      
      $gmi->js_files['control_extdragzoom']['extdragzoom'] = 1;
    }
  }
}

function gmaps_gmaps_map_page_ov_marker($op, &$gmi) {
  static $first_centered = array();
  
  if ($op == 'pre_process') {
    $marker = &$gmi->base->ov_data['marker'];
    if ($gmi->throttle || $gmi->se_agent) {
      $marker['basic']['icon'] = 0;
      $marker['basic']['protected_icon'] = 0;
      $marker['misc']['labels'] = '';
      $marker['labeled']['enabled'] = 0;
      $marker['protected_labeled']['enabled'] = 0;
      $marker['route']['enabled'] = 0;
      $marker['manager']['manager'] = '';
      $marker['tracker']['enabled'] = 0;
      if ($gmi->se_agent) {
        $marker['misc']['ajaxcontent'] = 0;
      }
    }
    if ($gmi->base->map_data['toc_map']['enabled'] && !in_array($gmi->base->map_data['toc_map']['style'], array('accordion', 'tabs'))) {
      $marker['misc']['ajaxcontent'] = 0;
    }
  }
  else if ($op == 'process' && isset($gmi->overlays['marker']) && !empty($gmi->overlays['marker'])) {
    foreach($gmi->overlays['marker'] as $id => $marker) {
      if (!isset($marker['point']) || $marker['point']->map_type != $gmi->base->type) {
        unset($gmi->overlays['marker'][$id]);
        continue;
      }
      $gmi->js_files['ov_marker']['api'] = 1;
      
      $marker['id'] = $id;
      $marker = _gmaps_marker_prepare($gmi, $marker);
      
      $gmi->overlays['marker'][$id] = $marker;
      
      if (!isset($first_centered[$gmi->id])) {
        $first_centered[$gmi->id] = TRUE;
        if ($gmi->base->ov_data['marker']['misc']['center_first']) {
          $gmi->base->center = $marker->point;
        }
      }
    }
    if ($gmi->base->ov_data['marker']['manager']['manager'] == 'clusterer') {
      foreach($gmi->base->ov_data['marker']['manager']['clusterer']['styles'] as $style) {
        if (!empty($style['icon'])) {
          $gmi->icons[$style['icon']] = $style['icon'];
        }
      }
    }
  }
  else if ($op == 'clean') {
    if (isset($gmi->overlays['marker'])) {
      if (empty($gmi->overlays['marker'])) {
        unset($gmi->overlays['marker'], $gmi->base->ov_data['marker']);
      }
      else {
        foreach($gmi->overlays['marker'] as $id => $marker) {
          unset($marker->basic['protected_icon'], $marker->protected_labeled, $marker->sticky, $marker->skip_privacy);
          
          if (empty($marker->title)) {
            unset($marker->title);
          }
          
          if (!$marker->labeled['enabled']) {
            unset($marker->labeled);
          }
          
          if (empty($marker->content['title'])) {
            unset($marker->content['title']);
          }
          
          if (!in_array('content', $marker->action)) {
            unset($marker->content, $marker->basic['info_window']);
          }
          else if (!$marker->basic['info_window'] || !gmaps_overlay_has_content($marker->content)) {
            unset($marker->content);
            foreach($marker->action as $e => $handler) {
              if ($handler == 'content') {
                unset($marker->action[$e]);
              }
            }
            unset($marker->basic['info_window']);
          }
          else {
            foreach($marker->content as $part => $value) {
              if (empty($value)) {
                unset($marker->content[$part]);
              }
            }
          }
  
          $has_blowup = in_array('blowup', $marker->action);
    
          if (!$marker->basic['info_window'] && !$has_blowup) {
            unset($marker->iw_data);
          }
          else if (isset($marker->iw_data)) {
            foreach($marker->iw_data as $iw => $iw_info) {
              if ($iw != $marker->basic['info_window'] && ($iw != 'google' || !$has_blowup)) {
                unset($marker->iw_data[$iw]);
              }
            }
            if (empty($marker->iw_data)) {
              unset($marker->iw_data);
            }
          }
          
          foreach($marker->action as $e => $handler) {
            if (empty($handler)) {
              unset($marker->action[$e]);
            }
          }
          if (empty($marker->action)) {
            unset($marker->action);
          }
          
          if (empty($marker->tracker['enabled'])) {
            unset($marker->tracker);
          }
  
          if (empty($marker->link)) {
            unset($marker->link, $marker->link_target);
          }
        }
      }
    }
    if (isset($gmi->base->ov_data['marker'])) {
      $misc = $gmi->base->ov_data['marker']['misc'];
      $gmi->base->ov_data['marker'] = array(
        'route' => $gmi->base->ov_data['marker']['route'],
        'manager' => $gmi->base->ov_data['marker']['manager'],
      );
      if ($misc['zoom_first']) {
        $gmi->base->ov_data['marker']['misc']['zoom_first'] = 1;
      }
      if (empty($gmi->base->ov_data['marker']['route']['enabled'])) {
        unset($gmi->base->ov_data['marker']['route']);
      }
      if (empty($gmi->base->ov_data['marker']['manager']['manager'])) {
        unset($gmi->base->ov_data['marker']['manager']);
      }
      else if ($gmi->base->ov_data['marker']['manager']['manager'] == 'clusterer') {
        unset($gmi->base->ov_data['marker']['manager']['mm']);
      }
      else {
        unset($gmi->base->ov_data['marker']['manager']['clusterer']);
      }
      if (empty($gmi->base->ov_data['marker'])) {
        unset($gmi->base->ov_data['marker']);
      }
    }
  }
}

function _gmaps_map_iw_after_process(&$gmi) {
  drupal_add_js('misc/collapse.js');
  jquery_ui_add('ui.accordion');
  jquery_ui_add('ui.tabs');
  jquery_ui_add('effects.core');
}

function _gmaps_marker_get_default(&$gmi) {
  static $default, $gmi_id;
  
  if (is_null($default) || $gmi->id != $gmi_id) {
    $base = $gmi->base->ov_data['marker'];
  
    $default = array(
      'basic' => $base['basic'],
      'link_target' => $base['misc']['link_target'],
      'options' => $base['options'],
      'labeled' => $base['labeled'],
      'protected_labeled' => $base['protected_labeled'],
      'action' => $base['action'],
      'tracker' => $base['tracker'],
    );
  }
  
  return $default;
}

function _gmaps_marker_prepare(&$gmi, $marker) {
  $base = &$gmi->base->ov_data['marker'];
  
  $default = _gmaps_marker_get_default($gmi);
  
  $marker = (object)gmaps_extend($default, $marker);
  
  if (!isset($marker->skip_privacy) || !$marker->skip_privacy) {
    gmaps_coords_apply_privacy($marker->point);
  }
  
  if ($marker->point->protected) {
    $marker->basic['icon'] = ($marker->basic['protected_icon'] == GMAPS_ICON_MAP_DEFAULT) ? $base['basic']['protected_icon'] : $marker->basic['protected_icon'];
    $marker->labeled = $marker->protected_labeled;
  }
  else if ($marker->basic['icon'] == GMAPS_ICON_MAP_DEFAULT) {
    $marker->basic['icon'] = $base['basic']['icon'];
  }
  
  if (!$gmi->throttle && !$gmi->se_agent && $base['misc']['labels']) {
    if ($base['misc']['labels'] == 'static') {
      $label = $marker->label ? check_plain($marker->label) : '';
    }
    else if ($base['misc']['labels'] == 'numeric') {
      $label = _gmaps_marker_label_numeric($gmi);
    }
    else if ($base['misc']['labels'] == 'capital') {
      $label = _gmaps_marker_label_letter($gmi, TRUE);
    }
    else if ($base['misc']['labels'] == 'letter') {
      $label = _gmaps_marker_label_letter($gmi);
    }
    else if ($base['misc']['labels'] == 'word') {
      $label = _gmaps_marker_label_ascii($gmi, FALSE);
    }
    else if ($base['misc']['labels'] == 'ascii') {
      $label = _gmaps_marker_label_ascii($gmi);
    }
    else if ($base['misc']['labels'] == 'custom') {
      $label = _gmaps_marker_label_custom($gmi);
    }
  }
  
  if ($marker->basic['icon']) {
    if ($gmi->throttle || $gmi->se_agent) {
      $gmi->icons[$marker->basic['icon']] = $marker->basic['icon'];
      $marker->labeled['enabled'] = 0;
    }
    else if ($gmi->se_agent) {
      $marker->basic['icon'] = 0;
    }
    else {
      $icon = gmaps_icon_create_icon($marker->basic['icon'], $marker->sticky, $label);
      if ($icon) {
        $marker->basic['icon'] = $icon->icid;
        $gmi->icons[$icon->icid] = $icon ;
        if ($label && $icon->label != $label && $marker->labeled['enabled']) {
          $marker->labeled['label'] = $label;
          $gmi->js_files['ov_marker']['labeledmarker'] = 1;
        }
        else {
          $marker->labeled['enabled'] = 0;
        }
      }
      else {
        $marker->basic['icon'] = 0;
      }
    }
  }
  else if ($label && $marker->labeled['enabled']) {
    $marker->labeled['label'] = $label;
    $gmi->js_files['ov_marker']['labeledmarker'] = 1;
  }
  
  if ($gmi->base->map_data['toc_map']['enabled']) {
    foreach($marker->action as $e => $handler) {
      if ($handler == 'content' || $handler == 'link') {
        $marker->action[$e] = NULL;
      }
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
      else if ($label && $marker->labeled['enabled']) {
        $content['title'] = $label;
      }
      if (!empty($marker->basic['icon']) && !$marker->labeled['enabled']) {
        $content['icon'] = $marker->basic['icon'];
      }
      $content['content']['title'] = NULL;
      
      if ($marker->content['ajax']) {
        $content['ahah'] = $marker->content['ajax'];
      }
      $gmi->content[$id] = $content;
      
      $marker->action['click'] = 'link';
      $marker->link = '#'. $gmi->id .'-'. $id;
    }
    $marker->content = array();
  }
  else if (!empty($marker->link)) {
    $marker->link = check_url($marker->link);
  }
  
  if (in_array('content', $marker->action) && gmaps_overlay_has_content($marker->content) && $marker->basic['info_window']) {
    $gmi->info_windows[$marker->basic['info_window']] = 1;
    if (!empty($marker->content['ajax'])) {
      $marker->content['ajax']['url'] = check_url(url($marker->content['ajax']['path']));
      unset($marker->content['ajax']['path']);
    }
  }
  
  if (!isset($marker->point->pov) || !$marker->point->pov['has_svu'] || !$marker->point->pov['enabled']) {
    foreach ($marker->action as $e => $handler) {
      if ($handler == 'svu') {
        $marker->action[$e] = '';
      }
    }
  }
  
  if (in_array('blowup', $marker->action)) {
    $gmi->info_windows['google'] = 1;
  }
  
  if (!isset($marker->options['clickable']) || $marker->options['clickable'] < 0) {
    $marker->options['clickable'] = (empty($marker->action) && !$marker->options['draggable']) ? 0 : 1;
  }
  
  if (!empty($marker->tracker['enabled'])) {
    $gmi->js_files['ov_marker']['markertracker'] = 1;
  }

  if (empty($marker->manager['skip_manager'])) {
    if ($base['manager']['manager'] == 'clusterer') {
      $gmi->js_files['ov_marker']['markerclusterer'] = 1;
    }
    else if ($base['manager']['manager'] == 'extended') {
      $gmi->js_files['ov_marker']['extmarkermanager'] = 1;
    }
  }
  
  return $marker;
}

function _gmaps_marker_label_numeric(&$gmi) {
  static $i, $gmi_id;
  
  if (is_null($i) || $gmi->id != $gmi_id) {
    $i = 1;
    $gmi_id = $gmi->id;
  }
  
  return $i++;
}

function _gmaps_marker_label_letter(&$gmi, $capitals = FALSE) {
  static $i, $gmi_id;
  
  if (is_null($i) || $gmi->id != $gmi_id) {
    $i = $capitals ? 'A' : 'a';
    $gmi_id = $gmi->id;
  }
  
  return $i++;
}

function _gmaps_marker_label_ascii(&$gmi, $full = TRUE) {
  static $i, $labels, $gmi_id;
  
  if (is_null($labels) || $gmi->id != $gmi_id) {
    $i = 0;
    $labels = array_merge(array_map('chr', range(65, 90)), array_map('chr', range(97, 122)), array_map('chr', range(48, 57)));
    if ($full) {
     $labels = array_merge($labels, array_diff(array_map('chr', range(33, 126)), $labels));
    }
    $gmi_id = $gmi->id;
  }
  
  if ($i >= count($labels)) {
    $i = 0;
  }
  
  return $labels[$i++];
}

function _gmaps_marker_label_custom(&$gmi) {
  static $i, $labels, $gmi_id;
  
  if (is_null($labels) || $gmi->id != $gmi_id) {
    $i = 0;
    $labels = array_map('check_plain', explode('|', $gmi->base->ov_data['marker']['misc']['custom_labels']));
    $gmi_id = $gmi->id;
  }
  
  if ($i >= count($labels)) {
    $i = 0;
  }
  
  return $labels[$i++];
}
