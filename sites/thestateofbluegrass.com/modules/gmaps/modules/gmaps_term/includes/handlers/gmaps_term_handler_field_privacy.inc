<?php
// $Id: gmaps_term_handler_field_privacy.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

class gmaps_term_handler_field_privacy extends views_handler_field_taxonomy {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = array('table' => 'term_data', 'field' => 'vid');
    $this->additional_fields['tid'] = array('table' => 'term_data', 'field' => 'tid');
  }
  
  function render($values) {
    static $map, $included, $countryicons, $languageicons;
    
    $value = $values->{$this->field_alias};
    if (empty($value) && ((string)$value !== '0' || $this->options['hide_empty'])) {
      return '';
    }
    
    if ($this->definition['gmaps_data_type'] == 'gmaps_address') {
      $levels = gmaps_address_privacy_levels();
      $value = $levels[$value];
    }
    else {
      $value = unserialize($value);
      $value = gmaps_length_format_plain($value, -1);
    }
    
    return $this->render_link($value, $values);
  }

}