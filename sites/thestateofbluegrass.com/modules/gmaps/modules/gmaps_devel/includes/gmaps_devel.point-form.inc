<?php
// $Id: gmaps_devel.point-form.inc,v 1.1.2.6 2009/10/30 16:58:06 xmarket Exp $

/**
 * @file
 * gmaps_point test form.
 */

function gmaps_devel_point_form(&$form_state = NULL) {
  $values = &$form_state['values'];
  $point = $values['point'];
  
  $form = array();

  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $map_type = isset($values['map_type']) ? $values['map_type'] : GMAPS_MAP_EARTH;
  $form['options']['map_type'] = array('#type' => 'select',
    '#title' => t('Map type'),
    '#default_value' => $map_type,
    '#options' => gmaps_map_get_types('names'),
  );

  $required = isset($values['required']) ? $values['required'] : 0;
  $form['options']['required'] = array('#type' => 'checkbox',
    '#title' => t('Required'),
    '#default_value' => $required,
  );

  $geocoder = isset($values['geocoder']) ? $values['geocoder'] : 0;
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

  if (!empty($point)) {
    $point->bounds = array();
  }
  $form['point'] = array('#type' => 'gmaps_point',
    '#title' => t('Point'),
    '#default_value' => $point,
    '#map_type' => $map_type,
    '#enable_geocoder' => $geocoder,
    '#enable_3D' => $elevation,
    '#enable_privacy' => $privacy,
    '#enable_bounds' => $bounds,
    '#enable_pov' => $pov,
    '#use_map' => $map ? TRUE : FALSE,
    '#input_disabled' => !$input,
    '#collapsible' => $collapsible,
    '#required' => $required,
  );

  if (!gmaps_point_empty($point)) {
    $form['result'] = array('#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => FALSE,
    );
    $form['result']['value'] = array('#type' => 'item',
      '#title' => t('Element value'),
      '#value' => print_r($point, 1),
      '#prefix' => '<pre>',
      '#suffix' => '</pre>',
    );
    $form['result']['dd_query'] = array('#type' => 'item',
      '#title' => t('DD query'),
      '#value' => gmaps_coords_format_dd_query($point),
    );
    $form['result']['named_dd_query'] = array('#type' => 'item',
      '#title' => t('Named DD query'),
      '#value' => gmaps_coords_format_dd_query($point, TRUE),
    );
    $form['result']['plain_dms'] = array('#type' => 'item',
      '#title' => t('Plain DMS'),
      '#value' => gmaps_coords_format_plain($point, 'dms', 0),
    );
    $form['result']['themed_cart'] = array('#type' => 'item',
      '#title' => t('Themed CART'),
      '#value' => gmaps_coords_format($point, 'cart', 0),
    );
    $form['result']['rendered'] = array('#type' => 'item',
      '#title' => t('Site/user configured'),
      '#value' => gmaps_coords_render($point),
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_devel_point_form_submit';

  return $form;
}

function _gmaps_devel_point_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
