<?php
// $Id: gmaps_devel.accordion-form.inc,v 1.1.2.3 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * gmaps_accordion test form.
 */

function gmaps_devel_accordion_form(&$form_state = NULL) {
  $values = &$form_state['values'];

  $form = array();
  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $active = isset($values['active']) ? $values['active'] : '';
  $form['options']['active'] = array('#type' => 'select',
    '#title' => t('Active panel'),
    '#default_value' => $active,
    '#options' => array('' => t('Collapsed')) + range(1, 5),
  );
  
  $event = isset($values['event']) ? $values['event'] : '';
  $form['options']['event'] = array('#type' => 'select',
    '#title' => t('Event'),
    '#default_value' => $event,
    '#options' => array('' => t('Default')) + _gmaps_devel_get_jquery_event_options(),
  );
  
  $effect = isset($values['effect']) ? $values['effect'] : '';
  $form['options']['effect'] = array('#type' => 'select',
    '#title' => t('Effect'),
    '#default_value' => $effect,
    '#options' => array('' => t('Default'), '0' => t('Disabled'), 'slide' => t('Slide'), 'bounceslide' => t('Bounce slide')),
  );
  
  $onoff = isset($values['onoff']) ? $values['onoff'] : array();
  $form['options']['onoff'] = array('#type' => 'checkboxes',
    '#title' => t('On/Off options'),
    '#default_value' => $onoff,
    '#options' => array(
      'autoHeight' => t('Auto height'),
      'clearStyle' => t('Clear style'),
      'collapsible' => t('Collapsible'),
      'fillSpace' => t('Fill space'),
      'navigation' => t('Navigation'),
    ),
  );
  
  $accordion = array(
    'active' => ($active == '') ? FALSE : $active,
  );
  if ($event != '') {
    $accordion['event'] = $event;
  }
  if ($effect != '') {
    $accordion['animated'] = $effect == '0' ? FALSE : $effect;
  }
  $accordion['autoHeight'] = in_array('autoHeight', $onoff);
  $accordion['clearStyle'] = in_array('clearStyle', $onoff);
  $accordion['fillSpace'] = in_array('fillSpace', $onoff);
  $accordion['navigation'] = in_array('navigation', $onoff);
  $form['accordion'] = array('#type' => 'gmaps_accordion',
    '#title' => t('Sample accordion panel'),
    '#accordion' => $accordion,
    '#collapsible' => in_array('collapsible', $onoff),
    '#collapsed' => ($active == ''),
    '#description' => t('Simple description example.'),
    '#value' => t('Simple value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Panel 1'),
    '#id' => 'panel-1',
    '#description' => t('Simple panel description example.'),
    '#value' => t('Simple panel value example: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );
  $form['accordion']['panel_2'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Panel 2'),
    '#id' => 'panel-2',
  );
  $form['accordion']['panel_2']['item'] = array('#type' => 'item',
    '#title' => t('An item element'),
    '#value' => 'Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li',
  );
  $form['accordion']['panel_3'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Panel 3'),
    '#id' => 'panel-3',
  );
  $form['accordion']['panel_3']['error'] = array('#type' => 'checkbox',
    '#title' => t('Test error handling'),
    '#default_value' => 0,
  );
  $form['accordion']['panel_3']['error_message'] = array('#type' => 'textfield',
    '#title' => t('Error message'),
    '#default_value' => '',
  );
  $form['accordion']['ahah'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('AHAH panel'),
    '#id' => 'panel-ahah',
    '#ahah' => array(
      'path' => 'devel/gmaps/ahah-content',
      'selector' => '#squeeze .node .content',
      'effect' => 'fade',
      'progress' => array('type' => 'bar', 'message' => t('Please wait...')),
      'ajax' => FALSE,
  ),
  );
  $form['accordion']['ajax'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('AJAX panel'),
    '#id' => 'panel-ajax',
    '#value' => t('Simple panel value example: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
    '#ahah' => array(
      'path' => 'devel/gmaps/ajax-content',
      'method' => 'prepend',
    ),
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#validate'][] = '_gmaps_devel_accordion_form_validate';
  $form['#submit'][] = '_gmaps_devel_accordion_form_submit';
  //$form['#pre_render'][] = '_gmaps_devel_accordion_form_pre_render';
  
  return $form;
}

function _gmaps_devel_accordion_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  
  if ($values['error']) {
    if (empty($values['error_message'])) {
      form_set_error('error_message', t('Error message is required.'));
    }
    else {
      form_set_error('error_message', t('Your favorite error message is %error', array('%error' => $values['error_message'])));
    }
  }
  else {
    form_set_value($form['options']['onoff'], array_filter($values['onoff']), $form_state);
  }
}

function _gmaps_devel_accordion_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function _gmaps_devel_accordion_form_pre_render($form) {
  return $form;
}
