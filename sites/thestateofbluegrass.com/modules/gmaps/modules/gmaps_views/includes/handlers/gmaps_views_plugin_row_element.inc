<?php
// $Id: gmaps_views_plugin_row_element.inc,v 1.1.2.2 2010/03/22 14:41:55 xmarket Exp $
/**
 * @file
 * Contains the GMaps Accordion and GMaps Tabs row style plugins.
 */

/**
 * The GMaps Accordion and GMaps Tabs row plugins.
 *
 * @ingroup views_row_plugins
 */
class gmaps_views_plugin_row_element extends views_plugin_row_fields {
  var $panel_class = 'gmaps_views_handler_row_panel';
  
  function option_definition() {
    $options = parent::option_definition();

    $options['collapsible'] = array('default' => 1);
    $options['collapsed'] = array('default' => 0);
    
    if ($this->definition['element type'] == 'tabs') {
      $options['nav_position'] = array('default' => GMAPS_TABS_NAV_TOP);
    }
    
    return $options;
  }

  /**
   * Provide a form for setting options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $options = array();
    foreach ($this->display->handler->get_handlers('field') as $field => $handler) {
      if ($handler instanceof $this->panel_class) {
        break;
      }

      if ($label = $handler->label()) {
        $options[$field] = $label;
      }
      else {
        $options[$field] = $handler->ui_name();
      }
    }
    if (count($options)) {
      $form['inline'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Inline fields'),
        '#options' => $options,
        '#default_value' => $this->options['inline'],
        '#description' => t('Inline fields will be displayed next to each other rather than one after another.'),
      );
    }
    else {
      $form['inline'] = array('#type' => 'value', '#value' => array());
    }

    
    $form['collapsible'] = array('#type' => 'checkbox',
      '#title' => t('Collapsible'),
      '#default_value' => $this->options['collapsible'],
    );
    $form['collapsed'] = array('#type' => 'checkbox',
      '#title' => t('Collapsed'),
      '#default_value' => $this->options['collapsed'],
    );
    if ($this->definition['element type'] == 'tabs') {
      $form['nav_position'] = array('#type' => 'select',
        '#title' => t('Tabs navigation position'),
        '#default_value' => $this->options['nav_position'],
        '#options' => array(
          GMAPS_TABS_NAV_TOP => t('Top'),
          GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
          GMAPS_TABS_NAV_LEFT => t('Left'),
          GMAPS_TABS_NAV_RIGHT => t('Right'),
        ),
      );
    }
  }

  function render($row) {
    $view = &$this->view;
    $options = $this->options;
  
    if ($this->definition['element type'] == 'tabs') {
      $tabs = TRUE;
      $element_type = 'gmaps_tabs';
      $panel_type = 'gmaps_tabs_panel';
      $class = 'gmaps-views-row-tabs';
    }
    else {
      $tabs = FALSE;
      $element_type = 'gmaps_accordion';
      $panel_type = 'gmaps_accordion_panel';
      $class = 'gmaps-views-row-accordion';
    }
    
    $element_info = _element_info($element_type);
    $panel_info = _element_info($panel_type);
    
    // Loop through the fields for this view.
    $inline = FALSE;
    $fields = $panel_fields = $panel_descriptions = array(); // ensure it's at least an empty array.
    foreach ($view->field as $id => $field) {
      if ($field instanceof $this->panel_class) {
        $current_panel = $id;
        $current_options = $field->options;
        $field_output = $view->style_plugin->get_field($view->row_index, $id);
        if (!empty($field_output)) {
          $panel_descriptions[$id] = $field_output;
        }
      }
      else if (isset($current_panel)) {
        if ($object = ($this->_build_field($view, $row, $id, $field, $current_options, $inline))) {
          $panel_fields[$current_panel][$id] = $object;
        }
      }
      else {
        if ($object = ($this->_build_field($view, $row, $id, $field, $options, $inline))) {
          $fields[$id] = $object;
        }
      }
    }
    
    if (empty($panel_fields)) {
      $view->gmaps_fields = $fields;
      $output = theme($this->content_theme_functions(), $view, $this->options, $row, $this->field_alias);
      unset($view->gmaps_fields);
      return $output;
    }
    else {
      $element = array('#type' => $element_type,
        '#id' => preg_replace('/[^a-z0-9]/', '-', 'gmaps-views-row-'. $view->name .'-'. $this->display->id .'-'. $view->row_index),  
        '#collapsible' => $options['collapsible'],
        '#collapsed' => $options['collapsed'],
        '#attributes' => array('class' => $this->element_classes($class)),
        '#defaults_loaded' => TRUE,
      );
      if ($tabs) {
        $element['#nav_position'] = $options['nav_position'];
      }
      $element += $element_info;
      
      if (!empty($fields)) {
        $view->gmaps_fields = $fields;
        $element['#value'] = theme($this->content_theme_functions(), $view, $this->options, $row, $this->field_alias);
        unset($view->gmaps_fields);
      }
      
      $count = 0;
      foreach ($panel_fields as $id => $fields) {
        $field = $view->field[$id];
        
        $panel_id = !empty($field->options['css_id']) ? $field->options['css_id'] : ($tabs ? 'tab-'. $count : 'panel-'. $count);
        
        $title = $field->label();
        if (empty($title)) {
          $title = $tabs ? t('Tab @count', array('@count' => $count + 1)) : t('Panel @count', array('@count' => $count + 1));
        }
        
        $view->gmaps_fields = $fields;
        $output = theme($this->content_theme_functions($panel_id), $view, $field->options, $row, $this->field_alias);
        unset($view->gmaps_fields);
        
        $element[$panel_id] = array('#type' => $panel_type,
          '#id' => $panel_id,
          '#title' => $title,
          '#value' => $output,
          '#defaults_loaded' => TRUE,
        );
        $element[$panel_id] += $panel_info;
        
        if ($field->options['selected_panel']) {
          if ($tabs) {
            $element[$panel_id]['#selected_panel'] = $field->options['selected_panel'];
            $element['#tabs']['selected'] = $count;
          }
          else {
            $element[$panel_id]['#active_panel'] = $field->options['selected_panel'];
            $element['#accordion']['active'] = $count;
          }
        }
        
        if (isset($panel_descriptions[$id])) {
          $element[$panel_id]['#description'] = $panel_descriptions[$id];
        }
        
        $count++;
      }
      
      $view->gmaps_element = $element;
      $output = theme($this->theme_functions(), $view, $this->options, $row, $this->field_alias);
      unset($view->gmaps_element);
      return $output;
    }
  }
  
  function content_theme_functions($panel = NULL) {
    $hook = 'gmaps_views_view_fields';
    return $panel ? $this->_panel_theme_functions($hook, $panel) : views_theme_functions($hook, $this->view, $this->display);
  }
  
  function element_classes($class) {
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
  
  protected function _build_field(&$view, $row, $id, &$field, $options, &$inline) {
    if (!isset($view->field[$id])) {
      return;
    }
    // render this even if set to exclude so it can be used elsewhere.
    $field_output = $view->style_plugin->get_field($view->row_index, $id);
    $empty = $field_output !== 0 && empty($field_output);
    if (empty($field->options['exclude']) && (!$empty || empty($field->options['hide_empty']))) {
      $object = new stdClass();

      $object->content = $field_output;
      if (isset($field->field_alias) && isset($row->{$field->field_alias})) {
        $object->raw = $row->{$field->field_alias};
      }
      else {
        $object->raw = NULL; // make sure it exists to reduce NOTICE
      }
      $object->inline = !empty($options['inline'][$id]);
      $object->inline_html = $object->inline ? 'span' : 'div';
      if (!empty($options['separator']) && $inline && $object->inline && $object->content) {
        $object->separator = filter_xss_admin($options['separator']);
      }

      $inline = $object->inline;

      $object->handler = &$view->field[$id];
      $object->element_type = $object->handler->element_type();

      $object->class = views_css_safe($id);
      $object->label = check_plain($field->label());
      return $object;
    }
  }
  
  protected function _panel_theme_functions($hook, $panel) {
    $panel_hook = $hook .'__'. ($this->definition['element type'] == 'tabs' ? 'tab' : 'panel') .'__'. $panel;
    $view = $this->view;
    $display = $this->display;
    
    $themes = array();
  
    if ($display) {
      $themes[] = $panel_hook . '__' . $view->name . '__' . $display->id;
      $themes[] = $hook . '__' . $view->name . '__' . $display->id;
      $themes[] = $panel_hook . '__' . $display->id;
      $themes[] = $hook . '__' . $display->id;
      $themes[] = $panel_hook . '__' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      $themes[] = $hook . '__' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      if ($display->id != $display->display_plugin) {
        $themes[] = $panel_hook . '__' . $view->name . '__' . $display->display_plugin;
        $themes[] = $hook . '__' . $view->name . '__' . $display->display_plugin;
        $themes[] = $panel_hook . '__' . $display->display_plugin;
        $themes[] = $hook . '__' . $display->display_plugin;
      }
    }
    $themes[] = $panel_hook . '__' . $view->name;
    $themes[] = $hook . '__' . $view->name;
    $themes[] = $panel_hook;
    $themes[] = $hook;
    
    return $themes;
  }
}

