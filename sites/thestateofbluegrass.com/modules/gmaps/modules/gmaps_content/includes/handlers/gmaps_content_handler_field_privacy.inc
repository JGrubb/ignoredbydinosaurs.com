<?php
// $Id: gmaps_content_handler_field_privacy.inc,v 1.1.2.1 2009/09/14 07:57:59 xmarket Exp $

class gmaps_content_handler_field_privacy extends views_handler_field_node {
  protected $content_field;

  function construct() {
    parent::construct();
  }

  function init(&$view, $options) {
    parent::init($view, $options);
  }

  protected function get_content_field() {
    if (is_null($this->content_field)) {
      $this->content_field = content_fields($this->definition['content_field_name']);
    }
    return $this->content_field;
  }
  
  function render($values) {
    static $map, $included, $countryicons, $languageicons;
    
    $value = $values->{$this->field_alias};
    if (empty($value) && ((string)$value !== '0' || $this->options['hide_empty'])) {
      return '';
    }
    
    if ($this->definition['table'] == 'gmaps_content_address') {
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