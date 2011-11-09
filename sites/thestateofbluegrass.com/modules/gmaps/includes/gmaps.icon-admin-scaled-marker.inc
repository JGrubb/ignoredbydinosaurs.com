<?php
// $Id: gmaps.icon-admin-scaled-marker.inc,v 1.1.2.5 2010/02/12 20:08:06 xmarket Exp $

/**
 * @file
 * Flat icon administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_icon_hook().
 */
function gmaps_gmaps_icon_scaled_marker($op, &$a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'data' => array(
        'cache' => 1,
        'scale' => 0.5,
        'rotation' => 0,
        'label_size' => 10,
        'label_style' => '_',
        'primary_color' => '#FF0000',
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_icon_form_scaled_marker($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_icon_form_scaled_marker_validate($a1, $a2);
  }
  else if($op == 'submit') {
    _gmaps_icon_form_scaled_marker_submit($a1, $a2);
  }
  else if($op == 'pre_render') {
    return _gmaps_icon_form_scaled_marker_pre_render($a1);
  }
  else if($op == 'clone') {
    _gmaps_icon_clone_scaled_marker($a1, $a2);
  }
  else if($op == 'reset') {
    _gmaps_icon_reset_scaled_marker($a1, $a2);
  }
}

/**
 * Implementation of hook_form_hook_alter().
 */
function gmaps_form_gmaps_icon_form_alter(&$form, &$form_state) {
  $form['settings']['label']['#description'] = t('You can separate lines by "|" (pipe).');
}


/**********************
 * Internal functions *
 **********************/


function _gmaps_icon_form_scaled_marker($form, &$form_state) {
  $icon = &$form['#gmaps_icon'];
  $type_form = array();

  $type_form['cache'] = array('#type' => 'checkbox',
    '#title' => t('Cache files'),
    '#default_value' => $icon->data['cache'],
  );
  $type_form['scale'] = array('#type' => 'textfield',
    '#title' => t('Scale'),
    '#default_value' => $icon->data['scale'],
    '#required' => TRUE,
    '#size' => 10,
    '#maxlength' => 5,
  );
  $type_form['rotation'] = array('#type' => 'textfield',
    '#title' => t('Rotation'),
    '#default_value' => $icon->data['rotation'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => '&deg;',
    '#description' => t('Counting counter-clockwise.'),
  );
  $type_form['label_size'] = array('#type' => 'textfield',
    '#title' => t('Label size'),
    '#default_value' => $icon->data['label_size'],
    '#required' => TRUE,
    '#size' => 4,
    '#maxlength' => 2,
    '#field_suffix' => 'px',
  );
  $type_form['label_style'] = array('#type' => 'select',
    '#title' => t('Label style'),
    '#default_value' => $icon->data['label_style'],
    '#options' => array(
      '_' => t('Normal'),
      'b' => t('Bold'),
    ),
  );

  //color form
  $type_form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Colors'),
    '#attributes' => array('id' => 'gmaps-icon-color-form', 'class' => 'gmaps-icon-color-form-scaled-marker'),
    '#theme' => 'gmaps_icon_color_form_scaled_marker',
  );
  $type_form['color']['primary_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Primary color'),
    '#default_value' => $icon->data['primary_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['preview'] = array('#type' => 'item',
    '#title' => t('Preview'),
    '#value' => theme('image', $icon->icon, '', '', array('id' => 'gmaps-icon-preview'), FALSE),
  );

  return $type_form;
}

function _gmaps_icon_form_scaled_marker_validate($form, &$form_state) {
  _gmaps_icon_validate_color_item($form, $form_state, 'primary_color');

  $icon = &$form_state['values'];

  if (!is_numeric($icon['data']['scale'])) {
    form_set_error('scale', t('!title must be a numeric value.', array('!title' => $form['data']['scale']['#title'])));
  }
  else if ($icon['data']['scale'] <= 0 || $icon['data']['scale'] > 9.66) {
    form_set_error('scale', t('!title must be greater than zero and lesser than or equal to 9.66.', array('!title' => $form['data']['scale']['#title'])));
  }
  if (!is_numeric($icon['data']['rotation'])) {
    form_set_error('rotation', t('!title must be a numeric value.', array('!title' => $form['data']['rotation']['#title'])));
  }
    if (!is_numeric($icon['data']['label_size'])) {
    form_set_error('label_size', t('!title must be a numeric value.', array('!title' => $form['data']['label_size']['#title'])));
  }

  if (form_get_errors()) {
    return;
  }

  $icon['data'] = array_merge($icon['data'], $icon['data']['color']);
  unset($icon['data']['color']);

  $gicon = _gmaps_create_scaled_marker_gicon($icon['label'], $icon['data']);
  $form_state['gmaps_gicon'] = $gicon;

  $form_state['gmaps_saved_icon_files'] = array();
  $saved_files = &$form_state['gmaps_saved_icon_files'];

  if ($filename = _gmaps_icon_data_save($gicon['icon'], 'data_icon')) {
    $saved_files['icon'] = $filename;
    $icon['icon_size'] = gmaps_pixels_parse(image_get_info($filename), 'wh');
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

function _gmaps_icon_form_scaled_marker_submit($form, &$form_state) {
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

function _gmaps_icon_form_scaled_marker_pre_render($form) {
  _gmaps_icon_add_color_form_client_files();

  $icon = &$form['#gmaps_icon'];

  drupal_add_js(array('gmapsIconColorForm' => array(
    'reference' => array(
      'primary_color' => $icon->data['primary_color'],
    ),
    'preview' => 'previewScaledMarker',
    'init' => 'initScaledMarker',
  )), 'setting');

  return $form;
}

function _gmaps_create_scaled_marker_gicon($label, $data) {
  if ($label) {
    $labels = explode('|', $label);
    foreach($labels as $i => $label) {
      $labels[$i] = drupal_urlencode($label);
    }
    $label = implode('|', $labels);
  }
  $size = array('width' => ceil(37 * $data['scale']), 'height' => ceil(66 * $data['scale']));

  $base_url = 'http://chart.apis.google.com/chart?chst=d_map_spin&chld=';

  $icon_url = $base_url . $data['scale'] .'|'. $data['rotation'] .'|'.
    str_replace('#', '', $data['primary_color']) .'|'.
    $data['label_size'] .'|'. $data['label_style'] .'|'. $label;

  $icon = array();
  $icon['icon'] = $icon_url .'&ext=.png';
  $icon['icon_size'] = $size;
  $icon['shadow_size'] = array('width' => 0, 'height' => 0);
  $icon['icon_anchor'] = array('x' => intval($size['width'] / 2), 'y' => $size['height']);
  $icon['info_window_anchor'] = array('x' => intval($size['width'] / 2), 'y' => intval($size['height'] / 12));
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

function _gmaps_icon_clone_scaled_marker($icon, &$values) {
  $gicon = _gmaps_create_scaled_marker_gicon($icon->label, $icon->data);

  $saved_files = array();
  $saved_files['icon'] = ($filename = _gmaps_icon_data_save($gicon['icon'])) ? $filename : '';
  $values['icon_size'] = gmaps_pixels_parse($filename ? image_get_info($filename) : '0x0', 'wh');

  $cache = &$icon->data['cache'];
  $saved_files['print_image'] = ($cache && ($filename = _gmaps_icon_data_save($gicon['print_image']))) ? $filename : '';
  $saved_files['moz_print_image'] = ($cache && ($filename = _gmaps_icon_data_save($gicon['moz_print_image']))) ? $filename : '';
  $saved_files['transparent'] = ($cache && ($filename = _gmaps_icon_data_save($gicon['transparent']))) ? $filename : '';

  foreach($saved_files as $key => $filename) {
    $values[$key] = (!empty($filename) && ($file = _gmaps_icon_file_save($key, $filename))) ? $file->filepath : '';
  }
}

function _gmaps_icon_reset_scaled_marker($base_icon, &$icon) {
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
 * Theme flat icon color form.
 */
function theme_gmaps_icon_color_form_scaled_marker($form) {
  // Wrapper
  $output .= '<div class="color-form clear-block">';

  // Palette
  $output .= '<div id="palette" class="clear-block">';
  $output .= drupal_render($form['primary_color']);
  $output .= '</div>';
  $output .= drupal_render($form['preview']);

  // Preview
  $output .= drupal_render($form);

  // Close wrapper
  $output .= '</div>';

  return $output;
}
