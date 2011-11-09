<?php
// $Id: gmaps_views_plugin_row_marker_node.inc,v 1.1.2.3 2010/03/22 14:41:55 xmarket Exp $
/**
 * @file
 * Contains the node marker row style plugin for interactive and static maps.
 */

/**
 * Plugin which performs a node_view on the resulting object,
 * and adds it to the current map/static map.
 */
class gmaps_views_plugin_row_marker_node extends views_plugin_row implements gmaps_views_map_overlay_marker, gmaps_views_static_map_overlay_marker {
  // Basic properties that let the row style follow relationships.
  var $base_table = 'node';
  var $base_field = 'nid';
  
  function option_definition() {
    $options = parent::option_definition();

    $opts = array();
    $opts['build_mode'] = array('default' => 'teaser');
    $opts['links'] = array('default' => TRUE);
    $opts['comments'] = array('default' => FALSE);
    
    $tab_opts = array(
      'title' => array('default' => '', 'translatable' => TRUE),
      'content' => array('default' => array()),
      'inline' => array('default' => array()),
      'separator' => array('default' => ''),
      'hide_empty' => array('default' => FALSE),
    );
    for ($i = 0; $i < GMAPS_IW_MAX_TABS; $i++) {
      $tabs['tab_'. $i] = array('contains' => $tab_opts);
    }
    
    $options['min'] = array(
      'contains' => $opts,
    );
    $options['tabs'] = array(
      'contains' => $tabs,
    );
    $options['max'] = array(
      'contains' => $opts,
    );
    $options['max']['contains']['build_mode']['default'] = '';
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form = array_merge($form, $this->_options_form('min'), $this->_options_form_tabs(), $this->_options_form('max'));
  }
  
  function _options_form($context) {
    $options = array('' => t('Hidden'));
    if ($modes = module_invoke('content', 'build_modes')) {
      foreach ($modes as $key => $value) {
        if (isset($value['views style']) && $value['views style']) {
          $options[$key] = $value['title'];
        }
      }
    }
    else {
      $options = $options + array(
        'teaser' => t('Teaser'),
        'full' => t('Full node')
      );
    }
    
    $form[$context] = array(
      '#type' => 'fieldset',
      '#title' => ($context == 'min') ? t('Minimized content') : t('Maximized content'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );
    $form[$context]['build_mode'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => t('Build mode'),
      '#default_value' => $this->options[$context]['build_mode'],
     );
    $form[$context]['links'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display links'),
      '#default_value' => $this->options[$context]['links'],
    );
    $form[$context]['comments'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display node comments'),
      '#default_value' => $this->options[$context]['comments'],
    );
    
    return $form;
  }
  
  function _options_form_tabs() {
    $smap = $this->definition['static maps api'];
    $option_interface = $smap ? 'gmaps_views_static_map_overlay_option_marker' : 'gmaps_views_map_overlay_option_marker';
    $map_option_interface = $smap ? 'gmaps_views_static_map_option' : 'gmaps_views_map_option';
    
    $form = array();
    
    $fields = array();
    foreach ($this->display->handler->get_handlers('field') as $field => $handler) {
      if (!($handler instanceof $option_interface) && !($handler instanceof $map_option_interface)) {
        $fields[$field] = $handler->ui_name() . (($label = $handler->label()) ? ' '. $label : '');
      }
    }
    
    if (empty($fields)) {
      $form['tabs_markup'] = array(
        '#type' => 'item',
        '#value' => t('If you add fields to the display, you can display them on the tabs of the info window.'),
      );
      $form['tabs'] = array('#type' => 'value', '#value' => array());
    }
    else {
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
        $form['tabs'][$tab]['title'] = array(
          '#type' => 'textfield',
          '#title' => t('Title'),
          '#default_value' => $this->options['tabs'][$tab]['title'],
          '#description' => '<strong>'. t('Translatable') .'</strong>',
        );
        $form['tabs'][$tab]['content'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Content'),
          '#default_value' => $this->options['tabs'][$tab]['content'],
          '#options' => $fields,
        );
        $form['tabs'][$tab]['inline'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Inline fields'),
          '#options' => $fields,
          '#default_value' => $this->options['tabs'][$tab]['inline'],
          '#description' => t('Inline fields will be displayed next to each other rather than one after another.'),
        );
        $form['tabs'][$tab]['separator'] = array(
          '#title' => t('Separator'),
          '#type' => 'textfield',
          '#size' => 10,
          '#default_value' => $this->options['tabs'][$tab]['separator'],
          '#description' => t('The separator may be placed between inline fields to keep them from squishing up next to each other. You can use HTML in this field.'),
        );
        $form['tabs'][$tab]['hide_empty'] = array(
          '#type' => 'checkbox',
          '#title' => t('Hide empty fields'),
          '#default_value' => $this->options['tabs'][$tab]['hide_empty'],
          '#description' => t('Do not display fields, labels or markup for fields that are empty.'),
        );
      }
    }
    
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
    $values = $form_state['values']['row_options'];
    
    foreach($values['tabs'] as $tab => $options) {
      $form_state['values']['row_options']['tabs'][$tab]['content'] = array_filter($form_state['values']['row_options']['tabs'][$tab]['content']);
      $form_state['values']['row_options']['tabs'][$tab]['inline'] = array_filter($form_state['values']['row_options']['tabs'][$tab]['inline']);
    }
  }
  
  function query() {
    parent::query();
    
    $this->aliases = array();
    foreach(array('title', 'sticky') as $info) {
      $this->aliases[$info] = $this->view->query->add_field($this->base_table, $info);
    }
  }
  
  function render($row) {
    $this->view->gmaps_context = 'min';
    $output = theme($this->theme_functions(), $this->view, $this->options['min'], $row, $this->field_alias);
    unset($this->view->gmaps_context);
    return $output;
  }
  
  function add_overlay(&$gmi, $row) {
    if (!isset($gmi->overlays['marker'])) {
      $gmi->overlays['marker'] = array();
    }
    
    $nid = $row->{$this->field_alias};
    if (!is_numeric($nid)) {
      return;
    }
    $title = $row->{$this->aliases['title']};
    $sticky = $row->{$this->aliases['sticky']};
    
    $smap = $this->definition['static maps api'];
    $option_interface = $smap ? 'gmaps_views_static_map_overlay_option_marker' : 'gmaps_views_map_overlay_option_marker';
    $map_option_interface = $smap ? 'gmaps_views_static_map_option' : 'gmaps_views_map_option';
    $map_type = $smap ? GMAPS_MAP_EARTH : $gmi->base->type;
    
    $marker = array(
      'sticky' => $sticky,
      'title' => $title,
    );
    if (!$smap) {
      $marker['link'] = url('node/'. $nid);
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
    
    $tabs = array();
    foreach($this->options['tabs'] as $tab => $options) {
      if (!empty($options['content']) && !empty($options['title'])) {
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
        if ($output = theme($this->tab_theme_functions($tab), $this->view, $options, $row, $this->field_alias)) {
          $tabs[t($options['title'])] = $output;
        }
        unset($this->view->gmaps_fields);
      }
    }
      
    $ajax = FALSE;
    if (!empty($tabs)) {
      $content['tabs'] = $tabs;
    }
    else if (!gmaps_agent_is_search_engine()) {
      if ($smap) {
        if ($gmi->base->content['ajax'] && in_array($gmi->base->content['style'], array('accordion', 'tabs'))) {
          $gmi->content_ahah = array(
            'path' => 'gmaps/views/node-marker/autocomplete',
            'data' => array(
              'smi' => gmaps_extend(array(), $gmi),
              'view' => $this->view->name,
              'display' => $this->display->id,
              'field_alias' => $this->field_alias,
            ),
          );
          $marker_clone = gmaps_extend(array(), $marker);
          $marker_clone['id'] = 'row_'. $this->view->row_index;
          $content = array(
            'ajax' => array(
              'data' => array(
                'marker' => $marker_clone,
                'nid' => $nid,
              ),
            ),
          );
          $ajax = TRUE;
        }
      }
      else {
        if ($gmi->base->ov_data['marker']['misc']['ajaxcontent']) {
          if (!$gmi->base->map_data['toc_map']['enabled']) {
            $content['ajax'] = array(
              'path' => 'gmaps/views/node-marker/autocomplete',
              'data' => array(
                'view' => $this->view->name,
                'display' => $this->display->id,
                'nid' => $nid,
                'field_alias' => $this->field_alias,
              ),
            );
            $ajax = TRUE;
          }
          else if (in_array($gmi->base->map_data['toc_map']['style'], array('accordion', 'tabs'))) {
            $gmi->toc_ahah = array(
              'path' => 'gmaps/views/node-marker/autocomplete',
              'data' => array(
                'view' => $this->view->name,
                'display' => $this->display->id,
                'field_alias' => $this->field_alias,
              ),
            );
            $content = array(
              'ajax' => array(
                'data' => array(
                  'nid' => $nid,
                ),
              ),
            );
            $ajax = TRUE;
          }
        }
      }
    }
    
    if (!$ajax) {
      if (!empty($this->options['min']['build_mode'])) {
        $this->view->gmaps_context = 'min';
        if ($output = theme($this->theme_functions(), $this->view, $this->options['min'], $row, $this->field_alias)) {
          $content['min'] = $output;
        }
        unset($this->view->gmaps_context);
      }
      
      if (!empty($this->options['max']['build_mode'])) {
        $this->view->gmaps_context = 'max';
        if ($output = theme($this->theme_functions(), $this->view, $this->options['max'], $row, $this->field_alias)) {
          $content['max'] = $output;
        }
        unset($this->view->gmaps_context);
      }
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
    
    $map = $this->display->handler->get_plugin('style')->$mid_func();
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

  function tab_theme_functions($tab) {
    $hook = 'gmaps_views_view_fields';
    return $this->_tab_theme_functions($hook, $tab);
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
  
  protected function _tab_theme_functions($hook, $tab) {
    $tab_hook = $hook .'__tab__'. $tab;
    $view = $this->view;
    $display = $this->display;
    
    $themes = array();
  
    if ($display) {
      $themes[] = $tab_hook . '__' . $view->name . '__' . $display->id;
      $themes[] = $hook . '__' . $view->name . '__' . $display->id;
      $themes[] = $tab_hook . '__' . $display->id;
      $themes[] = $hook . '__' . $display->id;
      $themes[] = $tab_hook . '__' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      $themes[] = $hook . '__' . preg_replace('/[^a-z0-9]/', '-', strtolower($view->tag));
      if ($display->id != $display->display_plugin) {
        $themes[] = $tab_hook . '__' . $view->name . '__' . $display->display_plugin;
        $themes[] = $hook . '__' . $view->name . '__' . $display->display_plugin;
        $themes[] = $tab_hook . '__' . $display->display_plugin;
        $themes[] = $hook . '__' . $display->display_plugin;
      }
    }
    $themes[] = $tab_hook . '__' . $view->name;
    $themes[] = $hook . '__' . $view->name;
    $themes[] = $tab_hook;
    $themes[] = $hook;
    
    return $themes;
  }
}
