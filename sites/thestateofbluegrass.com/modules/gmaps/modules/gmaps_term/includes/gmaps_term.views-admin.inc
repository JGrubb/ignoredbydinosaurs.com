<?php
// $Id: gmaps_term.views-admin.inc,v 1.1.2.5 2010/04/01 07:42:10 xmarket Exp $

function _gmaps_term_views_data() {
  $data = _gmaps_term_views_data_address();
  
  foreach(gmaps_map_get_types('names') as $map_type => $map_type_label) {
    $data += _gmaps_term_views_data_point($map_type, $map_type_label);
  }
  
  $data += _gmaps_term_views_data_mo();
  
  return $data;
}

function _gmaps_term_views_data_address() {
  static $additional;

  $table = 'gmaps_term_address';
  
  if (is_null($additional)) {
    $schema = drupal_get_schema($table);
    $skip = array('tid' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name] && substr($name, 0, 2) != 'u_') {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
    
  $group = t('Term Address');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['term_data'] = array(
    'table' => $table,
    'left_field' => 'tid',
    'field' => 'tid',
  );
  
  $title = t('Address');
  $data['address'] = array(
    'group' => $group,
    'title' => $title,
  );
  $data['address']['field'] = array(
    'title' => $title,
    'help' => t('Address display.'),
    'table' => $table,
    'field' => 'tid',
    'handler' => 'gmaps_term_handler_field',
    'click sortable' => TRUE,
    'additional fields' => $additional,
    'gmaps_data_type' => 'gmaps_address',
  );
  
  $title = t('Address map links');
  $data['map_links'] = array(
    'group' => $group,
    'title' => $title,
  );
  $data['map_links']['field'] = array(
    'title' => $title,
    'help' => t('Address based directions from/to and find on links to various map services.'),
    'table' => $table,
    'field' => 'tid',
    'handler' => 'gmaps_term_handler_field_map_links',
    'click sortable' => FALSE,
    'additional fields' => $additional,
    'gmaps_data_type' => 'gmaps_address',
  );

  //address parts
  foreach(gmaps_address_get_part_labels() as $part => $label) {
    $data[$part] = array(
      'group' => $group,
      'title' => t($label),
      'field' => array(
        'title' => t($label),
        'help' => t('Display an individual address part.'),
        'table' => $table,
        'field' => $part,
        'handler' => 'gmaps_term_handler_field_address_part',
        'click sortable' => TRUE,
        'gmaps_address_part' => $part,
      ),
      'filter' => array(
        'help' => t('Filter by an individual address part.'),
        'table' => $table,
        'field' => $part,
        'handler' => $part == 'country' ? 'gmaps_term_handler_filter_country' : 'gmaps_term_handler_filter_address_part',
        'allow empty' => TRUE,
      ),
      'sort' => array(
        'help' => t('Sort by an individual address part.'),
        'table' => $table,
        'field' => $part,
        'handler' => 'views_handler_sort',
      ),
    );
  }
  
  //privacy
  $title = t('Address privacy');
  $data['privacy'] = array(
    'group' => $group,
    'title' => $title,
    'field' => array(
      'title' => $title,
      'help' => t('Displays the privacy value of the address.'),
      'table' => $table,
      'field' => 'privacy',
      'handler' => 'gmaps_term_handler_field_privacy',
      'click sortable' => TRUE,
      'gmaps_data_type' => 'gmaps_address',
    ),
  );

  //search
  $title = t('Address search');
  $data['search'] = array(
    'group' => $group,
    'title' => $title,
    'field' => array(
      'title' => $title,
      'help' => t('Displays the last geocoding query used to find this address.'),
      'table' => $table,
      'field' => 'search',
      'handler' => 'gmaps_term_handler_field_search',
      'click sortable' => TRUE,
    ),
  );

  //proximity by address
  $title = t('Proximity by address');
  $data['proximity'] = array(
    'group' => $group,
    'title' => $title,
    'help' =>  t('This argument accepts a term ID and filters terms by the address of the given term.'),
    'argument' => array(
      'handler' => 'gmaps_term_handler_argument_proximity_address',
      'field' => 'tid',
      'name table' => 'term_data',
      'name field' => 'name',
      'numeric' => TRUE,
      'validate type' => 'tid',
    ),
  );

  //address with modifiers
  $part_title = t('Address with optional part modifiers');
  $part_title_short = t('Address');
  $data['address_country'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument filters by full or fast address and accepts the country part.'),
    'argument' => array(
      'handler' => 'gmaps_term_handler_argument_address_parts',
      'field' => 'country',
      'validate type' => 'gmaps term country',
      'accept part modifier' => TRUE,
    ),
  );
  
  //address part modifiers
  $parts = gmaps_address_get_part_labels();
  unset($parts['country']);
  foreach($parts as $part => $label) {
    $part_title = t('@part address modifier', array('@part' => $label));
    $part_title_short = t('@part modifier', array('@part' => $label));
    $data['address_'. $part] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Modifier for the corresponding address part.'),
      'argument' => array(
        'handler' => 'gmaps_term_handler_argument_address_part_modifier',
        'field' => $part,
        'gmaps_address_part' => $part,
      ),
    );
  }

  //address fast part filter and sort
  $part_title = $part_title_short = t('Fast address');
  $data['address_fast'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter or sort on fast address parts.'),
    'filter' => array(
      'handler' => 'gmaps_term_handler_filter_address_fast',
      'allow empty' => TRUE,
      'field' => 'u_locality',
    ),
    'sort' => array(
      'handler' => 'gmaps_term_handler_sort_address',
    ),
  );
  
  return array($table => $data);
}

function _gmaps_term_views_data_point($map_type, $map_type_label) {
  static $additional, $additional_anp;
  
  $table = 'gmaps_term_point';
  $table_alias = $table .'_'. $map_type;
  $address_table = 'gmaps_term_address';
  
  if (is_null($additional)) {
    $additional = $additional_anp = array();
    $skip = array('tid' => 1);
    $schema = drupal_get_schema($address_table);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name] && substr($name, 0, 2) != 'u_') {
        $additional_anp['a_'. $name] = array('table' => $address_table, 'field' => $name, 'serialize' => $info['serialize']);
      }
    }
    $schema = drupal_get_schema($table);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
        $additional_anp['p_'. $name] = array('table' => $table_alias, 'field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
  
  $group = t('Term Point - !map_type', array('!map_type' => $map_type_label));
  $anp_group = t('Term AnP');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['term_data'] = array(
    'table' => $table,
    'left_field' => 'tid',
    'field' => 'tid',
    'extra' => array(
      array(
        'field' => 'map_type',
        'value' => $map_type,
      ),
    ),
  );
  
  $title = t('Point');
  $data['point'] = array(
    'group' => $group,
    'title' => $title,
  );
  $data['point']['field'] = array(
    'title' => $title,
    'help' => t('Point display.'),
    'table' => $table,
    'field' => 'tid',
    'handler' => 'gmaps_term_handler_field',
    'click sortable' => FALSE,
    'additional fields' => $additional,
    'gmaps_data_type' => 'gmaps_point',
    'gmaps_map_type' => $map_type,
  );

  if ($map_type == GMAPS_MAP_EARTH) {
    $title = t('Address and Point');
    $title_short = 'AnP'; 
    $data['anp'] = array(
      'group' => $anp_group,
      'title' => $title,
      'title short' => $title_short,
    );
    $data['anp']['field'] = array(
      'title' => $title,
      'title short' => $title_short,
      'help' => t('Address and point display'),
      'table' => $table,
      'field' => 'tid',
      'handler' => 'gmaps_term_handler_field',
      'click sortable' => TRUE,
      'additional fields' => $additional_anp,
      'gmaps_data_type' => 'gmaps_anp',
      'gmaps_map_type' => $map_type,
      'gmaps_address_table' => $address_table,
    );
    
    $title = t('Point map links');
    $data['map_links'] = array(
      'group' => $group,
      'title' => $title,
    );
    $data['map_links']['field'] = array(
      'title' => $title,
      'help' => t('Point based directions from/to and find on links to various map services.'),
      'table' => $table,
      'field' => 'tid',
      'handler' => 'gmaps_term_handler_field_map_links',
      'click sortable' => FALSE,
      'additional fields' => $additional,
      'gmaps_data_type' => 'gmaps_point',
    );
    
    $title = t('AnP map links');
    $data['map_links_anp'] = array(
      'group' => $anp_group,
      'title' => $title,
    );
    $data['map_links_anp']['field'] = array(
      'title' => $title,
      'help' => t('Address and point based directions from/to and find on links to various map services.'),
      'table' => $table,
      'field' => 'tid',
      'handler' => 'gmaps_term_handler_field_map_links',
      'click sortable' => FALSE,
      'additional fields' => $additional_anp,
      'gmaps_data_type' => 'gmaps_anp',
      'gmaps_address_table' => $address_table,
    );
  }
  
  //point parts
  foreach(drupal_map_assoc(array('latitude', 'longitude', 'elevation'), 'ucfirst') as $part => $label) {
    $title = t($label);
    $data[$part] = array(
      'group' => $group,
      'title' => $title,
      'field' => array(
        'title' => $title,
        'help' => t('Display an individual point part.'),
        'table' => $table,
        'field' => $part,
        'handler' => 'gmaps_term_handler_field_point_part',
        'click sortable' => TRUE,
        'gmaps_point_part' => $part,
      ),
    );
  }
  
  //privacy
  $title = t('Point privacy');
  $data['privacy'] = array(
    'group' => $group,
    'title' => $title,
    'field' => array(
      'title' => $title,
      'help' => t('Displays the privacy value of the point.'),
      'table' => $table,
      'field' => 'privacy',
      'handler' => 'gmaps_term_handler_field_privacy',
      'click sortable' => TRUE,
      'gmaps_data_type' => 'gmaps_point',
    ),
  );

  //search
  $title = t('Point search');
  $data['search'] = array(
    'group' => $group,
    'title' => $title,
    'field' => array(
      'title' => $title,
      'help' => t('Displays the last geocoding query used to find this point.'),
      'table' => $table,
      'field' => 'search',
      'handler' => 'gmaps_term_handler_field_search',
      'click sortable' => TRUE,
    ),
  );

  //map type
  $title = t('Map type');
  $data['map_type'] = array(
    'group' => $group,
    'title' => $title,
    'field' => array(
      'title' => $title,
      'help' => t('Displays the map type of the point.'),
      'table' => $table,
      'field' => 'map_type',
      'handler' => 'gmaps_term_handler_field_map_type',
      'click sortable' => TRUE,
    ),
  );

  //distance from viewloc
  $title = t('Distance from view location');
  $data['viewloc'] = array(
    'group' => $group,
    'title' => $title,
    'field' => array(
      'title' => $title,
      'help' => t('Displays the distance of the point from the current view location set by a proximity argument handler.'),
      'table' => $table,
      'field' => 'tid',
      'handler' => 'gmaps_term_handler_field_distance_from',
      'click sortable' => FALSE,
      'gmaps_location_type' => 'viewloc',
    ),
    'sort' => array(
      'title' => $title,
      'help' => t('Sorts on the distance of the point from the current view location set by a proximity argument handler.'),
      'handler' => 'gmaps_term_handler_sort_distance_from',
      'gmaps_map_type' => $map_type,
      'gmaps_location_type' => 'viewloc',
    ),
  );
  
  if ($map_type == GMAPS_MAP_EARTH) {
    //distance from refloc
    $title = t('Distance from reference location');
    $data['refloc'] = array(
      'group' => $group,
      'title' => $title,
      'field' => array(
        'title' => $title,
        'help' => t('Displays the distance of the point from the current reference location.'),
        'table' => $table,
        'field' => 'tid',
        'handler' => 'gmaps_term_handler_field_distance_from',
        'click sortable' => FALSE,
        'gmaps_location_type' => 'refloc',
      ),
      'sort' => array(
        'title' => $title,
        'help' => t('Sorts the distance of the point from the current reference location.'),
        'handler' => 'gmaps_term_handler_sort_distance_from',
        'gmaps_map_type' => $map_type,
        'gmaps_location_type' => 'refloc',
      ),
    );
    
    //distance from geoip
    if (module_exists('geoip')) {
      $title = t('Distance from GeoIP location');
      $data['geoip'] = array(
        'group' => $group,
        'title' => $title,
        'field' => array(
          'title' => $title,
          'help' => t('Displays the distance of the point from the current GeoIP location.'),
          'table' => $table,
          'field' => 'tid',
          'handler' => 'gmaps_term_handler_field_distance_from',
          'click sortable' => FALSE,
          'gmaps_location_type' => 'geoip',
        ),
        'sort' => array(
          'title' => $title,
          'help' => t('Sorts on the distance of the point from the current GeoIP location.'),
          'handler' => 'gmaps_term_handler_sort_distance_from',
          'gmaps_map_type' => $map_type,
          'gmaps_location_type' => 'geoip',
        ),
      );
    }
  }

  //proximity by distance
  $part_title = t('Proximity by distance from term');
  $part_title_short = t('Proximity by distance');
  $data['proximity'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument requires a term ID and filters and/or sorts terms by the points of the given term.'),
    'argument' => array(
      'handler' => 'gmaps_term_handler_argument_proximity_distance',
      'field' => 'tid',
      'name table' => 'term_data',
      'name field' => 'name',
      'numeric' => TRUE,
      'validate type' => 'tid',
    ),
  );

  //location filter
  $part_title = t('Location filter');
  $part_title_short = t('Location');
  $data['location'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter by bounds and distance.'),
    'filter' => array(
      'handler' => 'gmaps_term_handler_filter_location',
      'allow empty' => TRUE,
      'allow address' => FALSE,
      'gmaps_map_type' => $map_type,
    ),
  );
  
  if ($map_type == GMAPS_MAP_EARTH) {
    //location and address filter
    $part_title = t('Location filter - Address and Point');
    $part_title_short = t('Location AnP');
    $data['location_anp'] = array(
      'group' => $anp_group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Filter by address, bounds and distance.'),
      'filter' => array(
        'handler' => 'gmaps_term_handler_filter_location',
        'allow empty' => TRUE,
        'allow address' => TRUE,
        'gmaps_map_type' => $map_type,
        'gmaps_address_table' => $address_table,
      ),
    );
  }
  
  if (module_exists('gmaps_views')) {
    //marker point option
    $group = t('Term marker option');
    $part_title = t('Point option - !map_type', array('!map_type' => $map_type_label));
    $part_title_short = t('Point');
    $data['marker_point'] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Interactive and static marker option.'),
      'field' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => 'tid',
        'handler' => 'gmaps_term_handler_field_marker_point',
        'click sortable' => FALSE,
        'additional fields' => $additional,
        'gmaps_map_type' => $map_type,
      ),
    );
  }

  return array($table_alias => $data);
}

function _gmaps_term_views_data_mo() {
  static $additional;
  $table = 'gmaps_term_marker_option';
  
  if (is_null($additional)) {
    $additional = array();
    $schema = drupal_get_schema($table);
    $skip = array('tid' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
  
  $group = t('Term marker option');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['term_data'] = array(
    'table' => $table,
    'left_field' => 'tid',
    'field' => 'tid',
  );
  
  $part_title = t('Icon options');
  $part_title_short = t('Icons');
  $data['marker_icon'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Interactive and static marker icon and protected icon options.'),
    'field' => array(
      'table' => $table,
      'field' => 'tid',
      'handler' => 'gmaps_term_handler_field_marker_icon',
      'click sortable' => FALSE,
      'additional fields' => $additional,
    ),
  );
    
  return array($table => $data);
}

function _gmaps_term_handler_field_options_form($s, $map_type) {
  $countryicons = module_exists('countryicons');
  $languageicons = module_exists('languageicons');
  
  module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
  
  $form = array('#type' => 'fieldset',
    '#title' => t('GMaps options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['formatter'] = array('#type' => 'select',
    '#title' => t('Formatter'),
    '#default_value' => $s['formatter'],
    '#options' => _gmaps_term_geo_data_formatters(),
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
    '#options' => array(0 => t('Disabled')) + gmaps_map_get_options($map_type),
  );
  if ($map_type == GMAPS_MAP_EARTH) {
    $form['map']['static_map'] = array('#type' => 'select',
      '#title' => t('Static map'),
      '#default_value' => $s['map']['static_map'],
      '#options' => array(0 => t('Disabled')) + gmaps_static_map_get_options(),
    );
  }
  else {
    $form['map']['static_map'] = array('#type' => 'value', '#value' => 0);
  }
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

function _gmaps_term_geo_data_formatters() {
  return array(
    'hidden' => t('<Hidden>'),
    'default' => t('Default (User/site render)'),
    'plain' => t('Plain text'),
    'themed' => t('Themed'),
    'map' => t('GMaps Map'),
    'static_map' => t('GMaps Static map'),
  );
}

