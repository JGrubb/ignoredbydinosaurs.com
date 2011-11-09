<?php
// $Id: gmaps_content_handler_field_distance.inc,v 1.1.2.1 2009/09/14 07:57:59 xmarket Exp $

class gmaps_content_handler_field_distance extends content_handler_field {

  function options(&$options) {
    global $user;
    parent::options($options);

    $options['gmaps'] = $user->gmaps_settings['length'];
    $options['gmaps']['user_settings'] = TRUE;
  }

  /**
   * Provide formatter option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $options = $this->options;

    $form['gmaps'] = array('#type' => 'fieldset',
      '#title' => t('GMaps settings'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($options['gmaps']),
      '#tree' => TRUE,
      '#weight' => 5,
    );
    
    $form['gmaps'] += _gmaps_call_func('gmaps_content', 'admin', '_gmaps_content_widget_settings_form_distance', array($options));
  }

  /**
   * Return DIV or SPAN based upon the field's formatter settings.
   */
  function element_type() {
    global $user;
    
    $options = $this->options;
    
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    else if ($options['format'] == 'themed' || ($options['format'] == 'default' && $user->gmaps_settings['length']['formatter'] == 'themed')) {
      return 'div';
    }
    else {
      return 'span';
    }
  }

  function render($values) {
    // We're down to a single node here, so we can retrieve the actual field
    // definition for the node type being considered.
    $field = content_fields($this->content_field['field_name'], $values->{$this->aliases['type']});
    $options = $this->options;
    $db_info = content_database_info($field);

    // Build a pseudo-node from the retrieved values.
    $node = drupal_clone($values);
    $node->type = $values->{$this->aliases['type']};
    $node->nid = $values->{$this->aliases['nid']};
    $node->vid = $values->{$this->aliases['vid']};
    // Some formatters need to behave differently depending on the build_mode
    // (for instance: preview), so we provide one.
    $node->build_mode = NODE_BUILD_NORMAL;
    
    $item = array();
    foreach ($db_info['columns'] as $column => $attributes) {
      $item[$column] = $values->{$this->aliases[$attributes['column']]};
    }
    
    if (gmaps_length_empty($item)) {
      return '';
    }

    $item['#delta'] = $field['multiple'] ?  $values->{$this->aliases['delta']} : 0;
    $node->{$field['field_name']}[$item['#delta']] = $item;
    
    // Render items.
    $formatter_name = $options['format'];
    if ($formatter = _content_get_formatter($formatter_name, $field['type'])) {
      $field['gmaps'] = array_merge($field['gmaps'], $options['gmaps']);
      $field['widget']['gmaps'] = array_merge($field['widget']['gmaps'], $options['gmaps']);
      if (content_handle('formatter', 'multiple values', $formatter) == CONTENT_HANDLE_CORE) {
        // Single-value formatter.
        $output = gmaps_content_format($field, $item, $formatter_name, $node);
      }
      else {
        // Multiple values formatter - we actually have only one value to display.
        $output = gmaps_content_format($field, array($item), $formatter_name, $node);
      }
      return $this->render_link($output, $values);
    }
    return '';
  }

}