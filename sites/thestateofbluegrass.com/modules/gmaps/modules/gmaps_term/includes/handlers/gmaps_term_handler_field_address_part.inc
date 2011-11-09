<?php
// $Id: gmaps_term_handler_field_address_part.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

class gmaps_term_handler_field_address_part extends views_handler_field_taxonomy {
  function construct() {
    parent::construct();
    $this->additional_fields['vid'] = array('table' => 'term_data', 'field' => 'vid');
    $this->additional_fields['tid'] = array('table' => 'term_data', 'field' => 'tid');
  }
  
  function init(&$view, $options) {
    parent::init($view, $options);
    
    $this->additional_fields['privacy'] = 'privacy';
    
    if ($this->definition['gmaps_address_part'] == 'adminarea') {
      $this->additional_fields['country'] = 'country';
    }
  }

  function options(&$options) {
    parent::options($options);

    $labels = gmaps_address_get_part_labels();
    $options['label'] = $labels[$this->definition['field']];
    
    $options['gmaps'] = array(
      'apply_privacy' => TRUE,
      'user_settings' => TRUE,
      'country' => 'name',
      'state' => '',
      'flag' => '',
    );
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $countryicons = module_exists('countryicons');
    //partially supported
    $languageicons = module_exists('languageicons');
    
    $options = $this->options;
    $gmaps = $options['gmaps'];

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
    
    if (in_array($this->definition['gmaps_address_part'], array('country', 'adminarea'))) {
      module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
      
      $form['gmaps']['user_settings'] = array('#type' => 'checkbox',
        '#title' => t('Use user/site settings'),
        '#default_value' => $gmaps['user_settings'],
      );
      
      if ($this->definition['gmaps_address_part'] == 'country') {
        $form['gmaps']['country'] = array('#type' => 'select',
          '#title' => t('Country display'),
          '#default_value' => $gmaps['country'],
          '#options' => _gmaps_address_get_country_options(),
        );
        $form['gmaps']['flag'] = array('#type' => 'select',
          '#title' => t('Display country flags'),
          '#default_value' => $gmaps['flag'],
          '#options' => _gmaps_address_get_flag_options(),
          '#access' => ($countryicons || $languageicons),
        );
      }
      else {
        $form['gmaps']['state'] = array('#type' => 'select',
          '#title' => t('State display'),
          '#default_value' => $gmaps['state'],
          '#options' => _gmaps_address_get_state_options(),
          '#description' => t('Applied to US and Canadian states only.'),
        );
      }
    }
  }

  /**
   * Return DIV or SPAN based upon the field's element type.
   */
  function element_type() {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    if (in_array($this->definition['gmaps_address_part'], array('country', 'adminarea'))) {
      return 'div';
    }
    else {
      return 'span';
    }
  }

  /**
   * Provide text for the administrative summary
   */
  function admin_summary() {
    if (in_array($this->definition['gmaps_address_part'], array('country', 'adminarea'))) {
      if ($this->options['gmaps']['user_settings']) {
        return t('User/site settings');
      }
      module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
      if ($this->definition['gmaps_address_part'] == 'country') {
        $country = _gmaps_address_get_country_options();
        
        $result = $country[$this->options['gmaps']['country']];
        
        $countryicons = module_exists('countryicons');
        $languageicons = module_exists('languageicons');
        if ($countryicons || $languageicons) {
          $flag = _gmaps_address_get_flag_options();
          $result .= ' - '. $flag[$this->options['gmaps']['flag']];
        }
      }
      else {
        $state = _gmaps_address_get_state_options();
        
        $result = $state[$this->options['gmaps']['state']];
      }
      return $result;
    }
  }

  function render($values) {
    global $user;
    static $map, $included, $countryicons, $languageicons;
    
    $value = $values->{$this->field_alias};
    if (empty($value)) {
      return '';
    }
    
    $part = $this->definition['gmaps_address_part'];
    if ($part == 'thoroughfare') {
      return check_plain($value);
    }
    else {
      //privacy
      if ($this->options['gmaps']['apply_privacy']) {
        $privacy = $values->{$this->aliases['privacy']};
        do {
          if (empty($privacy) || user_access('access protected gmaps data')) {
            break;
          }
        
          if (is_null($map)) {
            $map = _gmaps_address_get_parts();
            $map = array_keys(array_reverse($map));
          }
          
          $i = array_search($part, $map);
          if ($i < $privacy) {
            return '';
          }
        } while(0);
      }
      
      if (in_array($part, array('country', 'adminarea'))) {
        if (!$included) {
          $included = TRUE;
          module_load_include('inc', 'gmaps', 'includes/gmaps.address');
          $countryicons = module_exists('countryicons');
          $languageicons = module_exists('languageicons');
        }
        $s = $this->options['gmaps']['user_settings'] ? $user->gmaps_settings['address'] : $this->options['gmaps'];
        if ($part == 'country') {
          $value = _gmaps_address_format_country($value, $s['country']);
          if ($countryicons || $languageicons) {
            $value = _gmaps_address_format_country_flag($value, $values->{$this->field_alias}, $s['flag']);
          }
        }
        else {
          $country = $values->{$this->aliases['country']};
          $adr = (object)array('country' => $values->{$this->aliases['country']}, 'adminarea' => $value);
          $value = _gmaps_address_format_state($adr, $s['state']);
        }
      }
      else if ($part == 'postalcode') {
        $value = check_plain($value);
      }
      else {
        $value = check_plain(t($value));
      }
      
      return $this->render_link($value, $values);
    }
  }

}