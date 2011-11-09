<?php
// $Id: gmaps.image-file-element.inc,v 1.1.2.6 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_image_file element processors and validators
 */

/**
 * Return delete value for delete.
 */
function _gmaps_image_file_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    return array('delete' => 0);
  }
}

/**
 * Expands a gmaps_image_file element.
 */
function _gmaps_image_file_expand($element, $edit = FALSE, &$form_state = NULL) {
  static $settings;
  
  if (is_null($settings)) {
    $settings = gmaps_get_settings('gmaps_icon_settings');
  }
  $element['#tree'] = TRUE;

  $required = isset($element['#required']) ? $element['#required'] : FALSE;

  $value = NULL;
  if (($fbi = $form_state['values']['gif_build_id']) && ($cached = cache_get('gif_'. $fbi .'_'. $element['#id'], 'cache_form'))) {
    $value = $cached->data;
  }
  else if (isset($element['#default_value']) && !empty($element['#default_value'])) {
    $value = $element['#default_value'];
  }

  if ($value) {
    if (is_numeric($value)) {
      $file = db_fetch_object(db_query('SELECT * FROM {files} WHERE fid = %d', $value));
    }
    else if (is_string($value)) {
      $file = db_fetch_object(db_query("SELECT * FROM {files} WHERE filepath = '%s'", $value));
    }
    else if (is_array($value)) {
      $file = (object)$value;
    }
    else {
      $file = $value;
    }

    if ($file && !isset($element['#gmaps_new_image_file'])) {
        $element['#gmaps_image_file'] =  $file;
    }
  }
  else {
    $element['#gmaps_image_file'] = NULL;
  }

  if (isset($file)) {
    if (!isset($element['#preview_max_size'])) {
      $element['#preview_max_size'] = $settings['preview_size'];
    }
    $element['preview'] = array('#type' => 'markup',
      '#value' => '<div class="form-gmaps-image-file-preview">'. theme('gmaps_icon_preview', $file, $element['#preview_max_size']) .'</div>',
      '#weight' => 0,
    );
    if (!$required && !empty($element['#default_value'])) {
      $element['delete'] = array('#type' => 'checkbox',
        '#title' => t('Delete existing file.'),
        '#default_value' => 0,
        '#weight' => 1,
        '#attributes' => array('class' => 'form-gmaps-image-file-delete'),
      );
    }
  }

  $upload_name = implode('-', $element['#parents']);
  $element['upload'] = array('#type' => 'file',
    '#size' => isset($element['#size']) ? $element['#size'] : 32,
    '#name' => 'files['. $upload_name .']',
    '#weight' => 2,
    '#attributes' => array('class' => 'form-gmaps-image-file-upload'),
  );

  $element['#pre_render'][] = '_gmaps_image_file_pre_render';

  return $element;
}

/**
 * Validates a gmaps_image_file element.
 */
function _gmaps_image_file_validate(&$element, &$form_state) {
  global $gmaps_image_file_form_id, $gmaps_image_file_build_id;
  $t = get_t();

  $source = implode('-', $element['#parents']);

  //initialize storage for submit_handler
  if (!isset($form_state['_gmaps_image_file_storage'])) {
    if ($cached = cache_get('gif_storage_'. $form_state['values']['gif_build_id'], 'cache_form')) {
      $form_state['_gmaps_image_file_storage'] = $cached->data;
      $storage_cached = TRUE;
    }
    else {
      $form_state['_gmaps_image_file_storage'] = array(
        'save' => array(),
        'move' => array(),
        'delete' => array(),
      );
    }
  }

  //new upload
  if (isset($_FILES['files']) && $_FILES['files']['name'][$source] && is_uploaded_file($_FILES['files']['tmp_name'][$source])) {
    if (isset($element['#file_dest_dir'])) {
      $dest = $element['#file_dest_dir'];
      if (!file_check_directory($dest, (isset($element['#dir_check_mode']) ? $element['#dir_check_mode'] : FILE_CREATE_DIRECTORY ), implode('][', $element['#parents']))) {
        return;
      }
    }
    $validators = array(
      'file_validate_is_image' => array(),
    );
    $ranges = array();
    if (isset($element['#image_min_size']) && ($min = gmaps_pixels_parse($element['#image_min_size']))) {
      $ranges['min'] = gmaps_pixels_render($min, FALSE);
    }
    if (isset($element['#image_max_size']) && ($max = gmaps_pixels_parse($element['#image_max_size']))) {
      $ranges['max'] = gmaps_pixels_render($max, FALSE);
    }
    if (!empty($ranges)) {
      $validators['file_validate_image_resolution'] = array($ranges['max'], $ranges['min']);
    }

    if (isset($element['#image_max_filesize']) && !empty($element['#image_max_filesize'])) {
      $validators['file_validate_size'] = array($element['#image_max_filesize'] * 1024);
    }
    if (isset($element['#image_validators'])) {
      $validators = array_merge($validators, $element['#image_validators']);
    }
    if ($file = file_save_upload($source, $validators)) {
      //we should take care about things, if we have at least a destination directory
      if (isset($dest)) {
        if (isset($element['#file_name_pattern'])) {
          $tokens = array(
            '#name' => implode('-', $element['#parents']),
            '#upload_name' => $source,
            '#filename' => $file->filename,
            '#fid' => $file->fid,
          );
          $filename = str_replace(array_keys($tokens), $tokens, $element['#file_name_pattern']);
          $info = image_get_info($file->filepath);
          $filename .= '.'. $info['extension'];

          $new_file = new stdClass();
          $new_file->filename = $filename;
          //validator returns errors if the name is invalid
          if ($errors = file_validate_name_length($new_file)) {
            form_error($element, array_shift($errors));
            return;
          }
        }
        else {
          $filename = $file->filename;
        }
        $old_file = &$element['#gmaps_image_file'];
        $dest .= '/'. $filename;
        $update = isset($element['#file_replace_mode']) ? $element['#file_replace_mode'] : FILE_EXISTS_RENAME;
        //do NOT replace existing files in validation phase
        if ($old_file && $update == FILE_EXISTS_REPLACE && $old_file->filepath == $dest) { $move = TRUE; $update = FILE_EXISTS_RENAME; }

        if (file_move($file, $dest, $update)) {
          //delete previous upload
          if ($storage_cached) {
            if ($form_state['_gmaps_image_file_storage']['last_upload'] == 'move') {
              $previous = array_pop($form_state['_gmaps_image_file_storage']['move']);
            }
            else {
              $previous = array_pop($form_state['_gmaps_image_file_storage']['save']);
            }
            if ($previous) {
              $form_state['_gmaps_image_file_storage']['delete'][] = $previous;
            }
          }
          if ($move) {
            //we created a tmp file to avoid file overwrite in validation phase.
            $tmp_file = drupal_clone($file);
            $tmp_file->destination = $dest;
            $form_state['_gmaps_image_file_storage']['move'][] = $tmp_file;
            $form_state['_gmaps_image_file_storage']['last_upload'] = 'move';
            //the file already exists, although we will replace it in submit phase.
            $file->filepath = $dest;
            $file->filename = $filename;
            //remove old files record, but skip file_delete()
            if (!$storage_cached) {
              $old_file->filepath = NULL;
              $form_state['_gmaps_image_file_storage']['delete'][] = $old_file;
            }
          }
          else {
            $form_state['_gmaps_image_file_storage']['save'][] = $file;
            $form_state['_gmaps_image_file_storage']['last_upload'] = 'save';
            if (!$storage_cached && $old_file) {
              $form_state['_gmaps_image_file_storage']['delete'][] = $old_file;
            }
          }
        }
        else {
          form_error($element, t('Failed to copy the image to the specified location %loc.', array('%loc' => $filename)));
          return;
        }

      }
      $element['#value'] = $file;
      form_set_value($element, $element['#value'], $form_state);
      $gmaps_image_file_form_id = $form_state['values']['form_id'];
      $gmaps_image_file_build_id = $form_state['values']['gif_build_id'];
      cache_set('gif_'. $gmaps_image_file_build_id .'_'. $element['#id'], $file, 'cache_form', time() + 21600);
      cache_set('gif_storage_'. $gmaps_image_file_build_id, $form_state['_gmaps_image_file_storage'], 'cache_form', time() + 21600);
    }
  }
  //re-validation: use previous upload, if any
  else if ($cached = cache_get('gif_'. $form_state['values']['gif_build_id'] .'_'. $element['#id'], 'cache_form')) {
    $element['#value'] = $cached->data;
    form_set_value($element, $element['#value'], $form_state);
    $gmaps_image_file_form_id = $form_state['values']['form_id'];
    $gmaps_image_file_build_id = $form_state['values']['gif_build_id'];
  }
  //no new upload, no previous upload, but we have an old file
  else if ($file = $element['#gmaps_image_file']) {
    //sign for delete if needed
    if ($element['#value']['delete']) {
      $form_state['_gmaps_image_file_storage']['delete'][] = $file;
      $element['#value'] = NULL;
      form_set_value($element, $element['#value'], $form_state);
    }
    //pass back the file object
    else {
      $element['#value'] = $file;
      form_set_value($element, $element['#value'], $form_state);
    }
  }
  else if ($element['#required']) {
    form_error($element, $t('!name field is required.', array('!name' => $element['#title'])));
  }
  else {
    $element['#value'] = NULL;
    form_set_value($element, $element['#value'], $form_state);
  }
}

function _gmaps_image_file_pre_render($element) {
  $description = '';
  if (isset($element['#image_min_size'])) {
    $description = t('Minimum dimension are %dimensions.', array('%dimensions' => gmaps_pixels_render($element['#image_min_size'])));
  }
  if (isset($element['#image_max_size'])) {
    $description .= (empty($description) ? '' : ' ') . t('Maximum dimension are %dimensions.', array('%dimensions' => gmaps_pixels_render($element['#image_max_size'])));
  }
  if (isset($element['#image_max_filesize'])) {
    $description .= (empty($description) ? '' : ' ') . t('Maximum file size is %size kB.', array('%size' => $element['#image_max_filesize']));
  }
  $element['#description'] = empty($element['#description']) ? $description : ($element['#description'] .'<p>'. $description .'</p>');

  return $element;
}

/**
 * Automatically added to handlers by gmaps_image_file element if needed.
 */
function _gmaps_image_file_submit_handler($form, &$form_state) {
  global $gmaps_image_file_form_id, $gmaps_image_file_build_id;

  if ($storage = &$form_state['_gmaps_image_file_storage']) {
    //save new files
    foreach($storage['save'] as $file) {
      db_query("UPDATE {files} SET filename = '%s', filepath = '%s', status = %d WHERE fid = %d",array($file->filename, $file->filepath, FILE_STATUS_PERMANENT, $file->fid));
    }
    //delete old files
    foreach($storage['delete'] as $file) {
      if (empty($file->filepath) || file_delete($file->filepath)) {
        db_query('DELETE FROM {files} WHERE fid = %d', $file->fid);
      }
    }
    //replace old files by new ones
    foreach($storage['move'] as $file) {
      if (file_move($file, $file->destination, FILE_EXISTS_REPLACE)) {
        db_query("UPDATE {files} SET filename = '%s', filepath = '%s', status = %d WHERE fid = %d",array($file->filename, $file->filepath, FILE_STATUS_PERMANENT, $file->fid));
      }
    }
  }
  //skip pre_render
  $gmaps_image_file_form_id = $gmaps_image_file_build_id = NULL;

  //clean-up cache
  cache_clear_all('gif_'. $form_state['values']['gif_build_id'], 'cache_form', TRUE);
  cache_clear_all('gif_storage'. $form_state['values']['gif_build_id'], 'cache_form');
}

function _gmaps_image_file_form_pre_render($form) {
  global $gmaps_image_file_form_id, $gmaps_image_file_build_id;
  static $edit, $form_state;

  //do anything only if the form_id was set in validation phase.
  if (!empty($gmaps_image_file_form_id)) {
    //main form
    if ($form['#type'] == 'form' && $gmaps_image_file_form_id == $form['form_id']['#value'] && empty($form_state)) {
      $edit = FALSE;
      $form_state = array('values' => array('gif_build_id' => $form['gif_build_id']['#value']));
      $form = _gmaps_image_file_form_pre_render($form);
      $gmaps_image_file_form_id = $gmaps_image_file_build_id = $edit = $form_state = NULL;
    }
    else if (!empty($form_state)) {
      foreach(element_children($form) as $key) {
        $form[$key] = _gmaps_image_file_form_pre_render($form[$key]);
      }

      if ($form['#type'] == 'gmaps_image_file') {
        //re-expand
        $form = _gmaps_image_file_expand($form, $edit, $form_state);
      }
    }
  }

  return $form;
}




/**************
 * Themeables *
 **************/

/**
 * Format a gmaps_image_file element.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 * @return
 *   A themed HTML string representing the gmaps_image_file element.
 *
 * @ingroup themeable
 */
function theme_gmaps_image_file($element) {
  _form_set_class($element, array('form-gmaps-image-file'));
  preg_replace('|(\s*?)error(\s*?)|', '$1gmaps-image-file-error$2', $element['#attributes']['class']);
  return theme('form_element', $element, '<div id="'. $element['#id'] .'"'. drupal_attributes($element['#attributes']) .'>'. $element['#children'] . (isset($element['#gmaps_image_file']) ? '<div class="gmaps-image-file-clear-preview"></div>' : '').'</div>');
}
