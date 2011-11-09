<?php
// $Id: gmaps_taxonomy_handler_argument_location_term.inc,v 1.1.2.2 2010/01/19 15:03:05 xmarket Exp $
/**
 * An argument handler for gmaps location terms.
 *
 * @ingroup views_argument_handlers
 */
class gmaps_taxonomy_handler_argument_location_term extends views_handler_argument {
  function init(&$view, &$options) {
    parent::init($view, $options);
    $this->helper = new gmaps_taxonomy_many_to_one_helper($this);

    // Ensure defaults for these, during summaries and stuff:
    $this->operator = 'or';
    $this->value = array();
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['strict'] = array('default' => FALSE);
    $options['set_breadcrumb'] = array('default' => FALSE);
    $options['add_table'] = array('default' => FALSE);
    $options['require_value'] = array('default' => FALSE);

    gmaps_taxonomy_many_to_one_helper::option_definition($options);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['strict'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use strict filter'),
      '#description' => t('If selected, empty values will match only empty values rather than everything.'),
      '#default_value' => !empty($this->options['strict']),
    );
    $form['set_breadcrumb'] = array(
      '#type' => 'checkbox',
      '#title' => t("Set the breadcrumb for parent locations"),
      '#description' => t('If selected, the breadcrumb trail will include all parent locations, each one linking to this view. Note that this only works for the first location term.'),
      '#default_value' => !empty($this->options['set_breadcrumb']),
    );
    $form['set_breadcrumb'] = array(
      '#type' => 'checkbox',
      '#title' => t("Set the breadcrumb for parent locations"),
      '#description' => t('If selected, the breadcrumb trail will include all parent locations, each one linking to this view. Note that this only works for the first location term.'),
      '#default_value' => !empty($this->options['set_breadcrumb']),
    );
    $form['add_table'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow multiple arguments to work together.'),
      '#description' => t('If selected, multiple instances of this argument can work together, as though multiple terms were supplied to the same argument. This setting is not compatible with the "Reduce duplicates" setting.'),
      '#default_value' => !empty($this->options['add_table']),
    );

    $form['require_value'] = array(
      '#type' => 'checkbox',
      '#title' => t('Do not display items with no value in summary'),
      '#default_value' => !empty($this->options['require_value']),
    );

    $this->helper->options_form($form, $form_state);
  }

  function default_actions($which = NULL) {
    $action = parent::default_actions($which);
    
    $default = array(
      'title' => t('Continent summary'),
      'method' => 'continent_summary',
      'style plugin' => TRUE,
      'breadcrumb' => FALSE,
    );
    
    if ($which) {
      if ($action) {
        return $action;
      }
      else if ($which == 'continent_summary') {
        return $default;
      }
    }
    else {
      $action['continent_summary'] = $default;
      return $action;
    }
  }

  function continent_summary() {
    $this->view->build_info['summary'] = TRUE;
    $this->view->build_info['summary_level'] = $this->options['id'];

    // We return with no query; this will force the empty text.
    $this->view->built = TRUE;
    $this->view->executed = TRUE;

    // Change the display style to the summary style for this
    // argument.
    $this->view->plugin_name = $this->options['style_plugin'];
    $this->view->style_options = $this->options['style_options'];

    // Clear out the normal primary field and whatever else may have
    // been added and let the summary do the work.
    $this->query->clear_fields();
    
    $this->ensure_my_table();
    // Add the field.
    $this->base_alias = $this->query->add_field($this->table_alias, $this->real_field);

    $this->summary_name_field();
    
    $this->count_alias = 'num_records';
        
    $result = array();
    
    foreach(($continents = geonames_continent_codes('short')) as $code => $info) {
      $result[] = (object)array(
        $this->name_alias => 'continent:'. $code,
        $this->count_alias => 0,
        'continent' => $info,
      );
    }
  
    $this->view->result = $result;
    
    return TRUE;
  }

  /**
   * Override ensure_my_table so we can control how this joins in.
   * The operator actually has influence over joining.
   */
  function ensure_my_table() {
    $this->helper->ensure_my_table();
  }

  function query() {
    if (empty($this->argument)) {
      parent::ensure_my_table();
      $this->query->add_where(0, "$this->table_alias.$this->real_field IS NULL");
      return;
    }

    gmaps_taxonomy_break_phrase($this->argument, $this);

    $this->helper->add_filter();
  }

  function title() {
    if (!$this->argument) {
      return !empty($this->definition['empty field name']) ? $this->definition['empty field name'] : t('Uncategorized');
    }

    gmaps_taxonomy_break_phrase($this->argument, $this);

    if (empty($this->value)) {
      return !empty($this->definition['empty field name']) ? $this->definition['empty field name'] : t('Uncategorized');
    }

    if ($this->value === array(-1)) {
      return !empty($this->definition['invalid input']) ? $this->definition['invalid input'] : t('Invalid input');
    }
    
    $glue = ' '. t($this->operator) .' ';

    //return implode($this->operator == 'or' ? ' + ' : ', ', $this->title_query());
    return implode($glue, $this->title_query());
  }

  function summary_query() {
    $field = $this->table . '.' . $this->field;
    $join = $this->get_join();

    if (!empty($this->options['require_value'])) {
      $join->type = 'INNER';
    }

    if (empty($this->options['add_table']) || empty($this->view->many_to_one_tables[$field])) {
      $this->table_alias = $this->query->ensure_table($this->table, $this->relationship, $join);
    }
    else {
      $this->table_alias = $this->helper->summary_join();
    }

    // Add the field.
    $this->base_alias = $this->query->add_field($this->table_alias, $this->real_field);

    $this->summary_name_field();

    return $this->summary_basics();
  }

  function summary_argument($data) {
    $value = $data->{$this->base_alias};
    if (empty($value)) {
      $value = 0;
    }

    return $value;
  }

  function summary_name($data) {
    static $names;
    
    $value = $data->{$this->name_alias};
    if (empty($value)) {
      $value =  !empty($this->definition['empty field name']) ? $this->definition['empty field name'] : t('Uncategorized');
    }
    else {
      if ($data->continent) {
        $value = t($data->continent['name']);
      }
      else {
        if (!isset($names)) {
          $names = gmaps_get_countries('names');
        }
        $value = $names[$value] ? $names[$value] : t('Unknown country');
      }
    }
    
    return check_plain($value);
  }
  
  /**
   * Override for specific title lookups.
   */
  function title_query() {
    $titles = array();
    
    if (isset($this->continent)) {
      $continents = geonames_continent_codes('short');
      if ($info = $continents[$this->continent]) {
          $titles[] = check_plain(t($info['name']));
      }
      if (empty($titles)) {
        $titles = array(t('Unknown continent'));
      }
    }
    else {
      foreach($this->value as $adr) {
        $titles[] = gmaps_address_format_plain($adr, 'name', 'name', TRUE);
      }
      if (empty($titles)) {
        $titles = array(t('Unknown location'));
      }
    }
    
    return $titles;
  }
  
  function set_breadcrumb(&$breadcrumb) {
    if (empty($this->options['set_breadcrumb']) || isset($this->continent) || empty($this->value)) {
      return;
    }
    
    if ($new = _gmaps_taxonomy_get_location_breadcrumbs($this->value[0])) {
      drupal_set_breadcrumb($new);
      $breadcrumb = array();
    }
  }
}

class gmaps_taxonomy_many_to_one_helper {
  function gmaps_taxonomy_many_to_one_helper(&$handler) {
    $this->handler = &$handler;
  }

  function option_definition(&$options) {
    $options['reduce_duplicates'] = array('default' => FALSE);
  }

  function options_form(&$form, &$form_state) {
    $form['reduce_duplicates'] = array(
      '#type' => 'checkbox',
      '#title' => t('Reduce duplicates'),
      '#description' => t('This filter can cause items that have more than one of the selected options to appear as duplicate results. If this filter causes duplicate results to occur, this checkbox can reduce those duplicates; however, the more terms it has to search for, the less performant the query will be, so use this with caution. Shouldn\'t be set on single-value fields, as it may cause values to disappear from display, if used on an incompatible field.'),
      '#default_value' => !empty($this->handler->options['reduce_duplicates']),
    );
  }

  /**
   * Sometimes the handler might want us to use some kind of formula, so give
   * it that option. If it wants us to do this, it must set $helper->formula = TRUE
   * and implement handler->get_formula();
   */
  function get_field() {
    if (!empty($this->formula)) {
      return $this->handler->get_formula();
    }
    else {
      return $this->handler->table_alias . '.' . $this->handler->real_field;
    }
  }

  /**
   * Add a table to the query.
   *
   * This is an advanced concept; not only does it add a new instance of the table,
   * but it follows the relationship path all the way down to the relationship
   * link point and adds *that* as a new relationship and then adds the table to
   * the relationship, if necessary.
   */
  function add_table($join = NULL, $alias = NULL) {
    // This is used for lookups in the many_to_one table.
    $field = $this->handler->table . '.' . $this->handler->field;

    if (empty($join)) {
      $join = $this->get_join();
    }

    // See if there's a chain between us and the base relationship. If so, we need
    // to create a new relationship to use.
    $relationship = $this->handler->relationship;

    // Determine the primary table to seek
    if (empty($this->handler->query->relationships[$relationship])) {
      $base_table = $this->handler->query->base_table;
    }
    else {
      $base_table = $this->handler->query->relationships[$relationship]['base'];
    }

    // Cycle through the joins. This isn't as error-safe as the normal
    // ensure_path logic. Perhaps it should be.
    $r_join = drupal_clone($join);
    while ($r_join->left_table != $base_table) {
      $r_join = views_get_table_join($r_join->left_table, $base_table);
    }
    // If we found that there are tables in between, add the relationship.
    if ($r_join->table != $join->table) {
      $relationship = $this->handler->query->add_relationship($this->handler->table . '_' . $r_join->table, $r_join, $r_join->table, $this->handler->relationship);
    }

    // And now add our table, using the new relationship if one was used.
    $alias = $this->handler->query->add_table($this->handler->table, $relationship, $join, $alias);

    // Store what values are used by this table chain so that other chains can
    // automatically discard those values.
    if (empty($this->handler->view->many_to_one_tables[$field])) {
      $this->handler->view->many_to_one_tables[$field] = $this->handler->value;
    }
    else {
      $this->handler->view->many_to_one_tables[$field] = array_merge($this->handler->view->many_to_one_tables[$field], $this->handler->value);
    }

    return $alias;
  }

  function get_join() {
    return $this->handler->get_join();
  }
  
  function extra_to_string($table, $join) {
    if (!isset($join->extra)) {
      return FALSE;
    }
    else if (!is_array($join->extra)) {
      return $join->extra;
    }
    
    $extras = array();
    foreach ($join->extra as $info) {
      $extra = '';
      // Figure out the table name. Remember, only use aliases provided
      // if at all possible.
      $join_table = '';
      if (!array_key_exists('table', $info)) {
        $join_table = $table . '.';
      }
      elseif (isset($info['table'])) {
        $join_table = $info['table'] . '.';
      }

      // And now deal with the value and the operator.  Set $q to
      // a single-quote for non-numeric values and the
      // empty-string for numeric values, then wrap all values in $q.
      $raw_value = $join->db_safe($info['value']);
      $q = (empty($info['numeric']) ? "'" : '');

      if (is_array($raw_value)) {
        $operator = !empty($info['operator']) ? $info['operator'] : 'IN';
        // Transform from IN() notation to = notation if just one value.
        if (count($raw_value) == 1) {
          $value = $q . array_shift($raw_value) . $q;
          $operator = $operator == 'NOT IN' ? '!=' : '=';
        }
        else {
          $value = "($q" . implode("$q, $q", $raw_value) . "$q)";
        }
      }
      else {
        $operator = !empty($info['operator']) ? $info['operator'] : '=';
        $value = "$q$raw_value$q";
      }
      $extras[] = "$join_table$info[field] $operator $value";
    }

    if ($extras) {
      if (count($extras) == 1) {
        $output = array_shift($extras);
      }
      else {
        $output = '(' . implode(' ' . $join->extra_type . ' ', $extras) . ')';
      }
      
      return $output;
    }
    
    return FALSE;
  }

  /**
   * Provide the proper join for summary queries. This is important in part because
   * it will cooperate with other arguments if possible.
   */
  function summary_join() {
    $field = $this->handler->table . '.' . $this->handler->field;
    $join = $this->get_join();

    // shortcuts
    $options = $this->handler->options;
    $view = &$this->handler->view;
    $query = &$this->handler->query;

    if (!empty($options['require_value'])) {
      $join->type = 'INNER';
    }

    if (empty($options['add_table']) || empty($view->many_to_one_tables[$field])) {
      return $query->ensure_table($this->handler->table, $this->handler->relationship, $join);
    }
    else {
      if (!empty($view->many_to_one_tables[$field])) {
        foreach ($view->many_to_one_tables[$field] as $adr) {
          $result = gmaps_content_address_sql_filter($adr, $this->handler->table, $adr->last_part, $options['strict'], TRUE);
          if (!$result) {continue;};
          list($query, $args) = $result;
          _db_query_callback($args, TRUE);
          $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
          
          if ($extra_join = $this->extra_to_string($this->handler->table, $join)) {
            $query .= ' AND '. $extra_join;
          }
          
          $join->extra = $query;
        }
      }
      return $this->add_table($join);
    }
  }

  /**
   * Override ensure_my_table so we can control how this joins in.
   * The operator actually has influence over joining.
   */
  function ensure_my_table() {
    if (!isset($this->handler->table_alias)) {
      // For 'or' if we're not reducing duplicates, we get the absolute simplest:
      $field = $this->handler->table . '.' . $this->handler->field;
      $options = $this->handler->options;
      if ($this->handler->operator == 'or' && empty($this->handler->options['reduce_duplicates'])) {
        if (empty($this->handler->options['add_table']) && empty($this->handler->view->many_to_one_tables[$field])) {
          // query optimization, INNER joins are slightly faster, so use them
          // when we know we can.
          $join = $this->get_join();
          $join->type = 'INNER';
          $this->handler->table_alias = $this->handler->query->ensure_table($this->handler->table, $this->handler->relationship, $join);
          $this->handler->view->many_to_one_tables[$field] = $this->handler->value;
        }
        else {
          $join = $this->get_join();
          $join->type = 'LEFT';
          if (!empty($this->handler->view->many_to_one_tables[$field])) {
            foreach ($this->handler->view->many_to_one_tables[$field] as $adr) {
              $result = gmaps_content_address_sql_filter($adr, $this->handler->table, $adr->last_part, $options['strict'], TRUE);
              if (!$result) {continue;};
              list($query, $args) = $result;
              _db_query_callback($args, TRUE);
              $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
              
              if ($extra_join = $this->extra_to_string($this->handler->table, $join)) {
                $query .= ' AND '. $extra_join;
              }
              
              $join->extra = $query;
            }
          }

          $this->handler->table_alias = $this->add_table($join);
        }

        return $this->handler->table_alias;
      }

      if ($this->handler->operator != 'not') {
        // If it's an and or an or, we do one join per selected value.
        // Clone the join for each table:
        $this->handler->table_aliases = array();
        foreach ($this->handler->value as $adr) {
          $value = serialize($adr);
          // The table alias needs to be unique to this value across the
          // multiple times the filter or argument is called by the view.
          if (!isset($this->handler->view->many_to_one_aliases[$field][$value])) {
            if (!isset($this->handler->view->many_to_one_count[$this->handler->table])) {
              $this->handler->view->many_to_one_count[$this->handler->table] = 0;
            }
            $this->handler->view->many_to_one_aliases[$field][$value] = $this->handler->table . '_value_' . ($this->handler->view->many_to_one_count[$this->handler->table]++);
          }
          
          $join = $this->get_join();
          if ($this->handler->operator == 'and') {
            $join->type = 'INNER';
          }
          
          $result = gmaps_content_address_sql_filter($adr, $this->handler->view->many_to_one_aliases[$field][$value], $adr->last_part, $options['strict']);
          if (!$result) {continue;};
          list($query, $args) = $result;
          _db_query_callback($args, TRUE);
          $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
          
          if ($extra_join = $this->extra_to_string($this->handler->view->many_to_one_aliases[$field][$value], $join)) {
            $query .= ' AND '. $extra_join;
          }
          
          $join->extra = $query;
          
          $alias = $this->handler->table_aliases[$value] = $this->add_table($join, $this->handler->view->many_to_one_aliases[$field][$value]);

          // and set table_alias to the first of these.
          if (empty($this->handler->table_alias)) {
            $this->handler->table_alias = $alias;
          }
        }
      }
      else {
        // For not, we just do one join. We'll add a where clause during
        // the query phase to ensure that $table.$field IS NULL.
        $join = $this->get_join();
        $join->type = 'LEFT';
        $join->extra = array();
        $join->extra_type = 'OR';
        foreach ($this->handler->value as $adr) {
          $result = gmaps_content_address_sql_filter($adr, $this->handler->table, $adr->last_part, $options['strict']);
          if (!$result) {continue;};
          list($query, $args) = $result;
          _db_query_callback($args, TRUE);
          $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
          
          if ($extra_join = $this->extra_to_string($this->handler->table, $join)) {
            $query .= ' AND '. $extra_join;
          }
          
          $join->extra = $query;
        }

        $this->handler->table_alias = $this->add_table($join);
      }
    }
    return $this->handler->table_alias;
  }

  function add_filter() {
    if (empty($this->handler->value)) {
      return;
    }
    $this->handler->ensure_my_table();

    // Shorten some variables:
    $field = $this->get_field();
    $options = $this->handler->options;
    $operator = $this->handler->operator;
    if (empty($options['group'])) {
      $options['group'] = 0;
    }

    $placeholder = !empty($this->handler->definition['numeric']) ? '%d' : "'%s'";

    if ($operator == 'not') {
      $this->handler->query->add_where($options['group'], "$field IS NULL");
    }
    else if ($operator == 'or' && empty($options['reduce_duplicates'])) {
      $queries = $args = array();
      foreach($this->handler->value as $i => $adr) {
        $result = gmaps_content_address_sql_filter($adr, $this->handler->table_alias, $adr->last_part, $options['strict']);
        if (!$result) {continue;};
        list($query, $query_args) = $result;
        
        $queries[] = '('. $query .')';
        $args = array_merge($args, $query_args);
      }
      if (!empty($queries)) {
        $this->handler->query->add_where($options['group'], implode(' OR ', $queries), $args);
      }
    }
    else {
      $field = $this->handler->real_field;
      $clauses = $args = array();
      foreach ($this->handler->table_aliases as $value => $alias) {
        $adr = unserialize($value);

        $result = gmaps_content_address_sql_filter($adr, $alias, $adr->last_part, $options['strict']);
        if (!$result) {continue;};
        list($query, $query_args) = $result;
        
        $clauses[] = '('. $query .')';
        $args = array_merge($args, $query_args);
      }

      $group = empty($options['group']) ? 0 : $options['group'];

      // implode on either AND or OR.
      $this->handler->query->add_where($group, implode(' ' . strtoupper($operator) . ' ', $clauses), $args);
    }
  }
}

