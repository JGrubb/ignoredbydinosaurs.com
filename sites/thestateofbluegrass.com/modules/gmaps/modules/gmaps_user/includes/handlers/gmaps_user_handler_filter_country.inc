<?php
// $Id: gmaps_user_handler_filter_country.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

/**
 * Filter by ountry
 */
class gmaps_user_handler_filter_country extends views_handler_filter_in_operator {
  var $value_form_type = 'select';
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Countries');
      $names = gmaps_get_countries('names');
      
      $countries = array();
      $result = db_query("SELECT DISTINCT(gua.country) FROM {gmaps_user_address} gua");
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
