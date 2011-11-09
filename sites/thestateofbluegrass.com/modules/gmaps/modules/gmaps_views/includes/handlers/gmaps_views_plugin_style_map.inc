<?php
// $Id: gmaps_views_plugin_style_map.inc,v 1.1.2.2 2010/03/22 14:41:55 xmarket Exp $
/**
 * @file
 * Contains the GMaps map and GMaps static map style plugins.
 */

/**
 * Shared GMaps Map and GMaps static map style plugin.
 *
 * @ingroup views_style_plugins
 */
class gmaps_views_plugin_style_map extends views_plugin_style implements gmaps_views_map, gmaps_views_static_map {
  function option_definition() {
    $options = parent::option_definition();

    $mid = $this->definition['static maps api'] ? gmaps_static_map_get_mid_for_default('view_style') : gmaps_map_get_mid_for_default('view_style_earth');
    $options['preset'] = array('default' => $mid);
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['preset'] = array('#type' => 'select',
      '#title' => t('Map preset'),
      '#default_value' => $this->options['preset'],
      '#options' => $this->definition['static maps api'] ? gmaps_static_map_get_options() : gmaps_map_get_options(),
    );
  }
  
  function uses_fields() {
    return TRUE;
  }

  function render() {
    if (empty($this->row_plugin)) {
      vpr('gmaps_views_plugin_style_map: Missing row plugin');
      return;
    }
    else if (empty($this->options['preset'])) {
      vpr('gmaps_views_plugin_style_map: Missing map preset');
      return;
    }

    $smap = $this->definition['static maps api'];
    $cache_interface = $smap ? 'gmaps_views_cache_static_map' : 'gmaps_views_cache_map';
    $cache = array();
    
    $cache_plugin = $this->view->display_handler->get_cache_plugin();
    if ($cache_plugin instanceof $cache_interface ) {
      $cache = $smap ? $cache_plugin->get_static_map_cache_options() : $cache_plugin->get_map_cache_options();
    }
    
    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    $multiple = count($sets) > 1;

    $output = '';
    $this->view->set_index = 0;
    $this->view->row_index = 0;
    foreach ($sets as $title => $records) {
      $id = $smap ? 'gmaps-views-static-map-' : 'gmaps-views-map-';
      $id = preg_replace('/[^a-z0-9]/', '-', $id . $this->view->name .'-'. $this->display->id . ($multiple ? '-'. $this->view->set_index : ''));
      
      $current_cache = $cache;
      if (!empty($current_cache) && $multiple) {
        $current_cache['cid'] .= ':set.'. $this->view->set_index;
      }
      
      if ($smap) {
        $this->view->gmaps_static_map_item = gmaps_static_map_item_render($this->options['preset'], '_gmaps_views_plugin_style_static_map_smi', array(&$this, &$records), $current_cache, $id);
        $output .= theme($this->theme_functions(), $this->view, $this->options, $rows, $title);
        unset($this->view->gmaps_static_map_item);
      }
      else {
        $this->view->gmaps_map_item = gmaps_map_item_render($this->options['preset'], '_gmaps_views_plugin_style_map_gmi', array(&$this, &$records), $current_cache, $id);
        $output .= theme($this->theme_functions(), $this->view, $this->options, $rows, $title);
        unset($this->view->gmaps_map_item);
      }
      
      $this->view->set_index++;
    }
    unset($this->view->set_index, $this->view->row_index);
    return $output;
  }

  function validate() {
    $errors = parent::validate();

    $smap = $this->definition['static maps api'];
    $row_style_interface = $smap ? 'gmaps_views_static_map_overlay' : 'gmaps_views_map_overlay';
    
    $plugin = $this->display->handler->get_plugin('row');
    if (!empty($plugin)) {
      if (!($plugin instanceof $row_style_interface)) {
        if ($smap) {
          $errors[] = t('Style @style requires a GMaps static map overlay row style plugin.', array('@style' => $this->definition['title']));
        }
        else {
          $errors[] = t('Style @style requires a GMaps map overlay row style plugin.', array('@style' => $this->definition['title']));
        }
      }
      if ($result = $plugin->validate()) {
        $errors = array_merge($errors, $result);
      }
    }
    
    return $errors;
  }
  
  function map_item_classes($class) {
    $view = $this->view;
    $display = $this->display;
    
    $classes = array($class, $class . '-' . $view->name);
    
    if ($display) {
      if ($display->id != $display->display_plugin) {
        $classes[] = $class . '-' . $display->display_plugin;
        $classes[] = $class . '-' . $view->name . '-' . $display->display_plugin;
      }
      $classes[] = $class . '-' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      $classes[] = $class . '-' . $display->id;
      $classes[] = $class . '-' . $view->name . '-' . $display->id;
    }
    
    return implode(' ', $classes);
  }
  
  function get_map_reference_mid() {
    return $this->definition['static maps api'] ? NULL : $this->options['preset'];
  }
  
  function get_static_map_reference_mid() {
    return $this->definition['static maps api'] ? $this->options['preset'] : NULL;
  }
}

function _gmaps_views_plugin_style_map_gmi(&$gmi, &$style, &$records) {
  foreach ($records as $index => $row) {
    $style->row_plugin->add_overlay($gmi, $row);
    $style->view->row_index++;
  }
  
  $interface = 'gmaps_views_map_option';
  foreach ($style->view->field as $id => $field) {
    if ($field instanceof $interface) {
      $field->set_map_option($gmi);
    }
  }
  
  $gmi->attributes['class'] = $style->map_item_classes('gmaps-views-view-map');
}

function _gmaps_views_plugin_style_static_map_smi(&$smi, &$style, &$records) {
  foreach ($records as $index => $row) {
    $style->row_plugin->add_static_overlay($smi, $row);
    $style->view->row_index++;
  }
  
  $interface = 'gmaps_views_static_map_option';
  foreach ($style->view->field as $id => $field) {
    if ($field instanceof $interface) {
      $field->set_static_map_option($gmi);
    }
  }
  
  $smi->attributes['class'] = $style->map_item_classes('gmaps-views-view-static-map');
}
