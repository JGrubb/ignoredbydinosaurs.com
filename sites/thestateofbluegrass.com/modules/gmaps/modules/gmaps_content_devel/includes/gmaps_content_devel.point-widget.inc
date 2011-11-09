<?php
// $Id: gmaps_content_devel.point-widget.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_content_point test form.
 */

function gmaps_content_devel_point_widget(&$form_state = NULL) {
  $values = &$form_state['values'];
  $point = $values['point'];
  
  $form = array();

  $form['point'] = array('#type' => 'gmaps_content_point',
    '#title' => t('Point'),
    '#default_value' => $point,
    '#map_type' => GMAPS_MAP_EARTH,
    '#enable_geocoder' => TRUE,
    '#enable_3D' => TRUE,
    '#enable_privacy' => TRUE,
    '#enable_bounds' => TRUE,
    '#enable_pov' => TRUE,
    '#use_map' => TRUE,
    '#input_disabled' => TRUE,
    '#collapsible' => TRUE,
  );

  if (!empty($point)) {
    $form['result'] = array('#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => FALSE,
    );
    $form['result']['value'] = array('#type' => 'item',
      '#title' => t('Widget value'),
      '#value' => print_r($point, 1),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_content_devel_point_widget_submit';

  return $form;
}

function _gmaps_content_devel_point_widget_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
