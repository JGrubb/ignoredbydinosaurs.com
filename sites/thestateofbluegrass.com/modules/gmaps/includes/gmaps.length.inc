<?php
// $Id: gmaps.length.inc,v 1.1.2.5 2009/08/20 06:15:10 xmarket Exp $

/**
 * @file
 * Length processing functions.
 *
 * IMPORTANT NOTES ON USABILITY AND PRECISION
 *
 * PHP has a limited support for float numbers. Especially string-float corversation,
 * comparision, and numbers containing one of the digits 1, 3 and 7 has problems.
 *
 * Numbers containing the above digits can change during math operations,
 * because of internal rounding issues.
 * Very large and very small numbers can change during string-float conversions.
 *
 * Some examples:
 *   print_r(floatval((64.1*100)-6410)); //-9.09494701773E-13
 *   print_r(64.1 == 6410/100) ? 'yes' : 'no in PHP'); //yes
 *   print_r((64.1*100 == 6410) ? 'yes' : 'no in PHP'); //no in PHP
 *   print_r((round(64.1*100) == 6410) ? 'yes' : 'no in PHP'); //yes
 *
 * Because of these limitations, length processing has some restriction:
 * - If a conversation results in a too small or too large number, then result is ignored.
 * - Parsing of too small and too large numbers is disabled.
 *
 * See GMAPS_MAX_CONVERTED_NUMBER and GMAPS_MAX_PRECISION.
 */

/********************
 * Public functions *
 ********************/

/**
 * Format a length.
 *
 * @param $array
 *  (required) A length array. Keys: 'value', 'unit'
 * @param $precision
 *  (optional) Precision. Defaults to user/site precision setting.
 * @param $multiple
 *  (optional) Allow splitting multipart units into multiple parts or not. Defaults to TRUE.
 * @param $optmode
 *  (optional) INTERNAL!! Used by gmaps_length_render().
 *
 * @return
 *  The themed length.
 *
 * Always use gmaps_length_render(), except when the diplayed unit is important.
 */
function _gmaps_length_format($array, $precision = NULL, $multiple = TRUE, $optmode = NULL) {
  global $user;

  if (!($unit = gmaps_length_unit_load($array['unit']))) {
    return '';
  }

  $precision = is_null($precision) ? $user->gmaps_settings['length']['precision'] : $precision;
  $optmode = isset($optmode) ? $optmode : 0;

  //multipart
  if ($unit->group['multipart'] && $multiple) {
    $decimals = abs($array['value']) - floor(abs($array['value']));
    if ($decimals > 0) {
      $sign = (substr($array['value'], 0, 1) == '-') ? -1 : 1;
      $test = array('value' => $decimals, 'unit' => $array['unit']);
      $length = ($optmode == 0) ? _gmaps_length_convert_integer($test) : _gmaps_length_optimize($test, $optmode);
      //decimals of part 1 are converted
      if ($test != $length) {
        $parts[0] = array(
          'value' => _gmaps_number_format($sign * floor(abs($array['value'])), 0),
          'unit' => &$array['unit'],
        );
        $part2_length = $length['value'];
        $decimals = $length['value'] - floor($length['value']);
        if ($decimals > 0) {
          $test = array('value' => $decimals, 'unit' => $length['unit']);
          $length = ($optmode == 0) ? _gmaps_length_convert_integer($test) : _gmaps_length_optimize($test, $optmode);
          //decimals of part 2 are converted
          if ($test != $length) {
            $formatted = _gmaps_number_format($length['value'], $precision);
            $decimals = ($formatted > 0);
            $parts[1] = array(
              'value' => _gmaps_number_format(intval($part2_length), $decimals ? 0 : $precision),
              'unit' => $test['unit'],
            );
            if ($decimals) {
              $parts[2] = array(
                'value' => $formatted,
                'unit' => $length['unit'],
              );
            }
          }
          //no conversion occured to decimals of part 2
          else if (($formatted = _gmaps_number_format($part2_length, $precision)) && $formatted > 0) {
            $parts[1] = array(
              'value' => $formatted,
              'unit' => $test['unit'],
            );
          }
        }
        //no decimals on part 2
        else {
          $parts[1] = array(
            'value' => _gmaps_number_format($length['value'], $precision),
            'unit' => $length['unit'],
          );
        }
      }
      //no conversion occured to decimals of part 1
      else {
        $parts[0] = array(
          'value' => _gmaps_number_format($array['value'], $precision),
          'unit' => &$array['unit'],
        );
      }
    }
    //no decimals on part 1
    else {
      $parts[0] = array(
        'value' => _gmaps_number_format($array['value'], $precision),
        'unit' => &$array['unit'],
      );
    }

    foreach($parts as $i => $part) {
      $unit = gmaps_length_unit_load($part['unit']);

      $unit_value = _gmaps_length_unit_format_abbr($part['value'], $unit->abbr, $unit->title, $unit->title_plural);

      $parts[$i] = theme(array('gmaps_length__'.$unit->group['key'], 'gmaps_length'), check_plain($part['value']), $unit_value, $unit);
    }

    return theme(array('gmaps_length_multipart__'.$unit->group['key'], 'gmaps_length_multipart'), $parts, $unit->group['key']);
  }
  //single part
  else {
    $length = _gmaps_number_format($array['value'], $precision);

    $unit_value = _gmaps_length_unit_format_abbr($array['value'], $unit->abbr, $unit->title, $unit->title_plural);

    return theme(array('gmaps_length__'.$unit->group['key'], 'gmaps_length'), check_plain($length), $unit_value, $unit);
  }
}

/**
 * Format a length as a plain text.
 *
 * @param $length
 *  (required) A length array. Keys: 'value', 'unit'
 * @param $precision
 *  (optional) Precision. Defaults to user/site precision setting.
 *
 * @return
 *  The formatted length string.
 */
function _gmaps_length_format_plain($length, $precision = NULL) {
  global $user;

  if (!($unit = gmaps_length_unit_load($length['unit']))) {
    return '';
  }

  $precision = is_null($precision) ? $user->gmaps_settings['length']['precision'] : $precision;

  $formatted = _gmaps_number_format($length['value'], $precision);
  $unit_value = _gmaps_length_unit_format_abbr($length['value'], $unit->abbr_option, $unit->title, $unit->title_plural);

  //force "nowrap"
  return str_replace(' ', "\xC2\xA0", check_plain($formatted .' '. $unit_value));
}

/**
 * Parse a string into a length array.
 *
 * @param $formatted
 *  (required) The string to parse.
 *
 * @return
 *  The length array.
 *
 * Supports negative values and multiple parts.
 * Example: "103mi -3 mi" = array('value' => 100, 'unit' => 'en_mile').
 */
function _gmaps_length_parse($formatted) {
  $formatted = trim($formatted);
  $formatted = str_replace("\xC2\xA0", ' ', $formatted);

  //separate parts by '|'
  $formatted = preg_replace('#(\D\w?|\D{3,})\s+?(\d|\+|-)#u', '$1|$2', $formatted);

  //remove extra spaces
  $formatted = preg_replace('#\s+?\|#u', '|', $formatted);
  $formatted = preg_replace('#(\d)\s+#u', '$1', $formatted);

  $parts = preg_split('#\|#', $formatted, -1, PREG_SPLIT_NO_EMPTY);
  foreach($parts as $i => $subunit) {
    //add one space among value and unit
    $subunit = preg_replace('#(\d)(\D\w?$|\D{3,})#', '$1 $2', $subunit);
    $parts[$i] = is_null($subunit) ? NULL : explode(' ', $subunit, 2);
  }
  $parts = array_filter($parts);

  $base_unit = NULL;
  $length = NULL;
  foreach($parts as $i => $part) {
    $value = _gmaps_number_parse($part[0]);
    if (!empty($value)) {
      if (is_null($base_unit)) {
        $base_unit = _gmaps_length_unit_key_for_abbr($part[1]);
        if (!$base_unit) {
          return FALSE;
        }
      }
      if (is_null($length)) {
        $length = array('value' => $value, 'unit' => $base_unit);
      }
      else {
        $value = array('value' => $value, 'unit' => _gmaps_length_unit_key_for_abbr($part[1]));
        if ($value['unit']) {
          $value = _gmaps_length_convert($value, $base_unit);
          if ($value !== FALSE) {
            $length['value'] += $value;
          }
        }
      }
    }
  }

  return is_null($length) ? FALSE : $length;
}

/**
 * Convert a length from one unit to another.
 *
 * @param $length
 *  (required) A length array. Keys: 'value', 'unit'
 * @param $to
 *  (required) The new unit.
 *
 * @return
 *  The converted value, or FLASE in case of invalid units.
 */
function _gmaps_length_convert($length, $to = GMAPS_DEFAULT_LENGTH_UNIT) {
  //basic checks, nothing to do
  if ($length['unit'] == $to) {
    return $length['value'];
  }
  if ($length['value'] == 0) {
    return 0;
  }

  $units = gmaps_length_get_units();
  $unit_keys = array_keys($units);
  if (!in_array($length['unit'], $unit_keys) || !in_array($to, $unit_keys)) {
    return FALSE;
  }

  $from_unit = &$units[$length['unit']];
  $to_unit = &$units[$to];
  //convert using group rate
  if (isset($from_unit['group']['basis']) && isset($to_unit['group']['basis']) && ($from_unit['group']['basis'] == $from_unit['group']['basis']) && isset($from_unit['rate']) && isset($to_unit['rate'])) {
    //convert to basis
    $converted = $length['value'] * $from_unit['rate'];
    //covert from basis to $to
    if ($to != $from_unit['group']['basis']) {
      $converted = $converted / $to_unit['rate'];
    }
  }
  //convert using m rate
  else {
    //convert to meter(default)
    $converted = $length['value'] * $units[$length['unit']]['m'];
    //covert from meter to $to
    if ($to != GMAPS_DEFAULT_LENGTH_UNIT) {
      $converted = $converted / $units[$to]['m'];
    }
  }
  $rounded = round($converted, GMAPS_MAX_PRECISION);
  if ($converted >= GMAPS_MAX_CONVERTED_NUMBER || (abs($converted) < 1 && ($rounded <= GMAPS_CONVERSION_EPSYLON))) {return FALSE;}

  return $rounded;
}

/**
 * Optimize a length within its unit group.
 *
 * @param $length
 *  (required) A length array. Keys: 'value', 'unit'
 * @param $optmode
 *  (optional) Optimization mode. Defaults to user/site optimization settings.
 *
 * @return
 *  The optimized length array, or FALSE in case of invalid units.
 */
function _gmaps_length_optimize($length, $optmode = NULL) {
  global $user;
  static $rates = array();

  $optmode = is_null($optmode) ? $user->gmaps_settings['length']['optmode'] : $optmode;

  if (!($unit = gmaps_length_unit_load($length['unit']))) {
    return FALSE;
  }

  if (!$optmode || !$unit->group['optimize']) {
    return $length;
  }

  if (!isset($rates[$unit->group['key']])) {
    foreach(gmaps_length_get_units() as $key => $info) {
      if ($info['group']['key'] == $unit->group['key'] && $info['optimize']) {
        $rates[$unit->group['key']][] = $info;
      }
    }
  }

  $sign = ($length['value'] == abs($length['value'])) ? 1 : -1;
  //use ingroup rate if possible
  if (isset($unit->group['basis'])) {
    $basis = array('value' => abs(_gmaps_length_convert($length, $unit->group['basis'])), 'unit' => $unit->group['basis']);
  }
  else {
    $basis = array('value' => abs(_gmaps_length_convert($length, GMAPS_DEFAULT_LENGTH_UNIT)), 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
  }

  //if $length['unit'] is on the optimizable list, we do not need to walk through on the entire list.
  $index = NULL;
  foreach ($rates[$unit->group['key']] as $i => $info) {
    if ($info['key'] == $unit->key) {
      $index = $i;
      break;
    }
  }

  $optimized = NULL;

  //optimize for integer digits
  if ($optmode == 1) {
    //try to convert to a greater unit
    if ($length['value'] > 1) {
      $index = is_null($index) ? 0 : $index;
      while(($optimized > 1 || is_null($optimized)) && $rates[$unit->group['key']][$index]['key']) {
        if (!is_null($optimized)) {
          $previous = $optimized;
        }
        $optimized = _gmaps_length_convert($basis, $rates[$unit->group['key']][$index]['key']);
        $index++;
      }
      $index--;
      if (!$optimized || $optimized < 1) {
        $optimized = $previous;
        $index--;
      }
    }
    //try to convert to a smaller unit
    else {
      $index = is_null($index) ? count($rates[$unit->group['key']]) - 1 : $index;
      while(($optimized < 1 || is_null($optimized)) && $rates[$unit->group['key']][$index]['key']) {
        $optimized = _gmaps_length_convert($basis, $rates[$unit->group['key']][$index]['key']);
        $index--;
      }
      $index++;
    }
  }
  //optimize for decimal digits
  else {
    $pos = strpos($length['value'], '.');
    $rate_count = count($rates[$unit->group['key']]) - 1;
    //try to convert to a greater unit
    if ($pos === FALSE) {
      $index = is_null($index) ? 0 : $index;
      while($pos === FALSE && $index <= $rate_count && $rates[$unit->group['key']][$index]['key']) {
        $optimized = _gmaps_length_convert($basis, $rates[$unit->group['key']][$index]['key']);
        $index++;
        $pos = strpos($optimized, '.');
      }
      $index--;
    }
    //try to convert to a smaller unit
    else {
      $previous = $length['value'];
      $index = is_null($index) ? $rate_count : $index;
      while($pos !== FALSE && $index >= 0 && $rates[$unit->group['key']][$index]['key']) {
        if (!is_null($optimized)) {
          $previous = $optimized;
        }
        $optimized = _gmaps_length_convert($basis, $rates[$unit->group['key']][$index]['key']);
        $index--;
        $pos = strpos($optimized, '.');
      }
      $index++;
      if (!$optimized || $pos === FALSE) {
        $index++;
        $optimized = $previous;
      }
    }
  }
  
  return is_null($optimized) ? $length : array('value' => $sign * $optimized, 'unit' => $rates[$unit->group['key']][$index]['key']);
}

/**
 * Convert a length lower than 1 to the first lower unit where value is greater than or equal to one.
 *
 * @param $length
 *  (required) A length array. Keys: 'value', 'unit'
 *
 * @return
 *  The converted length array, or FLASE in case of invalid unit.
 */
function _gmaps_length_convert_integer($length) {
  static $rates = array();

  //nothing to do
  if (abs($length['value']) >= 1) {
    return $length;
  }

  //invalid unit
  if (!($unit = gmaps_length_unit_load($length['unit']))) {
    return FALSE;
  }

  if (!isset($rates[$unit->group['key']])) {
    foreach(gmaps_length_get_units() as $key => $info) {
      if ($info['group']['key'] == $unit->group['key']) {
        $rates[$unit->group['key']][] = $info;
      }
    }
  }

  $sign = ($length['value'] == abs($length['value'])) ? 1 : -1;
  //use ingroup rate if possible
  if (isset($unit->group['basis'])) {
    $basis = array('value' => abs(_gmaps_length_convert($length, $unit->group['basis'])), 'unit' => $unit->group['basis']);
  }
  else {
    $basis = array('value' => abs(_gmaps_length_convert($length, GMAPS_DEFAULT_LENGTH_UNIT)), 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
  }

  foreach ($rates[$unit->group['key']] as $index => $info) {
    if ($info['key'] == $unit->key) {
      break;
    }
  }

  $optimized = NULL;

  while(($optimized < 1 || is_null($optimized)) && $index > 0) {
    $index--;
    $optimized = _gmaps_length_convert($basis, $rates[$unit->group['key']][$index]['key']);
  }

  return is_null($optimized) ? $length : array('value' => $sign * $optimized, 'unit' => $rates[$unit->group['key']][$index]['key']);
}

/**
 * Apply length settings to the passed-in length and format it.
 *
 * @param $length
 *  (required) A length array. Keys: 'value', 'unit'
 * @param $settings
 *  (optional) Length settings. Defaults to user/site settings.
 *
 * @return
 *  The themed length.
 *
 * Basically this is the end-user length formatter function.
 * If the displayed unit is important, use gmaps_length_format().
 */
function _gmaps_length_render($length, $settings = NULL) {
  global $user, $gmaps_throttle;

  $settings = is_null($settings) ? $user->gmaps_settings['length'] : $settings;
  if ($gmaps_throttle) {
    $settings['unit'] = NULL;
    $settings['formatter'] = 'plain';
  }

  if (is_null($settings['unit'])) {
    $display = $length;
  }
  else {
    $display = _gmaps_length_convert($length, $settings['unit']->key);
    if ($display === FALSE) {
      $display = $length;
    }
    else {
      $display = array('value' => $display, 'unit' => $settings['unit']->key);
      $test = _gmaps_length_optimize($display, $settings['optmode']);
      if ($test !== FALSE) {
        $display = $test;
      }
    }
  }

  if ($settings['formatter'] == 'plain') {
    return '<span class="gmaps-length-plain">'. _gmaps_length_format_plain($display, $settings['precision']) .'</span>';
  }
  else {
    return _gmaps_length_format($display, $settings['precision'], $settings['multipart'], $settings['optmode']);
  }
}

/**********************
 * Internal functions *
 **********************/

/**
 * Helper function to format a length unit abbreviation.
 */
function _gmaps_length_unit_format_abbr($value, $abbr = NULL, $title = '', $title_plural = '') {
    //abbreviation
    return is_null($abbr) ? check_plain(format_plural($value, $title, $title_plural)) : $abbr;
}

/**
 * Helper function for abbr to unit conversion.
 *
 * @param abbr
 *  A unit abbreviation.
 *
 * @return
 *  The key of the unit or NULL in case of invalid abbreviation.
 */
function _gmaps_length_unit_key_for_abbr($abbr) {
  static $abbrs = NULL;

  if (is_null($abbrs)) {
    foreach(gmaps_length_get_units() as $key => $info) {
      //fallback to title
      $abbrs[isset($info['abbr_option']) ? $info['abbr_option'] : $info['title']] = $key;
    }
  }

  return $abbrs[$abbr];
}

