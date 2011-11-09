<?php
// $Id: gmaps_devel.geocoder-form.inc,v 1.1.2.4 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * gmaps_geocoder test form.
 */

function gmaps_devel_geocoder_form(&$form_state = NULL) {
  $form = array();

  $form['search'] = array('#type' => 'gmaps_geocoder',
    '#title' => 'Search address',
    '#default_value' => 'Budapest, Hungary',
    '#attributes' => array('class' => 'gmaps-devel-geocoder'),
  );
  $form['raw_result'] = array('#type' => 'item',
    '#title' => t('Raw result'),
    '#value' => '<pre id="gmaps-devel-geocoder-raw-result"></pre>',
  );
  $form['result'] = array('#type' => 'item',
    '#title' => t('Result'),
    '#value' => '<pre id="gmaps-devel-geocoder-result"></pre>',
  );

  $form['#validate'][] = '_gmaps_devel_geocoder_validate';
  $form['#submit'][] = '_gmaps_devel_geocoder_submit';
  $form['#pre_render'][] = '_gmaps_devel_geocoder_pre_render';

  return $form;
}

function _gmaps_devel_geocoder_validate($form, &$form_state) {
}

function _gmaps_devel_geocoder_submit($form, &$form_state) {
}

function _gmaps_devel_geocoder_pre_render($form) {
  drupal_add_js(drupal_get_path('module', 'gmaps_devel') .'/misc/gmaps-devel-geocoder.js');
  return $form;
}