<?php
// $Id: gmaps_content.token.inc,v 1.1.2.4 2009/10/16 11:17:01 xmarket Exp $

function _gmaps_content_token_list($type) {
  static $tokens;
  $at = $pt = $dt = array();

  if (!isset($tokens)) {
    $tokens = array();
    //distance tokens
    $dt['value'] = t('Distance value. WARNING - raw user input.');
    $dt['unit'] = t('Distance unit.');
    $dt['unit-code'] = t('Code of the distance unit.');
    $dt['unit-abbr'] = t('Abbreviation of the distance unit.');
    $dt['distance'] = t('Rendered distance.');
    $dt['distance-plain'] = t('Plain distance.');
    $dt['distance-themed'] = t('Themed distance.');
    
    //address tokens
    $at['country-code'] = t('Country part of the address: ISO 3166 alpha 2 code.');
    $at['country-code3'] = t('Country part of the address: ISO 3166 alpha 3 code.');
    $at['country'] = t('Country part of the address: english name.');
    $at['country-translated'] = t('Country part of the address: name in current language.');
    $at['adminarea'] = t('Administrative area part of the address.');
    $at['adminarea-raw'] = t('Unfiltered administrative area part of the address. WARNING - raw user input.');
    $at['adminarea-translated'] = t('Administrative area part of the address in current language.');
    $at['adminarea-code'] = t('Code of the administrative area part of the address. US and CA only.');
    $at['subadminarea'] = t('Sub-administrative area part of the address.');
    $at['subadminarea-raw'] = t('Unfiltered sub-administrative area part of the address. WARNING - raw user input.');
    $at['subadminarea-translated'] = t('Sub-dministrative area part of the address in current language.');
    $at['locality'] = t('Locality part of the address.');
    $at['locality-raw'] = t('Unfiltered locality part of the address. WARNING - raw user input.');
    $at['locality-translated'] = t('Locality part of the address in current language.');
    $at['deplocality'] = t('Dependent locality part of the address.');
    $at['deplocality-raw'] = t('Unfiltered dependent locality part of the address. WARNING - raw user input.');
    $at['deplocality-translated'] = t('dependent locality part of the address in current language.');
    $at['thoroughfare'] = t('Thoroughfare part of the address.');
    $at['thoroughfare-raw'] = t('Unfiltered thoroughfare part of the address. WARNING - raw user input.');
    $at['postalcode'] = t('Postal code part of the address.');
    $at['postalcode-raw'] = t('Unfiltered postal code part of the address. WARNING - raw user input.');
    $at['address'] = t('Rendered address.');
    $at['address-plain'] = t('Plain address.');
    $at['address-themed'] = t('Themed address.');
  
    //point tokens
    $pt['latitude'] = t('Latitude of the point in DMS (HTML).');
    $pt['latitude-plain'] = t('Latitude of the point in DMS (plain text).');
    $pt['latitude-raw'] = t('Latitude of the point in DD.');
    $pt['longitude'] = t('Longitude of the point in DMS (HTML).');
    $pt['longitude-plain'] = t('Longitude of the point in DMS (plain text).');
    $pt['longitude-raw'] = t('Longitude of the point in DD.');
    $pt['elevation'] = t('Rendered elevation.');
    $pt['elevation-plain'] = t('Plain elevation.');
    $pt['elevation-themed'] = t('Themed elevation.');
    $pt['elevation-raw'] = t('Raw elevation of the point.');
    $pt['x'] = t('X coordinate of the point.');
    $pt['y'] = t('Y coordinate of the point.');
    $pt['z'] = t('Z coordinate of the point.');
    $pt['ne-latitude'] = t('Latitude of the north-east bound in DMS (HTML).');
    $pt['ne-latitude-plain'] = t('Latitude of the north-east bound in DMS (plain text).');
    $pt['ne-latitude-raw'] = t('Latitude of the north-east bound in DD.');
    $pt['ne-longitude'] = t('Longitude of the north-east bound in DMS (HTML).');
    $pt['ne-longitude-plain'] = t('Longitude of the north-east bound in DMS (plain text).');
    $pt['ne-longitude-raw'] = t('Longitude of the north-east bound in DD.');
    $pt['sw-latitude'] = t('Latitude of the south-west bound in DMS (HTML).');
    $pt['sw-latitude-plain'] = t('Latitude of the south-west bound in DMS (plain text).');
    $pt['sw-latitude-raw'] = t('Latitude of the south-west bound in DD.');
    $pt['sw-longitude'] = t('Longitude of the south-west bound in DMS (HTML).');
    $pt['sw-longitude-plain'] = t('Longitude of the south-west bound in DMS (plain text).');
    $pt['sw-longitude-raw'] = t('Longitude of the south-west bound in DD.');
    $pt['point'] = t('Rendered point.');
    $pt['point-plain'] = t('Plain point.');
    $pt['point-themed'] = t('Themed point.');
    $pt['point-distance-refloc'] = t('Distance from the reference location. Empty when throttle active.');
    $pt['point-distance-geoip'] = t("Distance from the user's GeoIP location. Empty when throttle active.");
    
    foreach (content_fields() as $field_name => $field) {
      if ($field['type'] == 'gmaps_distance') {
        $temp = array();
        foreach($dt as $token => $desc) {
          $temp[$field_name .'-'. $token] = $desc;
        }
        if ($field['multiple']) {
          $temp += _gmaps_content_token_list_multiple($field_name, $dt);
        }
        $tokens['CCK gmaps distance'] = isset($tokens['CCK gmaps distance']) ? array_merge($tokens['CCK gmaps distance'], $temp) : $temp;
      }
      else if (in_array($field['type'], array('gmaps_address', 'gmaps_point', 'gmaps_anp'))) {
        $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
        $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');
        
        if ($address) {
          $temp = array();
          foreach($at as $token => $desc) {
            $temp[$field_name .'-'. $token] = $desc;
          }
          if ($field['multiple']) {
            $temp += _gmaps_content_token_list_multiple($field_name, $at);
          }
          $tokens['CCK gmaps address'] = isset($tokens['CCK gmaps address']) ? array_merge($tokens['CCK gmaps address'], $temp) : $temp;
        }
        
        if ($point) {
          $temp = array();
          foreach($pt as $token => $desc) {
            $temp[$field_name .'-'. $token] = $desc;
          }
          if ($field['multiple']) {
            $temp += _gmaps_content_token_list_multiple($field_name, $pt);
          }
          $pt['point-distance-refloc-min'] = $pt['point-distance-refloc'];
          $pt['point-distance-geoip-min'] = $pt['point-distance-geoip'];
          $tokens['CCK gmaps point'] = isset($tokens['CCK gmaps point']) ? array_merge($tokens['CCK gmaps point'], $temp) : $temp;
        }
      }
    }
  }
  
  return $tokens;
}

function _gmaps_content_token_values($type, $node, $options = array()) {
  global $gmaps_throttle;
  static $tokens = array();
  
  $id = md5(serialize($node));
  
  if (!isset($tokens[$id])) {
    if (!function_exists('_gmaps_length_format')) {
      module_load_include('inc', 'gmaps', 'includes/gmaps.length');
    }
    if (!function_exists('_gmaps_address_get_part_labels')) {
      module_load_include('inc', 'gmaps', 'includes/gmaps.address');
    }
    if (!function_exists('_gmaps_coords_get_line_end')) {
      module_load_include('inc', 'gmaps', 'includes/gmaps.coords');
    }
    $values = array();
    $type = content_types($node->type);
    foreach($type['fields'] as $field_name => $field) {
      if ($field['type'] == 'gmaps_distance') {
        if (isset($node->$field_name) && is_array($node->$field_name) && count($node->$field_name)) {
          $temp = array();
          if (($item = $node->{$field_name}[0]) && !gmaps_length_empty($item)) {
            $temp = _gmaps_content_token_values_distance($field, $item, '');
            
            if ($field['multiple'] && count($node->$field_name)) {
              $temp += _gmaps_content_token_values_multiple($field, $node->$field_name, $temp, '_gmaps_content_token_values_distance');
            }
            
            $values += $temp;
          }
        }
      }
      else if (in_array($field['type'], array('gmaps_address', 'gmaps_point', 'gmaps_anp'))) {
        if (isset($node->$field_name) && is_array($node->$field_name) && count($node->$field_name)) {
          $temp = array();
          $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
          $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');
          
          if ($item = $node->{$field_name}[0]) {
            if ($address && !gmaps_address_empty($item['value']->address)) {
              $temp = _gmaps_content_token_values_address($field, $item, '');
              
              if ($field['multiple'] && count($node->$field_name)) {
                $temp += _gmaps_content_token_values_multiple($field, $node->$field_name, $temp, '_gmaps_content_token_values_address');
              }
              
              $values += $temp;
            }

            if ($point && !gmaps_point_empty($item['value']->point)) {
              $temp = _gmaps_content_token_values_point($field, $item, '');
              
              if ($field['multiple'] && count($node->$field_name)) {
                $temp += _gmaps_content_token_values_multiple($field, $node->$field_name, $temp, '_gmaps_content_token_values_point');
              }
              
              $values += $temp;
            }
          }
          
          if (!$gmaps_throttle && $point) {
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
                $values[$field_name .'-point-distance-refloc-min'] = '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $refloc_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
              }
            }
            
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
                $values[$field_name .'-point-distance-geoip-min'] = '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $geoip_dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
                $added['geoip'] = TRUE;
              }
            }
          }
        }
      }
    }
    $tokens[$id] = $values;
  }
  
  return $tokens[$id];
}

function _gmaps_content_token_list_multiple($field_name, $base) {
  $tokens = array();
  
  foreach($base as $token => $desc) {
    $token = $field_name .'-'. $token;
    $tokens[$token .'-last'] = $desc;
    $tokens[$token .'-all'] = $desc;
    $tokens[$token .'-n'] = $desc .' ('. t('replace n in range 2 - 10.') .')';
  }
  
  return $tokens;
}

function _gmaps_content_token_values_multiple($field, $items, $first, $function) {
  $values = $all = array();
  
  foreach($first as $token => $value) {
    $all[$token] = array($value);
  }
  
  $last = count($items) - 1;
  
  foreach($items as $delta => $item) {
    if ($delta == 0) {
      continue;
    }
    $temp = $function($field, $item, $delta + 1);
    $values += $temp;
    
    if ($delta == $last) {
      $values += $function($field, $item, 'last');
    }
    
    foreach($first as $token => $value) {
      if ($value = $temp[$token .'-'. ($delta + 1)]) {
        $all[$token][] = $value;
      }
    }
  }
  
  foreach($all as $token => $tokens) {
    $values[$token .'-all'] = theme('item_list', $tokens);
  }
  
  return $values;
}

function _gmaps_content_token_values_distance($field, $item, $suffix = '') {
  global $user;
  $values = array();
  
  if (!($unit = gmaps_length_unit_load($item['unit']))) {
    return $values;
  }
  
  $s = $field['widget']['gmaps'];
  if ($s['user_settings']) {
    $s = $user->gmaps_settings['length'];
  }
    
  $prefix = $field['field_name'] .'-';
  $suffix = empty($suffix) ? '': '-'. $suffix;
    
  $values[$prefix .'value'. $suffix] = $item['value'];
  $values[$prefix .'unit'. $suffix] = format_plural($item['value'], $unit->title, $unit->title_plural);
  $values[$prefix .'unit-code'. $suffix] = $item['unit'];
  $values[$prefix .'unit-abbr'. $suffix] = _gmaps_length_unit_format_abbr($item['value'], $unit->abbr, $unit->title, $unit->title_plural);
  $values[$prefix .'distance'. $suffix] = _gmaps_length_render($item);
  $values[$prefix .'distance-plain'. $suffix] = _gmaps_length_format_plain($item, $s['precision']);
  $values[$prefix .'distance-themed'. $suffix] = _gmaps_length_format($item, $s['precision'], $s['multipart'], $s['optmode']);
  
  return $values;
}

function _gmaps_content_token_values_address($field, $item, $suffix = '') {
  global $user;
  $values = array();
  
  if (gmaps_address_empty($item['value']->address)) {
    return $values;
  }
  
  $a = (object)gmaps_extend(array(), $item['value']->address);
  _gmaps_address_apply_privacy($a);
  
  if (!($c = _gmaps_get_countries('country', $a->country))) {
    return $values;
  }
  
  $s = $field['widget']['gmaps']['formatter']['address'];
  if ($s['user_settings']) {
    $s = $user->gmaps_settings['address'];
  }
    
  $prefix = $field['field_name'] .'-';
  $suffix = empty($suffix) ? '': '-'. $suffix;
    
  $values[$prefix .'country-code'. $suffix] = $a->country;
  $values[$prefix .'country-code3'. $suffix] = $c['iso_alpha3'];
  $values[$prefix .'country'. $suffix] = $c['name'];
  $values[$prefix .'country-translated'. $suffix] = t($c['name']);
  
  $values[$prefix .'adminarea-raw'. $suffix] = $item['value']->address->adminarea;
  if (!empty($a->adminarea)) {
    $values[$prefix .'adminarea'. $suffix] = check_plain($a->adminarea);
    $values[$prefix .'adminarea-translated'. $suffix] = check_plain(_gmaps_address_format_state($a, 'name'));
    if (in_array($a->country, array('US', 'CA'))) {
      $values[$prefix .'adminarea-code'. $suffix] = _gmaps_address_format_state($a, 'code');
    }
  }
  
  $values[$prefix .'subadminarea-raw'. $suffix] = $item['value']->address->subadminarea;
  if (!empty($a->subadminarea)) {
    $values[$prefix .'subadminarea'. $suffix] = check_plain($a->subadminarea);
    $values[$prefix .'subadminarea-translated'. $suffix] = check_plain(t($a->subadminarea));
  }
  
  $values[$prefix .'locality-raw'. $suffix] = $item['value']->address->locality;
  if (!empty($a->locality)) {
    $values[$prefix .'locality'. $suffix] = check_plain($a->locality);
    $values[$prefix .'locality-translated'. $suffix] = check_plain(t($a->locality));
  }
  
  $values[$prefix .'deplocality-raw'. $suffix] = $item['value']->address->deplocality;
  if (!empty($a->deplocality)) {
    $values[$prefix .'deplocality'. $suffix] = check_plain($a->deplocality);
    $values[$prefix .'deplocality-translated'. $suffix] = check_plain(t($a->deplocality));
  }
  
  $values[$prefix .'thoroughfare-raw'. $suffix] = $item['value']->address->thoroughfare;
  if (!empty($a->thoroughfare)) {
    $values[$prefix .'thoroughfare'. $suffix] = check_plain($a->thoroughfare);
  }
  
  $values[$prefix .'postalcode-raw'. $suffix] = $item['value']->address->postalcode;
  if (!empty($a->postalcode)) {
    $values[$prefix .'postalcode'. $suffix] = check_plain($a->postalcode);
  }
  
  $values[$prefix .'address'. $suffix] = _gmaps_address_render($a);
  $values[$prefix .'address-plain'. $suffix] = _gmaps_address_format_plain($a, $s['country'], $s['state']);
  $values[$prefix .'address-themed'. $suffix] = _gmaps_address_format($a, $s['country'], $s['state'], $s['flag'], $s['taxonomy_links']);

  return $values;
}

function _gmaps_content_token_values_point($field, $item, $suffix = '') {
  global $user, $gmaps_throttle;
  $values = array();
  
  if (gmaps_point_empty($item['value']->point)) {
    return $values;
  }
  
  $p = (object)gmaps_extend(array(), $item['value']->point);
  _gmaps_coords_apply_privacy($p);
  
  $cart = _gmaps_coords_convert_latlng_to_cart($p);
  
  $coords = explode(',', _gmaps_coords_format_dd_query($item['value']->point));
  
  $s = $field['widget']['gmaps']['formatter']['point'];
  if ($s['user_settings']) {
    $s = $user->gmaps_settings['coordinate'];
  }
  
  $prefix = $field['field_name'] .'-';
  $suffix = empty($suffix) ? '': '-'. $suffix;
  
  $values[$prefix .'latitude'. $suffix] = check_plain(_gmaps_coords_format_dms_latitude($p->latitude, $s['dms_prec'], TRUE));
  $values[$prefix .'latitude-plain'. $suffix] = check_plain(_gmaps_coords_format_dms_latitude($p->latitude, $s['dms_prec']));
  $values[$prefix .'latitude-raw'. $suffix] = check_plain($coords[0]);

  $values[$prefix .'longitude'. $suffix] = check_plain(_gmaps_coords_format_dms_longitude($p->longitude, $s['dms_prec'], TRUE));
  $values[$prefix .'longitude-plain'. $suffix] = check_plain(_gmaps_coords_format_dms_longitude($p->longitude, $s['dms_prec']));
  $values[$prefix .'longitude-raw'. $suffix] = check_plain($coords[1]);

  if ($s['elevation'] == 2 || ($s['elevation'] && (float)$p->elevation)) {
    $e = array('value' => $p->elevation, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    $ls = $user->gmaps_settings['length'];
    $ls['precision'] = $s['elevation_prec'];
    $values[$prefix .'elevation'. $suffix] = _gmaps_length_render($e, $ls);
    $values[$prefix .'elevation-plain'. $suffix] = _gmaps_length_format_plain($e, $s['elevation_prec']);
    $values[$prefix .'elevation-themed'. $suffix] = _gmaps_length_format($e, $s['elevation_prec']);
  }
  $values[$prefix .'elevation-raw'. $suffix] = check_plain($p->elevation);
  
  if (!empty($cart)) {
    $values[$prefix .'x'. $suffix] = $cart['x'];
    $values[$prefix .'y'. $suffix] = $cart['y'];
    $values[$prefix .'z'. $suffix] = $cart['z'];
  }
  
  if (isset($p->bounds) && !empty($p->bounds)) {
    $nep = $p->bounds['northeast'];
    $nec = explode(',', _gmaps_coords_format_dd_query($item['value']->point->bounds['northeast']));
  
    $values[$prefix .'ne-latitude'. $suffix] = check_plain(_gmaps_coords_format_dms_latitude($nep->latitude, $s['dms_prec'], TRUE));
    $values[$prefix .'ne-latitude-plain'. $suffix] = check_plain(_gmaps_coords_format_dms_latitude($nep->latitude, $s['dms_prec']));
    $values[$prefix .'ne-latitude-raw'. $suffix] = check_plain($nec[0]);
  
    $values[$prefix .'ne-longitude'. $suffix] = check_plain(_gmaps_coords_format_dms_longitude($nep->longitude, $s['dms_prec'], TRUE));
    $values[$prefix .'ne-longitude-plain'. $suffix] = check_plain(_gmaps_coords_format_dms_longitude($nep->longitude, $s['dms_prec']));
    $values[$prefix .'ne-longitude-raw'. $suffix] = check_plain($nec[1]);

    $swp = $p->bounds['southwest'];
    $swc = explode(',', _gmaps_coords_format_dd_query($item['value']->point->bounds['southwest']));
  
    $values[$prefix .'ne-latitude'. $suffix] = check_plain(_gmaps_coords_format_dms_latitude($swp->latitude, $s['dms_prec'], TRUE));
    $values[$prefix .'ne-latitude-plain'. $suffix] = check_plain(_gmaps_coords_format_dms_latitude($swp->latitude, $s['dms_prec']));
    $values[$prefix .'ne-latitude-raw'. $suffix] = check_plain($swc[0]);
  
    $values[$prefix .'ne-longitude'. $suffix] = check_plain(_gmaps_coords_format_dms_longitude($swp->longitude, $s['dms_prec'], TRUE));
    $values[$prefix .'ne-longitude-plain'. $suffix] = check_plain(_gmaps_coords_format_dms_longitude($swp->longitude, $s['dms_prec']));
    $values[$prefix .'ne-longitude-raw'. $suffix] = check_plain($swc[1]);
  }

  $values[$prefix .'point'. $suffix] = _gmaps_coords_render($p);
  $values[$prefix .'point-plain'. $suffix] = _gmaps_coords_format_plain($p, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
  $values[$prefix .'point-themed'. $suffix] = _gmaps_coords_format($p, $s['format'], $s['elevation'], $s['dd_prec'], $s['dms_prec'], $s['cart_prec'], $s['elevation_prec']);
  
  if (!$gmaps_throttle) {
    if ($refloc = gmaps_get_reference_location()) {
      if ($dist = gmaps_coords_distance($p, $refloc->point)) {
        $refloc_label = (isset($refloc->address) && !gmaps_address_empty($refloc->address)) ? gmaps_address_format_plain($refloc->address, 'iso3_name', 'both') : 'reference location';
        $title = t('Distance from @label', array('@label' => $refloc_label));
        $values[$prefix .'point-distance-refloc'. $suffix] = '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
      }
    }

    $geoip = gmaps_get_geoip_location();
    if ($geoip) {
      if ($dist = gmaps_coords_distance($p, $geoip->point)) {
        $geoip_label = gmaps_address_format_plain($geoip->address, 'name', 'name');
        $title = t('Distance from @label', array('@label' => $geoip_label));
        $values[$prefix .'point-distance-geoip'. $suffix] = '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
      }
    }
  }

  return $values;
}
