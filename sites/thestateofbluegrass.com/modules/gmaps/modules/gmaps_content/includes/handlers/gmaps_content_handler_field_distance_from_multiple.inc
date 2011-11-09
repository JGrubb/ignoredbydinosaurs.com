<?php
// $Id: gmaps_content_handler_field_distance_from_multiple.inc,v 1.1.2.2 2009/10/24 16:20:10 xmarket Exp $

class gmaps_content_handler_field_distance_from_multiple extends gmaps_content_handler_field_distance_from {
  var $defer_query;

  function init(&$view, $options) {
    parent::init($view, $options);
    $field = $this->get_content_field();
    
    $this->defer_query = !empty($options['multiple']['group']) && $field['multiple'] && ($this->_get_location() || $this->options['field'] == 'viewloc');

    if ($this->defer_query) {
      $this->additional_fields = array(
        'nid' => array('table' => 'node', 'field' => 'nid'),
        'type' => array('table' => 'node', 'field' => 'type'),
      );
      if ($view->base_table == 'node_revisions') {
        $this->additional_fields['vid'] = array('table' => 'node_revisions', 'field' => 'vid');
        $this->additional_fields['uid'] = array('table' => 'node_revisions', 'field' => 'uid');
      }
      else {
        $this->additional_fields['vid'] = array('table' => 'node', 'field' => 'vid');
        $this->additional_fields['uid'] = array('table' => 'node', 'field' => 'uid');
      }
    }
  }

  function options(&$options) {
    parent::options($options);
    $options['multiple'] = array(
      'group' => 'min',
      'multiple_number' => '',
      'multiple_from' => '',
      'multiple_reversed' => FALSE,
    );
  }

  /**
   * Provide 'group multiple values' option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field = $this->get_content_field();
    $options = $this->options;

    $form['multiple'] = array(
      '#access' => $field['multiple'],
      '#weight' => 1,
    );
    $opts = array('' => t('All'), 'min' => t('Nearest'), 'max' => t('Farthest'), 'first' => t('First'), 'last' => t('Last'), 'range' => t('Range'));
    $form['multiple']['group'] = array(
      '#title' => t('Group multiple values'),
      '#type' => 'select',
      '#default_value' => $options['multiple']['group'],
      '#options' => $opts,
      '#description' => t('When All selected, each item in the field will create a new row, which may appear to cause duplicates. This setting is not compatible with click-sorting in table displays.'),
    );
    // Make the string translatable by keeping it as a whole rather than
    // translating prefix and suffix separately.
    list($prefix, $suffix) = explode('@count', t('Show @count value(s)'));
    $form['multiple']['multiple_number'] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#field_prefix' => $prefix,
      '#field_suffix' => $suffix,
      '#default_value' => $options['multiple']['multiple_number'],
      '#prefix' => '<div class="container-inline">',
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array('range')),
    );
    list($prefix, $suffix) = explode('@count', t('starting from @count'));
    $form['multiple']['multiple_from'] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#field_prefix' => $prefix,
      '#field_suffix' => $suffix,
      '#default_value' => $options['multiple']['multiple_from'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array('range')),
      '#description' => t('(first item is 0)'),
    );
    $form['multiple']['multiple_reversed'] = array(
      '#title' => t('Reversed'),
      '#type' => 'checkbox',
      '#default_value' => $options['multiple']['multiple_reversed'],
      '#suffix' => '</div>',
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array('range')),
      '#description' => t('(start from last values)'),
    );
  }

  function query() {
    // If this is not a grouped field, use the generic query().
    if (!$this->defer_query) {
      return parent::query();
    }

    // Grouped field: do NOT call ensure_my_table, only add additional fields.
    $this->add_additional_fields();
    $this->field_alias = $this->aliases['vid'];
  }

  function pre_render($values) {
    //now viewloc must be set, if any.
    $this->defer_query = !empty($this->options['multiple']['group']) && $field['multiple'] && $this->_get_location();
    
    // If there are no values to render (displaying a summary, or query returned no results),
    // or if this is not a grouped field, do nothing specific.
    if (isset($this->view->build_info['summary']) || empty($values) || !$this->defer_query) {
      return parent::pre_render($values);
    }
    
    $field = $this->get_content_field();

    $vids = array();
    foreach ($values as $result) {
      if (isset($result->{$this->field_alias}) && !isset($vids[$result->{$this->field_alias}])) {
        $node = drupal_clone($result);
        $node->type = $result->{$this->aliases['type']};
        $node->nid = $result->{$this->aliases['nid']};
        $node->vid = $result->{$this->aliases['vid']};
        $node->uid = $result->{$this->aliases['uid']};
        $node->build_mode = NODE_BUILD_NORMAL;
        $vids[$node->vid] = $node;
      }
    }

    // It may happend that the multiple values field is related to a non
    // required relation for which no node data related to the field being
    // processed here is available.
    if (empty($vids)) {
      return parent::pre_render($values);
    }

    $options = $this->options;
    $range = $options['multiple']['group'] == 'range';
    $this->field_values = array();
    
    foreach ($vids as $vid => $node) {
      if ($range) {
        $items = gmaps_content_point_field_load($node, $field, $options['multiple']['multiple_reversed']);
      }
      else {
        $items = gmaps_content_point_field_load($node, $field);
      }
      if ($items) {
        if ($range) {
          if ($items = $this->_filter_items($items)) {
            $this->field_values[$vid] = $items;
          }
        }
        else if ($options['multiple']['group'] == 'first') {
          $this->field_values[$vid] = array(array_shift($items));
        }
        else if ($options['multiple']['group'] == 'last') {
          $this->field_values[$vid] = array(array_pop($items));
        }
        else {
          $this->field_values[$vid] = $items;
        }
      }
    }
  }
  
  private function _filter_items($items) {
    $options = $this->options;
    $result = array();
    foreach ($items as $delta => $item) {
      if (empty($options['multiple']['multiple_from']) || ($delta >= $options['multiple']['multiple_from'])) {
        if (empty($options['multiple']['multiple_number']) || (count($result) < $options['multiple']['multiple_number'])) {
          $result[] = $item;
        }
        else {
          break;
        }
      }
    }
    
    return $result;
  }

  /**
   * Return DIV or SPAN based upon the field's element type.
   *
   * Fields rendered with the 'group multiple' option use <div> markers,
   * and thus shouldn't be wrapped in a <span>.
   */
  function element_type() {
    if (!$this->defer_query) {
      return parent::element_type();
    }

    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    return $this->options['multiple']['group'] == 'range' ? 'div' : 'span';
  }

  function render($values) {
    // If this is not a grouped field, use content_handler_field::render().
    if (!$this->defer_query) {
      return parent::render($values);
    }
    
    $loc = $this->_get_location();
    if (!$loc) {
      return;
    }
    
    $options = $this->options;
    $group = $this->options['multiple']['group'];
    
    $vid = $values->{$this->field_alias};
    if (isset($this->field_values[$vid])) {
      $items = $this->field_values[$vid];
      
      if ($group == 'range') {
        $rendered = array();
        
        foreach($items as $item) {
          if ($value = $this->_render_value($item['value']->point, $loc)) {
            $rendered[] = $value;
          }
        }
        
        if (count($rendered) == 1) {
          $value = $this->_wrap_value($rendered[0]);
          return $this->render_link($value, $values);
        }
        else {
          return $this->_wrap_value(implode(', ', $rendered));
        }
      }
      else if (in_array($group, array('first', 'last'))) {
        $item = array_shift($items);
        if ($value = $this->_render_value($item['value']->point, $loc)) {
          $value = $this->_wrap_value($value);
          return $this->render_link($value, $values);
        }
      }
      else {
        $dist = NULL;
        foreach($items as $item) {
          if ($d = gmaps_coords_distance($item['value']->point, $loc->point)) {
            $dist = is_null($dist) ? $d : $group($dist, $d);
          }
        }
        
        if ($dist) {
          if ($value = $this->_render_value(NULL, $loc, $dist)) {
            $value = $this->_wrap_value($value);
            return $this->render_link($value, $values);
          }
        }
      }
    }

    return;
  }
}