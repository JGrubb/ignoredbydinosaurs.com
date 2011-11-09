<?php
// $Id: gmaps_taxonomy.cron.inc,v 1.1.2.4 2009/11/23 13:23:48 xmarket Exp $

/**
 * @file
 * Location navigation menu and bulk generate related functions.
 */

function _gmaps_taxonomy_menu_rebuild() {
  $s = gmaps_taxonomy_get_settings('gmaps_taxonomy_menu');
  $cont_level = _gmaps_taxonomy_menu_level_included('continent');
  $country_level = _gmaps_taxonomy_menu_level_included('country');
  
  $countries = gmaps_get_countries();
  
  $codes = $conts = array();
  if (!$cont_level) {
    $codes[0] = array();
  }
  $sql = 'SELECT DISTINCT(gca.country) FROM {gmaps_content_address} gca INNER JOIN {node} n ON gca.vid = n.vid WHERE n.status = 1';
  if ($s['countries']) {
    $sql .= ' AND country IN ('. db_placeholders($s['countries'], 'char') .')';
  }
  $r = db_query($sql, $s['countries']);
  while ($c = db_fetch_object($r)) {
    if ($cont_level) {
      if (!isset($conts[$countries[$c->country]['continent']])) {
        $conts[$countries[$c->country]['continent']] = array();
      }
      $conts[$countries[$c->country]['continent']][] = $c->country;
    }
    else {
      $codes[0][] = $c->country;
    }
  }
  
  if (!$conts && !$codes[0]) {
    db_query("DELETE FROM {menu_links} WHERE menu_name = '%s'", GMAPS_TAXONOMY_MENU_NAME);
    cache_clear_all('%:'. GMAPS_TAXONOMY_MENU_NAME .':', 'cache_menu', TRUE);
    return;
  }
  
  if ($cont_level) {
    $continents = geonames_continent_codes('short');
    $existing = _gmaps_taxonomy_menu_get_children();
  
    foreach ($conts as $cont => $cont_codes) {
      $mlid = _gmaps_taxonomy_menu_link_save('continent:'. $cont, $continents[$cont]['name'], $existing['continent:'. $cont]);
      if ($mlid) {
        unset($existing['continent:'. $cont]);
        $codes[$mlid] = $cont_codes;
      }
    }
    
    foreach($existing as $item) {
      _gmaps_taxonomy_menu_delete_tree($item);
    }
    
    if (!$codes) {
      return;
    }
  }
  
  foreach($codes as $plid => $cont_codes) {
    $existing = _gmaps_taxonomy_menu_get_children($plid);
    
    foreach($cont_codes as $c) {
      $path = $c;
      
      $current_plid = $plid;
      if ($country_level) {
        $mlid = _gmaps_taxonomy_menu_link_save($c, $countries[$c]['name'], $existing[$c], $plid);
        if ($mlid) {
          unset($existing[$c]);
          $current_plid = $mlid;
        }
      }
      
      _gmaps_taxonomy_menu_build_tree(1, "UPPER(gca.country) = UPPER('%s')", array($c), $path, $current_plid);
    }
    
    foreach($existing as $item) {
      _gmaps_taxonomy_menu_delete_tree($item);
    }
  }
  
  variable_set('gmaps_taxonomy_menu_last_rebuild', time());
  cache_clear_all('%:'. GMAPS_TAXONOMY_MENU_NAME .':', 'cache_menu', TRUE);
}

function _gmaps_taxonomy_menu_level_included($level) {
  static $levels;
  
  if (is_null($levels)) {
    $s = gmaps_taxonomy_get_settings('gmaps_taxonomy_menu');
    $levels = $s['levels'];
  }
  
  $include = in_array($level, $levels);
  if ($level != 'thoroughfare') {
    $include = $include || empty($levels);
  }
  
  return $include;
}

function _gmaps_taxonomy_menu_get_children($plid = 0) {
  $items = array();
  $r = db_query("SELECT * FROM {menu_links} WHERE menu_name = '%s' AND plid = %d", GMAPS_TAXONOMY_MENU_NAME, $plid);
  while ($item = db_fetch_array($r)) {
    $items[array_pop(explode('taxonomy/location/', $item['link_path'], 2))] = $item;
  }
  return $items;
}

function _gmaps_taxonomy_menu_link_save($path, $title, $original_item = NULL, $plid = 0, $translatable = TRUE) {
  $item = array(
    'link_path' => 'taxonomy/location/'. $path,
    'link_title' => $title,
    'router_path' => 'taxonomy/location',
    'menu_name' => GMAPS_TAXONOMY_MENU_NAME,
    'module' => 'gmaps_taxonomy',
    'plid' => $plid,
  );
  if ($original_item) {
    $item += array(
      'mlid' => $original_item['mlid'],
      'options' => unserialize($original_item['options']),
      'hidden' => $original_item['hidden'],
      'expanded' => $original_item['expanded'],
      'weight' => $original_item['weight'],
      'customized' => $original_item['customized'],
      'updated' => $original_item['updated'],
    );
    
    if ($item['customized']) {
      $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_menu');
      if ($settings['set_customized']) {
        $item['customized'] = 0;
      }
    }
  }
  
  if (!$item['customized'] && $translatable) {
    if (!isset($item['options'])) {
      $item['options'] = array();
    }
    $item['options']['alter'] = TRUE;
  }
  
  return menu_link_save($item);
}

function _gmaps_taxonomy_menu_delete_tree($item, $direct = FALSE) {
  $r = db_query("SELECT DISTINCT(ml.mlid), ml.* FROM {menu_links} ml WHERE ml.plid = %d AND ml.menu_name = '%s'", $item['mlid'], GMAPS_TAXONOMY_MENU_NAME);
  while($child = db_fetch_object($r)) {
    _gmaps_taxonomy_menu_delete_tree($child, TRUE);
  }
  
  if ($direct) {
    db_query('DELETE FROM {menu_links} WHERE mlid = %d', $item['mlid']);
  }
  else {
    _menu_delete_item($item);
  }
}

function _gmaps_taxonomy_menu_build_tree($level, $where, $args, $path, $plid, $existing = NULL) {
  static $levels, $parts;

  if (is_null($levels)) {
    $s = gmaps_taxonomy_get_settings('gmaps_taxonomy_menu');
    $levels = $s['levels'];
    $parts = array_keys(_gmaps_address_get_parts());
  }
  
  $part = $parts[$level];
  if (!$part) {
    return;
  }
  
  $next_level = $level + 1;
  $translatable = !in_array($part, array('postalcode', 'thoroughfare'));
  
  $include = _gmaps_taxonomy_menu_level_included($part);
  if ($part == 'thoroughfare' && !$include) {
    return;
  }
  
  $subinclude = FALSE;
  $maxlevel = count($parts);
  for ($i = $next_level; !$subinclude && $i < $maxlevel; $i++) {
    $subinclude = _gmaps_taxonomy_menu_level_included($parts[$i]);
  }
  
  if (!$existing) {
    $existing = _gmaps_taxonomy_menu_get_children($plid);
  }
  
  $has_row = FALSE;
  $r = db_query("SELECT DISTINCT(gca.$part) FROM {gmaps_content_address} gca INNER JOIN {node} n ON gca.vid = n.vid WHERE n.status = 1 AND $where AND gca.$part IS NOT NULL AND gca.$part != ''", $args);
  while ($row = db_fetch_object($r)) {
    $has_row = TRUE;
    
    $current_path = $path .','. gmaps_address_part_urlencode($row->$part);
    $current_plid = $plid;
    
    if ($include) {
      $mlid = _gmaps_taxonomy_menu_link_save($current_path, $row->$part, $existing[$current_path], $plid, $translatable);
      if ($mlid) {
        unset($existing[$current_path]);
        $current_plid = $mlid;
      }
    }
    if ($subinclude) {
      _gmaps_taxonomy_menu_build_tree($next_level, $where ." AND UPPER(gca.$part) = UPPER('%s')", array_merge($args, array($row->$part)), $current_path, $current_plid);
    }
  }
  
  if (!$has_row && $subinclude) {
    $path .= ',';
    _gmaps_taxonomy_menu_build_tree($next_level, $where ." AND (gca.$part IS NULL OR gca.$part = '')", $args, $path, $plid, $existing);
  }
  else if ($include || !$subinclude) {
    foreach($existing as $key => $item) {
      if ($item) {
        _gmaps_taxonomy_menu_delete_tree($item);
      }
    }
  }
}

function _gmaps_taxonomy_generate_locality_terms() {
  global $gmaps_taxonomy_locality_cron_last_nid;
  
  $count = variable_get('gmaps_taxonomy_location_cron', 0);
  
  if (!$count || variable_get('gmaps_taxonomy_location_terms', 0) != GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED) {
    return;
  }
  
  module_load_include('inc', 'gmaps_taxonomy', 'includes/gmaps_taxonomy.node-form');
  register_shutdown_function('_gmaps_taxonomy_generate_locality_terms_cleanup');
  
  $types = content_types();
  
  $gmaps_taxonomy_locality_cron_last_nid = variable_get('gmaps_taxonomy_location_cron_last_nid', 0);
  
  $result = db_query_range("SELECT nid FROM {node} n WHERE n.nid > %d ORDER BY n.nid", $gmaps_taxonomy_locality_cron_last_nid, 0, $count);
  while ($n = db_fetch_object($result)) {
    $node = node_load($n->nid);
    
    $type = $types[$node->type];
    if (!$type) {continue;}
    
    foreach($type['fields'] as $field_name => $field) {
      if (in_array($field['type'], array('gmaps_address', 'gmaps_anp'))) {
        $items = $node->$field_name;
        _gmaps_taxonomy_location_terms_save($node, $items);
      }
    }
    
    $gmaps_taxonomy_locality_cron_last_nid = $node->nid;
  }
  
  variable_set('gmaps_taxonomy_location_cron_last_nid', $gmaps_taxonomy_locality_cron_last_nid);
  $gmaps_taxonomy_locality_cron_last_nid = 0;
}

function _gmaps_taxonomy_generate_locality_terms_cleanup() {
  global $gmaps_taxonomy_locality_cron_last_nid;
  
  if ($gmaps_taxonomy_locality_cron_last_nid) {
    variable_set('gmaps_taxonomy_location_cron_last_nid', $gmaps_taxonomy_locality_cron_last_nid);
  }
}

function _gmaps_taxonomy_generate_letter_terms() {
  global $gmaps_taxonomy_letter_cron_last_nid;
  
  $count = variable_get('gmaps_taxonomy_letter_cron', 0);
  
  if (!$count || !variable_get('gmaps_taxonomy_letter_terms', 0)) {
    return;
  }
  
  module_load_include('inc', 'gmaps_taxonomy', 'includes/gmaps_taxonomy.node-form');
  register_shutdown_function('_gmaps_taxonomy_generate_letter_terms_cleanup');
  
  $types = content_types();
  
  $gmaps_taxonomy_letter_cron_last_nid = variable_get('gmaps_taxonomy_letter_cron_last_nid', 0);
  
  $result = db_query_range("SELECT nid FROM {node} n WHERE n.nid > %d ORDER BY n.nid", $gmaps_taxonomy_letter_cron_last_nid, 0, $count);
  while ($n = db_fetch_object($result)) {
    $node = node_load($n->nid);
    
    $type = $types[$node->type];
    if (!$type) {continue;}
    
    foreach($type['fields'] as $field_name => $field) {
      if (in_array($field['type'], array('gmaps_address', 'gmaps_anp'))) {
        $items = $node->$field_name;
        _gmaps_taxonomy_letter_terms_save($node, $items);
      }
    }
    
    $gmaps_taxonomy_letter_cron_last_nid = $node->nid;
  }
  
  variable_set('gmaps_taxonomy_letter_cron_last_nid', $gmaps_taxonomy_letter_cron_last_nid);
  $gmaps_taxonomy_letter_cron_last_nid = 0;
}

function _gmaps_taxonomy_generate_letter_terms_cleanup() {
  global $gmaps_taxonomy_letter_cron_last_nid;
  
  if ($gmaps_taxonomy_letter_cron_last_nid) {
    variable_set('gmaps_taxonomy_letter_cron_last_nid', $gmaps_taxonomy_letter_cron_last_nid);
  }
}