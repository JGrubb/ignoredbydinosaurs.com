<?php
// $Id: gmaps_taxonomy_plugin_argument_validate_location_term.inc,v 1.1.2.1 2009/11/23 13:23:48 xmarket Exp $
/**
 * @file
 * Contains the 'gmaps country' argument validator plugin.
 */

/**
 * Validate whether an argument is an acceptable country.
 */
class gmaps_taxonomy_plugin_argument_validate_location_term extends views_plugin_argument_validate {
  var $option_name = 'validate_argument_gmaps_location_term';

  function validate_form(&$form, &$form_state) {
    $arg = $this->get_argument();
    if ($arg === '') {
      $arg = TRUE;
    }

    $form[$this->option_name] = array('#type' => 'checkbox',
      '#title' => t('Allow multiple location terms separated by | or :'),
      '#default_value' => $arg,
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-validate-type' => array($this->id)),
    );

    $form['validate_argument_gmaps_location_continent'] = array('#type' => 'checkbox',
      '#title' => t('Allow continent terms'),
      '#default_value' => isset($this->argument->options['validate_argument_gmaps_location_continent']) ? $this->argument->options['validate_argument_gmaps_location_continent'] : TRUE,
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-validate-type' => array($this->id)),
    );
  }

  function validate_argument($argument) {
    $terms = $this->argument->options[$this->option_name];
    $continent = isset($this->argument->options['validate_argument_gmaps_location_continent']) ? $this->argument->options['validate_argument_gmaps_location_continent'] : TRUE;
    
    $filter = gmaps_taxonomy_break_phrase($argument);
    
    if (isset($filter->continent)) {
      if (!$continent) {
        return FALSE;
      }
    }
    else {
      if (count($filter->value) > 1 && !$terms) {
        return FALSE;
      }
      if (empty($filter->value)) {
        return FALSE;
      }
    }
    
    return TRUE;
  }
}

