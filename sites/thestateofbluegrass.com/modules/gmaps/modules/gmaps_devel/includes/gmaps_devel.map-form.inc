<?php
// $Id: gmaps_devel.map-form.inc,v 1.1.2.5 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * gmaps_map_item test form.
 */

function gmaps_devel_map_form(&$form_state = NULL) {
  $base = $form_state['values']['base'] ? $form_state['values']['base'] : gmaps_map_get_mid_for_default('input_earth');

  $map = gmaps_map_load($base);
  $marker = &$map->ov_data['marker'];
  $icons = gmaps_get_icons();
  $info = gmaps_map_get_info($map->type);
  
  $values = $form_state['values'] ? $form_state['values']['marker'] : array(
    'point' => $map->center,
    'title' => '',
    'basic' => $marker['basic'],
    'sticky' => 0,
    'options' => array(),
    'label' => '',
    'labeled' => array(),
    'protected_labeled' => array(),
    'action' => array(),
    'iw_data' => array(
      'google' => array(),
      'bb' => array(),
      'extended' => array(),
      'tmc' => array(),
    ),
    'link' => '',
    'link_target' => $marker['misc']['link_target'],
    'content' => array(
      'tabs' => array(),
    ),
    'tracker' => array(),
    'manager' => array(),
  );

  $form = array(
    '#gmaps_values' => $values,
    '#gmaps_map_base' => $base,
  );
  
  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['options']['base'] = array('#type' => 'select',
    '#title' => t('Base map'),
    '#default_value' => $base,
    '#options' => gmaps_map_get_options(),
  );
  
  $form['options']['marker'] = array('#type' => 'fieldset',
    '#title' => t('Marker options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['options']['marker']['point'] = array('#type' => 'gmaps_point',
    '#title' => t('Location'),
    '#default_value' => $values['point'],
    '#required' => TRUE,
    '#map_type' => $map->type,
    '#use_map' => $map,
    '#enable_pov' => TRUE,
    '#enable_privacy' => TRUE,
  );
  $form['options']['marker']['title'] = array('#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $values['title'],
    '#maxlength' => GMAPS_ICON_LABEL_MAX_LENGTH,
  );
  $form['options']['marker']['basic'] = array('#type' => 'gmaps_marker_basic',
    '#title' => t('Basic options'),
    '#default_value' => $values['basic'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['sticky'] = array('#type' => 'checkbox',
    '#title' => t('Sticky'),
    '#default_value' => $values['sticky'],
  );
  $form['options']['marker']['options'] = array('#type' => 'gmaps_marker_options',
    '#title' => t('Marker options'),
    '#default_value' => $values['options'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['label'] = array('#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $values['label'],
    '#maxlength' => GMAPS_ICON_LABEL_MAX_LENGTH,
  );
  $form['options']['marker']['labeled'] = array('#type' => 'gmaps_marker_labeled',
    '#title' => t('Labeled marker options'),
    '#default_value' => $values['labeled'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['protected_labeled'] = array('#type' => 'gmaps_marker_labeled',
    '#title' => t('Protected labeled marker options'),
    '#default_value' => $values['protected_labeled'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['action'] = array('#type' => 'gmaps_marker_action',
    '#title' => t('Actions'),
    '#default_value' => $values['action'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['iw_data'] = array('#type' => 'fieldset',
    '#title' => t('Info window options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['iw_data']['google'] = array('#type' => 'gmaps_iw_data_google',
    '#title' => t($info->info_windows_names['google']),
    '#default_value' => $values['iw_data']['google'],
    '#map_type' => $map->type,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['iw_data']['bb'] = array('#type' => 'gmaps_iw_data_bb',
    '#title' => t($info->info_windows_names['bb']),
    '#default_value' => $values['iw_data']['bb'],
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['iw_data']['extended'] = array('#type' => 'gmaps_iw_data_extended',
    '#title' => t($info->info_windows_names['extended']),
    '#default_value' => $values['iw_data']['extended'],
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['iw_data']['tmc'] = array('#type' => 'gmaps_iw_data_tmc',
    '#title' => t($info->info_windows_names['tmc']),
    '#default_value' => $values['iw_data']['tmc'],
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['link'] = array('#type' => 'textfield',
    '#title' => t('Link'),
    '#default_value' => $values['link'],
  );
  $form['options']['marker']['link_target'] = array('#type' => 'select',
    '#title' => t('Link target'),
    '#options' => array(
      '_blank' => t('Blank'),
      '_self' => t('Self'),
      '_parent' => t('Parent'),
      '_top' => t('Top'),
    ),
    '#default_value' => $values['link_target'],
  );
  
  $form['options']['marker']['content'] = array('#type' => 'fieldset',
    '#title' => t('Content'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['content']['title'] = array('#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $values['content']['title'],
  );
  $form['options']['marker']['content']['min'] = array('#type' => 'fieldset',
    '#title' => t('Minimized'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['content']['min']['body'] = array(
    '#type' => 'textarea',
    '#default_value' => $values['content']['min']['body'],
    '#rows' => 10,
  );
  $form['options']['marker']['content']['min']['format'] = filter_form($values['content']['min']['format'], NULL, array('marker', 'content', 'min', 'format'));

  $form['options']['marker']['content']['tabs'] = array('#type' => 'gmaps_tabs',
    '#title' => t('Tabs'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  for ($i = 0; $i < GMAPS_IW_MAX_TABS; $i++) {
    $form['options']['marker']['content']['tabs'][$i] = array('#type' => 'gmaps_tabs_panel',
      '#title' => t('Tab !n', array('!n' => $i+1)),
    );
    $form['options']['marker']['content']['tabs'][$i]['title'] = array('#type' => 'textfield',
      '#title' => t('Title'),
      '#default_value' => $values['content']['tabs'][$i]['title'],
    );
    $form['options']['marker']['content']['tabs'][$i]['body'] = array(
      '#type' => 'textarea',
      '#default_value' => $values['content']['tabs'][$i]['body'],
      '#rows' => 10,
    );
    $form['options']['marker']['content']['tabs'][$i]['format'] = filter_form($values['content']['tabs'][$i]['format'], NULL, array('marker', 'content', 'tabs', $i, 'format'));
  }
  
  $form['options']['marker']['content']['max'] = array('#type' => 'fieldset',
    '#title' => t('Maximized'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['content']['max']['body'] = array(
    '#type' => 'textarea',
    '#default_value' => $values['content']['max']['body'],
    '#rows' => 10,
  );
  $form['options']['marker']['content']['max']['format'] = filter_form($values['content']['max']['format'], NULL, array('marker', 'content', 'max', 'format'));
  
  $form['options']['marker']['tracker'] = array('#type' => 'gmaps_marker_tracker',
    '#title' => t('Tracker options'),
    '#default_value' => $opts['tracker'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['options']['marker']['manager'] = array('#type' => 'gmaps_marker_manager',
    '#title' => t('Managing options'),
    '#default_value' => $opts['manager'],
    '#map_type' => $map->type,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['result'] = array('#type' => 'fieldset',
    '#title' => t('Result'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['result']['map'] = array('#type' => 'markup',
    '#value' => '',
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#validate'][] = '_gmaps_devel_map_form_validate';
  $form['#submit'][] = '_gmaps_devel_map_form_submit';
  $form['#pre_render'][] = '_gmaps_devel_map_form_pre_render';
  
  return $form;
}

function _gmaps_devel_map_form_validate($form, &$form_state) {
  $content = &$form_state['values']['marker']['content'];
  foreach($content['tabs'] as $i => $tab) {
    if (!empty($tab['body']) && empty($tab['title'])) {
      form_set_error('marker][content][tabs]['. $i .'][title', t('The tab title is required.'));
    }
  }
  
  if (!form_get_errors()) {
    $tabs = array();
    foreach($content['tabs'] as $i => $tab) {
      if (!empty($tab['title'])) {
        $tabs[] = $tab;
      }
    }
    form_set_value($form['options']['marker']['content']['tabs'], $tabs, $form_state);
  }
}

function _gmaps_devel_map_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function _gmaps_devel_map_form_pre_render($form) {
  $base = $form['#gmaps_map_base'];
  $values = $form['#gmaps_values'];

  $form['result']['map']['#value'] = gmaps_map_item_render($base, 'gmaps_devel_map_form_gmi', array($values));
  
  return $form;
}

function gmaps_devel_map_form_gmi(&$gmi, $values) {
  $values['point'] = (object)$values['point'];
  
  $content = &$values['content'];
  if (gmaps_overlay_has_content($content)) {
    if ($gmi->base->ov_data['marker']['misc']['ajaxcontent']) {
      if ($gmi->base->map_data['toc_map']['enabled']) {
        $gmi->toc_ahah = array(
          'path' => 'devel/gmaps/maps/simple-content',
          'data' => array('marker' => array('content' => $content)),
        );
        $content = array('ajax' => TRUE);
      }
      else {
        $content['ajax'] = array('path' => 'devel/gmaps/maps/simple-content');
      }
    }
    else {
      _gmaps_devel_map_form_prepare_gmi_content($content);
    }
  }
  
  $gmi->overlays['marker']['test'] = $values;
}

function gmaps_devel_map_form_gmi_content() {
  $status = FALSE;
  $data = array();
  if (!empty($_POST)) {
    if (isset($_POST['#accordion'])) {
      $gmi = $_POST['#accordion']['data']['gmi'];
      $marker = $_POST['#accordion']['data']['marker'];
    }
    else if (isset($_POST['#tabs'])) {
      $gmi = $_POST['#tabs']['data']['gmi'];
      $marker = $_POST['#tabs']['data']['marker'];
    }
    else {
      $gmi = $_POST['gmi'];
      $marker = $_POST['marker'];
    }
    $data = _gmaps_devel_map_form_prepare_gmi_content($marker['content']);
    if ($gmi['base']['map_data']['toc_map']['enabled']) {
      $data = gmaps_overlay_render_content($data, $gmi['base']['map_data']['toc_map']['content'], $gmi['id'] .'-'. $marker['id']);
    }
    $status = TRUE;
  }
  gmaps_ajax_json(array('status' => TRUE, 'data' => $data));
  exit();
}

function _gmaps_devel_map_form_prepare_gmi_content(&$content) {
  if (!empty($content['min']['body'])) {
    $content['min'] = check_markup($content['min']['body'], $content['min']['format'], FALSE);
  }
  else {
    unset($content['min']);
  }
  if (!empty($content['max']['body'])) {
    $content['max'] = check_markup($content['max']['body'], $content['max']['format'], FALSE);
  }
  else {
    unset($content['max']);
  }
  if (!empty($content['tabs'])) {
    foreach($content['tabs'] as $i => $tab) {
      unset($content['tabs'][$i]);
      if (!empty($tab['title'])) {
        $content['tabs'][check_plain($tab['title'])] = empty($tab['body']) ? '' : check_markup($tab['body'], $tab['format'], FALSE);
      }
    }
  }
  else {
    unset($content['tabs']);
  }
  return $content;
}