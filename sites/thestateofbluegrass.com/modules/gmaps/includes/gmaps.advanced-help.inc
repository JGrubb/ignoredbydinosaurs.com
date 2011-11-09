<?php
// $Id: gmaps.advanced-help.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * Advanced help related functions.
 */

/**
 * Inline help for formatted numeric fields.
 * Return an empty string if help doesn't needed.
 */

function _gmaps_get_number_formatting_help() {
  global $user;
  static $help = NULL;

  if (is_null($help)) {
    $help = '';
    if ($user->uid == 0 || !($fm = module_exists('format_number'))) {
      if ($fm) {
        $options = format_number_get_options();
        $thousand = format_number_get_thousands_separator_options();
        $decimal = format_number_get_decimal_point_options();
        $args = array(
          '%decimal' => $decimal[$options['decimal_point']],
          '%thousand' => $thousand[$options['thousands_sep']],
        );
      }
      else {
        $args = array(
          '%decimal' => t('full stop (.)'),
          '%thousand' => t('comma (,) and space'),
        );
      }
      $help = t('You can use %decimal as decimal and %thousand as thousand separator.', $args);
    }
  }

  return $help;
}

/**
 * Advanced help helper function for dynamic help pages.
 *
 * @param $key
 *  Which section should be returned.
 *
 * @return html
 *
 * @See: input-formatting.html
 */
function _gmaps_element_help_page($key) {
  if ($key == 'number-formatting') {
    if (module_exists('format_number')) {
      $options = format_number_get_options();
      $thousand = format_number_get_thousands_separator_options();
      $decimal = format_number_get_decimal_point_options();
      $args = array(
        '%decimal' => $decimal[$options['decimal_point']],
        '%thousand' => $thousand[$options['thousands_sep']],
      );
      print theme('box', t('Format Number API settings'), t('You can use %decimal as decimal and %thousand as thousand separator.', $args));
    }
  }

  if ($key == 'length-units') {
    $desc = '';
    foreach (gmaps_length_get_unit_options() as $key => $units) {
      $desc .= '<p><strong>' . $key .':</strong> \''. implode("', '", $units) .'\'</p>';
    }
    if (!empty($desc)) {
      print theme('box', t('You can use the following units (without quotes)'), $desc);
    }
  }

  exit();
}

