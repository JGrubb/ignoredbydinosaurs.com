<?php
// $Id: gmaps.icon-admin-labeled-marker.inc,v 1.1.2.3 2009/08/20 06:15:10 xmarket Exp $

/**
 * @file
 * Labeled marker icon administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_icon_hook().
 */
function gmaps_gmaps_icon_labeled_marker($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'data' => array(
        'cache' => 1,
        'pin_style' => 'pin',
        'label_cut' => 1,
        'primary_color' => '#FF0000',
        'stroke_color' => '#000000',
        'label_color' => '#000000',
        'star_primary_color' => '#FFFF00',
        'star_stroke_color' => '#0000FF',
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_icon_form_labeled_marker($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_icon_form_labeled_marker_validate($a1, $a2);
  }
  else if($op == 'submit') {
    _gmaps_icon_form_labeled_marker_submit($a1, $a2);
  }
  else if($op == 'pre_render') {
    return _gmaps_icon_form_labeled_marker_pre_render($a1);
  }
  else if($op == 'clone') {
    _gmaps_icon_clone_labeled_marker($a1, $a2);
  }
  else if($op == 'reset') {
    _gmaps_icon_reset_labeled_marker($a1, $a2);
  }
}




/**********************
 * Internal functions *
 **********************/

/**
 * Labeled marker icon specific form.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function _gmaps_icon_form_labeled_marker($form, &$form_state) {
  $icon = &$form['#gmaps_icon'];

  $type_form = array();

  $type_form['cache'] = array('#type' => 'checkbox',
    '#title' => t('Cache files'),
    '#default_value' => $icon->data['cache'],
  );
  $type_form['pin_style'] = array('#type' => 'select',
    '#title' => t('Pin style'),
    '#default_value' => $icon->data['pin_style'],
    '#options' => array(
      'pin' => t('Normal'),
      'pin_star' => t('Starred'),
      'pin_sleft' => t('Left-leaning'),
      'pin_sright' => t('Right-leaning'),
    ),
  );
  $type_form['label_cut'] = array('#type' => 'select',
    '#title' => t('Label cut'),
    '#default_value' => $icon->data['label_cut'],
    '#options' => drupal_map_assoc(array(0,1,2,3,4,5,6)),
  );

  //color form
  $type_form['color'] = array(
    '#type' => 'fieldset',
    '#title' => t('Colors'),
    '#attributes' => array('id' => 'gmaps-icon-color-form', 'class' => 'gmaps-icon-color-form-labeled-marker'),
    '#theme' => 'gmaps_icon_color_form_labeled_marker',
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
  $type_form['color']['stroke_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Stroke color'),
    '#default_value' => $icon->data['stroke_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['star_primary_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Star primary color'),
    '#default_value' => $icon->data['star_primary_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['star_stroke_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Star stroke color'),
    '#default_value' => $icon->data['star_stroke_color'],
    '#size' => 8,
    '#required' => TRUE,
  );
  $type_form['color']['preview'] = array('#type' => 'item',
    '#title' => t('Preview'),
    '#value' => theme('image', $icon->icon, '', '', array('id' => 'gmaps-icon-preview'), FALSE),
  );

  return $type_form;
}

function _gmaps_icon_form_labeled_marker_validate($form, &$form_state) {
  _gmaps_icon_validate_color_item($form, $form_state, 'label_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'primary_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'stroke_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'star_primary_color');
  _gmaps_icon_validate_color_item($form, $form_state, 'star_stroke_color');

  $icon = &$form_state['values'];

  $icon['data'] = array_merge($icon['data'], $icon['data']['color']);
  unset($icon['data']['color']);

  $gicon = _gmaps_create_labeled_marker_gicon($icon['label'], $icon['data']);
  $form_state['gmaps_gicon'] = $gicon;

  $icon['icon_size'] = $gicon['icon_size'];

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

function _gmaps_icon_form_labeled_marker_submit($form, &$form_state) {
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

function _gmaps_icon_form_labeled_marker_pre_render($form) {
  _gmaps_icon_add_color_form_client_files();

  $icon = &$form['#gmaps_icon'];

  drupal_add_js(array('gmapsIconColorForm' => array(
    'reference' => array(
      'primary_color' => $icon->data['primary_color'],
      'stroke_color' => $icon->data['stroke_color'],
      'label_color' => $icon->data['label_color'],
      'star_primary_color' => $icon->data['star_primary_color'],
      'star_stroke_color' => $icon->data['star_stroke_color'],
    ),
    'preview' => 'previewLabeledMarker',
    'init' => 'initLabeledMarker',
  )), 'setting');

  return $form;
}

function _gmaps_create_labeled_marker_gicon($label, $data) {
  $label = _gmaps_icon_escape_label($label);
  $size = array(21, 34);
  if ($data['pin_style'] == 'pin_star') {
    $size = array(23, 39);
  }
  else if ($data['pin_style'] == 'pin_sleft' || $data['pin_style'] == 'pin_sright') {
    $size = array(23, 33);
  }

  $base_url = 'http://chart.apis.google.com/chart?cht=d&chdp=mapsapi&chl=';

  $icon_url = $base_url . $data['pin_style'] .'\'i\\\'['. $label .
    '\'-' . $data['label_cut'] . '\'f\\hv\'a\\]h\\]o\\' .
    str_replace('#', '', $data['primary_color']) .'\'fC\\'.
    str_replace('#', '', $data['label_color']) .'\'tC\\'.
    str_replace('#', '', $data['stroke_color']) .'\'eC\\';
  if ($data['pin_style'] == 'pin_star') {
    $icon_url .= str_replace('#', '', $data['star_primary_color']) .'\'1C\\'.
      str_replace('#', '', $data['star_stroke_color']) .'\'0C\\';
  }

  $icon = array();
  $icon['icon'] = $icon_url .'Lauto\'f\\&ext=.png';
  $icon['icon_size'] = array('width' => $size[0], 'height' => $size[1]);
  $icon['shadow_size'] = array('width' => intval($size[0] * 1.6), 'height' => $size[1]);
  $icon['icon_anchor'] = array('x' => intval($size[0] / 2), 'y' => $size[1]);
  $icon['info_window_anchor'] = array('x' => intval($size[0] / 2), 'y' => intval($size[1] / 12));
  $icon['print_image'] = $icon_url .'&chof=gif';
  $icon['moz_print_image'] = $icon_url .'&chf=bg,s,ECECD8&chof=gif';
  $icon['transparent'] = $icon_url .'Lauto\'f\\&chf=a,s,ffffff11&ext=.png';

  $icon['imagemap'] = array(
    $size[0] / 2, $size[1],
    (7 / 16) * $size[0], (5 / 8) * $size[1],
    (5 / 16) * $size[0], (7 / 16) * $size[1],
    (7 / 32) * $size[0], (5 / 16) * $size[1],
    (5 / 16) * $size[0], (1 / 8) * $size[1],
    (1 / 2) * $size[0], 0,
    (11 / 16) * $size[0], (1 / 8) * $size[1],
    (25 / 32) * $size[0], (5 / 16) * $size[1],
    (11 / 16) * $size[0], (7 / 16) * $size[1],
    (9 / 16) * $size[0], (5 / 8) * $size[1],
  );
  foreach ($icon['imagemap'] as $i => $value) {
    $icon['imagemap'][$i] = intval($value);
  }
  $icon['imagemap'] = implode(',', $icon['imagemap']);

  return $icon;
}

function _gmaps_icon_clone_labeled_marker($icon, &$values) {
  $label = isset($values['label']) ? $values['label'] : $icon->label;
  $data = isset($values['data']) ? $values['data'] : $icon->data;
  $gicon = _gmaps_create_labeled_marker_gicon($label, $data);

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

function _gmaps_icon_reset_labeled_marker($base_icon, &$icon) {
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
 * Theme labeled marker icon color form.
 */
function theme_gmaps_icon_color_form_labeled_marker($form) {
  // Wrapper
  $output .= '<div class="color-form clear-block">';

  // Palette
  $output .= '<div id="palette" class="clear-block">';
  $output .= drupal_render($form['label_color']);
  $output .= drupal_render($form['primary_color']);
  $output .= drupal_render($form['stroke_color']);
  $output .= drupal_render($form['star_primary_color']);
  $output .= drupal_render($form['star_stroke_color']);
  $output .= '</div>';
  $output .= drupal_render($form['preview']);

  // Preview
  $output .= drupal_render($form);

  // Close wrapper
  $output .= '</div>';

  return $output;
}
