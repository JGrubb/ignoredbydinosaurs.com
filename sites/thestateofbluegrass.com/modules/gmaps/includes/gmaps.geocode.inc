<?php
// $Id: gmaps.geocode.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * Geocoder functions.
 */

function gmaps_geocode($query) {
  $key = _gmaps_get_api_key();
  if (empty($query) || empty($key)) {
    drupal_json(array('status' => FALSE));
    exit();
  }

  $request = '';
  //domain bias
  if (!empty($_GET['gl'])) {
    $request .= '&gl='. $_GET['gl'];
  }

  //viewport bias
  if (!empty($_GET['ll']) && !empty($_GET['spn'])) {
    $coords = new stdClass();
    list($coords->latitude, $coords->longitude) = explode(',', $_GET['ll']);
    $request .= '&ll='. gmaps_coords_format_dd_query($coords) .'&spn='. $_GET['spn'];
  }

  $request .= '&q='. urlencode($query);

  //check cache
  if ($cached = cache_get($request, 'cache_gmaps_geocode')) {
    $output = $cached->data;
  }
  else {
    $settings = gmaps_get_settings('gmaps_api_settings');

    $base_url = 'http://maps.google.'. $settings['domain'] .'/maps/geo?output=json&oe=utf8&sensor=false&hl=en&key='. $key;

    $result = drupal_http_request($base_url . $request);
    switch ($result->code) {
      case 304:
      case 301:
      case 200:
      case 302:
      case 307:
        $output = $result->data;
        if ($settings['geocode_lifetime']) {
          cache_set($request, $output, 'cache_gmaps_geocode',  time() + $settings['geocode_lifetime']);
        }
        break;
      default:
        $args = array('%error' => $result->code .' '. $result->error, '!url' => $base_url . $request);
        $output = t('The geocoder service seems to be broken, because of error "%error". Request: !url', $args);
        watchdog('gmaps geocode', 'The geocoder service seems to be broken, because of error "%error".', $args, WATCHDOG_ERROR, $base_url . $request);
        drupal_json(array('status' => FALSE, 'data' => $output));
        exit();
    }
  }

  drupal_set_header('Content-Type: text/javascript; charset=utf-8');
  echo $output;
  exit();
}
