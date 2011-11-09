<?php
// $Id: gmaps_taxonomy.views-admin.inc,v 1.1.2.1 2009/11/23 13:23:48 xmarket Exp $

//TODO: optional term handler overwrites for views

/*function _gmaps_content_field_views_data_address($field) {
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
  $part_title = t('@label (!name) - Address (country) with optional part modifiers', array('@label' => t($field['widget']['label']), '!name' => $field['field_name']));
  $part_title_short = t('@label-truncated - Address parts', array('@label-truncated' => $label_truncated));
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
  
  return array($table_alias => $data);
}*/

function _gmaps_taxonomy_views_data_alter(&$data) {
  /*$data['node']['gmaps_content_address'] = array(
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
  }*/
}
