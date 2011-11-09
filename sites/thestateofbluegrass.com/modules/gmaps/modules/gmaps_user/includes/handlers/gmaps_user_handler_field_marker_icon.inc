<?php
// $Id: gmaps_user_handler_field_marker_icon.inc,v 1.1.2.1 2010/03/10 13:52:18 xmarket Exp $

/**
 * @file
 * Icon and protected icon option for user markers.
 */
class gmaps_user_handler_field_marker_icon extends views_handler_field implements gmaps_views_map_overlay_option_marker, gmaps_views_static_map_overlay_option_marker {
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
    
    switch ($this->options['show']) {
      case 'normal':
        $icon = $values->{$this->aliases['icon']};
        if ($icon != GMAPS_ICON_MAP_DEFAULT && $icon != GMAPS_ICON_API_DEFAULT) {
          if ($icon = gmaps_icon_load($icon)) {
          $output = theme('gmaps_icon_preview', $icon->icon);
          }
        }
        break;
      case 'protected':
        $icon = $values->{$this->aliases['protected_icon']};
        if ($icon != GMAPS_ICON_MAP_DEFAULT && $icon != GMAPS_ICON_API_DEFAULT) {
          if ($icon = gmaps_icon_load($icon)) {
          $output = theme('gmaps_icon_preview', $icon->icon);
          }
        }
        break;
    }
    
    return $output;
  }
  
  function set_overlay_option(&$gmi, &$overlay, $values) {
    $icon = $values->{$this->aliases['icon']};
    if ($icon != GMAPS_ICON_MAP_DEFAULT) {
      $overlay['basic']['icon'] = $icon;
    }
    
    $icon = $values->{$this->aliases['protected_icon']};
    if ($icon != GMAPS_ICON_MAP_DEFAULT && $this->options['protected']) {
      $overlay['basic']['protected_icon'] = $icon;
    }
  }
  
  function set_static_overlay_option(&$smi, &$overlay, $values) {
    $icon = unserialize($values->{$this->aliases['static_icon']});
    if (!empty($icon)) {
      $overlay['style'] = $icon['style'];
      $overlay['style_sticky'] = $icon['style_sticky'];
    }
    
    $icon = unserialize($values->{$this->aliases['protected_static_icon']});
    if (!empty($icon) && $this->options['protected']) {
      $overlay['protected'] = $icon['style'];
      $overlay['protected_sticky'] = $icon['style_sticky'];
    }
  }
  
}