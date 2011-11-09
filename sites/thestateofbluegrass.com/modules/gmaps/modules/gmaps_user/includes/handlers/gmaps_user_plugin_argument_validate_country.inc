<?php
// $Id: gmaps_user_plugin_argument_validate_country.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $
/**
 * @file
 * Contains the 'gmaps user country' argument validator plugin.
 */

/**
 * Validate whether an argument is an acceptable country.
 */
class gmaps_user_plugin_argument_validate_country extends views_plugin_argument_validate {
  var $option_name = 'validate_argument_gmaps_user_country';

  function validate_form(&$form, &$form_state) {
    $continents = geonames_continent_codes('short');
    foreach ($continents as $continent => $info) {
      $options[$continent] = check_plain(t($info['name']));
    }

    $arg = $this->get_argument();
    if (empty($arg)) {
      $arg = array();
    }

    $form[$this->option_name] = array(
      '#type' => 'checkboxes',
      '#prefix' => '<div id="edit-options-validate-argument-gmaps-user-country-wrapper">',
      '#suffix' => '</div>',
      '#title' => t('Continents'),
      '#options' => $options,
      '#default_value' => $arg,
      '#description' => t('If you wish to validate for specific continents, check them; if none are checked, all continents will pass.'),
      '#process' => array('expand_checkboxes', 'views_process_dependency'),
      '#dependency' => array('edit-options-validate-type' => array($this->id)),
    );

    $form['validate_argument_country_code_type'] = array(
      '#type' => 'select',
      '#title' => t('Argument type'),
      '#options' => array(
        '' => t('All formats'),
        'iso2' => t('ISO 3166-1 alpha-2 code only'),
        'iso3' => t('ISO 3166-1 alpha-3 code only'),
        'iso2_3' => t('ISO 3166-1 aplha-2 and alpha-3 code'),
        'name' => t('Name only'),
      ),
      '#default_value' => isset($this->argument->options['validate_argument_country_code_type']) ? $this->argument->options['validate_argument_country_code_type'] : '',
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-validate-type' => array($this->id)),
    );
  }

  function validate_argument($argument) {
    $continents = array_filter($this->argument->options[$this->option_name]);

    $type = isset($this->argument->options['validate_argument_country_code_type']) ? $this->argument->options['validate_argument_country_code_type'] : '';
    
    $code = gmaps_address_get_country_code($argument);
    
    if (empty($code)) {
      return FALSE;
    }

    $len = strlen($argument);
    if ($type == 'iso2' && $len != 2) {
      return FALSE;
    }
    else if ($type == 'iso3' && $len != 3) {
      return FALSE;
    }
    else if ($type == 'iso2_3' && ($len < 2 || $len > 3)) {
      return FALSE;
    }
    else if ($type == 'name' && $len <= 3) {
      return FALSE;
    }
    
    $country = gmaps_get_countries('country', $code);
    
    if (!empty($continents)) {
      if (empty($continents[$country['continent']])) {
        return FALSE;
      }
    }
    
    $this->argument->argument = $code;
    $this->argument->validated_title = check_plain(t($country['name']));
    return TRUE;
  }
}

