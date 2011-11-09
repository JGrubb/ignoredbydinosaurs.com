<?php
// $Id: gmaps_devel.tabs-form.inc,v 1.1.2.3 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * gmaps_tabs test form.
 */

function gmaps_devel_tabs_form(&$form_state = NULL) {
  $values = &$form_state['values'];

  $form = array();
  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $nav = isset($values['nav_position']) ? $values['nav_position'] : GMAPS_TABS_NAV_TOP;
  $form['options']['nav_position'] = array('#type' => 'select',
    '#title' => t('Navigation position'),
    '#default_value' => $nav,
    '#options' => array(
      GMAPS_TABS_NAV_TOP => t('Top'),
      GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
      GMAPS_TABS_NAV_LEFT => t('Left'),
      GMAPS_TABS_NAV_RIGHT => t('Right'),
    ),
  );
  
  $collapsible = isset($values['collapsible']) ? $values['collapsible'] : FALSE;
  $form['options']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $collapsible,
  );
  
  $selected = isset($values['selected']) ? $values['selected'] : '';
  $form['options']['selected'] = array('#type' => 'select',
    '#title' => t('Selected tab'),
    '#default_value' => $selected,
    '#options' => array('' => t('Collapsed')) + range(1,5),
  );
  
  $event = isset($values['event']) ? $values['event'] : '';
  $form['options']['event'] = array('#type' => 'select',
    '#title' => t('Event'),
    '#default_value' => $event,
    '#options' => array('' => t('Default')) + _gmaps_devel_get_jquery_event_options(),
  );
  
  $disabled = isset($values['disabled']) ? $values['disabled'] : array();
  $form['options']['disabled'] = array('#type' => 'select',
    '#title' => t('Disabled tabs'),
    '#default_value' => $disabled,
    '#options' => range(1,5),
    '#multiple' => TRUE,
  );
  
  $tabs = array(
    'selected' => ($selected == '') ? NULL : $selected,
  );
  if ($event != '') {
    $tabs['event'] = $event;
  }
  if (!empty($disabled)) {
    $tabs['disabled'] = $disabled;
  }
  $form['tabs'] = array('#type' => 'gmaps_tabs',
    '#title' => t('Sample tabs panel'),
    '#tabs' => $tabs,
    '#nav_position' => $nav,
    '#collapsible' => $collapsible,
    '#collapsed' => ($selected == ''),
    '#description' => t('Simple description.'),
    '#value' => t('Value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['tabs']['tab_1'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Tab 1 example'),
    '#id' => 'panel-1',
    '#description' => t('Panel description example.'),
    '#value' => t('Simple sample panel value: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );
  $form['tabs']['tab_2'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Tab 2 example'),
    '#id' => 'panel-2',
    '#description' => t('This is a description example, too.'),
    '#value' => t('Yet another panel value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['tabs']['tab_2']['item'] = array('#type' => 'item',
    '#title' => t('Sample text'),
    '#value' => 'Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li',
  );
  $form['tabs']['tab_3'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Tab 3 example'),
    '#id' => 'panel-3',
  );
  $form['tabs']['tab_3']['error'] = array('#type' => 'checkbox',
    '#title' => t('Test error handling'),
    '#default_value' => 0,
  );
  $form['tabs']['tab_3']['error_message'] = array('#type' => 'textfield',
    '#title' => t('Error message'),
    '#default_value' => '',
  );
  $form['tabs']['ahah'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('AHAH tab'),
    '#id' => 'panel-ahah',
    '#ahah' => array(
      'path' => 'devel/gmaps/ahah-content',
      'selector' => '#squeeze .node .content',
      'effect' => 'fade',
      'progress' => array('type' => 'bar', 'message' => t('Please wait...')),
      'ajax' => FALSE,
  ),
  );
  $form['tabs']['ajax'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('AJAX tab'),
    '#id' => 'panel-ajax',
    '#value' => t('Simple panel value example: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
    '#ahah' => array(
      'path' => 'devel/gmaps/ajax-content',
      'method' => 'prepend',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait...'))
    ),
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#validate'][] = '_gmaps_devel_tabs_form_validate';
  $form['#submit'][] = '_gmaps_devel_tabs_form_submit';
  
  return $form;
}

function _gmaps_devel_tabs_form_validate($form, &$form_state) {
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
    form_set_value($form['options']['disabled'], array_filter($values['disabled']), $form_state);
  }
}

function _gmaps_devel_tabs_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
