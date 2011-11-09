<?php
// $Id: gmaps_user_handler_argument_address_part_modifier.inc,v 1.1.2.2 2010/04/01 07:42:10 xmarket Exp $

class gmaps_user_handler_argument_address_part_modifier extends views_handler_argument {
  function query() { }
  function pre_query() {
    // We don't know our argument yet, but it's based upon our position:
    $argument = isset($this->view->args[$this->position]) ? $this->view->args[$this->position] : NULL;
    if (is_null($argument) || ((string)$this->options['wildcard'] !== '' && $argument == $this->options['wildcard'])) {
      return;
    }
    
    // figure out which argument preceded us.
    $keys = array_reverse(array_keys($this->view->argument));
    $skip = TRUE;
    foreach ($keys as $key) {
      if ($key == $this->options['id']) {
        $skip = FALSE;
        continue;
      }

      if ($skip) {
        continue;
      }

      if (empty($this->view->argument[$key])) {
        continue;
      }

      if (isset($handler)) {
        unset($handler);
      }

      $handler = &$this->view->argument[$key];
      if (empty($handler->definition['accept part modifier'])) {
        continue;
      }

      // Finally!
      $handler->options['address'][$this->definition['gmaps_address_part']] = $argument;
    }
  }
  
  function title() {
    if (!empty($this->argument)) {
      return check_plain($this->argument);
    }
    $parts = gmaps_address_get_part_labels();
    return t('Unknown @part', array('@part' => $parts[$this->definition['gmaps_address_part']]));
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
    if (empty($value)) {
      if (empty($this->definition['empty field name'])) {
        $parts = gmaps_address_get_part_labels();
        return t('Unknown @part', array('@part' => $parts[$this->definition['gmaps_address_part']]));
      }
      else {
        return check_plain($this->definition['empty field name']);
      }
      
    }
    return check_plain($value);
  }

}

