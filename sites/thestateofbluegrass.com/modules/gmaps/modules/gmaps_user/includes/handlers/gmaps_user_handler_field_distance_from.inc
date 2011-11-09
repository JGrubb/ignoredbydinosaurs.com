<?php
// $Id: gmaps_user_handler_field_distance_from.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

class gmaps_user_handler_field_distance_from extends views_handler_field_user {
  private static $geoip;
  private static $refloc;

  function init(&$view, $options) {
    parent::init($view, $options);
    
    if ($this->_get_location() || strpos($this->options['field'], 'viewloc') === 0) {
      $this->additional_fields['latitude'] = 'latitude';
      $this->additional_fields['longitude'] = 'longitude';
      $this->additional_fields['elevation'] = 'elevation';
      $this->additional_fields['map_type'] = 'map_type';
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['gmaps'] = array(
      'contains' => array(
        'prefix' => array('default' => '', 'translatable' => TRUE),
        'suffix' => array('default' => '', 'translatable' => TRUE),
      ),
    );

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['gmaps']['#tree'] = TRUE;
    $form['gmaps']['prefix'] = array(
      '#title' => t('Value prefix text'),
      '#type' => 'textfield',
      '#default_value' => $this->options['gmaps']['prefix'],
      '#description' => t('Any text to display before the distance. You may include HTML.'),
    );
    $form['gmaps']['suffix'] = array(
      '#title' => t('Value suffix text'),
      '#type' => 'textfield',
      '#default_value' => $this->options['gmaps']['suffix'],
      '#description' => t('Any text to display after the distance. You may include HTML.'),
    );
  }

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
  
  function click_sortable() {
    return FALSE;
  }

  function click_sort($order) {
  }

  function render($values) {
    $options = $this->options;

    $loc = $this->_get_location();
    if (!$loc) {
      return;
    }
    
    $point = $this->_build_item_from_values($values);
    if (!$point) {
      return;
    }

    if ($value = $this->_render_value($point, $loc)) {
      $value = $this->_wrap_value($value);
      return $this->render_link($value, $values);
    }
    
    return;
  }
  
  private function _build_item_from_values($values) {
    $point = new stdClass();
    foreach(array('latitude', 'longitude', 'map_type', 'elevation') as $part) {
      if ($this->aliases[$part]) {
        $point->$part = $values->{$this->aliases[$part]};
      }
    }
    
    if (gmaps_point_empty($point)) {
      return NULL;
    }
    
    return $point;
  }
  
  protected function _render_value($point, $loc = NULL, $dist = NULL) {
    if (!$loc) {
      $loc = $this->_get_location();
      if (!$loc) {
        return;
      }
    }
    
    if ($dist || ($dist = gmaps_coords_distance($point, $loc->point))) {
      if ($this->options['field'] == 'refloc' || strpos($this->options['field'], 'viewloc') === 0) {
        if ((isset($loc->address) && !gmaps_address_empty($loc->address))) {
          $label = gmaps_address_format_plain($loc->address, 'iso3_name', 'both');
          $info = $this->options['field'] == 'refloc' ? t('Reference location') : (($loc->label ? $loc->label .' - ' : ''). t('View location'));
        }
        else {
          $label = $this->options['field'] == 'refloc' ? 'reference location' : 'view location';
        }
      }
      else {
        $label = gmaps_address_format_plain($loc->address, 'name', 'name');
        $info = t('GeoIP');
      }
      $title = t('Distance from @label', array('@label' => $label)) . ($info ? ' - '. $info : '');
      return '<span title="'. $title .'" alt="'. $title .'">'. gmaps_length_render(array('value' => $dist, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)) .'</span>';
    }
    
    return;
  }
  
  protected function _wrap_value($value) {
    $options = $this->options['gmaps'];
    
    if (!empty($value) && (!empty($options['prefix']) || !empty($options['suffix']))) {
      $result = '';
      $tokens = $this->get_render_tokens($options);
      
      if (!empty($options['prefix'])) {
        $result .= filter_xss_admin(strtr($options['prefix'], $tokens));
      }
      
      $result .= $value;
      
      if (!empty($options['suffix'])) {
        $result .= filter_xss_admin(strtr($options['suffix'], $tokens));
      }
      
      return $result;
    }
    
    return $value;
  }
}