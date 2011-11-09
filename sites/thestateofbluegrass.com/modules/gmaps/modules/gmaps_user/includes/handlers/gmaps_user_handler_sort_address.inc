<?php
// $Id: gmaps_user_handler_sort_address.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

/**
 * Handle various address sorts.
 */
class gmaps_user_handler_sort_address extends views_handler_sort {
  function option_definition() {
    $options = parent::option_definition();

    $options['fields'] = array('default' => 'fast');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $labels = gmaps_address_get_part_labels();
    $form['fields'] = array(
      '#type' => 'select',
      '#title' => t('Fields'),
      '#default_value' => $this->options['fields'],
      '#options' => array(
        'fast' => implode(', ', array($labels['country'], $labels['locality'])),
        'fast_admin' => implode(', ', array($labels['country'], $labels['adminarea'], $labels['locality'])),
        'all' => t('All fields'),
      ),
    );
  }
  
  function query() {
    $this->ensure_my_table();
    switch ($this->options['fields']) {
      case 'fast':
      case 'fast_admin':
        $this->query->add_orderby($this->table_alias, 'country', $this->options['order']);
        if ($this->options['fields'] == 'fast_admin') {
          $this->query->add_orderby($this->table_alias, 'u_adminarea', $this->options['order']);
        }
        $this->query->add_orderby($this->table_alias, 'u_locality', $this->options['order']);
        break;
        
      default:
        foreach (_gmaps_address_get_parts() as $part => $label) {
          $this->query->add_orderby($this->table_alias, ($part == 'country' ? '' : 'u_'). $part, $this->options['order']);
        }
    }
  }

}
