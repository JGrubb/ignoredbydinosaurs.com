<?php
// $Id: gmaps_content.pages.inc,v 1.1.2.9 2010/02/26 14:13:03 xmarket Exp $

/**
 * @file
 * CCK field related display functions.
 */

function _gmaps_content_preprocess_page(&$vars, &$node) {
  $tags = function_exists('nodewords_get_tags') ? nodewords_get_tags(NODEWORDS_TYPE_NODE, array($node->nid)) : array();
  $point_tags = (empty($tags['geo.position']) && empty($tags['icbm']));
  if ($output = _gmaps_content_meta_tags($node, $point_tags)) {
    $vars['head'] .= "\n$output";
  }
}

function _gmaps_content_meta_tags($node, $point_tags) {
  static $added = array('address' => FALSE, 'point' => FALSE);
  
  $output = '';
  
  if ($added['address'] && $added['point']) {
    return $output;
  }
  
  $type = content_types($node->type);
  foreach($type['fields'] as $field_name => $field) {
    if ($field['module'] == 'gmaps_content' && in_array($field['type'], array('gmaps_address', 'gmaps_point', 'gmaps_anp'))) {
      $gmaps = &$field['widget']['gmaps']['meta']['meta'];

      $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
      $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');

      if (!$point_tags) {
        $added['point'] = TRUE;
      }
      else if (!$added['point'] && $point && ($gmaps['geo_position'] || $gmaps['icbm'])) {
        $p = $node->{$field_name}[0]['value']->point;
        
        if (!gmaps_point_empty($p)) {
          $added['point'] = TRUE;
          
          if ($gmaps['geo_position']) {
            $output .= "\n" . '<meta name="geo.position" content="'. gmaps_coords_format_dd_query($p, FALSE, ';') .'" />';
          }
          if ($gmaps['icbm']) {
            $output .= "\n" . '<meta name="ICBM" content="'. gmaps_coords_format_dd_query($p) .'" />';
          }
        }
      }

      if (!$added['address'] && $address && ($gmaps['geo_placename'] || $gmaps['geo_region'])) {
        $adr = $node->{$field_name}[0]['value']->address;
        
        if (!gmaps_address_empty($adr)) {
          $added['address'] = TRUE;
          
          if ($gmaps['geo_placename']) {
            $output .= "\n" . '<meta name="geo.placename" content="'. gmaps_address_format_plain($adr, 'iso3_name', 'both') .'" />';
          }
          if ($gmaps['geo_region']) {
            module_load_include('inc', 'gmaps', 'includes/gmaps.address');
            $a = (object)gmaps_extend(array(), $adr);
            _gmaps_address_apply_privacy($a);
            $state = _gmaps_address_format_state($a, 'code');
            $output .= "\n" . '<meta name="geo.region" content="'. strtoupper($adr->country) . ($state ? '-'. $state : '') .'" />';
          }
        }
      }
    }
    if ($added['address'] && $added['point']) {
      break;
    }
  }
  
  return $output;
}

function _gmaps_content_nodeapi_rss_item(&$node) {
  static $added = FALSE;
  
  if ($added) {
    return;
  }
  
  $items = array();
  $type = content_types($node->type);
  
  foreach($type['fields'] as $field_name => $field) {
    if ($field['module'] == 'gmaps_content' && in_array($field['type'], array('gmaps_point', 'gmaps_anp'))) {
      $gmaps = &$field['widget']['gmaps']['meta']['rss'];
      
      $p = $node->{$field_name}[0]['value']->point;
      
      if (!gmaps_point_empty($p) && ($gmaps['w3c'] || $gmaps['simple'] || $gmaps['gml'])) {
        $added = TRUE;
        
        if ($gmaps['simple']) {
          $items[] = gmaps_coords_rss_item($p, 'simple');
        }
        if ($gmaps['gml']) {
          $items[] = gmaps_coords_rss_item($p, 'gml');
        }
        if ($gmaps['w3c']) {
          $items[] = gmaps_coords_rss_item($p, 'w3c');
        }
      }
    }
    if ($added) {
      break;
    }
  }
  
  return $items;
}

function _gmaps_content_nodeapi_search_result(&$node) {
  global $user, $gmaps_throttle;
  static $added = array('address' => FALSE, 'point' => FALSE, 'refloc' => FALSE, 'geoip' => FALSE);
  
  if ($gmaps_throttle || ($added['address'] && $added['point'] && $added['refloc'] && $added['geoip'])) {
    return;
  }
  
  $extra = array();
  
  $type = content_types($node->type);
  foreach($type['fields'] as $field_name => $field) {
    if ($field['module'] == 'gmaps_content' && in_array($field['type'], array('gmaps_address', 'gmaps_point', 'gmaps_anp'))) {
      $gmaps = &$field['widget']['gmaps']['meta']['search'];
      
      $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
      $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');
      
      if (!$added['address'] && $address && $gmaps['address_snippet']) {
        $adr = $node->{$field_name}[0]['value']->address;
        
        if (!gmaps_address_empty($adr)) {
          $added['address'] = TRUE;
          
          $s = $field['gmaps']['formatter']['address'];
          if ($s['user_settings']) {
            $s = $user->gmaps_settings['address'];
          }
          $extra[] = gmaps_address_format_plain($adr, $s['country'], $s['state']);
        }
      }

      if ($point) {
        if (!$added['point'] && $gmaps['point_snippet']) {
          $p = $node->{$field_name}[0]['value']->point;
          
          if (!gmaps_point_empty($p)) {
            $added['point'] = TRUE;
            
            $s = $field['gmaps']['formatter']['point'];
            if ($s['user_settings']) {
              $s = $user->gmaps_settings['coordinate'];
            }
            $extra[] = gmaps_coords_format_plain($p, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
          }
        }

        if (!$added['refloc'] && $gmaps['refloc_snippet']) {
          if ($refloc = gmaps_get_reference_location()) {
            $refloc_dist = NULL;
            foreach($node->{$field_name} as $delta => $value) {
              if (($p = $value['value']->point) && !gmaps_point_empty($p)) {
                $dist = gmaps_coords_distance($p, $refloc->point);
                if ($dist) {
                  $refloc_dist = is_null($refloc_dist) ? $dist : min($refloc_dist, $dist);
                }
              }
            }
            if ($refloc_dist) {
              $refloc_label = (isset($refloc->address) && !gmaps_address_empty($refloc->address)) ? gmaps_address_format_plain($refloc->address, 'iso3_name', 'both') : 'reference location';
              $title = t('Distance from @label', array('@label' => $refloc_label));
              $extra[] = '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $refloc_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
              $added['refloc'] = TRUE;
            }
          }
        }

        if (!$added['geoip'] && $gmaps['geoip_snippet']) {
          $geoip = gmaps_get_geoip_location();
          if ($geoip) {
            $geoip_dist = NULL;
            foreach($node->{$field_name} as $delta => $value) {
              if (($p = $value['value']->point) && !gmaps_point_empty($p)) {
                $dist = gmaps_coords_distance($p, $geoip->point);
                if ($dist) {
                  $geoip_dist = is_null($geoip_dist) ? $dist : min($geoip_dist, $dist);
                }
              }
            }
            if ($geoip_dist) {
              $geoip_label = gmaps_address_format_plain($geoip->address, 'name', 'name');
              $title = t('Distance from @label', array('@label' => $geoip_label));
              $extra[] = '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $geoip_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
              $added['geoip'] = TRUE;
            }
          }
        }
      }
    }
    if ($added['address'] && $added['point'] && $added['refloc'] && $added['geoip']) {
      break;
    }
  }
  
  return !empty($extra) ? $extra : NULL;
}

function _gmaps_content_nodeapi_update_index(&$node) {
  $text = '';
  $type = content_types($node->type);
  
  foreach($type['fields'] as $field_name => $field) {
    if ($field['module'] == 'gmaps_content' && in_array($field['type'], array('gmaps_address', 'gmaps_anp'))) {
      $gmaps = &$field['widget']['gmaps']['meta']['search'];
      
      if ($gmaps['address_index']) {
        foreach($node->{$field_name} as $value) {
          if (!gmaps_address_empty($value['value']->address)) {
            $text .= $value['value']->address->country .' '. gmaps_address_format_plain($value['value']->address, 'iso3_name', 'both');
          }
        }
      }
    }
  }
  return $text;
}

function _gmaps_content_link_node($node = NULL, $teaser = FALSE) {
  global $user, $gmaps_throttle;
  $links = $alinks = $plinks = $rlinks = $glinks = $vlinks = array();
  
  if (!$node || empty($node->type)) {
    return $links;
  }
  
  $type = content_types($node->type);
  
  if (!$gmaps_throttle) {
    $geoip = gmaps_get_geoip_location();
    if ($geoip) {
      $geoip_label = gmaps_address_format_plain($geoip->address, 'iso3_name', 'both');
      $geoip_info = t('GeoIP');
    }
    
    $refloc = gmaps_get_reference_location();
    if ($refloc) {
      if (isset($refloc->address) && !gmaps_address_empty($refloc->address)) {
        $refloc_label = gmaps_address_format_plain($refloc->address, 'iso3_name', 'both');
        $refloc_info = t('Reference location');
      }
      else {
        $refloc_label = 'reference location';
      }
    }
    
    $viewloc = views_get_current_view();
    if ($viewloc) {
      $viewloc = $viewloc->gmaps_viewloc;
    }
    if ($viewloc) {
      if (isset($viewloc->address) && !gmaps_address_empty($viewloc->address)) {
        $viewloc_label = gmaps_address_format_plain($viewloc->address, 'iso3_name', 'both');
        $viewloc_info = ($viewloc->label ? $viewloc->label .' - ' : ''). t('View location');
      }
      else {
        $viewloc_label = 'view location';
      }
    }
  }
  
  foreach($type['fields'] as $field_name => $field) {
    if ($field['module'] == 'gmaps_content' && in_array($field['type'], array('gmaps_address', 'gmaps_point', 'gmaps_anp'))) {
      $gmaps = &$field['widget']['gmaps']['meta']['link'][$teaser ? 'teaser' : 'full'];
      $c = &$gmaps['content'];
      $d = &$gmaps['distance'];
      
      $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
      $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');
      
      if (!$gmaps_throttle) {
        $label = t($field['widget']['label']);
        $refloc_title = t('Distance from @label by @title', array('@label' => $refloc_label, '@title' => $label)) . ($refloc_info ? ' - '. $refloc_info : '');
        $geoip_title = t('Distance from @label by @title', array('@label' => $geoip_label, '@title' => $label)) .' - '. $geoip_info;
        $viewloc_title = t('Distance from @label by @title', array('@label' => $viewloc_label, '@title' => $label)) . ($viewloc_info ? ' - '. $viewloc_info : '');
      }
        
      if ($c['address'] || $c['point']) {
        if ($address) {
          $adr_s = $field['widget']['gmaps']['formatter']['address'];
          if ($adr_s['user_settings']) {
            $adr_s = $user->gmaps_settings['address'];
          }
          $adr_s['formatter'] = $c['address'];
        }
        else {
          $adr_s = array();
        }
        
        if ($point) {
          $p_s = $field['widget']['gmaps']['formatter']['point'];
          if ($p_s['user_settings']) {
            $p_s = $user->gmaps_settings['coordinate'];
          }
          $p_s['formatter'] = $c['point'];
        }
        else {
          $p_s = array();
        }
        
        if ($c['show'] == 'all') {
          foreach($node->{$field_name} as $delta => $value) {
            if ($address && ($adr = $value['value']->address) && !gmaps_address_empty($adr)) {
              $alinks[$delta] = gmaps_address_render($adr, $adr_s, TRUE);
            }
            if ($point && ($p = $value['value']->point) && !gmaps_point_empty($p)) {
              $plinks[$delta] = gmaps_coords_render($p, $p_s);
            }
          }
        }
        else {
          if ($c['show'] == 'first') {
            $delta = 0;
          }
          else if ($c['show'] == 'last') {
            $delta = count($node->{$field_name}) - 1;
          }
          else {
            $delta = $c['show'];
          }
           
          if ($address && ($adr = $node->{$field_name}[$delta]['value']->address) && !gmaps_address_empty($adr)) {
            $alinks[$delta] = gmaps_address_render($adr, $adr_s, TRUE);
          }
          if ($point && ($p = $node->{$field_name}[$delta]['value']->point) && !gmaps_point_empty($p)) {
            $plinks[$delta] = gmaps_coords_render($p, $p_s);
          }
        }
      }
      foreach($alinks as $delta => $value) {
        $delta = 'gmaps_content_field_address_'. $delta;
        $links[$delta] = array(
          'title' => $value,
          'html' => TRUE,
        );
        if (!$gmaps_throttle) {
          $links[$delta]['attributes'] = array('title' => $label, 'alt' => $label);
        }
      }
      foreach($plinks as $delta => $value) {
        $delta = 'gmaps_content_field_point_'. $delta;
        $links[$delta] = array(
          'title' => $value,
          'html' => TRUE,
        );
        if (!$gmaps_throttle) {
          $links[$delta]['attributes'] = array('title' => $label, 'alt' => $label);
        }
      }
      $alinks = $plinks = array();
      
      if (!$gmaps_throttle && $point && (($d['refloc'] && $refloc) || ($d['geoip'] && $geoip) || ($d['viewloc'] && $viewloc))) {
        if ($d['show'] == 'all') {
          foreach($node->{$field_name} as $delta => $value) {
            if (($p = $value['value']->point) && !gmaps_point_empty($p)) {
              if ($d['refloc'] && $refloc) {
                $dist = gmaps_coords_distance($p, $refloc->point);
                if ($dist) {
                  $rlinks[$delta] = gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
                }
              }
              
              if ($d['viewloc'] && $viewloc) {
                $dist = gmaps_coords_distance($p, $viewloc->point);
                if ($dist) {
                  $vlinks[$delta] = gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
                }
              }
              
              if ($d['geoip'] && $geoip) {
                $dist = gmaps_coords_distance($p, $geoip->point);
                if ($dist) {
                  $glinks[$delta] = gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
                }
              }
            }
          }
        }
        else if ($d['show'] == 'min' || $d['show'] == 'max') {
          $refloc_dist = $geoip_dist = $viewloc_dist = NULL;
          foreach($node->{$field_name} as $delta => $value) {
            if (($p = $value['value']->point) && !gmaps_point_empty($p)) {
              if ($d['refloc'] && $refloc) {
                $dist = gmaps_coords_distance($p, $refloc->point);
                if ($dist) {
                  $refloc_dist = is_null($refloc_dist) ? $dist : $d['show']($refloc_dist, $dist);
                }
              }
              
              if ($d['viewloc'] && $viewloc) {
                $dist = gmaps_coords_distance($p, $viewloc->point);
                if ($dist) {
                  $viewloc_dist = is_null($viewloc_dist) ? $dist : $d['show']($viewloc_dist, $dist);
                }
              }
              
              if ($d['geoip'] && $geoip) {
                $dist = gmaps_coords_distance($p, $geoip->point);
                if ($dist) {
                  $geoip_dist = is_null($geoip_dist) ? $dist : $d['show']($geoip_dist, $dist);
                }
              }
            }
          }
          if ($refloc_dist) {
            $rlinks[$d['show']] = gmaps_length_render(array('value' => $refloc_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
          }
          if ($viewloc_dist) {
            $vlinks[$d['show']] = gmaps_length_render(array('value' => $viewloc_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
          }
          if ($geoip_dist) {
            $glinks[$d['show']] = gmaps_length_render(array('value' => $geoip_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
          }
        }
        else {
          if ($d['show'] == 'first') {
            $delta = 0;
          }
          else if ($d['show'] == 'last') {
            $delta = count($node->{$field_name}) - 1;
          }
          else {
            $delta = $d['show'];
          }
          
          if (($p = $node->{$field_name}[$delta]['value']->point) && !gmaps_point_empty($p)) {
            if ($d['refloc'] && $refloc) {
              $dist = gmaps_coords_distance($p, $refloc->point);
              if ($dist) {
                $rlinks[$delta] = gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
              }
            }
            
            if ($d['viewloc'] && $viewloc) {
              $dist = gmaps_coords_distance($p, $viewloc->point);
              if ($dist) {
                $vlinks[$delta] = gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
              }
            }
            
            if ($d['geoip'] && $geoip) {
              $dist = gmaps_coords_distance($p, $geoip->point);
              if ($dist) {
                $glinks[$delta] = gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
              }
            }
          }
        }
        foreach($rlinks as $delta => $value) {
          $delta = 'gmaps_content_distance_refloc_'. $delta;
          $links[$delta] = array(
            'title' => $value,
            'html' => TRUE,
            'attributes' => array('title' => $refloc_title, 'alt' => $refloc_title),
          );
        }
        foreach($vlinks as $delta => $value) {
          $delta = 'gmaps_content_distance_viewloc_'. $delta;
          $links[$delta] = array(
            'title' => $value,
            'html' => TRUE,
            'attributes' => array('title' => $viewloc_title, 'alt' => $viewloc_title),
          );
        }
        foreach($glinks as $delta => $value) {
          $delta = 'gmaps_content_distance_geoip_'. $delta;
          $links[$delta] = array(
            'title' => $value,
            'html' => TRUE,
            'attributes' => array('title' => $geoip_title, 'alt' => $geoip_title),
          );
        }
        $rlinks = $glinks = $vlinks = array();
      }
    }
  }
  
  return $links;
}
