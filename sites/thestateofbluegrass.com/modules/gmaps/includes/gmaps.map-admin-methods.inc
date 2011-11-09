<?php
// $Id: gmaps.map-admin-methods.inc,v 1.1.2.6 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Method administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_googlebar($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'showonload' => 0,
      'newstyle' => 0,
      'ads' => array(
        'safety' => 'high',
      ),
      'link_target' => '_blank',
      'listing_types' => 'blended',
      'result_list' => 'inline',
      'suppress_selection' => 0,
      'suppress_zoom' => 0,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_googlebar($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_adsmanager($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'maxads' => 3,
      'minzoom' => 6,
      'position' => array(
        'anchor' => GMAPS_CONTROL_POSITION_DEFAULT,
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_adsmanager($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_method_form_adsmanager_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_traffic($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'incidents' => 1,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_traffic($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_streetview($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'opensvu' => 1,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_streetview($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_method_form_streetview_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_layers($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'panoramio' => 1,
      'youtube' => 0,
      'webcams' => 0,
      'wikipedia' => 1,
      'wikipedia_en' => 1,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_layers($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_method_form_layers_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_keydragzoom($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'key' => 'shift',
      'border' => 'thin solid #FF0000',
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_keydragzoom($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_method_hook().
 */
function gmaps_gmaps_map_method_auto_center_zoom($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'mode' => GMAPS_MAP_ACZ_MODE_BOTH,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_method_form_auto_center_zoom($a1, $a2);
  }
}




/**********************
 * Internal functions *
 **********************/

function _gmaps_map_method_form_googlebar($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->method_data['googlebar'];
  $type_form = array();

  $type_form['showonload'] = array('#type' => 'checkbox',
    '#title' => t('Show by default'),
    '#default_value' => $opts['showonload'],
  );
  $type_form['newstyle'] = array('#type' => 'checkbox',
    '#title' => t('Use new style'),
    '#default_value' => 0,//$opts['newstyle'],
    '#disabled' => TRUE,
    '#description' => t('Disabled, because it is bogus.'),
  );
  $type_form['ads'] = array('#type' => 'fieldset',
    '#title' => t('Ads'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $type_form['ads']['client'] = array('#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => $opts['ads']['client'],
  );
  $type_form['ads']['channel'] = array('#type' => 'textfield',
    '#title' => t('Channel ID'),
    '#default_value' => $opts['ads']['channel'],
  );
  $type_form['ads']['safety'] = array('#type' => 'select',
    '#title' => t('Ad safety'),
    '#options' => array(
      'high' => t('High - No adult ads'),
      'medium' => t('Medium - Adult ads when requested, but no porn'),
      'low' => t('Low - Adult ads when requested'),
      'off' => t('No safety'),
    ),
    '#default_value' => $opts['ads']['safety'],
  );
  $type_form['link_target'] = array('#type' => 'select',
    '#title' => t('Target for links in search results'),
    '#options' => array(
      '_blank' => t('Blank'),
      '_self' => t('Self'),
      '_parent' => t('Parent'),
      '_top' => t('Top'),
    ),
    '#default_value' => $opts['link_target'],
  );
  $type_form['listing_types'] = array('#type' => 'select',
    '#title' => t('Listing types'),
    '#options' => array(
      'blended' => t('Blended'),
      'kmlonly' => t('Indexed KML/KMZ/GeoRSS files only'),
      'localonly' => t('Local business and geocode results only'),
    ),
    '#default_value' => $opts['linktarget'],
  );
  $type_form['result_list'] = array('#type' => 'select',
    '#title' => t('Result list style'),
    '#options' => array(
      'inline' => t('Inline'),
      'suppress' => t('Suppressed'),
      'bb' => t('Bulletin board'),
    ),
    '#default_value' => $opts['result_list'],
    '#description' => t('The Bulletin board is a container below the map.'),
  );
  $type_form['suppress_selection'] = array('#type' => 'checkbox',
    '#title' => t('Suppress initial result selection'),
    '#default_value' => $opts['suppress_selection'],
  );
  $type_form['suppress_zoom'] = array('#type' => 'checkbox',
    '#title' => t('Suppress zoom to bounds'),
    '#default_value' => $opts['suppress_zoom'],
  );

  return $type_form;
}

function _gmaps_map_method_form_adsmanager($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->method_data['adsmanager'];
  $type_form = array();

  $type_form['client'] = array('#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => $opts['client'],
  );
  $type_form['channel'] = array('#type' => 'textfield',
    '#title' => t('Channel ID'),
    '#default_value' => $opts['channel'],
  );
  $type_form['maxads'] = array('#type' => 'textfield',
    '#title' => t('Maximum number of ads'),
    '#default_value' => $opts['maxads'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 3,
  );
  $type_form['minzoom'] = array('#type' => 'select',
    '#title' => t('Minimum zoom level at which to show ads'),
    '#default_value' => $opts['minzoom'],
    '#options' => _gmaps_map_get_zoom_levels($form['#gmaps_map_info']->maptypes),
  );
  $type_form['position'] = array('#type' => 'gmaps_control_position',
    '#title' => t('Position'),
    '#default_value' => $opts['position'],
    '#collapsible' => TRUE,
    '#collapsed' => (empty($opts['position']) || $opts['position']['anchor'] == GMAPS_CONTROL_POSITION_DEFAULT),
  );
  
  return $type_form;
}

function _gmaps_map_method_form_adsmanager_validate($form, &$form_state) {
  $methods = &$form_state['values']['methods'];
  $adm = &$form_state['values']['method_data']['adsmanager'];

  if (intval($adm['maxads']) != $adm['maxads'] || $adm['maxads'] <= 0) {
    form_set_error('method_data][adsmanager][maxads', t('The AdSense manager maximum ads must be an integer greater than zero.'));
  }
  if (isset($methods['adsmanager']) && empty($adm['client'])) {
    form_set_error('method_data][adsmanager][client', t('The client ID is required when the AdSense manager is enabled.'));
  }
}

function _gmaps_map_method_form_traffic($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->method_data['traffic'];
  $type_form = array();

  $type_form['incidents'] = array('#type' => 'checkbox',
    '#title' => t('Show incidents'),
    '#default_value' => $opts['incidents'],
  );

  return $type_form;
}

function _gmaps_map_method_form_streetview($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->method_data['streetview'];
  $type_form = array();

  $type_form['opensvu'] = array('#type' => 'checkbox',
    '#title' => t('Open Street view on click'),
    '#default_value' => $opts['opensvu'],
  );

  return $type_form;
}

function _gmaps_map_method_form_streetview_validate($form, &$form_state) {
  $svu = &$form_state['values']['map_data']['svu'];
  $methods = &$form_state['values']['methods'];
  $sv_method = &$form_state['values']['method_data']['streetview'];

  if (in_array('streetview', $methods) && $sv_method['opensvu'] && !$svu['enabled']) {
    form_set_error('method_data][streetview][opensvu', t('Street view must be enabled when its support method on click option is set to true.'));
  }
}

function _gmaps_map_method_form_layers($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->method_data['layers'];
  $type_form = array();

  $type_form['panoramio'] = array('#type' => 'checkbox',
    '#title' => t('Show Panoramio photos'),
    '#default_value' => $opts['panoramio'],
  );
  $type_form['youtube'] = array('#type' => 'checkbox',
    '#title' => t('Show YouTube videos'),
    '#default_value' => $opts['youtube'],
  );
  $type_form['webcams'] = array('#type' => 'checkbox',
    '#title' => t('Show Webcams.travel cameras'),
    '#default_value' => $opts['webcams'],
  );
  
  $type_form['wikipedia'] = array('#type' => 'select',
    '#title' => t('Geotagged Wikipedia layers'),
    '#default_value' => $opts['wikipedia'],
    '#options' => array(t('Disabled'), t('Current language only'), t('All enabled languages')),
  );
  $type_form['wikipedia_en'] = array('#type' => 'checkbox',
    '#title' => t('Force english Wikipedia'),
    '#default_value' => $opts['wikipedia_en'],
    '#description' => t('Always add english if Wikipedia layers are enabled at the above option.'),
  );
    
  return $type_form;
}

function _gmaps_map_method_form_layers_validate($form, &$form_state) {
  $methods = &$form_state['values']['methods'];
  $layers = &$form_state['values']['method_data']['layers'];

  if (isset($methods['layers']) && !$layers['panoramio'] && !$layers['wikipedia']) {
    form_set_error('method_data][layers][panoramio', t('Neither Panoramio and Wikipedia layers are enabled.'));
  }
}

function _gmaps_map_method_form_keydragzoom($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  if (!($opts = &$map->method_data['keydragzoom'])) {
    $opts = gmaps_gmaps_map_method_keydragzoom('defaults', array());
  }
  $type_form = array();

  $type_form['key'] = array('#type' => 'select',
    '#title' => t('Hot key to activate a drag zoom'),
    '#default_value' => $opts['key'],
    '#options' => array(
      'shift' => t('Shift'),
      'ctrl' => t('Ctrl'),
      'alt' => t('Alt')),
  );
  $type_form['border'] = array('#type' => 'textfield',
    '#title' => t('Zoom box border style'),
    '#default_value' => $opts['border'],
    '#required' => TRUE,
    '#description' => t('CSS rule.'),
  );
    
  return $type_form;
}

function _gmaps_map_method_form_auto_center_zoom($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  if (!($opts = &$map->method_data['auto_center_zoom'])) {
    $opts = gmaps_gmaps_map_method_auto_center_zoom('defaults', array());
  }
  $type_form = array();

  $type_form['mode'] = array('#type' => 'radios',
    '#title' => t('Mode'),
    '#default_value' => $opts['mode'],
    '#options' => array(
      GMAPS_MAP_ACZ_MODE_BOTH => t('Center and zoom'),
      GMAPS_MAP_ACZ_MODE_CENTER => t('Center only'),
      GMAPS_MAP_ACZ_MODE_ZOOM => t('Zoom only')),
  );
    
  return $type_form;
}

