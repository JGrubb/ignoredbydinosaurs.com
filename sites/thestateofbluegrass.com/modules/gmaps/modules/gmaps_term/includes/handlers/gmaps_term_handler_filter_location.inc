<?php
// $Id: gmaps_term_handler_filter_location.inc,v 1.1.2.1 2010/04/01 07:42:10 xmarket Exp $

class gmaps_term_handler_filter_location extends views_handler_filter {
  var $no_single = TRUE;
  protected $helper;
  protected $address;
  
  function init(&$view, $options) {
    parent::init($view, $options);
    
    $this->address = !empty($this->definition['allow address']);
    if ($this->address) {
      $this->helper = new gmaps_term_address_fast_filter_helper($this);
    }
  }
  
  function option_definition() {
    $options = parent::option_definition();

    $options['value'] = array('default' => array());
    $options['allow_country'] = array('default' => FALSE);
    $options['allow_adminarea'] = array('default' => FALSE);
    
    _gmaps_term_address_fast_filter_option_definition($options);
            
    $options['geocoder'] = array('default' => TRUE);
    $options['use_map'] = array('default' => TRUE);
    
    $options['rounded'] = array('default' => FALSE);
    
    $options['viewloc'] = array('default' => TRUE);
    
    $options['radius'] = array(
      'contains' => array(
        0 => array('default' => array('value' => 1, 'unit' => 'kilometer')),
        1 => array('default' => array('value' => 5, 'unit' => 'kilometer')),
        2 => array('default' => array('value' => 10, 'unit' => 'kilometer')),
        3 => array('default' => array('value' => 0, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)),
        4 => array('default' => array('value' => 0, 'unit' => GMAPS_DEFAULT_LENGTH_UNIT)),
      ),
    );
    $options['radius_m'] = array(
      'contains' => array(
        0 => array('default' => 1000),
        1 => array('default' => 5000),
        2 => array('default' => 10000),
        3 => array('default' => 0),
        4 => array('default' => 0),
      ),
    );
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    if ($this->address) {
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
      _gmaps_term_address_fast_filter_options_form($form, $form_state, $this);
    }
    else {
      $form['allow_country'] = array('#type' => 'value', '#value' => FALSE);
      $form['allow_adminarea'] = array('#type' => 'value', '#value' => FALSE);
      $form['fast_filter_fields'] = array('#type' => 'value', '#value' => GMAPS_TERM_FAST_FILTER_LOC);
    }
    
    $form['geocoder'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable geocoder'),
      '#default_value' => $this->options['geocoder'],
    );
    $form['use_map'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use input map'),
      '#default_value' => $this->options['use_map'],
    );
    
    $form['rounded'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use rounded distance filter'),
      '#default_value' => $this->options['rounded'],
    );
    
    $form['viewloc'] = array(
      '#type' => 'checkbox',
      '#title' => t('Set view location'),
      '#default_value' => $this->options['viewloc'],
    );
    
    $form['radius'] = array(
      '#type' => 'fieldset',
      '#title' => t('Radius operator options'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    for($i = 0; $i < 5; $i++) {
      $form['radius'][$i] = array(
        '#type' => 'gmaps_distance',
        '#title' => t('Radius !i', array('!i' => $i + 1)),
        '#default_value' => $this->options['radius'][$i],
      );
    }
  }

  function options_validate($form, &$form_state) {
    parent::options_validate($form, $form_state);
    
    foreach($form_state['values']['options']['radius'] as $i => $length) {
      if (!gmaps_length_empty($length) && !gmaps_length_convert($length, GMAPS_DEFAULT_LENGTH_UNIT)) {
        form_error($form['radius'][$i], t('Unable to convert radius to the default length unit.'));
      }
    }
  }
  
  function options_submit($form, &$form_state) {
    if ($this->address && $this->options['allow_country'] != $form_state['values']['options']['allow_country']) {
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
    foreach($form_state['values']['options']['radius'] as $i => $length) {
      $form_state['values']['options']['radius_m'][$i] = gmaps_length_empty($length) ? 0 : gmaps_length_convert($length, GMAPS_DEFAULT_LENGTH_UNIT);
    }
    parent::options_submit($form, $form_state);
  }
  
  function operators() {
    global $user;
    $operators = array();
    
    if ($this->address) {
      $operators['address'] = array(
        'title' => t('Within address'),
        'short' => t('address'),
        'values' => 1,
      );
      $operators['not address'] = array(
        'title' => t('Not within address'),
        'short' => t('!address'),
        'values' => 1,
      );
    }
    $operators['bounds'] = array(
      'title' => t('Within bounds'),
      'short' => t('bounds'),
      'method' => 'op_bounds',
      'values' => 2,
    );
    $operators['not bounds'] = array(
      'title' => t('Not within bounds'),
      'short' => t('!bounds'),
      'method' => 'op_bounds',
      'values' => 2,
    );
    $s = $user->gmaps_settings['length'];
    $s['formatter'] = 'plain';
    foreach($this->options['radius'] as $i => $length) {
      if (!gmaps_length_empty($length)) {
        $label = gmaps_length_render($length, $s);
        $label_plain = str_replace(array('<span class="gmaps-length-plain">', '</span>'), '', $label);
        $operators["R$i"] = array(
          'title' => t('Radius <= !length', array('!length' => $label_plain)),
          'short' => t('R<=!length', array('!length' => $label)),
          'method' => 'op_distance',
          'values' => 2,
        );
        $operators["not R$i"] = array(
          'title' => t('Radius > !length', array('!length' => $label_plain)),
          'short' => t('R>!length', array('!length' => $label)),
          'method' => 'op_distance',
          'values' => 2,
        );
      }
    }
    // if the definition allows for the empty operator, add it.
    if (!empty($this->definition['allow empty'])) {
      $operators += array(
        'empty' => array(
          'title' => t('Is empty (NULL)'),
          'method' => 'op_empty',
          'short' => t('empty'),
          'values' => 0,
        ),
        'not empty' => array(
          'title' => t('Is not empty (NULL)'),
          'method' => 'op_empty',
          'short' => t('not empty'),
          'values' => 0,
        ),
      );
    }

    return $operators;
  }

  function operator_options($which = 'title') {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }

    return $options;
  }

  function operator_values($values = 1) {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      if (isset($info['values']) && $info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    $options = $this->operator_options('short');
    $output = check_plain($options[$this->operator]);
    if (in_array($this->operator, $this->operator_values(1))) {
      $adr = (object)array(
        'country' => $this->value['address']['country'],
        'adminarea' => $this->value['address']['adminarea'],
        'locality' => $this->value['address']['locality'],
      );
      if ($this->value['address']['country'] == 'US') {
        $adr->adminarea = $this->value['address']['state'];
      }
      else if ($this->value['address']['country'] == 'CA') {
        $adr->adminarea = $this->value['address']['province'];
      }
      $output .= ' ' . gmaps_address_format_plain($adr, NULL, NULL, TRUE);
    }
    return $output;
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
      if (!empty($this->options['expose']['optional'])) {
        if (in_array($this->operator, $this->operator_values(1))) {
          if ($value['address']['country'] == 'All') {
            return FALSE;
          }
        }
        else if (!in_array($this->operator, $this->operator_values(0)) && gmaps_point_empty($value['pw']['point'])) {
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

  function value_form(&$form, &$form_state) {
    // We have to make some choices when creating this as an exposed
    // filter form. For example, if the operator is locked and thus
    // not rendered, we can't render dependencies; instead we only
    // render the form items we need.
    $which = 'all';
    $country_source = 'edit-options-value-address-country';
    
    $wrapper_id = 'edit-options-value-pw';
    $limited = TRUE;
    
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }
    
    if (!empty($form_state['exposed'])) {
      $limited = $this->view->live_preview;
      $identifier = $this->options['expose']['identifier'];
      $clean_id = form_clean_id($identifier);
      $country_source = 'edit-' . $clean_id .'-address-country';
      $wrapper_id = 'edit-' . $clean_id .'-pw';
      
      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator'])) {
        // exposed and locked.
        if (in_array($this->operator, $this->operator_values(0))) {
          $which = 'none';
        }
        else if (in_array($this->operator, $this->operator_values(1))) {
          $which = 'address';
        }
        else {
          $which = 'point';
        }
      }
      else {
        $source = 'edit-' . form_clean_id($this->options['expose']['operator']);
      }
    }

    if ($which == 'all' || $which == 'address' || $which == 'point') {
      $form['value'] = array('#tree' => TRUE);
      
      if ($which != 'point') {
        $this->_value_form_address($form, $form_state, $country_source);
      }
      
      if ($which != 'address') {
        //point wrapper to let views dependency work
        $form['value']['pw'] = array(
          '#type' => 'item',
          '#input' => TRUE,
          '#id' => $wrapper_id,
          '#attributes' => array('id' => $wrapper_id),
        );
        $form['value']['pw']['point'] = array(
          '#type' => 'gmaps_point',
          '#title' => t('Point'),
          '#default_value' => $this->value['pw']['point'],
          '#enable_geocoder' => $limited ? FALSE : !empty($this->options['geocoder']),
          '#enable_bounds' => TRUE,
          '#use_map' => $limited ? FALSE : !empty($this->options['use_map']),
          '#enable_3D' => FALSE,
          '#enable_privacy' => FALSE,
          '#enable_pov' => FALSE,
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#map_type' => $this->definition['gmaps_map_type'],
          '#enable_tabs' => !$limited,
        );
      }
      
      if (!empty($form_state['exposed'])) {
        if (!isset($form_state['input'][$identifier])) {
          $form_state['input'][$identifier] = $this->_value_to_input($this->value);
        }
        else {
          $point = &$form_state['input'][$identifier]['pw']['point'];
          $point['tabs']['coords']['map_type'] = $this->definition['gmaps_map_type'];
          if (!empty($point['tabs']['bounds'])) {
            $point['tabs']['bounds']['northeast']['tabs']['coords']['map_type'] = $this->definition['gmaps_map_type'];
            $point['tabs']['bounds']['southwest']['tabs']['coords']['map_type'] = $this->definition['gmaps_map_type'];
          }
        }
      }

      if ($which == 'all') {
        if ($this->address) {
          if (empty($form_state['exposed']) || !empty($this->options['allow_country'])) {
            if (empty($form_state['exposed'])) {
              $form['value']['address']['country']['#dependency'] += array(
                 $source => $this->operator_values(1),
              );
              $form['value']['address']['country']['#dependency_count'] = 2;
            }
            else {
              $form['value']['address']['country'] += array(
                '#process' => array('views_process_dependency'),
                '#dependency' => array($source => $this->operator_values(1)),
              );
            }
            
            $form['value']['address']['state']['#dependency'][$source] = $this->operator_values(1);
            $form['value']['address']['state']['#dependency_count'] = empty($form_state['exposed']) ? 4 : 2;
            
            $form['value']['address']['province']['#dependency'][$source] = $this->operator_values(1);
            $form['value']['address']['province']['#dependency_count'] = empty($form_state['exposed']) ? 4 : 2;
            
            $form['value']['address']['adminarea']['#dependency'][$source] = $this->operator_values(1);
            $form['value']['address']['adminarea']['#dependency_count'] = empty($form_state['exposed']) ? 4 : 2;
          }
          
          $form['value']['address']['locality'] += array(
            '#process' => array('views_process_dependency'),
            '#dependency' => array($source => $this->operator_values(1)),
          );
        }
        
        $form['value']['pw'] += array(
          '#process' => array('views_process_dependency'),
          '#dependency' => array($source => $this->operator_values(2)),
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

  private function _value_form_address(&$form, &$form_state, $country_source) {
    if (!$this->address) {
      return;
    }
    
    $form['value']['address'] = array();
    $form['value']['address']['locality'] = array(
      '#type' => 'textfield',
      '#title' => $this->helper->locality_title(),
      '#size' => 30,
      '#default_value' => $this->value['address']['locality'],
    );
    
    $labels = gmaps_address_get_part_labels();
    $names = gmaps_get_countries('names');
    
    $any_label = variable_get('views_exposed_filter_any_label', 'old_any') == 'old_any' ? t('<Any>') : t('- Any -');
    
    $form['value']['address']['country'] = array(
      '#type' => 'select',
      '#title' => $labels['country'],
      '#default_value' => $this->value['address']['country'],
      '#options' => $this->helper->_get_country_values(),
      '#access' => empty($form_state['exposed']) || !empty($this->options['allow_country']),
    );
    if (!empty($this->options['expose']['optional'])) {
      $form['value']['address']['country']['#options'] = array('All' => $any_label) + $form['value']['address']['country']['#options'];
      $form['value']['address']['country']['#default_value'] = empty($form['value']['address']['country']['#default_value']) ? 'All' : $form['value']['address']['country']['#default_value'];
    }
    
    $allow_adminarea = empty($form_state['exposed']) || (!empty($this->options['allow_country']) && !empty($this->options['allow_adminarea']));
    $form['value']['address']['state'] = array(
      '#type' => 'select',
      '#title' => $labels['adminarea'] .' - '. $names['US'],
      '#default_value' => $this->value['address']['state'],
      '#options' => array('All' => $any_label) + array_map('drupal_ucfirst', geonames_us_states()),
      '#process' => array('views_process_dependency'),
      '#dependency' => array($country_source => array('US')),
      '#access' => $allow_adminarea,
    );
    
    $form['value']['address']['province'] = array(
      '#type' => 'select',
      '#title' => $labels['adminarea'] .' - '. $names['CA'],
      '#default_value' => $this->value['address']['province'],
      '#options' => array('All' => $any_label) + array_map('drupal_ucfirst', gmaps_canadian_provinces()),
      '#process' => array('views_process_dependency'),
      '#dependency' => array($country_source => array('CA')),
      '#access' => $allow_adminarea,
    );
    
    unset($names['US'], $names['CA']);
    $form['value']['address']['adminarea'] = array(
      '#type' => 'textfield',
      '#title' => $labels['adminarea'],
      '#size' => 30,
      '#default_value' => $this->value['address']['adminarea'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array($country_source => array_keys($names)),
      '#access' => $allow_adminarea,
    );
    
    if (empty($form_state['exposed'])) {
      $form['value']['address']['country'] += array(
        '#process' => array('views_process_dependency'),
        '#dependency' => array('edit-options-allow-country' => array(TRUE)),
      );
      
      $form['value']['address']['state']['#dependency'] += array(
        'edit-options-allow-country' => array(TRUE),
        'edit-options-allow-adminarea' => array(TRUE),
      );
      $form['value']['address']['state']['#dependency_count'] = 3;
      
      $form['value']['address']['province']['#dependency'] += array(
        'edit-options-allow-country' => array(TRUE),
        'edit-options-allow-adminarea' => array(TRUE),
      );
      $form['value']['address']['province']['#dependency_count'] = 3;
      
      $form['value']['address']['adminarea']['#dependency'] += array(
        'edit-options-allow-country' => array(TRUE),
        'edit-options-allow-adminarea' => array(TRUE),
      );
      $form['value']['address']['adminarea']['#dependency_count'] = 3;
    }
  }
  
  private function _value_to_input($input) {
    if (!is_array($input)) {
      return array();
    }
    if (empty($input)) {
      return $input;
    }
    $input['pw']['point'] = $this->_point_to_input_tree($input['pw']['point']);
    return $input;
  }
  
  private function _point_to_input_tree($point) {
    $result = array(
      'tabs' => array(
        'coords' => array(
          'latitude' => $point->latitude,
          'longitude' => $point->longitude,
          'elevation' => $point->elevation,
          'map_type' => $point->map_type,
        ),
      ),
    );
    if (!empty($point->bounds)) {
      $result['tabs']['bounds'] = array(
        'northeast' => $this->_point_to_input_tree($point->bounds['northeast']),
        'southwest' => $this->_point_to_input_tree($point->bounds['southwest']),
      );
    }
    return $result;
  }
  
  function store_exposed_input($input, $status) {
    if ($status) {
      $input[$this->options['expose']['identifier']] = $this->_value_to_input($input[$this->options['expose']['identifier']]);
    }
    return parent::store_exposed_input($input, $status);
  }

  function query() {
    $this->ensure_my_table();

    if ($this->address) {
      $a = $this->query->ensure_table($this->definition['gmaps_address_table'], $this->relationship);
      if ($a) {
        if (!array_key_exists('gtanp', $this->view->query->where)) {
          $db_field = $this->query->table_queue[$this->table]['join']->field;
          $p = $this->table_alias;
          $sql = "($p.$db_field IS NULL OR ($p.$db_field = $a.$db_field))";
          $this->view->query->add_where('gtanp', $sql);
        }
      }
    }

    if (in_array($this->operator, $this->operator_values(1))) {
      if ($a) {
        if (strpos($this->operator, 'not') === 0) {
          $this->helper->op_not_equal($this->value['address'], $a);
        }
        else {
          $this->helper->op_equal($this->value['address'], $a);
        }
      }
    }
    else {
      $value = $this->value['pw']['point'];
      
      $info = $this->operators();
      if (!empty($info[$this->operator]['method'])) {
        $this->{$info[$this->operator]['method']}($value);
        
        if ($this->options['viewloc'] && !empty($info[$this->operator]['values'])) {
          $this->view->gmaps_viewloc = (object)array('point' => $value);
        }
      }
    }
  }

  function op_bounds($point) {
    $not = (strpos($this->operator, 'not') === 0);
    $piece = gmaps_term_point_sql_filter_bounds($point, $not ? '>' : '<=', $this->table_alias);
    if ($piece) {
      list($filter, $args) = $piece;
      $this->query->add_where($this->options['group'], $filter, $args);
    }
  }
  
  function op_distance($point) {
    $not = (strpos($this->operator, 'not') === 0);
    $i = (int)substr($this->operator, $not ? 5 : 1);
    
    $dist = array('value' => $this->options['radius_m'][$i], 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
    
    $piece = gmaps_term_point_sql_filter_distance($point, $not ? '>' : '<=', $dist, NULL, $this->table_alias, !empty($this->options['rounded']));
    if ($piece) {
      list($filter, $args) = $piece;
      $this->query->add_where($this->options['group'], $filter, $args);
    }
  }
  
  function op_empty($point) {
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }
    
    $db_field = $this->query->table_queue[$this->table]['join']->field;
    $this->query->add_where($this->options['group'], "$this->table_alias.$db_field $operator");
  }

}
