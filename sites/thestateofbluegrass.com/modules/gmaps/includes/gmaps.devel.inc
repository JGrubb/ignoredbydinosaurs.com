<?php
// $Id: gmaps.devel.inc,v 1.1.2.1 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Data generator functions.
 */

/**
 * Generate length data.
 */
function _gmaps_generate_distance($negative = FALSE) {
  $length = array();
  $units = array_keys(gmaps_length_get_units());
  
  $length['value'] = $negative ? mt_rand(-5000, 5000) : mt_rand(0, 5000);
  $length['unit'] = $units[mt_rand(0, count($units) - 1)];
  
  return $length;
}

/**
 * Generate address data.
 */
function _gmaps_generate_address($privacy = FALSE, $countries = array()) {
  static $accounts;
  
  $adr = new stdClass();
  $adr->uid = 0; 
  
  $all = gmaps_get_countries('names');
  $allowed = array();
  if ($countries) {
    foreach($countries as $country) {
      if (isset($all[$country])) {
        $allowed[] = $country;
      }
    }
  }
  else {
    $allowed = array_keys($all);
  }
  if (empty($allowed)) {
    return;
  }
  
  $adr->country = $countries[mt_rand(0, count($countries) - 1)];
  
  if (mt_rand(0, 3)) {
    if ($adr->country == 'US') {
      $states = array_keys(geonames_us_states());
      $adr->adminarea = $states[mt_rand(0, count($states) - 1)];
    }
    else if ($adr->country == 'CA') {
      $states = array_keys(gmaps_canadian_provinces());
      $adr->adminarea = $states[mt_rand(0, count($states) - 1)];
    }
    else {
      $adr->adminarea = drupal_ucfirst(user_password(mt_rand(5, GMAPS_ADDRESS_PART_MAXLENGTH)));
    }
    if (mt_rand(0, 1)) {
      $adr->subadminarea = _gmaps_generate_address_part();
    }
  }
  
  if (mt_rand(0, 4)) {
    $adr->locality = _gmaps_generate_address_part();
    if (mt_rand(0,2)) {
      $adr->deplocality = _gmaps_generate_address_part();
    }
    if (mt_rand(0,3)) {
      $adr->postalcode = user_password(mt_rand(3, 16));
      $places = array('Street', 'St', 'Square', 'Road', 'Avenue', 'Boulevard', 'Circus', 'Beach');
      $name = function_exists('devel_create_greeking') ? devel_create_greeking(mt_rand(1,3), TRUE) : ucfirst(user_password(mt_rand(10,60)));
      $adr->thoroughfare = trim(substr(mt_rand(1, 500) ." $name ". $places[mt_rand(0, count($places) - 1)], 0, 255));
    }
  }
  
  if ($privacy) {
    if (is_null($accounts)) {
      $result = db_query('SELECT uid FROM {users} WHERE uid > 0');
      while ($account = db_fetch_object($result)) {
        $accounts[] = $account->uid;
      }
    }
    $adr->privacy = mt_rand(0, 6);
    $adr->uid = $accounts[mt_rand(0, count($accounts) - 1)];
  }
  
  return $adr;
}

function _gmaps_generate_address_part() {
  if (function_exists('devel_create_para')) {
    $part = trim(substr(devel_create_para(mt_rand(2,6)), 0, GMAPS_ADDRESS_PART_MAXLENGTH));
  }
  else {
    $text = ucfirst(user_password(mt_rand(8, 24)));
    if (mt_rand(0, 1)) {
      $text .= ' '. ucfirst(user_password(mt_rand(3, 5)));
      if (mt_rand(6, 15)) {
        $text .= ' '. ucfirst(user_password(mt_rand(3, 5)));
      }
    }
    $part = trim(substr($text, 0, GMAPS_ADDRESS_PART_MAXLENGTH));
  }
  
  return $part;
}

/**
 * Generate point data.
 */
function _gmaps_generate_point($map_type, $bounds = TRUE, $pov = FALSE, $elevation = FALSE, $privacy = FALSE) {
  static $accounts;
  
  if (!($type = gmaps_map_get_types('type', $map_type))) {
    return;
  }
  
  $point = (object)array(
    'latitude' => mt_rand(-85000, 85000) / 1000,
    'longitude' => mt_rand(-175000, 175000) / 1000,
    'elevation' => ($type->{'3D_enabled'} && $elevation) ? mt_rand(-90, 8000) : 0,
    'map_type' => $map_type,
    'uid' => 0,
  );
  
  if ($bounds) {
    $point->bounds = array(
      'northeast' => (object)array(
        'latitude' => $point->latitude + (mt_rand(10, 4000) / 1000),
        'longitude' => $point->longitude + (mt_rand(10, 4000) / 1000),
        'elevation' => 0,
        'map_type' => $map_type,
      ),
      'southwest' => (object)array(
        'latitude' => $point->latitude - (mt_rand(10, 4000) / 1000),
        'longitude' => $point->longitude - (mt_rand(10, 4000) / 1000),
        'elevation' => 0,
        'map_type' => $map_type,
      ),
    );
  }
  
  if ($pov) {
    $point->pov = array(
      'yaw' => mt_rand(0, 360),
      'pitch' => mt_rand(-90, 90),
      'zoom' => mt_rand(0, 2),
    );
  }
  
  if ($privacy) {
    if (is_null($accounts)) {
      $result = db_query('SELECT uid FROM {users} WHERE uid > 0');
      while ($account = db_fetch_object($result)) {
        $accounts[] = $account->uid;
      }
    }
    $point->privacy = _gmaps_generate_distance(FALSE);
    $point->uid = $accounts[mt_rand(0, count($accounts) - 1)];
  }
  
  return $point;
}

/**
 * Generate address and point data.
 */
function _gmaps_generate_anp($bounds = TRUE, $pov = FALSE, $elevation = FALSE, $privacy = FALSE, $countries = array()) {
  $anp = (object)array(
    'address' => _gmaps_generate_address($privacy, $countries),
    'point' => _gmaps_generate_point(GMAPS_MAP_EARTH, $bounds, $pov, $elevation, $privacy),
  );
  
  if (gmaps_anp_empty($anp)) {
    return;
  }

  $anp->uid = $anp->address->uid = $anp->point->uid;
  
  return $anp;
}
