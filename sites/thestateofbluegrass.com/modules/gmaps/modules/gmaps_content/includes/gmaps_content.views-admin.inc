<?php
// $Id: gmaps_content.views-admin.inc,v 1.1.2.5 2010/04/01 07:42:10 xmarket Exp $

function _gmaps_content_field_views_data($field) {
  switch ($field['type']) {
    case 'gmaps_distance':
      $data = content_views_field_views_data($field);
      if ($data) {
        $table_alias = content_views_tablename($field);
        $defs = $data[$table_alias];
        foreach($defs as $key => $def) {
          if ($key != 'delta' && isset($def['field'])) {
            $data[$table_alias][$key]['field']['handler'] = $field['multiple'] ? 'gmaps_content_handler_field_distance_multiple' : 'gmaps_content_handler_field_distance';
            break;
          }
        }
        return $data;
      }
      return;
      
    case 'gmaps_address':
      return _gmaps_content_field_views_data_address($field);
      
    case 'gmaps_point':
      return _gmaps_content_field_views_data_point($field);
      
    case 'gmaps_anp':
      return _gmaps_content_field_views_data_anp($field);
      
  }
}

function _gmaps_content_field_views_data_address($field) {
  static $additional;
  
  $anp = $field['type'] == 'gmaps_anp';
  $table_alias = content_views_tablename($field);
  if ($anp) {
    $table_alias .= '_a';
  }
  $table = 'gmaps_content_address';
  
  if (!$anp && is_null($additional)) {
    $schema = drupal_get_schema($table);
    $skip = array('vid' => 1, 'nid' => 1, 'field_name' => 1, 'delta' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name] && substr($name, 0, 2) != 'u_') {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
    
  $types = array();
  foreach (content_types() as $type) {
    if (isset($type['fields'][$field['field_name']])) {
      $types[] = $type['name'];
    }
  }
  
  $group = $anp ? t('Content AnP') : t('Content Address');
  
  $field_name = db_escape_string($field['field_name']);
  $delta_sql = $field['multiple'] ? '' : " AND gca.delta = 0";
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['node'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
    'extra' => array(
      array(
        'field' => 'field_name',
        'value' => $field['field_name'],
      ),
    ),
  );
  $data['table']['join']['node_revisions'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
    'extra' => array(
      array(
        'field' => 'field_name',
        'value' => $field['field_name'],
      ),
    ),
  );
  if (!$field['multiple']) {
    $data['table']['join']['node']['extra'][] = array('field' => 'delta', 'value' => 0, 'numeric' => TRUE);
    $data['table']['join']['node_revisions']['extra'][] = array('field' => 'delta', 'value' => 0, 'numeric' => TRUE);
  }
  
  $label_truncated = truncate_utf8(t($field['widget']['label']), 10, TRUE);
  $title = t('@label (!name)', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $title_short = check_plain($label_truncated);
  
  $field_help = ($anp ? t('GMaps Address and Point') : t('GMaps Address')) .' - '. t('Appears in: @types', array('@types' => implode(', ', $types)));
  
  //address only
  if (!$anp) {
    $data[$field['field_name']] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  $field_help,
    );
    $data[$field['field_name']]['field'] = array(
      'title' => $title,
      'title short' => $title_short,
      'table' => $table,
      'field' => 'vid',
      'handler' => $field['multiple'] ? 'gmaps_content_handler_field_multiple' : 'gmaps_content_handler_field',
      'click sortable' => TRUE,
      'additional fields' => $additional,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    );
    
    $part_title = t('@label (!name) - Map links', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - Map links', array('@label-truncated' => $label_truncated));
    $data['map_links'] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  $field_help,
    );
    $data['map_links']['field'] = array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'vid',
      'handler' => 'gmaps_content_handler_field_map_links',
      'click sortable' => FALSE,
      'additional fields' => $additional,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    );
    
    // Expose additional delta column for multiple value fields.
    if ($field['multiple']) {
      $part_title = t('@label (!name) - delta', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
      $part_title_short = t('@label-truncated - delta', array('@label-truncated' => $label_truncated));
  
      $part = 'delta';
      $data[$part] = array(
        'group' => $group,
        'title' => $part_title,
        'title short' => $part_title_short,
        'help' => $field_help,
      );
      $data[$part]['field'] = array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => $part,
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
        'access callback' => 'content_access',
        'access arguments' => array('view', $field),
      );
      $data[$part]['argument'] = array(
        'field' => $part,
        'table' => $table,
        'handler' => 'views_handler_argument_numeric',
        'empty field name' => t('<No value>'),
      );
      $data[$part]['filter'] = array(
        'field' => $part,
        'table' => $table,
        'handler' => 'views_handler_filter_numeric',
        'allow empty' => TRUE,
      );
      $data[$part]['sort'] = array(
        'field' => $part,
        'table' => $table,
        'handler' => 'views_handler_sort',
      );
    }
  }
  
  //address parts
  foreach(gmaps_address_get_part_labels() as $part => $label) {
    $part_title = t('@label (!name) - @column', array('@label' => t($field['widget']['label']), '!name' => $field['field_name'], '@column' => t($label)));
    $part_title_short = t('@label-truncated - @column', array('@label-truncated' => $label_truncated, '@column' => t($label)));
    $data[$part] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  $field_help,
      'field' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => $part,
        'handler' => 'gmaps_content_handler_field_address_part',
        'click sortable' => TRUE,
        'content_field_name' => $field['field_name'],
        'access callback' => 'content_access',
        'access arguments' => array('view', $field),
      ),
      'filter' => array(
        'table' => $table,
        'field' => $part,
        'handler' => $part == 'country' ? 'gmaps_content_handler_filter_country' : 'gmaps_content_handler_filter_address_part',
        'allow empty' => TRUE,
      ),
      'sort' => array(
        'table' => $table,
        'field' => $part,
        'handler' => 'views_handler_sort',
      ),
    );
  }
  
  //privacy
  $part_title = t('@label (!name) - Address privacy', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Address privacy', array('@label-truncated' => $label_truncated));
  $data['privacy'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  $field_help,
    'field' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'privacy',
      'handler' => 'gmaps_content_handler_field_privacy',
      'click sortable' => TRUE,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    ),
  );

  //search
  $part_title = t('@label (!name) - Address search', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Address search', array('@label-truncated' => $label_truncated));
  $data['search'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  $field_help,
    'field' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'search',
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    ),
  );

  //proximity by address
  $part_title = t('@label (!name) - Proximity by address of node', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Proximity by address', array('@label-truncated' => $label_truncated));
  $data['proximity'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument requires a node ID and filters the content by the addresses of the given node.') .' - '. $field_help,
    'argument' => array(
      'handler' => 'gmaps_content_handler_argument_proximity_address',
      'field' => 'nid',
      'name table' => 'node',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
      'content_field_name' => $field['field_name'],
    ),
  );

  //address with modifiers
  $part_title = t('@label (!name) - Address with optional part modifiers', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Address', array('@label-truncated' => $label_truncated));
  $data['address_country'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument filters by full or fast address and accepts the country part.') .' - '. $field_help,
    'argument' => array(
      'handler' => 'gmaps_content_handler_argument_address_parts',
      'field' => 'country',
      'validate type' => 'gmaps country',
      'content_field_name' => $field['field_name'],
      'accept part modifier' => TRUE,
    ),
  );
  
  //address fast part filter and sort
  $part_title = t('@label (!name) - Fast address filter and sort', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Fast address', array('@label-truncated' => $label_truncated));
  $data['address_fast'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter or sort on fast address parts.') .' - '. $field_help,
    'filter' => array(
      'handler' => 'gmaps_content_handler_filter_address_fast',
      'allow empty' => TRUE,
      'field' => 'u_locality',
    ),
    'sort' => array(
      'handler' => 'gmaps_content_handler_sort_address',
    ),
  );
  
  if (module_exists('gmaps_taxonomy')) {
    //location term
    $part_title = t('@label (!name) - Location term', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - Location term', array('@label-truncated' => $label_truncated));
    $data['location_term'] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('This argument accepts one or more location terms.') .' - '. $field_help,
      'argument' => array(
        'handler' => 'gmaps_taxonomy_handler_argument_location_term',
        'field' => 'country',
        'validate type' => 'gmaps location term',
        'content_field_name' => $field['field_name'],
      ),
    );
  }
  
  return array($table_alias => $data);
}

function _gmaps_content_field_views_data_point($field) {
  static $additional;
  
  $anp = $field['type'] == 'gmaps_anp';
  $table_alias = content_views_tablename($field);
  if ($anp) {
    $table_alias .= '_p';
  }
  $table = 'gmaps_content_point';
  
  if (is_null($additional)) {
    $additional = array();
    $schema = drupal_get_schema($table);
    $skip = array('vid' => 1, 'nid' => 1, 'field_name' => 1, 'delta' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
  
  $types = array();
  foreach (content_types() as $type) {
    if (isset($type['fields'][$field['field_name']])) {
      $types[] = $type['name'];
    }
  }
  
  $group = $anp ? t('Content AnP') : t('Content Point');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['node'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
    'extra' => array(
      array(
        'field' => 'field_name',
        'value' => $field['field_name'],
      ),
    ),
  );
  $data['table']['join']['node_revisions'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
    'extra' => array(
      array(
        'field' => 'field_name',
        'value' => $field['field_name'],
      ),
    ),
  );
  if (!$field['multiple']) {
    $data['table']['join']['node']['extra'][] = array('field' => 'delta', 'value' => 0, 'numeric' => TRUE);
    $data['table']['join']['node_revisions']['extra'][] = array('field' => 'delta', 'value' => 0, 'numeric' => TRUE);
  }
  
  $label_truncated = truncate_utf8(t($field['widget']['label']), 10, TRUE);
  $title = t('@label (!name)', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $title_short = check_plain($label_truncated);
  
  $field_help = ($anp ? t('GMaps Address and Point') : t('GMaps Point')) .' - '. t('Appears in: @types', array('@types' => implode(', ', $types)));
  
  //Point field only
  if (!$anp) {
    $data[$field['field_name']] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  $field_help,
    );
    $data[$field['field_name']]['field'] = array(
      'title' => $title,
      'title short' => $title_short,
      'table' => $table,
      'field' => 'vid',
      'handler' => $field['multiple'] ? 'gmaps_content_handler_field_multiple' : 'gmaps_content_handler_field',
      'click sortable' => FALSE,
      'additional fields' => $additional,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    );

    $part_title = t('@label (!name) - Map links', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - Map links', array('@label-truncated' => $label_truncated));
    $data['map_links'] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  $field_help,
    );
    $data['map_links']['field'] = array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'vid',
      'handler' => 'gmaps_content_handler_field_map_links',
      'click sortable' => FALSE,
      'additional fields' => $additional,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    );
    
  }
  // Expose additional delta column for multiple value fields.
  if ($field['multiple']) {
    $part_title = t('@label (!name) - delta', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - delta', array('@label-truncated' => $label_truncated));

    $part = 'delta';
    $data[$part] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' => $field_help,
      'field' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => $part,
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
        'access callback' => 'content_access',
        'access arguments' => array('view', $field),
      ),
      'argument' => array(
        'field' => $part,
        'table' => $table,
        'handler' => 'views_handler_argument_numeric',
        'additional fields' => $additional,
        'empty field name' => t('<No value>'),
      ),
      'filter' => array(
        'field' => $part,
        'table' => $table,
        'handler' => 'views_handler_filter_numeric',
        'additional fields' => $additional,
        'allow empty' => TRUE,
      ),
      'sort' => array(
        'field' => $part,
        'table' => $table,
        'title' => $part_title,
        'handler' => 'views_handler_sort',
        'additional fields' => $additional,
      ),
    );
  }
  
  //point parts
  foreach(drupal_map_assoc(array('latitude', 'longitude', 'elevation'), 'ucfirst') as $part => $label) {
    $part_title = t('@label (!name) - @column', array('@label' => t($field['widget']['label']), '!name' => $field['field_name'], '@column' => t($label)));
    $part_title_short = t('@label-truncated - @column', array('@label-truncated' => $label_truncated, '@column' => t($label)));
    $data[$part] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  $field_help,
      'field' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => $part,
        'handler' => 'gmaps_content_handler_field_point_part',
        'click sortable' => TRUE,
        'content_field_name' => $field['field_name'],
        'access callback' => 'content_access',
        'access arguments' => array('view', $field),
      ),
    );
  }
  
  //privacy
  $part_title = t('@label (!name) - Point privacy', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Point privacy', array('@label-truncated' => $label_truncated));
  $data['privacy'] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
    'help' =>  $field_help,
    'field' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'privacy',
      'handler' => 'gmaps_content_handler_field_privacy',
      'click sortable' => TRUE,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    ),
  );

  //search
  $part_title = t('@label (!name) - Point search', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Point search', array('@label-truncated' => $label_truncated));
  $data['search'] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
    'help' =>  $field_help,
    'field' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'search',
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    ),
  );

  //map type
  $part_title = t('@label (!name) - Map type', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Map type', array('@label-truncated' => $label_truncated));
  $data['map_type'] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
    'help' =>  $field_help,
    'field' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'map_type',
      'handler' => 'gmaps_content_handler_field_map_type',
      'click sortable' => TRUE,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    ),
  );

  //distance from viewloc
  $part_title = t('@label (!name) - Distance from view location', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Distance from view location', array('@label-truncated' => $label_truncated));
  $data['viewloc'] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
    'help' =>  $field_help,
    'field' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'table' => $table,
      'field' => 'vid',
      'handler' => $field['multiple'] ? 'gmaps_content_handler_field_distance_from_multiple' : 'gmaps_content_handler_field_distance_from',
      'click sortable' => FALSE,
      'content_field_name' => $field['field_name'],
      'access callback' => 'content_access',
      'access arguments' => array('view', $field),
    ),
    'sort' => array(
      'title' => $part_title,
      'title short' => $part_title_short,
      'handler' => 'gmaps_content_handler_sort_distance_from',
      'map_type' => $field['gmaps']['map_type'],
    ),
  );
  
  if ($field['gmaps']['map_type'] == GMAPS_MAP_EARTH) {
    //distance from refloc
    $part_title = t('@label (!name) - Distance from reference location', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - Distance from reference location', array('@label-truncated' => $label_truncated));
    $data['refloc'] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  $field_help,
      'field' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => 'vid',
        'handler' => $field['multiple'] ? 'gmaps_content_handler_field_distance_from_multiple' : 'gmaps_content_handler_field_distance_from',
        'click sortable' => FALSE,
        'content_field_name' => $field['field_name'],
        'access callback' => 'content_access',
        'access arguments' => array('view', $field),
      ),
      'sort' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'handler' => 'gmaps_content_handler_sort_distance_from',
        'map_type' => $field['gmaps']['map_type'],
      ),
    );
    
    //distance from geoip
    if (module_exists('geoip')) {
      $part_title = t('@label (!name) - Distance from GeoIP location', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
      $part_title_short = t('@label-truncated - Distance from GeoIP location', array('@label-truncated' => $label_truncated));
      $data['geoip'] = array(
        'group' => $group,
        'title' => $title,
        'title short' => $title_short,
        'help' =>  $field_help,
        'field' => array(
          'title' => $part_title,
          'title short' => $part_title_short,
          'table' => $table,
          'field' => 'vid',
          'handler' => $field['multiple'] ? 'gmaps_content_handler_field_distance_from_multiple' : 'gmaps_content_handler_field_distance_from',
          'click sortable' => FALSE,
          'content_field_name' => $field['field_name'],
          'access callback' => 'content_access',
          'access arguments' => array('view', $field),
        ),
        'sort' => array(
          'title' => $part_title,
          'title short' => $part_title_short,
          'handler' => 'gmaps_content_handler_sort_distance_from',
          'map_type' => $field['gmaps']['map_type'],
        ),
      );
    }
  }

  //proximity by distance
  $part_title = t('@label (!name) - Proximity by distance from node', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Proximity by distance', array('@label-truncated' => $label_truncated));
  $data['proximity'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument requires a node ID and filters and/or sorts the content by the points of the given node.') .' - '. $field_help,
    'argument' => array(
      'handler' => 'gmaps_content_handler_argument_proximity_distance',
      'field' => 'nid',
      'name table' => 'node',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
      'content_field_name' => $field['field_name'],
    ),
  );

  //location filter
  if (!$anp) {
    $part_title = t('@label (!name) - Location filter', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - Location', array('@label-truncated' => $label_truncated));
    $data['location'] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Filter by bounds and distance.') .' - '. $field_help,
      'filter' => array(
        'handler' => 'gmaps_content_handler_filter_location',
        'allow empty' => TRUE,
        'content_field_name' => $field['field_name'],
        'allow address' => FALSE,
        'map_type' => $field['gmaps']['map_type'],
      ),
    );
  }
  
  if (module_exists('gmaps_views')) {
    //marker point option
    $group = t('Marker option');
    $part_title = t('@label (!name) - Point option', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
    $part_title_short = t('@label-truncated - Point', array('@label-truncated' => $label_truncated));
    $data['marker_point'] = array(
      'group' => $group,
      'title' => $title,
      'title short' => $title_short,
      'help' =>  t('Interactive and static marker option.') .' - '. $field_help,
      'field' => array(
        'title' => $part_title,
        'title short' => $part_title_short,
        'table' => $table,
        'field' => 'vid',
        'handler' => 'gmaps_content_handler_field_marker_point',
        'click sortable' => FALSE,
        'content_field_name' => $field['field_name'],
        'additional fields' => $additional,
        'map_type' => $field['gmaps']['map_type'],
      ),
    );
  }

  return array($table_alias => $data);
}

function _gmaps_content_field_views_data_anp($field) {
  static $additional;
  
  $table_alias = content_views_tablename($field);
  $a_alias = $table_alias .'_a';
  $p_alias = $table_alias .'_p';
  $table = 'gmaps_content_point';
  
  if (is_null($additional)) {
    $schema = drupal_get_schema('gmaps_content_address');
    $skip = array('vid' => 1, 'nid' => 1, 'field_name' => 1, 'delta' => 1);
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
  
  $result = _gmaps_content_field_views_data_address($field) + _gmaps_content_field_views_data_point($field);
  
  $types = array();
  foreach (content_types() as $type) {
    if (isset($type['fields'][$field['field_name']])) {
      $types[] = $type['name'];
    }
  }
  
  $group = t('Content AnP');
  
  $data = array();
  
  $label_truncated = truncate_utf8(t($field['widget']['label']), 10, TRUE);
  $title = t('@label (!name)', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $title_short = check_plain($label_truncated);
  
  $field_help = t('GMaps Address and Point') .' - '. t('Appears in: @types', array('@types' => implode(', ', $types)));
  
  $data[$field['field_name']] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
    'help' =>  $field_help,
  );
  $data[$field['field_name']]['field'] = array(
    'title' => $title,
    'title short' => $title_short,
    'table' => $table,
    'field' => 'vid',
    'handler' => $field['multiple'] ? 'gmaps_content_handler_field_multiple' : 'gmaps_content_handler_field',
    'click sortable' => TRUE,
    'additional fields' => $additional,
    'address_table_alias' => $a_alias,
    'content_field_name' => $field['field_name'],
    'access callback' => 'content_access',
    'access arguments' => array('view', $field),
  );
  
  $part_title = t('@label (!name) - Map links', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Map links', array('@label-truncated' => $label_truncated));
  $data['map_links'] = array(
    'group' => $group,
    'title' => $title,
    'title short' => $title_short,
    'help' =>  $field_help,
  );
  $data['map_links']['field'] = array(
    'title' => $part_title,
    'title short' => $part_title_short,
    'table' => $table,
    'field' => 'vid',
    'handler' => 'gmaps_content_handler_field_map_links',
    'click sortable' => FALSE,
    'additional fields' => $additional,
    'address_table_alias' => $a_alias,
    'content_field_name' => $field['field_name'],
    'access callback' => 'content_access',
    'access arguments' => array('view', $field),
  );
  
  //location and address filter
  $part_title = t('@label (!name) - Location filter', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Location', array('@label-truncated' => $label_truncated));
  $data['location'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter by address, bounds and distance.') .' - '. $field_help,
    'filter' => array(
      'handler' => 'gmaps_content_handler_filter_location',
      'allow empty' => TRUE,
      'allow address' => TRUE,
      'map_type' => $field['gmaps']['map_type'],
      'address_table_alias' => $a_alias,
      'content_field_name' => $field['field_name'],
    ),
  );
  
  return gmaps_extend($result, array($p_alias => $data));
}

function _gmaps_content_views_data() {
  return _gmaps_content_views_data_address() + _gmaps_content_views_data_point();
}

function _gmaps_content_views_data_address() {
  $table = 'gmaps_content_address';
  
  $group = t('Content Address');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['node'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
  );
  $data['table']['join']['node_revisions'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
  );
  
  //proximity by address
  $part_title = t('Proximity by address of node');
  $part_title_short = t('Proximity by address');
  $data['proximity'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument requires a node ID and filters the content by the addresses of the given node.'),
    'argument' => array(
      'handler' => 'gmaps_content_handler_argument_proximity_address',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
  );

  //address with modifiers
  $part_title = t('Address (country) with optional part modifiers');
  $part_title_short = t('Address parts');
  $data['address_country'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument filters by full or fast address and accepts the country part.'),
    'argument' => array(
      'handler' => 'gmaps_content_handler_argument_address_parts',
      'field' => 'country',
      'validate type' => 'gmaps country',
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
        'handler' => 'gmaps_content_handler_argument_address_part_modifier',
        'field' => $part,
        'gmaps_address_part' => $part,
      ),
    );
  }

  //address parts
  foreach(gmaps_address_get_part_labels() as $part => $label) {
    $part_title = t('@part', array('@part' => $label));
    $part_title_short = t('@part', array('@part' => $label));
    $data[$part] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Filter or sort on the corresponding address part.'),
      'filter' => array(
        'table' => $table,
        'field' => $part,
        'handler' => $part == 'country' ? 'gmaps_content_handler_filter_country' : 'gmaps_content_handler_filter_address_part',
        'allow empty' => TRUE,
      ),
      'sort' => array(
        'table' => $table,
        'field' => $part,
        'handler' => 'views_handler_sort',
      ),
    );
  }
  
  //address fast part modifier
  $part_title = t('Fast address filter and sort');
  $part_title_short = t('Fast address');
  $data['address_fast'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Filter or sort on fast address parts.'),
    'filter' => array(
      'handler' => 'gmaps_content_handler_filter_address_fast',
      'allow empty' => TRUE,
      'field' => 'u_locality',
    ),
    'sort' => array(
      'handler' => 'gmaps_content_handler_sort_address',
    ),
  );
  
  if (module_exists('gmaps_taxonomy')) {
    //location term
    $part_title = t('Location term');
    $part_title_short = t('Location term');
    $data['location_term'] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('This argument accepts one or more location terms.'),
      'argument' => array(
        'handler' => 'gmaps_taxonomy_handler_argument_location_term',
        'field' => 'country',
        'validate type' => 'gmaps location term',
      ),
    );
  }
  
  return array($table => $data);
}

function _gmaps_content_views_data_point() {
  static $additional;
  $table = 'gmaps_content_point';
  
  if (is_null($additional)) {
    $additional = array();
    $schema = drupal_get_schema($table);
    $skip = array('vid' => 1, 'nid' => 1, 'field_name' => 1, 'delta' => 1);
    foreach($schema['fields'] as $name => $info) {
      if (!$skip[$name]) {
        $additional[$name] = array('field' => $name, 'serialize' => $info['serialize']);
      }
    }
  }
  
  $group = t('Content Point');
  
  $data = array();
  $data['table']['group']  = $group;
  $data['table']['join']['node'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
  );
  $data['table']['join']['node_revisions'] = array(
    'table' => $table,
    'left_field' => 'vid',
    'field' => 'vid',
  );
  
  //proximity by distance
  $part_title = t('Proximity by distance from node');
  $part_title_short = t('Proximity by distance');
  $data['proximity'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('This argument requires a node ID and filters and/or sorts the content by the points of the given node.'),
    'argument' => array(
      'handler' => 'gmaps_content_handler_argument_proximity_distance',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
  );

  //location and address filter
  foreach(gmaps_map_get_types('names') as $map_type => $map_type_label) {
    $part_title = t('Location filter - !map_type', array('!map_type' => $map_type_label));
    $part_title_short = t('Location');
    $data['location_'. $map_type] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  $map_type == GMAPS_MAP_EARTH ? t('Filter by address, bounds and distance.') : t('Filter by bounds and distance.'),
      'filter' => array(
        'handler' => 'gmaps_content_handler_filter_location',
        'allow empty' => TRUE,
        'allow address' => ($map_type == GMAPS_MAP_EARTH),
        'map_type' => $map_type,
      ),
    );
    if ($map_type == GMAPS_MAP_EARTH) {
      $data['location_'. $map_type]['filter']['address_table_alias'] = 'gmaps_content_address';
    }

    //distance from viewloc
    $part_title = t('Distance from view location - !map_type', array('!map_type' => $map_type_label));
    $part_title_short = t('Distance from view location');
    $data['viewloc_'. $map_type] = array(
      'group' => $group,
      'title' => $part_title,
      'title short' => $part_title_short,
      'help' =>  t('Distance from view location'),
      'field' => array(
        'table' => $table,
        'field' => 'vid',
        'handler' => 'gmaps_content_handler_field_distance_from_independent',
        'click sortable' => FALSE,
        'map_type' => $map_type,
      ),
      'sort' => array(
        'handler' => 'gmaps_content_handler_sort_distance_from',
        'map_type' => $map_type,
      ),
    );
  
    if ($map_type == GMAPS_MAP_EARTH) {
      //distance from refloc
      $part_title = t('Distance from reference location');
      $part_title_short = t('Distance from reference location');
      $data['refloc'] = array(
        'group' => $group,
        'title' => $part_title,
        'title short' => $part_title_short,
        'help' =>  t('Distance from reference location'),
        'field' => array(
          'table' => $table,
          'field' => 'vid',
          'handler' => 'gmaps_content_handler_field_distance_from_independent',
          'click sortable' => FALSE,
          'map_type' => $map_type,
        ),
        'sort' => array(
          'handler' => 'gmaps_content_handler_sort_distance_from',
          'map_type' => $map_type,
        ),
      );
      
      //distance from geoip
      if (module_exists('geoip')) {
        $part_title = t('Distance from GeoIP location');
        $part_title_short = t('Distance from GeoIP location');
        $data['geoip'] = array(
          'group' => $group,
          'title' => $part_title,
          'title short' => $part_title_short,
          'help' =>  t('Distance from GeoIP location'),
          'field' => array(
            'table' => $table,
            'field' => 'vid',
            'handler' => 'gmaps_content_handler_field_distance_from_independent',
            'click sortable' => FALSE,
            'map_type' => $map_type,
          ),
          'sort' => array(
            'handler' => 'gmaps_content_handler_sort_distance_from',
            'map_type' => $map_type,
          ),
        );
      }
    }
    
    if (module_exists('gmaps_views')) {
      //marker point option
      $part_title = t('Point option - !map_type', array('!map_type' => $map_type_label));
      $part_title_short = t('Point');
      $data['marker_point_'. $map_type] = array(
        'group' => t('Marker option'),
        'title' => $part_title,
        'title short' => $part_title_short,
        'help' => t('Interactive and static marker option.'),
        'field' => array(
          'table' => $table,
          'field' => 'vid',
          'handler' => 'gmaps_content_handler_field_marker_point',
          'click sortable' => FALSE,
          'additional fields' => $additional,
          'map_type' => $map_type,
        ),
      );
    }
  }
  
  return array($table => $data);
}

function _gmaps_content_views_data_alter(&$data) {
  $data['node']['gmaps_content_address'] = array(
    'group' => t('Content Address'),
    'title' => t('Addresses'),
    'help' => t('This relationship will cause duplicated records if there are multiple fields or multiple values per field.'),
    'relationship' => array(
      //'handler' => 'views_handler_relationship_node_term_data',
      'label' => t('address'),
      'base' => 'gmaps_content_address',
      'base field' => 'vid',
      'relationship field' => 'vid',
    ),
  );
  $data['node']['gmaps_content_point'] = array(
    'group' => t('Content Point'),
    'title' => t('Points'),
    'help' => t('This relationship will cause duplicated records if there are multiple fields or multiple values per field.'),
    'relationship' => array(
      //'handler' => 'views_handler_relationship_node_term_data',
      'label' => t('point'),
      'base' => 'gmaps_content_point',
      'base field' => 'vid',
      'relationship field' => 'vid',
    ),
  );
  
  foreach(content_fields() as $field_name => $field) {
    if (in_array($field['type'], array('gmaps_address', 'gmaps_point', 'gmaps_anp'))) {
      $a = $field['type'] == 'gmaps_address';
      $p = $field['type'] == 'gmaps_point';
      $anp = $field['type'] == 'gmaps_anp';
      $table_alias = content_views_tablename($field);
      
      if ($a || $anp) {
        $data['node'][$table_alias .'_a'] = array(
          'group' => $anp ? t('Content AnP') : t('Content Address'),
          'title' => t('@label (!name) - Addresses', array('@label' => t($field['widget']['label']), '!name' => $field['field_name'])),
          'help' => t('This relationship will cause duplicated records if there are multiple values.'),
          'relationship' => array(
            'label' => t('address'),
            'base' => 'gmaps_content_address',
            'base field' => 'vid',
            'relationship field' => 'vid',
            'extra' => array(
              array(
                'field' => 'field_name',
                'value' => $field['field_name'],
              ),
            ),
          ),
        );
        if (!$field['multiple']) {
          $data['node'][$table_alias .'_a']['relationship']['extra'][] = array('field' => 'delta', 'value' => 0, 'numeric' => TRUE);
        }
      }
      if ($p || $anp) {
        $data['node'][$table_alias .'_p'] = array(
          'group' => $anp ? t('Content AnP') : t('Content Point'),
          'title' => t('@label (!name) - Points', array('@label' => t($field['widget']['label']), '!name' => $field['field_name'])),
          'help' => t('This relationship will cause duplicated records if there are multiple values.'),
          'relationship' => array(
            'label' => t('point'),
            'base' => 'gmaps_content_point',
            'base field' => 'vid',
            'relationship field' => 'vid',
            'extra' => array(
              array(
                'field' => 'field_name',
                'value' => $field['field_name'],
              ),
            ),
          ),
        );
        if (!$field['multiple']) {
          $data['node'][$table_alias .'_p']['relationship']['extra'][] = array('field' => 'delta', 'value' => 0, 'numeric' => TRUE);
        }
      }
    }
  }
}
