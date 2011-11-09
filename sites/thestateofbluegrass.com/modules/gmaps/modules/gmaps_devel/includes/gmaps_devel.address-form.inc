<?php
// $Id: gmaps_devel.address-form.inc,v 1.1.2.6 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * gmaps_address test form.
 */

function gmaps_devel_address_form(&$form_state = NULL) {
  $values = &$form_state['values'];
  $adr = $values['address'];

  $form = array();
  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $countries = gmaps_get_countries('names');
  if ($countries) {
    $allowed_countries = isset($values['allowed_countries']) ? $values['allowed_countries'] : array();
    $form['options']['allowed_countries'] = array('#type' => 'select',
      '#title' => t('Allowed countries'),
      '#default_value' => $allowed_countries,
      '#options' => $countries,
      '#multiple' => TRUE,
      '#size' => 5,
    );
  }

  $accuracy = isset($values['accuracy']) ? $values['accuracy'] : 0;
  $form['options']['accuracy'] = array('#type' => 'select',
    '#title' => t('Accuracy'),
    '#default_value' => $accuracy,
    '#options' => gmaps_address_accuracy_levels(),
  );

  $geocoder = isset($values['geocoder']) ? $values['geocoder'] : TRUE;
  $form['options']['geocoder'] = array('#type' => 'checkbox',
    '#title' => t('Enable geocoder'),
    '#default_value' => $geocoder,
  );
  $privacy = isset($values['privacy']) ? $values['privacy'] : TRUE;
  $form['options']['privacy'] = array('#type' => 'checkbox',
    '#title' => t('Enable privacy'),
    '#default_value' => $privacy,
  );
  $use_map = isset($values['use_map']) ? $values['use_map'] : TRUE;
  $form['options']['use_map'] = array('#type' => 'checkbox',
    '#title' => t('Use map'),
    '#default_value' => $use_map,
  );
  $collapsible = isset($values['collapsible']) ? $values['collapsible'] : TRUE;
  $form['options']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $collapsible,
  );
  
  $form['address'] = array('#type' => 'gmaps_address',
    '#title' => t('Address'),
    '#default_value' => $adr,
    '#enable_geocoder' => $geocoder,
    '#allowed_countries' => $allowed_countries,
    '#required' => $accuracy,
    '#enable_privacy' => $privacy,
    '#use_map' => $use_map ? TRUE : FALSE,
    '#collapsible' => $collapsible,
  );

  if (!gmaps_address_empty($adr)) {
    $form['result'] = array('#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => FALSE,
    );
    $form['result']['value'] = array('#type' => 'item',
      '#title' => t('Element value'),
      '#value' => print_r($adr, 1),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
    $form['result']['plain'] = array('#type' => 'item',
      '#title' => t('Plain, minimal'),
      '#value' => gmaps_address_format_plain($adr, 'iso2', ''),
    );
    $form['result']['themed'] = array('#type' => 'item',
      '#title' => t('Themed, full'),
      '#value' => gmaps_address_format($adr, 'iso3_name', 'both', 'before', TRUE),
    );
    $form['result']['rendered'] = array('#type' => 'item',
      '#title' => t('Site/user configured'),
      '#value' => gmaps_address_render($adr),
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_devel_address_form_submit';

  return $form;
}

function _gmaps_devel_address_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
