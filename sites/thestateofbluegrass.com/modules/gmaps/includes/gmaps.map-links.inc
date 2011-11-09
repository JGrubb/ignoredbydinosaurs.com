<?php
// $Id: gmaps.map-links.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * Links to Google, Yahoo! and MapQuest services
 */


function _gmaps_map_link_google_find($object, $label = NULL, $isAdr = FALSE) {
  $prefix = 'http://maps.google.com?q=';

  if ($isAdr) {
    $query = urlencode(gmaps_address_format_plain($object, 'name', ''));
  }
  else {
    $ll = gmaps_coords_format_dd_query($object);
    $query = $ll . (empty($label) ? '' : '('. urlencode($label) .')') .'&ll='. $ll;
  }

  return $prefix . $query;
}

function _gmaps_map_link_google_directions($object, $to = FALSE, $label = NULL, $isAdr = FALSE) {
  $prefix = 'http://maps.google.com?'. ($to ? 'daddr=' : 'saddr=');

  if ($isAdr) {
    $query = urlencode(gmaps_address_format_plain($object, 'name', ''));
  }
  else {
    $query = gmaps_coords_format_dd_query($object);
    if (!empty($label)) {
      $query .= '('. urlencode($label) .')';
    }
  }

  return $prefix . $query;
}

function _gmaps_map_link_format_mapquest_adr_query($adr, $prefices) {
  $query = '';
  if (!empty($adr->locality)) {
    $query .= '&'. $prefices[0] .'='. urlencode($adr->locality);
  }
  if (!empty($adr->adminarea) && ($code = _gmaps_call_func('gmaps', 'address', '_gmaps_address_format_state', array($adr, 'code'))) &&
      strlen($code) == 2) {
    $query .= '&'. $prefices[1] .'='. urlencode($code);
  }
  if (!empty($adr->thoroughfare)) {
    $query .= '&'. $prefices[2] .'='. urlencode($adr->thoroughfare);
  }
  if (!empty($adr->postalcode)) {
    $query .= '&'. $prefices[3] .'='. urlencode($adr->locality);
  }
  if (!empty($adr->country)) {
    $query .= '&'. $prefices[4] .'='. urlencode($adr->country);
  }
  $query = substr($query, 1);

  return $query;
}

function _gmaps_map_link_mapquest_find($object, $label = NULL, $isAdr = FALSE) {
  $prefix = 'http://www.mapquest.com/maps?';

  if ($isAdr) {
    $query = _gmaps_map_link_format_mapquest_adr_query($object, array('city', 'state', 'address', 'zipcode', 'country'));
  }
  else {
    $query = 'latlongtype=decimal&'. gmaps_coords_format_dd_query($object, 'full');
  }

  return $prefix . $query . (empty($label) ? '' : '&title='. urlencode($label)) .'&cid=lfmaplink';
}

function _gmaps_map_link_mapquest_directions($object, $to = FALSE) {
  $prefix = 'http://www.mapquest.com/directions?';

  $query = _gmaps_map_link_format_mapquest_adr_query($object, $to ? array('2c', '2s', '2a', '2z', '2y') : array('1c', '1s', '1a', '1z', '1y'));

  return $prefix . $query .'&cid=lfddlink';
}

function _gmaps_map_link_yahoo_find($object, $label = NULL, $isAdr = FALSE) {
  $prefix = 'http://maps.yahoo.com/map?';

  if ($isAdr) {
    $query = 'q1='. urlencode(gmaps_address_format_plain($object, 'name', ''));
  }
  else {
    $query = gmaps_coords_format_dd_query($object, 'latlon') . (empty($label) ? '' : '&q1='. urlencode($label));
  }

  return $prefix . $query;
}

function _gmaps_map_link_yahoo_directions($object, $to = FALSE, $isAdr = FALSE) {
  $prefix = 'http://maps.yahoo.com/map?'. ($to ? 'q2=' : 'q1=');

  if ($isAdr) {
    $query = urlencode(gmaps_address_format_plain($object, 'name', ''));
  }
  else {
    $query = gmaps_coords_format_dd_query($object);
  }

  return $prefix . $query;
}

function _gmaps_anp_map_links($anp, $fmt) {
  $address = property_exists($anp, 'address') && !empty($anp->address);
  $point = property_exists($anp, 'point') && !empty($anp->point);
  
  if (!$address && !$point) {
    return FALSE;
  }
  
  $links = array();
  
  $label = ($point && $address) ? gmaps_address_format_plain($anp->address, 'name') : NULL;
  
  if ($fmt['google_find']) {
    $links[] = l(t('Find on Google Maps'), _gmaps_map_link_google_find($point ? $anp->point : $anp->address, $label, !$point));
  }
  if ($fmt['yahoo_find']) {
    $links[] = l(t('Find on Yahoo! Maps'), _gmaps_map_link_yahoo_find($point ? $anp->point : $anp->address, $label, !$point));
  }
  if ($fmt['mapquest_find']) {
    $links[] = l(t('Find on Mapquest'), _gmaps_map_link_mapquest_find($point ? $anp->point : $anp->address, $label, !$point));
  }
  
  if ($fmt['google_directions_from'] && $fmt['google_directions_to']) {
    $links[] = theme('gmaps_google_directions', $point ? $anp->point : $anp->address, $label, !$point);
  }
  else if ($fmt['google_directions_from']) {
    $links[] = l(t('Google directions from here'), _gmaps_map_link_google_directions($point ? $anp->point : $anp->address, FALSE, $label, !$point));
  }
  else if ($fmt['google_directions_to']) {
    $links[] = l(t('Google directions to here'), _gmaps_map_link_google_directions($point ? $anp->point : $anp->address, TRUE, $label, !$point));
  }
  
  if ($fmt['yahoo_directions_from'] && $fmt['yahoo_directions_to']) {
    $links[] = theme('gmaps_yahoo_directions', $point ? $anp->point : $anp->address, !$point);
  }
  else if ($fmt['yahoo_directions_from']) {
    $links[] = l(t('Yahoo! directions from here'), _gmaps_map_link_yahoo_directions($point ? $anp->point : $anp->address, FALSE, !$point));
  }
  else if ($fmt['yahoo_directions_to']) {
    $links[] = l(t('Yahoo! directions to here'), _gmaps_map_link_yahoo_directions($point ? $anp->point : $anp->address, TRUE, !$point));
  }
  
  if ($address) {
    if ($fmt['mapquest_directions_from'] && $fmt['mapquest_directions_to']) {
      $links[] = theme('gmaps_mapquest_directions', $anp->address);
    }
    else if ($fmt['mapquest_directions_from']) {
      $links[] = l(t('Mapquest directions from here'), _gmaps_map_link_mapquest_directions($anp->address, FALSE));
    }
    else if ($fmt['mapquest_directions_to']) {
      $links[] = l(t('Mapquest directions to here'), _gmaps_map_link_mapquest_directions($anp->address, TRUE));
    }
  }
  
  return empty($links) ? FALSE : $links;
}

