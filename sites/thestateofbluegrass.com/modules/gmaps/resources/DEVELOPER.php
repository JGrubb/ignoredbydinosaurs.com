<?php

/**
 * @file
 * Preliminary developer documentation.
 */

/**
 * Form elements.
 */
function hook_elements() {
  //basically a textfield.
  $elements['gmaps_distancefield'] = array(
    '#process' => array('gmaps_distancefield_expand'),
    '#element_validate' => array('_gmaps_distancefield_validate'),
    '#enable_negative' => FALSE,
  );
  $elements['gmaps_distancefield'] = array_merge_recursive($elements['gmaps_distancefield'], _element_info('textfield'));

  //successor of the distance field
  $elements['gmaps_distance'] = array(
    '#input' => TRUE,
    '#process' => array('gmaps_distance_expand'),
    '#element_validate' => array('_gmaps_distance_validate'),
    '#enable_negative' => FALSE,
  );

  //Input for image size or pixel coordinates
  //possible inputs ('#default_value'):
  // array('width' => 1, 'height' => 1), array('x' => 1, 'y' => 1),
  // array(1,1), '1x1', '1X1', and 1 (which means '1x1')
  $elements['gmaps_pixels'] = array(
    '#input' => TRUE,
    '#process' => array('_gmaps_pixels_expand'),
    '#element_validate' => array('_gmaps_pixels_validate'),
    '#min_pixels' => NULL,//unlimited: down to -9999 per value
    '#max_pixels' => NULL,//unlimited: up to 99999 per value
    '#return_value_keys' => 'wh',
    '#enable_negative' => FALSE,
  );
  $elements['gmaps_pixels'] = array_merge_recursive($elements['gmaps_pixels'], _element_info('textfield'));

  $elements['gmaps_image_file'] = array(
    '#input' => TRUE,
    '#process' => array('_gmaps_image_file_expand'),
    '#element_validate' => array('_gmaps_image_file_validate'),
    //supports!!
    '#required' => FALSE,
    '#preview_max_size' => GMAPS_IMAGE_THUMBNAIL,
    '#image_min_size' => 0,
    '#image_max_size' => 0,
    '#image_max_filesize' => 0,
    '#file_dest_dir' => 'sites/default/files',
    '#dir_check_mode' => FILE_CREATE_DIRECTORY,
    //'#name', '#filename', '#element'
    '#file_name_pattern' => 'any-text-#fid',
    '#file_replace_mode' => FILE_EXISTS_RENAME,
    //custom validators for file_save_upload
    '#image_validators' => array('validator', array('arg1', 'arg2')),
  );

}

/**
 * Icon informations.
 */
function hook_gmaps_icon_info() {
  $types = array();

  $types['custom'] = array(
    //TRANSLATABLE
    'title' => 'Custom',
    'js_callback' => 'GMaps.createCustomIcon',
    'js_files' => array('misc/gmaps-icons.js'),
    //relative to module's root
    //included as: module_load_include('inc', $info->module, $info->file);
    'file' => 'includes/gmaps.icon-form',
    //0 - disabled, -1 - unlimited, other - exact length
    'label_length' => 2,
  );
  return $types;
}


?>
