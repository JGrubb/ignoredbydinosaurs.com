<?php
// $Id: gmaps.map-admin-controls.inc,v 1.1.2.5 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Control administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_map_control_hook().
 */
function gmaps_gmaps_map_control_extnav($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'type' => 'large',
      'title' => array(
        'east' => 'east',
        'west' => 'west',
        'north' => 'north',
        'south' => 'south',
        'return' => 'home position',
        'zoomin' => 'zoom in',
        'zoomout' => 'zoom out',
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_control_form_extnav($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_control_hook().
 */
function gmaps_gmaps_map_control_maptype($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'shortnames' => 0,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_control_form_maptype($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_control_hook().
 */
function gmaps_gmaps_map_control_menumaptype($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'shortnames' => 0,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_control_form_menumaptype($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_control_hook().
 */
function gmaps_gmaps_map_control_extmaptype($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'traffic' => 1,
      'traffickey' => 1,
      'more' => 1,
      'save' => 0,
      'pos_right' => 0,
      'maptype' => 'extmaptype',
    );
  }
  else if($op == 'form') {
    return _gmaps_map_control_form_extmaptype($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_control_form_extmaptype_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_control_hook().
 */
function gmaps_gmaps_map_control_extdragzoom($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'boxstyle' => array(
        'fillcolor' => '#000',
        'border' => '2px solid blue',
        'opacity' => 0.2,
      ),
      'other' => array(
        'button_html' => '{zoom}',
        'zooming_html' => '{Drag a region on the map}',
        'back_html' => '{zoom back}',
        'overlay_time' => 2000,
        'dragsize' => 5,
        'back_button' => 1,
        'zoom_out' => 1,
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_control_form_extdragzoom($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_control_form_extdragzoom_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_control_hook().
 */
function gmaps_gmaps_map_control_breadcrumb($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'level' => 0,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_control_form_breadcrumb($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_control_form_breadcrumb_validate($a1, $a2);
  }
}





/**********************
 * Internal functions *
 **********************/

function _gmaps_map_control_form_extnav($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->control_data['extnav'];
  $type_form = array();

  $type_form['type'] = array('#type' => 'select',
    '#title' => t('Type'),
    '#default_value' => $opts['type'],
    '#options' => array('large' => t('Large'), 'small' => t('Small')),
  );
  $type_form['title'] = array('#type' => 'fieldset',
    '#title' => t('Titles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('All titles are <strong>TRANSLATABLE</strong>.'),
  );
  $type_form['title']['east'] = array('#type' => 'textfield',
    '#title' => t('Move east button title'),
    '#default_value' => $opts['title']['east'],
    '#required' => TRUE,
  );
  $type_form['title']['west'] = array('#type' => 'textfield',
    '#title' => t('Move west button title'),
    '#default_value' => $opts['title']['west'],
    '#required' => TRUE,
  );
  $type_form['title']['north'] = array('#type' => 'textfield',
    '#title' => t('Move north button title'),
    '#default_value' => $opts['title']['north'],
    '#required' => TRUE,
  );
  $type_form['title']['south'] = array('#type' => 'textfield',
    '#title' => t('Move south button title'),
    '#default_value' => $opts['title']['south'],
    '#required' => TRUE,
  );
  $type_form['title']['return'] = array('#type' => 'textfield',
    '#title' => t('Return button title'),
    '#default_value' => $opts['title']['return'],
    '#required' => TRUE,
  );
  $type_form['title']['zoomin'] = array('#type' => 'textfield',
    '#title' => t('Zoom in button title'),
    '#default_value' => $opts['title']['zoomin'],
    '#required' => TRUE,
  );
  $type_form['title']['zoomout'] = array('#type' => 'textfield',
    '#title' => t('Zoom out button title'),
    '#default_value' => $opts['title']['zoomout'],
    '#required' => TRUE,
  );
  
  return $type_form;
}

function _gmaps_map_control_form_maptype($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->control_data['maptype'];
  $type_form = array();

  $type_form['shortnames'] = array('#type' => 'checkbox',
    '#title' => t('Use short names'),
    '#default_value' => $opts['shortnames'],
  );

  return $type_form;
}

function _gmaps_map_control_form_menumaptype($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->control_data['menumaptype'];
  $type_form = array();

  $type_form['shortnames'] = array('#type' => 'checkbox',
    '#title' => t('Use short names'),
    '#default_value' => $opts['shortnames'],
  );

  return $type_form;
}

function _gmaps_map_control_form_extmaptype($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->control_data['extmaptype'];
  $type_form = array();

  $type_form['traffic'] = array('#type' => 'checkbox',
    '#title' => t('Show traffic button'),
    '#default_value' => $opts['traffic'],
  );
  $type_form['traffickey'] = array('#type' => 'checkbox',
    '#title' => t('Show traffic help button'),
    '#default_value' => $opts['traffickey'],
  );
  $type_form['more'] = array('#type' => 'checkbox',
    '#title' => t('Show more button'),
    '#default_value' => $opts['more'],
  );
  $type_form['save'] = array('#type' => 'checkbox',
    '#title' => t('Show save and restore buttons'),
    '#default_value' => $opts['save'],
  );
  $info = gmaps_map_get_info($map->type);
  $type_form['maptype'] = array('#type' => 'select',
    '#title' => t('Map type control'),
    '#default_value' => $opts['maptype'],
    '#options' => $info->control_types['maptype']->controls_names,
    '#description' => t('Which map type control to use to build the map type buttons.'),
  );
  $type_form['pos_right'] = array('#type' => 'textfield',
    '#title' => t('Right position'),
    '#default_value' => $opts['pos_right'],
    '#size' => 5,
    '#maxlength' => 3,
    '#field_suffix' => 'px',
    '#description' => t('The distance of the more and traffic buttons from the right side of the control, when an external map type control is used to build the map type buttons.'),
  );
  
  return $type_form;
}

function _gmaps_map_control_form_extmaptype_validate($form, &$form_state) {
  $values = &$form_state['values']['controls']['maptype']['control_data']['extmaptype'];
  if ($values['maptype'] != 'extmaptype') {
    if (empty($values['pos_right'])) {
      form_set_error('controls][maptype][control_data][extmaptype][pos_right', t('The right position of the extended map type control is required, when the map type buttons are built by an external map type control.'));
    }
    else if (intval($values['pos_right']) != $values['pos_right'] || $values['pos_right'] <= 0) {
      form_set_error('controls][maptype][control_data][extmaptype][pos_right', t('The right position of the extended map type control must be an integer greater than zero.'));
    }
  }
}

function _gmaps_map_control_form_extdragzoom($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->control_data['extdragzoom'];
  $type_form = array();

  $type_form['boxstyle'] = array('#type' => 'fieldset',
    '#title' => t('Box style'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    );
  $type_form['boxstyle']['fillcolor'] = array('#type' => 'textfield',
    '#title' => t('Fill color'),
    '#default_value' => $opts['boxstyle']['fillcolor'],
    '#required' => TRUE,
    '#description' => t('Any CSS color rule.'),
  );
  $type_form['boxstyle']['border'] = array('#type' => 'textfield',
    '#title' => t('Border'),
    '#default_value' => $opts['boxstyle']['border'],
    '#required' => TRUE,
    '#description' => t('CSS rule. Only named colors are allowed.'),
  );
  $type_form['boxstyle']['opacity'] = array('#type' => 'textfield',
    '#title' => t('Opacity'),
    '#default_value' => $opts['boxstyle']['opacity'],
    '#required' => TRUE,
  );
  
  $type_form['other'] = array('#type' => 'fieldset',
    '#title' => t('Others'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('Wrap translatable strings in curly brackets - {} - in *button html fields.'),
  );
  $type_form['other']['button_html'] = array('#type' => 'textfield',
    '#title' => t('Button html'),
    '#default_value' => $opts['other']['button_html'],
    '#required' => TRUE,
  );
  $type_form['other']['zooming_html'] = array('#type' => 'textfield',
    '#title' => t('Zooming button html'),
    '#default_value' => $opts['other']['zooming_html'],
    '#required' => TRUE,
  );
  $type_form['other']['back_html'] = array('#type' => 'textfield',
    '#title' => t('Back button html'),
    '#default_value' => $opts['other']['back_html'],
    '#required' => TRUE,
  );
  $type_form['other']['overlay_time'] = array('#type' => 'textfield',
    '#title' => t('Overlay remove time'),
    '#default_value' => $opts['other']['overlay_time'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 5,
    '#field_suffix' => 'ms',
  );
  $type_form['other']['dragsize'] = array('#type' => 'textfield',
    '#title' => t('Minimum drag size'),
    '#default_value' => $opts['other']['dragsize'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => 'px',
  );
  $type_form['other']['back_button'] = array('#type' => 'checkbox',
    '#title' => t('Back button enabled'),
    '#default_value' => $opts['other']['back_button'],
  );
  $type_form['other']['sticky_zoom'] = array('#type' => 'checkbox',
    '#title' => t('Sticky zoom enabled'),
    '#default_value' => $opts['other']['sticky_zoom'],
    '#description' => t('Whether or not the control stays in "zoom mode" until turned off. When enabled, the user can zoom repeatedly, until clicking on the zoom button again to turn zoom mode off.'),
  );
  $type_form['other']['zoom_out'] = array('#type' => 'checkbox',
    '#title' => t('Right click zoom out enabled'),
    '#default_value' => $opts['other']['zoom_out'],
    '#description' => t('Whether to zoom out when a drag with the right mouse button occurs.'),
  );

  $type_form['other']['style'] = array('#type' => 'fieldset',
    '#title' => t('Styles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $type_form['other']['style']['starting'] = array('#type' => 'textfield',
    '#title' => t('Button starting CSS class'),
    '#default_value' => $opts['other']['style']['starting'],
    '#description' => t('Common to both un-activated and activated state. Common: gmaps-map-control-extdragzoom-starting.'),
  );
  $type_form['other']['style']['button'] = array('#type' => 'textfield',
    '#title' => t('Button CSS class'),
    '#default_value' => $opts['other']['style']['button'],
    '#description' => t('Un-activated style. Common: gmaps-map-control-extdragzoom-button.'),
  );
  $type_form['other']['style']['zooming'] = array('#type' => 'textfield',
    '#title' => t('Button zooming CSS class'),
    '#default_value' => $opts['other']['style']['zooming'],
    '#description' => t('Activated style. Common: gmaps-map-control-extdragzoom-zooming.'),
  );
  $type_form['other']['style']['back'] = array('#type' => 'textfield',
    '#title' => t('Back button CSS class'),
    '#default_value' => $opts['other']['style']['back'],
    '#description' => t('Common: gmaps-map-control-extdragzoom-back.'),
  );
  
  return $type_form;
}

function _gmaps_map_control_form_extdragzoom_validate($form, &$form_state) {
  $ctrl = &$form_state['values']['controls']['dragzoom']['control_data']['extdragzoom']['boxstyle'];
  if ($ctrl['opacity'] < 0 || $ctrl['opacity'] > 1) {
    form_set_error('controls][dragzoom][control_data][extdragzoom][boxstyle][opacity', t('The opacity of the drag zoom box must be between zero and one.'));
  }
  
  $ctrl = &$form_state['values']['controls']['dragzoom']['control_data']['extdragzoom']['other'];
  if (intval($ctrl['overlay_time']) != $ctrl['overlay_time'] || $ctrl['overlay_time'] <= 0) {
    form_set_error('controls][dragzoom][control_data][extdragzoom][other][overlay_time', t('The overlay remove time of the extended drag zoom control must be an integer greater than zero.'));
  }
  if (intval($ctrl['dragsize']) != $ctrl['dragsize'] || $ctrl['dragsize'] < 0) {
    form_set_error('controls][dragzoom][control_data][extdragzoom][other][dragsize', t('The minimum drag size of the extended drag zoom control must be an integer greater than or equal to zero.'));
  }
}

function _gmaps_map_control_form_breadcrumb($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->control_data['breadcrumb'];
  
  if (!array_key_exists('level', (array)$opts)) {
    $opts['level'] = 0;
  }
  
  $type_form = array();

  $type_form['level'] = array('#type' => 'textfield',
    '#title' => t('Minimum address link level'),
    '#default_value' => $opts['level'],
    '#required' => TRUE,
    '#size' => 5,
    '#maxlength' => 2,
    '#description' => t('Set the minimal level of address which has a link in the label. For example, show address links for all addresses in "USA > NY >  Brooklyn" for level 0, and show address link only for "Brooklyn" if level is 2.'),
  );

  return $type_form;
}

function _gmaps_map_control_form_breadcrumb_validate($form, &$form_state) {
  $ctrl = &$form_state['values']['controls']['breadcrumb']['control_data']['breadcrumb'];
  if (intval($ctrl['level']) != $ctrl['level'] || $ctrl['level'] < 0) {
    form_set_error('controls][breadcrumb][control_data][breadcrumb][level', t('The minimum level of the breadcrumb control must be an integer greater than or equal to zero.'));
  }
}

