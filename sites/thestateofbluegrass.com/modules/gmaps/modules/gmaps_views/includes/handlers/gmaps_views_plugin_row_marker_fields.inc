<?php
// $Id: gmaps_views_plugin_row_marker_fields.inc,v 1.1.2.4 2010/03/24 10:40:05 xmarket Exp $
/**
 * @file
 * Contains the fields marker row style plugin for interactive and static maps.
 */

/**
 * Plugin which creates a merker based on the fields,
 * and adds it to the current map/static map.
 */
class gmaps_views_plugin_row_marker_fields extends views_plugin_row implements gmaps_views_map_overlay_marker, gmaps_views_static_map_overlay_marker {
  function option_definition() {
    $options = parent::option_definition();

    $options['title_field'] = array('default' => '');
    
    $title = array('title' => array('default' => '', 'translatable' => TRUE));
    $opts = array(
      'content' => array('default' => array()),
      'inline' => array('default' => array()),
      'separator' => array('default' => ''),
      'hide_empty' => array('default' => FALSE),
    );
    
    $options['min'] = array(
      'contains' => $opts,
    );
    
    for ($i = 0; $i < GMAPS_IW_MAX_TABS; $i++) {
      $tabs['tab_'. $i] = array('contains' => $title + $opts);
    }
    $options['tabs'] = array(
      'contains' => $tabs,
    );
    
    $options['max'] = array(
      'contains' => $opts,
    );
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $fields = array('' => t('<None>')) + $this->_get_field_options();
    
    $form['title_field'] = array('#type' => 'select',
      '#title' => t('Title field'),
      '#default_value' => $this->options['title_field'],
      '#options' => $fields,
    );
    
    if (count($fields) > 1) {
      $form['min'] = array(
        '#type' => 'fieldset',
        '#title' => t('Minimized content'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
      );
      $form['min'] = array_merge($form['min'], $this->_options_form($this->options['min']));
      
      $form['tabs'] = array(
        '#type' => 'fieldset',
        '#title' => t('Tabs'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
      );
      for ($i = 0; $i < GMAPS_IW_MAX_TABS; $i++) {
        $tab = 'tab_'. $i;
        $form['tabs'][$tab] = array(
          '#type' => 'fieldset',
          '#title' => t('Tab @count', array('@count' => $i + 1)),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#tree' => TRUE,
        );
        $form['tabs'][$tab] = array_merge($form['tabs'][$tab], $this->_options_form($this->options['tabs'][$tab], TRUE));
      }
      
      $form['max'] = array(
        '#type' => 'fieldset',
        '#title' => t('Maximized content'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => TRUE,
      );
      $form['max'] = array_merge($form['max'], $this->_options_form($this->options['max']));
    }
    else {
      $form['min'] = array('#type' => 'value', '#value' => array());
      $form['tabs'] = array('#type' => 'value', '#value' => array());
      $form['max'] = array('#type' => 'value', '#value' => array());
    }
  }
  
  function _get_field_options() {
    static $fields;
    
    if (is_null($fields)) {
      $smap = $this->definition['static maps api'];
      $option_interface = $smap ? 'gmaps_views_static_map_overlay_option_marker' : 'gmaps_views_map_overlay_option_marker';
      $map_option_interface = $smap ? 'gmaps_views_static_map_option' : 'gmaps_views_map_option';
      
      $fields = array();
      foreach ($this->display->handler->get_handlers('field') as $field => $handler) {
        if (!($handler instanceof $option_interface) && !($handler instanceof $map_option_interface)) {
          $fields[$field] = $handler->ui_name() . (($label = $handler->label()) ? ' '. $label : '');
        }
      }
    }
    
    return $fields;
  }
  
  function _options_form($options, $tab = FALSE) {
    $fields = $this->_get_field_options();

    $form = array();
    
    if ($tab) {
      $form['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value' => $options['title'],
        '#description' => '<strong>'. t('Translatable') .'</strong>',
      );
    }
    $form['content'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Content'),
      '#default_value' => $options['content'],
      '#options' => $fields,
    );
    $form['inline'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Inline fields'),
      '#options' => $fields,
      '#default_value' => $options['inline'],
      '#description' => t('Inline fields will be displayed next to each other rather than one after another.'),
    );
    $form['separator'] = array(
      '#title' => t('Separator'),
      '#type' => 'textfield',
      '#size' => 10,
      '#default_value' => $options['separator'],
      '#description' => t('The separator may be placed between inline fields to keep them from squishing up next to each other. You can use HTML in this field.'),
    );
    $form['hide_empty'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide empty fields'),
      '#default_value' => $options['hide_empty'],
      '#description' => t('Do not display fields, labels or markup for fields that are empty.'),
    );
        
    return $form;
  }
  
  function options_validate($form, &$form_state) {
    $values = $form_state['values']['row_options'];
    
    foreach($values['tabs'] as $tab => $options) {
      $content = array_filter($options['content']);
      if (!empty($content) && empty($options['title'])) {
        form_error($form['tabs'][$tab]['title'], t('The tab title is required when it has content.'));
      }
    }
  }
  
  function options_submit($form, &$form_state) {
    $form_state['values']['row_options']['min']['content'] = array_filter($form_state['values']['row_options']['min']['content']);
    $form_state['values']['row_options']['min']['inline'] = array_filter($form_state['values']['row_options']['min']['inline']);

    $form_state['values']['row_options']['max']['content'] = array_filter($form_state['values']['row_options']['max']['content']);
    $form_state['values']['row_options']['max']['inline'] = array_filter($form_state['values']['row_options']['max']['inline']);
    
    $values = $form_state['values']['row_options'];
    foreach($values['tabs'] as $tab => $options) {
      $form_state['values']['row_options']['tabs'][$tab]['content'] = array_filter($form_state['values']['row_options']['tabs'][$tab]['content']);
      $form_state['values']['row_options']['tabs'][$tab]['inline'] = array_filter($form_state['values']['row_options']['tabs'][$tab]['inline']);
    }
  }
  
  function theme_functions($section = NULL) {
    $hook = $this->definition['theme'];
    if (!$section) {
      $section = 'min';
    }
    return $this->_theme_functions($hook, $section);
  }
  
  function render($row) {
    return $this->_render_fields('min', $row, $this->options['min'], FALSE);
  }
  
  function add_overlay(&$gmi, $row) {
    if (!isset($gmi->overlays['marker'])) {
      $gmi->overlays['marker'] = array();
    }
    
    $smap = $this->definition['static maps api'];
    $option_interface = $smap ? 'gmaps_views_static_map_overlay_option_marker' : 'gmaps_views_map_overlay_option_marker';
    $map_type = $smap ? GMAPS_MAP_EARTH : $gmi->base->type;
    
    $marker = array();
    
    if (!empty($this->options['title_field']) && isset($this->view->field[$this->options['title_field']])) {
      $marker['title'] = $row->{$this->view->field[$this->options['title_field']]->field_alias};
    }

    $handlers = $this->view->field;
    foreach ($handlers as $id => $field) {
      if ($field instanceof $option_interface) {
        if ($smap) {
          $field->set_static_overlay_option($gmi, $marker, $row);
        }
        else {
          $field->set_overlay_option($gmi, $marker, $row);
        }
      }
    }
    
    if (!isset($marker['point']) || $marker['point']->map_type != $map_type) {
      return;
    }
    
    $content = array();
    
    if ($output = $this->_render_fields('min', $row, $this->options['min'], FALSE)) {
      $content['min'] = $output;
    }
    
    $tabs = array();
    foreach($this->options['tabs'] as $tab => $options) {
      if ($output = $this->_render_fields($tab, $row, $options, TRUE)) {
        $tabs[t($options['title'])] = $output;
      }
    }
    if (!empty($tabs)) {
      $content['tabs'] = $tabs;
    }
    
    if ($output = $this->_render_fields('max', $row, $this->options['max'], FALSE)) {
      $content['max'] = $output;
    }
    
    if (!empty($content)) {
      $marker['content'] = $content;
    }
    
    $gmi->overlays['marker']['row_'. $this->view->row_index] = $marker;
  }
  
  function add_static_overlay(&$smi, $row) {
    $this->add_overlay($smi, $row);
  }
  
  function validate() {
    $errors = array();
    
    $smap = $this->definition['static maps api'];
    $mid_func = $smap ? 'get_static_map_reference_mid' : 'get_map_reference_mid';
    $point_interface = $smap ? 'gmaps_views_static_map_overlay_option_marker_point' : 'gmaps_views_map_overlay_option_marker_point';
    
    $style_plugin = $this->display->handler->get_plugin('style');
    if ($style_plugin->$mid_func) {
      $map = $style_plugin->$mid_func();
    }
    if (empty($map)) {
      return;
    }
    $map = $smap ? gmaps_static_map_load($map) : gmaps_map_load($map);
    if (empty($map)) {
      return;
    }
    
    $has_point_handler = FALSE;
    foreach ($this->display->handler->get_handlers('field') as $field => $handler) {
      if ($handler instanceof $point_interface && ($smap || $handler->get_map_type() == $map->type)) {
        $has_point_handler = TRUE;
        break;
      }
    }
    
    if (!$has_point_handler) {
      if ($smap) {
        $errors[] = t('Row style @style requires a static marker point option handler field.', array('@style' => $this->definition['title']));
      }
      else {
        $errors[] = t('Row style @style requires a marker point option handler field.', array('@style' => $this->definition['title']));
      }
    }
    
    return $errors;
  }

  protected function _render_fields($section, $row, $options, $tab = FALSE) {
    if (!empty($options['content']) && (!$tab || !empty($options['title']))) {
      $handlers = $this->view->field;
      $fields = array();
      $inline = FALSE;
      foreach($handlers as $id => $field) {
        if ($options['content'][$id]) {
          if ($object = ($this->_build_field($this->view, $row, $id, $field, $options, $inline))) {
            $fields[$id] = $object;
          }
        }
      }
      $this->view->gmaps_fields = $fields;
      $output = theme($this->theme_functions($section), $this->view, $options, $row, $this->field_alias);
      unset($this->view->gmaps_fields);
      return $output;
    }
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
  
  protected function _theme_functions($hook, $section) {
    $section_hook = $hook .'__' . $section;
    $view = $this->view;
    $display = $this->display;
    
    $themes = array();
  
    if ($display) {
      $themes[] = $section_hook . '__' . $view->name . '__' . $display->id;
      $themes[] = $hook . '__' . $view->name . '__' . $display->id;
      $themes[] = $section_hook . '__' . $display->id;
      $themes[] = $hook . '__' . $display->id;
      $themes[] = $section_hook . '__' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      $themes[] = $hook . '__' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      if ($display->id != $display->display_plugin) {
        $themes[] = $section_hook . '__' . $view->name . '__' . $display->display_plugin;
        $themes[] = $hook . '__' . $view->name . '__' . $display->display_plugin;
        $themes[] = $section_hook . '__' . $display->display_plugin;
        $themes[] = $hook . '__' . $display->display_plugin;
      }
    }
    $themes[] = $section_hook . '__' . $view->name;
    $themes[] = $hook . '__' . $view->name;
    $themes[] = $section_hook;
    $themes[] = $hook;
    
    return $themes;
  }
}
