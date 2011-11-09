<?php
// $Id: gmaps_cmo.admin.inc,v 1.1.2.1 2010/02/18 16:36:22 xmarket Exp $

/**
 * @file
 * Administration functions
 */

function _gmaps_cmo_form_node_type_form_alter(&$form, &$form_state) {
  $type = $form['#node_type'];

  $mo_data = variable_get('gmaps_cmo_'. $type->type, _gmaps_cmo_get_defaults());

  $form['gmaps_cmo'] = array('#type' => 'fieldset',
    '#title' => t('GMaps marker options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  
  $icons = gmaps_get_icons();
  $form['gmaps_cmo']['icon'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Icon'),
    '#default_value' => $mo_data->icon,
    '#icons' => array(GMAPS_ICON_MAP_DEFAULT => GMAPS_INHERIT_LABEL, GMAPS_ICON_API_DEFAULT => t('API default')) + $icons,
  );
  $form['gmaps_cmo']['protected_icon'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Protected icon'),
    '#default_value' => $mo_data->protected_icon,
    '#icons' => array(GMAPS_ICON_MAP_DEFAULT => GMAPS_INHERIT_LABEL, GMAPS_ICON_API_DEFAULT => t('API default')) + $icons,
  );
  $form['gmaps_cmo']['static_icon'] = array('#type' => 'fieldset',
    '#title' => t('Static icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps_cmo']['static_icon']['style'] = array('#type' => 'gmaps_static_marker_style',
    '#title' => t('Style'),
    '#default_value' => $mo_data->static_icon['style'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps_cmo']['static_icon']['style_sticky'] = array('#type' => 'gmaps_static_marker_style',
    '#title' => t('Sticky style'),
    '#default_value' => $mo_data->static_icon['style_sticky'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps_cmo']['protected_static_icon'] = array('#type' => 'fieldset',
    '#title' => t('Protected static icon'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps_cmo']['protected_static_icon']['style'] = array('#type' => 'gmaps_static_marker_protected',
    '#title' => t('Style'),
    '#default_value' => $mo_data->protected_static_icon['style'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gmaps_cmo']['protected_static_icon']['style_sticky'] = array('#type' => 'gmaps_static_marker_protected',
    '#title' => t('Sticky style'),
    '#default_value' => $mo_data->protected_static_icon['style_sticky'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['#submit'][] = '_gmaps_cmo_form_node_type_form_submit';
}

function _gmaps_cmo_form_node_type_form_submit($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  $type = new stdClass();

  $type->type = trim($form_state['values']['type']);
  $type->old_type = isset($form_state['values']['old_type']) ? $form_state['values']['old_type'] : $type->type;
  
  $key = 'gmaps_cmo';
  $value = $form_state['values'][$key];
  
  $variable_new = $key .'_'. $type->type;
  $variable_old = $key .'_'. $type->old_type;

  if ($op == t('Reset to defaults')) {
    variable_del($variable_old);
  }
  else {
    if (empty($value['static_icon']['style']) && empty($value['static_icon']['style_sticky'])) {
      $edit['static_icon'] = array();
    }
    if (empty($value['protected_static_icon']['style']) && empty($value['protected_static_icon']['style_sticky'])) {
      $value['protected_static_icon'] = array();
    }
    variable_set($variable_new, (object)$value);

    if ($variable_new != $variable_old) {
      variable_del($variable_old);
    }
  }
}

function _gmaps_cmo_get_defaults() {
  return (object)array(
    'icon' => -1,
    'protected_icon' => -1,
    'static_icon' => array(
      'style' => array(
        'size' => 'small',
        'color' => 'red',
      ),
      'style_sticky' => array(
        'size' => 'normal',
      ),
    ),
    'protected_static_icon' => array(
      'style' => array(
        'quality' => 16,
        'style' => array(
          'color' => 'ff0000',
          'opacity' => '7f',
          'weight' => 5
        ),
      ),
      'style_sticky' => array(
        'style' => array(
          'opacity' => 'ff',
          'weight' => 10
        ),
      ),
    ),
  );
}
