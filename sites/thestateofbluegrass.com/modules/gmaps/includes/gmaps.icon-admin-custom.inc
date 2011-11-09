<?php
// $Id: gmaps.icon-admin-custom.inc,v 1.1.2.4 2009/08/20 06:15:10 xmarket Exp $

/**
 * @file
 * Custom icon administration functions.
 */


/************************
 * Hook implementations *
 ************************/

/**
 * Implementation of hook_gmaps_icon_hook().
 */
function gmaps_gmaps_icon_custom($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'throttle_support' => 1,
      'sticky' => 0,
      'sticky_id' => 0,
    );
  }
  else if($op == 'form') {
    return _gmaps_icon_form_custom($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_icon_form_custom_validate($a1, $a2);
  }
  else if($op == 'submit') {
    _gmaps_icon_form_custom_submit($a1, $a2);
  }
  else if($op == 'clone') {
    _gmaps_icon_clone_custom($a1, $a2);
  }
  else if($op == 'reset') {
    _gmaps_icon_reset_custom($a1, $a2);
  }
}



/**********************
 * Internal functions *
 **********************/

/**
 * Custom icon specific form.
 *
 * @param $form
 * @param $form_state
 * @return array
 */
function _gmaps_icon_form_custom($form, &$form_state) {
  $icon = &$form['#gmaps_icon'];
  $settings = &$form['#gmaps_icon_settings'];
  $path = gmaps_get_path('icon');

  $type_form['icon'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Icon'),
    '#default_value' => $icon->data['icon'],
    '#required' => TRUE,
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'icon-#fid',
    '#description' => t('Foreground image of the icon.'),
  );
  $type_form['icon_anchor'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Icon anchor'),
    '#default_value' => $icon->icon_anchor,
    '#required' => TRUE,
    '#min_pixels' => -512,
    '#max_pixels' => 512,
    '#enable_negative' => TRUE,
    '#return_value_keys' => 'xy',
    '#description' => t('The pixel coordinate relative to the top left corner of the icon image at which this icon is anchored to the map.'),
  );
  $type_form['info_window_anchor'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Info window anchor'),
    '#default_value' => $icon->info_window_anchor,
    '#required' => TRUE,
    '#min_pixels' => -512,
    '#max_pixels' => 512,
    '#enable_negative' => TRUE,
    '#return_value_keys' => 'xy',
    '#description' => t('The pixel coordinate relative to the top left corner of the icon image at which the info window is anchored to this icon.'),
  );
  $type_form['imagemap'] = array('#type' => 'textarea',
    '#title' => t('Imagemap'),
    '#default_value' => $icon->imagemap,
    '#required' => TRUE,
    '#rows' => 2,
    '#description' => t('A comma separated list of integers representing the x/y coordinates of the image map the API should use to specify the clickable part of the icon image in browsers other than Internet Explorer. See the <a href="@url">Client-side image maps: the MAP and AREA elements</a> reference, where shape is "poly".', array('@url' => url('http://www.w3.org/TR/html401/struct/objects.html#h-13.6.1', array('absolute' => TRUE)))),
  );
  $type_form['shadow'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Shadow icon'),
    '#default_value' => $icon->data['shadow'],
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'shadow-#fid',
    '#description' => t('Shadow image of the icon.'),
  );
  $type_form['shadow_size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Shadow size'),
    '#default_value' => $icon->shadow_size,
    '#description' => t('Use "0x0" to disable shadow.'),
    '#return_value_keys' => 'wh',
  );
  $type_form['print_shadow'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Print shadow icon'),
    '#default_value' => $icon->data['print_shadow'],
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'print-shadow-#fid',
    '#description' => t('The shadow image used for printed maps. It should be a GIF image since most browsers cannot print PNG images.'),
  );
  $type_form['print_image'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Print image'),
    '#default_value' => $icon->data['print_image'],
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'print-image-#fid',
    '#description' => t('Alternate foreground icon image used for printing on browsers incapable of handling the default icon.'),
  );
  $type_form['moz_print_image'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Non-transparent print image'),
    '#default_value' => $icon->data['moz_print_image'],
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'moz-print-image-#fid',
    '#description' => t('An alternate non-transparent icon image used for printing on browsers incapable of handling either transparent PNGs (provided in the default icon) or transparent GIFs (provided in print image).'),
  );
  $type_form['transparent'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Transparent icon'),
    '#default_value' => $icon->data['transparent'],
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'transparent-#fid',
    '#description' => t('A virtually transparent version of the foreground icon image used to capture click events in Internet Explorer. This image should be a 24-bit PNG version of the main icon image with 1% opacity, but the same shape and size as the main icon.'),
  );
  return $type_form;
}

function _gmaps_icon_form_custom_validate($form, &$form_state) {
  $icon = &$form_state['values'];
  $data = &$icon['data'];
  if (empty($data['shadow']) && !empty($data['print_shadow'])) {
    form_set_error('data][shadow', t('Shadow icon is required, if print shadow icon is set.'));
  }
  if (empty($data['print_image']) && !empty($data['moz_print_image'])) {
    form_set_error('data][print_image', t('Print image is required, if non-transparent print image is set.'));
  }
  $imagemap = array_filter(array_map('trim', explode(',', $data['imagemap'])), 'intval');
  if ((count($imagemap) % 2) != 0) {
    form_set_error('data][imagemap', t('Imagemap coordinates must be set in x,y pair of integers.'));
  }
  if (count($imagemap) < 4) {
    form_set_error('data][imagemap', t('At least two pairs of coordinates required for a valid imagemap value.'));
  }
}

function _gmaps_icon_form_custom_submit($form, &$form_state) {
  $icon = &$form_state['values'];
  $data = &$icon['data'];
  $icon['icon'] = $data['icon']->filepath;
  $icon['icon_size'] = gmaps_pixels_parse(image_get_info($icon['icon']), 'wh');
  $data['icon'] = $data['icon']->fid;

  $icon['icon_anchor'] = $data['icon_anchor'];
  unset($data['icon_anchor']);
  $icon['info_window_anchor'] = $data['info_window_anchor'];
  unset($data['info_window_anchor']);
  $icon['imagemap'] = $data['imagemap'];
  unset($data['imagemap']);

  if (!empty($data['shadow'])) {
    $icon['shadow'] = $data['shadow']->filepath;
    $data['shadow'] = $data['shadow']->fid;
    if (empty($data['shadow_size'])) {
      $data['shadow_size'] = gmaps_pixels_parse(image_get_info($icon['shadow']), 'wh');
    }
  }
  else {
    $icon['shadow'] = NULL;
    unset($data['shadow']);
  }
  $icon['shadow_size'] = $data['shadow_size'];
  unset($data['shadow_size']);

  if (!empty($data['print_shadow'])) {
    $icon['print_shadow'] = $data['print_shadow']->filepath;
    $data['print_shadow'] = $data['print_shadow']->fid;
  }
  else {
    $icon['print_shadow'] = NULL;
    unset($data['print_shadow']);
  }
  if (!empty($data['print_image'])) {
    $icon['print_image'] = $data['print_image']->filepath;
    $data['print_image'] = $data['print_image']->fid;
  }
  else {
    $icon['print_image'] = NULL;
    unset($data['print_image']);
  }
  if (!empty($data['moz_print_image'])) {
    $icon['moz_print_image'] = $data['moz_print_image']->filepath;
    $data['moz_print_image'] = $data['moz_print_image']->fid;
  }
  else {
    $icon['moz_print_image'] = NULL;
    unset($data['moz_print_image']);
  }
  if (!empty($data['transparent'])) {
    $icon['transparent'] = $data['transparent']->filepath;
    $data['transparent'] = $data['transparent']->fid;
  }
  else {
    $icon['transparent'] = NULL;
    unset($data['transparent']);
  }
}

function _gmaps_icon_clone_custom($icon, &$values) {
  $values['data'] = array();
  foreach(_gmaps_icon_get_file_fields() as $field) {
    if ($file = _gmaps_icon_file_clone($field, $icon->$field)) {
      $values[$field] = $file->filepath;
      $values['data'][$field] = $file->fid;
    }
    else {
      $values[$field] = '';
      $values['data'][$field] = '';
    }
  }
}

function _gmaps_icon_reset_custom($base_icon, &$icon) {
  $icon->icon_size = $base_icon->icon_size;
  $icon->shadow_size = $base_icon->shadow_size;
  $icon->icon_anchor = $base_icon->icon_anchor;
  $icon->info_window_anchor = $base_icon->info_window_anchor;
  $icon->imagemap = $base_icon->imagemap;

  foreach(_gmaps_icon_get_file_fields() as $field) {
    if (!empty($base_icon->$field) || !empty($icon->$field)) {
      _gmaps_icon_image_field_reset($field, $base_icon, $icon);
      if (is_object($icon->$field)) {
        $icon->data[$field] = $icon->$field->fid;
        $icon->$field = $icon->$field->filepath;
      }
    }
  }
}
