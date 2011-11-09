<?php
// $Id: gmaps_term.admin.inc,v 1.1.2.4 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Vocabulary and term administration functions
 */

/**
 * Executes gmaps_term_taxonomy().
 */
function _gmaps_term_taxonomy($op, $type, $array = NULL) {
  switch($type) {
    case 'vocabulary':
      $vid = $array['vid'];
      switch($op) {
        case 'insert':
        case 'update':
          if (array_key_exists('gmaps', $array)) {
          $settings = variable_get('gmaps_term_geo_data', array());
          $settings[$vid] = $array['gmaps']['geo'];
          variable_set('gmaps_term_geo_data', $settings);
          
          $settings = variable_get('gmaps_term_marker_option', array());
          $settings[$vid] = $array['gmaps']['mo'];
          variable_set('gmaps_term_marker_option', $settings);
          }
          
          break;
        
        case 'delete':
          $settings = variable_get('gmaps_term_geo_data', array());
          unset($settings[$vid]);
          if (empty($settings)) {
            variable_del('gmaps_term_geo_data');
          }
          else {
            variable_set('gmaps_term_geo_data', $settings);
          }
          
          $settings = variable_get('gmaps_term_marker_option', array());
          unset($settings[$vid]);
          if (empty($settings)) {
            variable_del('gmaps_term_marker_option');
          }
          else {
            variable_set('gmaps_term_marker_option', $settings);
          }
          
          break;
      }
      break;
    
    case 'term':
      $vid = $array['vid'];
      $tid = $array['tid'];
      switch ($op) {
        case 'insert':
        case 'update':
          if (array_key_exists('gmaps_geo', $array)) {
            $edit = $array['gmaps_geo'];
            $edit->vid = $vid;
            $edit->tid = $tid;
            _gmaps_term_geo_data_save($edit);
          }
          if (array_key_exists('gmaps_mo', $array)) {
            $edit = $array['gmaps_mo'];
            $edit['vid'] = $vid;
            $edit['tid'] = $tid;
            _gmaps_term_mo_data_save($edit);
          }
          break;
          
        case 'delete':
          _gmaps_term_geo_data_delete($tid);
          _gmaps_term_mo_data_delete($tid);
          break;
      }
      break;
  }
}

function _gmaps_term_form_taxonomy_form_vocabulary_alter(&$form, &$form_state) {
  $vid = $form['vid']['#value'];
  
  $settings = variable_get('gmaps_term_geo_data', array());
  $geo = isset($settings[$vid]) ? $settings[$vid] : array(
    'type' => '',
    'options' => array(
      'required' => 0,
      'allowed_countries' => array(),
      'map_type' => GMAPS_MAP_EARTH,
      'enable_bounds' => 1,
      'enable_3D' => 0,
      'enable_pov' => 0,
      'enable_privacy' => 0,
    ),
    'widget' => array(
      'enable_geocoder' => 1,
      'use_map' => 1,
      'collapsible' => 1,
      'collapsed' => -1,
      'input_disabled' => 1,
      'tabbed_parts' => -1,
    ),
  );
  
  $settings = variable_get('gmaps_term_marker_option', array());
  $mo = isset($settings[$vid]) ? $settings[$vid] : array(
    'icon' => array(
      'enabled' => 0,
      'required' => 0,
      'show' => GMAPS_TERM_ICON_DISPLAY_DISABLED,
    ),
    'protected_icon' => array(
      'enabled' => 0,
      'required' => 0,
    ),
    'static_icon' => array(
      'enabled' => 0,
      'required' => 0,
    ),
    'protected_static_icon' => array(
      'enabled' => 0,
      'required' => 0,
    ),
  );
  
  $delete_geo = $delete_mo = FALSE;
  $delete_mo_parts = array();
  if (isset($vid)) {
    if ($geo['type'] == 'address') {
      $delete_geo = db_result(db_query('SELECT COUNT(*) FROM {term_data} t INNER JOIN {gmaps_term_address} gta ON t.tid = gta.tid WHERE t.vid = %d', $vid));
    }
    else {
      $delete_geo = db_result(db_query('SELECT COUNT(*) FROM {term_data} t INNER JOIN {gmaps_term_point} gtp ON t.tid = gtp.tid WHERE t.vid = %d', $vid));
    }
    $result = db_query('SELECT gtmo.* FROM {term_data} t INNER JOIN {gmaps_term_marker_option} gtmo ON t.tid = gtmo.tid WHERE t.vid = %d', $vid);
    while ($gtmo = db_fetch_object($result)) {
      if ($gtmo->icon != GMAPS_ICON_MAP_DEFAULT) {
        $delete_mo_parts['icon'] = TRUE;
        $delete_mo = TRUE;
      }
      if ($gtmo->protected_icon != GMAPS_ICON_MAP_DEFAULT) {
        $delete_mo_parts['protected_icon'] = TRUE;
        $delete_mo = TRUE;
      }
      if (!$delete_mo_parts['static_icon']) {
        $s_icon = unserialize($gtmo->static_icon);
        if (!empty($s_icon)) {
          $delete_mo_parts['static_icon'] = TRUE;
          $delete_mo = TRUE;
        }
      }
      if (!$delete_mo_parts['protected_static_icon']) {
        $ps_icon = unserialize($gtmo->protected_static_icon);
        if (!empty($ps_icon)) {
          $delete_mo_parts['protected_static_icon'] = TRUE;
          $delete_mo = TRUE;
        }
      }
      if ($delete_mo_parts['icon'] && $delete_mo_parts['protected_icon'] && $delete_mo_parts['static_icon'] && $delete_mo_parts['protected_static_icon']) {
        break;
      }
    }
  }
  
  $form['gmaps'] = array('#type' => 'gmaps_accordion',
    '#title' => t('GMaps settings'),
    '#collapsible' => TRUE,
    '#collapsed' => ($vid),
    '#tree' => TRUE,
  );
  
  $form['gmaps']['geo'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Geodesical data'),
    '#id' => 'geo',
  );
  $type_options = array(
    '' => t('Disabled'),
    'gmaps_address' => t('Address'),
    'gmaps_point' => t('Point'),
    'gmaps_anp' => t('Address and point'),
  );
  if ($delete_geo) {
    $form['gmaps']['geo']['type'] = array('#type' => 'value', '#value' => $geo['type']);
    $form['gmaps']['geo']['type_display'] = array('#type' => 'item',
      '#title' => t('Data type'),
      '#value' => theme('placeholder', $type_options[$geo['type']]),
    );
  }
  else {
    $form['gmaps']['geo']['type'] = array('#type' => 'select',
      '#title' => t('Data type'),
      '#default_value' => $geo['type'],
      '#options' => $type_options,
    );
  }
  $form['gmaps']['geo']['options'] = array('#type' => 'fieldset',
    '#title' => t('Data options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps']['geo']['options']['required'] = array('#type' => 'select',
    '#title' => t('Required'),
    '#options' => gmaps_address_accuracy_levels(),
    '#default_value' => $geo['options']['required'],
  );
  $form['gmaps']['geo']['options']['allowed_countries'] = array('#type' => 'select',
    '#title' => t('Allowed countries'),
    '#options' => gmaps_get_countries('names'),
    '#multiple' => TRUE,
    '#size' => 5,
    '#default_value' => $geo['options']['allowed_countries'],
    '#description' => t('If none selected all countries will be allowed.'),
  );
  $map_types = gmaps_map_get_types('names');
  if ($delete_geo) {
    $form['gmaps']['geo']['options']['map_type'] = array('#type' => 'value', '#value' => $geo['options']['map_type']);
    $form['gmaps']['geo']['options']['map_type_display'] = array('#type' => 'item',
      '#title' => t('Map type'),
      '#value' => theme('placeholder', $map_types[$geo['options']['map_type']]),
    );
  } else {
    $form['gmaps']['geo']['options']['map_type'] = array('#type' => 'select',
      '#title' => t('Map type'),
      '#options' => $map_types,
      '#default_value' => $geo['options']['map_type'],
    );
  }
  $form['gmaps']['geo']['options']['enable_bounds'] = array('#type' => 'checkbox',
    '#title' => t('Enable bounds'),
    '#default_value' => $geo['options']['enable_bounds'],
  );
  $form['gmaps']['geo']['options']['enable_3D'] = array('#type' => 'checkbox',
    '#title' => t('Enable elevation'),
    '#default_value' => $geo['options']['enable_3D'],
  );
  $form['gmaps']['geo']['options']['enable_pov'] = array('#type' => 'checkbox',
    '#title' => t('Enable point of view'),
    '#default_value' => $geo['options']['enable_pov'],
  );
  $form['gmaps']['geo']['options']['enable_privacy'] = array('#type' => 'checkbox',
    '#title' => t('Enable privacy'),
    '#default_value' => $geo['options']['enable_privacy'],
  );
  $form['gmaps']['geo']['widget'] = array('#type' => 'fieldset',
    '#title' => t('Widget options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps']['geo']['widget']['enable_geocoder'] = array('#type' => 'checkbox',
    '#title' => t('Enable geocoder'),
    '#default_value' => $geo['widget']['enable_geocoder'],
  );
  $form['gmaps']['geo']['widget']['use_map'] = array('#type' => 'checkbox',
    '#title' => t('Use input map'),
    '#default_value' => $geo['widget']['use_map'],
  );
  $form['gmaps']['geo']['widget']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $geo['widget']['collapsible'],
  );
  $form['gmaps']['geo']['widget']['collapsed'] = array('#type' => 'select',
    '#title' => t('Collapsed'),
    '#default_value' => $geo['widget']['collapsed'],
    '#options' => array(-1 => t('When not empty'), 0 => t('Collapsible'), 1 => t('Collapsed'))
  );
  $form['gmaps']['geo']['widget']['input_disabled'] = array('#type' => 'checkbox',
    '#title' => t('Disable numeric input fields'),
    '#default_value' => $geo['widget']['input_disabled'],
  );
  $form['gmaps']['geo']['widget']['tabbed_parts'] = array('#type' => 'select',
    '#title' => t('Show parts on tabs'),
    '#default_value' => $geo['widget']['tabbed_parts'],
    '#options' => array(-1 => t('When not empty'), 0 => t('Never'), 1 => t('Always'))
  );
  
  $form['gmaps']['mo'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Marker options'),
    '#id' => 'mo',
  );
  //mo:icon
  $form['gmaps']['mo']['icon'] = array('#type' => 'fieldset',
    '#title' => t('Icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['icon']) {
    $form['gmaps']['mo']['icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['gmaps']['mo']['icon']['required'] = array('#type' => 'value', '#value' => $mo['icon']['required']);
    $form['gmaps']['mo']['icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable icons for terms'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['gmaps']['mo']['icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['gmaps']['mo']['icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable icons for terms'),
      '#default_value' => $mo['icon']['enabled'],
    );
    $form['gmaps']['mo']['icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['icon']['required'],
    );
  }
  $form['gmaps']['mo']['icon']['show'] = array('#type' => 'select',
    '#title' => t('Show icon on term links'),
    '#default_value' => $mo['icon']['show'],
    '#options' => _gmaps_term_icon_display_options(),
  );
  //mo:protected icon
  $form['gmaps']['mo']['protected_icon'] = array('#type' => 'fieldset',
    '#title' => t('Protected icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['protected_icon']) {
    $form['gmaps']['mo']['protected_icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['gmaps']['mo']['protected_icon']['required'] = array('#type' => 'value', '#value' => $mo['protected_icon']['required']);
    $form['gmaps']['mo']['protected_icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable protected icons for terms'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['gmaps']['mo']['protected_icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['protected_icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['gmaps']['mo']['protected_icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable protected icons for terms'),
      '#default_value' => $mo['protected_icon']['enabled'],
    );
    $form['gmaps']['mo']['protected_icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['protected_icon']['required'],
    );
  }
  //mo:static icon
  $form['gmaps']['mo']['static_icon'] = array('#type' => 'fieldset',
    '#title' => t('Static icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['static_icon']) {
    $form['gmaps']['mo']['static_icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['gmaps']['mo']['static_icon']['required'] = array('#type' => 'value', '#value' => $mo['static_icon']['required']);
    $form['gmaps']['mo']['static_icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable static icons for terms'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['gmaps']['mo']['static_icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['static_icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['gmaps']['mo']['static_icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable static icons for terms'),
      '#default_value' => $mo['static_icon']['enabled'],
    );
    $form['gmaps']['mo']['static_icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['static_icon']['required'],
    );
  }
  //mo: protected static icon
  $form['gmaps']['mo']['protected_static_icon'] = array('#type' => 'fieldset',
    '#title' => t('Protected static con'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['protected_static_icon']) {
    $form['gmaps']['mo']['protected_static_icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['gmaps']['mo']['protected_static_icon']['required'] = array('#type' => 'value', '#value' => $mo['protected_static_icon']['required']);
    $form['gmaps']['mo']['protected_static_icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable protected static icons for terms'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['gmaps']['mo']['protected_static_icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['protected_static_icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['gmaps']['mo']['protected_static_icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable protected static icons for terms'),
      '#default_value' => $mo['protected_static_icon']['enabled'],
    );
    $form['gmaps']['mo']['protected_static_icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['protected_static_icon']['required'],
    );
  }
  
  $form['#validate'][] = '_gmaps_term_form_taxonomy_form_vocabulary_validate';
  
  //add weight for buttons
  $form['submit']['#weight'] = 100;
  if (isset($vid)) {
    $form['delete']['#weight'] = 101;
    if ($delete_geo) {
      $form['delete_gmaps_geo'] = array('#type' => 'submit',
        '#value' => t('Delete geodesical data'),
        '#submit' => array('_gmaps_term_form_taxonomy_form_vocabulary_submit_geo'),
        '#weight' => 102,
      );
    }
    if ($delete_mo) {
      $form['delete_gmaps_mo'] = array('#type' => 'submit',
        '#value' => t('Delete marker option values'),
        '#submit' => array('_gmaps_term_form_taxonomy_form_vocabulary_submit_mo'),
        '#weight' => 103,
      );
    }
  }
}

function _gmaps_term_form_taxonomy_form_vocabulary_validate(&$form, &$form_state) {
  $value = $form_state['values']['gmaps']['geo'];
  if ($value['type'] != 'point' && $value['options']['map_type'] != GMAPS_MAP_EARTH) {
    form_set_error('gmaps][geo][options][map_type', t('Only point type data can have map type other than Earth.'));
  }
}

function _gmaps_term_form_taxonomy_form_vocabulary_submit_geo(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/taxonomy/delete-gmaps/'. $form_state['values']['vid'] .'/geo';
}

function _gmaps_term_form_taxonomy_form_vocabulary_submit_mo(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/taxonomy/delete-gmaps/'. $form_state['values']['vid'] .'/mo';
}

function gmaps_term_data_confirm_delete(&$form_state, $vocabulary, $type) {
  switch($type) {
    case 'geo':
      $form['vocabulary'] = array('#type' => 'value', '#value' => $vocabulary);
      $form['type'] = array('#type' => 'value', '#value' => $type);
      return confirm_form($form,
                      t('Are you sure you want to delete all geodesical data of terms within the vocabulary %title?',
                      array('%title' => $vocabulary->name)),
                      'admin/content/taxonomy/edit/vocabulary/'. $vocabulary->vid,
                      t('This action cannot be undone.'),
                      t('Delete'),
                      t('Cancel'));
      break;
      
    case 'mo':
      $form['vocabulary'] = array('#type' => 'value', '#value' => $vocabulary);
      $form['type'] = array('#type' => 'value', '#value' => $type);
      $form['mode'] = array('#type' => 'radios',
        '#title' => t('Mode'),
        '#default_value' => 'parts',
        '#options' => array(
          'all' => t('Delete all'),
          'parts' => t('Delete selected parts only'),
        ),
      );
      $form['parts'] = array('#type' => 'checkboxes',
        '#title' => t('Parts to delete'),
        '#default_value' => array(),
        '#options' => array(
          'icon' => t('Icon'),
          'protected_icon' => t('Protected icon'),
          'static_icon' => t('Static icon'),
          'protected_static_icon' => t('Protected static icon'),
        ),
      );
      return confirm_form($form,
                      t('Are you sure you want to delete marker option values of terms within the vocabulary %title?',
                      array('%title' => $vocabulary->name)),
                      'admin/content/taxonomy/edit/vocabulary/'. $vocabulary->vid,
                      t('This action cannot be undone.'),
                      t('Delete'),
                      t('Cancel'));
      break;
    
  }
  return array();
}

function gmaps_term_data_confirm_delete_submit(&$form, &$form_state) {
  $vocabulary = $form_state['values']['vocabulary'];
  
  if ($form_state['values']['confirm']) {
    if ($form_state['values']['type'] == 'geo') {
      $result = db_query('SELECT * FROM {term_data} t WHERE t.vid = %d', $vocabulary->vid);
      while ($term = db_fetch_object($result)) {
        _gmaps_term_geo_data_delete($term->tid);
      }
    }
    else if ($form_state['values']['type'] == 'mo') {
      if ($form_state['values']['mode'] == 'parts') {
        $parts = array_keys(array_filter($form_state['values']['parts']));
      }
      if (!isset($parts) || !empty($parts)) {
        $result = db_query('SELECT * FROM {term_data} t WHERE t.vid = %d', $vocabulary->vid);
        while ($term = db_fetch_object($result)) {
          _gmaps_term_mo_data_delete($term->tid, $parts);
        }
      }
    }
  }
  
  $form_state['redirect'] = 'admin/content/taxonomy/edit/vocabulary/'. $vocabulary->vid;
}

function _gmaps_term_form_taxonomy_form_term_alter(&$form, &$form_state) {
  if (isset($form_state['confirm_delete']) || isset($form_state['confirm_parents'])) {
    return;
  }
  
  $vid = $form['#vocabulary']['vid'];
  $tid = $form['#term']['tid'];
  
  $settings = variable_get('gmaps_term_geo_data', array());
  $geo = isset($settings[$vid]) ? $settings[$vid] : array('type' => '');
  
  $settings = variable_get('gmaps_term_marker_option', array());
  $mo = isset($settings[$vid]) ? $settings[$vid] : array(
    'icon' => array('enabled' => 0),
    'protected_icon' => array('enabled' => 0),
    'static_icon' => array('enabled' => 0),
    'protected_static_icon' => array('enabled' => 0),
  );
  
  if (!empty($tid)) {
    $geo_data = gmaps_term_geo_data_load($tid, $vid, FALSE);
    $mo_data = gmaps_term_mo_data_load($tid, FALSE);
  }
  if (empty($mo_data)) {
    $mo_data = _gmaps_term_mo_get_defaults($mo);
  }
  
  if (!empty($geo['type'])) {
    $element = array(
      '#type' => $geo['type'],
      '#title' => t('GMaps geodesical data'),
      '#required' => $geo['options']['required'],
    );
    switch($geo['type']) {
      case 'gmaps_address':
        $element['#default_value'] = $geo_data->address;
        break;
        
      case 'gmaps_point':
        $element['#default_value'] = $geo_data->point;
        break;
        
      case 'gmaps_anp':
        $element['#default_value'] = $geo_data;
        break;
    }
    if ($geo['type'] == 'gmaps_address' || $geo['type'] == 'gmaps_anp') {
      $element['#allowed_countries'] = $geo['options']['allowed_countries'];
    }
    if ($geo['type'] == 'gmaps_point' || $geo['type'] == 'gmaps_anp') {
      $element['#map_type'] = $geo['options']['map_type'] ? $geo['options']['map_type'] : GMAPS_MAP_EARTH;
      $element['#enable_bounds'] = isset($geo['options']['enable_bounds']) ? $geo['options']['enable_bounds'] : TRUE;
      $element['#enable_3D'] = $geo['options']['enable_3D'];
      $element['#enable_pov'] = $geo['options']['enable_pov'];
      
      $element['#input_disabled'] = isset($geo['widget']['input_disabled']) ? $geo['widget']['input_disabled'] : TRUE;
    }
    if ($geo['type'] == 'gmaps_anp' && $geo['widget']['tabbed_parts'] > -1) {
      $element['#tabbed_parts'] = $geo['widget']['tabbed_parts'];
    }
    $element['#enable_privacy'] = $geo['options']['enable_privacy'];

    $element['#enable_geocoder'] = $geo['widget']['enable_geocoder'];
    $element['#use_map'] = $geo['widget']['use_map'] ? TRUE : FALSE;
    $element['#collapsible'] = $geo['widget']['collapsible'];
    if ($geo['widget']['collapsed'] > -1) {
      $element['#collapsed'] = $geo['widget']['collapsed'];
    }
    $form['gmaps_geo'] = $element;
  }
  
  if (!empty($mo['icon']['enabled']) || !empty($mo['protected_icon']['enabled']) || !empty($mo['static_icon']['enabled']) || !empty($mo['protected_static_icon']['enabled'])) {
    $form['gmaps_mo'] = array('#type' => 'fieldset',
      '#title' => t('GMaps marker options'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );
    
    $icons = gmaps_get_icons();
    if (!empty($mo['icon']['enabled'])) {
      $inherit = $mo['icon']['required'] ? array() : array(GMAPS_ICON_MAP_DEFAULT => GMAPS_INHERIT_LABEL);
      $form['gmaps_mo']['icon'] = array('#type' => 'gmaps_icon_select',
        '#title' => t('Icon'),
        '#default_value' => $mo_data->icon,
        '#icons' => $inherit + array(GMAPS_ICON_API_DEFAULT => t('API default')) + $icons,
      );
    }
    if (!empty($mo['protected_icon']['enabled'])) {
      $inherit = $mo['protected_icon']['required'] ? array() : array(GMAPS_ICON_MAP_DEFAULT => GMAPS_INHERIT_LABEL);
      $form['gmaps_mo']['protected_icon'] = array('#type' => 'gmaps_icon_select',
        '#title' => t('Protected icon'),
        '#default_value' => $mo_data->protected_icon,
        '#icons' => $inherit + array(GMAPS_ICON_API_DEFAULT => t('API default')) + $icons,
      );
    }
    if (!empty($mo['static_icon']['enabled'])) {
      $form['gmaps_mo']['static_icon'] = array('#type' => 'fieldset',
        '#title' => t('Static icon'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['gmaps_mo']['static_icon']['style'] = array('#type' => 'gmaps_static_marker_style',
        '#title' => t('Style'),
        '#default_value' => $mo_data->static_icon['style'],
        '#required' => $mo['static_icon']['required'],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['gmaps_mo']['static_icon']['style_sticky'] = array('#type' => 'gmaps_static_marker_style',
        '#title' => t('Sticky style'),
        '#default_value' => $mo_data->static_icon['style_sticky'],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    }
    if (!empty($mo['protected_static_icon']['enabled'])) {
      $form['gmaps_mo']['protected_static_icon'] = array('#type' => 'fieldset',
        '#title' => t('Protected static icon'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['gmaps_mo']['protected_static_icon']['style'] = array('#type' => 'gmaps_static_marker_protected',
        '#title' => t('Style'),
        '#default_value' => $mo_data->protected_static_icon['style'],
        '#required' => $mo['protected_static_icon']['required'],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['gmaps_mo']['protected_static_icon']['style_sticky'] = array('#type' => 'gmaps_static_marker_protected',
        '#title' => t('Sticky style'),
        '#default_value' => $mo_data->protected_static_icon['style_sticky'],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    }
  }
  
  //add weight for buttons
  $form['submit']['#weight'] = 100;
  if (isset($vid)) {
    $form['delete']['#weight'] = 101;
  }
}

function _gmaps_term_icon_display_options() {
  $options = array(
    GMAPS_TERM_ICON_DISPLAY_DISABLED => t('Disabled'),
    GMAPS_TERM_ICON_DISPLAY_BEFORE => t('Before'),
    GMAPS_TERM_ICON_DISPLAY_AFTER => t('After'),
    GMAPS_TERM_ICON_DISPLAY_REPLACE => t('Replace'),
  );
  return $options;
}

function _gmaps_term_mo_get_defaults($options) {
  return (object)array(
    'icon' => $options['icon']['required'] ? 0 : -1,
    'protected_icon' => $options['protected_icon']['required'] ? 0 : -1,
    'static_icon' => $options['static_icon']['required'] ? array(
      'style' => array(
        'size' => 'small',
        'color' => 'red',
      ),
      'style_sticky' => array(
        'size' => 'normal',
      ),
    ) : array(),
    'protected_static_icon' => $options['protected_static_icon']['required'] ? array(
      'style' => array(
        'quality' => 16,
        'style' => array(
          'color' => 'ff0000',
          'opacity' => '7f',
          'weight' => 5
        ),
      ),
      'style_sticky' => array(
        'style' => array(
          'opacity' => 'ff',
          'weight' => 10
        ),
      ),
    ) : array(),
  );
}

function _gmaps_term_geo_data_save(&$edit) {
  $vid = $edit->vid;
  
  $settings = variable_get('gmaps_term_geo_data', array());
  $geo = isset($settings[$vid]) ? $settings[$vid] : array('type' => '');
  
  if (empty($geo['type'])) {
    return;
  }
  
  switch($geo['type']) {
    case 'gmaps_address':
      $status = _gmaps_term_geo_address_save($edit);
      break;
      
    case 'gmaps_point':
      $status = _gmaps_term_geo_point_save($edit);
      break;
      
    case 'gmaps_anp':
      $status = _gmaps_term_geo_anp_save($edit);
      break;
  }
  
  return $status;
}

function _gmaps_term_geo_address_save(&$adr) {
  $result = SAVED_NEW;
  if (_gmaps_term_geo_data_delete($adr->tid) == SAVED_DELETED) {
    $result = SAVED_UPDATED;
  }
  
  if (gmaps_address_empty($adr)) {
    return;
  }
  
  gmaps_address_prepare_record($adr);
  
  drupal_write_record('gmaps_term_address', $adr);
  
  return $result;
}

function _gmaps_term_geo_point_save(&$point) {
  $result = SAVED_NEW;
  if (_gmaps_term_geo_data_delete($point->tid) == SAVED_DELETED) {
    $result = SAVED_UPDATED;
  }
  
  if (gmaps_point_empty($point)) {
    return;
  }
  
  gmaps_coords_add_cart($point);
  if (property_exists($point, 'bounds') && gmaps_bounds_empty($point->bounds)) {
    $point->bounds = array();
  }
  if (property_exists($point, 'pov') && gmaps_pov_empty($point->pov)) {
    $point->pov = array();
  }
  
  drupal_write_record('gmaps_term_point', $point);
    
  return $result;
}

function _gmaps_term_geo_anp_save(&$anp) {
  $result = SAVED_NEW;
  if (_gmaps_term_geo_data_delete($anp->tid) == SAVED_DELETED) {
    $result = SAVED_UPDATED;
  }
  
  if (gmaps_anp_empty($anp)) {
    return;
  }
  
  $adr = &$anp->address;
  $adr->tid = $anp->tid;
  $adr->search = $anp->search;
  
  gmaps_address_prepare_record($adr);
  drupal_write_record('gmaps_term_address', $adr);
  
  $point = &$anp->point;
  $point->tid = $anp->tid;
  $point->search = $anp->search;
  
  gmaps_coords_add_cart($point);
  if (property_exists($point, 'bounds') && gmaps_bounds_empty($point->bounds)) {
    $point->bounds = array();
  }
  if (property_exists($point, 'pov') && gmaps_pov_empty($point->pov)) {
    $point->pov = array();
  }
  
  drupal_write_record('gmaps_term_point', $point);
    
  return $result;
}

function _gmaps_term_geo_data_delete($tid) {
  db_query("DELETE FROM {gmaps_term_address} WHERE tid = %d", $tid);
  if (db_affected_rows()) {
    $result = SAVED_DELETED;
  }
  db_query("DELETE FROM {gmaps_term_point} WHERE tid = %d", $tid);
  if (db_affected_rows()) {
    $result = SAVED_DELETED;
  }
  
  return $result;
}

function _gmaps_term_mo_data_save(&$edit) {
  $vid = $edit['vid'];
  $tid = $edit['tid'];
  
  $settings = variable_get('gmaps_term_marker_option', array());
  $mo = isset($settings[$vid]) ? $settings[$vid] : array(
    'icon' => array(
      'enabled' => 0,
    ),
    'protected_icon' => array(
      'enabled' => 0,
    ),
    'static_icon' => array(
      'enabled' => 0,
    ),
    'protected_static_icon' => array(
      'enabled' => 0,
    ),
  );
  $icon = $mo['icon']['enabled'];
  $p_icon = $mo['protected_icon']['enabled'];
  $s_icon = $mo['static_icon']['enabled'];
  $ps_icon = $mo['protected_static_icon']['enabled'];
  
  $result = SAVED_NEW;
  if (_gmaps_term_mo_data_delete($tid) == SAVED_DELETED) {
    $result = SAVED_UPDATED;
  }
  
  if (!$icon && !$p_icon && !$s_icon && !$ps_icon) {
    return;
  }
  
  if (!$icon) {
    $edit['icon'] = GMAPS_ICON_MAP_DEFAULT;
  }
  if (!$p_icon) {
    $edit['protected_icon'] = GMAPS_ICON_MAP_DEFAULT;
  }
  if (!$s_icon || (empty($edit['static_icon']['style']) && empty($edit['static_icon']['style_sticky']))) {
    $edit['static_icon'] = array();
  }
  if (!$ps_icon || (empty($edit['protected_static_icon']['style']) && empty($edit['protected_static_icon']['style_sticky']))) {
    $edit['protected_static_icon'] = array();
  }
  
  drupal_write_record('gmaps_term_marker_option', $edit);
    
  return $result;
}

function _gmaps_term_mo_data_delete($tid, $parts = array()) {
  if (empty($parts)) {
    db_query("DELETE FROM {gmaps_term_marker_option} WHERE tid = %d", $tid);
    if (db_affected_rows()) {
      $result = SAVED_DELETED;
    }
  }
  else {
    if ($mo_data = gmaps_term_mo_data_load($tid, FALSE)) {
      foreach ($parts as $part) {
       switch($part) {
         case 'icon':
           $mo_data->icon = GMAPS_ICON_MAP_DEFAULT;
           break;
           
         case 'protected_icon':
           $mo_data->protected_icon = GMAPS_ICON_MAP_DEFAULT;
           break;
           
         case 'static_icon':
           $mo_data->static_icon = array();
           break;
           
         case 'protected_static_icon':
           $mo_data->protected_static_icon = array();
           break;
           
       }
      }
      
      drupal_write_record('gmaps_term_marker_option', $mo_data, array('tid'));
      $result = SAVED_DELETED;
    }
  }
  
  return $result;
}

function _gmaps_term_form_devel_generate_taxonomy_form_submit($form, &$form_state) {
  if ($form_state['values']['kill_taxonomy']) {
    variable_del('gmaps_term_geo_data');
    variable_del('gmaps_term_marker_option');
    
    db_query('DELETE FROM {gmaps_term_address}');
    db_query('DELETE FROM {gmaps_term_point}');
    db_query('DELETE FROM {gmaps_term_marker_option}');
  }
  
  if ($form_state['values']['kill_taxonomy'] || $form_state['values']['num_vocab']) {
    _gmaps_term_generate_vocabulary_options();
  }
  
  $geo = variable_get('gmaps_term_geo_data', array());
  $mo = variable_get('gmaps_term_marker_option', array());
  
  $result = db_query("SELECT vid FROM {vocabulary}");
  while($voc = db_fetch_object($result)){
    $geo_voc = isset($geo[$voc->vid]) ? $geo[$voc->vid] : array();
    $mo_voc = isset($mo[$voc->vid]) ? $mo[$voc->vid] : array();
    
    if ($geo_voc['type'] || $mo_voc['icon']['enabled'] || $mo_voc['protected_icon']['enabled'] || $mo_voc['static_icon']['enabled'] || $mo_voc['protected_static_icon']['enabled']) {
      _gmaps_term_generate_term_data($voc->vid, $geo_voc, $mo_voc);
    }
  }
}

function _gmaps_term_generate_vocabulary_options() {
  $geo = variable_get('gmaps_term_geo_data', array());
  $mo = variable_get('gmaps_term_marker_option', array());
  
  $geo_types = array('gmaps_address', 'gmaps_point', 'gmaps_anp');
  $geo_types_count = count($geo_types) - 1;
  
  $mo_show = array(GMAPS_TERM_ICON_DISPLAY_DISABLED, GMAPS_TERM_ICON_DISPLAY_BEFORE, GMAPS_TERM_ICON_DISPLAY_AFTER, GMAPS_TERM_ICON_DISPLAY_REPLACE);
  $mo_show_count = count($mo_show) - 1;
  
  $result = db_query("SELECT vid FROM {vocabulary}");
  while($voc = db_fetch_object($result)){
    if (!isset($geo[$voc->vid]) && mt_rand(0,2)) {
      $geo[$voc->vid] = array(
        'type' => $geo_types[mt_rand(0, $geo_types_count)],
        'options' => array(
          'required' => mt_rand(0, 1),
          'allowed_countries' => array(),
          'map_type' => GMAPS_MAP_EARTH,
          'enable_bounds' => mt_rand(0, 1),
          'enable_3D' => mt_rand(0, 1),
          'enable_pov' => mt_rand(0, 1),
          'enable_privacy' => mt_rand(0, 1),
        ),
        'widget' => array(
          'enable_geocoder' => mt_rand(0, 1),
          'use_map' => mt_rand(0, 1),
          'collapsible' => mt_rand(0, 1),
          'collapsed' => mt_rand(-1, 1),
          'input_disabled' => mt_rand(0, 1),
          'tabbed_parts' => mt_rand(-1, 1),
        ),
      );
    }
    
    if (!isset($mo[$voc->vid]) && mt_rand(0,2)) {
      $mo[$voc->vid] = array(
        'icon' => array(
          'enabled' => 1,
          'required' => mt_rand(0, 1),
          'show' => $mo_show[mt_rand(0, $mo_show_count)],
        ),
        'protected_icon' => array(
          'enabled' => mt_rand(0, 1),
          'required' => mt_rand(0, 1),
        ),
        'static_icon' => array(
          'enabled' => mt_rand(0, 1),
          'required' => mt_rand(0, 1),
        ),
        'protected_static_icon' => array(
          'enabled' => mt_rand(0, 1),
          'required' => mt_rand(0, 1),
        ),
      );
    }
  }
  
  variable_set('gmaps_term_geo_data', $geo);
  variable_set('gmaps_term_marker_option', $mo);
}

function _gmaps_term_generate_term_data($vid, $geo, $mo) {
  static $icons;
  
  $result = db_query('SELECT tid FROM {term_data} WHERE vid = %d', $vid);
  while($term = db_fetch_object($result)) {
    if ($geo['type']) {
      $o = $geo['options'];
      switch($geo['type']) {
        case 'gmaps_address':
          $edit = gmaps_generate_address($o['enable_privacy'], $o['allowed_countries']);
          break;
          
        case 'gmaps_point':
          $edit = gmaps_generate_point($o['map_type'], $o['enable_bounds'], $o['enable_pov'], $o['enable_3D'], $o['enable_privacy']);
          break;
          
        case 'gmaps_anp':
          $edit = gmaps_generate_anp($o['enable_bounds'], $o['enable_pov'], $o['enable_3D'], $o['enable_privacy'], $o['allowed_countries']);
          break;
          
      }
      
      if ($edit) {
        $edit->tid = $term->tid;
        $edit->vid = $vid;
        _gmaps_term_geo_data_save($edit);
      }
    }
    
    if ($mo['icon']['enabled'] || $mo['protected_icon']['enabled'] || $mo['static_icon']['enabled'] || $mo['protected_static_icon']['enabled']) {
      if (is_null($icons)) {
        $icons = array_keys(array(GMAPS_ICON_MAP_DEFAULT => 1, GMAPS_ICON_API_DEFAULT => 1) + gmaps_get_icons());
        $icons_count = count($icons) - 1;
      }
      
      $edit = array(
        'tid' => $term->tid,
        'vid' => $vid,
        'icon' => $icons[mt_rand(0, $icons_count)],
        'protected_icon' => $icons[mt_rand(0, $icons_count)],
        'static_icon' => mt_rand(0, 3) ? array(
          'style' => array(
            'size' => 'small',
            'color' => 'red',
          ),
          'style_sticky' => array(
            'size' => 'normal',
          )) : array(),
        'protected_static_icon' => mt_rand(0, 4) ? array(
          'style' => array(
            'size' => 'small',
            'color' => 'red',
          ),
          'style_sticky' => array(
            'size' => 'normal',
          )) : array(),
      );
      
      _gmaps_term_mo_data_save($edit);
    }
  }
}
