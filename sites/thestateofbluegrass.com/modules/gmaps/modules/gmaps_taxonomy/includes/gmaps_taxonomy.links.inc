<?php
// $Id: gmaps_taxonomy.links.inc,v 1.1.2.3 2009/11/23 13:23:48 xmarket Exp $

/**
 * @file
 * Node links.
 */

function _gmaps_taxonomy_link_alter(&$links, $node) {
  if (!empty($node->taxonomy)) {
    $terms = FALSE;
    foreach ($links as $module => $link) {
      if (strpos($module, 'taxonomy_term') !== FALSE) {
        $terms = TRUE;
        break;
      }
    }
    if (!$terms) {
      return;
    }
    unset($links['taxonomy_term_0'], $node->taxonomy[0]);
    
    $location = variable_get('gmaps_taxonomy_location_terms', 0);
    $letter = variable_get('gmaps_taxonomy_letter_terms', 0);
    $letter_view = variable_get('gmaps_taxonomy_letter_term_view', 'Locality: @letter');
    if ($location || $letter) {
      $vids = variable_get('gmaps_taxonomy_location_vids', array());
      $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
      
      foreach($node->taxonomy as $term) {
        if (is_object($term)) {
          if ($location && in_array($term->vid, $vids)) {
            unset($links['taxonomy_term_'. $term->tid]);
          }
          
          if ($letter && $term->vid == $vid) {
            if (!variable_get('gmaps_taxonomy_letter_term_show', 0)) {
              unset($links['taxonomy_term_'. $term->tid]);
            }
            else if (!empty($letter_view)) {
              $links['taxonomy_term_'. $term->tid]['title'] = t($letter_view, array('@letter' => $term->name));
            }
          }
        }
      }
      
      if ($location) {
        $links += _gmaps_taxonomy_build_location_term_links($node);
      }
    }
  }
}

function _gmaps_taxonomy_build_location_term_links($node) {
  global $language, $user;
  static $continents;
  
  $cid = _gmaps_taxonomy_get_links_cache_id($node);
  if ($cid && ($links = cache_get($cid, GMAPS_TAXONOMY_CACHE_TABLE_LINKS))) {
    return $links->data;
  }

  $countries = gmaps_get_countries();
  if (is_null($continents)) {
    $continents = geonames_continent_codes('short');
  }
  $type = content_types($node->type);
  $view = variable_get('gmaps_taxonomy_location_term_view', 'raw');
  $country_view = variable_get('gmaps_taxonomy_location_term_view_country', 'raw');
  $links = array();
  $added = array();

  foreach($type['fields'] as $field_name => $field) {
    $gmaps = $field['widget']['gmaps']['meta']['taxonomy'];
    if (in_array($field['type'], array('gmaps_address', 'gmaps_anp')) && $gmaps['location'] && isset($node->$field_name) && !empty($node->$field_name)) {
      $s = $user->gmaps_settings['address'];
      $s['taxonomy_links'] = 0;
      $gmaps = $gmaps['show'];
      foreach($node->$field_name as $delta => $item) {
        if (gmaps_address_empty($item['value']->address)) {
          continue;
        }
        $adr = drupal_clone($item['value']->address);
        gmaps_address_apply_privacy($adr);
        
        $pathes = gmaps_taxonomy_location_pathes($adr);
        
        if ($gmaps['continent'] && isset($pathes['continent']) && !isset($added[$pathes['continent']])) {
          $links['taxonomy_location_'. $field_name .'_'. $delta .'_continent'] = array(
            'title' => t($continents[$countries[$adr->country]['continent']]['name']),
            'href' => $pathes['continent'],
            'attributes' => array('rel' => 'tag')
          );
          $added[$pathes['continent']] = 1;
        }
        
        if ($gmaps['last_part']) {
          $last_part = 'country';
          foreach($pathes as $part => $path) {
            if (!empty($adr->$part) || gmaps_address_has_subpart($adr, $part)) {
              $last_part = $part;
            }
          }
          
          if (isset($added[$pathes[$last_part]])) {
            continue;
          }
          
          $current_view = ($last_part == 'country') ? $country_view : $view;
          $current_flag = ($last_part == 'country') ? $s['flag'] : FALSE;
          if ($current_view == 'raw') {
            if ($last_part == 'country') {
              $title = t($countries[$adr->$last_part]['name']);
            }
            else {
              $title = $last_part != 'postalcode' ? t($adr->$last_part) : $adr->$last_part;
            }
          }
          else if ($current_view == 'plain') {
            $title = gmaps_address_format_plain($adr, $s['country'], $s['state'], TRUE);
          }
          else if ($current_view == 'themed') {
            $title = gmaps_address_format($adr, $s['country'], $s['state'], $current_flag, FALSE, TRUE);
          }
          else {
            $title = gmaps_address_render($adr, $s, TRUE);
          }
          $links['taxonomy_location_'. $field_name .'_'. $delta .'_'. $last_part] = array(
            'title' => $title,
            'href' => $pathes[$last_part],
            'attributes' => array('rel' => 'tag'),
            'html' => $current_view != 'raw',
          );
          $added[$pathes[$last_part]] = 1;
        }
        else {
          $a = (object)array('country' => $adr->country);
          $show_empty = $gmaps['empty'];
          $gmaps = $gmaps['parts'];
          if ($gmaps['country'] && !isset($added[$pathes['country']])) {
            if ($country_view == 'raw') {
              $title = t($countries[$adr->country]['name']);
            }
            else if ($country_view == 'plain') {
              $title = gmaps_address_format_plain($a, $s['country'], $s['state'], TRUE);
            }
            else if ($country_view == 'themed') {
              $title = gmaps_address_format($a, $s['country'], $s['state'], $s['flag'], FALSE, TRUE);
            }
            else {
              $title = gmaps_address_render($a, $s, TRUE);
            }
            $links['taxonomy_location_'. $field_name .'_'. $delta .'_country'] = array(
              'title' => $title,
              'href' => $pathes['country'],
              'attributes' => array('rel' => 'tag'),
              'html' => $country_view != 'raw',
            );
            $added[$pathes['country']] = 1;
          }
          unset($pathes['continent'], $pathes['country']);
      
          $s['flag'] = FALSE;
          foreach($pathes as $part => $path) {
            if (empty($gmaps[$part]) || isset($added[$path]) || (empty($adr->$part) && !$show_empty)) {
              continue;
            }
            
            $a->$part = $adr->$part;
            if ($view == 'raw') {
              $title = $part != 'postalcode' ? t($a->$part) : $a->$part;
            }
            else if ($view == 'plain') {
              $title = gmaps_address_format_plain($a, $s['country'], $s['state'], TRUE);
            }
            else if ($view == 'themed') {
              $title = gmaps_address_format($a, $s['country'], $s['state'], FALSE, FALSE, TRUE);
            }
            else {
              $title = gmaps_address_render($a, $s, TRUE);
            }

            $links['taxonomy_location_'. $field_name .'_'. $delta .'_'. $part] = array(
              'title' => $title,
              'href' => $pathes[$part],
              'attributes' => array('rel' => 'tag'),
              'html' => $view != 'raw',
            );
            $added[$pathes[$part]] = 1;
          }
        }
      }
    }
  }
  
  //never cache se visits
  if ($cid && !gmaps_agent_is_search_engine()) {
    $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_links_cache');
    cache_set($cid, $links, GMAPS_TAXONOMY_CACHE_TABLE_LINKS, empty($settings['expires']) ? CACHE_TEMPORARY : (time() + $settings['expires']));
  }
  
  return $links;
}

function _gmaps_taxonomy_get_links_cache_id($node) {
  global $theme, $user;
  
  $cid_parts = array();
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_links_cache');
  
  if ($user->uid != 1 && $settings['enabled']) {
    if (empty($theme)) {
      init_theme();
    }
    
    //common parts
    $cid_parts[] = $node->nid;
    $cid_parts[] = $node->vid;
    $cid_parts[] = $theme;
    if (module_exists('locale')) {
      global $language;
      $cid_parts[] = $language->language;
    }
    
    $user_mask = $settings['user_mask'];
    if (count(module_implements('node_grants'))) {
      $user_mask = GMAPS_CACHE_PER_USER;
    }
    if ($user_mask == GMAPS_CACHE_PER_ROLE) {
      $cid_parts[] = "r.". implode(',', array_keys($user->roles));
    }
    else if ($user_mask == GMAPS_CACHE_PER_USER) {
      $cid_parts[] = "u$user->uid.";
    }
    
    return implode(':', $cid_parts);
  }
}
