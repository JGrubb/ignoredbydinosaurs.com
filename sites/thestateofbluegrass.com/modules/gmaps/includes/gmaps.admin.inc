<?php
// $Id: gmaps.admin.inc,v 1.1.2.13 2010/05/06 15:15:07 xmarket Exp $

/**
 * @file
 * Core settings.
 */

/********************
 * Public functions *
 ********************/

/**
 * Admin settings form.
 */
function gmaps_admin_settings() {
  module_load_include('inc', 'gmaps', 'includes/gmaps.address');

  $form['api'] = array('#type' => 'fieldset',
    '#title' => t('API settings'),
    '#weight' => -10,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  if (module_exists('keys')) {
    $key = keys_get_key(array('service' => 'google_maps'));
    if (!empty($key)) {
      $message = t('Google Maps Tools currently using the API key set via keys.module.');
    } else {
      $message = '<div class="error">'. t('Please define a Google Maps API Key in !keys config.', array('!keys' => l('Keys', 'admin/settings/keys'))) .'</div>';
    }
    $form['api']['gmaps_api_key'] = array(
      '#value' => $message,
      '#prefix' => '<div>',
      '#suffix' => '</div>'
    );
  } else {
    $key = variable_get('gmaps_api_key', '');
    $form['api']['gmaps_api_key'] = array('#type' => 'textfield',
      '#title' => t('Google MAPS API key'),
      '#default_value' => $key,
      '#required' => TRUE,
    );
  }
  $form['api']['#collapsed'] = !empty($key);

  $form['api']['gmaps_api_settings'] = array('#tree' => TRUE);
  $settings = gmaps_get_settings('gmaps_api_settings');

  $form['api']['gmaps_api_settings']['domain'] = array('#type' => 'select',
    '#title' => t('Default API domain'),
    '#default_value' => $settings['domain'],
    '#options' => _gmaps_get_maps_api_domain_options(),
    '#description' => t('The API will be downloaded from this domain, and this will be the default for the geocoder, too.'),
  );
  $form['api']['gmaps_api_settings']['site_language'] = array('#type' => 'radios',
    '#title' => t('Use site language'),
    '#default_value' => $settings['site_language'],
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('The language of the current request (depends on language settings) will be added to the API call as an argument when enabled, otherwise the argument is skipped and the API will determine the current language depending on the browser settings. If the sent code is not supported by the API, then english is used and browser settings are skipped. For supported language codes, see the <a href="!url">Maps Coverage - Google Document</a>.', array('!url' => url('http://spreadsheets.google.com/pub?key=p9pdwsai2hDMsLkXsoM05KQ&gid=1', array('absolute' => TRUE)))),
  );
  $form['api']['gmaps_api_settings']['geocode_lifetime'] = array('#type' => 'select',
    '#title' => t('Geocode cache lifetime'),
    '#default_value' => $settings['geocode_lifetime'],
    '#options' => array(0 => t('Disabled')) + drupal_map_assoc(array(3600, 7200, 10800, 21600, 43200, 86400, 172800, 259200, 604800, 1209600, 1814400, 2419200, 4838400, 7257600, 15724800, 31536000), 'format_interval'),
    '#description' => t('Geocoder API responses will be cached for this interval.'),
  );

  $form['api']['gmaps_elevation_service'] = array('#type' => 'select',
    '#title' => t('Elevation service'),
    '#default_value' => variable_get('gmaps_elevation_service', ''),
    '#options' => array('' => t('Disabled'), 'gtopo30' => t('GTOPO30 (~1 kilometer resolution)'), 'srtm3' => t('SRTM3 (~90 meters resolution)')),
    '#description' => t('SRTM3 data available only in the range N 60˚ - S 56˚. Out of its range GTOPO30 data will be used.'),
  );

  $form['api']['gmaps_external_js_compression'] = array('#type' => 'checkbox',
    '#title' => t('Use compressed external JS files'),
    '#default_value' => variable_get('gmaps_external_js_compression', 1),
    '#description' => t('External JS libraries will be added to pages in packed or minified versions when enabled.'),
  );

  $settings = gmaps_get_settings('gmaps_api_autoload');
  $form['api']['gmaps_api_autoload'] = array('#type' => 'fieldset',
    '#title' => t('Autoload settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['api']['gmaps_api_autoload']['enabled'] = array('#type' => 'select',
    '#title' => t('Enable API autoload'),
    '#default_value' => $settings['enabled'],
    '#options' => array(0 => t('Disabled'), 1 => t('IE6 only'), 2 => t('All browsers')),
  );
  
  $access = user_access('use PHP for gmaps API autoload');

  if ($settings['mode'] == 2 && !$access) {
    $form['api']['gmaps_api_autoload']['mode'] = array('#type' => 'value', '#value' => 2);
    $form['api']['gmaps_api_autoload']['pages'] = array('#type' => 'value', '#value' => $settings['pages']);
  }
  else {
    $options = array(t('Autoload on every page except the listed pages.'), t('Autoload on only the listed pages.'));
    $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if ($access) {
      $options[] = t('Autoload if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
      $description .= ' '. t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    $form['api']['gmaps_api_autoload']['mode'] = array(
      '#type' => 'radios',
      '#title' => t('Autoload API on specific pages'),
      '#options' => $options,
      '#default_value' => $settings['mode'],
    );
    $form['api']['gmaps_api_autoload']['pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Pages'),
      '#default_value' => $settings['pages'],
      '#description' => $description,
    );
  }
  
  
  $settings = gmaps_get_settings('gmaps_path_settings');
  $form['gmaps_path_settings'] = array('#type' => 'fieldset',
    '#title' => t('Path settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $parent = file_directory_path() .'/';
  $form['gmaps_path_settings']['base'] = array('#type' => 'textfield',
    '#title' => t('Base path'),
    '#default_value' => $settings['base'],
    '#required' => TRUE,
    '#field_prefix' => $parent,
    '#description' => t('Base directory for gmaps related files a directories.'),
  );
  $parent .= $settings['base'] .'/';
  $form['gmaps_path_settings']['icon'] = array('#type' => 'textfield',
    '#title' => t('Icon path'),
    '#default_value' => $settings['icon'],
    '#required' => TRUE,
    '#field_prefix' => $parent,
    '#description' => t('Directory of gmaps related icon files.'),
  );
  $form['gmaps_path_settings']['library'] = array('#type' => 'textfield',
    '#title' => t('Library path'),
    '#default_value' => $settings['library'],
    '#required' => TRUE,
    '#field_prefix' => $parent,
    '#description' => t('The content of uploded icon libraries will be extracted to this directory.'),
  );
  $form['gmaps_path_settings']['smcache'] = array('#type' => 'textfield',
    '#title' => t('Cache path for static maps'),
    '#default_value' => $settings['smcache'],
    '#required' => TRUE,
    '#field_prefix' => $parent,
    '#description' => t('Downloaded static map images will be kept in this directory.'),
  );

  $settings = variable_get('gmaps_address_part_labels', array());
  $form['address'] = array('#type' => 'fieldset',
    '#title' => t('Address settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['address']['gmaps_address_part_labels'] = array('#type' => 'fieldset',
    '#title' => t('Labels'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('Site-wide default labels for address detail fields <strong>in english.</strong> Empty fields will reset to default labels.'),
  );
  foreach(_gmaps_address_get_parts() as $part => $label) {
    $form['address']['gmaps_address_part_labels'][$part] = array('#type' => 'textfield',
      '#title' => t($label),
      '#default_value' => $settings[$part],
      '#description' => t('Default label in english: %label', array('%label' => $label)),
    );
  }
  $countries = array();
  $allowed_countries = variable_get('gmaps_address_allowed_countries', array());
  $allowed = t('All');
  foreach (geonames_countries() as $c) {
    if (empty($allowed) || isset($allowed[$c['iso_alpha2']])) {
      $countries[$c['iso_alpha2']] = t($c['name']);
    }
  }
  if ($countries) {
    asort($countries);
    if ($allowed_countries) {
      $allowed = array();
      foreach($allowed_countries as $country) {
        $allowed[] = $countries[$country];
      }
      asort($allowed);
      $allowed = implode(', ', $allowed);
    }
  }
  $form['address']['gmaps_address_allowed_countries'] = array('#type' => 'select',
    '#title' => t('Allowed countries'),
    '#default_value' => $allowed_countries,
    '#options' => $countries,
    '#multiple' => TRUE,
    '#size' => 8,
    '#description' => t('Site-wide restriction. It will affect all GMaps address selector and element. Leave empty to allow all countries.') .'<p>'. t('Now allowed: @allowed', array('@allowed' => $allowed)) .'</p>',
  );

  $form['settings'] = array('#type' => 'fieldset',
    '#title' => t('Display settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['settings']['gmaps_enable_user_settings'] = array('#type' => 'checkbox',
    '#title' => t('Enable user configurable settings'),
    '#default_value' => variable_get('gmaps_enable_user_settings', 0),
    '#description' => t('When enabled, users can set their own settings.'),
  );
  $form['settings']['gmaps_throttle_support'] = array('#type' => 'checkbox',
    '#title' => t('Enable throttle support'),
    '#default_value' => variable_get('gmaps_throttle_support', 0),
    '#description' => t('See descriptions below for effects.'),
  );

  $settings = gmaps_get_settings('gmaps_length_settings');
  $form['settings']['gmaps_length_settings'] = _gmaps_length_settings_form($settings);
  $form['settings']['gmaps_length_settings']['#description'] = t("<strong>Throttle effect:</strong> units are displayed 'as is' and formatted as plain text.");

  $settings = gmaps_get_settings('gmaps_address_settings');
  $form['settings']['gmaps_address_settings'] = _gmaps_address_settings_form($settings);
  $form['settings']['gmaps_address_settings']['#description'] = '<p>'. t("<strong>Throttle effect:</strong> addresses are formatted as plain text, countries asre displayed by their name, states are displayed 'as is' and flags are disabled.") .'</p>'. $form['settings']['gmaps_address_settings']['#description'];

  $settings = gmaps_get_settings('gmaps_coordinate_settings');
  $form['settings']['gmaps_coordinate_settings'] = _gmaps_coordinate_settings_form($settings);
  $form['settings']['gmaps_coordinate_settings']['#description'] = '<p>'. t("<strong>Throttle effect:</strong> coordinates are formatted as plain text in DD format.") .'</p>'. $form['settings']['gmaps_coordinate_settings']['#description'];

  $settings = gmaps_get_settings('gmaps_icon_settings');
  $form['gmaps_icon_settings'] = array('#type' => 'fieldset',
    '#title' => t('Icon settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['gmaps_icon_settings']['min_size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Minimum icon size'),
    '#default_value' => $settings['min_size'],
    '#required' => TRUE,
    '#min_pixels' => GMAPS_IMAGE_BULLET,
    '#max_pixels' => GMAPS_IMAGE_LARGE_THUMBNAIL,
    '#description' => t('Applied to all uploaded icon files.'),
  );
  $form['gmaps_icon_settings']['max_size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Maximum icon size'),
    '#default_value' => $settings['max_size'],
    '#required' => TRUE,
    '#min_pixels' => GMAPS_IMAGE_BULLET,
    '#max_pixels' => GMAPS_IMAGE_LARGE_THUMBNAIL,
  '#description' => t('Applied to all uploaded icon files.'),
  );
  $form['gmaps_icon_settings']['file_size'] = array('#type' => 'textfield',
    '#title' => t('Maximum file size'),
    '#default_value' => $settings['file_size'],
    '#required' => TRUE,
    '#field_suffix' => 'kB',
    '#size' => 19,
    '#description' => t('Maximum file size of the uploaded icons.'),
  );
  $form['gmaps_icon_settings']['preview_size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Maximum preview size'),
    '#default_value' => $settings['preview_size'],
    '#required' => TRUE,
    '#min_pixels' => GMAPS_IMAGE_BULLET,
    '#max_pixels' => GMAPS_IMAGE_LARGE_THUMBNAIL,
    '#description' => t('Applied to all icon previews. Icons larger than this size will be resized (on display) to this size.'),
  );
  $form['gmaps_icon_settings']['inline_preview_size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Inline preview size'),
    '#default_value' => $settings['inline_preview_size'],
    '#required' => TRUE,
    '#min_pixels' => 0,
    '#max_pixels' => GMAPS_IMAGE_LARGE_ICON,
    '#description' => t("You can disable icon preview on select elements using '0'. Applied to all icon select elements."),
  );

  $form['se_support'] = array('#type' => 'fieldset',
    '#title' => t('Search engine support'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['se_support']['gmaps_se_cache_lifetime'] = array('#type' => 'select',
    '#title' => t('Cache lifetime'),
    '#default_value' => variable_get('gmaps_se_cache_lifetime', 2419200),
    '#options' => drupal_map_assoc(array(3600, 7200, 10800, 21600, 43200, 86400, 172800, 259200, 604800, 1209600, 1814400, 2419200, 4838400, 7257600, 15724800, 31536000), 'format_interval'),
    '#description' => t('How often download user-agents from <a href="!url">User-Agents.org</a>.', array('!url' => 'http://www.user-agents.org/')),
  );
  $form['se_support']['gmaps_se_track_access'] = array('#type' => 'checkbox',
    '#title' => t('Track search engine accesses'),
    '#default_value' => variable_get('gmaps_se_track_access', 1),
  );
  
  $form['cache'] = array('#type' => 'fieldset',
    '#title' => t('Cache'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['cache']['gmaps_map_cache'] = array('#type' => 'checkbox',
    '#title' => t('Enable map cache'),
    '#default_value' => variable_get('gmaps_map_cache', 1),
  );
  $form['cache']['gmaps_static_map_cache'] = array('#type' => 'checkbox',
    '#title' => t('Enable static map cache'),
    '#default_value' => variable_get('gmaps_static_map_cache', 1),
  );
  $form['cache']['gmaps_static_map_cache_image'] = array('#type' => 'checkbox',
    '#title' => t('Enable static map image cache'),
    '#default_value' => variable_get('gmaps_static_map_cache_image', 1),
  );
  $form['cache']['clear_geocode_cache'] = array('#type' => 'submit',
    '#value' => t('Clear geocode cache'),
    '#submit' => array('_gmaps_clear_geocode_cache_submit'),
  );
  $form['cache']['rebuild_length_unit_cache'] = array('#type' => 'submit',
    '#value' => t('Re-build length unit cache'),
    '#submit' => array('_gmaps_rebuild_length_unit_cache_submit'),
  );
  $form['cache']['rebuild_search_engine_cache'] = array('#type' => 'submit',
    '#value' => t('Re-build search engine cache'),
    '#submit' => array('_gmaps_rebuild_search_engine_cache_submit'),
  );
  
  $form['#validate'][] = '_gmaps_path_settings_validate';
  $form['#submit'][] = '_gmaps_path_settings_submit';
  $form['#submit'][] = '_gmaps_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * User settings form.
 *
 * @param array $edit
 *   The array of form values submitted by the user.
 * @return array
 *   A $form array containing the form elements to display.
 */
function gmaps_user_settings(&$edit) {
  $form = array();

  $form['gmaps_settings'] = array('#type' => 'fieldset',
    '#title' => t('GMaps display settings'),
    '#collapsible' => TRUE,
    '#collapsed' => isset($edit['gmaps_settings']),
    '#weight' => 10,
    '#tree' => TRUE,
  );

  $settings = _gmaps_user_get_settings((object)$edit);
  $form['gmaps_settings']['length'] = _gmaps_length_settings_form($settings['length']);

  $form['gmaps_settings']['address'] = _gmaps_address_settings_form($settings['address']);
  unset($form['gmaps_settings']['address']['#description']);

  $form['gmaps_settings']['coordinate'] = _gmaps_coordinate_settings_form($settings['coordinate']);

  return $form;
}



/**********************
 * Internal functions *
 **********************/


/**
 * Execute re-build length unit cache.
 */
function _gmaps_rebuild_length_unit_cache_submit($form, &$form_state) {
  _gmaps_length_get_units();
}

/**
 * Execute clear geocode cache.
 */
function _gmaps_clear_geocode_cache_submit($form, &$form_state) {
  cache_clear_all('*', 'cache_gmaps_geocode', TRUE);
}

/**
 * Execute re-build length unit cache.
 */
function _gmaps_rebuild_search_engine_cache_submit($form, &$form_state) {
  _gmaps_get_search_engines();
}

/**
 * Execute gmaps_admin_settings.
 *
 * Preprocess some form variables for system_settings_form_submit().
 */
function _gmaps_admin_settings_submit($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $values = &$form_state['values'];

  if ($op != t('Reset to defaults')) {
    $values['gmaps_address_part_labels'] = array_filter($values['gmaps_address_part_labels']);
  }

  $settings = gmaps_get_settings('gmaps_api_settings');
  if ($value['gmaps_api_settings']['domain'] != $settings['domain']) {
    cache_clear_all('*', 'cache_gmaps_geocode', TRUE);
  }
  //user settings option changed and now disabled
  if ($values['gmaps_enable_user_settings'] != variable_get('gmaps_enable_user_settings', 0) && !$values['gmaps_enable_user_settings']) {
    db_query('DELETE FROM {gmaps_user_settings}');
  }

  if (!$form_state['values']['gmaps_static_map_cache'] || !$form_state['values']['gmaps_static_map_cache_image']) {
    gmaps_static_map_cache_clear_all('*', NULL, TRUE);
  }
  
}

/**
 * Validate new file settings
 */
function _gmaps_path_settings_validate($form, &$form_state) {
  $settings = $form_state['values']['gmaps_path_settings'];
  $settings['base'] = file_create_path($settings['base']);
  //check subdirectories only if base is valid
  if (file_check_directory($settings['base'], FILE_CREATE_DIRECTORY, 'gmaps_path_settings][base')) {
    $settings['icon'] = $settings['base'] .'/'. $settings['icon'];
    file_check_directory($settings['icon'], FILE_CREATE_DIRECTORY, 'gmaps_path_settings][icon');
    $settings['library'] = $settings['base'] .'/'. $settings['library'];
    file_check_directory($settings['library'], FILE_CREATE_DIRECTORY, 'gmaps_path_settings][library');
    $settings['smcache'] = $settings['base'] .'/'. $settings['smcache'];
    file_check_directory($settings['smcache'], FILE_CREATE_DIRECTORY, 'gmaps_path_settings][smcache');
    if (!form_get_errors()) {
      $form_state['gmaps_new_path_settings'] = $settings;
    }
  }
}

/**
 * Move gmaps related files to their new location and update database if need.
 */
function _gmaps_path_settings_submit($form, &$form_state) {
  if (isset($form_state['gmaps_new_path_settings'])) {
    $new = &$form_state['gmaps_new_path_settings'];
    $old['base'] = gmaps_get_path('base');
    $old['icon'] = gmaps_get_path('icon');
    $old['library'] = gmaps_get_path('library');
    $old['smcache'] = gmaps_get_path('smcache');
  }
  else {
    return;
  }

  if ($old['smcache'] != $new['smcache']) {
    gmaps_static_map_cache_clear_all('*', NULL, TRUE);
    file_scan_directory($old['smcache'], '.+', array('.', '..'), 'file_delete');
    foreach(array_merge(array(GMAPS_CACHE_TABLE_STATIC_MAP), module_invoke_all('gmaps_caches', 'static_map')) as $cache) {
      $cache = $old['smcache'] .'/'. strtr($cache, '_', '-');
      if (is_dir($cache)) {
        @rmdir($cache);
      }
    }
    @rmdir($old['smcache']);
  }
  
  $operations = array();
  if ($old['icon'] != $new['icon']) {
    $operations[] = array('_gmaps_move_icon_files', array($old['icon'], $new['icon']));
  }
  if ($old['library'] != $new['library'] && ($libraries = file_scan_directory($old['library'], '.*\.gic\.info'))) {
    $operations[] = array('_gmaps_move_library_files', array($libraries, $old['library'], $new['library']));
  }
  if ($old['base'] != $new['base']) {
    $operations[] = array('_gmaps_remove_old_basedir', array($old['base']));
  }

  if (!empty($operations)) {
    $batch = array(
      'title' => t('Moving GMaps files'),
      'operations' => $operations,
      'finished' => '_gmaps_file_moving_finished',
      'file' => drupal_get_path('module', 'gmaps') .'/includes/gmaps.admin.inc',
    );
    batch_set($batch);
  }
}

/**
 * Length display settings
 */
function _gmaps_length_settings_form($settings) {
  $form = array('#type' => 'fieldset',
    '#title' => t('Length settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['unit'] = array('#type' => 'select',
    '#title' => t('Default length unit'),
    '#default_value' => is_object($settings['unit']) ? $settings['unit']->key : $settings['unit'],
    '#options' => array('' => t("Display 'as is'")) + _gmaps_length_get_unit_admin_options(),
    '#description' => t("If you choose a unit other than Display 'as is', then all length values will be displayed in that unit."),
  );
  $form['optmode'] = array('#type' => 'select',
    '#title' => t('Length optimization method'),
    '#default_value' => $settings['optmode'],
    '#options' => _gmaps_length_get_optimization_modes(),
    '#description' => t("It has effect only if the default unit is other than Display 'as is'. If you enable it, then default unit means default group, and all values will be converted within that group to the most human-readable value."),
  );
  $form['precision'] = array('#type' => 'select',
    '#title' => t('Precision'),
    '#default_value' => $settings['precision'],
    '#options' => _gmaps_length_get_precision_options(),
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['formatter'] = array('#type' => 'select',
    '#title' => t('Formatter'),
    '#default_value' => $settings['formatter'],
    '#options' => _gmaps_length_get_formatter_options(),
  );
  $form['input'] = array('#type' => 'select',
    '#title' => t('Input element'),
    '#default_value' => $settings['input'],
    '#options' => _gmaps_length_get_input_element_options(),
  );
  $form['multipart'] = array('#type' => 'checkbox',
    '#title' => t('Allow multiple parts'),
    '#default_value' => $settings['multipart'],
  );
  
  return $form;
}

/**
 * Address display settings
 */
function _gmaps_address_settings_form($settings) {
  $countryicons = module_exists('countryicons');
  $gmaps_taxonomy = module_exists('gmaps_taxonomy');
  //partially supported
  $languageicons = module_exists('languageicons');

  $form = array('#type' => 'fieldset',
    '#title' => t('Address settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['formatter'] = array('#type' => 'select',
    '#title' => t('Formatter'),
    '#default_value' => $settings['formatter'],
    '#options' => _gmaps_address_get_formatter_options(),
  );
  $form['country'] = array('#type' => 'select',
    '#title' => t('Country display'),
    '#default_value' => $settings['country'],
    '#options' => _gmaps_address_get_country_options(),
  );
  $form['state'] = array('#type' => 'select',
    '#title' => t('State display'),
    '#default_value' => $settings['state'],
    '#options' => _gmaps_address_get_state_options(),
    '#description' => t('Applied to US and Canadian states only.'),
  );
  $form['flag'] = array('#type' => 'select',
    '#title' => t('Display country flags'),
    '#default_value' => $settings['flag'],
    '#options' => _gmaps_address_get_flag_options(),
    '#access' => ($countryicons || $languageicons),
  );
  $gt = module_exists('gmaps_taxonomy');
  $form['taxonomy_links'] = array('#type' => 'checkbox',
    '#title' => t('With taxonomy links'),
    '#default_value' => $gt ? $settings['taxonomy_links'] : 0,
    '#access' => $gt,
  );
  
  if (!$countryicons || !$gmaps_taxonomy) {
    $desc = array();
    if (!$gmaps_taxonomy) {
      $desc[] = t('If you enable the GMaps Taxonomy module, then you can render addresses with links to location listing pages.');
    }
    if (!$countryicons) {
      $desc[] = t('If you install the <a href="!ci_url">Country Icons module</a>, then you can display country flags for all countries in themed addresses.', array('!ci_url' => 'http://drupal.org/project/countryicons'));
    }
    $form['#description'] = implode(' ', $desc);
  }

  return $form;
}

/**
 * Coordinate display settings
 */
function _gmaps_coordinate_settings_form($settings) {
  $form = array('#type' => 'fieldset',
    '#title' => t('Coordinate settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('Length settings, except precision, will be applied to CART coordinates and heights.'),
  );
  $form['formatter'] = array('#type' => 'select',
    '#title' => t('Formatter'),
    '#default_value' => $settings['formatter'],
    '#options' => _gmaps_coordinate_get_formatter_options(),
  );
  $form['format'] = array('#type' => 'select',
    '#title' => t('Format'),
    '#default_value' => $settings['format'],
    '#options' => _gmaps_coordinate_get_format_options(),
    '#description' => t('Cartesian format can fall back to decimal degrees on non-convertable coordinates (sky for example).'),
  );
  $form['elevation'] = array('#type' => 'select',
    '#title' => t('Elevation display'),
    '#default_value' => $settings['elevation'],
    '#options' => _gmaps_coordinate_get_elevation_options(),
  );

  $precisions = _gmaps_length_get_precision_options();
  $form['dd_prec'] = array('#type' => 'select',
    '#title' => t('DD precision'),
    '#default_value' => $settings['dd_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['dms_prec'] = array('#type' => 'select',
    '#title' => t('Second precision in DMS'),
    '#default_value' => $settings['dms_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['cart_prec'] = array('#type' => 'select',
    '#title' => t('CART precision'),
    '#default_value' => $settings['cart_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['elevation_prec'] = array('#type' => 'select',
    '#title' => t('Elevation precision'),
    '#default_value' => $settings['elevation_prec'],
    '#options' => $precisions,
    '#description' => t('Ammount of decimal digits to display.'),
  );

  return $form;
}

/**
 * Helper function for extensible length units.
 */
function _gmaps_length_parse_units_info() {
  $units = array();
  $fix = array(
    'groups' => array(
      'keys' => array('metric', 'english'),
      'metric.title' => 'Metric',
      'metric.optimize' => 1,
      'metric.multipart' => 0,
      'english.title' => 'English',
      'english.optimize' => 1,
      'english.multipart' => 1,
      'english.basis' => 'en_foot',
    ),
    'metric' => array(
      'keys' => array('meter'),
      'meter.title' => 'Meter',
      'meter.title_plural' => 'Meters',
      'meter.abbr' => 'm',
      'meter.m' => '1',
      'meter.optimize' => 1,
    ),
    'english' => array(
      'keys' => array('en_foot'),
      'en_foot.title' => 'Foot',
      'en_foot.title_plural' => 'Feet',
      'en_foot.abbr' => 'ft',
      'en_foot.m' => '0.3048',
      'en_foot.optimize' => 1,
      'en_foot.rate' => 1,
    ),
  );

  $path = './'. drupal_get_path('module', 'gmaps') .'/resources/length.units.info';
  if (is_file($path) && ($extra = parse_ini_file($path, TRUE))) {
    foreach ($extra as $group => $info) {
      if (isset($fix[$group])) {
        $fix[$group]['keys'] = array_unique(array_merge($fix[$group]['keys'], $info['keys']));
        $fix[$group] = array_merge($info, $fix[$group]);
      }
      else {
        $fix[$group] = $info;
      }
    }
  }
  //build group info
  $groups = array();
  foreach($fix['groups']['keys'] as $group) {
    if (!empty($group)) {
      $groups[$group]['key'] = $group;
      $groups[$group]['title'] = (isset($fix['groups'][$group.'.title']) && !empty($fix['groups'][$group.'.title'])) ?  $fix['groups'][$group.'.title'] : drupal_ucfirst($group);
      $groups[$group]['optimize'] = (isset($fix['groups'][$group.'.optimize']) && $fix['groups'][$group.'.optimize']) ?  1 : 0;
      $groups[$group]['multipart'] = (isset($fix['groups'][$group.'.multipart']) && $fix['groups'][$group.'.multipart']) ?  1 : 0;
      if (isset($fix['groups'][$group.'.basis']) && !empty($fix['groups'][$group.'.basis'])) {
        $groups[$group]['basis'] = $fix['groups'][$group.'.basis'];
      }
    }
  }
  unset($fix['groups']);

  //build units
  foreach($groups as $group => $group_info) {
    $group_units = array();
    $optimizable = 0;
    foreach($fix[$group]['keys'] as $key) {
      if (!empty($key)) {
        //skip unconvertable units. zero is not acceptable as rate, so empty can be used.
        if (isset($fix[$group][$key.'.m']) && !empty($fix[$group][$key.'.m'])) {
          $group_units[$key] = array(
            'key' => $key,
            'title' => (isset($fix[$group][$key.'.title']) && !empty($fix[$group][$key.'.title'])) ? $fix[$group][$key.'.title'] : drupal_ucfirst($key),
            'm' => eval('return (double)'.$fix[$group][$key.'.m'].';'),
            'optimize' => (isset($fix[$group][$key.'.optimize']) && $fix[$group][$key.'.optimize']) ? 1 : 0,
          );
          //can fallback to 'title'
          $group_units[$key]['title_plural'] = (isset($fix[$group][$key.'.title_plural']) && !empty($fix[$group][$key.'.title_plural'])) ? $fix[$group][$key.'.title_plural'] : $group_units[$key]['title'];
          if (isset($fix[$group][$key.'.abbr']) && !empty($fix[$group][$key.'.abbr'])) {
            $group_units[$key]['abbr'] = filter_xss($fix[$group][$key.'.abbr'], array('em', 'strong', 'sub', 'sup'));
            $group_units[$key]['abbr_option'] = filter_xss($fix[$group][$key.'.abbr'], array());
          }
          if (isset($group_info['basis']) && isset($fix[$group][$key.'.rate']) && !empty($fix[$group][$key.'.rate'])) {
            $group_units[$key]['rate'] = eval('return (double)'.$fix[$group][$key.'.rate'].';');
          }
          $optimizable = $group_units[$key]['optimize'] ? ++$optimizable : $optimizable;
        }
      }
    }

    if ($group_info['optimize'] && $optimizable == 0) {
      $group_info['optimize'] = 0;
    }

    uasort($group_units, '_gmaps_length_sort_units_by_rate');
    foreach ($group_units as $key => $info) {
      $info['group'] = $group_info;
      $units[$key] = $info;
    }
  }

  return $units;
}

/**
 * Helper function for usort to sort units by their rate to meter in a group.
 */
function _gmaps_length_sort_units_by_rate($a, $b) {
  if ($a['m'] == $b['m']) {
    return 0;
  }
  return ($a['m'] < $b['m']) ? -1 : 1;
}

/**
 * Read length units database.
 *
 * @return
 *  An array of available length units, keyed by unit keys. Titles are translated.
 */
function _gmaps_length_get_units() {
  $units = _gmaps_length_parse_units_info();

  drupal_alter('gmaps_length_units', $units);
  
  cache_set('gmaps_length_units', $units, 'cache');

  return $units;
}

/**
 * Helper function to generate values for the #options property of select elements.
 * An *(asterisk) added to optimizable groups and units.
 * This version can be used for administration purposes,
 * like configuring a default unit.
 * If need units for user input use gmaps_length_get_unit_options().
 *
 * @return
 *  An grouped array of length units.
 */
function _gmaps_length_get_unit_admin_options() {
  static $options = array();

  if (empty($options)) {
    foreach(gmaps_length_get_units() as $key => $info) {
      $options[$info['group']['title'] . ($info['group']['optimize'] ? '*' : '')][$key] = $info['title'] . ($info['optimize'] ? '*' : '') . (isset($info['abbr_option']) ? ' ('. $info['abbr_option'] .')' : '');
    }

    foreach($options as $group => $units) {
      asort($options[$group]);
    }
  }

  return $options;
}

function _gmaps_length_get_optimization_modes() {
  $options =  array(
    0 => t('Disabled'),
    1 => t('For integer digits'),
    2 => t('For decimal digits'),
  );
  return $options;
}

function _gmaps_length_get_precision_options() {
  $options = array(-1 => t('Varying')) + drupal_map_assoc(array(0,1,2,3,4,5,6));
  return $options;
}

function _gmaps_length_get_formatter_options() {
  $options = array(
    'plain' => t('Plain text'),
    'themed' => t('Themed length'),
  );
  return $options;
}

function _gmaps_length_get_input_element_options() {
  $options = array(
    'gmaps_distancefield' => t('Textfield only'),
    'gmaps_distance' => t('Textfield with unit selector'),
  );
  return $options;
}

function _gmaps_address_get_formatter_options() {
  $options = array(
    'plain' => t('Plain text'),
    'themed' => t('Themed address'),
  );
  return $options;
}

function _gmaps_address_get_country_options() {
  $options = array(
    'iso2' => t('ISO 3166-1 alpha-2 code only'),
    'iso3' => t('ISO 3166-1 alpha-3 code only'),
    'name' => t('Name only'),
    'iso2_name' => t('ISO 3166-1 alpha-2 code and name'),
    'iso3_name' => t('ISO 3166-1 alpha-3 code and name'),
    );
  return $options;
}

function _gmaps_address_get_state_options() {
  $options = array(
    '' => t("Display 'as is'"),
    'code' => t('Show code only'),
    'name' => t('Show name only'),
    'both' => t('Show both'),
  );
  return $options;
}

function _gmaps_address_get_flag_options() {
  $options = array(
    '' => t('Disabled'),
    'before' => t('Before'),
    'after' => t('After'),
    'replace' => t('Replace'),
  );
  return $options;
}

function _gmaps_coordinate_get_formatter_options() {
  $options = array(
    'plain' => t('Plain text'),
    'themed' => t('Themed coordinates'),
  );
  return $options;
}

function _gmaps_coordinate_get_format_options() {
  $options = array(
    'dd' => t('DD - Decimal degrees'),
    'dms' => t('DMS - Degree, minute, second'),
    'cart' => t('CART - Cartesian coordinates'),
  );
  return $options;
}

function _gmaps_coordinate_get_elevation_options() {
  $options = array(
    0 => t('Disabled'),
    1 => t('If non-zero'),
    2 => t('Always'),
  );
  return $options;
}

function _gmaps_move_icon_files($from, $to, &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_icon'] = 0;
    $context['sandbox']['max'] = db_result(db_query('SELECT COUNT(*) FROM {gmaps_icons}'));
    $context['finished'] = 0;
  }
  $t_args = array();

  module_load_include('inc', 'gmaps', 'includes/gmaps.icon-admin');
  //one icon can have seven images + db update.....
  $limit = 14;
  $result = db_query_range("SELECT * FROM {gmaps_icons} WHERE icid > %d ORDER BY icid ASC", $context['sandbox']['current_icon'], 0, $limit);
  while ($icon = db_fetch_object($result)) {
    $broken = FALSE;
    foreach(_gmaps_icon_get_file_fields() as $field) {
      if (!empty($icon->$field) && !_gmaps_icon_file_move($icon->$field, $to)) {
        $broken = TRUE;
        break;
      }
    }
    if ($broken) {
      $context['sandbox']['progress'] = $context['sandbox']['max'];
      break;
    }
    drupal_write_record('gmaps_icons', $icon, 'icid');
    $context['sandbox']['current_icon'] = $icon->icid;
    $context['sandbox']['progress']++;
    $t_args = array('@icon' => $icon->name);
  }
  $context['message'] = empty($t_args) ? '' : t('Moving icon: @icon', $t_args);

  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  } else {
    $context['finished'] = 1;
    if (!$broken && is_dir($from)) {
      @rmdir($from);
    }
  }
}

function _gmaps_move_library_files($libraries, $from, $to, &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['libraries'] = $libraries;
    $context['sandbox']['max'] = count($libraries);
    $context['finished'] = 0;
  }
  $limit = 100;
  $count = 0;
  $t_args = array();
  while(count($context['sandbox']['libraries']) && $count < $limit) {
    $file = $context['sandbox']['libraries'][key($context['sandbox']['libraries'])];

    list($dir, ) = explode('.', $file->basename, 2);
    $from_dir = $from .'/'. $dir;
    $t_args = array('@dir' => $dir);
    if (is_dir($from_dir) && ($icons = file_scan_directory($from_dir, '.*'))) {
      $to_dir = $to .'/'. $dir;
      if (file_check_directory($to_dir, FILE_CREATE_DIRECTORY)) {
        foreach($icons as $icon) {
          file_move($icon->filename, $to_dir, FILE_EXISTS_REPLACE);
          if (++$count == $limit){
            break 2;
          }
        }
        @rmdir($from_dir);
      }
    }
    $count++;
    $context['sandbox']['progress']++;
    array_shift($context['sandbox']['libraries']);
    file_move($file->filename, $to, FILE_EXISTS_REPLACE);
  }
  $context['message'] = empty($t_args) ? '' : t('Moving library: @dir', $t_args);

  if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  } else {
    $context['finished'] = 1;
    if ((!isset($context['results']) || empty($context['results'])) && is_dir($from)) {
      @rmdir($from);
    }
  }
}

function _gmaps_remove_old_basedir($old, &$context) {
  if (is_dir($old)) {
    @rmdir($old);
  }
  $context['finished'] = 1;
}

function _gmaps_file_moving_finished($success, $results, $operations) {
  if ($success) {
    $message = t('File moving susseccfully finished.');
    if (in_array('_gmaps_move_icon_files', $operations)) {
      $message .= t('Icon database susseccfully updated.');
    }
  }
  else {
    $message = t('File moving finished with an error.');
  }
  drupal_set_message($message);
}

function _gmaps_get_search_engines() {
  $xml = NULL;
  $result = drupal_http_request(GMAPS_USER_AGENT_SOURCE);
  switch ($result->code) {
    case 304:
    case 301:
    case 200:
    case 302:
    case 307:
      $xml = simplexml_load_string($result->data);
      break;
    default:
      $args = array('%error' => $result->code .' '. $result->error);
      watchdog('gmaps user-agents', 'The user-agent.org xml service seems to be broken, because of error "%error".', $args, WATCHDOG_ERROR, GMAPS_USER_AGENT_SOURCE);
  }
  if (!$xml) {
    return NULL;
  }
  
  $robots = array();
  foreach($xml->{'user-agent'} as $ua) {
    //robot but not spam
    $type = (string)$ua->Type;
    if (strpos($type, 'R') !== FALSE && strpos($type, 'S') === FALSE && strpos($type, 'P') === FALSE && strpos($type, 'B') === FALSE) {
      $robots[(string)$ua->ID] = (string)$ua->String;
    }
  }
  
  if (!empty($robots)) {
    cache_set('gmaps_search_engines', $robots, 'cache', time() + variable_get('gmaps_se_cache_lifetime', 2419200));
  }
  
  return $robots;
}
