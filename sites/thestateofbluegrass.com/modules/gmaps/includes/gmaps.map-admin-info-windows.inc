<?php
// $Id: gmaps.map-admin-info-windows.inc,v 1.1.2.6 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Info window administration functions.
 */


/************************
 * Hook implementations *
 ************************/


/**
 * Implementation of hook_gmaps_map_iw_hook().
 */
function gmaps_gmaps_map_iw_google($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'maxwidth' => 0,
      'nocloseonclick' => 0,
      'offset' => 0,
      'maximized' => 0,
      'blowup' => array(
        'zoom' => -1,
        'type' => '',
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_iw_form_google($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_iw_form_google_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_iw_hook().
 */
function gmaps_gmaps_map_iw_bb($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'maximized' => 0,
      'style' => 'tabs',
      'collapsible' => 1,
      'collapsed' => 0,
      'tabs_nav' => GMAPS_TABS_NAV_TOP,
      'tabs_state' => 'max',
      'grid' => array(
        'columns' => 2,
        'alignment' => GMAPS_GRID_ALIGN_VER,
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_iw_form_bb($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_iw_form_bb_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_iw_hook().
 */
function gmaps_gmaps_map_iw_extended($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'beak_offset' => 1,
      'padding' => 0,
      'max_panning' => 500,
      'nocloseonclick' => 0,
      'style' => 'tabs',
      'collapsible' => 1,
      'collapsed' => 0,
      'tabs_nav' => GMAPS_TABS_NAV_TOP,
      'tabs_state' => 'max',
      'grid' => array(
        'columns' => 2,
        'alignment' => GMAPS_GRID_ALIGN_VER,
      ),
    );
  }
  else if($op == 'form') {
    return _gmaps_map_iw_form_extended($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_iw_form_extended_validate($a1, $a2);
  }
}

/**
 * Implementation of hook_gmaps_map_iw_hook().
 */
function gmaps_gmaps_map_iw_tmc($op, $a1, &$a2 = NULL) {
  if($op == 'defaults') {
    return array(
      'maxwidth' => 0,
      'nocloseonclick' => 0,
      'offset' => 0,
      'maximized' => 0,
    );
  }
  else if($op == 'form') {
    return _gmaps_map_iw_form_tmc($a1, $a2);
  }
  else if($op == 'validate') {
    _gmaps_map_iw_form_tmc_validate($a1, $a2);
  }
}



/**********************
 * Internal functions *
 **********************/

function _gmaps_map_iw_form_google($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->iw_data['google'];
  $type_form = array();

  $type_form['options'] = array('#type' => 'gmaps_iw_data_google',
    '#default_value' => $opts,
    '#required' => TRUE,
    '#collapsible' => FALSE,
    '#map_type' => $map->type,
  );

  return $type_form;
}

function _gmaps_map_iw_form_google_validate($form, &$form_state) {
  $value = &$form_state['values']['iw_data']['google']['options'];
  $element = &$form['accordion']['iw_panel']['iw_data']['google'];
  form_set_value($element, $value, $form_state);
}

function _gmaps_map_iw_form_bb($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->iw_data['bb'];
  $type_form = array();

  $type_form['options'] = array('#type' => 'gmaps_iw_data_bb',
    '#default_value' => $opts,
    '#required' => TRUE,
    '#collapsible' => FALSE,
  );

  return $type_form;
}

function _gmaps_map_iw_form_bb_validate($form, &$form_state) {
  $value = &$form_state['values']['iw_data']['bb']['options'];
  $element = &$form['accordion']['iw_panel']['iw_data']['bb'];
  form_set_value($element, $value, $form_state);
}

function _gmaps_map_iw_form_extended($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->iw_data['extended'];
  $type_form = array();

  $type_form['options'] = array('#type' => 'gmaps_iw_data_extended',
    '#default_value' => $opts,
    '#required' => TRUE,
    '#collapsible' => FALSE,
  );
  
  return $type_form;
}

function _gmaps_map_iw_form_extended_validate($form, &$form_state) {
  $value = &$form_state['values']['iw_data']['extended']['options'];
  $element = &$form['accordion']['iw_panel']['iw_data']['extended'];
  form_set_value($element, $value, $form_state);
}

function _gmaps_map_iw_form_tmc($form, &$form_state) {
  $map = &$form['#gmaps_map'];
  $opts = &$map->iw_data['tmc'];
  $type_form = array();

  $type_form['options'] = array('#type' => 'gmaps_iw_data_tmc',
    '#default_value' => $opts,
    '#required' => TRUE,
    '#collapsible' => FALSE,
  );
  
  return $type_form;
}

function _gmaps_map_iw_form_tmc_validate($form, &$form_state) {
  $value = &$form_state['values']['iw_data']['tmc']['options'];
  $element = &$form['accordion']['iw_panel']['iw_data']['tmc'];
  form_set_value($element, $value, $form_state);
}
