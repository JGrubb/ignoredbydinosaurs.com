<?php
// $Id: gmaps_views_handler_row_panel.inc,v 1.1.2.2 2010/03/22 14:41:55 xmarket Exp $

/**
 * A handler to provide a field that shows where to start a new panel
 * for accordion and tabs row style plugins.
 *
 * @ingroup views_field_handlers
 */
class gmaps_views_handler_row_panel extends views_handler_field {
  function query() {
    // do nothing -- to override the parent query.
  }

  function option_definition() {
    $options = parent::option_definition();

    // Override the alter text option to always alter the text.
    $options['alter']['contains']['alter_text'] = array('default' => TRUE);
    
    $options['inline'] = array('default' => array());
    $options['separator'] = array('default' => '');
    
    $options['css_id'] = array('default' => '');
    $options['selected_panel'] = array('default' => 0);
    
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Remove the checkbox
    unset($form['alter']['alter_text']);
    unset($form['alter']['text']['#dependency']);
    unset($form['alter']['text']['#process']);
    
    $form['hide_empty'] = array('#type' => 'value','#value' => FALSE);
    
    $options = array();
    $start = FALSE;
    foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
      if ($handler == $this) {
        $start = TRUE;
      }
      else if ($start && get_class($handler) == get_class($this)) {
        break; 
      }
      else if ($start) {
        if ($label = $handler->label()) {
          $options[$field] = $label;
        }
        else {
          $options[$field] = $handler->ui_name();
        }
      }
    }

    if (empty($options) || empty($this->options['inline'])) {
      $this->options['inline'] = array();
    }

    if (!empty($options)) {
      $form['inline'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Inline fields'),
        '#options' => $options,
        '#default_value' => $this->options['inline'],
        '#description' => t('Inline fields will be displayed next to each other rather than one after another.'),
      );

      $form['separator'] = array(
        '#title' => t('Separator'),
        '#type' => 'textfield',
        '#size' => 10,
        '#default_value' => isset($this->options['separator']) ? $this->options['separator'] : '',
        '#description' => t('The separator may be placed between inline fields to keep them from squishing up next to each other. You can use HTML in this field.'),
      );
    }

    $form['css_id'] = array(
      '#title' => t('CSS ID'),
      '#type' => 'textfield',
      '#size' => 10,
      '#default_value' => isset($this->options['css_id']) ? $this->options['css_id'] : '',
      '#description' => t('CSS ID to use for the panel.'),
    );
    $form['selected_panel'] = array('#type' => 'checkbox',
      '#title' => t('Select this panel by default'),
      '#default_value' => $this->options['selected_panel'],
    );
  }

  function options_submit($form, &$form_state) {
    if (isset($form_state['values']['row_options']['inline'])) {
      $form_state['values']['row_options']['inline'] = array_filter($form_state['values']['row_options']['inline']);
    }
  }
  
  function render($values) {
    // Nothing to render.
    return '';
  }
}
