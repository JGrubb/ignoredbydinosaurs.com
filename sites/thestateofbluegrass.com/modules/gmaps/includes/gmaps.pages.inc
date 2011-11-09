<?php
// $Id: gmaps.pages.inc,v 1.1.2.8 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * Functions for user accessible functionality.
 */

/**
 * The length converter callback.
 */
function gmaps_length_converter_page() {
  global $gmaps_throttle;
  $length = &$_SESSION['gmaps_length_converter_filter'];
  $length = is_array($length) ? $length : array();

  module_load_include('inc', 'gmaps', 'includes/gmaps.length');

  $output = drupal_get_form('gmaps_length_converter_filter_form');

  if (!empty($length) && $length['value'] != 0) {
    $results = $titles = array();
    $base_group = NULL;
    foreach(gmaps_length_get_units() as $key => $info) {
      if ($key != $length['unit']) {
        $result = _gmaps_length_convert($length, $key);
        if ($result !== FALSE) {
          $result = array('value' => $result, 'unit' => $key);
          $results[$info['group']['key']][$key] = $gmaps_throttle ? _gmaps_length_format_plain($result, GMAPS_MAX_PRECISION) : _gmaps_length_format($result, -1, FALSE);
        } else {
          $results[$info['group']['key']][$key] = t('Invalid result: @unit', array('@unit' => $info['title']));
        }
        $titles[$info['group']['key']] = $info['group']['title'];
      }
      else {
        $base_group = $info['group']['key'];
      }
    }

    if (!empty($results)) {
      foreach($results as $group => $values) {
        $values = array_reverse($values, TRUE);
        $fieldset = array(
          '#title' => $titles[$group],
          '#collapsible' => TRUE,
          '#collapsed' => ($group != $base_group),
          '#value' => theme('item_list', $values, NULL, 'ul', array('class' => 'result-list')),
          '#attributes' => array('class' => 'gmaps-length-converter-box gmaps-length-converter-box-'. strtr($group, '_', '-')),
        );
        $output .= theme('fieldset', $fieldset);
      }
    }
  }

  return $output;
}

/**
 * Input form for length converter page.
 *
 * If you call it directly, you should load the length include before the call as follows:
 * module_load_include('inc', 'gmaps', 'includes/gmaps.length')
 */
function gmaps_length_converter_filter_form() {
  global $user;
  $session = &$_SESSION['gmaps_length_converter_filter'];
  $session = is_array($session) ? $session : array();

  if (isset($_GET['length'])) {
    $length = _gmaps_length_parse($_GET['length']);
    if ($length !== FALSE) {
      $session = $length;
    }
  }

  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Convert length'),
    '#collapsible' => FALSE,
  );
  $form['filter']['length'] = array('#type' => $user->gmaps_settings['length']['input'],
    '#title' => t('Length to convert'),
    '#default_value' => !empty($session) ? $session : '',
    '#size' => 30,
  );
  if ($user->gmaps_settings['length']['input'] == 'gmaps_distancefield' && !module_exists('advanced_help')) {
    $desc = t('You can use the following units (without quotes):');
    foreach (gmaps_length_get_unit_options() as $key => $units) {
      $desc .= '<p><strong>' . $key .':</strong> \''. implode("', '", $units) .'\'</p>';
    }
    $form['filter']['available_units'] = array(
      '#type' => 'fieldset',
      '#title' => t('Available units'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($session),
      '#description' => $desc,
    );
  }

  $form['filter']['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Convert'),
  );
  $form['filter']['buttons']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('gmaps_length_converter_filter_form_reset_submit'),
  );

  $form['#submit'][] = 'gmaps_length_converter_filter_form_submit';

  return $form;
}

function gmaps_length_converter_filter_form_reset_submit($form, &$form_state) {
  $_SESSION['gmaps_length_converter_filter'] = array();
}

function gmaps_length_converter_filter_form_submit($form, &$form_state) {
  $_SESSION['gmaps_length_converter_filter'] = isset($form_state['values']['length']) ? $form_state['values']['length'] : array();
}

/**
 * $form_state based 'one-shot' only
 */
function gmaps_length_converter_block_form(&$form_state = NULL) {
  $values = &$form_state['values'];

  module_load_include('inc', 'gmaps', 'includes/gmaps.length');
  $settings = gmaps_get_settings('gmaps_length_converter_block_settings');

  if ($settings['ahah']) {
    $form = array(
      '#prefix' => '<div class="clear-block" id="gmaps-length-converter-block">',
      '#suffix' => '</div>',
      '#cache' => TRUE,
    );
  }
  $form['#gmaps_block_settings'] = $settings;

  $form['length'] = array('#type' => 'gmaps_distancefield',
    '#title' => t('Length to convert'),
    '#default_value' => isset($values['length']) ? $values['length'] : '',
    '#size' => 20,
    '#required' => TRUE,
    //'#disabled'(as is) ignores default help
    '#description' => '#disabled',
  );
  $form['units'] = array(
    '#type' => 'select',
    '#title' => $settings['multiple'] ? t('Target units') : t('Target unit'),
    '#default_value' => isset($values['units']) ? $values['units'] : array(),
    '#options' => gmaps_length_get_unit_options(),
    '#required' => TRUE,
  );
  if ($settings['multiple']) {
    $form['units']['#multiple'] = TRUE;
    $form['units']['#size'] = 8;
  }
  //'submit' fails on pages where more than one form and not this form is the first.
  $form['gmaps_length_converter_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Convert'),
  );
  if ($settings['ahah']) {
    $form['gmaps_length_converter_submit']['#ahah'] = array(
      'path' => 'gmaps/length-converter-js',
      'wrapper' => 'gmaps-length-converter-block',
      'method' => 'replace',
      'effect' => 'fade',
    );
  }

  $storage = $form_state['storage']['gmaps_length_converter_block_results'];
  if (!empty($storage)) {
    $form = array_merge($form, $storage);
  }

  $form['#submit'][] = 'gmaps_length_converter_block_form_submit';

  return $form;
}

function gmaps_length_converter_block_form_submit($form, &$form_state) {
  if ($form_state['values']['length']['value'] > 0) {
    global $gmaps_throttle;
    $length = &$form_state['values']['length'];
    $units = &$form_state['values']['units'];
    if (!is_array($units)) {
      $units = array($units);
    }
    $storage = &$form_state['storage']['gmaps_length_converter_block_results'];
    $results = array();
    foreach(gmaps_length_get_units() as $key => $info) {
      if ($key != $length['unit'] && in_array($key, $units)) {
        $result = _gmaps_length_convert($length, $key);
        if ($result !== FALSE) {
          $result = array('value' => $result, 'unit' => $key);
          $results[$key] = $gmaps_throttle ? _gmaps_length_format_plain($result, 2) : _gmaps_length_format($result, 2, FALSE);
        }
        else {
          $results[$key] = t('Invalid result: @unit', array('@unit' => $info['title']));
        }
      }
    }

    if (!empty($results)) {
      $results = array_reverse($results);
      $storage['results'] = array(
        '#value' => theme('item_list', $results, NULL, 'ul', array('class' => 'result-list')),
      );
    }

    if ($form['#gmaps_block_settings']['link_to_page']) {
      $storage['link_to_page'] = array('#value' => '<div>'. l(t('Convert to all units'), 'length-converter', array('query' => array('length' => _gmaps_length_format_plain($length)))) .'</div>');
    }
  }
}

function gmaps_length_converter_block_js() {
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  // Get the form from the cache.
  $form = form_get_cache($form_build_id, $form_state);
  if (!$form) {
    drupal_json(array('status' => FALSE, 'data' => ''));
    exit();
  }
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  if ($form_id != 'gmaps_length_converter_block_form') {
    drupal_json(array('status' => FALSE, 'data' => ''));
    exit();
  }
  // We will run some of the submit handlers so we need to disable redirecting.
  $form['#redirect'] = FALSE;
  // We need to process the form, prepare for that by setting a few internals
  // variables.
  $form['#post'] = $_POST;
  $form['#programmed'] = FALSE;
  $form_state['post'] = $_POST;
  // Build, validate and if possible, submit the form.
  drupal_process_form($form_id, $form, $form_state);
  // This call recreates the form relying solely on the form_state that the
  // drupal_process_form set up.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  // Render the new output.
  //$choice_form = $form['choice_wrapper']['choice'];
  unset($form['#prefix'], $form['#suffix']); // Prevent duplicate wrappers.
  $output = theme('status_messages') . drupal_render($form);

  drupal_json(array('status' => TRUE, 'data' => $output));
}

function gmaps_reference_location_block_form() {

  $settings = gmaps_get_settings('gmaps_reference_location_block_settings');
  $loc = gmaps_get_reference_location();

  $form['set_location'] = array('#type' => 'fieldset',
    '#title' => t('New location'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['set_location']['location'] = array('#type' => 'gmaps_anp',
    '#default_value' => $loc,
    '#required' => FALSE,
    '#collapsible' => FALSE,
    '#size' => 20,
    '#enable_3D' => $settings['elevation'],
    '#enable_bounds' => $settings['bounds'],
    '#use_map' => $settings['use_map'] ? TRUE : FALSE,
  );

  $form['set_location']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Set'),
  );

  $form['reference'] = array('#type' => 'item',
    '#title' => t('Current location'),
    '#value' => $loc ? ((isset($loc->address) ? gmaps_address_render($loc->address) : '') . (isset($loc->point) ? gmaps_coords_render($loc->point) : '')) : t('No location set.'),
    '#prefix' => '<div class="gmaps-reference-location">',
    '#suffix' => '</div>',
  );
  if ($loc) {
    $form['clear_reference'] = array('#type' => 'submit',
      '#value' => t('Clear'),
      '#submit' => array('_gmaps_reference_location_block_form_clear_submit'),
    );
  }
  
  $form['#submit'][] = 'gmaps_reference_location_block_form_submit';

  return $form;
}

function _gmaps_reference_location_block_form_clear_submit($form, &$form_state) {
  gmaps_get_reference_location(FALSE);
}

function gmaps_reference_location_block_form_submit($form, &$form_state) {
  gmaps_get_reference_location(gmaps_anp_empty($form_state['values']['location']) ? FALSE : $form_state['values']['location']);
}

