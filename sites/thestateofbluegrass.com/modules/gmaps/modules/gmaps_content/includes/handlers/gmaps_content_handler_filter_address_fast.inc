<?php
// $Id: gmaps_content_handler_filter_address_fast.inc,v 1.1.2.3 2010/03/10 13:52:18 xmarket Exp $

class gmaps_content_handler_filter_address_fast extends views_handler_filter_string {
  protected $helper;
  
  function init(&$view, $options) {
    parent::init($view, $options);
    
    
    $this->helper = new gmaps_content_address_fast_filter_helper($this);
  }
  
  
  function option_definition() {
    $options = parent::option_definition();

    $options['case'] = array('default' => FALSE);
    $options['value'] = array('default' => array());
    $options['allow_country'] = array('default' => TRUE);
    $options['allow_adminarea'] = array('default' => TRUE);
    
    _gmaps_content_address_fast_filter_option_definition($options);
                
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['case'] = array('#type' => 'value', '#value' => FALSE);
    
    $form['allow_country'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow country selector'),
      '#default_value' => $this->options['allow_country'],
      '#description' => t('If selected, a dropdown list will be displayed below the input field. This option is designed to use with the "Address parts" argument and its modifiers. These selectors can be hidden, if those arguments are added to the view.'),
    );
    $form['allow_adminarea'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow administrative area selector'),
      '#default_value' => $this->options['allow_adminarea'],
      '#description' => t('If selected, an extra input field (or a dropdown list for US, CA) will be displayed below the country selector.'),
    );
    _gmaps_content_address_fast_filter_options_form($form, $form_state, $this);
  }

  function options_submit($form, &$form_state) {
    if ($this->options['allow_country'] != $form_state['values']['options']['allow_country']) {
      $display_id = ($this->view->display_handler->is_defaulted('filters')) ? 'default' : $this->view->current_display;
  
      // shortcut test.
      $operator = !empty($this->options['expose']['use_operator']) && !empty($this->options['expose']['operator']);
  
      // false means that we got a setting that means to recuse ourselves,
      // so we should erase whatever happened to be there.
      if (isset($_SESSION['views'][$this->view->name][$display_id])) {
        $session = &$_SESSION['views'][$this->view->name][$display_id];
        if ($operator && isset($session[$this->options['expose']['operator']])) {
          unset($session[$this->options['expose']['operator']]);
        }
  
        if (isset($session[$this->options['expose']['identifier']])) {
          unset($session[$this->options['expose']['identifier']]);
        }
      }
    }
    parent::options_submit($form, $form_state);
  }
  
  function expose_options() {
    parent::expose_options();
    $this->options['expose']['auto_label'] = TRUE;
  }
  
  function expose_form_left(&$form, &$form_state) {
    parent::expose_form_left($form, $form_state);
    $form['expose']['auto_label'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use input field dependent label'),
      '#default_value' => !empty($this->options['expose']['auto_label']),
    );
  }
  
  function exposed_translate(&$form, $type) {
    if ($type != 'value') {
      return parent::exposed_translate($form, $type);
    }
    
    if (!empty($this->options['allow_country'])) {
      unset($form['locality']['#title']);
    }
  }

  function accept_exposed_input($input) {
    if (empty($this->options['exposed'])) {
      return TRUE;
    }


    if (!empty($this->options['expose']['use_operator']) && !empty($this->options['expose']['operator']) && isset($input[$this->options['expose']['operator']])) {
      $this->operator = $input[$this->options['expose']['operator']];
    }

    if (!empty($this->options['expose']['identifier'])) {
      $value = $input[$this->options['expose']['identifier']];
      
      // Various ways to check for the absence of optional input.
      if (!empty($this->options['expose']['optional']) && !in_array($this->operator, $this->operator_values(0))) {
        //
        if (empty($value) || $value['country'] == 'All') {
          return FALSE;
        }
      }


      if (isset($value)) {
        $this->value = $value;
      }
      else {
        return FALSE;
      }
    }

    return TRUE;
  }

  function operators() {
    $operators = parent::operators();
    $operators['!=']['method'] = 'op_not_equal';

    return $operators;
  }

  function exposed_info() {
    if (empty($this->options['exposed'])) {
      return;
    }

    return array(
      'operator' => $this->options['expose']['operator'],
      'value' => $this->options['expose']['identifier'],
      'label' => !empty($this->options['expose']['auto_label']) ? t('Search by !locality_fields', array('!locality_fields' => $this->helper->locality_title())) : $this->options['expose']['label'],
    );
  }
  
  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    $options = $this->operator_options('short');
    $output = check_plain($options[$this->operator]);
    if (in_array($this->operator, $this->operator_values(1))) {
      $adr = (object)array(
        'country' => $this->value['country'],
        'adminarea' => $this->value['adminarea'],
        'locality' => $this->value['locality'],
      );
      if ($this->value['country'] == 'US') {
        $adr->adminarea = $this->value['state'];
      }
      else if ($this->value['country'] == 'CA') {
        $adr->adminarea = $this->value['province'];
      }
      $output .= ' ' . gmaps_address_format_plain($adr, NULL, NULL, TRUE);
    }
    return $output;
  }

  function value_form(&$form, &$form_state) {
    // We have to make some choices when creating this as an exposed
    // filter form. For example, if the operator is locked and thus
    // not rendered, we can't render dependencies; instead we only
    // render the form items we need.
    $which = 'all';
    $country_source = 'edit-options-value-country';
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];
      $country_source = 'edit-' . form_clean_id($identifier) .'-country';
      
      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator'])) {
        // exposed and locked.
        $which = in_array($this->operator, $this->operator_values(1)) ? 'value' : 'none';
      }
      else {
        $source = 'edit-' . form_clean_id($this->options['expose']['operator']);
      }
    }

    if ($which == 'all' || $which == 'value') {
      $form['value'] = array('#tree' => TRUE);
      
      $form['value']['locality'] = array(
        '#type' => 'textfield',
        '#title' => $this->helper->locality_title(),
        '#size' => 30,
        '#default_value' => $this->value['locality'],
      );
      
      $labels = gmaps_address_get_part_labels();
      $names = gmaps_get_countries('names');
      
      $any_label = variable_get('views_exposed_filter_any_label', 'old_any') == 'old_any' ? t('<Any>') : t('- Any -');
      
      $form['value']['country'] = array(
        '#type' => 'select',
        '#title' => $labels['country'],
        '#default_value' => $this->value['country'],
        '#options' => $this->helper->_get_country_values(),
        '#access' => empty($form_state['exposed']) || !empty($this->options['allow_country']),
      );
      if (!empty($this->options['expose']['optional'])) {
        $form['value']['country']['#options'] = array('All' => $any_label) + $form['value']['country']['#options'];
        $form['value']['country']['#default_value'] = empty($form['value']['country']['#default_value']) ? 'All' : $form['value']['country']['#default_value'];
      }
      
      $allow_adminarea = empty($form_state['exposed']) || (!empty($this->options['allow_country']) && !empty($this->options['allow_adminarea']));
      $form['value']['state'] = array(
        '#type' => 'select',
        '#title' => $labels['adminarea'] .' - '. $names['US'],
        '#default_value' => $this->value['state'],
        '#options' => array('All' => $any_label) + array_map('drupal_ucfirst', geonames_us_states()),
        '#process' => array('views_process_dependency'),
        '#dependency' => array($country_source => array('US')),
        '#access' => $allow_adminarea,
      );
      
      $form['value']['province'] = array(
        '#type' => 'select',
        '#title' => $labels['adminarea'] .' - '. $names['CA'],
        '#default_value' => $this->value['province'],
        '#options' => array('All' => $any_label) + array_map('drupal_ucfirst', gmaps_canadian_provinces()),
        '#process' => array('views_process_dependency'),
        '#dependency' => array($country_source => array('CA')),
        '#access' => $allow_adminarea,
      );
      
      unset($names['US'], $names['CA']);
      $form['value']['adminarea'] = array(
        '#type' => 'textfield',
        '#title' => $labels['adminarea'],
        '#size' => 30,
        '#default_value' => $this->value['adminarea'],
        '#process' => array('views_process_dependency'),
        '#dependency' => array($country_source => array_keys($names)),
        '#access' => $allow_adminarea,
      );
      
      if (empty($form_state['exposed'])) {
        $form['value']['country'] += array(
          '#process' => array('views_process_dependency'),
          '#dependency' => array('edit-options-allow-country' => array(TRUE)),
        );
        
        $form['value']['state']['#dependency'] += array(
          'edit-options-allow-country' => array(TRUE),
          'edit-options-allow-adminarea' => array(TRUE),
        );
        $form['value']['state']['#dependency_count'] = 3;
        
        $form['value']['province']['#dependency'] += array(
          'edit-options-allow-country' => array(TRUE),
          'edit-options-allow-adminarea' => array(TRUE),
        );
        $form['value']['province']['#dependency_count'] = 3;
        
        $form['value']['adminarea']['#dependency'] += array(
          'edit-options-allow-country' => array(TRUE),
          'edit-options-allow-adminarea' => array(TRUE),
        );
        $form['value']['adminarea']['#dependency_count'] = 3;
      }
      
      if (!empty($form_state['exposed']) && !isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }

      if ($which == 'all') {
        if (empty($form_state['exposed']) || !empty($this->options['allow_country'])) {
          if (empty($form_state['exposed'])) {
            $form['value']['country']['#dependency'] += array(
               $source => $this->operator_values(1),
            );
            $form['value']['country']['#dependency_count'] = 2;
          }
          else {
            $form['value']['country'] += array(
              '#process' => array('views_process_dependency'),
              '#dependency' => array($source => $this->operator_values(1)),
            );
          }
          
          $form['value']['state']['#dependency'][$source] = $this->operator_values(1);
          $form['value']['state']['#dependency_count'] = empty($form_state['exposed']) ? 4 : 2;
          
          $form['value']['province']['#dependency'][$source] = $this->operator_values(1);
          $form['value']['province']['#dependency_count'] = empty($form_state['exposed']) ? 4 : 2;
          
          $form['value']['adminarea']['#dependency'][$source] = $this->operator_values(1);
          $form['value']['adminarea']['#dependency_count'] = empty($form_state['exposed']) ? 4 : 2;
        }
        
        $form['value']['locality'] += array(
          '#process' => array('views_process_dependency'),
          '#dependency' => array($source => $this->operator_values(1)),
        );
      }
    }

    if (!isset($form['value'])) {
      // Ensure there is something in the 'value'.
      $form['value'] = array(
        '#type' => 'value',
        '#value' => array(),
      );
    }
  }

  function case_transform() {
    return '';
  }

  function query() {
    $this->ensure_my_table();
    
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->helper->{$info[$this->operator]['method']}($this->value, $this->table_alias);
    }
  }
  
}
