<?php
// $Id: gmaps_content_handler_argument_proximity_distance.inc,v 1.1.2.3 2010/04/01 07:42:10 xmarket Exp $

class gmaps_content_handler_argument_proximity_distance extends views_handler_argument_node_nid {
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
    
    $options['filter_fields'] = array('default' => array());
    $options['result_fields'] = array('default' => array());
    $options['values'] = array('default' => 'first');
    
    $options['viewloc'] = array('default' => 1);
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field = $this->get_content_field();
    
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
      '#title' => t('Exclude the argument node'),
      '#description' => t("If selected, the node entered in the argument will be excluded from the results."),
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
    
    $fields = array();
    foreach(content_fields() as $field_name => $field_info) {
      if (in_array($field_info['type'], array('gmaps_address', 'gmaps_anp'))) {
        $fields[$field_name] = t('@label (!name)', array('@label' => t($field_info['widget']['label']), '!name' => $field_name));
      }
    }
    $form['filter_fields'] = array(
      '#type' => 'select',
      '#title' => t('Filter fields'),
      '#default_value' => $options['filter_fields'],
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
        '#default_value' => $options['result_fields'],
        '#options' => $fields,
        '#multiple' => TRUE,
        '#size' => 3,
        '#description' => t('Filter results for these fields.'),
      );
    }
    $form['values'] = array(
      '#type' => 'select',
      '#title' => t('Check values'),
      '#default_value' => $options['values'],
      '#options' => array('first' => t('First'), 'all' => t('All (OR)')),
      '#access' => !$field || $field['multiple'],
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

    $node = node_load($this->argument);
    if (!$node) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $fields = $filters = $args = array();
    $order = '';
    
    $field = $this->get_content_field();
    
    if (!empty($this->options['filter_fields'])) {
      $fields = $this->options['filter_fields'];
    }
    else {
      $type = content_types($node->type);
      foreach($type['fields'] as $field_name => $field_info) {
        if (in_array($field_info['type'], array('gmaps_point', 'gmaps_anp'))) {
          $fields[] = $field_name;
        }
      }
    }
    if (empty($fields)) {
      if (!$not) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
      }
      return;
    }
    
    $dist = array('value' => $this->options['dist_m'], 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    $maxdist = array('value' => $this->options['maxdist_m'], 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    
    foreach($fields as $field_name) {
      $items = empty($node->{$field_name}) ? array() : $node->{$field_name};
      
      if (empty($items)) {
        continue;
      }
      if ($this->options['values'] == 'first') {
        $items = array(array_shift($items));
      }
      
      foreach($items as $delta => $item) {
        if (empty($map_type) || ($map_type == $item['value']->point->map_type)) {
          if ($this->options['filter']) {
            $piece = gmaps_content_point_sql_filter_distance($item['value']->point, $this->options['operator'], $dist, $maxdist, $this->table_alias, !empty($this->options['rounded']));
            if ($piece) {
              $map_type = $item['value']->point->map_type;
              list($filter, $filter_args) = $piece;
              $filters[] = '('. $filter .')';
              $args = array_merge($args, $filter_args);
            }
          }
          if ($this->options['sort'] && empty($order)) {
            if ($piece = gmaps_content_point_sql_sort($item['value']->point, $this->table_alias)) {
              $map_type = $item['value']->point->map_type;
              $order = $piece;
              $order_item = $item;
            }
          }
          if ($this->options['viewloc'] && $piece && empty($viewloc)) {
            $viewloc = $item['value'];
          }
        }
      }
    }
    
    if (!empty($filters) || !empty($order)) {
      if (!empty($this->options['not_arg'])) {
        $this->query->add_where(0, "$this->table_alias.$this->real_field != %d", $this->argument);
      }
      if (!empty($filters)) {
        $op = ' OR ';
        if (!$field && !empty($this->options['result_fields'])) {
          $filters = array('('. implode(' OR ', $filters) .')');
          $op = ' AND ';
          if (count($this->options['result_fields']) > 1) {
            $placeholders = implode(', ', array_fill(0, sizeof($this->options['result_fields']), "'%s'"));
            $filters[] = "$this->table_alias.field_name IN ($placeholders)";
          }
          else {
            $filters[] = "$this->table_alias.field_name = '%s'";
          }
          $args = array_merge($args, $this->options['result_fields']);
        }
        $this->query->add_where(0, implode($op, $filters), $args);
      }
      if (!empty($order)) {
        if (empty($filters)) {
          $this->query->add_where(0, "$this->table_alias.map_type = '%s'", $order_item['value']->point->map_type);
        }
        $this->query->add_orderby(NULL, $order, $this->options['order'], $this->table_alias . '_proximity_order');
      }
      if ($this->options['viewloc'] && $viewloc) {
        $viewloc->label = $node->title;
        $this->view->gmaps_viewloc = $viewloc;
      }
    }
    else if (!$not) {
      $this->query->add_where(0, "$this->table_alias.$this->real_field = 0");
    }
  }
  
}

