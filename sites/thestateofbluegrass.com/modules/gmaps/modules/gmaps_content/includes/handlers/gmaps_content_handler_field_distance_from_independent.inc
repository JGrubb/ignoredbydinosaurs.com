<?php
// $Id: gmaps_content_handler_field_distance_from_independent.inc,v 1.1.2.2 2009/10/24 16:20:10 xmarket Exp $

class gmaps_content_handler_field_distance_from_independent extends gmaps_content_handler_field_distance_from {
  var $defer_query;

  function init(&$view, $options) {
    parent::init($view, $options);
    
    $this->defer_query = !empty($options['group']) && ($this->_get_location() || strpos($this->options['field'], 'viewloc') === 0);

    if ($this->defer_query) {
      $this->additional_fields = array();
      if ($view->base_table == 'node_revisions') {
        $this->additional_fields['vid'] = array('table' => 'node_revisions', 'field' => 'vid');
      }
      else {
        $this->additional_fields['vid'] = array('table' => 'node', 'field' => 'vid');
      }
    }
  }

  function options(&$options) {
    parent::options($options);
    $options['group'] = 'min';
  }

  /**
   * Provide 'group multiple values' option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $options = $this->options;

    $opts = array('' => t('All'), 'min' => t('Nearest'), 'max' => t('Farthest'));
    $form['group'] = array(
      '#title' => t('Show values'),
      '#type' => 'select',
      '#default_value' => $options['group'],
      '#options' => $opts,
      '#description' => t('When All selected, each item in the field will create a new row, which may appear to cause duplicates. This setting is not compatible with click-sorting in table displays.'),
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
    $this->defer_query = !empty($this->options['group']) && $this->_get_location();
    
    // If there are no values to render (displaying a summary, or query returned no results),
    // or if this is not a grouped field, do nothing specific.
    if (isset($this->view->build_info['summary']) || empty($values) || !$this->defer_query) {
      return parent::pre_render($values);
    }
    
    $vids = array();
    foreach ($values as $result) {
      if (isset($result->{$this->field_alias}) && !isset($vids[$result->{$this->field_alias}])) {
        $vids[$result->{$this->field_alias}] = $result->{$this->field_alias};
      }
    }

    // It may happend that the multiple values field is related to a non
    // required relation for which no node data related to the field being
    // processed here is available.
    if (empty($vids)) {
      return parent::pre_render($values);
    }

    $this->field_values = array();
    
    foreach ($vids as $vid) {
      $result = db_query("SELECT * FROM {gmaps_content_point} WHERE vid = %d AND map_type = '%s'", $vid, $this->definition['map_type']);
      while($item = db_fetch_object($result)) {
        $items[] = $item;
      }
      if ($items) {
        $this->field_values[$vid] = $items;
      }
    }
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
    return 'span';
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
    $group = $this->options['group'];
    
    $vid = $values->{$this->field_alias};
    if (isset($this->field_values[$vid])) {
      $items = $this->field_values[$vid];
      
      $dist = NULL;
      foreach($items as $item) {
        if ($d = gmaps_coords_distance($item, $loc->point)) {
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

    return;
  }
}