<?php
// $Id: gmaps.coords.inc,v 1.1.2.6 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * Coordinate processing functions.
 */


function _gmaps_coords_get_line_end($coords, $bearing, $length) {
  if ($length['value'] <= 0) {
    return array('latitude' => $coords->latitude, 'longitude' => $coords->longitude);
  }

  if (!isset($coords->map_type) || !($type = gmaps_map_get_types('type', $coords->map_type)) || !isset($type->coords) || !($function = $type->coords['callbacks']['line_end'])) {
    return NULL;
  }

  if (isset($type->coords['file']) && !function_exists($function)) {
    module_load_include('inc', $type->module, $type->coords['file']);
  }

  if (!isset($type->coords['arguments'])) {
    $end = $function($coords, deg2rad($bearing), gmaps_length_convert($length));
  }
  else {
    $args = array($coords, deg2rad($bearing), gmaps_length_convert($length));
    if (is_array($type->coords['arguments'])) {
      $args = array_merge($args, $type->coords['arguments']);
    }
    else {
      $args[] = $type->coords['arguments'];
    }
    $end = call_user_func_array($function, $args);
  }
  
  if ($end) {
    if ($end[1] >= 180) {
      $end[1] -= 360;
    }
    else if ($end[1] <= -180) {
      $end[1] += 360;
    }
    return array('latitude' => $end[0], 'longitude' => $end[1]);
  }
  
  return NULL;
}

function _gmaps_coords_convert_latlng_to_cart($coords) {
  if (!isset($coords->map_type) || !($type = gmaps_map_get_types('type', $coords->map_type))) {
    return NULL;
  }
  
  if (isset($coords->x) && isset($coords->y) && isset($coords->z)) {
    return array('x' => $coords->x, 'y' => $coords->y, 'z' => $coords->z);
  }

  if (!isset($type->coords) || !($function = $type->coords['callbacks']['latlng_to_cart'])) {
    return NULL;
  }

  if (isset($type->coords['file']) && !function_exists($function)) {
    module_load_include('inc', $type->module, $type->coords['file']);
  }

  if (!isset($type->coords['arguments'])) {
    $cart = $function($coords);
  }
  else {
    $args = array($coords);
    if (is_array($type->coords['arguments'])) {
      $args = array_merge($args, $type->coords['arguments']);
    }
    else {
      $args[] = $type->coords['arguments'];
    }
    $cart = call_user_func_array($function, $args);
  }
  
  if ($cart) {
    return array('x' => $cart[0], 'y' => $cart[1], 'z' => $cart[2]);
  }
  
  return NULL;
}

function _gmaps_coords_map_radius($coords) {
  if (!isset($coords->map_type) || !($type = gmaps_map_get_types('type', $coords->map_type)) || !isset($type->coords) || !($function = $type->coords['callbacks']['radius'])) {
    return FALSE;
  }

  if (isset($type->coords['file']) && !function_exists($function)) {
    module_load_include('inc', $type->module, $type->coords['file']);
  }

  if (!isset($type->coords['arguments'])) {
    $r = $function($coords);
  }
  else {
    if (is_array($type->coords['arguments'])) {
      $args = $type->coords['arguments'];
    }
    else {
      $args = array($type->coords['arguments']);
    }
    $args = array_merge(array($coords), $args);
    $r = call_user_func_array($function, $args);
  }
  
  return empty($r) ? NULL : $r;
}

function _gmaps_coords_add_cart(&$coords) {
  $coords->x = $coords->y = $coords->z = NULL;
  $cart = _gmaps_coords_convert_latlng_to_cart($coords);
  if (!$cart) {
    $cart = array('x' => 0, 'y' => 0, 'z' => 0);
  }
  foreach($cart as $axis => $value) {
    $coords->$axis = $value;
  }
}

function _gmaps_coords_distance($coords1, $coords2) {
  //invalid input
  if (!isset($coords1->map_type) || !isset($coords2->map_type) || $coords1->map_type != $coords2->map_type || !($type = gmaps_map_get_types('type', $coords1->map_type))) {
    return NULL;
  }
  //same points
  if ($coords1 == $coords2) {
    return 0;
  }
  
  if (isset($type->coords) && ($function = $type->coords['callbacks']['distance'])) {
    if (isset($type->coords['file']) && !function_exists($function)) {
      module_load_include('inc', $type->module, $type->coords['file']);
    }
    if (!isset($type->coords['arguments'])) {
      $dist = $function($coords1, $coords2);
    }
    else {
      $args = array($coords1, $coords2);
      if (is_array($type->coords['arguments'])) {
        $args = array_merge($args, $type->coords['arguments']);
      }
      else {
        $args[] = $type->coords['arguments'];
      }
      $dist = call_user_func_array($function, $args);
    }
  }

  if (!isset($dist)) {
    $c1 = _gmaps_coords_convert_latlng_to_cart($coords1);
    $c2 = _gmaps_coords_convert_latlng_to_cart($coords2);
    if ($c1 && $c2) {
      $dist = sqrt(pow(($c2['x']-$c1['x']), 2) + pow(($c2['y']-$c1['y']), 2) + pow(($c2['z']-$c1['z']), 2));
      if ($dist > GMAPS_MAX_FAST_FILTER_DISTANCE) {
        $dist = NULL;
      }
    }
  }
  
  return $dist;
}

function _gmaps_coords_apply_privacy_bounds(&$coords, $end) {
  if (!empty($coords->bounds)) {
    $dlat = $end['latitude'] - $coords->latitude;
    $dlon = $end['longitude'] - $coords->longitude;
    
    $coords->bounds['southwest']->latitude += $dlat;
    $coords->bounds['northeast']->latitude += $dlat;
    
    $coords->bounds['southwest']->longitude += $dlon;
    $coords->bounds['northeast']->longitude += $dlon;
  }
}

/**
 * Apply coordinate privacy by reference.
 *
 * @param object $point
 *
 * @return null
 */
function _gmaps_coords_apply_privacy(&$coords) {
  global $user;

  if (isset($coords->protected)) {
    return;
  }
  //non-protected or self-owned or current user has access
  if (!isset($coords->privacy) || gmaps_length_empty($coords->privacy) || (!empty($coords->uid) && $coords->uid == $user->uid) || user_access('access protected gmaps data')) {
    $coords->protected = false;
    return;
  }

  //random bearing
  $bearing = mt_rand(0, 360);
  //random distance between privacy and 2*privacy
  $length = array('value' => mt_rand($coords->privacy['value'], 2*$coords->privacy['value']), 'unit' => $coords->privacy['unit']);

  $end = _gmaps_coords_get_line_end($coords, $bearing, $length);

  if ($end) {
    _gmaps_coords_apply_privacy_bounds($coords, $end);

    $coords->latitude = $end['latitude'];
    $coords->longitude = $end['longitude'];

    if (!empty($coords->elevation)) {
      $coords->elevation = $coords->elevation + $coords->elevation*(mt_rand(-20, 20)/100);
    }
    
    if (isset($coords->pov)) {
      unset($coords->pov);
    }
  }

  $coords->protected = true;
}

function _gmaps_coords_get_circle($center, $radius, $quality) {
  if (!($type_r = _gmaps_coords_map_radius($center))) {
    return FALSE;
  }
  
  $r = gmaps_length_convert($radius);
  if ($r === FALSE) {
    return FALSE;
  }
  $r = $r / $type_r;
  
  if ($quality < 5) {
    $q = 5;
  }
  else if ($quality > 360) {
    $q = 360;
  }
  else {
    $q = $quality;
  }
  $q = 360/$q;
  
  $circle = (object)array(
    'points' => array(),
    'bounds' => array(
      'northeast' => gmaps_extend(new stdClass(), $center),
      'southwest' => gmaps_extend(new stdClass(), $center),
    ),
  );

  $lat1 = deg2rad($center->latitude); // radians
  $lng1 = deg2rad($center->longitude); // radians

  for ($a = 0 ; $a <= 360 ; $a+=$q ) {
      $tc = deg2rad($a);
      $y = asin(sin($lat1)*cos($r)+cos($lat1)*sin($r)*cos($tc));
      $dlng = atan2(sin($tc)*sin($r)*cos($lat1),cos($r)-(sin($lat1)*sin($y)));
      $x = fmod(($lng1-$dlng+M_PI), (2*M_PI)) - M_PI; // MOD function
      $point = (object)array(
        'latitude' => rad2deg($y),
        'longitude' => rad2deg($x),
      );
      $circle->points[] = $point;
      _gmaps_coords_bounds_extend($circle->bounds, $point);
  }
  $circle->points[] = $circle->points[0];
  
  return $circle;
}

function _gmaps_coords_bounds($coords, $length, $external = TRUE) {
  $l = gmaps_length_convert($length, GMAPS_DEFAULT_LENGTH_UNIT);
  if (empty($l)) {
    return;
  }
  if ($external) {
    return _gmaps_coords_bounds_external($coords, $length, $l);
  }
  else {
    return _gmaps_coords_bounds_internal($coords, $length, $l);
  }
}

function _gmaps_coords_bounds_external($coords, $length, $l) {
  if ($coods->latitude >= 0) {
    $south = _gmaps_coords_get_line_end($coords, 180, $length);
  }
  else {
    $north = _gmaps_coords_get_line_end($coords, 0, $length);
  }
  if (empty($south) && empty($north)) {
    return;
  }
  
  $east = _gmaps_coords_get_line_end($coords, 90, $length);
  if (empty($east)) {
    return;
  }
  
  if ($south) {
    $sw = (object)array(
      'latitude' => $south['latitude'],
      'longitude' => $coords->longitude - ($east['longitude'] - $coords->longitude),
      'map_type' => $coords->map_type,
    );
    
    $ne = (object)array(
      'latitude' => $coords->latitude + ($coords->latitude - $south['latitude']),
      'longitude' => $east['longitude'],
      'map_type' => $coords->map_type,
    );
  }
  else {
    $ne = (object)array(
      'latitude' => $north['latitude'],
      'longitude' => $east['longitude'],
      'map_type' => $coords->map_type,
    );
    
    $sw = (object)array(
      'latitude' => $coords->latitude - ($north['latitude'] - $coords->latitude),
      'longitude' => $coords->longitude - ($east['longitude'] - $coords->longitude),
      'map_type' => $coords->map_type,
    );
  }
  if ($sw->longitude < -180) {
    $sw->longitude += 360;
  }
  else if ($sw->longitude > 180) {
    $sw->longitude -= 360;
  }
  
  return array('northeast' => $ne, 'southwest' => $sw);
}

function _gmaps_coords_bounds_internal($coords, $length, $l) {
  if ($coods->latitude >= 0) {
    if ($coords->longitude >= 0) {
      $sw = _gmaps_coords_get_line_end($coords, 225, $length);
    }
    else {
      $se = _gmaps_coords_get_line_end($coords, 135, $length);
    }
  }
  else {
    if ($coords->longitude >= 0) {
     $nw = _gmaps_coords_get_line_end($coords, 315, $length);
    }
    else {
     $ne = _gmaps_coords_get_line_end($coords, 45, $length);
    }
  }
  if (empty($sw) && empty($se) && empty($nw) && empty($ne)) {
    return;
  }
  
  if ($sw) {
    $ne = (object)array(
      'latitude' => $coords->latitude + ($coords->latitude - $sw['latitude']),
      'longitude' => $coords->longitude + ($coords->longitude - $sw['longitude']),
      'map_type' => $coords->map_type,
    );
    $sw = (object)$sw;
  }
  else if ($ne) {
    $sw = (object)array(
      'latitude' => $coords->latitude - ($ne['latitude'] - $coords->latitude),
      'longitude' => $coords->longitude - ($ne['longitude'] - $coords->longitude),
      'map_type' => $coords->map_type,
    );
    $ne = (object)$ne;
  }
  else if ($se) {
    $sw = (object)array(
      'latitude' => $se['latitude'],
      'longitude' => $coords->longitude - ($se['longitude'] - $coords->longitude),
      'map_type' => $coords->map_type,
    );
    
    $ne = (object)array(
      'latitude' => $coords->latitude + ($coords->latitude - $se['latitude']),
      'longitude' => $se['longitude'],
      'map_type' => $coords->map_type,
    );
  }
  //nw
  else {
    $ne = (object)array(
      'latitude' => $nw['latitude'],
      'longitude' => $coords->longitude + ($coords->longitude - $nw['longitude']),
      'map_type' => $coords->map_type,
    );
    
    $sw = (object)array(
      'latitude' => $coords->latitude - ($nw['latitude'] - $coords->latitude),
      'longitude' => $sw['longitude'],
      'map_type' => $coords->map_type,
    );
  }
  if ($ne->latitude > 90) {
    $ne->latitude = 90;
  }
  if ($ne->longitude > 180) {
    $ne->longitude -= 360;
  }
  
  if ($sw->latitude < -90) {
    $sw->latitude = -90;
  }
  if ($sw->longitude < -180) {
    $sw->longitude += 360;
  }
  
  return array('northeast' => $ne, 'southwest' => $sw);
}

function _gmaps_coords_bounds_extend(&$bounds, $coords) {
  $ne = &$bounds['northeast'];
  $sw = &$bounds['southwest'];
  
  if ($coords->latitude > $ne->latitude) {
    $ne->latitude = $coords->latitude;
  }
  else if ($coords->latitude < $sw->latitude) {
    $sw->latitude = $coords->latitude;
  }

  if ($coords->longitude > $ne->longitude) {
    $ne->longitude = $coords->longitude;
  }
  else if ($coords->longitude < $sw->longitude) {
    $sw->longitude = $coords->longitude;
  }
}

function _gmaps_coords_bounds_contains($bounds, $coords) {
  $ne = &$bounds['northeast'];
  $sw = &$bounds['southwest'];
  return ($ne->latitude >= $coords->latitude && $ne->longitude >= $coords->longitude && $sw->latitude <= $coords->latitude && $sw->longitude <= $coords->longitude);
}

function _gmaps_coords_bounds_span($bounds) {
  $ne = &$bounds['northeast'];
  $sw = &$bounds['southwest'];
  return (object)array(
    'latitude' => $ne->latitude - $sw->latitude,
    'longitude' => $ne->longitude - $sw->longitude,
  );
}

function _gmaps_coords_bounds_center($bounds) {
  $ne = &$bounds['northeast'];
  $sw = &$bounds['southwest'];
  $span = _gmaps_coords_bounds_span($bounds);
  return (object)array(
    'latitude' => $sw->latitude + ($span->latitude/2),
    'longitude' => $sw->longitude + ($span->longitude/2)
  );
}

function _gmaps_coords_path_bounds($points) {
  if (!empty($points)) {
    $p = array_shift($points);
    $b = array(
      'northeast' => gmaps_extend(new stdClass(), $p),
      'southwest' => gmaps_extend(new stdClass(), $p),
    );
    foreach($points as $p) {
      _gmaps_coords_bounds_extend($b, $p);
    }
  }
  
  return $b;
}

function _gmaps_coords_format_dms_coord($coord, $precision, $html = FALSE) {
  $suffix = $html ? array('&deg;', '&prime;', '&Prime;') : array('°', '′', '″');
  $coord = abs($coord);
  $deg = floor($coord);
  $min = ($coord - $deg) * 60;
  $sec = ($min - floor($min)) * 60;
  return $deg . $suffix[0] . floor($min) . $suffix[1] . _gmaps_number_format($sec, $precision) . $suffix[2];
}

function _gmaps_coords_format_dms_latitude($coord, $precision, $html = FALSE) {
  $pos = (abs($coord) == $coord);
  $arg = array('!coord' => _gmaps_coords_format_dms_coord($coord, $precision, $html));
  return $pos ? t('!coord N', $arg) : t('!coord S', $arg);
}

function _gmaps_coords_format_dms_longitude($coord, $precision, $html = FALSE) {
  $pos = (abs($coord) == $coord);
  $arg = array('!coord' => _gmaps_coords_format_dms_coord($coord, $precision, $html));
  return $pos ? t('!coord E', $arg) : t('!coord W', $arg);
}

function _gmaps_coords_format_plain($coordinates, $format = NULL, $elevation = NULL, $dd_prec = NULL, $dms_prec = NULL, $cart_prec = NULL, $elevation_prec = NULL) {
  global $user;

  $coords = (object)gmaps_extend(array(), $coordinates);
  _gmaps_coords_apply_privacy($coords);

  if (gmaps_point_empty($coords) && !($coords->latitude == '0' && $coords->longitude == '0' && $coords->elevation == '0')) {
    return '';
  }

  $settings = &$user->gmaps_settings['coordinate'];
  $format = is_null($format) ? $settings['format'] : $format;

  $result = array();
  if ($format == 'dd') {
    $dd_prec = is_null($dd_prec) ? $settings['dd_prec'] : $dd_prec;
    $result['lat'] = _gmaps_number_format($coords->latitude, $dd_prec) .'°';
    $result['lng'] = _gmaps_number_format($coords->longitude, $dd_prec) .'°';
  }
  else if ($format == 'dms') {
    $dms_prec = is_null($dms_prec) ? $settings['dms_prec'] : $dms_prec;
    $result['lat'] = _gmaps_coords_format_dms_latitude($coords->latitude, $dms_prec);
    $result['lng'] = _gmaps_coords_format_dms_longitude($coords->longitude, $dms_prec);
  }
  else if ($format == 'cart') {
    $cart_prec = is_null($cart_prec) ? $settings['cart_prec'] : $cart_prec;
    $cart = _gmaps_coords_convert_latlng_to_cart($coords);
    if ($cart) {
      foreach ($cart as $axis => $value) {
        $value = array('value' => $value, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
        $result[$axis] = gmaps_length_format_plain($value, $cart_prec);
      }
    }
    else {
      return '';
    }
  }

  $elevation = is_null($elevation) ? $settings['elevation'] : $elevation;
  if ($format != 'cart' && ($elevation == 2 || ($elevation && !gmaps_floats_equal($coords->elevation, 0)))) {
    $elevation_prec = is_null($elevation_prec) ? $settings['elevation_prec'] : $elevation_prec;
    $elevation = array('value' => $coords->elevation, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    $result['h'] = gmaps_length_format_plain($elevation, $elevation_prec);
  }

  $nbsp = "\xC2\xA0";
  foreach($result as $key => $value) {
    $result[$key] = str_replace(' ', $nbsp, $value);
  }

  return check_plain(theme('gmaps_coords_plain', $result));
}

/**
 * Format latitude and longitude to use in URL.
 *
 * @param $coordinates
 * @param mixed $names
 *  String: 'latlng', 'latlon', 'full'(latitude,longitude)
 *  Linear array of prefixes
 *
 * @return string
 */
function _gmaps_coords_format_dd_query($coordinates, $names = FALSE, $separator = ',') {
  $coords = (object)gmaps_extend(array(), $coordinates);
  _gmaps_coords_apply_privacy($coords);
  
  if (gmaps_point_empty($coords) && !($coords->latitude == '0' && $coords->longitude == '0')) {
    return '';
  }

  if ($names) {
    if ($names == 'latlng') {
      $names = array('lat', 'lng');
    }
    else if ($names == 'latlon') {
      $names = array('lat', 'lon');
    }
    else if ($names == 'full') {
      $names = array('latitude', 'longitude');
    }
  }

  $coords->latitude = ($names ? $names[0] .'=':'') . check_plain(round($coords->latitude * 1e6) / 1e6);
  $coords->longitude = ($names ? $names[1] .'=':'') . check_plain(round($coords->longitude * 1e6) / 1e6);

  return $coords->latitude . ($names ? '&':$separator) . $coords->longitude;
}

function _gmaps_coords_format($coordinates, $format = NULL, $elevation = NULL, $dd_prec = NULL, $dms_prec = NULL, $cart_prec = NULL, $elevation_prec = NULL) {
  global $user;

  $coords = (object)gmaps_extend(array(), $coordinates);
  _gmaps_coords_apply_privacy($coords);

  if (gmaps_point_empty($coords) && !($coords->latitude == '0' && $coords->longitude == '0' && $coords->elevation == '0')) {
    return '';
  }

  $settings = &$user->gmaps_settings['coordinate'];
  $format = is_null($format) ? $settings['format'] : $format;

  $result = array();

  $elevation = is_null($elevation) ? $settings['elevation'] : $elevation;
  if ($format != 'cart' && ($elevation == 2 || ($elevation && (float)$coords->elevation))) {
    $elevation_prec = is_null($elevation_prec) ? $settings['elevation_prec'] : $elevation_prec;
    $elevation_settings = array_merge($user->gmaps_settings['length'], array('precision' => $elevation_prec));
    $elevation = array('value' => $coords->elevation, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    $result['h'] = gmaps_length_render($elevation, $elevation_settings);
  }

  list($coords->latitude, $coords->longitude) = explode(',', _gmaps_coords_format_dd_query($coords));
  $coords->elevation = round($coords->elevation);

  if ($format == 'dd' || $format == 'cart') {
    $dd_prec = is_null($dd_prec) ? $settings['dd_prec'] : $dd_prec;
    $result['lat'] = _gmaps_number_format($coords->latitude, $dd_prec) .'&deg;';
    $result['lng'] = _gmaps_number_format($coords->longitude, $dd_prec) .'&deg;';
    if ($format == 'dd') {
      return theme('gmaps_coords', $result, $coords);
    }
  }
  if ($format == 'dms') {
    $dms_prec = is_null($dms_prec) ? $settings['dms_prec'] : $dms_prec;
    $result['lat'] = _gmaps_coords_format_dms_latitude($coords->latitude, $dms_prec, TRUE);
    $result['lng'] = _gmaps_coords_format_dms_longitude($coords->longitude, $dms_prec, TRUE);
    return theme('gmaps_coords', $result, $coords, TRUE);
  }
  else if ($format == 'cart') {
    $cart_prec = is_null($cart_prec) ? $settings['cart_prec'] : $cart_prec;
    $cart = _gmaps_coords_convert_latlng_to_cart($coords);
    if ($cart) {
      foreach ($cart as $axis => $value) {
        $value = array('value' => $value, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
        $result[$axis] = gmaps_length_format($value, $cart_prec, FALSE);
      }
      return theme('gmaps_coords_cart', $result, $coords);
    }
    else {
      return '';
    }
  }

  return '';
}

function _gmaps_coords_render($coords, $settings = NULL) {
  global $user, $gmaps_throttle;

  $settings = is_null($settings) ? $user->gmaps_settings['coordinate'] : $settings;
  if ($gmaps_throttle) {
    $settings['formatter'] = 'plain';
    $settings['format'] = 'dd';
    $settings['elevation'] = 0;
  }

  if ($settings['formatter'] == 'plain') {
    if ($output = _gmaps_coords_format_plain($coords, $settings['format'], $settings['elevation'], $settings['dd_prec'], $settings['dms_prec'], $settings['cart_prec'], $settings['elevation_prec'])) {
      return '<span class="gmaps-coords-plain">'. $output .'</span>';
    }
    return '';
  }
  else {
    return _gmaps_coords_format($coords, $settings['format'], $settings['elevation'], $settings['dd_prec'], $settings['dms_prec'], $settings['cart_prec'], $settings['elevation_prec']);
  }
}

function _gmaps_coords_rss_item($coords, $type = 'simple') {
  if (gmaps_point_empty($coords) && !($coords->latitude == '0' && $coords->longitude == '0' && $coords->elevation == '0')) {
    return FALSE;
  }

  if ($type == 'simple') {
    return array(
      'key'       => 'georss:point',
      'namespace' => array('xmlns:georss' => 'http://www.georss.org/georss'),
      'value'     => _gmaps_coords_format_dd_query($coords, FALSE, ' '),
    );
  }
  else if ($type == 'gml') {
    return array(
      'key' => 'georss:where',
      'namespace' => array(
        'xmlns:georss' => 'http://www.georss.org/georss',
        'xmlns:gml' => 'http://www.opengis.net/gml',
      ),
      'value' => array(
        'gml:Point' => array(
          'gml:pos' => _gmaps_coords_format_dd_query($coords, FALSE, ' '),
        ),
      ),
    );
  }
  else if ($type == 'w3c') {
    $c = explode(',', _gmaps_coords_format_dd_query($coords));
    return array(
      'key'       => 'geo:Point',
      'namespace' => array('xmlns:geo' => 'http://www.w3.org/2003/01/geo/wgs84_pos#'),
      'value'     => array(
        array('key' => 'geo:lat', 'value' => $c[0]),
        array('key' => 'geo:long', 'value' => $c[1]),
      ),
    );
  }
  
  return FALSE;
}
