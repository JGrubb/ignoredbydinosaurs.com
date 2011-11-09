<?php
// $Id: gmaps_content_handler_field_map_type.inc,v 1.1.2.2 2010/04/01 07:42:10 xmarket Exp $

class gmaps_content_handler_field_map_type extends views_handler_field_node {
  function construct() {
    parent::construct();
  }

  function init(&$view, $options) {
    parent::init($view, $options);
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