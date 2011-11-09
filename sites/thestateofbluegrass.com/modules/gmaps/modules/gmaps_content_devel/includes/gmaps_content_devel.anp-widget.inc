<?php
// $Id: gmaps_content_devel.anp-widget.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_anp test form.
 */

function gmaps_content_devel_anp_widget(&$form_state = NULL) {
  $values = &$form_state['values'];
  $anp = $values['anp'];

  $form = array();

  $form['anp'] = array('#type' => 'gmaps_content_anp',
    '#title' => t('Address and Point'),
    '#default_value' => $anp,
    '#required' => 2,
    '#enable_geocoder' => TRUE,
    '#enable_3D' => TRUE,
    '#enable_privacy' => TRUE,
    '#enable_bounds' => TRUE,
    '#enable_pov' => TRUE,
    '#use_map' => TRUE,
    '#input_disabled' => TRUE,
    '#collapsible' => TRUE,
    '#tabbed_parts' => TRUE,
  );

  if (!empty($anp)) {
    $form['result'] = array('#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => FALSE,
    );
    $form['result']['value'] = array('#type' => 'item',
      '#title' => t('Widget value'),
      '#value' => print_r($anp, 1),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_content_devel_anp_widget_submit';

  return $form;
}

function _gmaps_content_devel_anp_widget_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
