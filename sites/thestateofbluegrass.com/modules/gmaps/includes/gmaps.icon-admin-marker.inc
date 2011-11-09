<?php
// $Id: gmaps.icon-admin-marker.inc,v 1.1.2.4 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * Marker icon administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_icon_hook().
 */
function gmaps_gmaps_icon_marker($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'icon_size' => GMAPS_IMAGE_ICON,
      'data' => array(
        'cache' => 1,
        'primary_color' => '#FF0000',
        'stroke_color' => '#000000',
        'corner_color' => '#FFFFFF',
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_icon_form_marker($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_icon_form_marker_validate($a1, $a2);
  }
  else if($op == 'submit') {
    _gmaps_icon_form_marker_submit($a1, $a2);
  }
  else if($op == 'pre_render') {
    return _gmaps_icon_form_marker_pre_render($a1);
  }
  else if($op == 'clone') {
    _gmaps_icon_clone_marker($a1, $a2);
  }
  else if($op == 'reset') {
    _gmaps_icon_reset_marker($a1, $a2);
  }
}




/**********************
 * Internal functions *
 **********************/

/**
 * Marker icon specific form.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function _gmaps_icon_form_marker($form, &$form_state) {
  $icon = &$form['#gmaps_icon'];
  $settings = &$form['#gmaps_icon_settings'];
  $type_form = array();

  $type_form['cache'] = array('#type' => 'checkbox',
    '#title' => t('Cache files'),
    '#default_value' => $icon->data['cache'],
  );
  $type_form['icon_size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Icon size'),
    '#default_value' => $icon->icon_size,
    '#required' => TRUE,
    '#min_pixels' => $settings['min_pixels'],
    '#max_pixels' => $settings['max_pixels'],
    '#return_value_keys' => 'wh',
  );

  //color form
  $type_form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Colors'),
    '#attributes' => array('id' => 'gmaps-icon-color-form', 'class' => 'gmaps-icon-color-form-marker'),
    '#theme' => 'gmaps_icon_color_form_marker',
  );
  $type_form['color']['corner_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Corner color'),
    '#default_value' => $icon->data['corner_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['primary_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Primary color'),
    '#default_value' => $icon->data['primary_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['stroke_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Stroke color'),
    '#default_value' => $icon->data['stroke_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['preview'] = array('#type' => 'item',
    '#title' => t('Preview'),
    '#value' => theme('image', $icon->icon, '', '', array('id' => 'gmaps-icon-preview'), FALSE),
  );

  return $type_form;
}

function _gmaps_icon_form_marker_validate($form, &$form_state) {
  _gmaps_icon_validate_color_item($form, $form_state, 'corner_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'primary_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'stroke_color');

  $icon = &$form_state['values'];

  $icon['icon_size'] = $icon['data']['icon_size'];
  unset($icon['data']['icon_size']);

  $icon['data'] = array_merge($icon['data'], $icon['data']['color']);
  unset($icon['data']['color']);

  $gicon = _gmaps_create_marker_gicon($icon['icon_size'], $icon['data']);
  $form_state['gmaps_gicon'] = $gicon;

  $form_state['gmaps_saved_icon_files'] = array();
  $saved_files = &$form_state['gmaps_saved_icon_files'];

  if ($filename = _gmaps_icon_data_save($gicon['icon'], 'data_icon')) {
    $saved_files['icon'] = $filename;
  }

  if ($icon['data']['cache']) {
    if ($filename = _gmaps_icon_data_save($gicon['print_image'], 'data_print_image')) {
      $saved_files['print_image'] = $filename;
    }
    if ($filename = _gmaps_icon_data_save($gicon['moz_print_image'], 'data_moz_print_image')) {
      $saved_files['moz_print_image'] = $filename;
    }
    if ($filename = _gmaps_icon_data_save($gicon['transparent'], 'data_transparent')) {
      $saved_files['transparent'] = $filename;
    }
  }
}

function _gmaps_icon_form_marker_submit($form, &$form_state) {
  $icon_path = gmaps_get_path('icon');

  $original_icon = &$form['#gmaps_icon'];
  $icon = &$form_state['values'];

  $saved_files = &$form_state['gmaps_saved_icon_files'];
  $gicon = &$form_state['gmaps_gicon'];

  foreach($saved_files as $key => $filename) {
    if ($file = _gmaps_icon_file_save($key, $filename, $original_icon->$key)) {
      $icon[$key] = $file->filepath;
    }
  }

  if ($icon['data']['cache']) {
    $icon = array_merge($gicon, $icon);
  }
  else {
    foreach(array('print_image', 'moz_print_image', 'transparent') as $field) {
      if (!empty($original_icon->$field)) {
        _gmaps_icon_file_delete($original_icon->$field);
      }
    }
    unset($gicon['icon'], $gicon['icon_size']);
    foreach($gicon as $field => $value) {
      $icon[$field] = '';
    }
  }
}

function _gmaps_icon_form_marker_pre_render($form) {
  _gmaps_icon_add_color_form_client_files();

  $icon = &$form['#gmaps_icon'];

  drupal_add_js(array('gmapsIconColorForm' => array(
    'reference' => array(
      'primary_color' => $icon->data['primary_color'],
      'stroke_color' => $icon->data['stroke_color'],
      'corner_color' => $icon->data['corner_color'],
    ),
    'preview' => 'previewMarker',
    'init' => 'initMarker',
  )), 'setting');

  return $form;
}

function _gmaps_create_marker_gicon($size, $colors) {
  $base_url = 'http://chart.apis.google.com/chart?cht=mm';

  $icon_url = $base_url .'&chs='. $size['width'] .'x'. $size['height'] .
      '&chco='. str_replace('#', '', $colors['corner_color']) .','.
      str_replace('#', '', $colors['primary_color']) .','.
      str_replace('#', '', $colors['stroke_color']);

  $icon = array();
  $icon['icon'] = $icon_url .'&ext=.png';
  $icon['icon_size'] = $size;
  $icon['shadow_size'] = array(intval($size['width'] * 1.6), $size['height']);
  $icon['icon_anchor'] = array(intval($size['width'] / 2), $size['height']);
  $icon['info_window_anchor'] = array(intval($size['width'] / 2), intval($size['height'] / 12));
  $icon['print_image'] = $icon_url .'&chof=gif';
  $icon['moz_print_image'] = $icon_url .'&chf=bg,s,ECECD8&chof=gif';
  $icon['transparent'] = $icon_url .'&chf=a,s,ffffff11&ext=.png';

  $icon['imagemap'] = array(
    $size['width'] / 2, $size['height'],
    (7 / 16) * $size['width'], (5 / 8) * $size['height'],
    (5 / 16) * $size['width'], (7 / 16) * $size['height'],
    (7 / 32) * $size['width'], (5 / 16) * $size['height'],
    (5 / 16) * $size['width'], (1 / 8) * $size['height'],
    (1 / 2) * $size['width'], 0,
    (11 / 16) * $size['width'], (1 / 8) * $size['height'],
    (25 / 32) * $size['width'], (5 / 16) * $size['height'],
    (11 / 16) * $size['width'], (7 / 16) * $size['height'],
    (9 / 16) * $size['width'], (5 / 8) * $size['height'],
  );
  foreach ($icon['imagemap'] as $i => $value) {
    $icon['imagemap'][$i] = intval($value);
  }
  $icon['imagemap'] = implode(',', $icon['imagemap']);

  return $icon;
}

function _gmaps_icon_clone_marker($icon, &$values) {
  $gicon = _gmaps_create_marker_gicon($icon->icon_size, $icon->data);

  $saved_files = array();
  $saved_files['icon'] = ($filename = _gmaps_icon_data_save($gicon['icon'])) ? $filename : '';

  $cache = &$icon->data['cache'];
  $saved_files['print_image'] = ($cache && ($filename = _gmaps_icon_data_save($gicon['print_image']))) ? $filename : '';
  $saved_files['moz_print_image'] = ($cache && ($filename = _gmaps_icon_data_save($gicon['moz_print_image']))) ? $filename : '';
  $saved_files['transparent'] = ($cache && ($filename = _gmaps_icon_data_save($gicon['transparent']))) ? $filename : '';

  foreach($saved_files as $key => $filename) {
    $values[$key] = (!empty($filename) && ($file = _gmaps_icon_file_save($key, $filename))) ? $file->filepath : '';
  }
}

function _gmaps_icon_reset_marker($base_icon, &$icon) {
  $icon->icon_size = $base_icon->icon_size;
  $icon->data = $base_icon->data;

  _gmaps_icon_file_copy($base_icon->icon, $icon->icon);

  foreach(array('print_image', 'moz_print_image', 'transparent') as $field) {
    if ($icon->data['cache']) {
      _gmaps_icon_file_copy($base_icon->$field, $icon->$field);
    }
    else {
      $icon->$field = '';
    }
  }
}





/**************
 * Themeables *
 **************/

/**
 * Theme marker icon color form.
 */
function theme_gmaps_icon_color_form_marker($form) {
  // Wrapper
  $output .= '<div class="color-form clear-block">';

  // Palette
  $output .= '<div id="palette" class="clear-block">';
  $output .= drupal_render($form['corner_color']);
  $output .= drupal_render($form['primary_color']);
  $output .= drupal_render($form['stroke_color']);
  $output .= '</div>';
  $output .= drupal_render($form['preview']);

  // Preview
  $output .= drupal_render($form);

  // Close wrapper
  $output .= '</div>';

  return $output;
}
