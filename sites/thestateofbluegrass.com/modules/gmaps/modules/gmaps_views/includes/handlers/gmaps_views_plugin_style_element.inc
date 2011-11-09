<?php
// $Id: gmaps_views_plugin_style_element.inc,v 1.1.2.2 2010/03/22 14:41:55 xmarket Exp $
/**
 * @file
 * Contains the GMaps accordion and GMaps tabs style plugins.
 */

/**
 * GMaps Accordion and GMaps tabs style plugin.
 *
 * @ingroup views_style_plugins
 */
class gmaps_views_plugin_style_element extends views_plugin_style {
  function option_definition() {
    $options = parent::option_definition();

    $options['collapsible'] = array('default' => 1);
    $options['collapsed'] = array('default' => 0);
    $options['title_field'] = array('default' => '');
    
    if ($this->definition['element type'] == 'tabs') {
      $options['nav_position'] = array('default' => GMAPS_TABS_NAV_TOP);
    }
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
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
    
    $options = array('' => t('<None>'));
    foreach ($this->display->handler->get_handlers('field') as $field => $handler) {

      if ($label = $handler->label()) {
        $options[$field] = $label;
      }
      else {
        $options[$field] = $handler->ui_name();
      }
    }
    if (count($options) > 1) {
      $form['title_field'] = array('#type' => 'select',
        '#title' => t('Title field'),
        '#default_value' => $this->options['title_field'],
        '#options' => $options,
      );
    }
    else {
      $form['title_field'] = array('#type' => 'value', '#value' => '');
    }
  }
  
  function uses_fields() {
    return TRUE;
  }

  function render_fields($result) {
    if (isset($this->rendered_fields)) {
      return $this->rendered_fields;
    }

    $title_field = $this->options['title_field'];
    $this->view->row_index = 0;
    $keys = array_keys($this->view->field);
    foreach ($result as $count => $row) {
      $this->view->row_index = $count;
      foreach ($keys as $id) {
        $this->rendered_fields[$count][$id] = $this->view->field[$id]->theme($row);
      }
      
      if (!empty($title_field) && !empty($this->rendered_fields[$count][$title_field])) {
        $this->row_titles[$count] = $this->rendered_fields[$count][$title_field];
      }
      else {
        $this->row_titles[$count] = t('Row @count', array('@count' => $count + 1));
      }
    }
    unset($this->view->row_index);
  }
  
  function render() {
    if (empty($this->row_plugin)) {
      vpr('views_plugin_style_default: Missing row plugin');
      return;
    }

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    $multiple = count($sets) > 1;
    
    if ($this->definition['element type'] == 'tabs') {
      $tabs = TRUE;
      $element_type = 'gmaps_tabs';
      $panel_type = 'gmaps_tabs_panel';
      $class = 'gmaps-views-view-tabs';
    }
    else {
      $tabs = FALSE;
      $element_type = 'gmaps_accordion';
      $panel_type = 'gmaps_accordion_panel';
      $class = 'gmaps-views-view-accordion';
    }
    
    $options = $this->options;
    $element_info = _element_info($element_type);
    $panel_info = _element_info($panel_type);
    
    // Render each group separately and concatenate.  Plugins may override this
    // method if they wish some other way of handling grouping.
    $output = '';
    $this->view->set_index = 0;
    $this->view->row_index = 0;
    foreach ($sets as $title => $records) {
      $element = array('#type' => $element_type,
        '#id' => preg_replace('/[^a-z0-9]/', '-', 'gmaps-views-view-'. $this->view->name .'-'. $this->display->id . ($multiple ? '-'. $this->view->set_index : '')),
        '#collapsible' => $options['collapsible'],
        '#collapsed' => $options['collapsed'],
        '#attributes' => array('class' => $this->element_classes($class)),
      );
      if (!empty($title)) {
        $element['#title'] = $title;
      }
      if ($tabs) {
        $element['#nav_position'] = $options['nav_position'];
      }
      $element += $element_info;
      
      $rows = array();
      foreach ($records as $index => $row) {
        $panel_content = $this->row_plugin->render($row);
        $rows[] = $panel_content;
        $this->view->row_index++;
        
        $element[$index] = array('#type' => $panel_type,
          '#title' => $this->row_titles[$index],
          '#value' => $panel_content,
        );
        
        $element[$index] += $panel_info;
      }

      $this->view->gmaps_element = $element;
      $output .= theme($this->theme_functions(), $this->view, $this->options, $rows, $title);
      unset($this->view->gmaps_element);
      
      $this->view->set_index++;
    }
    unset($this->view->set_index, $this->view->row_index);
    return $output;
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
  
}
