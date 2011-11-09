<?php
// $Id: gmaps.icon-admin.inc,v 1.1.2.9 2009/10/16 11:17:00 xmarket Exp $

/**
 * @file
 * Icon administration functions.
 */


/************************
 * hook implementations *
 ************************/

/**
 * Implementation of hook_gmaps_icon_info().
 */
function gmaps_gmaps_icon_info($op) {
  if ($op == 'types') {
    $types = array();

    $types['custom'] = array(
      'name' => 'Custom',
      'file' => 'includes/gmaps.icon-admin-custom',
    );
    $types['marker'] = array(
      'name' => 'Marker',
      'file' => 'includes/gmaps.icon-admin-marker',
      'weight' => 4,
    );
    $types['labeled_marker'] = array(
      'name' => 'Labeled marker',
      'file' => 'includes/gmaps.icon-admin-labeled-marker',
      'label_length' => 2,
      'weight' => 1,
    );
    $types['flat'] = array(
      'name' => 'Flat',
      'file' => 'includes/gmaps.icon-admin-flat',
      'label_length' => -1,
      'weight' => 0,
    );
    $types['iconic_marker'] = array(
      'name' => 'Iconic marker',
      'file' => 'includes/gmaps.icon-admin-iconic-marker',
      'label_length' => 0,
      'weight' => 2,
    );
    $types['scaled_marker'] = array(
      'name' => 'Scaled marker',
      'file' => 'includes/gmaps.icon-admin-scaled-marker',
      'label_length' => -1,
      'weight' => 3,
    );

    return $types;
  }
  else if ($op == 'default_icons') {
    $icons = array();
    
    //throbber for ajax content loading
    $icons['throbber'] = array(
      'type' => 'custom',
      'name' => 'Default throbber',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'throbber.gif',
      'icon_size' => gmaps_pixels_parse(16, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('8x16', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('8x2', 'xy'),
      'imagemap' => '0,0,16,0,16,16,0,16',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    
    //bounds
    $icons['bound_ne'] = array(
      'type' => 'custom',
      'name' => 'Bound northeast',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'bound-ne.png',
      'icon_size' => gmaps_pixels_parse(32, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('32x0', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('32x0', 'xy'),
      'imagemap' => '0,0,32,0,32,32,0,32',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['bound_sw'] = array(
      'type' => 'custom',
      'name' => 'Bound southwest',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'bound-sw.png',
      'icon_size' => gmaps_pixels_parse(32, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('0x32', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('8x24', 'xy'),
      'imagemap' => '0,0,32,0,32,32,0,32',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    
    //privacy
    $icons['protected_small'] = array(
      'type' => 'custom',
      'name' => 'Protected small',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-small.png',
      'icon_size' => gmaps_pixels_parse(48, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('24x24', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('24x24', 'xy'),
      'imagemap' => '0,0,48,0,48,48,0,48',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky_id' => 'protected_small_sticky',
    );
    $icons['protected_small_sticky'] = array(
      'type' => 'custom',
      'name' => 'Protected small sticky',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-small-sticky.png',
      'icon_size' => gmaps_pixels_parse(48, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('24x24', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('24x24', 'xy'),
      'imagemap' => '0,0,48,0,48,48,0,48',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky' => 1,
    );
    $icons['protected_medium'] = array(
      'type' => 'custom',
      'name' => 'Protected medium',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-medium.png',
      'icon_size' => gmaps_pixels_parse(64, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('32x32', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('32x32', 'xy'),
      'imagemap' => '0,0,64,0,64,64,0,64',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky_id' => 'protected_medium_sticky',
    );
    $icons['protected_medium_sticky'] = array(
      'type' => 'custom',
      'name' => 'Protected medium sticky',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-medium-sticky.png',
      'icon_size' => gmaps_pixels_parse(64, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('32x32', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('32x32', 'xy'),
      'imagemap' => '0,0,64,0,64,64,0,64',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky' => 1,
    );
    $icons['protected_large'] = array(
      'type' => 'custom',
      'name' => 'Protected large',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-large.png',
      'icon_size' => gmaps_pixels_parse(100, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('50x50', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('50x50', 'xy'),
      'imagemap' => '0,0,100,0,100,100,0,100',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky_id' => 'protected_large_sticky',
    );
    $icons['protected_large_sticky'] = array(
      'type' => 'custom',
      'name' => 'Protected large sticky',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-large-sticky.png',
      'icon_size' => gmaps_pixels_parse(100, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('50x50', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('50x50', 'xy'),
      'imagemap' => '0,0,100,0,100,100,0,100',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky' => 1,
    );
    $icons['protected_xxl'] = array(
      'type' => 'custom',
      'name' => 'Protected extra large',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-xxl.png',
      'icon_size' => gmaps_pixels_parse(150, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('75x75', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('75x75', 'xy'),
      'imagemap' => '0,0,150,0,150,150,0,150',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky_id' => 'protected_xxl_sticky',
    );
    $icons['protected_xxl_sticky'] = array(
      'type' => 'custom',
      'name' => 'Protected extra large sticky',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'protected-xxl-sticky.png',
      'icon_size' => gmaps_pixels_parse(150, 'wh'),
      'icon_anchor' => gmaps_pixels_parse('75x75', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('75x75', 'xy'),
      'imagemap' => '0,0,150,0,150,150,0,150',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      'sticky' => 1,
    );
    
    //street view markers
    $compass = array('n', 'nne', 'ne', 'ene', 'e', 'ese', 'se', 'sse', 's', 'ssw', 'sw', 'wsw', 'w', 'wnw', 'nw', 'nnw');
    foreach($compass as $bearing) {
      $icons['svuman_'. $bearing] = array(
        'type' => 'custom',
        'name' => 'Street view man '. strtoupper($bearing),
        'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'svuman_'. $bearing .'.png',
        'icon_size' => gmaps_pixels_parse('49x52', 'wh'),
        'icon_anchor' => gmaps_pixels_parse('25x35', 'xy'),
        'info_window_anchor' => gmaps_pixels_parse('25x5', 'xy'),
        'imagemap' => '26,13, 30,14, 32,28, 27,28, 28,36, 18,35, 18,27, 16,26,16,20, 16,14, 19,13, 22,8',
        'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
        'transparent' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'svuman_tp.png',
      );
    }

    //cluster markers
    $levels = array('1', '2', '3', '4', '5');
    foreach($levels as $level) {
      $icons['cluster_default_'. $level] = array(
        'type' => 'custom',
        'name' => 'Cluster default '. $level,
        'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'m'. $level .'.png',
        'icon_size' => gmaps_pixels_parse('66x65', 'wh'),
        'icon_anchor' => gmaps_pixels_parse('33x35', 'xy'),
        'info_window_anchor' => gmaps_pixels_parse('33x5', 'xy'),
        'imagemap' => '0,0, 66,0, 66,65, 0,65',
        'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
      );
    }
    $icons['cluster_default_1']['icon_size'] = gmaps_pixels_parse('53x52', 'wh');
    $icons['cluster_default_1']['icon_anchor'] = gmaps_pixels_parse('27x26', 'xy');
    $icons['cluster_default_1']['info_window_anchor'] = gmaps_pixels_parse('27x5', 'xy');
    $icons['cluster_default_1']['imagemap'] = '0,0, 53,0, 53,52, 0,52';
    
    $icons['cluster_default_2']['icon_size'] = gmaps_pixels_parse('56x55', 'wh');
    $icons['cluster_default_2']['icon_anchor'] = gmaps_pixels_parse('28x27', 'xy');
    $icons['cluster_default_2']['info_window_anchor'] = gmaps_pixels_parse('28x5', 'xy');
    $icons['cluster_default_2']['imagemap'] = '0,0, 56,0, 56,55, 0,55';
    
    $icons['cluster_conv_30'] = array(
      'type' => 'custom',
      'name' => 'Cluster conversation 30',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'conv30.png',
      'icon_size' => gmaps_pixels_parse('30x27', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('30x27', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('30x27', 'xy'),
      'imagemap' => '0,0, 30,0, 30,27, 0,27',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['cluster_conv_40'] = array(
      'type' => 'custom',
      'name' => 'Cluster conversation 40',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'conv40.png',
      'icon_size' => gmaps_pixels_parse('40x36', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('40x36', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('40x36', 'xy'),
      'imagemap' => '0,0, 40,0, 40,36, 0,36',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['cluster_conv_50'] = array(
      'type' => 'custom',
      'name' => 'Cluster conversation 50',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'conv50.png',
      'icon_size' => gmaps_pixels_parse('50x45', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('50x45', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('50x45', 'xy'),
      'imagemap' => '0,0, 50,0, 50,45, 0,45',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    
    $icons['cluster_heart_30'] = array(
      'type' => 'custom',
      'name' => 'Cluster heart 30',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'heart30.png',
      'icon_size' => gmaps_pixels_parse('30x26', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('15x26', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('15x2', 'xy'),
      'imagemap' => '0,0, 30,0, 30,26, 0,26',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['cluster_heart_40'] = array(
      'type' => 'custom',
      'name' => 'Cluster heart 40',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'heart40.png',
      'icon_size' => gmaps_pixels_parse('40x35', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('20x35', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('20x2', 'xy'),
      'imagemap' => '0,0, 40,0, 40,35, 0,35',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['cluster_heart_50'] = array(
      'type' => 'custom',
      'name' => 'Cluster heart 50',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'heart50.png',
      'icon_size' => gmaps_pixels_parse('50x44', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('25x44', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('25x2', 'xy'),
      'imagemap' => '0,0, 50,0, 50,44, 0,44',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );

    $icons['cluster_people_35'] = array(
      'type' => 'custom',
      'name' => 'Cluster people 35',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'people35.png',
      'icon_size' => gmaps_pixels_parse('35x35', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('18x35', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('18x2', 'xy'),
      'imagemap' => '0,0, 35,0, 35,35, 0,35',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['cluster_people_45'] = array(
      'type' => 'custom',
      'name' => 'Cluster people 45',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'people45.png',
      'icon_size' => gmaps_pixels_parse('45x45', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('23x45', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('23x2', 'xy'),
      'imagemap' => '0,0, 45,0, 45,45, 0,45',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    $icons['cluster_people_55'] = array(
      'type' => 'custom',
      'name' => 'Cluster people 55',
      'icon' => drupal_get_path('module', 'gmaps') . GMAPS_ICON_RESOURCE_PATH .'people55.png',
      'icon_size' => gmaps_pixels_parse('55x55', 'wh'),
      'icon_anchor' => gmaps_pixels_parse('28x55', 'xy'),
      'info_window_anchor' => gmaps_pixels_parse('28x2', 'xy'),
      'imagemap' => '0,0, 55,0, 55,55, 0,55',
      'shadow_size' => gmaps_pixels_parse('0x0', 'wh'),
    );
    
    return $icons;
  }
}





/********************
 * Public functions *
 ********************/

function gmaps_icon_invoke($type, $op, $a1, &$a2 = NULL) {
  static $included = array();
  $info = gmaps_icon_get_types('type', $type);

  if (isset($info->file) && !isset($included[$type])) {
    module_load_include('inc', $info->module, $info->file);
    $included[$type] = TRUE;
  }

  $function = $info->module .'_gmaps_icon_'. $type;
  if (function_exists($function)) {
    return $function($op, $a1, $a2);
  }

  return NULL;
}

/**
 * Icon administration page.
 *
 * @return html
 */
function gmaps_icon_admin_overview() {
  _gmaps_icon_install_default_icons();
  
  $output = drupal_get_form('gmaps_icon_admin_filter');

  $types = gmaps_icon_get_types();

  $dynamic_t = t('Dynamic');
  $sticky_t = t('Sticky');
  $default_t = t('Pre-defined icon. Delete not allowed.');
  $header = array(
    array('data' => t('Prev.'), 'alt' => t('Preview'), 'title' => t('Preview')),
    array('data' => t('Name'), 'field' => 'i.name', 'sort' => 'asc'),
    array('data' => t('Type'), 'field' => 'i.type'),
    array('data' => t('Label'), 'field' => 'i.label'),
    array('data' => t('Size'), 'field' => 'i.icon_size'),
    array('data' => t('Operations'), 'colspan' => '1')
  );

  $rows = array();
  $path = 'admin/gmaps/icon/';
  $misc_path = drupal_get_path('module', 'gmaps') . '/misc/';

  $sql = 'SELECT i.icid, i.* FROM {gmaps_icons} i';
  $filter = $_SESSION['gmaps_icon_admin_filter']['sql'];
  $tablesort = tablesort_sql($header);
  if (!empty($filter)) {
    $result = pager_query($sql . $filter['query'] . $tablesort, 50, 0, NULL, $filter['args']);
  }
  else {
    $result = pager_query($sql . $tablesort, 50);
  }
  while ($rec = db_fetch_object($result)) {
    $ops = array(
      l(t('edit'), $path . $rec->icid .'/edit'),
      l(t('clone'), $path . $rec->icid .'/clone'),
    );
    if (!$rec->locked) {
      $ops[] = l(t('delete'), $path . $rec->icid .'/delete');
    }
    if (($rec->locked || $rec->base_id) && $rec->modified) {
      $ops[] = l(t('reset'), $path . $rec->icid .'/reset');
    }
    $type_name = t($types[$rec->type]->name);
    $name = t($rec->name);
    $rows[] = array(
      theme('gmaps_icon_preview', $rec->icon),
      l(drupal_strlen($name) > 15 ? drupal_substr($name, 0, 10).'...' : $name, $path . $rec->icid .'/edit', array('attributes' => array('title' => $name, 'alt' => $name))),
      l(drupal_strlen($type_name) > 15 ? drupal_substr($type_name, 0, 10).'...' : $type_name, 'admin/gmaps/icons/add/' . $types[$rec->type]->type_url),
      drupal_strlen($rec->label) > 15 ? drupal_substr($rec->label, 0, 10).'...' : $rec->label,
      gmaps_pixels_render(unserialize($rec->icon_size), FALSE),
      implode(', ', $ops),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No icons available.'), 'colspan' => '6', 'class' => 'message'));
  }

  $output .= theme('table', $header, $rows) . theme('pager', NULL, 50, 0);
  return $output;
}

/**
 * Filter form for icon admin overview.
 */
function gmaps_icon_admin_filter() {
  $form = array();

  $settings = gmaps_get_settings('gmaps_icon_settings');

  if (empty($_SESSION['gmaps_icon_admin_filter'])) {
    $_SESSION['gmaps_icon_admin_filter'] = array();
    $_SESSION['gmaps_icon_admin_filter']['type'] = 'all';
    $_SESSION['gmaps_icon_admin_filter']['sticky'] = 'all';
    $_SESSION['gmaps_icon_admin_filter']['dynamic'] = 'all';
  }

  $types = array('all' => t('All types')) + gmaps_icon_get_types('names');

  $sticky = array(
    'all' => t('All icons'),
    '0' => t('Non-sticky'),
    '1' => t('Sticky'),
  );

  $dynamic = array(
    'all' => t('All icons'),
    '0' => t('User-defined'),
    '-1' => t('All dynamic'),
  );

  $dynamic += gmaps_get_icons(NULL, FALSE);

  $form['filter'] = array('#type' => 'fieldset',
    '#title' => t('Filters'),
    '#collapsible' => TRUE,
    '#collapsed' => !empty($_SESSION['gmaps_icon_admin_filter']['sql']),
  );
  $form['filter']['type'] = array(
    '#type' => 'select',
    '#title' => t('Filter by type'),
    '#options' => $types,
    '#default_value' => $_SESSION['gmaps_icon_admin_filter']['type'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $form['filter']['sticky'] = array(
    '#type' => 'select',
    '#title' => t('Filter by sticky'),
    '#options' => $sticky,
    '#default_value' => $_SESSION['gmaps_icon_admin_filter']['sticky'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $form['filter']['dynamic'] = array(
    '#type' => 'gmaps_icon_select',
    '#title' => t('Filter by dynamic'),
    '#icons' => $dynamic,
    '#default_value' => $_SESSION['gmaps_icon_admin_filter']['dynamic'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#preview_max_size' => $settings['inline_preview_size'],
  );
  $form['filter']['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Filter by label'),
    '#default_value' => $_SESSION['gmaps_icon_admin_filter']['label'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#size' => GMAPS_ICON_LABEL_MAX_LENGTH + 3,
    '#maxlength' => GMAPS_ICON_LABEL_MAX_LENGTH,
  );

  $form['filter']['submit'] = array('#type' => 'submit', '#value' => t('Filter'));
  $form['#redirect'] = FALSE;

  return $form;
}

function gmaps_icon_admin_filter_submit($form, &$form_state) {
  $values = &$form_state['values'];
  $_SESSION['gmaps_icon_admin_filter']['type'] = $values['type'];
  $_SESSION['gmaps_icon_admin_filter']['sticky'] = $values['sticky'];
  $_SESSION['gmaps_icon_admin_filter']['dynamic'] = $values['dynamic'];
  $_SESSION['gmaps_icon_admin_filter']['label'] = $values['label'];

  $filters = $args = array();
  if ($values['type'] != 'all') {
    $filters[] = "i.type = '%s'";
    $args[] = $values['type'];
  }
  if ($values['sticky'] != 'all') {
    $filters[] = "i.sticky = %d";
    $args[] = intval($values['sticky']);
  }
  if ($values['dynamic'] != 'all') {
    if (intval($values['dynamic']) < 0) {
      $filters[] = "i.base_id != 0";
    }
    else {
      $filters[] = "i.base_id = %d";
      $args[] = intval($values['dynamic']);
    }
  }
  if (!empty($values['label']) || $values['label'] === '0') {
    $filters[] = "UPPER(i.label) LIKE UPPER('%s%%')";
    $args[] = $values['label'];
  }

  if (empty($filters)) {
    $_SESSION['gmaps_icon_admin_filter']['sql'] = array();
  }
  else {
    $_SESSION['gmaps_icon_admin_filter']['sql'] = array();
    $_SESSION['gmaps_icon_admin_filter']['sql']['query'] = ' WHERE '. implode(' AND ', array_values($filters));
    $_SESSION['gmaps_icon_admin_filter']['sql']['args'] = $args;
  }
}

/**
 * Icon add menu callback.
 */
function gmaps_icon_add($type, $info) {
  $icon = gmaps_icon_invoke('custom', 'defaults', $info);
  if ($type != 'custom') {
    $type_icon = gmaps_icon_invoke($type, 'defaults', $info);
  }
  if (!empty($type_icon)) {
    $icon = gmaps_extend($icon, $type_icon);
  }

  $icon = array_merge($icon, array(
    'icid' => 0,
    'default_id' => '',
    'type' => $type,
    'name' => '',
    'modified' => 0,
    'locked' => 0,
    'label_length' => $info->label_length,
    'base_id' => 0,
  ));

  drupal_alter('gmaps_icon_defaults', $icon);

  $icon = (object)$icon;

  return drupal_get_form('gmaps_icon_form', $icon, $info);
}

/**
 * Icon edit menu callback.
 */
function gmaps_icon_edit($icon) {
  drupal_set_title(t('Edit icon %icon', array('%icon' => t($icon->name))));

  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Icons'), 'admin/gmaps/icons');
  drupal_set_breadcrumb($bc);

  $info = gmaps_icon_get_types('type', $icon);

  return drupal_get_form('gmaps_icon_form', $icon, $info);
}

/**
 * Icon clone menu callback.
 */
function gmaps_icon_clone($icon) {
  drupal_set_title(t('Clone icon %icon', array('%icon' => t($icon->name))));

  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Icons'), 'admin/gmaps/icons');
  drupal_set_breadcrumb($bc);

  $info = gmaps_icon_get_types('type', $icon);

  $icon->icid = 0;
  $icon->default_id = '';
  $icon->name = '';
  $icon->label = '';
  $icon->modified = 0;
  $icon->locked = 0;


  return drupal_get_form('gmaps_icon_form', $icon, $info, TRUE);
}

/**
 * Icon reset confirm form.
 */
function gmaps_icon_reset_confirm($form_state = NULL, $icon) {
  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Icons'), 'admin/gmaps/icons');
  drupal_set_breadcrumb($bc);

  $form['#gmaps_icon'] = $icon;

  $form = confirm_form($form,
    t('Are you sure you want to reset %title?', array('%title' => t($icon->name))),
    'admin/gmaps/icons',
    t('This action cannot be undone.'),
    t('Reset'),
    t('Cancel')
  );

  $form['#submit'][] = '_gmaps_icon_reset_submit';

  return $form;
}

/**
 * Icon delete menu callback.
 */
function gmaps_icon_delete($icon) {
  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Icons'), 'admin/gmaps/icons');
  drupal_set_breadcrumb($bc);

  $refs = gmaps_references_invoke('icon', $icon);
  if (!empty($refs)) {
    $output = '<p>'. t('Unable to delete this icon because of the following references. Remove them if you really want to delete the icon.') .'</p>';
    $output .= theme('gmaps_references', $refs);
    $output .= '<p>'. l(t('Cancel'), 'admin/gmaps/icons') .'</p>';
    return $output;
  }

  return drupal_get_form('_gmaps_icon_delete_form', $icon);
}

/**
 * Icon form used on add/edit/clone.
 *
 * @param array $form_state
 * @param object $icon
 * @param object $info
 *  Icon type info
 * @param boolean $clone
 *
 * @return array
 */
function gmaps_icon_form(&$form_state, &$icon, &$info, $clone = FALSE) {
  $form = _gmaps_icon_form($form_state, $icon, $info, $clone);
  if ($clone) {
    $form['#validate'][] = '_gmaps_icon_form_validate_main';
    $form['#submit'][] = '_gmaps_icon_form_submit_clone';
  }
  else {
    $form['#validate'][] = '_gmaps_icon_form_validate_all';
    $form['#submit'][] = '_gmaps_icon_form_submit';
  }
  
  //form_alter helper for default icon providers
  if ($icon->locked) {
    $data = &$form;
    $data['__drupal_alter_by_ref'] = array(&$form_state);
    drupal_alter('gmaps_default_icon_form_'. $icon->default_id, $data);
  }

  return $form;
}



/**********************
 * Internal functions *
 **********************/

/**
 * Collect and prepare icon info from hook implementations.
 *
 * @return array of arrays
 */
function _gmaps_icon_get_types() {
  $types = array();
  foreach (module_implements('gmaps_icon_info') as $module) {
    $function = $module .'_gmaps_icon_info';
    $result = $function('types');
    if (isset($result)) {
      foreach($result as $key => $info) {
        //'info' can conflict hook_gmaps_icon_info(), 'api' conflicts in file info.
        if ($key != 'info' && $key != 'api') {
          $info['type'] = $key;
          $info['module'] = $module;
          $info['type_url'] = strtr($key, '_', '-');
          if (!isset($info['name'])) {
            //type is ascii only
            $info['name'] = ucfirst($info['type']);
          }
          if (!isset($info['label_length'])) {
            $info['label_length'] = 0;
          }
          else if ($info['label_length'] < 0 || $info['label_length'] > GMAPS_ICON_LABEL_MAX_LENGTH) {
            $info['label_length'] = GMAPS_ICON_LABEL_MAX_LENGTH;
          }
          if (!isset($info['weight'])) {
            $info['weight'] = 0;
          }
          $types[$key] = $info;
        }
      }
    }
  }
  if (!empty($types)) {
    drupal_alter('gmaps_icon_types', $types);
    cache_set('gmaps_icon_types', $types, 'cache');
  }

  return $types;
}

/**
 * Generates the icon editing form.
 */
function _gmaps_icon_form(&$form_state, &$icon, &$info, $clone = FALSE) {
  $form = array();
  $form['#has_gmaps_image_file'] = TRUE;

  $form['#gmaps_icon'] = $icon;
  $form['#gmaps_icon_info'] = $info;
  $form['#gmaps_icon_settings'] = gmaps_get_settings('gmaps_icon_settings');
  //should be checked in hook_form_alter implementations
  $form['#gmaps_clone'] = $clone;

  $settings = &$form['#gmaps_icon_settings'];

  $form['identity'] = array('#type' => 'fieldset',
    '#title' => t('Identification'),
    '#collapsible' => TRUE,
    '#collapsed' => !empty($icon->icid),
  );
  $form['identity']['name'] = array('#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $icon->name,
    '#description' => t('<em>TRANSLATABLE.</em> The human-readable name of this icon. This text will be displayed in select list. This name must be unique.'),
    '#required' => TRUE,
  );

  $form['settings'] = array('#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => !empty($icon->icid),
  );
  $form['settings']['throttle_support'] = array('#type' => 'radios',
    '#title' => t('Throttle support'),
    '#default_value' => $icon->throttle_support,
    '#options' => array(t('Disabled'), t('Enabled')),
  );
  $form['settings']['sticky'] = array('#type' => 'checkbox',
    '#title' => t('Sticky icon'),
    '#default_value' => $icon->sticky,
    '#access' => empty($icon->icid) && empty($icon->base_id),
    '#description' => t("Sticky icons are automatically replaces their normal counterparts on sticky nodes. They won't come up in select list."),
  );
  $form['settings']['sticky_id'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Sticky version'),
    '#default_value' => $icon->sticky_id,
    '#icons' => array(0 => t('Disabled')) + gmaps_get_icons(TRUE),
    '#description' => t('Choose an icon which will replace this one on sticky nodes.'),
    '#preview_max_size' => $settings['inline_preview_size'],
  );
  if (!empty($info->label_length)) {
    if ($clone || empty($icon->base_id)) {
      $form['settings']['label'] = array('#type' => 'textfield',
        '#title' => t('Label'),
        '#default_value' => $icon->label,
        '#size' => $info->label_length + 3,
        '#maxlength' => $info->label_length,
        '#required' => $icon->base_id ? TRUE : FALSE,
      );
    }
    else {
      $form['settings']['label'] = array('#type' => 'item',
        '#title' => t('Label'),
        '#value' => '<div id="edit-label">'. $icon->label .'</divy',
      );
    }
  }

  //type specific settings and image files are cloned during clone form submit,
  //so we can't edit them before the clone form has been submitted.
  if ($clone) {
    $form['button']['submit'] = array('#type' => 'submit',
      '#value' => t('Clone'),
    );
    return $form;
  }

  //type specific form
  $type_form = gmaps_icon_invoke($info->type, 'form', $form, $form_state);
  if ($type_form) {
    $form['data'] = array('#type' => 'fieldset',
      '#title' => t('!title icon settings', array('!title' => $info->name)),
      '#tree' => TRUE,
    );
    $form['data'] = array_merge($form['data'], $type_form);
  }

  $path = gmaps_get_path('icon');
  $form['drag_cross'] = array('#type' => 'fieldset',
    '#title' => t('Drag cross settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('If you do not set any cross image, than the API default is used.'),
  );
  $form['drag_cross']['maxheight'] = array('#type' => 'textfield',
    '#title' => t('Maximum rise'),
    '#default_value' => $icon->maxheight,
    '#size' => 5,
    '#maxlength' => 4,
    '#description' => t('Specifies the distance in pixels in which a marker will visually "rise" vertically when dragged.'),
  );
  $form['drag_cross']['drag_cross_image'] = array('#type' => 'gmaps_image_file',
    '#title' => t('Drag cross icon'),
    '#default_value' => $icon->drag_cross_image,
    '#image_min_size' => $settings['min_size'],
    '#image_max_size' => $settings['max_size'],
    '#image_max_filesize' => $settings['file_size'],
    '#preview_max_size' => $settings['preview_size'],
    '#file_dest_dir' => $path,
    '#file_replace_mode' => FILE_EXISTS_REPLACE,
    '#file_name_pattern' => 'drag-cross-#fid',
    '#description' => t('The cross image used when an icon is dragged.'),
  );
  $form['drag_cross']['drag_cross_anchor'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Drag cross anchor'),
    '#default_value' => $icon->drag_cross_anchor,
    '#min_pixels' => -512,
    '#max_pixels' => 512,
    '#enable_negative' => TRUE,
    '#return_value_keys' => 'xy',
    '#description' => t('Specifies the pixel coordinate offsets (relative to the iconAnchor) of the cross image when an icon is dragged.'),
  );

  $form['button']['submit'] = array('#type' => 'submit',
    '#value' => t('Save'),
  );
  if (isset($icon->icid) && $icon->icid > 0) {
    $form['button']['clone'] = array('#type' => 'submit',
      '#value' => t('Clone'),
      '#submit' => array('_gmaps_icon_form_clone_submit'),
    );
    if (($icon->locked || $icon->base_id != 0) && $icon->modified) {
      $form['button']['reset'] = array('#type' => 'submit',
        '#value' => t('Reset'),
        '#submit' => array('_gmaps_icon_form_reset_submit'),
      );
    }
    if (!$icon->locked) {
      $form['button']['delete'] = array('#type' => 'submit',
        '#value' => t('Delete'),
        '#submit' => array('_gmaps_icon_form_delete_submit'),
      );
    }
  }

  $form['#pre_render'][] = '_gmaps_icon_form_pre_render';

  return $form;
}

function _gmaps_icon_form_validate_main($form, &$form_state) {
  $icon = &$form_state['values'];
  $icon['icid'] = $form['#gmaps_icon']->icid;
  $icon['default_id'] = $form['#gmaps_icon']->default_id ? $form['#gmaps_icon']->default_id : md5(uniqid(mt_rand(), true));

  $exists = db_fetch_object(db_query("SELECT * FROM {gmaps_icons} i WHERE i.icid != %d AND i.name = '%s'", $icon['icid'], $icon['name']));
  if ($exists) {
    form_set_error('name', t('The name %name is already taken.', array('%name' => $icon['name'])));
  }
  if ($icon['sticky'] && $icon['sticky_id']) {
    form_set_error('sticky_id', t("Sticky icons cannot have sticky counter-parts."));
  }
}

function _gmaps_icon_form_validate_all($form, &$form_state) {
  $icon = &$form_state['values'];
  $icon['icid'] = $form['#gmaps_icon']->icid;

  //type specific validator
  gmaps_icon_invoke($form['#gmaps_icon']->type, 'validate', $form, $form_state);

  _gmaps_icon_form_validate_main($form, $form_state);

  if (!empty($icon['drag_cross_image']) && empty($icon['drag_cross_anchor'])) {
    form_set_error('drag_cross_anchor', t('Drag cross anchor is required, if drag cross icon is set.'));
  }

  if ($form['#gmaps_icon']->base_id) {
    $exists = db_fetch_object(db_query("SELECT * FROM {gmaps_icons} i WHERE i.icid != %d AND i.base_id = %d AND i.label = '%s'", $icon['icid'], $form['#gmaps_icon']->base_id, $icon['label']));
    if ($exists) {
      form_set_error('label', t('The dynamic icon for this label %label is already exists.', array('%label' => $icon['label'])));
    }
  }

}

function _gmaps_icon_form_submit($form, &$form_state) {
  $original_icon = $form['#gmaps_icon'];
  $icon = &$form_state['values'];

  gmaps_icon_invoke($original_icon->type, 'submit', $form, $form_state);

  $icon['type'] = $original_icon->type;
  if ($original_icon->locked || $original_icon->base_id) {
    $icon['modified'] = 1;
  }
  $icon['label_length'] = $form['#gmaps_icon_info']->label_length;

  if (!empty($icon['drag_cross_image'])) {
    $icon['drag_cross_iamge'] = $icon['drag_cross_image']->filepath;
    $info = image_get_info($icon['drag_cross_image']);
    $icon['drag_cross_size'] = gmaps_pixels_parse($info);
  }
  else {
    $icon['drag_cross_image'] = NULL;
    $icon['drag_cross_size'] = NULL;
    $icon['drag_cross_anchor'] = NULL;
  }

  $save = drupal_write_record('gmaps_icons', $icon, $original_icon->icid ? 'icid' : array());
  if ($save) {
    if ($save == SAVED_NEW) {
      drupal_set_message(t('The icon has been created.'));
    }
    else {
      drupal_set_message(t('The icon has been updated.'));
      $form_state['redirect'] = 'admin/gmaps/icons';
    }
  }
  
  if ($original_icon->icid) {
    gmaps_map_cache_clear_all('*', NULL, TRUE);
    cache_clear_all();
  }
}

function _gmaps_icon_form_submit_clone($form, &$form_state) {
  $icon = $form['#gmaps_icon'];

  if ($icon = _gmaps_icon_clone($icon, $form_state['values'])) {
    drupal_set_message(t('The icon has been cloned.'));
    $form_state['redirect'] = 'admin/gmaps/icon/'. $icon->icid .'/edit';
  }
  else {
    $form_state['redirect'] = 'admin/gmaps/icons';
  }
}

/**
 * Helper function to allow usage of drupal_add_css() and drupal_add_js() on cached forms.
 *
 * @param $form
 * @return array
 */
function _gmaps_icon_form_pre_render($form) {
  if ($altered_form = gmaps_icon_invoke($form['#gmaps_icon']->type, 'pre_render', $form)) {
    return $altered_form;
  }

  return $form;
}

function _gmaps_icon_add_color_form_client_files() {
  // Add Farbtastic color picker
  drupal_add_css('misc/farbtastic/farbtastic.css', 'module', 'screen', FALSE);
  drupal_add_js('misc/farbtastic/farbtastic.js', 'module', 'header', FALSE, TRUE, FALSE);

  _gmaps_add_maps_api();
  $base = drupal_get_path('module', 'gmaps');

  drupal_add_css($base .'/misc/gmaps-admin.css', 'module', 'screen', FALSE);
  drupal_add_js($base .'/misc/gmaps-icon.js', 'module', 'header', FALSE, TRUE, FALSE);
  drupal_add_js($base .'/misc/gmaps-icon-admin.js', 'module', 'header', FALSE, TRUE, FALSE);
}

function _gmaps_icon_validate_color_item($form, $form_state, $field) {
  if (!preg_match('|#[0-9A-Fa-f]{6,6}|', $form_state['values']['data']['color'][$field])) {
    form_set_error('data][color]['. $field, t('The !title must be given as a hexadecimal RGB value prefixed by a hashmark.', array('!title' => $form['data']['color'][$field]['#title'])));
  }
}

function _gmaps_icon_escape_label($label) {
  if (is_null($label) || $label == '') {
    return '';
  }
  $search = array('@', '\\', '\'', '[', ']');
  $replace = array('@@', '@\\', '@\'', '@[', '@]');
  $label = str_replace($search, $replace, $label);
  return drupal_urlencode($label);
}

/**
 * Download and save an icon file in a temporary file.
 * @param $url
 *  Icon URL to download.
 * @param $field
 *  (optional) Dummy field to assign form error to.
 *
 * @return string
 *  Path ofd the temporary file.
 */
function _gmaps_icon_data_save($url, $field = NULL) {
  global $user;

  $file = null;
  $result = drupal_http_request($url);
  switch ($result->code) {
    case 304:
    case 301:
    case 200:
    case 302:
    case 307:
      $dest = file_directory_temp() .'/'. md5(uniqid(mt_rand(), true));
      $file = file_save_data($result->data, $dest, FILE_EXISTS_RENAME);
      break;
    default:
      $args = array('%error' => $result->code .' '. $result->error, '!url' => $url);
      $msg = t('The icon maker chart seems to be broken, because of error "%error". Request: !url', $args);
      if (is_null($field)) {
        watchdog('gmaps icon chart', 'The icon maker chart seems to be broken, because of error "%error".', $args, WATCHDOG_ERROR, $url);
        if (!empty($user->uid)) {
          drupal_set_message($msg, 'error');
        }
      }
      else {
        form_set_error($field, $msg);
      }
      break;
  }
  return $file;
}

/**
 * Move temporary icon file (result of _gmaps_icon_data_save()) to its final location,
 * and create/update related files records.
 *
 * @param $key
 *  Which icon field. Used in icon file naming.
 * @param $source
 *  Path of the temporary file by _gmaps_icon_data_save().
 * @param $old_source
 *  Path of the previous icon file, if any.
 *
 * @return mixed
 *  Final file record or FALSE in case of error.
 */
function _gmaps_icon_file_save($key, $source, $old_source = NULL) {
  $image = image_get_info($source);

  if ($image) {
    if (empty($old_source)) {
      $file = new stdClass();
    }
    else {
      if (!($file = db_fetch_object(db_query("SELECT * FROM {files} f WHERE f.filepath = '%s'", $old_source)))) {
        $file = new stdClass();
        file_delete($old_source);
        $old_source = NULL;
      }
    }
    $file->filepath = $source;
    $file->filename = basename($source);
    $file->filemime = $image['mime_type'];
    $file->filesize = $image['file_size'];
    $file->uid = $user->uid;
    $file->status = FILE_STATUS_PERMANENT;
    $file->timestamp = time();

    $saved = drupal_write_record('files', $file, empty($old_source) ? array() : 'fid');

    if ($saved && file_move($file, (gmaps_get_path('icon') .'/'. strtr($key, '_', '-') .'-'. $file->fid .'.'. $image['extension']), FILE_EXISTS_REPLACE)) {
      if (drupal_write_record('files', $file, 'fid')) {
        return $file;
      }
    }
  }

  return FALSE;
}

function _gmaps_icon_file_delete($source) {
  if (file_delete($source)) {
    db_query("DELETE FROM {files} WHERE filepath = '%s'", $source);
    return TRUE;
  }

  return FALSE;
}

function _gmaps_icon_file_clone($key, $source) {
  global $user;
  $image = image_get_info($source);

  if ($image) {
    $tmp_file = $source;
    $dest = file_directory_temp() .'/'. md5(uniqid(mt_rand(), true));
    if (file_copy($tmp_file, $dest, FILE_EXISTS_RENAME)) {
      $file = new stdClass();
      $file->filepath = $tmp_file;
      $file->filename = basename($tmp_file);
      $file->filemime = $image['mime_type'];
      $file->filesize = $image['file_size'];
      $file->uid = $user->uid;
      $file->status = FILE_STATUS_PERMANENT;
      $file->timestamp = time();

      $saved = drupal_write_record('files', $file);

      if ($saved && file_move($file, (gmaps_get_path('icon') .'/'. strtr($key, '_', '-') .'-'. $file->fid .'.'. $image['extension']), FILE_EXISTS_REPLACE)) {
        if (drupal_write_record('files', $file, 'fid')) {
          return $file;
        }
      }
    }
  }

  return FALSE;
}

/**
 * Copy the content of an icon file to another.
 *
 * @param $source
 * @param $dest
 *
 * @return bool
 */
function _gmaps_icon_file_copy($source, $dest) {
  global $user;
  $image = image_get_info($source);

  if ($image && file_copy($source, $dest, FILE_EXISTS_REPLACE)) {
    db_query("UPDATE {files} SET uid = %d, filesize = %d, timestamp = %d WHERE filepath = '%s'", $user->uid, $image['file_size'], time(), $dest);
    return TRUE;
  }

  return FALSE;
}

function _gmaps_icon_file_move(&$source, $dest) {
  global $user;
  $file = $source;
  if (file_move($file, $dest, FILE_EXISTS_REPLACE)) {
    db_query("UPDATE {files} SET filepath = '%s', uid = %d, timestamp = %d WHERE filepath = '%s'", $file, $user->uid, time(), $source);
    $source = $file;
    return TRUE;
  }

  return FALSE;
}

function _gmaps_icon_clone($icon, $values) {
  gmaps_icon_invoke($icon->type, 'clone', $icon, $values);

  if (!isset($values['name']) || empty($values['name'])) {
    $values['name'] = $icon->name . (isset($values['label']) ? ' - '. $values['label'] : '') .' - '. md5(uniqid(mt_rand(), true));
  }
  if (!isset($values['default_id']) || empty($values['default_id'])) {
    $values['default_id'] = md5(uniqid(mt_rand(), true));
  }

  if (!empty($icon->drag_cross_image)) {
    $values['drag_cross_image'] = ($file = _gmaps_icon_file_clone('drag_cross_image', $icon->drag_cross_image)) ? $file->filepath : '';
  }

  foreach($values as $field => $value) {
    $icon->$field = $values[$field];
  }
  $icon->icid = 0;

  drupal_write_record('gmaps_icons', $icon);

  return $icon;
}

/**
 * Submit handler for the icon clone button.
 * Redirects to the clone page.
 */
function _gmaps_icon_form_clone_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/icon/'. $form['#gmaps_icon']->icid .'/clone';
}

/**
 * Submit handler for the icon reset button.
 * Redirects to the reset page.
 */
function _gmaps_icon_form_reset_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/icon/'. $form['#gmaps_icon']->icid .'/reset';
}

/**
 * Submit handler for the icon delete button.
 * Redirects to the delete page.
 */
function _gmaps_icon_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/icon/'. $form['#gmaps_icon']->icid .'/delete';
}

function _gmaps_icon_reset_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $icon = &$form['#gmaps_icon'];
    if ($icon->locked) {
      $default_icons = _gmaps_icon_get_default_icons();
      $base_icon = (object)$default_icons[$icon->default_id];
    }
    else {
      $base_icon = gmaps_icon_load($icon->base_id, FALSE, FALSE);
    }

    gmaps_icon_invoke($icon->type, 'reset', $base_icon, $icon);

    $icon->sticky_id = $base_icon->sticky_id;
    $icon->maxheight = $base_icon->maxheight;
    $icon->modified = 0;

    if ($icon->locked) {
      if (!db_result(db_query("SELECT 1 FROM {gmaps_icons} WHERE name = '%s' AND icid != %d", $base_icon->name, $icon->icid))) {
        $icon->name = $base_icon->name;
      }
    }

    if (!empty($base_icon->drag_cross_image) || !empty($icon->drag_cross_image)) {
      _gmaps_icon_image_field_reset('drag_cross_image', $base_icon, $icon);
      if (is_object($icon->drag_cross_image)) {
        $icon->drag_cross_image = $icon->drag_cross_image->filepath;
      }

      if (empty($icon->drag_cross_image)) {
        $icon->drag_cross_size = NULL;
        $icon->drag_cross_anchor = NULL;
      }
      else {
        $icon->drag_cross_size = $base_icon->drag_cross_size;
        $icon->drag_cross_anchor = $base_icon->drag_cross_anchor;
      }
    }

    drupal_write_record('gmaps_icons', $icon, 'icid');
  }
  $form_state['redirect'] = 'admin/gmaps/icons';

  gmaps_map_cache_clear_all('*', NULL, TRUE);
  cache_clear_all();
}

function _gmaps_icon_image_field_reset($field, $base_icon, &$icon) {
  if (!empty($base_icon->$field) && empty($icon->$field)) {
    $icon->$field = _gmaps_icon_file_clone($field, $base_icon->$field);
  }
  else if (!empty($base_icon->$field) && !empty($icon->$field)) {
    _gmaps_icon_file_copy($base_icon->$field, $icon->$field);
  }
  else {
    _gmaps_icon_file_delete($icon->$field);
    $icon->$field = NULL;
  }
}

/**
 * Present a form for deleting an icon.
 */
function _gmaps_icon_delete_form($form_state = NULL, $icon) {

  $form = array();
  $form['#gmaps_icon'] = $icon;
  $form['delete_files'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete icon files'),
    '#default_value' => 1,
  );
  $form['#submit'][] = '_gmaps_icon_delete_form_submit';

  $output = confirm_form($form,
    t('Are you sure you want to delete the icon %icon?', array('%icon' => t($icon->name))),
    'admin/gmaps/icons',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );

  return $output;
}

function _gmaps_icon_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $icon = &$form['#gmaps_icon'];

    $dynamics = gmaps_get_icons(NULL, $icon->icid);
    foreach($dynamics as $dynamic) {
      _gmaps_icon_delete($dynamic, $form_state['values']['delete_files']);
    }

    _gmaps_icon_delete($icon, $form_state['values']['delete_files']);
  }

  $form_state['redirect'] = 'admin/gmaps/icons';

  gmaps_map_cache_clear_all('*', NULL, TRUE);
  cache_clear_all();
}

function _gmaps_icon_delete($icon, $delete_files = TRUE) {
  if ($delete_files) {
    foreach(_gmaps_icon_get_file_fields() as $field) {
      if (!empty($icon->$field)) {
        _gmaps_icon_file_delete($icon->$field);
      }
    }
  }

  db_query("DELETE FROM {gmaps_icons} WHERE icid = %d", $icon->icid);
}

function _gmaps_icon_get_default_icons() {
  $types = gmaps_icon_get_types();
  $icons = array();
  $defaults = array('custom' => gmaps_icon_invoke('custom', 'defaults', $types['custom']));

  foreach (module_implements('gmaps_icon_info') as $module) {
    $function = $module .'_gmaps_icon_info';
    $result = $function('default_icons');
    if ($result) {
      foreach($result as $default_id => $icon) {
        if (isset($types[$icon['type']])) {
          if (!isset($defaults[$icon['type']])) {
            $defaults[$icon['type']] = ($defs = gmaps_icon_invoke($icon['type'], 'defaults', $types[$icon['type']])) ? $defs : array();
          }
          //fill default values
          $icon = array_merge($defaults['custom'], $icon);
          if ($icon['type'] != 'custom') {
            $icon = array_merge($defaults[$icon['type']], $icon);
          }

          $icon['icid'] = 0;
          $icon['default_id'] = $default_id;
          $icon['base_id'] = 0;
          $icon['modified'] = 0;
          $icon['locked'] = 1;

          $icons[$default_id] = $icon;
        }
      }
    }
  }

  return $icons;
}

/**
 * Install default icons
 * 
 * @param $icons
 *   INTERNAL ONLY!!
 * 
 * @return array of newly installed icons
 */
function _gmaps_icon_install_default_icons($icons = NULL) {
  if (empty($icons)) {
    $icons =  _gmaps_icon_get_default_icons();
  }
  $installed = array();

  if ($icons) {
    //skip previously installed icons
    $result = db_query("SELECT icid, default_id FROM {gmaps_icons} WHERE locked = 1");
    while($rec = db_fetch_object($result)) {
      unset($icons[$rec->default_id]);
      $installed[$rec->default_id] = $rec->icid;
    }

    foreach($icons as $default_id => $icon) {
      if (isset($icon['sticky_id']) && !empty($icon['sticky_id'])) {
        if (isset($installed[$icon['sticky_id']])) {
          $icon['sticky_id'] = $installed[$icon['sticky_id']];
        }
        else if (isset($icons[$icon['sticky_id']])) {
          $sticky_icon = $icons[$icon['sticky_id']];
          $values = array(
            'name' => $sticky_icon['name'],
            'default_id' => $sticky_icon['default_id'],
          );

          $sticky_icon = (object)$sticky_icon;
          $icons[$sticky_icon->default_id] = _gmaps_icon_clone($sticky_icon, $values);

          $installed[$sticky_icon->default_id] = $sticky_icon->icid;
          
          $icon['sticky_id'] = $sticky_icon->icid;
        }
        else {
          $icon['sticky_id'] = 0;
        }
      }
      //skip sticky incons, that installed above
      if (!isset($installed[$icon['default_id']])) {
        $values = array(
          'name' => $icon['name'],
          'default_id' => $icon['default_id'],
        );
        $icon = (object)$icon;
        $icons[$default_id] = _gmaps_icon_clone($icon, $values);
      }
    }
  }

  return $icons;
}
