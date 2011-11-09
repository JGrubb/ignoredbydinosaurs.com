<?php
// $Id: gmaps_user_handler_argument_proximity_distance.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

class gmaps_user_handler_argument_proximity_distance extends views_handler_argument_user_uid {
  function option_definition() {
    $options = parent::option_definition();

    $options['break_phrase'] = array('default' => FALSE);
    $options['not'] = array('default' => FALSE);
    $options['not_arg'] = array('default' => TRUE);

    $options['filter'] = array('default' => 0);
    $options['operator'] = array('default' => '<');
    $options['dist'] = array('default' => array());
    $options['maxdist'] = array('default' => array('value' => GMAPS_MAX_FAST_FILTER_DISTANCE, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT));
    $options['dist_m'] = array('default' => 0);
    $options['maxdist_m'] = array('default' => GMAPS_MAX_FAST_FILTER_DISTANCE);
    $options['rounded'] = array('default' => 0);
    
    $options['sort'] = array('default' => 1);
    $options['order'] = array('default' => 'ASC');
    
    $options['viewloc'] = array('default' => 1);
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $options = $this->options;
    
    $form['break_phrase'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
    $form['not'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
    $form['not_arg'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude the argument user'),
      '#description' => t("If selected, the user entered in the argument will be excluded from the results."),
      '#default_value' => !empty($options['not_arg']),
    );
    
    $form['filter'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter by distance'),
      '#default_value' => !empty($options['filter']),
    );
    $form['operator'] = array(
      '#type' => 'select',
      '#title' => t('Filter operator'),
      '#default_value' => $options['operator'],
      '#options' => array('<=' => t("Is less than or equal to"), '>' => t("Is greater than"), 'between' => t('Is between'), 'not between' => t('Is not between')),
    );
    $form['dist'] = array(
      '#type' => 'gmaps_distance',
      '#title' => t('Filter value'),
      '#default_value' => $options['dist'],
    );
    $form['maxdist'] = array(
      '#type' => 'gmaps_distance',
      '#title' => t('Maximum filter value'),
      '#default_value' => $options['maxdist'],
    );
    $form['rounded'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use rounded filter'),
      '#default_value' => !empty($options['rounded']),
      '#description' => t("A bounds based rectangular filter is always used. If selected, a rounded filter is used, too."),
    );
    
    $form['sort'] = array(
      '#type' => 'checkbox',
      '#title' => t('Sort by distance'),
      '#default_value' => !empty($options['sort']),
    );
    $form['order'] = array(
      '#type' => 'select',
      '#title' => t('Sort order'),
      '#default_value' => $options['order'],
      '#options' => array('ASC' => t('Ascending'), 'DESC' => t('Descending')),
    );
    
    $form['viewloc'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set view location to the first used point during execution.'),
      '#default_value' => !empty($options['viewloc']),
    );
  }

  function options_validate($form, &$form_state) {
    $options = $form_state['values']['options'];
    if (empty($options['filter']) && empty($options['sort'])) {
      form_set_error('options][dist', t('Either filter or sort must be enabled.'));
    }
    if (!empty($options['filter'])) {
      if (empty($options['dist']) || gmaps_length_empty($options['dist'])) {
        form_set_error('options][dist', t('Filter distance must be set, when filter is enabled.'));
      }
      else if (!($dist = gmaps_length_convert($options['dist'], GMAPS_DEFAULT_LENGTH_UNIT))) {
        form_set_error('options][dist', t('Unable to contvert filter distance to the internal unit.'));
      }
      
      if (in_array($options['operator'], array('between', 'not between'))) {
        if (empty($options['maxdist']) || gmaps_length_empty($options['maxdist'])) {
          form_set_error('options][maxdist', t("Maximum distance must be set, when operation is either 'Is between' or 'Is not between'."));
        }
        else if (!($maxdist = gmaps_length_convert($options['maxdist'], GMAPS_DEFAULT_LENGTH_UNIT))) {
          form_set_error('options][maxdist', t('Unable to contvert maximum distance to the internal unit.'));
        }
        else if ($dist <= $maxdist) {
          form_set_error('options][maxdist', t('Maximum distance must be greater than filter distance.'));
        }
      }
    }
  }
  
  function options_submit($form, &$form_state) {
    $options = &$form_state['values']['options'];
    $options['dist_m'] = gmaps_length_empty($options['dist']) ? 0 : gmaps_length_convert($options['dist'], GMAPS_DEFAULT_LENGTH_UNIT);
    $options['maxdist_m'] = gmaps_length_empty($options['maxdist']) ? 0 : gmaps_length_convert($options['maxdist'], GMAPS_DEFAULT_LENGTH_UNIT);
  }
  
  function summary_query() {
    $this->ensure_my_table();
    // Add the field.
    $this->base_alias = $this->query->add_field($this->table_alias, $this->real_field);
    //Skip empty records (LEFT JOIN)
    $this->query->add_where(0, "$this->table_alias.$this->real_field IS NOT NULL");

    $this->summary_name_field();
    return $this->summary_basics();
  }

  function query() {
    $this->ensure_my_table();
    
    $not = ($this->options['operator'] == '>' || $this->options['operator'] == 'not between');
    
    if ($this->argument <= 0) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }

    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $account = user_load(array('uid' => $this->argument));
    if (!$account) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $geo_data = $account->gmaps['geo'];
    if (empty($geo_data) || ($geo['type'] == 'gmaps_anp' && gmaps_anp_empty($geo_data)) || gmaps_point_empty($geo_data->point)) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $filters = $args = array();
    $order = '';
    
    $dist = array('value' => $this->options['dist_m'], 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    $maxdist = array('value' => $this->options['maxdist_m'], 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    
    if ($this->options['filter']) {
      $piece = gmaps_user_point_sql_filter_distance($geo_data->point, $this->options['operator'], $dist, $maxdist, $this->table_alias, !empty($this->options['rounded']));
      if ($piece) {
        list($filter, $filter_args) = $piece;
        $filters[] = '('. $filter .')';
        $args = array_merge($args, $filter_args);
      }
    }
    if ($this->options['sort']) {
      if ($piece = gmaps_user_point_sql_sort($geo_data->point, $this->table_alias)) {
        $order = $piece;
      }
    }
    if ($this->options['viewloc'] && $piece) {
      $viewloc = $geo_data;
    }
    
    if (!empty($filters) || !empty($order)) {
      if (!empty($this->options['not_arg'])) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field != %d", $this->argument);
      }
      if (!empty($filters)) {
        $op = ' OR ';
        $this->query->add_where(0, implode($op, $filters), $args);
      }
      if (!empty($order)) {
        if (empty($filters)) {
          $this->query->add_where(0, "$this->table_alias.map_type = '%s'", $geo_data->point->map_type);
        }
        $this->query->add_orderby(NULL, $order, $this->options['order'], $this->table_alias . '_proximity_order');
      }
      if ($this->options['viewloc'] && $viewloc) {
        $viewloc->label = $account->name;
        $this->view->gmaps_viewloc = $viewloc;
      }
    }
    else if (!$not) {
      $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
    }
  }
  
}

