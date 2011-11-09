<?php
// $Id: gmaps_user_handler_field_privacy.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

class gmaps_user_handler_field_privacy extends views_handler_field_user {
  function render($values) {
    static $map, $included, $countryicons, $languageicons;
    
    $value = $values->{$this->field_alias};
    if (empty($value) && ((string)$value !== '0' || $this->options['hide_empty'])) {
      return '';
    }
    
    if ($this->definition['table'] == 'gmaps_user_address') {
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