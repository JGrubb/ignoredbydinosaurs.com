<?php
// $Id: gmaps_taxonomy.pages.inc,v 1.1.2.7 2009/11/23 13:23:48 xmarket Exp $

/**
 * @file
 * GMaps Taxonomy location terms page functions.
 */

/**
 * Menu callback overwrite; displays all nodes associated with a term.
 */
function gmaps_taxonomy_term_page($str_tids = '', $depth = 0, $op = 'page') {
  $terms = taxonomy_terms_parse_string($str_tids);
  if ($terms['operator'] != 'and' && $terms['operator'] != 'or') {
    drupal_not_found();
  }

  if ($terms['tids']) {
    $result = db_query(db_rewrite_sql('SELECT t.tid, t.name, t.vid FROM {term_data} t WHERE t.tid IN ('. db_placeholders($terms['tids']) .')', 't', 'tid'), $terms['tids']);
    $tids = array(); // we rebuild the $tids-array so it only contains terms the user has access to.
    $names = array();
    $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
    $vids = variable_get('gmaps_taxonomy_location_vids', array());
    $letter_view = variable_get('gmaps_taxonomy_letter_term_view', 'Locality: @letter');
    while ($term = db_fetch_object($result)) {
      $tids[] = $term->tid;
      if (!in_array($term->vid, $vids) || $term->name != GMAPS_TAXONOMY_LOCATION_EMPTY_TERM) {
        $names[] = ($term->vid == $vid && $letter_view) ? t($letter_view, array('@letter' => $term->name)) : $term->name;
      }
    }

    if ($tids) {
      $title = implode(', ', $names);
      drupal_set_title(check_plain($title));

      switch ($op) {
        case 'page':
          // Build breadcrumb based on first hierarchy of first term:
          $current->tid = $tids[0];
          $breadcrumb = array();
          while ($parents = taxonomy_get_parents($current->tid)) {
            $current = array_shift($parents);
            if (!in_array($current->vid, $vids) || $current->name != GMAPS_TAXONOMY_LOCATION_EMPTY_TERM) {
              $breadcrumb[] = l($current->name, 'taxonomy/term/'. $current->tid);
            }
          }
          $breadcrumb[] = l(t('Home'), NULL);
          $breadcrumb = array_reverse($breadcrumb);
          drupal_set_breadcrumb($breadcrumb);

          $output = theme('taxonomy_term_page', $tids, taxonomy_select_nodes($tids, $terms['operator'], $depth, TRUE));
          drupal_add_feed(url('taxonomy/term/'. $str_tids .'/'. $depth .'/feed'), 'RSS - '. $title);
          return $output;
          break;

        case 'feed':
          $channel['link'] = url('taxonomy/term/'. $str_tids .'/'. $depth, array('absolute' => TRUE));
          $channel['title'] = variable_get('site_name', 'Drupal') .' - '. $title;
          // Only display the description if we have a single term, to avoid clutter and confusion.
          if (count($tids) == 1) {
            $term = taxonomy_get_term($tids[0]);
            // HTML will be removed from feed description, so no need to filter here.
            $channel['description'] = $term->description;
          }

          $result = taxonomy_select_nodes($tids, $terms['operator'], $depth, FALSE);
          $items = array(); 
          while ($row = db_fetch_object($result)) {
            $items[] = $row->nid;
          }

          node_feed($items, $channel);
          break;

        default:
          drupal_not_found();
      }
    }
    else {
      drupal_not_found();
    }
  }
}

/**
 * Menu callback; displays all nodes associated with the given location.
 */
function gmaps_taxonomy_location_page($args_str = '', $term_str = 0, $depth = 0, $op = 'page') {
  global $user, $language;

  //continents list
  if (empty($args_str)) {
    return theme('gmaps_taxonomy_continents_page');
  }
  //continent or addresses
  else {
    $args = gmaps_taxonomy_location_terms_parse($args_str);
    if (empty($args)) {
      drupal_not_found();
      exit();
    }
    
    if (isset($args['continent'])) {
      $continent = $args['continent'];
      $continents = geonames_continent_codes('short');
      $title = t($continents[$continent]['name']);
    }
    else {
      $names = array();
      foreach($args['locations'] as $adr) {
        $names[] = gmaps_address_format_plain($adr);
      }
      
      $title = implode(', ', $names);
      if (drupal_strlen($title) > 128) {
        $title = drupal_substr($title, 0, 125) .'...';
      }
    }
  }
  
  if (!empty($term_str)) {
    $terms = taxonomy_terms_parse_string($term_str);
    if (empty($terms['operator'])) {
      unset($terms);
    }
  }
  else {
    $term_str = 0;
  }
  
  if ($op == 'page') {
    drupal_set_title($title);
    if (!$continent) {
      drupal_set_breadcrumb(_gmaps_taxonomy_get_location_breadcrumbs($args['locations'][0], $term_str));
    }
    
    $cid = _gmaps_taxonomy_get_page_cache_id(FALSE);
    if ($cid && ($cached = cache_get($cid, GMAPS_TAXONOMY_CACHE_TABLE_PAGE))) {
      $output = $cached->data['output'];
      $headers = $cached->data['headers'];
    }
    
    if (!$output) {
      $se_agent = gmaps_agent_is_search_engine();
      //never cache se visits
      if (!$se_agent && $cid) {
        $cache = array('headers' => gmaps_gather_headers());
      }
      $output = theme('gmaps_taxonomy_location_page', _gmaps_taxonomy_select_nodes($args, $terms, $depth, TRUE));
      
      if (!$se_agent && $cid) {
        $cache['headers'] = gmaps_gather_headers($cache['headers']);
        $s = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
        cache_set($cid, $cache + array('output' => $output), GMAPS_TAXONOMY_CACHE_TABLE_PAGE, empty($s['output_expires']) ? CACHE_TEMPORARY : (time() + $s['output_expires']));
      }
    }
    
    if ($headers) {
      gmaps_restore_headers($headers);
    }
    
    drupal_add_feed(url('taxonomy/location/'. $args_str .'/'. $term_str .'/'. $depth .'/feed'), 'RSS - '. $title);
    
    return $output;
  }
  else if ($op == 'feed') {
    $channel['link'] = url('taxonomy/location/'. $args_str .($term_str ? '/'. $term_str .($depth ? '/'. $depth : '') : ''), array('absolute' => TRUE));
    $channel['title'] = variable_get('site_name', 'Drupal') .' - '. $title;

    $items = _gmaps_taxonomy_select_nodes($args, $terms, $depth, FALSE);

    node_feed($items, $channel);
  }
  else {
    drupal_not_found();
    exit();
  }
}

function theme_gmaps_taxonomy_continents_page() {
  $codes = array();
  $result = db_query('SELECT DISTINCT(gca.country) FROM {gmaps_content_address} gca INNER JOIN {node} n ON gca.vid = n.vid WHERE n.status = 1');
  while ($c = db_fetch_object($result)) {
    $codes[] = $c->country;
  }
  
  if (empty($codes)) {
    drupal_not_found();
    exit();
  }
  
  $countries = gmaps_get_countries();
  $continents = geonames_continent_codes('short');
  
  $result = array();
  
  foreach($codes as $c) {
    if (!isset($result[$countries[$c]['continent']])) {
      $result[$countries[$c]['continent']] = t($continents[$countries[$c]['continent']]['name']);
    }
  }
  
  if (count($result) == 1) {
    $c = array_shift($codes);
    drupal_goto('taxonomy/location/continent:'. $countries[$c]['continent']);
  }
  
  asort($result);
  foreach($result as $code => $name) {
    $result[$code] = l($name, 'taxonomy/location/continent:'. $code);
  }
  
  drupal_set_title(t('Continents'));
  return theme('item_list', $result);
}

function theme_gmaps_taxonomy_location_page($nids) {
  $output = '';
  foreach($nids as $nid) {
    $output .= node_view(node_load($nid), 1);
  }
  if ($output) {
    $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page');
    $output .= theme('pager', NULL, $settings['page'], 0);
  }
  else {
    $output = '<p>'. t('There are currently no posts at this location.') .'</p>';
  }
  return $output;
}

function theme_gmaps_taxonomy_letter_selector($path = '', $counter = 0, $title = TRUE) {
  $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
  if (!$vid) {
    return;
  }
  $terms = $counts = array();
  
  if ($counter) {
    $sql = db_rewrite_sql('SELECT t.tid, t.name, COUNT(*) count FROM {term_data} t INNER JOIN {term_node} tn ON t.tid = tn.tid INNER JOIN {node} n ON n.vid = tn.vid WHERE n.status = 1 AND t.vid = %d GROUP BY t.tid, t.name ORDER BY t.weight, t.name');
  }
  else {
    $sql = db_rewrite_sql('SELECT DISTINCT(t.tid), t.name FROM {term_data} t INNER JOIN {term_node} tn ON t.tid = tn.tid INNER JOIN {node} n ON n.vid = tn.vid WHERE n.status = 1 AND t.vid = %d ORDER BY t.weight, t.name');
  }
  $result = db_query($sql, $vid);
  while($term = db_fetch_object($result)) {
    $terms[$term->tid] = $term->name;
    if ($counter) {
      $counts[$term->tid] = $term->count;
    }
  }

  $c = count($terms);
  if (($c == 1 && !$path) || $c > 1) {
    if ($path) {
      $terms = array(t('Any')) + $terms;
    }
    else {
      $path = 'taxonomy/term';
    }
    
    foreach($terms as $tid => $name) {
      $terms[$tid] = l($name, $path . ($tid ? '/'. $tid : '')) . (($counter && $tid) ? (' ('. $counts[$tid] .')') : '');
    }
    
    $output = '<div class="gmaps-taxonomy-letter-selector">';
    $output .= theme('item_list', $terms, $title ? t('Locality name starts with') : NULL);
    $output .= '</div>';
    
    return $output;
  }
  
  return '';
}

function theme_gmaps_taxonomy_subpart_selector($subparts, $term_str, $title = TRUE) {
  $term_str = $term_str ? '/'. $term_str : '';
  
  $links = array();
  
  foreach($subparts as $path => $text) {
    $links[] = l($text, 'taxonomy/location/'. $path . $term_str);
  }
  
  $output = '<div class="gmaps-taxonomy-subpart-selector">';
  $output .= theme('item_list', $links, $title ? t('Subregions') : NULL);
  $output .= '</div>';
    
  return $output;
}

function _gmaps_taxonomy_select_nodes($locations, $terms = array(), $depth = 0, $pager = TRUE, $order = NULL) {
  $cid = _gmaps_taxonomy_get_page_cache_id();
  if ($cid && ($cached = cache_get($cid, GMAPS_TAXONOMY_CACHE_TABLE_RESULT))) {
    return $cached->data;
  }
  
  $sql = 'SELECT DISTINCT(n.nid), n.sticky, n.title, n.created FROM {node} n';
  $sql_count = 'SELECT COUNT(DISTINCT(n.nid)) FROM {node} n';
  $filter = '';
  $args = array();
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page');
  
  if ($locations['operator'] == 'and') {
    foreach($locations['locations'] as $id => $adr) {

      //allow to use gca in configurable order clause
      $s = $id ? $id : '';
      $result = gmaps_content_address_sql_filter($adr, 'gca'. $s, $adr->last_part, $settings['strict']);
      if (!$result) continue;
      
      list ($query, $query_args) = $result;
      $filter .= (!empty($filter) ? ' AND ' : '') . '(' . $query .')';
      $args = array_merge($args, $query_args);

      $sql .= " INNER JOIN {gmaps_content_address} gca$s ON n.vid = gca$s.vid";
      $sql_count .= " INNER JOIN {gmaps_content_address} gca$s ON n.vid = gca$s.vid";
    }

  }
  else {
    foreach($locations['locations'] as $id => $adr) {
      $result = gmaps_content_address_sql_filter($adr, 'gca', $adr->last_part, $settings['strict']);
      if (!$result) continue;
      
      list ($query, $query_args) = $result;
      $filter .= (!empty($filter) ? ' OR ' : '') . '(' . $query .')';
      $args = array_merge($args, $query_args);
    }
    $filter = $filter ? '('. $filter .')' : '';

    $sql .= ' INNER JOIN {gmaps_content_address} gca ON n.vid = gca.vid';
    $sql_count .= ' INNER JOIN {gmaps_content_address} gca ON n.vid = gca.vid';
  }

  if (!empty($terms)) {
    $tids = $terms['tids'];
    // For each term ID, generate an array of descendant term IDs to the right depth.
    $descendant_tids = array();
    if ($depth === 'all') {
      $depth = NULL;
    }
    foreach ($tids as $index => $tid) {
      $term = taxonomy_get_term($tid);
      $tree = taxonomy_get_tree($term->vid, $tid, -1, $depth);
      $descendant_tids[] = array_merge(array($tid), array_map('_taxonomy_get_tid_from_term', $tree));
    }

    if ($terms['operator'] == 'or') {
      $query_args = call_user_func_array('array_merge', $descendant_tids);
      $placeholders = db_placeholders($query_args, 'int');
      $filter .= (!empty($filter) ? ' AND ' : '') .'(tn.tid IN ('. $placeholders .'))';
      $args = array_merge($args, $query_args);
      $sql .= ' INNER JOIN {term_node} tn ON n.vid = tn.vid';
      $sql_count .= ' INNER JOIN {term_node} tn ON n.vid = tn.vid';
    }
    else {
      $joins = '';
      $wheres = '';
      foreach ($descendant_tids as $index => $tids) {
        $filter .= (!empty($filter) ? ' AND ' : '') .'(tn'. $index .'.tid IN ('. db_placeholders($tids, 'int') .'))';
        $args = array_merge($args, $tids);

        $sql .= ' INNER JOIN {term_node} tn'. $index .' ON n.vid = tn'. $index .'.vid';
        $sql_count .= ' INNER JOIN {term_node} tn'. $index .' ON n.vid = tn'. $index .'.vid';
      }
    }
  }
  
  if (!$order) {
    $order = $settings['order'];
  }
  if (!$order) {
    $order = 'n.sticky DESC, n.created DESC';
  }
  
  $sql .= ' WHERE '. $filter . ($filter ? ' AND ' : '') .'n.status = 1 ORDER BY '. $order;
  $sql_count .= ' WHERE '. $filter . ($filter ? ' AND ' : '') .'n.status = 1';
  
  $sql = db_rewrite_sql($sql);
  $sql_count = db_rewrite_sql($sql_count);
  
  if ($pager) {
    $result = pager_query($sql, $settings['page'], 0, $sql_count, $args);
  }
  else {
    $result = db_query_range($sql, $args, 0, $settings['feed']);
  }
  
  $nids = array();
  while($node = db_fetch_object($result)) {
    $nids[] = $node->nid;
  }
  
  //never cache se visits
  if ($cid && !gmaps_agent_is_search_engine()) {
    $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
    cache_set($cid, $nids, GMAPS_TAXONOMY_CACHE_TABLE_RESULT, empty($settings['result_expires']) ? CACHE_TEMPORARY : (time() + $settings['result_expires']));
  }
    
  return $nids;
}

function _gmaps_taxonomy_get_continent_subparts($countries) {
  $names = gmaps_get_countries('names');
  $args = array();
  $counter = variable_get('gmaps_taxonomy_subpart_counter', 0);
  
  foreach($countries as $adr) {
    $args[$adr->country] = "'%s'";
  }
  
  $result = array();
  if ($counter) {
    $sql = 'SELECT gca.country, COUNT(*) count FROM {gmaps_content_address} gca INNER JOIN {node} n ON gca.vid = n.vid WHERE gca.country IN ('. implode(',', $args).') AND n.status = 1 GROUP BY gca.country';
  }
  else {
    $sql = 'SELECT DISTINCT(gca.country) FROM {gmaps_content_address} gca INNER JOIN {node} n ON gca.vid = n.vid WHERE gca.country IN ('. implode(',', $args).') AND n.status = 1';
  }
  $sql = db_rewrite_sql($sql);
  $r = db_query($sql, array_keys($args));
  while ($c = db_fetch_object($r)) {
    $result[$c->country] = $counter ? t('@title (@count)', array('@title' => $names[$c->country], '@count' => $c->count)) : $names[$c->country];
  }
  
  asort($result);
  
  return $result;
}

function _gmaps_taxonomy_get_address_subparts($adr) {
  if ($adr->last_part == 'thoroughfare') {
    return array();
  }
  
  $parts = array_keys(_gmaps_address_get_parts());
  
  $part = array_search($adr->last_part, $parts);
  if ($part === FALSE) {
    return array();
  }
  $part = $parts[$part+1];
  
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page');

  if (!($r = gmaps_content_address_sql_filter($adr, 'gca', $adr->last_part, $settings['strict']))) {
    return array();
  }
  
  list($query, $args) = $r;
  $a = drupal_clone($adr);
  
  $counter = variable_get('gmaps_taxonomy_subpart_counter', 0);
  
  $result = array();
  if ($counter) {
    $sql = "SELECT gca.$part, COUNT(*) count FROM {gmaps_content_address} gca INNER JOIN {node} n ON n.vid = gca.vid WHERE $query AND n.status = 1 GROUP BY gca.$part";
  }
  else {
    $sql = "SELECT DISTINCT(gca.$part) FROM {gmaps_content_address} gca INNER JOIN {node} n ON n.vid = gca.vid WHERE $query AND n.status = 1";
  }
  $sql = db_rewrite_sql($sql);
  $r = db_query($sql, $args);
  while($row = db_fetch_object($r)) {
    $a->$part = $row->$part;
    $args = gmaps_address_url_args($a, $part);
    
    if (empty($row->$part)) {
      if ($settings['strict']) {
        $empty = array($args[$part] => $counter ? t('Not set (@count)', array('@count' => $row->count)) : t('Not set'));
      }
    }
    else {
      if ($part == 'adminarea') {
        $title = gmaps_address_format_state($a, 'name');
      }
      else {
        $title = ($part == 'postalcode' || $part == 'thoroughfare') ? $a->$part : t($a->$part);
      }
      $result[$args[$part]] = $counter ? t('@title (@count)', array('@title' => $title, '@count' => $row->count)) : $title;
    }
  }
  
  asort($result);
  if ($empty) {
    $result = array_merge($empty, $result);
  }
  
  return $result;
}

function _gmaps_taxonomy_block_view($delta = 0) {
  static $args_str, $args, $term_str;
  
  if (is_null($args)) {
    $args_str = arg(2);
    $term_str = arg(3);
    if ($depth = arg(4)) {
      $term_str .= '/'. $depth;
    }
    
    $args = gmaps_taxonomy_location_terms_parse($args_str);
  }
  
  if (!$args) {
    return;
  }
  
  if ($delta == 'letter') {
    $counter = variable_get('gmaps_taxonomy_letter_counter', 0);
    $block = array(
      'subject' => t('Locality name starts with'),
      'content' => theme('gmaps_taxonomy_letter_selector', 'taxonomy/location/'. $args_str, $counter, FALSE),
    );
  }
  else if ($delta == 'subpart') {
    if ($args['continent']) {
      $subparts = _gmaps_taxonomy_get_continent_subparts($args['locations']);
    }
    else {
      $subparts = _gmaps_taxonomy_get_address_subparts($args['locations'][0]);
    }
    if ($subparts) {
      $block = array(
        'subject' => t('Subregions'),
        'content' => theme('gmaps_taxonomy_subpart_selector', $subparts, $term_str, FALSE),
      );
    }
  }
  
  if ($block) {
    drupal_add_css(drupal_get_path('module', 'gmaps_taxonomy') .'/misc/gmaps-taxonomy.css');
  }
  
  return $block;
}
