<?php
// $Id: gmaps_content.admin.inc,v 1.1.2.8 2010/03/18 10:52:17 xmarket Exp $

/**
 * @file
 * CCK field administration function
 */

function gmaps_content_generate_point(&$node, $field) {
  $gmaps = &$field['gmaps'];
  
  $point = array(
    'latitude' => mt_rand(-85000, 85000) / 1000,
    'longitude' => mt_rand(-175000, 175000) / 1000,
    'elevation' => $gmaps['enable_3D'] ? mt_rand(-90, 8000) : 0,
    'map_type' => $gmaps['map_type'],
  );
  
  if ($gmaps['enable_bounds']) {
    $point['bounds'] = array(
      'northeast' => $point,
      'southwest' => $point,
    );
    
    $point['bounds']['northeast']['latitude'] += 1;
    $point['bounds']['northeast']['longitude'] += 1;
    $point['bounds']['southwest']['latitude'] -= 1;
    $point['bounds']['southwest']['longitude'] -= 1;
    
    $point['bounds']['northeast'] = (object)$point['bounds']['northeast'];
    $point['bounds']['southwest'] = (object)$point['bounds']['southwest'];
  }
  
  if ($gmaps['enable_pov']) {
    $point['pov'] = array(
      'yaw' => mt_rand(0, 360),
      'pitch' => mt_rand(-90, 90),
      'zoom' => mt_rand(0, 2),
    );
  }
  
  if ($gmaps['enable_privacy']) {
    $point['privacy'] = gmaps_content_generate_distance($node, array());
  }
  
  return (object)$point;
}

function _gmaps_content_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      $form = array();
      $form['gmaps'] = array('#type' => 'fieldset',
        '#title' => t('GMaps settings'),
        '#collapsible' => TRUE,
        '#collapsed' => !empty($field['gmaps']),
        '#tree' => TRUE,
      );
      $type = &$field['type'];
      if ($type == 'gmaps_distance') {
        $form['gmaps']['enable_negative'] = array('#type' => 'checkbox',
          '#title' => t('Enable negative values'),
          '#default_value' => $field['gmaps']['enable_negative'],
        );
      }
      else {
        $empty = FALSE;
        if (empty($field['gmaps'])) {
          $empty = TRUE;
          $field['gmaps'] = _gmaps_content_field_default_settings($field);
        }
        if ($type == 'gmaps_address') {
          $form['gmaps'] += _gmaps_content_field_settings_form_address($field, $empty);
        }
        else if ($type == 'gmaps_point') {
          $form['gmaps'] += _gmaps_content_field_settings_form_point($field, $empty);
        }
        else if ($type == 'gmaps_anp') {
          $form['gmaps'] += _gmaps_content_field_settings_form_anp($field, $empty);
        }
      }
      return $form;

    case 'save':
      return array('gmaps');
      break;

    case 'database columns':
      if ($field['type'] == 'gmaps_distance') {
        return array(
          'value' => array(
            'type' => 'numeric',
            'not null' => FALSE,
            'precision' => 20,
            'scale' => 6,
            'sortable' => TRUE,
            'views' => TRUE,
          ),
          'unit' => array(
            'type' => 'varchar',
            'length' => 255,
            'not null' => FALSE,
            'sortable' => TRUE,
            'views' => TRUE,
          ),
        );
      }
      break;

  }
}

function _gmaps_content_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      $form = array();
      if (in_array($widget['type'], array('gmaps_distance', 'gmaps_user_distance', 'gmaps_distancefield'))) {
        $form['gmaps'] = array('#type' => 'fieldset',
          '#title' => t('GMaps settings'),
          '#collapsible' => TRUE,
          '#collapsed' => !empty($widget['gmaps']),
          '#tree' => TRUE,
        );
        
        $form['gmaps'] += _gmaps_content_widget_settings_form_distance($widget);
      }
      else {
        $empty = FALSE;
        if (empty($widget['gmaps'])) {
          $empty = TRUE;
          $widget['gmaps'] = _gmaps_content_widget_default_settings($widget);
        }
        $form['gmaps'] = array('#type' => 'gmaps_accordion',
          '#title' => t('GMaps settings'),
          '#collapsible' => TRUE,
          '#collapsed' => !empty($widget['gmaps']),
          '#tree' => TRUE,
        );
        $form['gmaps'] += _gmaps_content_widget_settings_form($widget, $empty);
      }
      return $form;
    
    case 'validate':
      if (in_array($widget['type'], array('gmaps_point', 'gmaps_anp'))) {
        $expires = $widget['gmaps']['formatter']['map']['cache_expires'];
        if (!empty($expires) && (intval($expires) != $expires || $expires < -1)) {
          form_set_error('gmaps][formatter][map][cache_expires', t('Cache expires value must be an integer greater than or equal to -1.'));
        }
        
        $map_type = ($widget['type'] == 'gmaps_anp') ? GMAPS_MAP_EARTH : $widget['gmaps']['map_type'];
      
        if ($map_type != GMAPS_MAP_EARTH && !empty($widget['gmaps']['formatter']['map']['static_map'])) {
          form_set_error('gmaps][formatter][map][static_map', t('The static map must be disabled for non @earth based fields.', array('@earth' => GMAPS_MAP_EARTH)));
        }
        
        $map = gmaps_map_load($widget['gmaps']['formatter']['map']['map']);
        if (!$map || $map->type != $map_type) {
          form_set_error('gmaps][formatter][map][map', t('The map type of the display map must match the map type of the field.'));
        }
      }
      break;

    case 'save':
      return array('gmaps');
      break;
  }
}

function _gmaps_content_field_default_settings($field) {
  $address = ($field['type'] == 'gmaps_address' || $field['type'] == 'gmaps_anp');
  $point = ($field['type'] == 'gmaps_point' || $field['type'] == 'gmaps_anp');
  
  $gmaps = array();
  if ($address) {
    $gmaps += array(
      'allowed_countries' => array(),
      'enable_privacy' => 0,
    );
  }
  if ($point) {
    $gmaps += array(
      'map_type' => GMAPS_MAP_EARTH,
      'enable_bounds' => 1,
      'enable_3D' => 0,
      'enable_pov' => 0,
      'enable_privacy' => 0,
    );
  }
  
  return $gmaps;
}

function _gmaps_content_widget_default_settings($widget) {
  $address = ($widget['type'] == 'gmaps_content_address' || $widget['type'] == 'gmaps_content_anp');
  $point = ($widget['type'] == 'gmaps_content_point' || $widget['type'] == 'gmaps_content_anp');
  $anp = $address & $point;
  
  $gmaps = array(
    'input' => array(
      'enable_geocoder' => TRUE,
      'use_map' => TRUE,
      'collapsible' => TRUE,
      'collapsed' => -1,
    ),
    'meta' => array(
      'search' => array(),
      'meta' => array(),
    ),
    'formatter' => array(
      'map_links' => array(
        'google_find' => 0,
        'google_directions_from' => 0,
        'google_directions_to' => 0,
        'yahoo_find' => 0,
        'yahoo_directions_from' => 0,
        'yahoo_directions_to' => 0,
        'mapquest_find' => 0,
      ),
    ),
  );
    
  //input
  if (!$address) {
    $gmaps['input']['input_disabled'] = TRUE;
  }
  if ($anp) {
    $gmaps['input']['tabbed_parts'] = -1;
  }
  
  //meta
  $meta = &$gmaps['meta'];
  if ($address) {
    $meta['taxonomy'] = array(
      'location' => TRUE,
      'letter' => TRUE,
      'show' => array(
        'continent' => TRUE,
        'last_part' => FALSE,
        'parts' => array(),
        'empty' => FALSE,
      ),
    );
    foreach (_gmaps_address_get_parts() as $part => $label) {
      $mata['taxonomy']['show']['parts'][$part] = TRUE;
    }
    $meta['search']['address_index'] = TRUE;
    $meta['search']['address_snippet'] = TRUE;
    $meta['meta']['geo_placename'] = TRUE;
    $meta['meta']['geo_region'] = TRUE;
  }
  if ($point) {
    $meta_link = array(
      'content' => array(
        'show' => 'all',
        'address' => 'plain',
        'point' => 'plain',
      ),
      'distance' => array(
        'show' => 'min',
        'refloc' => TRUE,
        'geoip' => TRUE,
        'viewloc' => FALSE,
      )
    );
    $meta['link'] = array(
      'teaser' => $meta_link,
      'full' => $meta_link,
    );
    $meta['search']['point_snippet'] = TRUE;
    $meta['search']['refloc_snippet'] = TRUE;
    $meta['search']['geoip_snippet'] = TRUE;
    $meta['meta']['icbm'] = TRUE;
    $meta['meta']['geo_position'] = TRUE;
    $meta['rss'] = array(
      'w3c' => FALSE,
      'simple' => TRUE,
      'gml' => TRUE,
    );
  }
  
  //formatter
  $formatter = &$gmaps['formatter'];
  if ($anp) {
    $formatter['show'] = 'anp';
  }
  
  if ($address) {
    $formatter['address'] = array(
      'user_settings' => TRUE,
      'country' => 'name',
      'state' => '',
      'flag' => '',
      'taxonomy_links' => 0,
    );
    $formatter['map_links']['mapquest_directions_from'] = 0;
    $formatter['map_links']['mapquest_directions_to'] = 0;
  }
  
  if ($point) {
    $formatter['point'] = array(
      'user_settings' => TRUE,
      'format' => 'dd',
      'elevation' => 1,
      'dd_prec' => 6,
      'dms_prec' => 0,
      'cart_prec' => 2,
      'elevation_prec' => 0,
    );
    $formatter['map'] = array(
      'map' => gmaps_map_get_mid_for_default('field_display_'. GMAPS_MAP_EARTH),
      'static_map' => gmaps_static_map_get_mid_for_default('field_display'),
      'cache' => 1,
      'cache_expires' => 0,
      'content' => 'default',
    );
  }
  
  return $gmaps;
}

function _gmaps_content_field_settings_form_address($field, $empty = FALSE) {
  $gmaps = &$field['gmaps'];
  
  $form = array();
  
  $form['allowed_countries'] = array('#type' => 'select',
    '#title' => t('Allowed countries'),
    '#options' => gmaps_get_countries('names'),
    '#multiple' => TRUE,
    '#size' => 5,
    '#default_value' => $gmaps['allowed_countries'],
    '#description' => t('If none selected all countries will be allowed.'),
  );
  $form['enable_privacy'] = array('#type' => 'checkbox',
    '#title' => t('Enable privacy'),
    '#default_value' => $gmaps['enable_privacy'],
  );
  
  return $form;
}

function _gmaps_content_field_settings_form_point($field, $empty = FALSE) {
  $gmaps = &$field['gmaps'];
  
  if ($field['type'] == 'gmaps_anp') {
    $gmaps['map_type'] = GMAPS_MAP_EARTH;
  }
  $form = array();
  
  $map_types = gmaps_map_get_types('names');
  if ($empty && $field['type'] != 'gmaps_anp') {
    $form['map_type'] = array('#type' => 'select',
      '#title' => t('Map type'),
      '#options' => $map_types,
      '#default_value' => $gmaps['map_type'],
    );
  } else {
    $form['map_type'] = array('#type' => 'value', '#value' => $gmaps['map_type']);
    $form['map_type_display'] = array('#type' => 'item',
      '#title' => t('Map type'),
      '#value' => theme('placeholder', $map_types[$gmaps['map_type']]),
    );
  }
  $form['enable_bounds'] = array('#type' => 'checkbox',
    '#title' => t('Enable bounds'),
    '#default_value' => $gmaps['enable_bounds'],
  );
  $form['enable_3D'] = array('#type' => 'checkbox',
    '#title' => t('Enable elevation'),
    '#default_value' => $gmaps['enable_3D'],
  );
  $form['enable_pov'] = array('#type' => 'checkbox',
    '#title' => t('Enable point of view'),
    '#default_value' => $gmaps['enable_pov'],
  );
  $form['enable_privacy'] = array('#type' => 'checkbox',
    '#title' => t('Enable privacy'),
    '#default_value' => $gmaps['enable_privacy'],
  );
  
  return $form;
}

function _gmaps_content_field_settings_form_anp($field, $empty = FALSE) {
  $form = _gmaps_content_field_settings_form_address($field, $empty);
  unset($form['enable_privacy']);
  return $form + _gmaps_content_field_settings_form_point($field, $empty);
}

function _gmaps_content_widget_settings_form_distance($widget) {
  global $user;
  
  $gmaps = &$widget['gmaps'];
  
  if (empty($gmaps)) {
    $gmaps = $user->gmaps_settings['length'];
    $gmaps['user_settings'] = TRUE;
  }
  
  $form = array();
  
  module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
  
  $form['user_settings'] = array('#type' => 'checkbox',
    '#title' => t('Use user/site settings'),
    '#default_value' => $gmaps['user_settings'],
  );
  $form['unit'] = array('#type' => 'select',
    '#title' => t('Displayed length unit'),
    '#default_value' => is_object($gmaps['unit']) ? $gmaps['unit']->key : $gmaps['unit'],
    '#options' => array('' => t("Display 'as is'")) + _gmaps_length_get_unit_admin_options(),
  );
  $form['optmode'] = array('#type' => 'select',
    '#title' => t('Length optimization method'),
    '#default_value' => $gmaps['optmode'],
    '#options' => _gmaps_length_get_optimization_modes(),
  );
  $form['precision'] = array('#type' => 'select',
    '#title' => t('Precision'),
    '#default_value' => $gmaps['precision'],
    '#options' => _gmaps_length_get_precision_options(),
    '#description' => t('Ammount of decimal digits to display.'),
  );
  $form['multipart'] = array('#type' => 'checkbox',
    '#title' => t('Allow multiple parts'),
    '#default_value' => $gmaps['multipart'],
  );
  
  return $form;
}

function _gmaps_content_widget_settings_form(&$widget, $empty = FALSE) {
  $form = array();
  
  $form += _gmaps_content_widget_settings_form_input($widget, $empty);
  
  $form += _gmaps_content_widget_settings_form_meta($widget, $empty);
  
  $form += _gmaps_content_widget_settings_form_formatter($widget, $empty);
  
  return $form;
}

function _gmaps_content_widget_settings_form_input(&$widget, $empty = FALSE) {
  $gmaps = &$widget['gmaps']['input'];
    
  $address = ($widget['type'] == 'gmaps_content_address');
  $anp = ($widget['type'] == 'gmaps_content_anp');
  
  $form = array();
  
  $form['input'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Input settings'),
    '#id' => 'input'
  );
  
  $form['input']['enable_geocoder'] = array('#type' => 'checkbox',
    '#title' => t('Enable geocoder'),
    '#default_value' => $gmaps['enable_geocoder'],
  );
  $form['input']['use_map'] = array('#type' => 'checkbox',
    '#title' => t('Use input map'),
    '#default_value' => $gmaps['use_map'],
  );
  $form['input']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $gmaps['collapsible'],
  );
  $form['input']['collapsed'] = array('#type' => 'select',
    '#title' => t('Collapsed'),
    '#default_value' => $gmaps['collapsed'],
    '#options' => array(-1 => t('When not empty'), 0 => t('Collapsible'), 1 => t('Collapsed'))
  );
  
  if (!$address) {
    $form['input']['input_disabled'] = array('#type' => 'checkbox',
      '#title' => t('Disable numeric input fields'),
      '#default_value' => $gmaps['input_disabled'],
    );
  }
  
  if ($anp) {
    $form['input']['tabbed_parts'] = array('#type' => 'select',
      '#title' => t('Show parts on tabs'),
      '#default_value' => $gmaps['tabbed_parts'],
      '#options' => array(-1 => t('When not empty'), 0 => t('Never'), 1 => t('Always'))
    );
  }
  
  return $form;
}

function _gmaps_content_widget_settings_form_meta(&$widget, $empty = FALSE) {
  $gmaps = &$widget['gmaps']['meta'];
    
  $address = ($widget['type'] == 'gmaps_content_address' || $widget['type'] == 'gmaps_content_anp');
  $point = ($widget['type'] == 'gmaps_content_point' || $widget['type'] == 'gmaps_content_anp');
  
  $formatters = array(
    '' => t('<Hidden>'),
    'plain' => t('Plain text'),
    'themed' => t('Themed'),
  );

  $form = array();
  
  $form['meta'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Meta settings'),
    '#id' => 'meta'
  );
  
  if ($address) {
    $exists = module_exists('gmaps_taxonomy');
    $form['meta']['taxonomy'] = array('#type' => 'fieldset',
      '#title' => t('Taxonomy terms'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    if (!$exists) {
      $form['meta']['taxonomy']['#description'] = t("If you enable the GMaps Taxonomy module, you can add address related terms to nodes.");
    }
    $form['meta']['taxonomy']['location'] = array('#type' => 'checkbox',
      '#title' => t('Add location terms'),
      '#default_value' => $gmaps['taxonomy']['location'],
      '#access' => $exists,
    );
    $form['meta']['taxonomy']['letter'] = array('#type' => 'checkbox',
      '#title' => t('Add locality first letter terms'),
      '#default_value' => $gmaps['taxonomy']['letter'],
      '#access' => $exists,
    );
    $form['meta']['taxonomy']['show'] = array('#type' => 'fieldset',
      '#title' => t('Show terms'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#access' => $exists,
    );
    $form['meta']['taxonomy']['show']['continent'] = array('#type' => 'checkbox',
      '#title' => t('Show continent term'),
      '#default_value' => $gmaps['taxonomy']['show']['continent'],
    );
    $form['meta']['taxonomy']['show']['last_part'] = array('#type' => 'checkbox',
      '#title' => t('Show only the most accurate location term'),
      '#default_value' => $gmaps['taxonomy']['show']['last_part'],
      '#description' => '<strong>'. t('This option takes precedence over the next one.') .'</strong>',
    );
    $form['meta']['taxonomy']['show']['parts'] = array('#type' => 'checkboxes',
      '#title' => t('Show only the selected location terms'),
      '#default_value' => $gmaps['taxonomy']['show']['parts'],
      '#options' => gmaps_address_get_part_labels(),
      '#description' => '<strong>'. t('Only the selected parts will be displayed.') .'</strong>',
    );
    $form['meta']['taxonomy']['show']['empty'] = array('#type' => 'checkbox',
      '#title' => t('Show location terms for empty parts'),
      '#default_value' => $gmaps['taxonomy']['show']['empty'],
      '#description' => t('Has effect only when terms displayed on a location part basis.'),
    );
  }
  $form['meta']['search'] = array('#type' => 'fieldset',
    '#title' => t('Search support'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($address) {
    $form['meta']['search']['address_index'] = array('#type' => 'checkbox',
      '#title' => t('Add address to search index'),
      '#default_value' => $gmaps['search']['address_index'],
    );
    $form['meta']['search']['address_snippet'] = array('#type' => 'checkbox',
      '#title' => t('Show address in search results'),
      '#default_value' => $gmaps['search']['address_snippet'],
    );
  }
  if ($point) {
    $form['meta']['search']['point_snippet'] = array('#type' => 'checkbox',
      '#title' => t('Show point in search results'),
      '#default_value' => $gmaps['search']['point_snippet'],
    );
    $form['meta']['search']['refloc_snippet'] = array('#type' => 'checkbox',
      '#title' => t('Show distance from reference location in search results'),
      '#default_value' => $gmaps['search']['refloc_snippet'],
    );
    $form['meta']['search']['geoip_snippet'] = array('#type' => 'checkbox',
      '#title' => t("Show distance from the user's GeoIP location in search results"),
      '#default_value' => $gmaps['search']['geoip_snippet'],
    );
  }
  
  $form['meta']['meta'] = array('#type' => 'fieldset',
    '#title' => t('META tags'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($address) {
    $form['meta']['meta']['geo_placename'] = array('#type' => 'checkbox',
      '#title' => t('Set geo.placename META tag'),
      '#default_value' => $gmaps['meta']['geo_placename'],
    );
    $form['meta']['meta']['geo_region'] = array('#type' => 'checkbox',
      '#title' => t('Set geo.region META tag'),
      '#default_value' => $gmaps['meta']['geo_region'],
    );
  }
  if ($point) {
    $form['meta']['meta']['geo_position'] = array('#type' => 'checkbox',
      '#title' => t('Set geo.position META tag'),
      '#default_value' => $gmaps['meta']['geo_position'],
    );
    $form['meta']['meta']['icbm'] = array('#type' => 'checkbox',
      '#title' => t('Set ICBM (Geourl) META tag'),
      '#default_value' => $gmaps['meta']['icbm'],
    );
  }
  
  if ($point) {
    $form['meta']['rss'] = array('#type' => 'fieldset',
      '#title' => t('RSS items'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['meta']['rss']['w3c'] = array('#type' => 'checkbox',
      '#title' => t('Add W3C Geo tag to feeds - DEPRECATED'),
      '#default_value' => $gmaps['rss']['w3c'],
    );
    $form['meta']['rss']['simple'] = array('#type' => 'checkbox',
      '#title' => t('Add GeoRSS-Simple tag to feeds'),
      '#default_value' => $gmaps['rss']['simple'],
    );
    $form['meta']['rss']['gml'] = array('#type' => 'checkbox',
      '#title' => t('Add GeoRSS-GML tag to feeds'),
      '#default_value' => $gmaps['rss']['gml'],
    );
  }

  $form['meta']['link'] = array('#type' => 'fieldset',
    '#title' => t('Node links'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $geoip = module_exists('geoip');
  if (!$geoip) {
    $form['meta']['link']['#description'] = t("Enable the GeoIP module to show distance from the user's GeoIP location.");
  }
  $views = module_exists('views');
  $form['meta']['link']['teaser'] = array('#type' => 'fieldset',
    '#title' => t('Teaser'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['meta']['link']['teaser'] += _gmaps_content_widget_settings_form_meta_link($gmaps['link']['teaser'], $address, $point, $formatters, $geoip, $views, $empty);
  
  $form['meta']['link']['full'] = array('#type' => 'fieldset',
    '#title' => t('Full node'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['meta']['link']['full'] += _gmaps_content_widget_settings_form_meta_link($gmaps['link']['full'], $address, $point, $formatters, $geoip, $views, $empty);
  
  return $form;
}

function _gmaps_content_widget_settings_form_meta_link($gmaps, $address, $point, $formatters, $geoip, $views, $empty = FALSE) {
  $form = array();
  $form['content'] = array();
  $form['content']['show'] = array('#type' => 'select',
    '#title' => t('Show field values'),
    '#default_value' => $gmaps['content']['show'],
    '#options' => array('all' => t('All'), 'first' => t('First'), 'last' => t('Last')) + range(1, 10),
  );
  if ($address) {
    $form['content']['address'] = array('#type' => 'select',
      '#title' => t('Address formatter'),
      '#default_value' => $gmaps['content']['address'],
      '#options' => $formatters,
    );
  }
  if ($point) {
    $form['content']['point'] = array('#type' => 'select',
      '#title' => t('Point formatter'),
      '#default_value' => $gmaps['content']['point'],
      '#options' => $formatters,
    );
    $form['distance'] = array();
    $form['distance']['show'] = array('#type' => 'select',
      '#title' => t('Show distances from values'),
      '#default_value' => $gmaps['distance']['show'],
      '#options' => array('min' => t('Nearest'), 'max' => t('Farthest'), 'all' => t('All'), 'first' => t('First'), 'last' => t('Last')) + range(1, 10),
    );
    $form['distance']['refloc'] = array('#type' => 'checkbox',
      '#title' => t('Show distance from reference location'),
      '#default_value' => $gmaps['distance']['refloc'],
    );
    $form['distance']['viewloc'] = array('#type' => 'checkbox',
      '#title' => t("Show distance from the current view location"),
      '#default_value' => $gmaps['distance']['viewloc'],
      '#access' => $views,
    );
    $form['distance']['geoip'] = array('#type' => 'checkbox',
      '#title' => t("Show distance from the user's GeoIP location"),
      '#default_value' => $gmaps['distance']['geoip'],
      '#access' => $geoip,
    );
  }
  return $form;
}

function _gmaps_content_widget_settings_form_formatter(&$widget, $empty = FALSE) {
  $countryicons = module_exists('countryicons');
  $languageicons = module_exists('languageicons');
  
  module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
  
  $gmaps = &$widget['gmaps']['formatter'];
    
  $address = ($widget['type'] == 'gmaps_content_address' || $widget['type'] == 'gmaps_content_anp');
  $point = ($widget['type'] == 'gmaps_content_point' || $widget['type'] == 'gmaps_content_anp');
  
  $form = array();
  
  $form['formatter'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Formatter settings'),
    '#id' => 'formatter'
  );
  
  if ($address && $point) {
    $form['formatter']['show'] = array('#type' => 'select',
      '#title' => t('Show part'),
      '#default_value' => $gmaps['show'],
      '#options' => array(
        'address' => t('Address'),
        'point' => t('Point'),
        'anp' => t('Address and point'),
        'pna' => t('Point and address'),
      ),
    );
    $empty = FALSE;
  }
  
  if ($address) {
    $form['formatter']['address'] = array('#type' => 'fieldset',
      '#title' => t('Address display'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['formatter']['address']['user_settings'] = array('#type' => 'checkbox',
      '#title' => t('Use user/site settings'),
      '#default_value' => $gmaps['address']['user_settings'],
    );
    $form['formatter']['address']['country'] = array('#type' => 'select',
      '#title' => t('Country display'),
      '#default_value' => $gmaps['address']['country'],
      '#options' => _gmaps_address_get_country_options(),
    );
    $form['formatter']['address']['state'] = array('#type' => 'select',
      '#title' => t('State display'),
      '#default_value' => $gmaps['address']['state'],
      '#options' => _gmaps_address_get_state_options(),
      '#description' => t('Applied to US and Canadian states only.'),
    );
    $form['formatter']['address']['flag'] = array('#type' => 'select',
      '#title' => t('Display country flags'),
      '#default_value' => $gmaps['address']['flag'],
      '#options' => _gmaps_address_get_flag_options(),
      '#access' => ($countryicons || $languageicons),
    );
    $gt = module_exists('gmaps_taxonomy');
    $form['formatter']['address']['taxonomy_links'] = array('#type' => 'checkbox',
      '#title' => t('With taxonomy links'),
      '#default_value' => $gt ? $gmaps['address']['taxonomy_links'] : 0,
      '#access' => $gt,
    );
  }
  
  if ($point) {
    $form['formatter']['point'] = array('#type' => 'fieldset',
      '#title' => t('Point display'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['formatter']['point']['user_settings'] = array('#type' => 'checkbox',
      '#title' => t('Use user/site settings'),
      '#default_value' => $gmaps['point']['user_settings'],
    );
    $form['formatter']['point']['format'] = array('#type' => 'select',
      '#title' => t('Format'),
      '#default_value' => $gmaps['point']['format'],
      '#options' => _gmaps_coordinate_get_format_options(),
      '#description' => t('Cartesian format can fall back to decimal degrees on non-convertable coordinates (sky for example).'),
    );
    $form['formatter']['point']['elevation'] = array('#type' => 'select',
      '#title' => t('Elevation display'),
      '#default_value' => $gmaps['point']['elevation'],
      '#options' => _gmaps_coordinate_get_elevation_options(),
    );
  
    $precisions = _gmaps_length_get_precision_options();
    $form['formatter']['point']['dd_prec'] = array('#type' => 'select',
      '#title' => t('DD precision'),
      '#default_value' => $gmaps['point']['dd_prec'],
      '#options' => $precisions,
      '#description' => t('Ammount of decimal digits to display.'),
    );
    $form['formatter']['point']['dms_prec'] = array('#type' => 'select',
      '#title' => t('Second precision in DMS'),
      '#default_value' => $gmaps['point']['dms_prec'],
      '#options' => $precisions,
      '#description' => t('Ammount of decimal digits to display.'),
    );
    $form['formatter']['point']['cart_prec'] = array('#type' => 'select',
      '#title' => t('CART precision'),
      '#default_value' => $gmaps['point']['cart_prec'],
      '#options' => $precisions,
      '#description' => t('Ammount of decimal digits to display.'),
    );
    $form['formatter']['point']['elevation_prec'] = array('#type' => 'select',
      '#title' => t('Elevation precision'),
      '#default_value' => $gmaps['point']['elevation_prec'],
      '#options' => $precisions,
      '#description' => t('Ammount of decimal digits to display.'),
    );

    $map_type = $widget['gmaps']['map_type'];
    if (empty($map_type)) {
      $map_type = GMAPS_MAP_EARTH;
    }
    $form['formatter']['map'] = array('#type' => 'fieldset',
      '#title' => t('Map display'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['formatter']['map']['map'] = array('#type' => 'select',
      '#title' => t('Map'),
      '#default_value' => $gmaps['map']['map'],
      '#options' => array(0 => t('Disabled')) + gmaps_map_get_options($empty ? NULL : $map_type),
    );
    if ($empty || $map_type == GMAPS_MAP_EARTH) {
      $form['formatter']['map']['static_map'] = array('#type' => 'select',
        '#title' => t('Static map'),
        '#default_value' => $gmaps['map']['static_map'],
        '#options' => array(0 => t('Disabled')) + gmaps_static_map_get_options(),
      );
    }
    else {
      $form['formatter']['map']['static_map'] = array('#type' => 'value', '#value' => 0);
    }
    $form['formatter']['map']['cache_notes'] = array('#type' => 'item',
      '#title' => t('Cache notes'),
      '#value' => t('When privacy enabled for this field or field value is displayed on the map and GMaps user settings are enabled, then cache ids contain both the node id and the user id. In this case, the field can have as many cache records as the result of the nodes * users expression, which can extremly slow down your system. To avoid this problem, you should disable the cache or set the expiration to a small but non-zero value.'),
      '#attributes' => array('class' => 'error'),
    );
    $form['formatter']['map']['cache'] = array('#type' => 'checkbox',
      '#title' => t('Cache maps'),
      '#default_value' => $gmaps['map']['cache'],
    );
    $form['formatter']['map']['cache_expires'] = array('#type' => 'textfield',
      '#title' => t('Cache expires'),
      '#default_value' => $gmaps['map']['cache_expires'],
      '#description' => t('Relative seconds from the time of map rendering. For example 21600 for six hours. Leave it empty or set to <strong>0 (zero) for PERMANENT caching</strong>, or to <strong>-1 for TEMPORARY caching</strong>.'),
    );
    $form['formatter']['map']['content'] = array('#type' => 'select',
      '#title' => t('Content formatter'),
      '#default_value' => $gmaps['map']['content'],
      '#options' => array(
        'hidden' => t('<Hidden>'),
        'default' => t('Default (User/site render)'),
        'plain' => t('Plain text'),
        'themed' => t('Themed'),
      ),
    );
  }
  
  $form['formatter']['map_links'] = array('#type' => 'fieldset',
    '#title' => t('Map links'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['formatter']['map_links']['google_find'] = array('#type' => 'checkbox',
    '#title' => t('Find on Google Maps'),
    '#default_value' => $gmaps['map_links']['google_find'],
  );
  $form['formatter']['map_links']['google_directions_from'] = array('#type' => 'checkbox',
    '#title' => t('Google directions from'),
    '#default_value' => $gmaps['map_links']['google_directions_from'],
  );
  $form['formatter']['map_links']['google_directions_to'] = array('#type' => 'checkbox',
    '#title' => t('Google directions to'),
    '#default_value' => $gmaps['map_links']['google_directions_to'],
  );
  $form['formatter']['map_links']['yahoo_find'] = array('#type' => 'checkbox',
    '#title' => t('Find on Yahoo! Maps'),
    '#default_value' => $gmaps['map_links']['yahoo_find'],
  );
  $form['formatter']['map_links']['yahoo_directions_from'] = array('#type' => 'checkbox',
    '#title' => t('Yahoo! directions from'),
    '#default_value' => $gmaps['map_links']['yahoo_directions_from'],
  );
  $form['formatter']['map_links']['yahoo_directions_to'] = array('#type' => 'checkbox',
    '#title' => t('Yahoo! directions to'),
    '#default_value' => $gmaps['map_links']['yahoo_directions_to'],
  );
  $form['formatter']['map_links']['mapquest_find'] = array('#type' => 'checkbox',
    '#title' => t('Find on Mapquest'),
    '#default_value' => $gmaps['map_links']['mapquest_find'],
  );
  $form['formatter']['map_links']['mapquest_directions_from'] = array('#type' => 'checkbox',
    '#title' => t('Mapquest directions from'),
    '#default_value' => $gmaps['map_links']['mapquest_directions_from'],
    '#access' => $address,
  );
  $form['formatter']['map_links']['mapquest_directions_to'] = array('#type' => 'checkbox',
    '#title' => t('Mapquest directions to'),
    '#default_value' => $gmaps['map_links']['mapquest_directions_to'],
    '#access' => $address,
  );
  
  return $form;
}

function _gmaps_content_content_field_edit_form_submit($form, &$form_state) {
  gmaps_content_field_cache_clear_all('%:'. $form['#field']['field_name'] .':');
}

function _gmaps_content_content_generate(&$node, $field) {
  if ($field['type'] == 'gmaps_distance') {
    return gmaps_generate_distance($field['gmaps']['enable_negative']);
  }
  else if ($field['type'] == 'gmaps_address') {
    return _gmaps_content_content_generate_address($node, $field);
  }
  else if ($field['type'] == 'gmaps_point') {
    return _gmaps_content_content_generate_point($node, $field);
  }
  else if ($field['type'] == 'gmaps_anp') {
    return _gmaps_content_content_generate_anp($node, $field);
  }
}

function _gmaps_content_content_generate_address(&$node, $field) {
  $gmaps = &$field['gmaps'];
  
  if ($adr = gmaps_generate_address($gmaps['enable_privacy'], $gmaps['allowed_countries'])) {
    return array('value' => (object)array('address' => $adr));
  }
}

function _gmaps_content_content_generate_point(&$node, $field) {
  $gmaps = &$field['gmaps'];
  
  if ($point = gmaps_generate_point($gmaps['map_type'], $gmaps['enable_bounds'], $gmaps['enable_pov'], $gmaps['enable_3D'], $gmaps['enable_privacy'])) {
    return array('value' => (object)array('point' => $point));
  }
}

function _gmaps_content_content_generate_anp(&$node, $field) {
  $gmaps = &$field['gmaps'];
  
  if ($anp = gmaps_generate_anp($gmaps['enable_bounds'], $gmaps['enable_pov'], $gmaps['enable_3D'], $gmaps['enable_privacy'], $gmaps['allowed_countries'])) {
    return array('value' => $anp);
  }
}
