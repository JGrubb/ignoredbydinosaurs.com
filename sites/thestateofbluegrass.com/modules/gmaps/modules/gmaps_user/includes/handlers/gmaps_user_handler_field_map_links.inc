<?php
// $Id: gmaps_user_handler_field_map_links.inc,v 1.1.2.2 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Map link handler.
 */
class gmaps_user_handler_field_map_links extends views_handler_field_user {
  function options(&$options) {
    parent::options($options);

    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    $address = ($geo['type'] == 'gmaps_address' || $geo['type'] == 'gmaps_anp');
    
    $options['gmaps'] = array(
      'google_find' => 1,
      'google_directions_from' => 1,
      'google_directions_to' => 1,
      'yahoo_find' => 0,
      'yahoo_directions_from' => 0,
      'yahoo_directions_to' => 0,
      'mapquest_find' => 0,
    );

    if ($address) {
      $options['gmaps']['mapquest_directions_from'] = 0;
      $options['gmaps']['mapquest_directions_to'] = 0;
    }
  }

  /**
   * Provide formatter option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['link_to_user'] = array('#type' => 'value', '#value' => FALSE);
    
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    $geo['formatter']['map_links'] = $this->options['gmaps'];

    $form['gmaps'] = array('#type' => 'fieldset',
      '#title' => t('GMaps settings'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($this->options['gmaps']),
      '#tree' => TRUE,
      '#weight' => 5,
    );
    
    $formatter_form = _gmaps_call_func('gmaps_user', 'admin', '_gmaps_user_geo_data_formatter_form', array($geo, TRUE));
    $form['gmaps'] += $formatter_form['map_links'];
  }

  function options_validate($form, &$form_state) {
    if (!array_filter($form_state['values']['options']['gmaps'])) {
      form_set_error('options][gmaps', t('You should select at least one map link to render.'));
    }
  }
  
  /**
   * Return DIV or SPAN based upon the field's element type.
   */
  function element_type() {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    
    return 'div';
  }

  function click_sortable() {
    return FALSE;
  }
  
  /**
   * Called to determine what to tell the clicksorter.
   */
  function click_sort($order) {
  }

  function query() {
    parent::query();
    
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    if ($geo['type'] == 'gmaps_anp' && !array_key_exists('guanp_'. $this->table_alias, $this->view->query->where)) {
      $a = $this->definition['address_table_alias'];
      $p = $this->table_alias;
      $sql = "($p.uid IS NULL OR ($p.uid = $a.uid))";
      $this->view->query->add_where('guanp_'. $this->table_alias, $sql);
    }
  }

  function render($values) {
    $geo = variable_get('gmaps_user_geo_data', array());
    if (empty($geo['type'])) {
      return;
    }
    
    $geo['formatter']['map_links'] = $this->options['gmaps'];

    $item = $this->_build_item_from_values($values, $geo);
    if (!$item) {
      return '';
    }
    
    if ($links = gmaps_anp_map_links($item, $geo['formatter']['map_links'])) {
      return theme('item_list', $links);
    }
    
    return '';
  }
  
  private function _build_item_from_values($values, $geo) {
    $a = in_array($geo['type'], array('gmaps_address', 'gmaps_anp'));
    $p = in_array($geo['type'], array('gmaps_point', 'gmaps_anp'));
    $nanp = !($a && $p);
    $item = new stdClass();
    
    if ($a) {
      $address = new stdClass();
      foreach($this->definition['additional fields'] as $alias => $info) {
        if ($nanp || strpos($alias, 'a_') === 0) {
          $value = $values->{$this->aliases[$alias]};
          $address->{$info['field']} = ($info['serialize'] && !empty($value)) ? unserialize($value) : $value;
        }
      }
      $address->uid = $values->uid;
      $item->address = $address;
      if ($nanp && gmaps_address_empty($address)) {
        return NULL;
      }
    }
    
    if ($p) {
      $point = new stdClass();
      foreach($this->definition['additional fields'] as $alias => $info) {
        if ($nanp || strpos($alias, 'p_') === 0) {
          $value = $values->{$this->aliases[$alias]};
          $point->{$info['field']} = ($info['serialize'] && !empty($value)) ? unserialize($value) : $value;
        }
      }
      $point->uid = $values->uid;
      $item->point = $point;
      if ($nanp && gmaps_point_empty($point)) {
        return NULL;
      }
    }
    
    if (!$nanp) {
      $item->uid = $item->point->uid;
      $item->search = $item->point->search;
      if (gmaps_anp_empty($item)) {
        return NULL;
      }
    }
    
    return $item;
  }

}