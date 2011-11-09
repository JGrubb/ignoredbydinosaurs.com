<?php
// $Id: gmaps_devel.anp-form.inc,v 1.1.2.6 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * gmaps_anp test form.
 */

function gmaps_devel_anp_form(&$form_state = NULL) {
  $values = &$form_state['values'];
  $anp = $values['anp'];

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

  $geocoder = isset($values['geocoder']) ? $values['geocoder'] : 1;
  $form['options']['geocoder'] = array('#type' => 'checkbox',
    '#title' => t('Enable geocoder'),
    '#default_value' => $geocoder,
  );

  $elevation = isset($values['elevation']) ? $values['elevation'] : 0;
  $form['options']['elevation'] = array('#type' => 'checkbox',
    '#title' => t('Enable 3D'),
    '#default_value' => $elevation,
  );

  $privacy = isset($values['privacy']) ? $values['privacy'] : 0;
  $form['options']['privacy'] = array('#type' => 'checkbox',
    '#title' => t('Enable privacy'),
    '#default_value' => $privacy,
  );

  $bounds = isset($values['bounds']) ? $values['bounds'] : 0;
  $form['options']['bounds'] = array('#type' => 'checkbox',
    '#title' => t('Enable bounds'),
    '#default_value' => $bounds,
  );

  $pov = isset($values['pov']) ? $values['pov'] : 0;
  $form['options']['pov'] = array('#type' => 'checkbox',
    '#title' => t('Enable point of view'),
    '#default_value' => $pov,
  );

  $map = isset($values['map']) ? $values['map'] : 0;
  $form['options']['map'] = array('#type' => 'checkbox',
    '#title' => t('Use map'),
    '#default_value' => $map,
  );

  $input = isset($values['input']) ? $values['input'] : 0;
  $form['options']['input'] = array('#type' => 'checkbox',
    '#title' => t('Enable input'),
    '#default_value' => $input,
  );

  $collapsible = isset($values['collapsible']) ? $values['collapsible'] : 1;
  $form['options']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $collapsible,
  );

  $tabbed = isset($values['tabbed_parts']) ? $values['tabbed_parts'] : 1;
  $form['options']['tabbed_parts'] = array('#type' => 'checkbox',
    '#title' => t('Show parts on tabs'),
    '#default_value' => $tabbed,
  );

  $form['anp'] = array('#type' => 'gmaps_anp',
    '#title' => t('Address and Point'),
    '#default_value' => $anp,
    '#allowed_countries' => $allowed_countries,
    '#required' => $accuracy,
    '#enable_geocoder' => $geocoder,
    '#enable_3D' => $elevation,
    '#enable_privacy' => $privacy,
    '#enable_bounds' => $bounds,
    '#enable_pov' => $pov,
    '#use_map' => $map ? TRUE : FALSE,
    '#input_disabled' => !$input,
    '#collapsible' => $collapsible,
    '#tabbed_parts' => $tabbed,
  );

  if (!gmaps_anp_empty($anp)) {
    $form['result'] = array('#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => FALSE,
    );
    $form['result']['value'] = array('#type' => 'item',
      '#title' => t('Element value'),
      '#value' => print_r($anp, 1),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
    $form['result']['find_google'] = array('#type' => 'item',
      '#value' => l(t('Find on Google Maps'), gmaps_map_link_google_find($anp->address, NULL, TRUE)),
    );
    $form['result']['find_mapquest'] = array('#type' => 'item',
      '#value' => l(t('Find on MapQuest'), gmaps_map_link_mapquest_find($anp->address, NULL, TRUE)),
    );
    $form['result']['find_yahoo'] = array('#type' => 'item',
      '#value' => l(t('Find on Yahoo! Maps'), gmaps_map_link_yahoo_find($anp->address, NULL, TRUE)),
    );
    $form['result']['google_directions'] = array('#type' => 'item',
      '#value' => theme('gmaps_google_directions', $anp->point, gmaps_address_format_plain($anp->address, 'name', '')),
    );
    $form['result']['mapquest_directions'] = array('#type' => 'item',
      '#value' => theme('gmaps_mapquest_directions', $anp->address),
    );
    $form['result']['yahoo_directions'] = array('#type' => 'item',
      '#value' => theme('gmaps_yahoo_directions', $anp->address, TRUE),
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_devel_anp_form_submit';

  return $form;
}

function _gmaps_devel_anp_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
