<?php
// $Id: gmaps_term_handler_sort_distance_from.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

/**
 * Handle various distance sorts.
 */
class gmaps_term_handler_sort_distance_from extends views_handler_sort {
  private static $geoip;
  private static $refloc;

  protected function _get_location() {
    if ($this->definition['gmaps_location_type'] == 'refloc') {
      if (is_null(self::$refloc)) {
        self::$refloc = gmaps_get_reference_location();
        if (empty(self::$refloc)) {
          self::$refloc = FALSE;
        }
      }
      return self::$refloc;
    }
    else if ($this->definition['gmaps_location_type'] == 'geoip') {
      if (is_null(self::$geoip)) {
        self::$geoip = gmaps_get_geoip_location();
        if (empty(self::$geoip)) {
          self::$geoip = FALSE;
        }
      }
      return self::$geoip;
    }
    else if ($this->definition['gmaps_location_type'] == 'viewloc') {
      return $this->view->gmaps_viewloc;
    }
    return FALSE;
  }
  
  function query() {
    if (($loc = $this->_get_location()) && $loc->point->map_type == $this->definition['gmaps_map_type']) {
      $this->ensure_my_table();
      if ($sort = gmaps_term_point_sql_sort($loc->point, $this->table_alias)) {
        $this->query->add_where(0, "$this->table_alias.map_type = '%s'", $loc->point->map_type);
        $this->query->add_orderby(NULL, $sort, $this->options['order'], $this->table_alias .'_'. $this->definition['gmaps_location_type']);
      }
    }
  }

}
