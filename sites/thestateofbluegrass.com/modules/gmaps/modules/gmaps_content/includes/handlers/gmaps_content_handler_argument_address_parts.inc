<?php
// $Id: gmaps_content_handler_argument_address_parts.inc,v 1.1.2.2 2010/04/01 07:42:10 xmarket Exp $

class gmaps_content_handler_argument_address_parts extends views_handler_argument {
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

    //$options['set_breadcrumb'] = array('default' => FALSE);
    $options['not'] = array('default' => FALSE);
    $options['strict'] = array('default' => FALSE);
    $options['fields'] = array('default' => array());
    
    $options['fast_filter'] = array('default' => FALSE);
    
    _gmaps_content_address_fast_filter_option_definition($options);
            
    $options['address'] = array('default' => array());
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $field = $this->get_content_field();
    
    $form['not'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude addresses'),
      '#description' => t('If selected, the addresses entered in the argument, or in modifier arguments will be excluded rather than limiting the view.'),
      '#default_value' => !empty($this->options['not']),
    );
    $form['strict'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use strict filter'),
      '#description' => t('If selected, empty values will match only empty values rather than everything.'),
      '#default_value' => !empty($this->options['strict']),
    );
    if ($field) {
      $form['fields'] = array(
        '#type' => 'value',
        '#value' => array(),
      );
    }
    else {
      $fields = array();
      foreach(content_fields() as $field_name => $field_info) {
        if (in_array($field_info['type'], array('gmaps_address', 'gmaps_anp'))) {
          $fields[$field_name] = t('@label (!name)', array('@label' => t($field_info['widget']['label']), '!name' => $field_name));
        }
      }
      $form['fields'] = array(
        '#type' => 'select',
        '#title' => t('Result fields'),
        '#default_value' => $this->options['fields'],
        '#options' => $fields,
        '#multiple' => TRUE,
        '#size' => 3,
        '#description' => t('Filter results for these fields.'),
      );
    }

    $form['fast_filter'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use fast filter'),
      '#description' => t('If selected, only administrative area and locality modifiers will be used.'),
      '#default_value' => !empty($this->options['fast_filter']),
    );
    
    _gmaps_content_address_fast_filter_options_form($form, $form_state, $this);
  }

  function query() {
    $this->ensure_my_table();
    $table_alias = empty($this->options['not']) ? $this->table_alias : 'gca';
      
    $field = $this->get_content_field();
    
    $options = $this->options;
    
    $adr = (object)$this->options['address'];
    $adr->country = $this->argument;
    if (gmaps_address_empty($adr) && empty($options['not']) && empty($options['strict'])) {
      return;
    }
    
    if (!empty($options['fast_filter']) || gmaps_address_empty($adr)) {
      $piece = gmaps_content_address_sql_filter_fast($adr, $table_alias, !empty($options['strict']), $options['fast_filter_fields']);
      if ($piece) {
        list($filter, $args) = $piece;
      }
    }
    else {
      $last_part = 'country';
      $parts = _gmaps_address_get_parts();
      foreach($parts as $part => $label) {
        if (array_key_exists($part, $this->options['address']) && !is_null($this->options['address'][$part])) {
          $last_part = $part;
        }
      }
      
      $piece = gmaps_content_address_sql_filter($adr, $table_alias, $last_part, !empty($this->options['strict']));
      if ($piece) {
        list($filter, $args) = $piece;
      }
    }
    
    if (!empty($filter)) {
      if (!$field && !empty($this->options['fields'])) {
        if (count($this->options['fields']) > 1) {
          $placeholders = implode(', ', array_fill(0, sizeof($this->options['fields']), "'%s'"));
          $filter .= " AND ($table_alias.field_name IN ($placeholders) OR $table_alias.field_name IS NULL)";
        }
        else {
          $filter .= " AND ($table_alias.field_name = '%s' OR $table_alias.field_name IS NULL)";
        }
        $args = array_merge($args, $this->options['fields']);
      }
      
      if (empty($this->options['not'])) {
        $this->query->add_where(0, $filter, $args);
      }
      else {
        $db_table = $this->query->table_queue[$this->table]['join']->table;
        $id_field = $this->query->table_queue[$this->table]['join']->field;
        $result = db_query("SELECT DISTINCT(gca.$id_field) FROM {". $db_table ."} gca WHERE $filter", $args);
        
        $ids = array();
        while($rec = db_fetch_object($result)) {
          $ids[$rec->$id_field] = $rec->$id_field;
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
    }
    else if (empty($this->options['not'])) {
      $this->query->add_where(0, $this->view->base_table .'.'. $this->view->base_field ." = 0");
    }
  }

  function title() {
    $names = gmaps_get_countries('names');
    if (!empty($names[$this->argument])) {
      return check_plain($names[$this->argument]);
    }
    return t('Unknown country');
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

  function summary_name($data) {
    $value = $data->{$this->name_alias};
    $names = gmaps_get_countries('names');
    if (empty($value) || empty($names[$value])) {
      if (empty($this->definition['empty field name'])) {
        return t('Unknown country');
      }
      else {
        return check_plain($this->definition['empty field name']);
      }
      
    }
    return check_plain($names[$value]);
  }

}
