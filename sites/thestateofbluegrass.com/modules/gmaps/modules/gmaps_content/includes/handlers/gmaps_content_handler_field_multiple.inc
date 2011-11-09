<?php
// $Id: gmaps_content_handler_field_multiple.inc,v 1.1.2.1 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file
 * An extended subclass for field handling that adds multiple field grouping.
 *
 * Fields that want multiple value grouping options in addition to basic
 * field and formatter handling can extend this class.
 */
class gmaps_content_handler_field_multiple extends gmaps_content_handler_field {
  var $defer_query;

  function init(&$view, $options) {
    parent::init($view, $options);
    $field = $this->get_content_field();
    
    $this->defer_query = !empty($options['multiple']['group']) && $field['multiple'];

    if ($this->defer_query) {
      // Grouped field: ditch the existing additional_fields (field columns + delta).
      // In the main query we'll only need:
      // - vid, which will be used to retrieve the actual values in pre_render,
      // - node type and nid, which wil be used in the pseudo-node used when
      // rendering.
      $this->additional_fields = array(
        'type' => array('table' => 'node', 'field' => 'type'),
        'nid' => array('table' => 'node', 'field' => 'nid'),
      );
      if ($view->base_table == 'node_revisions') {
        $this->additional_fields['vid'] = array('table' => 'node_revisions', 'field' => 'vid');
        $this->additional_fields['uid'] = array('table' => 'node_revisions', 'field' => 'uid');
      }
      else {
        $this->additional_fields['vid'] = array('table' => 'node', 'field' => 'vid');
        $this->additional_fields['uid'] = array('table' => 'node', 'field' => 'uid');
      }
    }
  }

  function options(&$options) {
    parent::options($options);
    $options['multiple'] = array(
      'group' => TRUE,
      'multiple_number' => '',
      'multiple_from' => '',
      'multiple_reversed' => FALSE,
    );
  }

  /**
   * Provide 'group multiple values' option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field = $this->get_content_field();
    $options = $this->options;

    $form['multiple'] = array(
      '#access' => $field['multiple'],
      '#weight' => 1,
    );
    $form['multiple']['group'] = array(
      '#title' => t('Group multiple values'),
      '#type' => 'checkbox',
      '#default_value' => $options['multiple']['group'],
      '#description' => t('If unchecked, each item in the field will create a new row, which may appear to cause duplicates. This setting is not compatible with click-sorting in table displays.'),
    );
    // Make the string translatable by keeping it as a whole rather than
    // translating prefix and suffix separately.
    list($prefix, $suffix) = explode('@count', t('Show @count value(s)'));
    $form['multiple']['multiple_number'] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#field_prefix' => $prefix,
      '#field_suffix' => $suffix,
      '#default_value' => $options['multiple']['multiple_number'],
      '#prefix' => '<div class="container-inline">',
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array(TRUE)),
    );
    list($prefix, $suffix) = explode('@count', t('starting from @count'));
    $form['multiple']['multiple_from'] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#field_prefix' => $prefix,
      '#field_suffix' => $suffix,
      '#default_value' => $options['multiple']['multiple_from'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array(TRUE)),
      '#description' => t('(first item is 0)'),
    );
    $form['multiple']['multiple_reversed'] = array(
      '#title' => t('Reversed'),
      '#type' => 'checkbox',
      '#default_value' => $options['multiple']['multiple_reversed'],
      '#suffix' => '</div>',
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array(TRUE)),
      '#description' => t('(start from last values)'),
    );
  }

  /**
   * Determine if this field is click sortable.
   */
  function click_sortable() {
    return !empty($this->definition['click sortable']) && !$this->defer_query && parent::click_sortable();
  }

  function query() {
    // If this is not a grouped field, use the generic query().
    if (!$this->defer_query) {
      return parent::query();
    }

    // Grouped field: do NOT call ensure_my_table, only add additional fields.
    $this->add_additional_fields();
    $this->field_alias = $this->aliases['vid'];
  }

  function pre_render($values) {
    // If there are no values to render (displaying a summary, or query returned no results),
    // or if this is not a grouped field, do nothing specific.
    if (isset($this->view->build_info['summary']) || empty($values) || !$this->defer_query) {
      return parent::pre_render($values);
    }
    
    $field = $this->get_content_field();

    $vids = array();
    foreach ($values as $result) {
      if (isset($result->{$this->field_alias}) && !isset($vids[$result->{$this->field_alias}])) {
        $node = drupal_clone($result);
        $node->type = $result->{$this->aliases['type']};
        $node->nid = $result->{$this->aliases['nid']};
        $node->vid = $result->{$this->aliases['vid']};
        $node->uid = $result->{$this->aliases['uid']};
        $node->build_mode = NODE_BUILD_NORMAL;
        $vids[$node->vid] = $node;
      }
    }

    // It may happend that the multiple values field is related to a non
    // required relation for which no node data related to the field being
    // processed here is available.
    if (empty($vids)) {
      return parent::pre_render($values);
    }

    $options = $this->options;
    $this->field_values = array();
    
    if ($field['type'] == 'gmaps_address') {
      foreach ($vids as $vid => $node) {
        $items = gmaps_content_address_field_load($node, $field, $options['multiple']['multiple_reversed']);
        if ($items && ($items == $this->_filter_items($items))) {
          $this->field_values[$vid] = $items;
        }
      }
    }
    else if ($field['type'] == 'gmaps_point') {
      foreach ($vids as $vid => $node) {
        $items = gmaps_content_point_field_load($node, $field, $options['multiple']['multiple_reversed']);
        if ($items && ($items == $this->_filter_items($items))) {
          $this->field_values[$vid] = $items;
        }
      }
    }
    if ($field['type'] == 'gmaps_anp') {
      foreach ($vids as $vid => $node) {
        $items = gmaps_content_anp_field_load($node, $field, $options['multiple']['multiple_reversed']);
        if ($items && ($items == $this->_filter_items($items))) {
          $this->field_values[$vid] = $items;
        }
      }
    }
  }
  
  private function _filter_items($items) {
    $options = $this->options;
    $result = array();
    foreach ($items as $delta => $item) {
      if (empty($options['multiple']['multiple_from']) || ($delta >= $options['multiple']['multiple_from'])) {
        if (empty($options['multiple']['multiple_number']) || (count($result) < $options['multiple']['multiple_number'])) {
          $result[] = $item;
        }
        else {
          break;
        }
      }
    }
    
    return $result;
  }

  /**
   * Return DIV or SPAN based upon the field's element type.
   *
   * Fields rendered with the 'group multiple' option use <div> markers,
   * and thus shouldn't be wrapped in a <span>.
   */
  function element_type() {
    if (!$this->defer_query) {
      return parent::element_type();
    }

    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }
    return 'div';
  }

  function render($values) {
    // If this is not a grouped field, use content_handler_field::render().
    if (!$this->defer_query) {
      return parent::render($values);
    }
    
    // We're down to a single node here, so we can retrieve the actual field
    // definition for the node type being considered.
    $field = $this->get_content_field();
    $field = content_fields($field['field_name'], $values->{$this->aliases['type']});
    $options = $this->options;
    
    $vid = $values->{$this->field_alias};
    if (isset($this->field_values[$vid])) {
      $items = $this->field_values[$vid];
      
      // Build a pseudo-node from the retrieved values.
      $node = drupal_clone($values);
      // content_format and formatters will need a 'type'.
      $node->type = $values->{$this->aliases['type']};
      $node->nid = $values->{$this->aliases['nid']};
      $node->vid = $values->{$this->aliases['vid']};
      $node->uid = $values->{$this->aliases['uid']};
      
      // Some formatters need to behave differently depending on the build_mode
      // (for instance: preview), so we provide one.
      $node->build_mode = NODE_BUILD_NORMAL;
      $node->{$field['field_name']} = $items;
      
      // Render items.
      $formatter_name = $options['format'];
      if ($formatter = _content_get_formatter($formatter_name, $field['type'])) {
        $rendered = array();
        $field['gmaps'] = array_merge($field['gmaps'], $options['gmaps']);
        $field['widget']['gmaps'] = array_merge($field['widget']['gmaps'], $options['gmaps']);
        if (content_handle('formatter', 'multiple values', $formatter) == CONTENT_HANDLE_CORE) {
          // Single-value formatter.
          foreach ($items as $item) {
            $output = gmaps_content_format($field, $item, $formatter_name, $node);
            if (!empty($output)) {
              $rendered[] = $this->render_link($output, $node);
            }
          }
        }
        else {
          // Multiple values formatter.
          $output = gmaps_content_format($field, $items, $formatter_name, $node);
          if (!empty($output)) {
            $rendered[] = $this->render_link($output, $node);
          }
        }

        if (count($rendered) > 1) {
          return theme('content_view_multiple_field', $rendered, $field, $values);
        }
        elseif ($rendered) {
          return $rendered[0];
        }
      }
    }

    return '';
  }
}