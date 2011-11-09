<?php
// $Id: gmaps_content_handler_filter_country.inc,v 1.1.2.2 2009/11/23 13:23:48 xmarket Exp $
/**
 * Filter by ountry
 */
class gmaps_content_handler_filter_country extends views_handler_filter_in_operator {
  var $value_form_type = 'select';
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Countries');
      $names = gmaps_get_countries('names');
      
      $countries = array();
      $result = db_query("SELECT DISTINCT(gca.country) FROM {gmaps_content_address} gca");
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
