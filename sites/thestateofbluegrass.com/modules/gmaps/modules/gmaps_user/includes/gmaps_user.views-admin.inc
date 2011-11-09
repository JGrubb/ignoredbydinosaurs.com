<?php
// $Id: gmaps_user.views-admin.inc,v 1.1.2.3 2010/04/01 07:42:10 xmarket Exp $

function _gmaps_user_views_data() {
  $result = array();
  
  $geo = variable_get('gmaps_user_geo_data', array());
  $mo = variable_get('gmaps_user_marker_option', array());
  
  
  if ($geo['type']) {
    switch ($geo['type']) {
      case 'gmaps_address':
        $result = _gmaps_user_views_data_address($geo);
        break;
        
      case 'gmaps_point':
        $result = _gmaps_user_views_data_point($geo);
        break;
        
      case 'gmaps_anp':
        $result = _gmaps_user_views_data_anp($geo);
        break;
        
    }
  }
  
  if ($mo['icon']['enabled'] || $mo['protected_icon']['enabled'] || $mo['static_icon']['enabled'] || $mo['protected_static_icon']['enabled']) {
    $result = gmaps_extend($result, _gmaps_user_views_data_mo($mo));
  }
  
  return $result;
}

function _gmaps_user_views_data_address($geo) {
  static $additional;

  $table = $table_alias = 'gmaps_user_address';
  
  $anp = $geo['type'] == 'gmaps_anp';
  if ($anp) {
    $table_alias .= '_a';
  }
  
  if (!$anp && is_null($additional)) {
    $schema = drupal_get_schema($table);
    $skip = array('uid' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name] && substr($name, 0, 2) != 'u_') {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
    
  $group = $anp ? t('User AnP') : t('User Address');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['users'] = array(
    'table' => $table,
    'left_field' => 'uid',
    'field' => 'uid',
  );
  
  //address only
  if (!$anp) {
    $title = t('Address');
    $data['address'] = array(
      'group' => $group,
      'title' => $title,
    );
    $data['address']['field'] = array(
      'title' => $title,
      'help' => t('Address display.'),
      'table' => $table,
      'field' => 'uid',
      'handler' => 'gmaps_user_handler_field',
      'click sortable' => TRUE,
      'additional fields' => $additional,
    );
    
    $title = t('Map links');
    $data['map_links'] = array(
      'group' => $group,
      'title' => $title,
    );
    $data['map_links']['field'] = array(
      'title' => $title,
      'help' => t('Address based directions from/to and find on links to various map services.'),
      'table' => $table,
      'field' => 'uid',
      'handler' => 'gmaps_user_handler_field_map_links',
      'click sortable' => FALSE,
      'additional fields' => $additional,
    );
    
  }
  
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
        'handler' => 'gmaps_user_handler_field_address_part',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'help' => t('Filter by an individual address part.'),
        'table' => $table,
        'field' => $part,
        'handler' => $part == 'country' ? 'gmaps_user_handler_filter_country' : 'gmaps_user_handler_filter_address_part',
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
      'handler' => 'gmaps_user_handler_field_privacy',
      'click sortable' => TRUE,
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
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
  );

  //proximity by address
  $title = t('Proximity by address');
  $data['proximity'] = array(
    'group' => $group,
    'title' => $title,
    'help' =>  t('This argument requires a user ID and filters users by the addresses of the given user.'),
    'argument' => array(
      'handler' => 'gmaps_user_handler_argument_proximity_address',
      'field' => 'uid',
      'name table' => 'users',
      'name field' => 'name',
      'numeric' => TRUE,
      'validate type' => 'uid',
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
      'handler' => 'gmaps_user_handler_argument_address_parts',
      'field' => 'country',
      'validate type' => 'gmaps user country',
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
        'handler' => 'gmaps_user_handler_argument_address_part_modifier',
        'field' => $part,
        'gmaps_address_part' => $part,
      ),
    );
  }

  //address fast part filter and sort
  $part_title = t('Fast address filter and sort');
  $part_title_short = t('Fast address');
  $data['address_fast'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter or sort on fast address parts.'),
    'filter' => array(
      'handler' => 'gmaps_user_handler_filter_address_fast',
      'allow empty' => TRUE,
      'field' => 'u_locality',
    ),
    'sort' => array(
      'handler' => 'gmaps_user_handler_sort_address',
    ),
  );
  
  return array($table_alias => $data);
}

function _gmaps_user_views_data_point($geo) {
  static $additional;

  $table = $table_alias = 'gmaps_user_point';
  
  $anp = $geo['type'] == 'gmaps_anp';
  if ($anp) {
    $table_alias .= '_p';
  }
  
  if (is_null($additional)) {
    $schema = drupal_get_schema($table);
    $skip = array('uid' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
    
  $group = $anp ? t('User AnP') : t('User Point');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['users'] = array(
    'table' => $table,
    'left_field' => 'uid',
    'field' => 'uid',
  );
  
  //Point field only
  if (!$anp) {
    $title = t('Point');
    $data['point'] = array(
      'group' => $group,
      'title' => $title,
    );
    $data['point']['field'] = array(
      'title' => $title,
      'help' => t('Point display.'),
      'table' => $table,
      'field' => 'uid',
      'handler' => 'gmaps_user_handler_field',
      'click sortable' => FALSE,
      'additional fields' => $additional,
    );

    $title = t('Map links');
    $data['map_links'] = array(
      'group' => $group,
      'title' => $title,
    );
    $data['map_links']['field'] = array(
      'title' => $title,
      'help' => t('Point based directions from/to and find on links to various map services.'),
      'table' => $table,
      'field' => 'uid',
      'handler' => 'gmaps_user_handler_field_map_links',
      'click sortable' => FALSE,
      'additional fields' => $additional,
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
        'handler' => 'gmaps_user_handler_field_point_part',
        'click sortable' => TRUE,
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
      'handler' => 'gmaps_user_handler_field_privacy',
      'click sortable' => TRUE,
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
      'handler' => 'views_handler_field_user',
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
      'handler' => 'gmaps_user_handler_field_map_type',
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
      'field' => 'uid',
      'handler' => 'gmaps_user_handler_field_distance_from',
      'click sortable' => FALSE,
    ),
    'sort' => array(
      'title' => $title,
      'help' => t('Sorts on the distance of the point from the current view location set by a proximity argument handler.'),
      'handler' => 'gmaps_user_handler_sort_distance_from',
      'map_type' => $geo['options']['map_type'],
    ),
  );
  
  if ($geo['options']['map_type'] == GMAPS_MAP_EARTH) {
    //distance from refloc
    $title = t('Distance from reference location');
    $data['refloc'] = array(
      'group' => $group,
      'title' => $title,
      'field' => array(
        'title' => $title,
        'help' => t('Displays the distance of the point from the current reference location.'),
        'table' => $table,
        'field' => 'uid',
        'handler' => 'gmaps_user_handler_field_distance_from',
        'click sortable' => FALSE,
      ),
      'sort' => array(
        'title' => $title,
        'help' => t('Sorts the distance of the point from the current reference location.'),
        'handler' => 'gmaps_user_handler_sort_distance_from',
        'map_type' => $geo['options']['map_type'],
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
          'field' => 'uid',
          'handler' => 'gmaps_user_handler_field_distance_from',
          'click sortable' => FALSE,
        ),
        'sort' => array(
          'title' => $title,
          'help' => t('Sorts on the distance of the point from the current GeoIP location.'),
          'handler' => 'gmaps_user_handler_sort_distance_from',
          'map_type' => $geo['options']['map_type'],
        ),
      );
    }
  }

  //proximity by distance
  $part_title = t('Proximity by distance from user');
  $part_title_short = t('Proximity by distance');
  $data['proximity'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument requires a user ID and filters and/or sorts users by the points of the given user.'),
    'argument' => array(
      'handler' => 'gmaps_user_handler_argument_proximity_distance',
      'field' => 'uid',
      'name table' => 'users',
      'name field' => 'name',
      'numeric' => TRUE,
      'validate type' => 'uid',
    ),
  );

  //location filter
  if (!$anp) {
    $part_title = t('Location filter');
    $part_title_short = t('Location');
    $data['location'] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Filter by bounds and distance.'),
      'filter' => array(
        'handler' => 'gmaps_user_handler_filter_location',
        'allow empty' => TRUE,
        'allow address' => FALSE,
        'map_type' => $geo['options']['map_type'],
      ),
    );
  }
  
  if (module_exists('gmaps_views')) {
    //marker point option
    $group = t('User marker option');
    $part_title = t('Point option');
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
        'field' => 'uid',
        'handler' => 'gmaps_user_handler_field_marker_point',
        'click sortable' => FALSE,
        'additional fields' => $additional,
        'map_type' => $geo['options']['map_type'],
      ),
    );
  }

  return array($table_alias => $data);
}

function _gmaps_user_views_data_anp($geo) {
  static $additional;
  
  $table = $table_alias = 'gmaps_user_point';
  $a_alias = 'gmaps_user_address_a';
  $p_alias = $table_alias .'_p';
  
  if (is_null($additional)) {
    $schema = drupal_get_schema('gmaps_user_address');
    $skip = array('uid' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name] && substr($name, 0, 2) != 'u_') {
        $additional['a_'. $name] = array('table' => $a_alias, 'field' => $name, 'serialize' => $info['serialize']);
      }
    }
    $schema = drupal_get_schema($table);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional['p_'. $name] = array('table' => $p_alias, 'field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
  
  $result = _gmaps_user_views_data_address($geo) + _gmaps_user_views_data_point($geo);
  
  $group = t('User AnP');
  
  $data = array();
  
  $title = t('Address and Point');
  $title_short = 'AnP'; 
  $data['anp'] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
  );
  $data['anp']['field'] = array(
    'title' => $title,
    'title short' => $title_short,
    'help' => t('Address and point display'),
    'table' => $table,
    'field' => 'uid',
    'handler' => 'gmaps_user_handler_field',
    'click sortable' => TRUE,
    'additional fields' => $additional,
    'address_table_alias' => $a_alias,
  );
  
  $title = t('Map links');
  $data['map_links'] = array(
    'group' => $group,
    'title' => $title,
  );
  $data['map_links']['field'] = array(
    'title' => $title,
    'help' => t('Address and point based directions from/to and find on links to various map services.'),
    'table' => $table,
    'field' => 'uid',
    'handler' => 'gmaps_user_handler_field_map_links',
    'click sortable' => FALSE,
    'additional fields' => $additional,
    'address_table_alias' => $a_alias,
  );
  
  //location and address filter
  $part_title = t('Location filter');
  $part_title_short = t('Location');
  $data['location'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter by address, bounds and distance.'),
    'filter' => array(
      'handler' => 'gmaps_user_handler_filter_location',
      'allow empty' => TRUE,
      'allow address' => TRUE,
      'map_type' => $geo['options']['map_type'],
      'address_table_alias' => $a_alias,
    ),
  );
  
  return gmaps_extend($result, array($p_alias => $data));
}

function _gmaps_user_views_data_mo() {
  static $additional;
  $table = 'gmaps_user_marker_option';
  
  if (is_null($additional)) {
    $additional = array();
    $schema = drupal_get_schema($table);
    $skip = array('uid' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
  
  $group = t('User marker option');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['users'] = array(
    'table' => $table,
    'left_field' => 'uid',
    'field' => 'uid',
  );
  
  $part_title = $part_title_short = t('Icon options');
  $data['icon'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Interactive and static marker icon and protected icon options.'),
    'field' => array(
      'table' => $table,
      'field' => 'uid',
      'handler' => 'gmaps_user_handler_field_marker_icon',
      'click sortable' => FALSE,
      'additional fields' => $additional,
    ),
  );
    
  return array($table => $data);
}

