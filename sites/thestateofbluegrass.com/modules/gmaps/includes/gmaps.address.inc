<?php
// $Id: gmaps.address.inc,v 1.1.2.9 2009/10/30 16:58:06 xmarket Exp $

/**
 * @file
 * Address processing functions.
 */


/**
 * Helper function for configurable address detail field labels.
 *
 * @param $reset
 *  (optional) Reset the cached values or not.
 *
 * @return
 *  An array of labels in english, keyed by the parts.
 */
function _gmaps_address_get_part_labels() {
  static $result = array();

  if (empty($result)) {
    $labels = variable_get('gmaps_address_part_labels', array());

    foreach(_gmaps_address_get_parts() as $part => $label) {
      $result[$part] = (isset($labels[$part]) && !empty($labels[$part])) ? $labels[$part] : $label;
    }
  }

  return $result;
}

function _gmaps_address_has_subpart($adr, $from) {
  $check = FALSE;
  
  foreach(_gmaps_address_get_parts() as $part => $albel) {
    if ($check && !empty($adr->$part)) {
      return TRUE;
    }
    
    if ($part == $from) {
      $check = TRUE;
    }
  }
  
  return FALSE;
}

/**
 * Helper function for address privacy.
 *
 * @return array
 *
 */
function _gmaps_address_privacy_levels() {
  static $levels = NULL;

  if (is_null($levels)) {
    $levels = _gmaps_address_get_part_labels();
    $levels = array_values(array_reverse($levels));
  }

  return $levels;
}

function _gmaps_address_accuracy_levels() {
  static $levels = NULL;

  if (is_null($levels)) {
    $levels = array(t('Not required'),
      t('Country level'),
      t('Town (city, village) level'),
      t('Street, address level'),
    );
  }

  return $levels;
}

function _gmaps_get_countries($op = 'countries', $country = NULL, $reset = FALSE) {
  static $countries, $names, $iso3;

  if ($reset || !isset($countries)) {
    $allowed = variable_get('gmaps_address_allowed_countries', array());
    foreach (geonames_countries() as $c) {
      if (empty($allowed) || isset($allowed[$c['iso_alpha2']])) {
        $countries[$c['iso_alpha2']] = $c;
        $names[$c['iso_alpha2']] = t($c['name']);
        $iso3[$c['iso_alpha2']] = $c['iso_alpha3'];
      }
    }

    if ($names) {
      asort($names);
    }
  }

  if (!empty($country)) {
    if (is_object($country)) {
      $iso2 = $country->iso_alpha2;
    }
    elseif (is_string($country)) {
      $iso2 = $country;
    }
    elseif (is_array($country)) {
      $iso2 = $country['iso_alpha2'];
    }
    if (!isset($countries[$iso2])) {
      return FALSE;
    }
  }

  switch ($op) {
    case 'countries':
      return $countries;
    case 'country':
      return $countries[$iso2];
    case 'names':
      return $names;
    case 'name':
      return $names[$iso2];
    case 'all_iso3':
      return $iso3;
    case 'iso3':
      return $iso3[$iso2];
    default:
      return FALSE;
  }
}

/**
 * Return Info about Canadian Provinces.
 * Canadian counterpart of geonames_us_states().
 */
function _gmaps_canadian_provinces($province = NULL) {
  $provinces = array(
    'AB' => 'Alberta',
    'BC' => 'British Columbia',
    'MB' => 'Manitoba',
    'NB' => 'New Brunswick',
    'NL' => 'Newfoundland and Labrador',
    'NS' => 'Nova Scotia',
    'NT' => 'Northwest Territories',
    'NU' => 'Nunavut',
    'ON' => 'Ontario',
    'PE' => 'Prince Edward Island',
    'QC' => 'Quebec',
    'SK' => 'Saskatchewan',
    'YT' => 'Yukon Territory',
  );
  return ($province) ? $provinces[$province] : $provinces;
}

/**
 * Apply address privacy by reference.
 *
 * @param object $adr
 *
 * @return null
 */
function _gmaps_address_apply_privacy(&$adr) {
  global $user;
  static $map = NULL;

  if (isset($adr->protected)) {
    return;
  }
  //non-protected or self-owned or current user has access
  if (empty($adr->privacy) || (!empty($adr->uid) && $adr->uid == $user->uid) || user_access('access protected gmaps data')) {
    $adr->protected = false;
    return;
  }

  if (is_null($map)) {
    $map = _gmaps_address_get_parts();
    $map = array_keys(array_reverse($map));
  }

  foreach($map as $i => $part) {
    if ($i < $adr->privacy) {
      unset($adr->$part);
    }
  }

  $adr->protected = true;
}

/**
 * Format the country part of an address.
 *
 * @param string $country
 * @param string $format
 *
 * @return string
 */
function _gmaps_address_format_country($country, $format = NULL) {
  global $user;

  if (empty($country)) {
    return $country;
  }

  $format = is_null($format) ? $user->gmaps_settings['address']['country'] : $format;

  if (empty($format)) {
    return $country;
  }

  $name = _gmaps_get_countries('name', $country);
  if ($format == 'name') {
    return $name;
  }
  else {
    if (strpos($format, 'iso3') !== FALSE) {
      $country = _gmaps_get_countries('iso3', $country);
    }
    if (strpos($format, '_') !== FALSE) {
      return theme('gmaps_adr_country', $country, $name);
    }
    return $country;
  }
}

/**
 * Format the adminarea part of US and Canadian addresses.
 *
 * @param object $adr
 * @param string $format
 *
 * @return string
 */
function _gmaps_address_format_state($adr, $format = NULL) {
  global $user;

  $format = is_null($format) ? $user->gmaps_settings['address']['state'] : $format;

  if (empty($adr->adminarea)) {
    return '';
  }
  else if (empty($format) || !in_array($adr->country, array('US', 'CA'))) {
    return t($adr->adminarea);
  }
  
  if ($adr->country == 'US') {
    $states = geonames_us_states();
  }
  else {
    $states = _gmaps_canadian_provinces();
  }

  if (strlen($adr->adminarea) == 2) {
    $code = strtoupper($adr->adminarea);
    $name = isset($states[$code]) ? $states[$code] : '';
  }
  else {
    $name = drupal_ucfirst($adr->adminarea);
    $code = strtolower($name);
    $states = array_map('strtolower', $states);
    $states = array_flip($states);
    $code = isset($states[$code]) ? $states[$code] : '';
  }

  if ($format == 'code') {
    return empty($code) ? t($adr->adminarea) : $code;
  }
  else if ($format == 'name') {
    return empty($name) ? t($adr->adminarea) : t($name);
  }
  //both
  else {
    return theme('gmaps_adr_state', $code, t($name), t($adr->adminarea));
  }
}

function _gmaps_address_translate(&$adr) {
  $parts = _gmaps_address_get_part_labels();
  unset($parts['country'], $parts['adminarea'], $parts['postalcode'], $parts['thoroughfare']);
  
  foreach(array_keys($parts) as $part) {
    if (!empty($adr->$part)) {
      $adr->$part = t($adr->$part);
    }
  }
}

function _gmaps_address_check(&$adr) {
  if (isset($adr->checked)) {
    return;
  }
  $parts = _gmaps_address_get_part_labels();
  
  foreach(array_keys($parts) as $part) {
    if (!empty($adr->$part)) {
      $adr->$part = check_plain($adr->$part);
    }
  }
  
  $adr->checked = TRUE;
}

function _gmaps_address_format_plain($address, $country = NULL, $state = NULL, $link_context = FALSE) {
  if (gmaps_address_empty($address)) {
    return '';
  }

  $adr = drupal_clone($address);
  _gmaps_address_apply_privacy($adr);

  $country_code = strtolower($adr->country);

  $adr->adminarea = _gmaps_address_format_state($adr, $state);
  $adr->country = _gmaps_address_format_country($adr->country, $country);
  
  _gmaps_address_translate($adr);
  _gmaps_address_check($adr);
  
  if ($link_context) {
    return theme('gmaps_adr_link_plain', $adr);
  }
  else {
    $templates = array();
    if (!empty($country_code)) {
      $templates[] = 'gmaps_adr_plain__'. $country_code;
    }
    $templates[] = 'gmaps_adr_plain';
  
    return theme($templates, $adr);
  }
}

function _gmaps_address_url_args($adr, $last = NULL) {
  $arg = $adr->country;
  $args = array(
    'country' => $arg,
  );
  
  $parts = _gmaps_address_get_parts();
  unset($parts['country']);
  
  foreach($parts as $part => $label) {
    $set = (!is_null($adr->$part) && $adr->$part !== '');
    $enc = $set ? gmaps_address_part_urlencode($adr->$part) : '';
    $arg .= ','. ($set ? $enc : '');
    $args[$part] = $arg;
    
    if ($part == $last) {
      break;
    }
  }
  
  return $args;
}

function _gmaps_address_url_arg_parse($arg) {
  $args = explode(',', $arg);
  $country = array_shift($args);
  if (!($country = _gmaps_address_get_country_code(gmaps_address_part_urldecode($country)))) {
    return;
  }
  $parts = array_keys(_gmaps_address_get_parts());
  array_shift($parts);
  
  $adr = (object)array(
    'country' => $country,
    'last_part' => empty($args) ? 'country' : $parts[count($args) - 1],
  );
  
  foreach($parts as $i => $part) {
    $adr->$part = gmaps_address_part_urldecode($args[$i]);
  }
  
  $adr->adminarea = _gmaps_address_get_adminarea_code($adr->adminarea, $adr->country);
  
  return $adr;
}

function _gmaps_address_get_country_code($str) {
  if (empty($str)) {
    return;
  }

  $str = strtoupper($str);
  
  if (strlen($str) == 2) {
    $iso3 = _gmaps_get_countries('all_iso3');
    return array_key_exists($str, $iso3) ? $str : NULL;
  }
  else if (strlen($str) == 3) {
    if ($idx = array_search($str, _gmaps_get_countries('all_iso3'))) {
      return $idx;
    }
  }
  //name or translated name
  else {
    foreach(_gmaps_get_countries() as $c) {
      if (strtoupper($c['name']) == $str || strtoupper(t($c['name'])) == $str) {
        return $c['iso_alpha2'];
      }
    }
  }
}

/*
 * Try to normalize adminarea string to state/province code in US/CA
 */
function _gmaps_address_get_adminarea_code($str, $country) {
  if (empty($str) || !in_array($country, array('US', 'CA'))) {
    return $str;
  }
  
  $str = strtoupper($str);
  
  if (strlen($str) == 2) {
    return $str;
  }
  else {
    if ($country == 'US') {
      $states = geonames_us_states();
    }
    else {
      $states = _gmaps_canadian_provinces();
    }

    $states = array_map('strtoupper', $states);
    $states = array_flip($states);
    
    if (array_key_exists($str, $states)) {
      return $states[$str];
    }
    else {
      $str = strtr($str, ' ', '');
      $states = array('NCAROLINA' => 'NC', 'SCAROLINA' => 'SC', 'NDAKOTA' => 'ND', 'SDAKOTA' => 'SD');
      if (array_key_exists($str, $states)) {
        return $states[$str];
      }
    }
  }
  
  return $str;
}

function _gmaps_address_set_location_links(&$links, $adr) {
  if (empty($adr->country) || !module_exists('gmaps_taxonomy')) {
    return;
  }
  
  foreach(gmaps_taxonomy_location_pathes($adr) as $part => $path) {
    if (isset($links->$part)) {
      $links->{$part}['href'] = $path;
    }
  }
}

function _gmaps_address_format_country_flag($title, $country, $format = NULL) {
  if (empty($country) || (!($ci = module_exists('countryicons')) && !module_exists('languageicons'))) {
    return $title;
  }

  $format = is_null($format) ? $user->gmaps_settings['address']['flag'] : $format;

  if (empty($format)) {
    return $title;
  }

  if ($ci) {
    $flag = theme('countryicons_icon', strtolower($country), 'shiny', $title, $title);
  }
  else {
    $c = _gmaps_get_countries('country', $country);
    $languages = explode(',', strtolower($c['languages']));
    $language = new stdClass();
    foreach($languages as $lang) {
      $language->language = NULL;
      if (file_exists(variable_get('languageicons_path', drupal_get_path('module', 'languageicons') .'/flags/'. $lang .'.png'))) {
        $language->language = $lang;
      }
      else if (strpos($lang, '-') !== FALSE) {
        $lang = explode('-', $lang);
        $lang = $lang[0];
        if (file_exists(variable_get('languageicons_path', drupal_get_path('module', 'languageicons') .'/flags/'. $lang .'.png'))) {
          $language->language = $lang;
        }
      }
      if (!is_null($language->language)) {
        $flag = theme('languageicons_icon', $language, $title);
        if ($flag) {
          break;
        }
      }
    }
  }

  if ($flag) {
    switch($format) {
      case 'before':
        $title = $flag .' '. check_plain($title);
        break;
      case 'after':
        $title = check_plain($title) .' '. $flag;
        break;
      default:
        $title = $flag;
    }
  }

  return $title;
}

function _gmaps_address_format($address, $country = NULL, $state = NULL, $flag = NULL, $taxonomy_links = FALSE, $link_context = FALSE) {
  if (gmaps_address_empty($address)) {
    return '';
  }

  $adr = drupal_clone($address);
  _gmaps_address_apply_privacy($adr);

  //classes for adr microformat
  //See: http://microformats.org/wiki/adr
  $adr_mf_classes = array(
    'country' => ' country-name',
    'adminarea' => ' region',
    'locality' => ' locality',
    'postalcode' => ' postal-code',
    'thoroughfare' => ' street-address',
  );

  $links = new stdClass();
  foreach(_gmaps_address_get_parts() as $part => $label) {
    if (!empty($adr->$part)) {
      $links->$part = array(
        'title' => $adr->$part,
        'attributes' => array(
          'class' => 'gmaps-adr-'. $part . (isset($adr_mf_classes[$part]) ? ' '. $adr_mf_classes[$part] : '')
        ),
      );
    }
  }


  if ($taxonomy_links) {
    _gmaps_address_set_location_links($links, $adr);
  }

  $links->adminarea['title'] = _gmaps_address_format_state($adr, $state);
  $links->country['title'] = _gmaps_address_format_country($adr->country, $country);

  $title = _gmaps_address_format_country_flag($links->country['title'], $adr->country, $flag);
  if ($links->country['title'] != $title) {
    $links->country['html'] = TRUE;
  }
  $links->country['title'] = $title;

  if ($link_context) {
    return theme('gmaps_adr_link', $links, $adr);
  }
  else {
    $templates = array();
    if (!empty($adr->country)) {
      $templates[] = 'gmaps_adr__'. strtolower($adr->country);
    }
    $templates[] = 'gmaps_adr';
  
    return theme($templates, $links, $adr);
  }
}

function _gmaps_address_render($adr, $settings = NULL, $link_context = FALSE) {
  global $user, $gmaps_throttle;

  $settings = is_null($settings) ? $user->gmaps_settings['address'] : $settings;
  if ($gmaps_throttle) {
    $settings['formatter'] = 'plain';
    $settings['country'] = 'name';
    $settings['state'] = '';
    $settings['flag'] = '';
    $settings['taxonomy_links'] = FALSE;
  }

  if ($settings['formatter'] == 'plain') {
    if ($output = _gmaps_address_format_plain($adr, $settings['country'], $settings['state'], $link_context)) {
      if ($link_context) {
        return '<span class="gmaps-adr-link-plain">'. $output .'</span>';
      }
      else {
        return '<span class="gmaps-adr-plain">'. $output .'</span>';
      }
    }
    return '';
  }
  else {
    return _gmaps_address_format($adr, $settings['country'], $settings['state'], $settings['flag'], $settings['taxonomy_links'], $link_context);
  }
}

function _gmaps_address_prepare_record(&$adr) {
  $parts = _gmaps_address_get_parts();
  unset($parts['country']);
  
  $adr->country = _gmaps_address_get_country_code($adr->country);
  if (empty($adr->country)) {
    $adr->country = '';
  }

  $adr->adminarea = _gmaps_address_get_adminarea_code($adr->adminarea, $adr->country);

  foreach($parts as $part => $label) {
    if (empty($adr->$part)) {
      $adr->$part = NULL;
      $adr->{'u_'. $part} = NULL;
    }
    else {
      $adr->{'u_'. $part} = strtoupper($adr->$part);
    }
  }
}
