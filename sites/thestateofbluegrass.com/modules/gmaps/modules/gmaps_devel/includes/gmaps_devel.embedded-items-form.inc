<?php
// $Id: gmaps_devel.embedded-items-form.inc,v 1.1.2.3 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * embedded items test form.
 */

function gmaps_devel_embedded_items_form(&$form_state = NULL) {
  $values = &$form_state['values'];

  $form = array();
  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $active = isset($values['active']) ? $values['active'] : '';
  $form['options']['active'] = array('#type' => 'select',
    '#title' => t('Activate top accordion'),
    '#default_value' => $active,
    '#options' => array('' => t('Collapsed')) + range(1, 3),
  );
  $subactive = isset($values['subactive']) ? $values['subactive'] : '';
  $form['options']['subactive'] = array('#type' => 'select',
    '#title' => t('Activate sub-accordion'),
    '#default_value' => $subactive,
    '#options' => array('' => t('Collapsed')) + range(1, 3),
  );
  $subsubactive = isset($values['subsubactive']) ? $values['subsubactive'] : '';
  $form['options']['subsubactive'] = array('#type' => 'select',
    '#title' => t('Activate sub-sub-accordion'),
    '#default_value' => $subsubactive,
    '#options' => array('' => t('Collapsed')) + range(1, 3),
  );

  $selected = isset($values['selected']) ? $values['selected'] : '';
  $form['options']['selected'] = array('#type' => 'select',
    '#title' => t('Select top tab'),
    '#default_value' => $selected,
    '#options' => array('' => t('Collapsed')) + range(1, 5),
  );
  $subselected = isset($values['subselected']) ? $values['subselected'] : '';
  $form['options']['subselected'] = array('#type' => 'select',
    '#title' => t('Select sub-tab'),
    '#default_value' => $subselected,
    '#options' => array('' => t('Collapsed')) + range(1, 3),
  );
  $subsubselected = isset($values['subsubselected']) ? $values['subsubselected'] : '';
  $form['options']['subsubselected'] = array('#type' => 'select',
    '#title' => t('Select sub-sub-tab'),
    '#default_value' => $subsubselected,
    '#options' => array('' => t('Collapsed')) + range(1, 3),
  );
  
  $positions = array(
    GMAPS_TABS_NAV_TOP => t('Top'),
    GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
    GMAPS_TABS_NAV_LEFT => t('Left'),
    GMAPS_TABS_NAV_RIGHT => t('Right'),
  );
  $nav = isset($values['nav']) ? $values['nav'] : GMAPS_TABS_NAV_TOP;
  $form['options']['nav'] = array('#type' => 'select',
    '#title' => t('Top tab navigation position'),
    '#default_value' => $nav,
    '#options' => $positions,
  );
  
  
  $form['results'] = array('#type' => 'item',
    '#title' => t('Initialized items'),
    '#value' => '<div id="initialized-items"></div>',
  );
  
  //accordion
  $form['accordion'] = array('#type' => 'gmaps_accordion',
    '#id' => 'top',
    '#title' => t('Sample accordion panel'),
    '#accordion' => array('active' => $active == '' ? FALSE : $active),
    '#collapsible' => TRUE,
    '#description' => t('Simple description example.'),
    '#value' => t('Simple value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Panel 1'),
    '#description' => t('Simple panel description example.'),
    '#value' => t('Simple panel value example: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );
  
  //tabs
  $form['accordion']['panel_1']['tabs'] = array('#type' => 'gmaps_tabs',
    '#id' => 'top',
    '#title' => t('Sample tabs panel'),
    '#tabs' => array('selected' => $selected == '' ? NULL : $selected),
    '#collapsible' => TRUE,
    '#nav_position' => $nav,
    '#description' => t('Simple description.'),
    '#value' => t('Value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1']['tabs']['tab_1'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Tab 1'),
    '#description' => t('Panel description example.'),
    '#value' => t('Simple sample panel value: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );
  
  //subtabs
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs'] = array('#type' => 'gmaps_tabs',
    '#id' => 'sub',
    '#title' => t('Sample sub-tabs panel'),
    '#tabs' => array('selected' => $subselected == '' ? NULL : $subselected),
    '#collapsible' => TRUE,
    '#nav_position' => GMAPS_TABS_NAV_TOP,
    '#description' => t('Simple description.'),
    '#value' => t('Value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_1'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Subtab 1'),
    '#description' => t('Panel description example.'),
    '#value' => t('Simple sample panel value: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );

  //subsubtabs
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_1']['subsubtabs'] = array('#type' => 'gmaps_tabs',
    '#id' => 'subsub',
    '#title' => t('Sample sub-sub-tabs panel'),
    '#tabs' => array('selected' => $subsubselected == '' ? NULL : $subsubselected),
    '#collapsible' => TRUE,
    '#nav_position' => GMAPS_TABS_NAV_BOTTOM,
    '#description' => t('Simple description.'),
    '#value' => t('Value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_1']['subsubtabs']['subsubtab_1'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Sub-subtab 1'),
    '#description' => t('Panel description example.'),
    '#value' => t('Simple sample panel value: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_1']['subsubtabs']['subsubtab_2'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Sub-subtab 2'),
    '#description' => t('This is a description example, too.'),
    '#value' => t('Yet another panel value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_1']['subsubtabs']['subsubtab_3'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Sub-subtab 3 bla birt jf ewgfw'),
    '#value' => t('Yet another panel value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_2'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Subtab 2'),
    '#description' => t('This is a description example, too.'),
    '#value' => t('Yet another panel value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1']['tabs']['tab_1']['subtabs']['subtab_3'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Subtab 3 bla birt jf ewgfw'),
    '#value' => t('Yet another panel value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  
  $form['accordion']['panel_1']['tabs']['tab_2'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Tab 2'),
    '#description' => t('This is a description example, too.'),
    '#value' => t('Yet another panel value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_1']['tabs']['tab_2']['item'] = array('#type' => 'item',
    '#title' => t('Sample text'),
    '#value' => 'Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li',
  );
  $form['accordion']['panel_1']['tabs']['tab_3'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('Tab 3'),
  );
  $form['accordion']['panel_1']['tabs']['tab_3']['error'] = array('#type' => 'checkbox',
    '#title' => t('Test error handling'),
    '#default_value' => 0,
  );
  $form['accordion']['panel_1']['tabs']['tab_3']['error_message'] = array('#type' => 'textfield',
    '#title' => t('Error message'),
    '#default_value' => '',
  );
  $form['accordion']['panel_1']['tabs']['tab_4'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('AHAH tab'),
    '#ahah' => array(
      'path' => 'devel/gmaps/ahah-content',
      'selector' => '#squeeze .node .content',
      'effect' => 'fade',
      'progress' => array('type' => 'bar', 'message' => t('Please wait...')),
      'ajax' => FALSE,
  ),
  );
  $form['accordion']['panel_1']['tabs']['tab_5'] = array('#type' => 'gmaps_tabs_panel',
    '#title' => t('AJAX tab'),
    '#ahah' => array(
      'path' => 'devel/gmaps/ajax-content',
      'method' => 'prepend',
      'progress' => array('type' => 'throbber', 'message' => t('Please wait...'))
    ),
  );
  
  $form['accordion']['panel_2'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Panel 2'),
  );
  $form['accordion']['panel_2']['item'] = array('#type' => 'item',
    '#title' => t('Sample text'),
    '#value' => 'Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li',
  );

  //subaccordion
  $form['accordion']['panel_2']['accordion'] = array('#type' => 'gmaps_accordion',
    '#id' => 'sub',
    '#title' => t('Sample sub-accordion panel'),
    '#accordion' => array('active' => $subactive == '' ? FALSE : $subactive),
    '#collapsible' => TRUE,
    '#description' => t('Simple description example.'),
    '#value' => t('Simple value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_2']['accordion']['panel_1'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Sub-panel 1'),
    '#description' => t('Simple panel description example.'),
    '#value' => t('Simple panel value example: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );

  //subsubaccordion
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion'] = array('#type' => 'gmaps_accordion',
    '#id' => 'subsub',
    '#title' => t('Sample sub-sub-accordion panel'),
    '#accordion' => array('active' => $subsubactive == '' ? FALSE : $subsubactive),
    '#collapsible' => TRUE,
    '#description' => t('Simple description example.'),
    '#value' => t('Simple value example: %value.', array('%value' => 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut')),
  );
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion']['panel_1'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Sub-sub-panel 1'),
    '#description' => t('Simple panel description example.'),
    '#value' => t('Simple panel value example: %value.', array('%value' => 'Epsum factorial non deposit quid pro quo hic escorol. Olypian quarrels et gorilla congolium sic ad n')),
  );
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion']['panel_2'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Sub-sub-panel 2'),
  );
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion']['panel_2']['item'] = array('#type' => 'item',
    '#title' => t('Sample text'),
    '#value' => 'Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li',
  );
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion']['panel_3'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Sub-sub-panel 3'),
  );
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion']['panel_3']['subsuberror'] = array('#type' => 'checkbox',
    '#title' => t('Test error handling'),
    '#default_value' => 0,
  );
  $form['accordion']['panel_2']['accordion']['panel_1']['accordion']['panel_3']['subsuberror_message'] = array('#type' => 'textfield',
    '#title' => t('Error message'),
    '#default_value' => '',
  );
  
  $form['accordion']['panel_2']['accordion']['panel_2'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Sub-panel 2'),
  );
  $form['accordion']['panel_2']['accordion']['panel_2']['item'] = array('#type' => 'item',
    '#title' => t('Sample text'),
    '#value' => 'Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li',
  );
  $form['accordion']['panel_2']['accordion']['panel_3'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Sub-panel 3'),
  );
  $form['accordion']['panel_2']['accordion']['panel_3']['suberror'] = array('#type' => 'checkbox',
    '#title' => t('Test error handling'),
    '#default_value' => 0,
  );
  $form['accordion']['panel_2']['accordion']['panel_3']['suberror_message'] = array('#type' => 'textfield',
    '#title' => t('Error message'),
    '#default_value' => '',
  );
  $form['accordion']['panel_2']['accordion']['panel_3']['fieldset'] = array('#type' => 'fieldset',
    '#title' => t('Collapsed fieldset'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['panel_2']['accordion']['panel_3']['fieldset']['map'] = array('#type' => 'item',
    '#value' => '',
  );
  
  $form['accordion']['panel_3'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Panel 3'),
  );
  $form['accordion']['panel_3']['error'] = array('#type' => 'checkbox',
    '#title' => t('Test error handling'),
    '#default_value' => 0,
  );
  $form['accordion']['panel_3']['error_message'] = array('#type' => 'textfield',
    '#title' => t('Error message'),
    '#default_value' => '',
  );
  $map = new stdClass();
  $map->type = GMAPS_MAP_EARTH;
  $form['accordion']['panel_3']['address'] = array('#type' => 'gmaps_address',
    '#title' => t('Address'),
    '#default_value' => array(),
    '#enable_geocoder' => TRUE,
    '#enable_privacy' => TRUE,
    '#use_map' => $map,
  );
  $form['accordion']['panel_3']['point'] = array('#type' => 'gmaps_point',
    '#title' => t('Point'),
    '#default_value' => array(),
    '#map_type' => 'earth',
    '#enable_geocoder' => TRUE,
    '#enable_privacy' => TRUE,
    '#enable_3D' => TRUE,
    '#use_map' => TRUE,
  );
  $form['accordion']['panel_3']['map'] = array('#type' => 'item',
    '#value' => '',
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#validate'][] = '_gmaps_devel_embedded_items_form_validate';
  $form['#submit'][] = '_gmaps_devel_embedded_items_form_submit';
  $form['#pre_render'][] = '_gmaps_devel_embedded_items_form_pre_render';
  $form['#post_render'][] = '_gmaps_devel_embedded_items_form_post_render';
  
  return $form;
}

function _gmaps_devel_embedded_items_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  
  if ($values['error'] || $values['suberror'] || $values['subsuberror']) {
    form_set_error('error_message', t('Some error happened.'));
    form_set_error('suberror_message', t('Some error happened.'));
    form_set_error('subsuberror_message', t('Some error happened.'));
  }
}

function _gmaps_devel_embedded_items_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function _gmaps_devel_embedded_items_form_pre_render($form) {
  $form['accordion']['panel_3']['map']['#value'] = gmaps_map_item_render(gmaps_map_get_mid_for_default('input_sky'));
  $form['accordion']['panel_2']['accordion']['panel_3']['fieldset']['map']['#value'] = gmaps_map_item_render(gmaps_map_get_mid_for_default('input_moon'));
  return $form;
}

function _gmaps_devel_embedded_items_form_post_render($content, $form) {
  drupal_add_js(drupal_get_path('module', 'gmaps_devel') .'/misc/gmaps-devel-embedded-items.js');
  drupal_add_css(drupal_get_path('module', 'gmaps_devel') .'/misc/gmaps-devel-embedded-items.css');
  return $content;
}
