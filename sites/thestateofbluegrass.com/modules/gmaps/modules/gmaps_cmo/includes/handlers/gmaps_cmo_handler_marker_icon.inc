<?php
// $Id: gmaps_cmo_handler_marker_icon.inc,v 1.1.2.1 2010/02/18 16:36:22 xmarket Exp $

/**
 * @file
 * Icon and protected icon option for map markers.
 */
class gmaps_cmo_handler_marker_icon extends views_handler_field implements gmaps_views_map_overlay_option_marker, gmaps_views_static_map_overlay_option_marker {
  protected $content_field;

  function construct() {
    parent::construct();
  }

  function init(&$view, $options) {
    parent::init($view, $options);
  }

  function option_definition() {
    $options = parent::option_definition();
    
    $options['protected'] = array('default' => TRUE);
    $options['show'] = array('default' => '');
    
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['protected'] = array('#type' => 'checkbox',
      '#title' => t('Set protected icon'),
      '#default_value' => $this->options['protected'],
    );
    $form['show'] = array('#type' => 'select',
      '#title' => t('Show icon'),
      '#default_value' => $this->options['show'],
      '#options' => array(
        '' => t('Disabled'),
        'normal' => t('Normal icon'),
        'protected' => t('Protected icon'),
      ),
    );
  }
  
  function admin_summary() {
    return '';
  }
  
  function click_sortable() {
    FALSE;
  }

  function render($values) {
    $output = '';
    
    $type = $values->{$this->field_alias};
    $mo_data = variable_get('gmaps_cmo_'. $type, NULL);
    
    switch ($this->options['show']) {
      case 'normal':
        if ($mo_data && $mo_data->icon != GMAPS_ICON_MAP_DEFAULT && $mo_data->icon != GMAPS_ICON_API_DEFAULT) {
          if ($icon = gmaps_icon_load($mo_data->icon)) {
            $output = theme('gmaps_icon_preview', $icon->icon);
          }
        }
        break;
      case 'protected':
        if ($mo_data && $mo_data->protected_icon != GMAPS_ICON_MAP_DEFAULT && $mo_data->protected_icon != GMAPS_ICON_API_DEFAULT) {
          if ($icon = gmaps_icon_load($mo_data->protected_icon)) {
            $output = theme('gmaps_icon_preview', $icon->icon);
          }
        }
        break;
    }
    
    return $output;
  }
  
  function set_overlay_option(&$gmi, &$overlay, $values) {
    $type = $values->{$this->field_alias};
    $mo_data = variable_get('gmaps_cmo_'. $type, NULL);
    
    if ($mo_data && $mo_data->icon != GMAPS_ICON_MAP_DEFAULT) {
      $overlay['basic']['icon'] = $mo_data->icon;
    }
    
    if ($mo_data && $mo_data->protected_icon != GMAPS_ICON_MAP_DEFAULT && $this->options['protected']) {
      $overlay['basic']['protected_icon'] = $mo_data->protected_icon;
    }
  }
  
  function set_static_overlay_option(&$smi, &$overlay, $values) {
    $type = $values->{$this->field_alias};
    $mo_data = variable_get('gmaps_cmo_'. $type, NULL);
    
    if ($mo_data && !empty($mo_data->static_icon)) {
      $overlay['style'] = $mo_data->static_icon['style'];
      $overlay['style_sticky'] = $mo_data->static_icon['style_sticky'];
    }
    
    if ($mo_data && !empty($mo_data->protected_static_icon) && $this->options['protected']) {
      $overlay['protected'] = $mo_data->protected_static_icon['style'];
      $overlay['protected_sticky'] = $mo_data->protected_static_icon['style_sticky'];
    }
  }
  
}