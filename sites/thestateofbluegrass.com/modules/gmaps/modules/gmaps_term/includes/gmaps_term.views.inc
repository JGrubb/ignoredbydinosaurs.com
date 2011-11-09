<?php
// $Id: gmaps_term.views.inc,v 1.1.2.4 2010/04/01 07:42:10 xmarket Exp $

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_term_views_handlers() {
  $handlers = array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_term') . '/includes/handlers',
    ),
    'handlers' => array(

      // field handlers
      'gmaps_term_handler_field' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_address_part' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_point_part' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_privacy' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_search' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_map_type' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_map_links' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      'gmaps_term_handler_field_distance_from' => array(
        'parent' => 'views_handler_field_taxonomy',
      ),
      
      // argument handlers
      'gmaps_term_handler_argument_proximity_address' => array(
        'parent' => 'views_handler_argument_numeric',
      ),
      'gmaps_term_handler_argument_address_parts' => array(
        'parent' => 'views_handler_argument',
      ),
      'gmaps_term_handler_argument_address_part_modifier' => array(
        'parent' => 'views_handler_argument',
      ),
      'gmaps_term_handler_argument_proximity_distance' => array(
        'parent' => 'views_handler_argument_numeric',
      ),
      
      // filter handlers
      'gmaps_term_handler_filter_country' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
      'gmaps_term_handler_filter_address_part' => array(
        'parent' => 'views_handler_filter_string',
      ),
      'gmaps_term_handler_filter_address_fast' => array(
        'parent' => 'views_handler_filter_string',
      ),
      'gmaps_term_handler_filter_location' => array(
        'parent' => 'views_handler_filter',
      ),
      
      // sort handlers
      'gmaps_term_handler_sort_address' => array(
        'parent' => 'views_handler_sort',
      ),
      'gmaps_term_handler_sort_distance_from' => array(
        'parent' => 'views_handler_sort',
      ),
    ),
  );
  
  if (module_exists('gmaps_views')) {
    $handlers['handlers']['gmaps_term_handler_field_marker_point'] = array(
      'parent' => 'views_handler_field',
    );
    $handlers['handlers']['gmaps_term_handler_field_marker_icon'] = array(
      'parent' => 'views_handler_field',
    );
  }
  
  return $handlers;
}

/**
 * Implementation of hook_views_plugins
 */
function gmaps_term_views_plugins() {
  return array(
    'module' => 'gmaps_term', // This just tells our themes are elsewhere.
    'argument validator' => array(
      'gmaps_term_country' => array(
        'title' => t('GMaps Term country'),
        'handler' => 'gmaps_term_plugin_argument_validate_country',
        'path' => drupal_get_path('module', 'gmaps_term') . '/includes/handlers',
        'type' => 'gmaps term country',
      ),
    ),
  );
}

function _gmaps_term_address_fast_filter_option_definition(&$options) {
  $options['fast_filter_fields'] = array('default' => GMAPS_TERM_FAST_FILTER_LOC);
}

function _gmaps_term_address_fast_filter_options_form(&$form, &$form_state, &$handler) {
  $labels = gmaps_address_get_part_labels();
  
  $t_args = array(
    '!locality' => $labels['locality'],
    '!deplocality' => $labels['deplocality'],
    '!postalcode' => $labels['postalcode'],
    '!thoroughfare' => $labels['thoroughfare'],
  );
  
  $form['fast_filter_fields'] = array(
    '#type' => 'select',
    '#title' => t('Check locality input on these fields'),
    '#default_value' => $handler->options['fast_filter_fields'],
    '#options' => array(
      GMAPS_TERM_FAST_FILTER_LOC => $labels['locality'],
      GMAPS_TERM_FAST_FILTER_NH => $labels['deplocality'],
      GMAPS_TERM_FAST_FILTER_ZIP => $labels['postalcode'],
      GMAPS_TERM_FAST_FILTER_ST => $labels['thoroughfare'],
      
      GMAPS_TERM_FAST_FILTER_LOC_NH => t('!locality and !deplocality', $t_args),
      GMAPS_TERM_FAST_FILTER_LOC_ZIP => t('!locality and !postalcode', $t_args),
      GMAPS_TERM_FAST_FILTER_LOC_ST => t('!locality and !thoroughfare', $t_args),
      GMAPS_TERM_FAST_FILTER_NH_ZIP => t('!deplocality and !postalcode', $t_args),
      GMAPS_TERM_FAST_FILTER_NH_ST => t('!deplocality and !thoroughfare', $t_args),
      GMAPS_TERM_FAST_FILTER_ZIP_ST => t('!postalcode and !thoroughfare', $t_args),
      
      GMAPS_TERM_FAST_FILTER_LOC_NH_ZIP => t('!locality, !deplocality and !postalcode', $t_args),
      GMAPS_TERM_FAST_FILTER_LOC_NH_ST => t('!locality, !deplocality and !thoroughfare', $t_args),
      GMAPS_TERM_FAST_FILTER_LOC_ZIP_ST => t('!locality, !postalcode and !thoroughfare', $t_args),
      GMAPS_TERM_FAST_FILTER_NH_ZIP_ST => t('!deplocality, !postalcode and !thoroughfare', $t_args),
      
      GMAPS_TERM_FAST_FILTER_ALL => t('!locality, !deplocality, !postalcode and !thoroughfare', $t_args),
    ),
    '#description' => t('Multiple fields are check in an OR relation. For example "locality = sg OR postalcode = sg".'),
  );
}

class gmaps_term_address_fast_filter_helper {
  protected $handler;
  
  function gmaps_term_address_fast_filter_helper(&$handler) {
    $this->handler = &$handler;
  }
  
  function __construct(&$handler) {
    $this->handler = &$handler;
  }

  function locality_title() {
    $labels = gmaps_address_get_part_labels();
    $fields = $this->handler->options['fast_filter_fields'];
    
    $t_args = array(
      '!locality' => $labels['locality'],
      '!deplocality' => $labels['deplocality'],
      '!postalcode' => $labels['postalcode'],
      '!thoroughfare' => $labels['thoroughfare'],
    );
    
    switch ($fields) {
      case GMAPS_TERM_FAST_FILTER_LOC:
        return $labels['locality'];
      
      case GMAPS_TERM_FAST_FILTER_NH:
        return $labels['deplocality'];
      
      case GMAPS_TERM_FAST_FILTER_ZIP:
        return $labels['postalcode'];
      
      case GMAPS_TERM_FAST_FILTER_ST:
        return $labels['thoroughfare'];
      
      case GMAPS_TERM_FAST_FILTER_LOC_NH:
        return t('!locality or !deplocality', $t_args);
        
      case GMAPS_TERM_FAST_FILTER_LOC_ZIP:
        return t('!locality or !postalcode', $t_args);
        
      case GMAPS_TERM_FAST_FILTER_LOC_ST:
        return t('!locality or !thoroughfare', $t_args);
        
      case GMAPS_TERM_FAST_FILTER_NH_ZIP:
        return t('!deplocality or !postalcode', $t_args);

      case GMAPS_TERM_FAST_FILTER_NH_ST:
        return t('!deplocality or !thoroughfare', $t_args);

      case GMAPS_TERM_FAST_FILTER_ZIP_ST:
        return t('!postalcode or !thoroughfare', $t_args);

      case GMAPS_TERM_FAST_FILTER_LOC_NH_ZIP:
        return t('!locality, !deplocality or !postalcode', $t_args);
        
      case GMAPS_TERM_FAST_FILTER_LOC_NH_ST:
        return t('!locality, !deplocality or !thoroughfare', $t_args);
        
      case GMAPS_TERM_FAST_FILTER_LOC_ZIP_ST:
        return t('!locality, !postalcode or !thoroughfare', $t_args);
        
      case GMAPS_TERM_FAST_FILTER_NH_ZIP_ST:
        return t('!deplocality, !postalcode or !thoroughfare', $t_args);
      
      default:
        return t('!locality, !deplocality, !postalcode or !thoroughfare', $t_args);
    }
  }
  
  function op_equal($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_equal_sql');
  }

  function op_not_equal($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_not_equal_sql', TRUE);
  }

  function op_contains($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_contains_sql');
  }

  function op_starts($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_starts_sql');
  }

  function op_not_starts($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_not_starts_sql', TRUE);
  }

  function op_ends($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_ends_sql');
  }

  function op_not_ends($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_not_ends_sql', TRUE);
  }

  function op_not($values, $table_alias) {
    $this->_op_common($values, $table_alias, '_op_not_sql', TRUE);
  }

  function op_word($values, $table_alias) {
    $value = strtoupper($values['locality']);
    $filters = $args = $loc_where = $nh_where = $ps_where = $th_where = array();

    $this->_op_equal_region_sql($values, $table_alias, $filters, $args);
    
    if ($value !== '') {
      $words = array();
      preg_match_all('/ (-?)("[^"]+"|[^" ]+)/i', ' ' . $value, $matches, PREG_SET_ORDER);
      foreach ($matches as $match) {
        $phrase = false;
        // Strip off phrase quotes
        if ($match[2]{0} == '"') {
          $match[2] = substr($match[2], 1, -1);
          $phrase = true;
        }
        $word = trim($match[2], ',?!();:-');
        $words = array_merge($words, $phrase ? array($word) : preg_split('/ /', $word, -1, PREG_SPLIT_NO_EMPTY));
      }
  
      if (!empty($words)) {
        $fast_filters = array();
        
        $this->_op_word($table_alias, 'locality', $words, $fast_filters, $args);
        $this->_op_word($table_alias, 'deplocality', $words, $fast_filters, $args);
        $this->_op_word($table_alias, 'postalcode', $words, $fast_filters, $args);
        $this->_op_word($table_alias, 'thoroughfare', $words, $fast_filters, $args);
        
        $filters[] = '('. implode(' OR ', $fast_filters) .')';
      }
    }
    
    $this->handler->query->add_where($this->options['group'], implode(' AND ', $filters), $args);
  }

  function op_empty($values, $table_alias) {
    if ($this->handler->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }

    $db_field = $this->handler->query->table_queue[$this->handler->table]['join']->field;
    $this->handler->query->add_where($this->handler->options['group'], "$table_alias.$db_field $operator");
  }

  function _op_common($values, $table_alias, $sql, $not = FALSE) {
    $filters = $args = array();

    $region_op = $not ? '_op_not_equal_region_sql' : '_op_equal_region_sql';
    $this->$region_op($values, $table_alias, $filters, $args);
    
    if ($values['locality'] !== '') {
      $value = strtoupper($values['locality']);
      $c = $this->handler->options['fast_filter_fields'];
      
      $filter = array();
      if (gmaps_term_fast_filter_check_field('locality', $c)) {
        $filter[] = $this->$sql($table_alias, 'u_locality');
        $args[] = $value;
      }
      if (gmaps_term_fast_filter_check_field('deplocality', $c)) {
        $filter[] = $this->$sql($table_alias, 'u_deplocality');
        $args[] = $value;
      }
      if (gmaps_term_fast_filter_check_field('postalcode', $c)) {
        $filter[] = $this->$sql($table_alias, 'u_postalcode');
        $args[] = $value;
      }
      if (gmaps_term_fast_filter_check_field('thoroughfare', $c)) {
        $filter[] = $this->$sql($table_alias, 'u_thoroughfare');
        $args[] = $value;
      }
      
      $filters[] = '('. implode($not ? ' AND ' : ' OR ', $filter) .')';
    }
    
    $this->handler->query->add_where($this->options['group'], implode($not ? ' OR ' : ' AND ', $filters), $args);
  }
  
  function _op_word($table_alias, $field, $words, &$filters, &$args) {
    $op = $this->handler->operator;
    $c = $this->handler->options['fast_filter_fields'];
    
    if (gmaps_term_fast_filter_check_field($field, $c)) {
      $fast_filters = $fast_args = array();
      foreach ($words as $word) {
        $fast_filters[] = "$table_alias.u_$field LIKE '%%%s%%'";
        $fast_args[] = trim($word, " ,!?");
      }
      
      $filters[] = '(' . implode(($op == 'word') ? ' OR ' : ' AND ', $fast_filters) . ')';
      $args = array_merge($args, $fast_args);
    }
  }
  
  function _op_equal_sql($table_alias, $real_field) {
    return "$table_alias.$real_field = '%s'";
  }
  
  function _op_not_equal_sql($table_alias, $real_field) {
    return "($table_alias.$real_field != '%s' OR $table_alias.$real_field IS NULL)";
  }

  function _op_contains_sql($table_alias, $real_field) {
    return "$table_alias.$real_field LIKE '%%%s%%'";
  }

  function _op_starts_sql($table_alias, $real_field) {
    return "$table_alias.$real_field LIKE '%s%%'";
  }

  function _op_not_starts_sql($table_alias, $real_field) {
    return "$table_alias.$real_field NOT LIKE '%s%%'";
  }

  function _op_ends_sql($table_alias, $real_field) {
    return "$table_alias.$real_field LIKE '%%%s'";
  }

  function _op_not_ends_sql($table_alias, $real_field) {
    return "$table_alias.$real_field NOT LIKE '%%%s'";
  }

  function _op_not_sql($table_alias, $real_field) {
    return "$table_alias.$real_field NOT LIKE '%%%s%%'";
  }

  function _op_equal_region_sql($values, $table_alias, &$filters, &$args) {
    if (!empty($this->handler->options['allow_country'])) {
      $filters[] = "$table_alias.country = '%s'";
      $args[] = $values['country'];
      
      if ($values['country'] == 'US' && !empty($values['state']) && $values['state'] != 'All') {
        $filters[] = "$table_alias.u_adminarea = '%s'";
        $args[] = $values['state'];
      }
      else if ($values['country'] == 'CA' && !empty($values['province']) && $values['province'] != 'All') {
        $filters[] = "$table_alias.u_adminarea = '%s'";
        $args[] = $values['province'];
      }
      else if (!empty($values['adminarea'])) {
        $filters[] = "$table_alias.u_adminarea = '%s'";
        $args[] = strtoupper($values['adminarea']);
      }
    }
  }
  
  function _op_not_equal_region_sql($values, $table_alias, &$filters, &$args) {
    if (!empty($this->handler->options['allow_country'])) {
      $filters[] = "$table_alias.country != '%s'";
      $args[] = $values['country'];
      
      if ($values['country'] == 'US' && !empty($values['state']) && $values['state'] != 'All') {
        $filters[] = "($table_alias.u_adminarea != '%s' OR $table_alias.u_adminarea IS NULL)";
        $args[] = $values['state'];
      }
      else if ($values['country'] == 'CA' && !empty($values['province']) && $values['province'] != 'All') {
        $filters[] = "($table_alias.u_adminarea != '%s' OR $table_alias.u_adminarea IS NULL)";
        $args[] = $values['province'];
      }
      else if (!empty($values['adminarea'])) {
        $filters[] = "($table_alias.u_adminarea != '%s' OR $table_alias.u_adminarea IS NULL)";
        $args[] = strtoupper($values['adminarea']);
      }
    }
  }
  
  function _get_country_values() {
    $names = gmaps_get_countries('names');
    
    $countries = array();
    
    $result = db_query("SELECT DISTINCT(gta.country) FROM {gmaps_term_address} gta");
    while ($c = db_fetch_object($result)) {
      if ($names[$c->country]) {
        $countries[$c->country] = $names[$c->country];
      }
    }
    
    asort($countries);
    
    return $countries;
  }

}
