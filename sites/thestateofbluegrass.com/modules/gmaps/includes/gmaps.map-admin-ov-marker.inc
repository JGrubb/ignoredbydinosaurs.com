<?php
// $Id: gmaps.map-admin-ov-marker.inc,v 1.1.2.6 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Marker overlays administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_map_ov_hook().
 */
function gmaps_gmaps_map_ov_marker($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'basic' => array(
        'icon' => 0,
        'protected_icon' => gmaps_icon_get_icid_for_default('protected_medium'),
        'info_window' => 'google'
      ),
      'options' => array(
        'clickable' => -1,
        'draggable' => 0,
        'hide' => 0,
        'autopan' => 1,
        'dcmove' => 0,
        'bouncy' => 0,
        'gravity' => 1,
      ),
      'labeled' => array(
        'enabled' => 1,
        'class' => 'gmaps-labeled-marker-label',
        'offset' => 0,
      ),
      'protected_labeled' => array(
        'enabled' => 1,
        'class' => 'gmaps-labeled-marker-protected-label',
        'offset' => '-8x-12',
      ),
      'action' => array(
        'hover' => '',
        'click' => 'content',
        'dblclick' => 'svu',
        'rightclick' => 'blowup',
      ),
      'tracker' => array(
        'enabled' => 0,
        'scale' => 0,
        'padding' => 20,
        'color' => '#ff0000',
        'weight' => 10,
        'length' => 20,
        'opacity' => 0.8,
        'update_event' => 'moveend',
        'quick_pan' => 1,
        'pan_event' => 'click',
      ),
      'route' => array(
        'enabled' => 0,
        'color' => '#ff0000',
        'weight' => 10,
        'opacity' => 1,
        'geodesic' => 0,
      ),
      'manager' => array(
        'manager' => '',
        'mm' => array(
          'padding' => 0,
          'maxzoom' => -1,
          'trackmarkers' => 0,
        ),
        'clusterer' => array(
          'gridsize' => 60,
          'maxzoom' => -1,
        ),
      ),
      'misc' => array(
        'ajaxcontent' => 1,
        'link_target' => '_self',
        'labels' => 'capital',
        'center_first' => 0,
        'zoom_first' => 0,
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_ov_form_marker($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_ov_form_marker_validate($a1, $a2);
  }
}




/**********************
 * Internal functions *
 **********************/

function _gmaps_map_ov_form_marker($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $info = &$form['#gmaps_map_info'];
  $opts = &$map->ov_data['marker'];
  $type_form = array();
  
  $icons = gmaps_get_icons();
  
  //basic
  $type_form['basic'] = array('#type' => 'gmaps_marker_basic',
    '#title' => t('Basic options'),
    '#default_value' => $opts['basic'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  //options
  $type_form['options'] = array('#type' => 'gmaps_marker_options',
    '#title' => t('Marker options'),
    '#default_value' => $opts['options'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  //labeled markers
  $type_form['labeled'] = array('#type' => 'gmaps_marker_labeled',
    '#title' => t('Labeled marker options'),
    '#default_value' => $opts['labeled'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Labeled marker will be created when a marker has label, but its icon does not support labels.'),
  );
  $type_form['protected_labeled'] = array('#type' => 'gmaps_marker_labeled',
    '#title' => t('Protected labeled marker options'),
    '#default_value' => $opts['protected_labeled'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  //actions
  $type_form['action'] = array('#type' => 'gmaps_marker_action',
    '#title' => t('Actions'),
    '#default_value' => $opts['action'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
    
  //tracker
  $type_form['tracker'] = array('#type' => 'gmaps_marker_tracker',
    '#title' => t('Tracker'),
    '#default_value' => $opts['tracker'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  //routes
  $type_form['route'] = array('#type' => 'fieldset',
    '#title' => t('Route'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('A marker route is a line between markers that follows the order the markers has been added to the map.'),
  );
  $type_form['route']['enabled'] = array('#type' => 'checkbox',
    '#title' => t('Enable routes'),
    '#default_value' => $opts['route']['enabled'],
  );
  $type_form['route']['color'] = array('#type' => 'textfield',
    '#title' => t('Color'),
    '#default_value' => $opts['route']['color'],
    '#required' => TRUE,
    '#size' => 10,
  );
  $type_form['route']['weight'] = array('#type' => 'textfield',
    '#title' => t('Weight'),
    '#default_value' => $opts['route']['weight'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => 'px',
    '#description' => t('Thickness of the line.'),
  );
  $type_form['route']['opacity'] = array('#type' => 'textfield',
    '#title' => t('Opacity'),
    '#default_value' => $opts['route']['opacity'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#descripion' => t('A number between 0 and 1.'),
  );
  $type_form['route']['geodesic'] = array('#type' => 'checkbox',
    '#title' => t('Geodesic line'),
    '#default_value' => $opts['route']['geodesic'],
    '#description' => t('It has any sense, when the distance between markers is larger.'),
  );
  
  //manager
  $type_form['manager'] = array('#type' => 'fieldset',
    '#title' => t('Manager'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $type_form['manager']['manager'] = array('#type' => 'select',
    '#title' => t('Manager'),
    '#default_value' => $opts['manager']['manager'],
    '#options' => array(
      '' => t('Disabled'),
      'google' => t('API default'),
      'extended' => t('Extended manager'),
      'clusterer' => t('Clusterer'),
   ),
  );
  
  //manager - manager
  $type_form['manager']['mm'] = array('#type' => 'fieldset',
    '#title' => t('Marker manager settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $type_form['manager']['mm']['padding'] = array('#type' => 'textfield',
    '#title' => t('Border padding'),
    '#default_value' => $opts['manager']['mm']['padding'],
    '#required' => TRUE,
    '#maxlength' => 4,
    '#size' => 6,
    '#field_suffix' => 'px',
    '#description' => t('Specifies, in pixels, the extra padding outside the map\'s current viewport monitored by a manager. Markers that fall within this padding are added to the map, even if they are not fully visible.'),
  );
  $type_form['manager']['mm']['maxzoom'] = array('#type' => 'select',
    '#title' => t('Maximum zoom level'),
    '#options' => array(-1 => t('Auto')) + _gmaps_map_get_zoom_levels($info->maptypes),
    '#default_value' => $opts['manager']['mm']['maxzoom'],
    '#description' => t('Set it to 0 (zero) to use the maximum of the map type.'),
  );
  $type_form['manager']['mm']['trackmarkers'] = array('#type' => 'checkbox',
    '#title' => t('Track markers'),
    '#default_value' => $opts['manager']['mm']['trackmarkers'],
    '#description' => t('Indicates whether or not a marker manager should track markers\' movements. If you wish to move managed markers using the setPoint method, this option should be enabled.'),
  );
  
  //manager - clusterer
  $type_form['manager']['clusterer'] = array('#type' => 'fieldset',
    '#title' => t('Clusterer settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $type_form['manager']['clusterer']['gridsize'] = array('#type' => 'textfield',
    '#title' => t('Grid size'),
    '#default_value' => $opts['manager']['clusterer']['gridsize'],
    '#required' => TRUE,
    '#maxlength' => 4,
    '#size' => 6,
    '#field_suffix' => 'px',
    '#description' => t('The grid size of a cluster in pixel. Each cluster will be a square. If you want the algorithm to run faster, you can set this value larger.'),
  );
  $type_form['manager']['clusterer']['maxzoom'] = array('#type' => 'select',
    '#title' => t('Maximum zoom level'),
    '#options' => array(-1 => t('Auto')) + _gmaps_map_get_zoom_levels($info->maptypes),
    '#default_value' => $opts['manager']['clusterer']['maxzoom'],
    '#description' => t('Set it to 0 (zero) to use the maximum of the map type.'),
  );
  $type_form['manager']['clusterer']['styles'] = array('#type' => 'fieldset',
    '#title' => t('Styles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  for ($i = 0; $i < GMAPS_MAP_CLUSTERER_MAX_STYLES; $i++) {
    $type_form['manager']['clusterer']['styles'][$i] = array('#type' => 'fieldset',
      '#title' => '#'. ($i+1),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $type_form['manager']['clusterer']['styles'][$i]['icon'] = array('#type' => 'gmaps_icon_select',
      '#title' => t('Icon'),
      '#default_value' => $opts['manager']['clusterer']['styles'][$i]['icon'],
      '#icons' => array(0 => t('Auto')) + $icons,
    );
    $type_form['manager']['clusterer']['styles'][$i]['color'] = array('#type' => 'textfield',
      '#title' => t('Text color'),
      '#default_value' => $opts['manager']['clusterer']['styles'][$i]['color'],
    );
    $type_form['manager']['clusterer']['styles'][$i]['anchor'] = array('#type' => 'gmaps_pixels',
      '#title' => t('Text anchor'),
      '#default_value' => $opts['manager']['clusterer']['styles'][$i]['anchor'],
      '#description' => t('Leave empty to auto align center and middle.'),
    );
  }
  
  $type_form['misc'] = array('#type' => 'fieldset',
    '#title' => t('Miscellaneous'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $type_form['misc']['ajaxcontent'] = array('#type' => 'checkbox',
    '#title' => t('Enable AJAX content loading'),
    '#default_value' => $opts['misc']['ajaxcontent'],
    '#description' => t('This option is automatically disabled, if the TOC map option is enabled and its style does not support ajax loading, or when the current client is a search engine and its support is enabled.'),
  );
  $type_form['misc']['link_target'] = array('#type' => 'select',
    '#title' => t('Link target'),
    '#options' => array(
      '_blank' => t('Blank'),
      '_self' => t('Self'),
      '_parent' => t('Parent'),
      '_top' => t('Top'),
    ),
    '#default_value' => $opts['misc']['link_target'],
  );
  $type_form['misc']['labels'] = array('#type' => 'select',
    '#title' => t('Labels'),
    '#options' => array(
      '' => t('Disabled'),
      'numeric' => t('Numeric sequence'),
      'capital' => t('Capitals'),
      'letter' => t('Small letters'),
      'word' => t('Capitals, small letters, numbers'),
      'ascii' => t('Printable ASCII characters'),
      'custom' => t('Custom sequence'),
      'static' => t('Static'),
    ),
    '#default_value' => $opts['misc']['labels'],
    '#description' => t('When set to static, then labels assigned to markers will be used.'),
  );
  $type_form['misc']['custom_labels'] = array('#type' => 'textfield',
    '#title' => t('Custom labels'),
    '#default_value' => $opts['misc']['custom_labels'],
    '#description' => t('Pipe (|) separated list of labels.'),
  );
  $type_form['misc']['center_first'] = array('#type' => 'checkbox',
    '#title' => t('Center map to first marker'),
    '#default_value' => $opts['misc']['center_first'],
  );
  $type_form['misc']['zoom_first'] = array('#type' => 'checkbox',
    '#title' => t('Zoom map to first marker'),
    '#default_value' => $opts['misc']['zoom_first'],
  );
  
  return $type_form;
}

function _gmaps_map_ov_form_marker_validate($form, &$form_state) {
  $marker = &$form_state['values']['ov_data']['marker'];

  if ($marker['misc']['labels'] == 'custom' && (empty($marker['misc']['custom_labels']) || count(explode('|', $marker['misc']['custom_labels'])) < 2)) {
    form_set_error('ov_data][marker][misc][custom_labels', t('At least two custom labels should be provided, when labels are set to custom.'));
  }
  
  //route
  if (!preg_match('|#[0-9A-Fa-f]{6,6}|', $marker['route']['color'])) {
    form_set_error('ov_data][marker][route][color', t('The color of the marker route must be given as a hexadecimal RGB value prefixed by a hashmark.'));
  }
  if (intval($marker['route']['weight']) != $marker['route']['weight'] || $marker['route']['weight'] <= 0) {
    form_set_error('ov_data][marker][route][weight', t('The weight of the marker route must be an integer greater than zero.'));
  }
  if ($marker['route']['opacity'] < 0 || $marker['route']['opacity'] > 1) {
    form_set_error('ov_data][marker][route][opacity', t('The opacity of the marker route must be between zero and one.'));
  }

  //manager - manager
  $mm = &$marker['manager']['mm'];
  if (intval($mm['padding']) != $mm['padding'] || $mm['padding'] < 0) {
    form_set_error('ov_data][marker][manager][mm][padding', t('The marker manager padding must be an integer greater than or equal to zero.'));
  }

  //manager - clusterer
  $ctr = &$marker['manager']['clusterer'];
  if (intval($ctr['gridsize']) != $ctr['gridsize'] || $ctr['gridsize'] <= 0) {
    form_set_error('ov_data][marker][manager][clusterer][gridsize', t('The marker clusterer grid size must be an integer greater than zero.'));
  }
}
