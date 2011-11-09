<?php
// $Id: gmaps_user_handler_filter_address_part.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

class gmaps_user_handler_filter_address_part extends views_handler_filter_string {

  function option_definition() {
    $options = parent::option_definition();

    $options['case'] = array('default' => FALSE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['case']['#description'] = t('Address part filters are optimized for both case sensitive and case insensitive searching. MySQL might ignore case sensitivity.');
  }

  function case_transform() {
    return '';
  }
  
  function query() {
    $this->ensure_my_table();
    if (empty($this->options['case'])) {
      $this->value = strtoupper($this->value);
    }
    $field = "$this->table_alias.". (empty($this->options['case']) ? 'u_' : '') ."$this->real_field";
    $upper = $this->case_transform();

    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field, $upper);
    }
  }

}
