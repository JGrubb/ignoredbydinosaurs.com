<?php
// $Id: gmaps_content_handler_argument_proximity_address.inc,v 1.1.2.3 2010/04/01 07:42:10 xmarket Exp $

class gmaps_content_handler_argument_proximity_address extends views_handler_argument_node_nid {
  protected $content_field;

  protected function get_content_field() {
    if (is_null($this->content_field)) {
      if ($this->definition['content_field_name']) {
        $this->content_field = content_fields($this->definition['content_field_name']);
      }
      else {
        $this->content_field = FALSE;
      }
    }
    return $this->content_field;
  }
  
  function option_definition() {
    $options = parent::option_definition();

    $options['break_phrase'] = array('default' => FALSE);
    $options['not_arg'] = array('default' => TRUE);
    $options['level'] = array('default' => '');
    $options['countries'] = array('default' => array());
    $options['filter_fields'] = array('default' => array());
    $options['result_fields'] = array('default' => array());
    $options['values'] = array('default' => 'first');
    $options['strict'] = array('default' => TRUE);
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field = $this->get_content_field();
    
    $form['break_phrase'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
    $form['not']['#title'] = t('Exclude addresses');
    $form['not']['#description'] = t('If selected, nodes have the same addresses as the node entered in the argument will be excluded rather than limiting the view.');
    $form['not_arg'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude the argument node'),
      '#description' => t("If selected, the node entered in the argument will be excluded from the results."),
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
    $fields = array();
    foreach(content_fields() as $field_name => $field_info) {
      if (in_array($field_info['type'], array('gmaps_address', 'gmaps_anp'))) {
        $fields[$field_name] = t('@label (!name)', array('@label' => t($field_info['widget']['label']), '!name' => $field_name));
      }
    }
    $form['filter_fields'] = array(
      '#type' => 'select',
      '#title' => t('Filter fields'),
      '#default_value' => $this->options['filter_fields'],
      '#options' => $fields,
      '#multiple' => TRUE,
      '#size' => 3,
      '#description' => t('Use only these fields of the argument node to filter. All fields will be used, if none selected.'),
    );
    if ($field) {
      $form['result_fields'] = array(
        '#type' => 'value',
        '#value' => array(),
      );
    }
    else {
      $form['result_fields'] = array(
        '#type' => 'select',
        '#title' => t('Result fields'),
        '#default_value' => $this->options['result_fields'],
        '#options' => $fields,
        '#multiple' => TRUE,
        '#size' => 3,
        '#description' => t('Filter results for these fields.'),
      );
    }
    $form['values'] = array(
      '#type' => 'select',
      '#title' => t('Check values'),
      '#default_value' => $this->options['values'],
      '#options' => array('first' => t('First'), 'all' => t('All (OR)')),
      '#access' => !$field || $field['multiple'],
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
        $this->query->add_where(0, "node.vid = 0");
      }
      return;
    }

    $node = node_load($this->argument);
    if (!$node) {
      if (!$not) {
        $this->query->add_where(0, "node.vid = 0");
      }
      return;
    }
    
    $fields = $filters = $args = array();
    
    $field = $this->get_content_field();
    
    if (!empty($this->options['filter_fields'])) {
      $fields = $this->options['filter_fields'];
    }
    else {
      $type = content_types($node->type);
      foreach($type['fields'] as $field_name => $field_info) {
        if (in_array($field_info['type'], array('gmaps_address', 'gmaps_anp'))) {
          $fields[] = $field_name;
        }
      }
    }
    if (empty($fields)) {
      if (!$not) {
        $this->query->add_where(0, "node.vid = 0");
      }
      return;
    }
    
    $table_alias = $not ? 'gca' : $this->table_alias;
    foreach($fields as $field_name) {
      $items = empty($node->{$field_name}) ? array() : $node->{$field_name};
      
      if (empty($items)) {
        continue;
      }
      if ($this->options['values'] == 'first') {
        $items = array(array_shift($items));
      }
      
      foreach($items as $delta => $item) {
        if (!empty($this->options['countries']) && empty($this->options['countries'][$item['value']->address->country])) {
          continue;
        }
        $piece = gmaps_content_address_sql_filter($item['value']->address, $table_alias, empty($this->options['level']) ? NULL : $this->options['level'], !empty($this->options['strict']));
        if ($piece) {
          list($filter, $filter_args) = $piece;
          $filters[] = '('. $filter .')';
          $args = array_merge($args, $filter_args);
        }
      }
    }
    
    if (!empty($filters)) {
      $op = ' OR ';
      if (!$field && !empty($this->options['result_fields'])) {
        $filters = array('('. implode(' OR ', $filters) .')');
        $op = ' AND ';
        if (count($this->options['result_fields']) > 1) {
          $placeholders = implode(', ', array_fill(0, sizeof($this->options['result_fields']), "'%s'"));
          $filters[] = "($table_alias.field_name IN ($placeholders) OR $table_alias.field_name IS NULL)";
        }
        else {
          $filters[] = "($table_alias.field_name = '%s' OR $table_alias.field_name IS NULL)";
        }
        $args = array_merge($args, $this->options['result_fields']);
      }
      if ($not) {
        $db_table = $this->query->table_queue[$this->table]['join']->table;
        $db_field = $this->query->table_queue[$this->table]['join']->field;
        $sql = "SELECT DISTINCT(gca.$db_field) FROM {". $db_table ."} gca";
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

}

