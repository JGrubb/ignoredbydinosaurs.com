<?php
// $Id: gmaps_term_handler_field_map_type.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

class gmaps_term_handler_field_map_type extends views_handler_field_taxonomy {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = array('table' => 'term_data', 'field' => 'vid');
    $this->additional_fields['tid'] = array('table' => 'term_data', 'field' => 'tid');
  }
  
  function render($values) {
    $names = gmaps_map_get_types('names');
    $value = $values->{$this->field_alias};
    if ($names[$value]) {
      $value = $names[$value];
    }
    return $this->render_link($value, $values);
  }

}