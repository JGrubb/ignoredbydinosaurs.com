<?php
// $Id: gmaps_term_handler_filter_country.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

/**
 * Filter by ountry
 */
class gmaps_term_handler_filter_country extends views_handler_filter_in_operator {
  var $value_form_type = 'select';
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Countries');
      $names = gmaps_get_countries('names');
      
      $countries = array();
      $result = db_query("SELECT DISTINCT(gta.country) FROM {gmaps_term_address} gta");
      while ($c = db_fetch_object($result)) {
        if ($names[$c->country]) {
          $countries[$c->country] = $names[$c->country];
        }
      }
      
      asort($countries);
      
      $this->value_options = $countries;
    }
  }
}
