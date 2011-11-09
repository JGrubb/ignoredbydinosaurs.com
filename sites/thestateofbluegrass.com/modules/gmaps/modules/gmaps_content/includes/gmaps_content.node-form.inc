<?php
// $Id: gmaps_content.node-form.inc,v 1.1.2.8 2009/12/07 09:21:49 xmarket Exp $

/**
 * @file
 * CCK field node edit form related functions.
 */

function gmaps_content_address_field_invoke_all($op, &$node, $field, &$items, $teaser, $page) {
  $return = array();
  foreach(module_implements('gmaps_address_field') as $module) {
    $function = $module .'_gmaps_address_field';
    $result = $function($op, $node, $field, $items, $teaser, $page);
    if ($result) {
      if (is_array($result)) {
        $return = array_merge($return, $result);
      }
      else {
        $return[] = $result;
      }
    }
  }
  return $return;
}

function _gmaps_content_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  global $user;
  $element = array(
    '#type' => 'markup',
    '#default_value' => isset($items[$delta]) ? $items[$delta] : array(),
  );
  
  $type = &$field['type'];
  if ($type == 'gmaps_distance') {
    $element['#type'] = ($field['widget']['type'] == 'gmaps_user_distance') ? $user->gmaps_settings['length']['input'] : $field['widget']['type'];
    $element['#enable_negative'] = $field['gmaps']['enable_negative'];
  }
  else {
    $element['#type'] = $field['widget']['type'];
    
    $gmaps = &$field['gmaps'];
    $input = &$field['widget']['gmaps']['input'];
    
    if ($type == 'gmaps_address' || $type == 'gmaps_anp') {
      $element['#allowed_countries'] = $gmaps['allowed_countries'];
    }
    if ($type == 'gmaps_point' || $type == 'gmaps_anp') {
      $element['#map_type'] = $gmaps['map_type'] ? $gmaps['map_type'] : GMAPS_MAP_EARTH;
      $element['#enable_bounds'] = isset($gmaps['enable_bounds']) ? $gmaps['enable_bounds'] : TRUE;
      $element['#enable_3D'] = $gmaps['enable_3D'];
      $element['#enable_pov'] = $gmaps['enable_pov'];
      
      $element['#input_disabled'] = isset($input['input_disabled']) ? $input['input_disabled'] : TRUE;
    }
    if ($type == 'gmaps_anp' && $input['tabbed_parts'] > -1) {
      $element['#tabbed_parts'] = $input['tabbed_parts'];
    }
    $element['#enable_privacy'] = $gmaps['enable_privacy'];

    $element['#enable_geocoder'] = $input['enable_geocoder'];
    $element['#use_map'] = $input['use_map'] ? TRUE : FALSE;
    $element['#collapsible'] = $input['collapsible'];
    if ($input['collapsed'] > -1) {
      $element['#collapsed'] = $input['collapsed'];
    }
  }
  
  return $element;
}

/**
 * Convert #default_value to CCK compatible partial anp array.
 */
function _gmaps_content_address_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    if (isset($element['#default_value'])) {
      $value = is_object($element['#default_value']) ? (array)$element['#default_value'] : $element['#default_value'];
      if (!isset($value['value'])) {
        $value = array('value' => $value);
      }
      else if (is_object($value['value'])) {
        $value['value'] = (array)$value['value'];
      }
      if (!isset($value['value']['address'])) {
        $value = array('value' => array('address' => $value['value']));
      }
      else if (is_object($value['value']['address'])){
        $value['value']['address'] = (array)$value['value']['address'];
      }
      return $value;
    }
    return array();
  }
  else if (!empty($edit)) {
    $edit['value']['address'] = form_type_gmaps_address_value(array(), $edit['value']['address']);
    return $edit;
  }
}

function _gmaps_content_address_expand($element, $edit, $form_state, $form) {
  $field = $form['#field_info'][$element['#field_name']];
  $element['#required'] = $field['required'];
  $element['#tree'] = TRUE;
  $element['value'] = array(
    '#weight' => 0,
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
  );
  $element['value']['address'] = array(
    '#type' => 'gmaps_address',
    '#default_value' => $element['#value']['value']['address'],
    '#weight' => 0,
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    //gmaps options
    '#allowed_countries' => $element['#allowed_countries'],
    '#enable_privacy' => $element['#enable_privacy'],
    '#enable_geocoder' => $element['#enable_geocoder'],
    '#use_map' => $element['#use_map'],
    '#collapsible' => $element['#collapsible'],
    '#search_value' => $element['#search_value'],
  );
  if (isset($element['#collapsed'])) {
    $element['value']['address']['#collapsed'] = $element['#collapsed'];
  }

  return $element;
}

function _gmaps_content_address_validate($element, &$form_state) {
  //always return array for CCK compatibility
  if (!form_get_errors()) {
    $value = &$element['#value']['value']['address'];
    $value = (object)$value;
    $element['#value']['value'] = (object)$element['#value']['value'];
    form_set_value($element, $element['#value'], $form_state);
  }
}

function theme_gmaps_content_address($element) {
  return $element['#children'];
}

/**
 * Convert #default_value to CCK compatible partial anp array.
 */
function _gmaps_content_point_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    if (isset($element['#default_value'])) {
      $value = is_object($element['#default_value']) ? (array)$element['#default_value'] : $element['#default_value'];
      if (!isset($value['value'])) {
        $value = array('value' => $value);
      }
      else if (is_object($value['value'])) {
        $value['value'] = (array)$value['value'];
      }
      if (!isset($value['value']['point'])) {
        $value = array('value' => array('point' => $value['value']));
      }
      else if (is_object($value['value']['point'])){
        $value['value']['point'] = (array)$value['value']['point'];
      }
      return $value;
    }
    return array();
  }
  else if (!empty($edit)) {
    $edit['value']['point'] = form_type_gmaps_point_value(array(), $edit['value']['point']);
    return $edit;
  }
}

function _gmaps_content_point_expand($element, $edit, $form_state, $form) {
  $field = $form['#field_info'][$element['#field_name']];
  $element['#tree'] = TRUE;
  $element['value'] = array(
    '#weight' => 0,
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
  );
  $element['value']['point'] = array(
    '#type' => 'gmaps_point',
    '#default_value' => $element['#value']['value']['point'],
    '#weight' => 0,
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    //gmaps options
    '#map_type' => $element['#map_type'],
    '#enable_bounds' => $element['#enable_bounds'],
    '#enable_3D' => $element['#enable_3D'],
    '#enable_pov' => $element['#enable_pov'],
    '#enable_privacy' => $element['#enable_privacy'],
    '#enable_geocoder' => $element['#enable_geocoder'],
    '#use_map' => $element['#use_map'],
    '#collapsible' => $element['#collapsible'],
    '#input_disabled' => $element['#input_disabled'],
    '#search_value' => $element['#search_value'],
  );
  if (isset($element['#collapsed'])) {
    $element['value']['point']['#collapsed'] = $element['#collapsed'];
  }

  return $element;
}

function _gmaps_content_point_validate($element, &$form_state) {
  //always return array for CCK compatibility
  if (!form_get_errors()) {
    $value = &$element['#value']['value']['point'];
    _gmaps_point_set_return_value($value, $element['#map_type']);
    $element['#value']['value'] = (object)$element['#value']['value'];
    form_set_value($element, $element['#value'], $form_state);
  }
}

function theme_gmaps_content_point($element) {
  return $element['#children'];
}

/**
 * Convert #default_value to CCK compatible partial anp array.
 */
function _gmaps_content_anp_value($element, $edit = FALSE) {
  if ($edit === FALSE) {
    if (isset($element['#default_value'])) {
      $value = is_object($element['#default_value']) ? (array)$element['#default_value'] : $element['#default_value'];
      if (!isset($value['value'])) {
        $value = array('value' => $value);
      }
      else if (is_object($value['value'])) {
        $value['value'] = (array)$value['value'];
      }
      return $value;
    }
    return array();
  }
  else if (!empty($edit)) {
    $edit['value'] = form_type_gmaps_anp_value(array(), $edit['value']);
    return $edit;
  }
}

function _gmaps_content_anp_expand($element, $edit, $form_state, $form) {
  $field = $form['#field_info'][$element['#field_name']];
  $element['#required'] = $field['required'];
  $element['#tree'] = TRUE;
  $element['value'] = array(
    '#type' => 'gmaps_anp',
    '#default_value' => $element['#value']['value'],
    '#weight' => 0,
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#title' => $element['#title'],
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    //gmaps options
    '#allowed_countries' => $element['#allowed_countries'],
    '#enable_bounds' => $element['#enable_bounds'],
    '#enable_3D' => $element['#enable_3D'],
    '#enable_pov' => $element['#enable_pov'],
    '#enable_privacy' => $element['#enable_privacy'],
    '#enable_geocoder' => $element['#enable_geocoder'],
    '#use_map' => $element['#use_map'],
    '#collapsible' => $element['#collapsible'],
    '#input_disabled' => $element['#input_disabled'],
    '#search_value' => $element['#search_value'],
  );
  if (isset($element['#collapsed'])) {
    $element['value']['#collapsed'] = $element['#collapsed'];
  }
  if (isset($element['#tabbed_parts'])) {
    $element['value']['#tabbed_parts'] = $element['#tabbed_parts'];
  }
  
  return $element;
}

function _gmaps_content_anp_validate($element, &$form_state) {
  //always return array for CCK compatibility
  if (!form_get_errors()) {
    $value = &$element['#value']['value'];
    $value['address'] = (object)$value['address'];
    _gmaps_point_set_return_value($value['point'], $element['value']['point']['#map_type']);
    $value = (object)$value;
    form_set_value($element, $element['#value'], $form_state);
  }
}

function theme_gmaps_content_anp($element) {
  return $element['#children'];
}



function _gmaps_content_field($op, &$node, $field, &$items, $teaser, $page) {
  if (in_array($field['type'], array('gmaps_address', 'gmaps_anp'))) {
    $extra = gmaps_content_address_field_invoke_all($op, $node, $field, $items, $teaser, $page);
  }
  switch ($op) {
    case 'insert':
    case 'update':
      if ($field['type'] == 'gmaps_address') {
        _gmaps_content_address_field_save($node, $field, $items, $op == 'update');
      }
      else if ($field['type'] == 'gmaps_point') {
        _gmaps_content_point_field_save($node, $field, $items, $op == 'update');
      }
      else if ($field['type'] == 'gmaps_anp') {
        _gmaps_content_anp_field_save($node, $field, $items, $op == 'update');
      }

      break;
    case 'delete':
      db_query('DELETE FROM {gmaps_content_address} WHERE nid = %d', $node->nid);
      db_query('DELETE FROM {gmaps_content_point} WHERE nid = %d', $node->nid);

      gmaps_content_field_cache_clear_all('%:%:'. $node->nid .':');

      break;
    case 'delete revision':
      db_query('DELETE FROM {gmaps_content_address} WHERE vid = %d', $node->vid);
      db_query('DELETE FROM {gmaps_content_point} WHERE vid = %d', $node->vid);

      gmaps_content_field_cache_clear_all($node->vid .':');

      break;
    case 'prepare translation':
      if (in_array($field['type'], array('gmaps_address', 'gmaps_anp'))) {
        return $extra;
      }
      break;
  }
}

function _gmaps_content_address_field_save(&$node, $field, &$items, $update = FALSE) {
  if ($update) {
    db_query("DELETE FROM {gmaps_content_address} WHERE vid = %d AND field_name = '%s'", $node->vid, $field['field_name']);
  }
  $result = array();
  $delta = 0;
  foreach($items as $item) {
    if (gmaps_address_empty($item['value']->address)) {
      continue;
    }
    $adr = &$item['value']->address;
    $adr->vid = $node->vid;
    $adr->nid = $node->nid;
    $adr->field_name = $field['field_name'];
    $adr->delta = $delta;
    
    gmaps_address_prepare_record($adr);
    drupal_write_record('gmaps_content_address', $adr);
    
    $result[$delta] = $item;
    $delta++;
  }
  
  $items = $result;
}

function _gmaps_content_point_field_save(&$node, $field, &$items, $update = FALSE) {
  if ($update) {
    db_query("DELETE FROM {gmaps_content_point} WHERE vid = %d AND field_name = '%s'", $node->vid, $field['field_name']);
  }
  $result = array();
  $delta = 0;
  foreach($items as $item) {
    if (gmaps_point_empty($item['value']->point)) {
      continue;
    }
    
    $point = &$item['value']->point;
    
    gmaps_coords_add_cart($point);
    if (property_exists($point, 'bounds') && gmaps_bounds_empty($point->bounds)) {
      $point->bounds = array();
    }
    if (property_exists($point, 'pov') && gmaps_pov_empty($point->pov)) {
      $point->pov = array();
    }
    
    $point->vid = $node->vid;
    $point->nid = $node->nid;
    $point->field_name = $field['field_name'];
    $point->delta = $delta;
    
    drupal_write_record('gmaps_content_point', $point);
    
    $result[$delta] = $item;
    $delta++;
  }
  
  $items = $result;

  gmaps_content_field_cache_clear_all($node->vid .':'. $field['field_name'] .':');
}

function _gmaps_content_anp_field_save(&$node, $field, &$items, $update = FALSE) {
  if ($update) {
    db_query("DELETE FROM {gmaps_content_address} WHERE vid = %d AND field_name = '%s'", $node->vid, $field['field_name']);
    db_query("DELETE FROM {gmaps_content_point} WHERE vid = %d AND field_name = '%s'", $node->vid, $field['field_name']);
  }
  $result = array();
  $delta = 0;
  foreach($items as $item) {
    if (gmaps_anp_empty($item['value'])) {
      continue;
    }
    
    $adr = &$item['value']->address;
    $adr->vid = $node->vid;
    $adr->nid = $node->nid;
    $adr->field_name = $field['field_name'];
    $adr->delta = $delta;
    $adr->search = $item['value']->search;
    
    gmaps_address_prepare_record($adr);
    drupal_write_record('gmaps_content_address', $adr);
    
    $point = &$item['value']->point;
    
    gmaps_coords_add_cart($point);
    if (property_exists($point, 'bounds') && gmaps_bounds_empty($point->bounds)) {
      $point->bounds = array();
    }
    if (property_exists($point, 'pov') && gmaps_pov_empty($point->pov)) {
      $point->pov = array();
    }
        
    $point->vid = $node->vid;
    $point->nid = $node->nid;
    $point->field_name = $field['field_name'];
    $point->delta = $delta;
    $point->search = $item['value']->search;
    
    drupal_write_record('gmaps_content_point', $point);
    
    $result[$delta] = $item;
    $delta++;
  }
  
  $items = $result;

  gmaps_content_field_cache_clear_all($node->vid .':'. $field['field_name'] .':');
}
