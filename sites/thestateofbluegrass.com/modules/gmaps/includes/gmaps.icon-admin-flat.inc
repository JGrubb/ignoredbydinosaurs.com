<?php
// $Id: gmaps.icon-admin-flat.inc,v 1.1.2.3 2009/08/20 06:15:10 xmarket Exp $

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
function gmaps_gmaps_icon_flat($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'icon_size' => GMAPS_IMAGE_ICON,
      'data' => array(
        'cache' => 1,
        'label_size' => 0,
        'shape' => 'circle',
        'shadow_from_transparency' => 'FF',
        'shadow_to_transparency' => '01',
        'label_color' => '#000000',
        'primary_color' => '#FF0000',
        'shadow_from_color' => '#000000',
        'shadow_to_color' => '#000000',
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_icon_form_flat($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_icon_form_flat_validate($a1, $a2);
  }
  else if($op == 'submit') {
    _gmaps_icon_form_flat_submit($a1, $a2);
  }
  else if($op == 'pre_render') {
    return _gmaps_icon_form_flat_pre_render($a1);
  }
  else if($op == 'clone') {
    _gmaps_icon_clone_flat($a1, $a2);
  }
  else if($op == 'reset') {
    _gmaps_icon_reset_flat($a1, $a2);
  }
}



/**********************
 * Internal functions *
 **********************/

function _gmaps_icon_form_flat($form, &$form_state) {
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
  $type_form['label_size'] = array('#type' => 'textfield',
    '#title' => t('Label size'),
    '#default_value' => $icon->data['label_size'],
    '#required' => TRUE,
    '#size' => 4,
    '#maxlength' => 2,
    '#field_suffix' => 'px',
  );
  $type_form['shape'] = array('#type' => 'select',
    '#title' => t('Shape'),
    '#default_value' => $icon->data['shape'],
    '#options' => array('circle' => t('Circle'), 'roundrect' => t('Rounded rectangle')),
  );
  $type_form['shadow_from_transparency'] = array('#type' => 'textfield',
    '#title' => t('Shadow from transparency'),
    '#default_value' => $icon->data['shadow_from_transparency'],
    '#required' => TRUE,
    '#size' => 4,
    '#maxlength' => 2,
    '#field_prefix' => '#',
    '#description' => t('Hexadecimal value between 00 and FF.'),
  );
  $type_form['shadow_to_transparency'] = array('#type' => 'textfield',
    '#title' => t('Shadow to transparency'),
    '#default_value' => $icon->data['shadow_to_transparency'],
    '#required' => TRUE,
    '#size' => 4,
    '#maxlength' => 2,
    '#field_prefix' => '#',
    '#description' => t('Hexadecimal value between 00 and FF.'),
  );

  //color form
  $type_form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Colors'),
    '#attributes' => array('id' => 'gmaps-icon-color-form', 'class' => 'gmaps-icon-color-form-flat'),
    '#theme' => 'gmaps_icon_color_form_flat',
  );
  $type_form['color']['label_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Label color'),
    '#default_value' => $icon->data['label_color'],
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
  $type_form['color']['shadow_from_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Shadow from color'),
    '#default_value' => $icon->data['shadow_from_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['shadow_to_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Shadow to color'),
    '#default_value' => $icon->data['shadow_to_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['preview'] = array('#type' => 'item',
    '#title' => t('Preview'),
    '#value' => theme('image', $icon->icon, '', '', array('id' => 'gmaps-icon-preview'), FALSE),
  );

  return $type_form;
}

function _gmaps_icon_form_flat_validate($form, &$form_state) {
  _gmaps_icon_validate_color_item($form, $form_state, 'label_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'primary_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'shadow_from_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'shadow_to_color');

  $icon = &$form_state['values'];

  if (intval($icon['data']['label_size']) != $icon['data']['label_size'] || $icon['data']['label_size'] <= 0) {
    form_set_error('data][label_size', t('The !title must be an integer greater than or equeal to zero.', array('!title' => $form['data']['label_size']['#title'])));
  }
  if (!preg_match('|[0-9A-Fa-f]{2,2}|', $icon['data']['shadow_from_transparency'])) {
    form_set_error('data][shadow_from_transparency', t('The !title must be given as a hexadecimal RGB value between 00 and FF.', array('!title' => $form['data']['shadow_from_transparency']['#title'])));
  }
  if (!preg_match('|[0-9A-Fa-f]{2,2}|', $icon['data']['shadow_to_transparency'])) {
    form_set_error('data][shadow_to_transparency', t('The !title must be given as a hexadecimal RGB value between 00 and FF.', array('!title' => $form['data']['shadow_to_transparency']['#title'])));
  }

  $icon['icon_size'] = $icon['data']['icon_size'];
  unset($icon['data']['icon_size']);

  $icon['data'] = array_merge($icon['data'], $icon['data']['color']);
  unset($icon['data']['color']);

  $gicon = _gmaps_create_flat_gicon($icon['label'], $icon['icon_size'], $icon['data']);
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

function _gmaps_icon_form_flat_submit($form, &$form_state) {
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

function _gmaps_icon_form_flat_pre_render($form) {
  _gmaps_icon_add_color_form_client_files();

  $icon = &$form['#gmaps_icon'];

  drupal_add_js(array('gmapsIconColorForm' => array(
    'reference' => array(
      'primary_color' => $icon->data['primary_color'],
      'label_color' => $icon->data['label_color'],
      'shadow_from_color' => $icon->data['shadow_from_color'],
      'shadow_to_color' => $icon->data['shadow_to_color'],
    ),
    'preview' => 'previewFlat',
    'init' => 'initFlat',
  )), 'setting');

  return $form;
}

function _gmaps_create_flat_gicon($label, $size, $data) {
  $label = drupal_urlencode($label);
  $shape_code = $data['shape'] == "circle" ? "it" : "itr";

  $base_url = 'http://chart.apis.google.com/chart?cht='. $shape_code;

  $icon_url = $base_url .'&chs='. $size['width'] .'x'. $size['height'] .
      '&chco='. str_replace('#', '', $data['primary_color']) .','.
      str_replace('#', '', $data['shadow_from_color']) . $data['shadow_from_transparency'] .','.
      str_replace('#', '', $data['shadow_to_color']) . $data['shadow_to_transparency'] .
      '&chl='. $label .'&chx='. str_replace('#', '', $data['label_color']) .','. $data['label_size'];

  $icon = array();
  $icon['icon'] = $icon_url .'&chf=bg,s,00000000&ext=.png';
  $icon['icon_size'] = $size;
  $icon['shadow_size'] = array('width' => 0, 'height' => 0);
  $icon['icon_anchor'] = array('x' => intval($size['width'] / 2), 'y' => intval($size['height'] / 2));
  $icon['info_window_anchor'] = array('x' => intval($size['width'] / 2), 'y' => intval($size['height'] / 2));
  $icon['print_image'] = $icon_url .'&chof=gif';
  $icon['moz_print_image'] = $icon_url .'&chf=bg,s,ECECD8&chof=gif';
  $icon['transparent'] = $icon_url .'&chf=a,s,ffffff11&ext=.png';

  $icon['imagemap'] = array();
  if ($shape_code == "itr") {
    $icon['imagemap'] = array(0, 0, $size['width'], 0, $size['width'], $size['height'], 0, $size['height']);
  } else {
    $poly_num_sides = 8;
    $poly_side_length = 360 / $poly_num_sides;
    $poly_radius = min($size['width'], $size['height']) / 2;
    for ($a = 0; $a < ($poly_num_sides + 1); $a++) {
      $a_rad = deg2rad($poly_side_length * $a);
      $x = $poly_radius + ($poly_radius * cos($a_rad));
      $y = $poly_radius + ($poly_radius * sin($a_rad));
      $icon['imagemap'][] = intval($x);
      $icon['imagemap'][] = intval($y);
    }
  }
  $icon['imagemap'] = implode(',', $icon['imagemap']);

  return $icon;
}

function _gmaps_icon_clone_flat($icon, &$values) {
  $gicon = _gmaps_create_flat_gicon($icon->label, $icon->icon_size, $icon->data);

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

function _gmaps_icon_reset_flat($base_icon, &$icon) {
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
function theme_gmaps_icon_color_form_flat($form) {
  // Wrapper
  $output .= '<div class="color-form clear-block">';

  // Palette
  $output .= '<div id="palette" class="clear-block">';
  $output .= drupal_render($form['label_color']);
  $output .= drupal_render($form['primary_color']);
  $output .= drupal_render($form['shadow_from_color']);
  $output .= drupal_render($form['shadow_to_color']);
  $output .= '</div>';
  $output .= drupal_render($form['preview']);

  // Preview
  $output .= drupal_render($form);

  // Close wrapper
  $output .= '</div>';

  return $output;
}
