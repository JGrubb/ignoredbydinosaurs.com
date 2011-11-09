<?php
// $Id: gmaps_user.admin.inc,v 1.1.2.3 2010/03/18 10:52:17 xmarket Exp $

/**
 * @file
 * User settings and user administration functions
 */

/**
 * Executor of hook_user() implementation.
 */
function _gmaps_user_user($op, &$edit, &$account, $category = NULL) {
  switch ($op) {
    case 'categories':
      return array(
        array(
          'name' => 'gmaps',
          'title' => t('GMaps data'),
          'weight' => 2,
        ),
      );
      break;
    
    case 'register':
      $geo = variable_get('gmaps_user_geo_data', array());
      $mo = variable_get('gmaps_user_marker_option', array());
      if ($geo['register'] || $mo['register']) {
        return _gmaps_user_edit_form($edit, $account, TRUE);
      }
      break;
      
    case 'form':
      if ($category == 'gmaps') {
        return _gmaps_user_edit_form($edit, $account);
      }
      break;
    
    case 'insert':
    case 'update':
      if (array_key_exists('gmaps_geo', $edit)) {
        $edit['gmaps_geo']->uid = $account->uid;
        _gmaps_user_geo_data_save($edit['gmaps_geo']);
        $edit['gmaps_geo'] = NULL;
      }
      if (array_key_exists('gmaps_mo', $edit)) {
        $edit['gmaps_mo']['uid'] = $account->uid;
        _gmaps_user_mo_data_save($edit['gmaps_mo']);
        $edit['gmaps_mo'] = NULL;
      }
      break;
      
    case 'delete':
      _gmaps_user_geo_data_delete($account->uid);
      _gmaps_user_mo_data_delete($account->uid);
      break;
      
    case 'view':
      break;
  }
}

function _gmaps_user_form_user_admin_settings_alter(&$form, &$form_state) {
  $geo = variable_get('gmaps_user_geo_data', array(
    'type' => '',
    'register' => 0,
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
    'formatter' => array(
      'formatter' => 'hidden',
      'show' => 'anp',
      'address' => array(
        'user_settings' => TRUE,
        'country' => 'name',
        'state' => '',
        'flag' => '',
        'profile_links' => 0,
      ),
      'point' => array(
        'user_settings' => TRUE,
        'format' => 'dd',
        'elevation' => 1,
        'dd_prec' => 6,
        'dms_prec' => 0,
        'cart_prec' => 2,
        'elevation_prec' => 0,
      ),
      'map' => array(
        'map' => gmaps_map_get_mid_for_default('user_display_'. GMAPS_MAP_EARTH),
        'static_map' => gmaps_static_map_get_mid_for_default('user_display'),
        'cache' => 1,
        'cache_expires' => 0,
        'content' => 'default',
      ),
      'map_links' => array(
        'google_find' => 0,
        'google_directions_from' => 0,
        'google_directions_to' => 0,
        'yahoo_find' => 0,
        'yahoo_directions_from' => 0,
        'yahoo_directions_to' => 0,
        'mapquest_find' => 0,
        'mapquest_directions_from' => 0,
        'mapquest_directions_to' => 0,
      ),
    ),
    'meta' => array(
      'set_refloc' => 0,
      'meta' => array(
        'geo_placename' => 0,
        'geo_region' => 0,
        'geo_position' => 0,
        'icbm' => 0,
      ),
    ),
  ));
  
  $mo = variable_get('gmaps_user_marker_option', array(
    'register' => 0,
    'icon' => array(
      'enabled' => 0,
      'required' => 0,
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
  ));
  
  $delete_geo = $delete_mo = FALSE;
  $delete_mo_parts = array();
  if ($geo['type'] == 'address') {
    $delete_geo = db_result(db_query('SELECT COUNT(*) FROM {gmaps_user_address} gua WHERE gua.uid > 0'));
  }
  else {
    $delete_geo = db_result(db_query('SELECT COUNT(*) FROM {gmaps_user_point} gup WHERE gup.uid > 0'));
  }
  $result = db_query('SELECT gumo.* FROM {gmaps_user_marker_option} gumo WHERE gumo.uid > 0');
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
  
  $form['gmaps'] = array('#type' => 'gmaps_accordion',
    '#title' => t('GMaps settings'),
    '#collapsible' => TRUE,
    '#collapsed' => ($geo['type']),
  );
  
  $form['gmaps']['gmaps_user_geo_data'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Geodesical data'),
    '#id' => 'geo',
    '#tree' => TRUE,
  );
  $type_options = array(
    '' => t('Disabled'),
    'gmaps_address' => t('Address'),
    'gmaps_point' => t('Point'),
    'gmaps_anp' => t('Address and point'),
  );
  if ($delete_geo) {
    $form['gmaps']['gmaps_user_geo_data']['type'] = array('#type' => 'value', '#value' => $geo['type']);
    $form['gmaps']['gmaps_user_geo_data']['type_display'] = array('#type' => 'item',
      '#title' => t('Data type'),
      '#value' => theme('placeholder', $type_options[$geo['type']]),
    );
  }
  else {
    $form['gmaps']['gmaps_user_geo_data']['type'] = array('#type' => 'select',
      '#title' => t('Data type'),
      '#default_value' => $geo['type'],
      '#options' => $type_options,
    );
  }
  $form['gmaps']['gmaps_user_geo_data']['register'] = array('#type' => 'checkbox',
    '#title' => t('Collect data on registration'),
    '#default_value' => $geo['register'],
  );
  
  $form['gmaps']['gmaps_user_geo_data']['options'] = _gmaps_user_geo_data_options_form($geo, $delete_geo);
  
  $form['gmaps']['gmaps_user_geo_data']['widget'] = _gmaps_user_geo_data_widget_form($geo, $delete_geo);
  
  $form['gmaps']['gmaps_user_geo_data']['formatter'] = _gmaps_user_geo_data_formatter_form($geo, $delete_geo);
  
  $form['gmaps']['gmaps_user_geo_data']['meta'] = _gmaps_user_geo_data_meta_form($geo, $delete_geo);
  
  $form['gmaps']['gmaps_user_marker_option'] = _gmaps_user_marker_option_form($mo, $delete_mo, $delete_mo_parts);
  
  $form['#submit'][] = '_gmaps_user_form_user_admin_settings_submit';
  //add weight for buttons
  $form['buttons']['#weight'] = 100;
  if ($delete_geo) {
    $form['buttons']['delete_gmaps_geo'] = array('#type' => 'submit',
      '#value' => t('Delete geodesical data'),
      '#submit' => array('_gmaps_user_form_user_admin_settings_submit_geo'),
      '#weight' => 102,
    );
  }
  if ($delete_mo) {
    $form['buttons']['delete_gmaps_mo'] = array('#type' => 'submit',
      '#value' => t('Delete marker option values'),
      '#submit' => array('_gmaps_user_form_user_admin_settings_submit_mo'),
      '#weight' => 103,
    );
  }
}

function _gmaps_user_geo_data_options_form($geo, $delete_geo) {
  $form = array('#type' => 'fieldset',
    '#title' => t('Data options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['required'] = array('#type' => 'select',
    '#title' => t('Required'),
    '#options' => gmaps_address_accuracy_levels(),
    '#default_value' => $geo['options']['required'],
  );
  $form['allowed_countries'] = array('#type' => 'select',
    '#title' => t('Allowed countries'),
    '#options' => gmaps_get_countries('names'),
    '#multiple' => TRUE,
    '#size' => 5,
    '#default_value' => $geo['options']['allowed_countries'],
    '#description' => t('If none selected all countries will be allowed.'),
  );
  $map_types = gmaps_map_get_types('names');
  $form['map_type'] = array('#type' => 'value', '#value' => GMAPS_MAP_EARTH);
  $form['map_type_display'] = array('#type' => 'item',
    '#title' => t('Map type'),
    '#value' => theme('placeholder', $map_types[GMAPS_MAP_EARTH]),
  );
  $form['enable_bounds'] = array('#type' => 'checkbox',
    '#title' => t('Enable bounds'),
    '#default_value' => $geo['options']['enable_bounds'],
  );
  $form['enable_3D'] = array('#type' => 'checkbox',
    '#title' => t('Enable elevation'),
    '#default_value' => $geo['options']['enable_3D'],
  );
  $form['enable_pov'] = array('#type' => 'checkbox',
    '#title' => t('Enable point of view'),
    '#default_value' => $geo['options']['enable_pov'],
  );
  $form['enable_privacy'] = array('#type' => 'checkbox',
    '#title' => t('Enable privacy'),
    '#default_value' => $geo['options']['enable_privacy'],
  );
  
  return $form;
}

function _gmaps_user_geo_data_widget_form($geo, $delete_geo) {
  $form = array('#type' => 'fieldset',
    '#title' => t('Widget options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['enable_geocoder'] = array('#type' => 'checkbox',
    '#title' => t('Enable geocoder'),
    '#default_value' => $geo['widget']['enable_geocoder'],
  );
  $form['use_map'] = array('#type' => 'checkbox',
    '#title' => t('Use input map'),
    '#default_value' => $geo['widget']['use_map'],
  );
  $form['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $geo['widget']['collapsible'],
  );
  $form['collapsed'] = array('#type' => 'select',
    '#title' => t('Collapsed'),
    '#default_value' => $geo['widget']['collapsed'],
    '#options' => array(-1 => t('When not empty'), 0 => t('Collapsible'), 1 => t('Collapsed'))
  );
  $form['input_disabled'] = array('#type' => 'checkbox',
    '#title' => t('Disable numeric input fields'),
    '#default_value' => $geo['widget']['input_disabled'],
  );
  $form['tabbed_parts'] = array('#type' => 'select',
    '#title' => t('Show parts on tabs'),
    '#default_value' => $geo['widget']['tabbed_parts'],
    '#options' => array(-1 => t('When not empty'), 0 => t('Never'), 1 => t('Always'))
  );
  
  return $form;
}

function _gmaps_user_geo_data_formatter_form($geo, $delete_geo) {
  $countryicons = module_exists('countryicons');
  $languageicons = module_exists('languageicons');
  
  module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
  
  $s = $geo['formatter'];
  
  $form = array('#type' => 'fieldset',
    '#title' => t('Formatter options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['formatter'] = array('#type' => 'select',
    '#title' => t('Formatter'),
    '#default_value' => $s['formatter'],
    '#options' => _gmaps_user_geo_data_formatters(),
  );
  $form['show'] = array('#type' => 'select',
    '#title' => t('Show part'),
    '#default_value' => $s['show'],
    '#options' => array(
      'address' => t('Address'),
      'point' => t('Point'),
      'anp' => t('Address and point'),
      'pna' => t('Point and address'),
    ),
  );
  
  $form['address'] = array('#type' => 'fieldset',
    '#title' => t('Address display'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['address']['user_settings'] = array('#type' => 'checkbox',
    '#title' => t('Use user/site settings'),
    '#default_value' => $s['address']['user_settings'],
  );
  $form['address']['country'] = array('#type' => 'select',
    '#title' => t('Country display'),
    '#default_value' => $s['address']['country'],
    '#options' => _gmaps_address_get_country_options(),
  );
  $form['address']['state'] = array('#type' => 'select',
    '#title' => t('State display'),
    '#default_value' => $s['address']['state'],
    '#options' => _gmaps_address_get_state_options(),
    '#description' => t('Applied to US and Canadian states only.'),
  );
  $form['address']['flag'] = array('#type' => 'select',
    '#title' => t('Display country flags'),
    '#default_value' => $s['address']['flag'],
    '#options' => _gmaps_address_get_flag_options(),
    '#access' => ($countryicons || $languageicons),
  );
  if (module_exists('gmaps_profile')) {
    $form['address']['profile_links'] = array('#type' => 'checkbox',
      '#title' => t('With profile links'),
      '#default_value' => $s['address']['profile_links'],
    );
  }
  else {
    $form['address']['profile_links'] = array('#type' => 'value', '#value' => 0);
  }
  
  $form['point'] = array('#type' => 'fieldset',
    '#title' => t('Point display'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['point']['user_settings'] = array('#type' => 'checkbox',
    '#title' => t('Use user/site settings'),
    '#default_value' => $s['point']['user_settings'],
  );
  $form['point']['format'] = array('#type' => 'select',
    '#title' => t('Format'),
    '#default_value' => $s['point']['format'],
    '#options' => _gmaps_coordinate_get_format_options(),
    '#description' => t('Cartesian format can fall back to decimal degrees on non-convertable coordinates (sky for example).'),
  );
  $form['point']['elevation'] = array('#type' => 'select',
    '#title' => t('Elevation display'),
    '#default_value' => $s['point']['elevation'],
    '#options' => _gmaps_coordinate_get_elevation_options(),
  );

  $precisions = _gmaps_length_get_precision_options();
  $form['point']['dd_prec'] = array('#type' => 'select',
    '#title' => t('DD precision'),
    '#default_value' => $s['point']['dd_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['point']['dms_prec'] = array('#type' => 'select',
    '#title' => t('Second precision in DMS'),
    '#default_value' => $s['point']['dms_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['point']['cart_prec'] = array('#type' => 'select',
    '#title' => t('CART precision'),
    '#default_value' => $s['point']['cart_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['point']['elevation_prec'] = array('#type' => 'select',
    '#title' => t('Elevation precision'),
    '#default_value' => $s['point']['elevation_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );

  $form['map'] = array('#type' => 'fieldset',
    '#title' => t('Map display'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['map']['map'] = array('#type' => 'select',
    '#title' => t('Map'),
    '#default_value' => $s['map']['map'],
    '#options' => array(0 => t('Disabled')) + gmaps_map_get_options(GMAPS_MAP_EARTH),
  );
  $form['map']['static_map'] = array('#type' => 'select',
    '#title' => t('Static map'),
    '#default_value' => $s['map']['static_map'],
    '#options' => array(0 => t('Disabled')) + gmaps_static_map_get_options(),
  );
  $form['map']['cache_notes'] = array('#type' => 'item',
    '#title' => t('Cache notes'),
    '#value' => t('When privacy enabled for this field or field value is displayed on the map and GMaps user settings are enabled, then cache ids contain both the node id and the user id. In this case, the field can have as many cache records as the result of the nodes * users expression, which can extremly slow down your system. To avoid this problem, you should disable the cache or set the expiration to a small but non-zero value.'),
    '#attributes' => array('class' => 'error'),
  );
  $form['map']['cache'] = array('#type' => 'checkbox',
    '#title' => t('Cache maps'),
    '#default_value' => $s['map']['cache'],
  );
  $form['map']['cache_expires'] = array('#type' => 'textfield',
    '#title' => t('Cache expires'),
    '#default_value' => $s['map']['cache_expires'],
    '#description' => t('Relative seconds from the time of map rendering. For example 21600 for six hours. Leave it empty or set to <strong>0 (zero) for PERMANENT caching</strong>, or to <strong>-1 for TEMPORARY caching</strong>.'),
  );
  $form['map']['content'] = array('#type' => 'select',
    '#title' => t('Content formatter'),
    '#default_value' => $s['map']['content'],
    '#options' => array(
      'hidden' => t('<Hidden>'),
      'default' => t('Default (User/site render)'),
      'plain' => t('Plain text'),
      'themed' => t('Themed'),
    ),
  );
  
  $form['map_links'] = array('#type' => 'fieldset',
    '#title' => t('Map links'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['map_links']['google_find'] = array('#type' => 'checkbox',
    '#title' => t('Find on Google Maps'),
    '#default_value' => $s['map_links']['google_find'],
  );
  $form['map_links']['google_directions_from'] = array('#type' => 'checkbox',
    '#title' => t('Google directions from'),
    '#default_value' => $s['map_links']['google_directions_from'],
  );
  $form['map_links']['google_directions_to'] = array('#type' => 'checkbox',
    '#title' => t('Google directions to'),
    '#default_value' => $s['map_links']['google_directions_to'],
  );
  $form['map_links']['yahoo_find'] = array('#type' => 'checkbox',
    '#title' => t('Find on Yahoo! Maps'),
    '#default_value' => $s['map_links']['yahoo_find'],
  );
  $form['map_links']['yahoo_directions_from'] = array('#type' => 'checkbox',
    '#title' => t('Yahoo! directions from'),
    '#default_value' => $s['map_links']['yahoo_directions_from'],
  );
  $form['map_links']['yahoo_directions_to'] = array('#type' => 'checkbox',
    '#title' => t('Yahoo! directions to'),
    '#default_value' => $s['map_links']['yahoo_directions_to'],
  );
  $form['map_links']['mapquest_find'] = array('#type' => 'checkbox',
    '#title' => t('Find on Mapquest'),
    '#default_value' => $s['map_links']['mapquest_find'],
  );
  $form['map_links']['mapquest_directions_from'] = array('#type' => 'checkbox',
    '#title' => t('Mapquest directions from'),
    '#default_value' => $s['map_links']['mapquest_directions_from'],
  );
  $form['map_links']['mapquest_directions_to'] = array('#type' => 'checkbox',
    '#title' => t('Mapquest directions to'),
    '#default_value' => $s['map_links']['mapquest_directions_to'],
  );
  
  return $form;
}

function _gmaps_user_geo_data_meta_form($geo, $delete_geo) {
  $form = array('#type' => 'fieldset',
    '#title' => t('Meta options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['set_refloc'] = array('#type' => 'checkbox',
    '#title' => t('Set reference location when user logs in.'),
    '#default_value' => $geo['meta']['set_refloc'],
  );
  $form['meta'] = array('#type' => 'fieldset',
    '#title' => t('META tags'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['meta']['geo_placename'] = array('#type' => 'checkbox',
    '#title' => t('Set geo.placename META tag'),
    '#default_value' => $geo['meta']['geo_placename'],
  );
  $form['meta']['geo_region'] = array('#type' => 'checkbox',
    '#title' => t('Set geo.region META tag'),
    '#default_value' => $geo['meta']['geo_region'],
  );
  $form['meta']['geo_position'] = array('#type' => 'checkbox',
    '#title' => t('Set geo.position META tag'),
    '#default_value' => $geo['meta']['geo_position'],
  );
  $form['meta']['icbm'] = array('#type' => 'checkbox',
    '#title' => t('Set ICBM (Geourl) META tag'),
    '#default_value' => $geo['meta']['icbm'],
  );
  
  return $form;
}

function _gmaps_user_marker_option_form($mo, $delete_mo, $delete_mo_parts) {
  $form = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Marker options'),
    '#id' => 'mo',
    '#tree' => TRUE,
  );
  
  $form['register'] = array('#type' => 'checkbox',
    '#title' => t('Collect data on registration'),
    '#default_value' => $mo['register'],
  );
  
  //mo:icon
  $form['icon'] = array('#type' => 'fieldset',
    '#title' => t('Icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['icon']) {
    $form['icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['icon']['required'] = array('#type' => 'value', '#value' => $mo['icon']['required']);
    $form['icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable icons for users'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable icons for users'),
      '#default_value' => $mo['icon']['enabled'],
    );
    $form['icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['icon']['required'],
    );
  }
  //mo:protected icon
  $form['protected_icon'] = array('#type' => 'fieldset',
    '#title' => t('Protected icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['protected_icon']) {
    $form['protected_icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['protected_icon']['required'] = array('#type' => 'value', '#value' => $mo['protected_icon']['required']);
    $form['protected_icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable protected icons for users'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['protected_icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['protected_icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['protected_icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable protected icons for users'),
      '#default_value' => $mo['protected_icon']['enabled'],
    );
    $form['protected_icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['protected_icon']['required'],
    );
  }
  //mo:static icon
  $form['static_icon'] = array('#type' => 'fieldset',
    '#title' => t('Static icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['static_icon']) {
    $form['static_icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['static_icon']['required'] = array('#type' => 'value', '#value' => $mo['static_icon']['required']);
    $form['static_icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable static icons for users'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['static_icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['static_icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['static_icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable static icons for users'),
      '#default_value' => $mo['static_icon']['enabled'],
    );
    $form['static_icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['static_icon']['required'],
    );
  }
  //mo: protected static icon
  $form['protected_static_icon'] = array('#type' => 'fieldset',
    '#title' => t('Protected static con'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($delete_mo_parts['protected_static_icon']) {
    $form['protected_static_icon']['enabled'] = array('#type' => 'value', '#value' => 1);
    $form['protected_static_icon']['required'] = array('#type' => 'value', '#value' => $mo['protected_static_icon']['required']);
    $form['protected_static_icon']['enabled_display'] = array('#type' => 'item',
      '#title' => t('Enable protected static icons for users'),
      '#value' => theme('placeholder', t('Enabled')),
    );
    $form['protected_static_icon']['required_display'] = array('#type' => 'item',
      '#title' => t('Required'),
      '#value' => theme('placeholder', $mo['protected_static_icon']['required'] ? t('Yes') : t('No')),
    );
  }
  else {
    $form['protected_static_icon']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable protected static icons for users'),
      '#default_value' => $mo['protected_static_icon']['enabled'],
    );
    $form['protected_static_icon']['required'] = array('#type' => 'checkbox',
      '#title' => t('Required'),
      '#default_value' => $mo['protected_static_icon']['required'],
    );
  }

  return $form;
}

function _gmaps_user_form_user_admin_settings_submit(&$form, &$form_state) {
  variable_del('delete_gmaps_geo');
  variable_del('delete_gmaps_mo');
}

function _gmaps_user_form_user_admin_settings_submit_geo(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/user/delete-gmaps/geo';
}

function _gmaps_user_form_user_admin_settings_submit_mo(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/user/delete-gmaps/mo';
}

function gmaps_user_data_confirm_delete(&$form_state, $type) {
  switch($type) {
    case 'geo':
      $form['type'] = array('#type' => 'value', '#value' => $type);
      return confirm_form($form,
                      t('Are you sure you want to delete all geodesical data of all users?'),
                      'admin/user/settings',
                      t('This action cannot be undone.'),
                      t('Delete'),
                      t('Cancel'));
      break;
      
    case 'mo':
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
                      t('Are you sure you want to delete marker option values of all users?'),
                      'admin/user/settings',
                      t('This action cannot be undone.'),
                      t('Delete'),
                      t('Cancel'));
      break;
    
  }
  return array();
}

function gmaps_user_data_confirm_delete_submit(&$form, &$form_state) {
  if ($form_state['values']['confirm']) {
    if ($form_state['values']['type'] == 'geo') {
      $result = db_query('SELECT * FROM {users} u WHERE u.uid > 0');
      while ($account = db_fetch_object($result)) {
        _gmaps_user_geo_data_delete($account->uid);
      }
    }
    else if ($form_state['values']['type'] == 'mo') {
      if ($form_state['values']['mode'] == 'parts') {
        $parts = array_keys(array_filter($form_state['values']['parts']));
      }
      if (!isset($parts) || !empty($parts)) {
        $result = db_query('SELECT * FROM {users} u WHERE u.uid > 0');
        while ($account = db_fetch_object($result)) {
          _gmaps_user_mo_data_delete($account->uid, $parts);
        }
      }
    }
  }
  
  $form_state['redirect'] = 'admin/user/settings';
}

function _gmaps_user_geo_data_formatters() {
  return array(
    'hidden' => t('<Hidden>'),
    'default' => t('Default (User/site render)'),
    'plain' => t('Plain text'),
    'themed' => t('Themed'),
    'map' => t('GMaps Map'),
    'static_map' => t('GMaps Static map'),
  );
}

function _gmaps_user_edit_form(&$edit, &$account, $register = FALSE) {
  $geo = variable_get('gmaps_user_geo_data', array());
  $mo = variable_get('gmaps_user_marker_option', array());
  
  if ($register && !($geo['register'] || $mo['register'])) {
    return;
  }
  
  if (!$geo['type'] && !$mo['icon']['enabled'] && !$mo['protected_icon']['enabled'] && !$mo['static_icon']['enabled'] && !$mo['protected_static_icon']['enabled']) {
    return;
  }
  
  $uid = ($account) ? $account->uid : 0;
  
  if (!empty($uid)) {
    $geo_data = $account->gmaps['geo'];
    $mo_data = $account->gmaps['mo'];
  }
  if (empty($mo_data)) {
    $mo_data = _gmaps_user_mo_get_defaults($mo);
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
  
  return $form;
}

function _gmaps_user_mo_get_defaults($options) {
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

function _gmaps_user_geo_data_save(&$edit) {
  $geo = variable_get('gmaps_user_geo_data', array());
  
  if (empty($geo['type'])) {
    return;
  }
  
  switch($geo['type']) {
    case 'gmaps_address':
      $status = _gmaps_user_geo_address_save($edit);
      break;
      
    case 'gmaps_point':
      $status = _gmaps_user_geo_point_save($edit);
      break;
      
    case 'gmaps_anp':
      $status = _gmaps_user_geo_anp_save($edit);
      break;
  }
  
  return $status;
}

function _gmaps_user_geo_address_save(&$adr) {
  $result = SAVED_NEW;
  if (_gmaps_user_geo_data_delete($adr->uid) == SAVED_DELETED) {
    $result = SAVED_UPDATED;
  }
  
  if (gmaps_address_empty($adr)) {
    return;
  }
  
  gmaps_address_prepare_record($adr);
  
  drupal_write_record('gmaps_user_address', $adr);
  
  return $result;
}

function _gmaps_user_geo_point_save(&$point) {
  $result = SAVED_NEW;
  if (_gmaps_user_geo_data_delete($point->uid) == SAVED_DELETED) {
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
  
  drupal_write_record('gmaps_user_point', $point);
    
  return $result;
}

function _gmaps_user_geo_anp_save(&$anp) {
  $result = SAVED_NEW;
  if (_gmaps_user_geo_data_delete($anp->uid) == SAVED_DELETED) {
    $result = SAVED_UPDATED;
  }
  
  if (gmaps_anp_empty($anp)) {
    return;
  }
  
  $adr = &$anp->address;
  $adr->uid = $anp->uid;
  $adr->search = $anp->search;
  
  gmaps_address_prepare_record($adr);
  drupal_write_record('gmaps_user_address', $adr);
  
  $point = &$anp->point;
  $point->uid = $anp->uid;
  $point->search = $anp->search;
  
  gmaps_coords_add_cart($point);
  if (property_exists($point, 'bounds') && gmaps_bounds_empty($point->bounds)) {
    $point->bounds = array();
  }
  if (property_exists($point, 'pov') && gmaps_pov_empty($point->pov)) {
    $point->pov = array();
  }
  
  drupal_write_record('gmaps_user_point', $point);
    
  return $result;
}

function _gmaps_user_geo_data_delete($uid) {
  db_query("DELETE FROM {gmaps_user_address} WHERE uid = %d", $uid);
  if (db_affected_rows()) {
    $result = SAVED_DELETED;
  }
  db_query("DELETE FROM {gmaps_user_point} WHERE uid = %d", $uid);
  if (db_affected_rows()) {
    $result = SAVED_DELETED;
  }
  
  return $result;
}

function _gmaps_user_mo_data_save(&$edit) {
  $uid = $edit['uid'];
  
  $mo = variable_get('gmaps_user_marker_option', array(
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
  ));
  
  $icon = $mo['icon']['enabled'];
  $p_icon = $mo['protected_icon']['enabled'];
  $s_icon = $mo['static_icon']['enabled'];
  $ps_icon = $mo['protected_static_icon']['enabled'];
  
  $result = SAVED_NEW;
  if (_gmaps_user_mo_data_delete($uid) == SAVED_DELETED) {
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
  
  drupal_write_record('gmaps_user_marker_option', $edit);
    
  return $result;
}

function _gmaps_user_mo_data_delete($uid, $parts = array()) {
  if (empty($parts)) {
    db_query("DELETE FROM {gmaps_user_marker_option} WHERE uid = %d", $uid);
    if (db_affected_rows()) {
      $result = SAVED_DELETED;
    }
  }
  else {
    if ($mo_data = gmaps_user_mo_data_load($uid)) {
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
      
      drupal_write_record('gmaps_user_marker_option', $mo_data, array('uid'));
      $result = SAVED_DELETED;
    }
  }
  
  return $result;
}

function _gmaps_user_form_devel_generate_users_form_submit($form, &$form_state) {
  static $icons;
  
  $geo = variable_get('gmaps_user_geo_data', array());
  $mo = variable_get('gmaps_user_marker_option', array());
  
  if ($form_state['values']['kill_users']) {
    db_query('DELETE FROM {gmaps_user_address} WHERE uid > 1');
    db_query('DELETE FROM {gmaps_user_point} WHERE uid > 1');
    db_query('DELETE FROM {gmaps_user_marker_option} WHERE uid > 1');
  }
  
  $result = db_query('SELECT uid FROM {users} WHERE uid > 1');
  while($account = db_fetch_object($result)) {
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
        $edit->uid = $account->uid;
        _gmaps_user_geo_data_save($edit);
      }
    }
    
    if ($mo['icon']['enabled'] || $mo['protected_icon']['enabled'] || $mo['static_icon']['enabled'] || $mo['protected_static_icon']['enabled']) {
      if (is_null($icons)) {
        $icons = array_keys(array(GMAPS_ICON_MAP_DEFAULT => 1, GMAPS_ICON_API_DEFAULT => 1) + gmaps_get_icons());
        $icons_count = count($icons) - 1;
      }
      
      $edit = array(
        'uid' => $account->uid,
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
      
      _gmaps_user_mo_data_save($edit);
    }
  }
}
