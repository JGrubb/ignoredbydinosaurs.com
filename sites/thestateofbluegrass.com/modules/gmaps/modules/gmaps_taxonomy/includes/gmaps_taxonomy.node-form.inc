<?php
// $Id: gmaps_taxonomy.node-form.inc,v 1.1.2.4 2009/10/16 11:17:00 xmarket Exp $

/**
 * @file
 * GMaps Taxonomy node form related functions.
 */

function _gmaps_taxonomy_gmaps_address_field($op, &$node, $field, &$items, $teaser, $page) {
  $gmaps = &$field['widget']['gmaps']['meta']['taxonomy'];
  if ($gmaps['location'] && variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED) {
    _gmaps_taxonomy_location_terms_save($node, $items);
  }
  if ($gmaps['letter'] && variable_get('gmaps_taxonomy_letter_terms', 0)) {
    _gmaps_taxonomy_letter_terms_save($node, $items);
  }
}

function _gmaps_taxonomy_location_terms_save(&$node, &$items) {
  if (empty($items)) return; 
  
  $terms = $vids = array();

  $countries = gmaps_get_countries();
  $continents = geonames_continent_codes('short');
  
  $parts = _gmaps_address_get_parts();
  unset($parts['country'], $parts['thoroughfare']);

  _gmaps_taxonomy_unset_location_terms($node);
  
  foreach($items as $delta => $item) {
    if (gmaps_address_empty($item['value']->address)) {
      continue;
    }
    $a = &$item['value']->address;
    
    if (!isset($vids[$a->country])) {
      $vid = _gmaps_taxonomy_get_location_vocabulary($a->country);
      if ($vid) {
        $vids[$a->country] = $vid;
      }
      else {
        continue;
      }
    }
    else {
      $vid = $vids[$a->country];
    }
    
    if (!isset($vocs)) {
      $vocs = taxonomy_get_vocabularies($node->type);
    }

    if (!isset($vocs[$vid])) {
      continue;
    }
    
    if (variable_get('gmaps_taxonomy_location_vocabulary', GMAPS_TAXONOMY_LOCATION_VOCABULARY_GLOBAL) == GMAPS_TAXONOMY_LOCATION_VOCABULARY_GLOBAL
         && variable_get('gmaps_taxonomy_location_continent_term', 1)) {
      $term = _gmaps_taxonomy_get_location_term($continents[$countries[$a->country]['continent']]['name'], $vid);
      $terms[$term->tid] = drupal_clone($term);
      $weight = 1;
    }
    else {
      $term = 0;
      $weight = 0;
    }
    
    $term = _gmaps_taxonomy_get_location_term($countries[$a->country]['name'], $vid, $term, $weight);
    $terms[$term->tid] = drupal_clone($term);
    $weight++;
    
    foreach($parts as $part => $label) {
      $name = NULL;
      if (!empty($a->$part) || gmaps_address_has_subpart($a, $part)) {
        $name = empty($a->$part) ? GMAPS_TAXONOMY_LOCATION_EMPTY_TERM : $a->$part;
        $term = _gmaps_taxonomy_get_location_term($name, $vid, $term, $weight);
        $weight++;
        if (!empty($a->$part)) {
          $terms[$term->tid] = drupal_clone($term);
        }
      }
      if (is_null($name)) {
        break;
      }
    }
    
  }
  
  if (!empty($terms)) {
    foreach($terms as $tid => $term) {
      $node->taxonomy[$tid] = $term;
    }
    taxonomy_node_save($node, $node->taxonomy);
  }
}

function _gmaps_taxonomy_unset_location_terms(&$node) {
  if (!isset($node->taxonomy)) {
    return;
  }
  $vids = variable_get('gmaps_taxonomy_location_vids', array());
  if (!empty($vids)) {
    $terms = array();
    $result = db_query('SELECT tid FROM {term_data} WHERE vid IN ('. implode(',', $vids) .')');
    while($term = db_fetch_object($result)) {
      unset($node->taxonomy[$term->tid]);
      $terms[$term->tid] = $term->tid;
    }
    
    if (!empty($terms)) {
      foreach($node->taxonomy as $node_vid => $node_terms) {
        if (is_array($node_terms)) {
          foreach($terms as $tid) {
            if (isset($node_terms[$tid])) {
              unset($node->taxonomy[$node_vid][$tid], $terms[$tid]);
            }
          }
          if (empty($node->taxonomy[$node_vid])) {
            unset($node->taxonomy[$node_vid]);
          }
        }
      }
    }
  }
}

function _gmaps_taxonomy_letter_terms_save(&$node, &$items) {
  if (empty($items)) return; 
  
  $terms = $vocs = array();

  _gmaps_taxonomy_unset_letter_terms($node);
  
  $vid = _gmaps_taxonomy_get_letter_vocabulary();
  
  $vocs = taxonomy_get_vocabularies($node->type);

  if (!isset($vocs[$vid])) {
    return;
  }
  
  foreach($items as $delta => $item) {
    if (empty($item['value']->address->locality)) {
      continue;
    }
    
    $locality = &$item['value']->address->locality;
    
    if ($term = _gmaps_taxonomy_get_letter_term($locality)) {
      $terms[$term->tid] = drupal_clone($term);
    }
  }
  
  if (!empty($terms)) {
    foreach($terms as $tid => $term) {
      $node->taxonomy[$tid] = $term;
    }
    taxonomy_node_save($node, $node->taxonomy);
  }
}

function _gmaps_taxonomy_unset_letter_terms(&$node) {
  if (!isset($node->taxonomy)) {
    return;
  }
  $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
  if (!empty($vid)) {
    $terms = array();
    $result = db_query('SELECT tid FROM {term_data} WHERE vid = %d', $vid);
    while($term = db_fetch_object($result)) {
      unset($node->taxonomy[$term->tid]);
      $terms[$term->tid] = $term->tid;
    }
    
    if (!empty($terms)) {
      foreach($node->taxonomy as $node_vid => $node_terms) {
        if (is_array($node_terms)) {
          foreach($terms as $tid) {
            if (isset($node_terms[$tid])) {
              unset($node->taxonomy[$node_vid][$tid], $terms[$tid]);
            }
          }
          if (empty($node->taxonomy[$node_vid])) {
            unset($node->taxonomy[$node_vid]);
          }
        }
      }
    }
  }
}

function _gmaps_taxonomy_get_letter_vocabulary() {
  static $nodes;

  if (!variable_get('gmaps_taxonomy_letter_terms', 0) || !(module_exists('pathauto') && file_exists(drupal_get_path('module', 'pathauto') .'/i18n-ascii.txt'))) {
    return NULL;
  }
  
  if (is_null($nodes)) {
    $nodes = array();
    foreach (content_types() as $type_name => $type) {
      foreach($type['fields'] as $field_name => $field) {
        if (in_array($field['type'], array('gmaps_address', 'gmaps_anp')) && $field['widget']['gmaps']['meta']['taxonomy']['letter']) {
          $nodes[$type_name] = 1;
          break;
        }
      }
    }
  }

  if (!count($nodes)) {
    return NULL;
  }
  
  $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
  if (!$vid) {
    $edit = array('name' => 'Locality first letters', 'multiple' => 1, 'hierarchy' => 0, 'required' => 0, 'relations' => 0, 'tags' => 0, 'module' => 'gmaps_taxonomy', 'weight' => 0, 'nodes' => $nodes);
    taxonomy_save_vocabulary($edit);
    $vid = $edit['vid'];
    variable_set('gmaps_taxonomy_letter_vid', $vid);
  }

  return $vid;
}

function _gmaps_taxonomy_get_letter_term($locality) {
  static $tr, $weights;
  
  if (empty($locality)) {
    return NULL;
  }
  
  $vid = _gmaps_taxonomy_get_letter_vocabulary();
  if (!$vid) {
    return NULL;
  }
  
  if (is_null($tr)) {
    $tr = parse_ini_file(drupal_get_path('module', 'pathauto') .'/i18n-ascii.txt');
  }
  
  if (is_null($weights)) {
    $weights = array_flip(range('A', 'Z'));
  }
  
  $letter = strtoupper(substr(strtr($locality, $tr), 0, 1));
  
  $db_result = db_query("SELECT t.tid, t.* FROM {term_data} t WHERE LOWER('%s') = LOWER(t.name) AND t.vid = %d", $letter, $vid);
  if ($term = db_fetch_object($db_result)) {
    return $term;
  }
  else {
    $term = array('name' => $letter, 'vid' => $vid, 'weight' => isset($weights[$letter]) ? $weights[$letter] : 26);
    taxonomy_save_term($term);
    return (object)$term;
  }
}

function _gmaps_taxonomy_node_form_alter(&$form, &$form_state) {
  if (variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED) {
    $vids = variable_get('gmaps_taxonomy_location_vids', array());
    foreach ($vids as $vid) {
      unset($form['taxonomy'][$vid]);
    }
  }
  if (variable_get('gmaps_taxonomy_letter_terms', 0)) {
    unset($form['taxonomy'][variable_get('gmaps_taxonomy_letter_vid', 0)]);
  }
  
  $vids = element_children($form['taxonomy']);
  if (!count($vids)) {
    unset($form['taxonomy']);
  } else if (count($vids) == 1) {
    $form['taxonomy']['#type'] = 'markup';
  }
}