<?php
// $Id: gmaps_user_handler_field_address_part.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

class gmaps_user_handler_field_address_part extends views_handler_field_user {
  const ADDRESS_LINK_TO_USER = 1;
  const ADDRESS_LINK_TO_PROFILE_LIST = 2;

  function init(&$view, $options) {
    parent::init($view, $options);
    
    $this->additional_fields['privacy'] = 'privacy';
    
    if ($this->definition['field'] != 'thoroughfare' && $this->options['gmaps']['link_to'] == self::ADDRESS_LINK_TO_PROFILE_LIST) {
      $parts = _gmaps_address_get_parts();
      unset($parts[$this->definition['field']], $parts['thoroughfare']);
      
      foreach($parts as $part => $label) {
        $this->additional_fields[$part] = $part;
      }
    }
    else if ($this->definition['field'] == 'adminarea') {
      $this->additional_fields['country'] = 'country';
    }
  }

  function options(&$options) {
    parent::options($options);

    $labels = gmaps_address_get_part_labels();
    $options['label'] = $labels[$this->definition['field']];
    
    $options['link_to_user'] = 0;
    
    $options['gmaps'] = array(
      'apply_privacy' => TRUE,
      'link_to' => 0,
      'user_settings' => TRUE,
      'country' => 'name',
      'state' => '',
      'flag' => '',
    );
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $countryicons = module_exists('countryicons');
    $gmaps_profile = module_exists('gmaps_profile');
    //partially supported
    $languageicons = module_exists('languageicons');
    
    $options = $this->options;
    $gmaps = $options['gmaps'];

    $form['link_to_user'] = array('#type' => 'value', '#value' => 0);
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
    
    $link_to = array(t('Disabled'), t('User'));
    if ($gmaps_profile) {
      $link_to[] = t('Profile list');
    }
    $form['gmaps']['link_to'] = array('#type' => 'select',
      '#title' => t('Link this field to'),
      '#description' => t('This will override any other link path you have set.'),
      '#default_value' => $gmaps['link_to'],
      '#options' => $link_to,
    );
    
    if (in_array($this->definition['field'], array('country', 'adminarea'))) {
      module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
      
      $form['gmaps']['user_settings'] = array('#type' => 'checkbox',
        '#title' => t('Use user/site settings'),
        '#default_value' => $gmaps['user_settings'],
      );
      
      if ($this->definition['field'] == 'country') {
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
    if (in_array($this->definition['field'], array('country', 'adminarea'))) {
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
    if (in_array($this->definition['field'], array('country', 'adminarea'))) {
      if ($this->options['gmaps']['user_settings']) {
        return t('User/site settings');
      }
      module_load_include('inc', 'gmaps', 'includes/gmaps.admin');
      if ($this->definition['field'] == 'country') {
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
    
    $part = $this->definition['field'];
    if ($part == 'thoroughfare') {
      return check_plain($value);
    }
    else {
      //privacy
      if ($this->options['gmaps']['apply_privacy']) {
        $privacy = $values->{$this->aliases['privacy']};
        $uid = $values->uid;
        do {
          if (empty($privacy) || (!empty($uid) && $uid == $user->uid) || user_access('access protected gmaps data')) {
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
      
      if ($this->options['gmaps']['link_to']) {
        if ($this->options['gmaps']['link_to'] == self::ADDRESS_LINK_TO_USER) {
          return $this->render_link($value, $values);
        }
        else {
          return $this->render_link_profile_list($value, $values);
        }
      }
      
      return $value;
    }
  }

  function render_link($data, $values) {
    if ($this->options['gmaps']['link_to'] == self::ADDRESS_LINK_TO_USER && $data !== NULL && $data !== '') {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "user/" . $values->uid;
      $this->options['alter']['alt'] = check_plain($data);
    }
    return $data;
  }
  
  /**
   * Render whatever the data is as a link to profile list.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link_profile_list($data, $values) {
    if ($data !== NULL && $data !== '' && $this->definition['field'] != 'thoroughfare' && $this->options['gmaps']['link_to'] == self::ADDRESS_LINK_TO_PROFILE_LIST && module_exists('gmaps_profile')) {
      $adr = $this->_build_item_from_values($values);
      /*if ($adr && $pathes = gmaps_taxonomy_location_pathes($adr, $this->definition['field'])) {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = $pathes[$this->definition['field']];
        $this->options['alter']['alt'] = check_plain($data);
      }*/
    }
    return $data;
  }

  private function _build_item_from_values($values) {
    if ($this->options['gmaps']['link_to'] != self::ADDRESS_LINK_TO_PROFILE_LIST) {
      return NULL;
    }
    
    $address = new stdClass();
    $address->{$this->definition['field']} = $values->{$this->field_alias};
    foreach(_gmaps_address_get_parts() as $part => $label) {
      if ($this->aliases[$part]) {
        $value = $values->{$this->aliases[$part]};
        $address->$part = $value;
      }
    }
    $address->uid = $values->uid;
    
    if (gmaps_address_empty($address)) {
      return NULL;
    }
    
    if ($this->options['gmaps']['apply_privacy']) {
      $address->privacy = $values->{$this->aliases['privacy']};
      gmaps_address_apply_privacy($address);
    }
    
    return $address;
  }

}