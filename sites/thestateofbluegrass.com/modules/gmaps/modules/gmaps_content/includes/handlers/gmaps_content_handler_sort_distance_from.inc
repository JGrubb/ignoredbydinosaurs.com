<?php
// $Id: gmaps_content_handler_sort_distance_from.inc,v 1.1.2.1 2009/09/14 07:57:59 xmarket Exp $

/**
 * Handle various distance sorts.
 */
class gmaps_content_handler_sort_distance_from extends views_handler_sort {
  private static $geoip;
  private static $refloc;

  protected function _get_location() {
    if ($this->options['field'] == 'refloc') {
      if (is_null(self::$refloc)) {
        self::$refloc = gmaps_get_reference_location();
        if (empty(self::$refloc)) {
          self::$refloc = FALSE;
        }
      }
      return self::$refloc;
    }
    else if ($this->options['field'] == 'geoip') {
      if (is_null(self::$geoip)) {
        self::$geoip = gmaps_get_geoip_location();
        if (empty(self::$geoip)) {
          self::$geoip = FALSE;
        }
      }
      return self::$geoip;
    }
    else {
      return $this->view->gmaps_viewloc;
    }
  }
  
  function query() {
    if (($loc = $this->_get_location()) && $loc->point->map_type == $this->definition['map_type']) {
      $this->ensure_my_table();
      if ($sort = gmaps_content_point_sql_sort($loc->point, $this->table_alias)) {
        $this->query->add_where(0, "$this->table_alias.map_type = '%s'", $loc->point->map_type);
        $this->query->add_orderby(NULL, $sort, $this->options['order'], $this->table_alias .'_'. $this->options['field']);
      }
    }
  }

}
