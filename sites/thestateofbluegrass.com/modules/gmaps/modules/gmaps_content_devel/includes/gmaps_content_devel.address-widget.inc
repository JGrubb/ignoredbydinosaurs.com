<?php
// $Id: gmaps_content_devel.address-widget.inc,v 1.1.2.3 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * gmaps_content_address test form.
 */

function gmaps_content_devel_address_widget(&$form_state = NULL) {
  $values = &$form_state['values'];
  $adr = $values['address'];

  $form = array();
  
  $form['address'] = array('#type' => 'gmaps_content_address',
    '#title' => t('Address'),
    '#default_value' => $adr,
    '#enable_geocoder' => TRUE,
    '#required' => 2,
    '#enable_privacy' => TRUE,
    '#use_map' => TRUE,
    '#collapsible' => TRUE,
  );

  if (!empty($adr)) {
    $form['result'] = array('#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => FALSE,
    );
    $form['result']['value'] = array('#type' => 'item',
      '#title' => t('Widget value'),
      '#value' => print_r($adr, 1),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_content_devel_address_widget_submit';

  return $form;
}

function _gmaps_content_devel_address_widget_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
