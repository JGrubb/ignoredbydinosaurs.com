<?php
// $Id: gmaps_term_handler_field_point_part.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

class gmaps_term_handler_field_point_part extends views_handler_field_taxonomy {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = array('table' => 'term_data', 'field' => 'vid');
    $this->additional_fields['tid'] = array('table' => 'term_data', 'field' => 'tid');
  }
  
  function init(&$view, $options) {
    parent::init($view, $options);
    
    if ($this->options['gmaps']['apply_privacy']) {
      if ($this->definition['gmaps_point_part'] != 'elevation') {
        $this->additional_fields['map_type'] = 'map_type';
        $this->additional_fields['privacy'] = 'privacy';
        if ($this->definition['gmaps_point_part'] == 'latitude') {
          $this->additional_fields['longitude'] = 'longitude';
        }
        else {
          $this->additional_fields['latitude'] = 'latitude';
        }
      }
    }
  }

  function options(&$options) {
    parent::options($options);

    $labels = drupal_map_assoc(array('latitude', 'longitude', 'elevation'), 'ucfirst');
    $options['label'] = $labels[$this->definition['gmaps_point_part']];
    
    $options['gmaps'] = array(
      'apply_privacy' => TRUE,
      'user_settings' => TRUE,
      'format' => 0,
      'elevation' => 0,
      'prec' => $this->definition['gmaps_point_part'] == 'elevation' ? 0 : 2,
    );
  }

  /**
   * Provide formatter option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $options = $this->options;
    $gmaps = $options['gmaps'];

    module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
    $precisions = _gmaps_length_get_precision_options();
    
    $form['gmaps'] = array('#type' => 'fieldset',
      '#title' => t('GMaps settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );
    $form['gmaps']['apply_privacy'] = array('#type' => 'checkbox',
      '#title' => t('Apply privacy'),
      '#default_value' => $gmaps['apply_privacy'],
    );
    $form['gmaps']['user_settings'] = array('#type' => 'checkbox',
      '#title' => t('Use user/site settings'),
      '#default_value' => $gmaps['user_settings'],
    );
      
    if ($this->definition['gmaps_point_part'] != 'elevation') {
      $form['gmaps']['format'] = array('#type' => 'checkbox',
        '#title' => t('Use DMS format'),
        '#default_value' => $gmaps['format'],
        '#description' => t('DD will be used when disabled.'),
      );
    }
    else {
      $form['gmaps']['elevation'] = array('#type' => 'checkbox',
        '#title' => t('Always show elevation'),
        '#default_value' => $gmaps['elevation'],
        '#description' => t('Non-empty values will be displayed only, when disabled.'),
      );
    }
    $form['gmaps']['prec'] = array('#type' => 'select',
      '#title' => t('Precision'),
      '#default_value' => $gmaps['prec'],
      '#options' => $precisions,
      '#description' => t('Ammount of decimal digits to display.'),
    );
  }

  /**
   * Provide text for the administrative summary
   */
  function admin_summary() {
    $s = $this->get_render_settings();
    if ($this->definition['gmaps_point_part'] == 'elevation') {
      return $s['elevation'] ? t('Always') : t('Non-empty');
    }
    else {
      return $s['format'] ? 'DMS' : 'DD';
    }
  }

  function render($values) {
    global $user;
    
    $value = $values->{$this->field_alias};
    $part = $this->definition['gmaps_point_part'];
    
    if ($part == 'elevation') {
      $s = $this->get_render_settings();
      
      if ($this->options['gmaps']['apply_privacy']) {
        do {
          if (user_access('access protected gmaps data')) {
            break;
          }
          $value = $value + $value*(mt_rand(-20, 20)/100);
        } while(0);
      }
      
      if (($s['elevation'] || !gmaps_floats_equal($value, 0))) {
        $elevation = array('value' => $value, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
        $value = gmaps_length_format_plain($elevation, $s['prec']);
      }
      else {
        return '';
      }
    }
    else {
      if (empty($value)) {
        return '';
      }
      if ($this->options['gmaps']['apply_privacy'] && !($point = $this->_build_item_from_values($values))) {
        return '';
      }
      if ($point) {
        $value = $point->{$part};
      }
      
      $s = $this->get_render_settings();
      
      if ($s['format']) {
        $func = 'gmaps_coords_format_dms_'. $part;
        $value = $func($value, $s['prec']);
      }
      else {
        $value = _gmaps_number_format($value, $s['prec']) .'°';
      }
    }
    return $this->render_link($value, $values);
  }

  private function _build_item_from_values($values) {
    if (!$this->options['gmaps']['apply_privacy']) {
      return NULL;
    }
    
    $point = new stdClass();
    $point->{$this->definition['gmaps_point_part']} = $values->{$this->field_alias};
    foreach(array('latitude', 'longitude', 'map_type', 'privacy') as $part) {
      if ($this->aliases[$part]) {
        if ($part == 'privacy') {
          $value = !empty($values->{$this->aliases[$part]}) ? unserialize($values->{$this->aliases[$part]}) : array();
        }
        else {
          $value = $values->{$this->aliases[$part]};
        }
        $point->$part = $value;
      }
    }
    $point->uid = 0;
    
    if (gmaps_point_empty($point)) {
      return NULL;
    }
    
    gmaps_coords_apply_privacy($point);
    
    return $point;
  }
  
  private function get_render_settings() {
    global $user;
    
    if ($this->definition['gmaps_point_part'] == 'elevation') {
      return $this->options['gmaps']['user_settings'] ?
        array(
          'elevation' => $user->gmaps_settings['coordinate']['elevation'] == 2 ? 1 : 0,
          'prec' => $user->gmaps_settings['coordinate']['elevation_prec'],
        ) :
        $this->options['gmaps'];
    }
    else {
      if ($this->options['gmaps']['user_settings']) {
        $format = $user->gmaps_settings['coordinate']['format'] == 'dd' ? 0 : 1;
        $prec = $user->gmaps_settings['coordinate'][$format ? 'dms_prec' : 'dd_prec']; 
      }
      else {
        $format = $this->options['gmaps']['format'];
        $prec = $this->options['gmaps']['prec'];
      }
      return array('format' => $format, 'prec' => $prec);
    }
  }

}