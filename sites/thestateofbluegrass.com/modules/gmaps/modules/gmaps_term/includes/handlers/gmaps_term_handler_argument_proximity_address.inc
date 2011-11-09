<?php
// $Id: gmaps_term_handler_argument_proximity_address.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

class gmaps_term_handler_argument_proximity_address extends views_handler_argument_numeric {
  function option_definition() {
    $options = parent::option_definition();

    $options['break_phrase'] = array('default' => FALSE);
    $options['not_arg'] = array('default' => TRUE);
    $options['level'] = array('default' => '');
    $options['countries'] = array('default' => array());
    $options['strict'] = array('default' => TRUE);
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['break_phrase'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
    $form['not']['#title'] = t('Exclude addresses');
    $form['not']['#description'] = t('If selected, terms have the same address as the term entered in the argument will be excluded rather than limiting the view.');
    $form['not_arg'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude the argument term'),
      '#description' => t("If selected, the term entered in the argument will be excluded from the results."),
      '#default_value' => !empty($this->options['not_arg']),
    );
    
    $parts = gmaps_address_get_part_labels();
    //equals to All
    unset($parts['thoroughfare']);
    $form['level'] = array(
      '#type' => 'select',
      '#title' => t('Deepest part to check'),
      '#default_value' => $this->options['level'],
      '#options' => array('' => t('All')) + $parts,
    );
    $form['countries'] = array(
      '#type' => 'select',
      '#title' => t('Allowed countries'),
      '#default_value' => $this->options['countries'],
      '#options' => gmaps_get_countries('names'),
      '#multiple' => TRUE,
      '#size' => 8,
      '#description' => t('Use only addresses from these countries.'),
    );
    $form['strict'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use strict filter'),
      '#description' => t('If selected, empty values will match only empty values rather than everything.'),
      '#default_value' => !empty($this->options['strict']),
    );
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
    
    $not = !empty($this->options['not']);
    
    if ($this->argument <= 0) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }

    $term = taxonomy_get_term($this->argument);
    if (empty($term)) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $geo = variable_get('gmaps_term_geo_data', array());
    $geo = $geo[$term->vid];
    if (empty($geo) || empty($geo['type']) || $geo['type'] == 'gmaps_point') {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $geo_data = gmaps_term_geo_data_load($term->tid, $term->vid);
    if (empty($geo_data) || ($geo['type'] == 'gmaps_anp' && gmaps_anp_empty($geo_data)) || gmaps_address_empty($geo_data->address)) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $filters = $args = array();
    $table_alias = $not ? 'gta' : $this->table_alias;
    
    if (empty($this->options['countries']) || !empty($this->options['countries'][$geo_data->address->country])) {
      $piece = gmaps_term_address_sql_filter($geo_data->address, $table_alias, empty($this->options['level']) ? NULL : $this->options['level'], !empty($this->options['strict']));
      if ($piece) {
        list($filter, $filter_args) = $piece;
        $filters[] = '('. $filter .')';
        $args = array_merge($args, $filter_args);
      }
    }
    
    if (!empty($filters)) {
      $op = ' OR ';
      if ($not) {
        $db_table = $this->query->table_queue[$this->table]['join']->table;
        $db_field = $this->query->table_queue[$this->table]['join']->field;
        $sql = "SELECT DISTINCT(gta.$db_field) FROM {". $db_table ."} gta";
        $sql .= ' WHERE ('. implode($op, $filters) .')';

        $ids = empty($this->options['not_arg']) ? array() : array($this->argument => $this->argument);
        $result = db_query($sql, $args);
        while($rec = db_fetch_object($result)) {
          $ids[$rec->$db_field] = $rec->$db_field;
        }
        if (empty($ids)) {
          return;
        }
        
        if (count($ids) > 1) {
          $placeholders = implode(', ', array_fill(0, sizeof($ids), '%d'));
          $this->query->add_where(0, $this->view->base_table .'.'. $this->view->base_field ." NOT IN ($placeholders)", $ids);
        }
        else {
          $this->query->add_where(0, $this->view->base_table .'.'. $this->view->base_field ." != %d", $ids);
        }
      }
      else {
        if (!empty($this->options['not_arg'])) {
          $this->query->add_where(0, "$this->table_alias.$this->real_field != %d", $this->argument);
        }
        $this->query->add_where(0, implode($op, $filters), $args);
      }
    }
    else if (!$not) {
      $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
    }
  }

  function title() {
    $term = taxonomy_get_term($this->argument);
    if (!empty($term)) {
      return check_plain($term->name);
    }
    return t('Unknown term');
  }
  
}

