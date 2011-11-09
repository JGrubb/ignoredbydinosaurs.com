<?php
// $Id: gmaps.static-map-admin.inc,v 1.1.2.8 2010/03/18 10:52:16 xmarket Exp $

/**
 * @file
 * Static map administration functions.
 */


/********************
 * Public functions *
 ********************/

/**
 * Static map administration page.
 * 
 * @return html
 */
function gmaps_static_map_admin_overview() {
  _gmaps_static_map_install_default_maps();

  $output = drupal_get_form('gmaps_static_map_admin_filter');
  
  $types = _gmaps_static_map_maptypes();
  $formats = _gmaps_static_map_format_options();
  
  $header = array(
    array('data' => t('Name'), 'field' => 'm.name', 'sort' => 'asc'),
    array('data' => t('Type'), 'field' => 'm.maptype'),
    array('data' => t('Format'), 'field' => 'm.format'),
    array('data' => t('Size')),
    array('data' => t('Operations'), 'colspan' => '1')
  );

  $rows = array();
  $path = 'admin/gmaps/static-map/';

  $sql = 'SELECT m.mid, m.* FROM {gmaps_static_maps} m';
  $filter = $_SESSION['gmaps_static_map_admin_filter']['sql'];
  $tablesort = tablesort_sql($header);
  if (!empty($filter)) {
    $result = pager_query($sql . $filter['query'] . $tablesort, 50, 0, NULL, $filter['args']);
  }
  else {
    $result = pager_query($sql . $tablesort, 50);
  }
  while ($rec = db_fetch_object($result)) {
    $ops = array(
      l(t('edit'), $path . $rec->mid .'/edit'),
      l(t('clone'), $path . $rec->mid .'/clone'),
    );
    if (!$rec->locked) {
      $ops[] = l(t('delete'), $path . $rec->mid .'/delete');
    }
    if ($rec->locked && $rec->modified) {
      $ops[] = l(t('reset'), $path . $rec->mid .'/reset');
    }
    $name = t($rec->name);
    $rows[] = array(
      l(drupal_strlen($name) > 15 ? drupal_substr($name, 0, 10).'...' : $name, $path . $rec->mid .'/edit', array('attributes' => array('title' => $name))),
      $types[$rec->maptype],
      $formats[$rec->format],
      gmaps_pixels_render(unserialize($rec->size), FALSE),
      implode(', ', $ops),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No static maps available.'), 'colspan' => '5', 'class' => 'message'));
  }

  $output .= theme('table', $header, $rows) . theme('pager', NULL, 50, 0);
  return $output;
}

/**
 * Filter form for static map admin overview.
 */
function gmaps_static_map_admin_filter() {
  $form = array();

  if (empty($_SESSION['gmaps_static_map_admin_filter'])) {
    $_SESSION['gmaps_static_map_admin_filter'] = array();
    $_SESSION['gmaps_static_map_admin_filter']['maptype'] = 'all';
    $_SESSION['gmaps_static_map_admin_filter']['format'] = 'all';
  }

  $types = array('all' => t('All types')) + _gmaps_static_map_maptypes();
  $formats = array('all' => t('All formats')) + _gmaps_static_map_format_options();
  
  $form['filter'] = array('#type' => 'fieldset',
    '#title' => t('Filters'),
    '#collapsible' => TRUE,
    '#collapsed' => !empty($_SESSION['gmaps_static_map_admin_filter']['sql']),
  );
  $form['filter']['maptype'] = array(
    '#type' => 'select',
    '#title' => t('Filter by map type'),
    '#options' => $types,
    '#default_value' => $_SESSION['gmaps_static_map_admin_filter']['maptype'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  $form['filter']['format'] = array(
    '#type' => 'select',
    '#title' => t('Filter by format'),
    '#options' => $formats,
    '#default_value' => $_SESSION['gmaps_static_map_admin_filter']['format'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );
  
  $form['filter']['submit'] = array('#type' => 'submit', '#value' => t('Filter'));
  $form['#redirect'] = FALSE;

  return $form;
}

function gmaps_static_map_admin_filter_submit($form, &$form_state) {
  $values = &$form_state['values'];
  $_SESSION['gmaps_static_map_admin_filter']['maptype'] = $values['maptype'];
  $_SESSION['gmaps_static_map_admin_filter']['format'] = $values['format'];
  
  $filters = $args = array();
  if ($values['maptype'] != 'all') {
    $filters[] = "m.maptype = '%s'";
    $args[] = $values['maptype'];
  }
  if ($values['format'] != 'all') {
    $filters[] = "m.format = '%s'";
    $args[] = $values['format'];
  }
  
  if (empty($filters)) {
    $_SESSION['gmaps_static_map_admin_filter']['sql'] = array();
  }
  else {
    $_SESSION['gmaps_static_map_admin_filter']['sql'] = array();
    $_SESSION['gmaps_static_map_admin_filter']['sql']['query'] = ' WHERE '. implode(' AND ', array_values($filters));
    $_SESSION['gmaps_static_map_admin_filter']['sql']['args'] = $args;
  }
}

/**
 * Static map add menu callback.
 */
function gmaps_static_map_add() {
  $map = _gmaps_static_map_get_defaults();
  
  $map = array_merge($map, array(
    'mid' => 0,
    'default_id' => '',
    'name' => '',
    'modified' => 0,
    'locked' => 0,
  ));

  drupal_alter('gmaps_static_map_defaults', $map);
  
  $map = (object)$map;
  
  return drupal_get_form('gmaps_static_map_form', $map);
}

/**
 * Static map edit menu callback.
 */
function gmaps_static_map_edit($map) {
  drupal_set_title(t('Edit static map preset %map', array('%map' => t($map->name))));

  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Static map presets'), 'admin/gmaps/static-maps');
  drupal_set_breadcrumb($bc);

  return drupal_get_form('gmaps_static_map_form', $map);
}

/**
 * Static map clone menu callback.
 */
function gmaps_static_map_clone($map) {
  drupal_set_title(t('Clone static map preset %map', array('%map' => t($map->name))));

  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Static map presets'), 'admin/gmaps/static-maps');
  drupal_set_breadcrumb($bc);

  $map->mid = 0;
  $map->default_id = '';
  $map->name = '';
  $map->modified = 0;
  $map->locked = 0;

  return drupal_get_form('gmaps_static_map_form', $map, TRUE);
}

/**
 * Static map reset confirm form.
 */
function gmaps_static_map_reset_confirm($form_state = NULL, $map) {
  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Static map presets'), 'admin/gmaps/static-maps');
  drupal_set_breadcrumb($bc);

  $form['#gmaps_static_map'] = $map;

  $form = confirm_form($form,
    t('Are you sure you want to reset the static map preset %title?', array('%title' => t($map->name))),
    'admin/gmaps/static-maps',
    t('This action cannot be undone.'),
    t('Reset'),
    t('Cancel')
  );

  $form['#submit'][] = '_gmaps_static_map_reset_submit';

  return $form;
}

/**
 * Static map delete menu callback.
 */
function gmaps_static_map_delete($map) {
  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Static map presets'), 'admin/gmaps/static-maps');
  drupal_set_breadcrumb($bc);

  $refs = gmaps_references_invoke('static_map', $map);
  if (!empty($refs)) {
    $output = '<p>'. t('Unable to delete this static map preset because of the following references. Remove them if you really want to delete the static map.') .'</p>';
    $output .= theme('gmaps_references', $refs);
    $output .= '<p>'. l(t('Cancel'), 'admin/gmaps/static-maps') .'</p>';
    return $output;
  }

  return drupal_get_form('_gmaps_static_map_delete_form', $map);
}

/**
 * Static map form used on add/edit/clone.
 *
 * @param array $form_state
 * @param object $map
 * @param boolean $clone
 *
 * @return array
 */
function gmaps_static_map_form(&$form_state, &$map, $clone = FALSE) {
  $form = _gmaps_static_map_form($form_state, $map, $clone);
  $form['#validate'][] = '_gmaps_static_map_form_validate';
  if ($clone) {
    $form['#submit'][] = '_gmaps_static_map_form_submit_clone';
  }
  else {
    $form['#submit'][] = '_gmaps_static_map_form_submit';
  }
  
  //form_alter helper for default icon providers
  if ($map->locked) {
    $data = &$form;
    $data['__drupal_alter_by_ref'] = array(&$form_state);
    drupal_alter('gmaps_default_static_map_form_'. $map->default_id, $data);
  }
    
  return $form;
}





/**********************
 * Internal functions *
 **********************/

function _gmaps_static_map_get_defaults() {
  //core defaults
  $map = array(
    'throttle_support' => 1,
    'throttle_lifetime' => 3600,
    'se_support' => 1,
    'size' => gmaps_pixels_parse(GMAPS_IMAGE_SMALL_LANDSCAPE, 'wh'),
    'center' => (object)array(
      'latitude' => 0,
      'longitude' => 0,
      'map_type' => GMAPS_MAP_EARTH,
      'bounds' => array(
        'northeast' => (object)array(
          'latitude' => 1,
          'longitude' => 1,
        ),
        'southwest' => (object)array(
          'latitude' => -1,
          'longitude' => -1,
        ),
      ),
    ),
    'maptype' => 'roadmap',
    'format' => 'gif',
    'fix_viewport' => 0,
    'zoom' => -1,
    'frame' => 0,
    'content' => array(
      'ajax' => 1,
      'icon' => 'before',
      'style' => 'accordion',
      'collapsible' => 1,
      'collapsed' => 0,
      'tabs_nav' => GMAPS_TABS_NAV_TOP,
      'grid' => array(
        'columns' => 2,
        'alignment' => GMAPS_GRID_ALIGN_VER,
      ),
      'ahah' => array(
        'progress' => '',
        'message' => '',
        'effect' => '',
      ),
      'content' => array(
        'style' => 'tabs',
        'collapsible' => 1,
        'collapsed' => 0,
        'tabs_nav' => GMAPS_TABS_NAV_TOP,
        'grid' => array(
          'columns' => 2,
          'alignment' => GMAPS_GRID_ALIGN_VER,
        ),
      ),
    ),
    'ov_data' => array(
      'marker' => array(
        'style' => array(
          'size' => 'small',
          'color' => 'red',
        ),
        'style_sticky' => array(
          'size' => 'normal',
        ),
        'protected' => array(
          'quality' => 16,
          'style' => array(
            'color' => 'ff0000',
            'opacity' => '7f',
            'weight' => 5
          ),
        ),
        'protected_sticky' => array(
          'style' => array(
            'opacity' => 'ff',
            'weight' => 10
          ),
        ),
        'labels' => '',
        'center_first' => 0,
        'route' => array(
          'enabled' => 0,
          'style' => array(
            'color' => '00ff00',
            'opacity' => '7f',
            'weight' => 5
          ),
        ),
      ),
      'path' => array(
        'style' => array(
          'color' => '0000ff',
          'opacity' => '7f',
          'weight' => 5
        ),
        'style_sticky' => array(
          'color' => '0000ff',
          'opacity' => 'ff',
          'weight' => 10
        ),
      ),
    ),
  );
  
  return $map;
}

/**
 * Generates the map editing form.
 */
function _gmaps_static_map_form(&$form_state, &$map, $clone = FALSE) {
  $form = array();

  $form['#gmaps_static_map'] = $map;
  $form['#gmaps_clone'] = $clone;
  
  $form['accordion'] = array('#type' => 'gmaps_accordion',
    '#id' => 'gmaps-static-map-form-accordion',
    '#collapsible' => TRUE,
    '#collapsed' => $map->mid,
  );
  
  $form['accordion']['map'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Map settings'),
    '#id' => 'map',
  );
  $form['accordion']['map']['name'] = array('#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $map->name,
    '#description' => t('<em>TRANSLATABLE.</em> The human-readable name of this static map. This text will be displayed in select list. This name must be unique.'),
    '#required' => TRUE,
  );
  $form['accordion']['map']['throttle_support'] = array('#type' => 'radios',
    '#title' => t('Throttle support'),
    '#default_value' => $map->throttle_support,
    '#options' => array(t('Disabled'), t('Enabled')),
  );
  $form['accordion']['map']['throttle_lifetime'] = array('#type' => 'textfield',
    '#title' => t('Throttle cache lifetime'),
    '#default_value' => $map->throttle_lifetime,
    '#required' => TRUE,
    '#description' => t('Minimum cache lifetime when throttle is active. Use 0 (zero) to disable cache.'),
    '#size' => 16,
    '#field_suffix' => t('seconds'),
  );
  $form['accordion']['map']['se_support'] = array('#type' => 'radios',
    '#title' => t('Search engine support'),
    '#default_value' => $map->se_support,
    '#options' => array(t('Disabled'), t('Enabled')),
  );
  $form['accordion']['map']['size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Map size'),
    '#default_value' => $map->size,
    '#required' => TRUE,
    '#return_value_keys' => 'wh',
    '#min_pixels' => GMAPS_IMAGE_ICON,
    '#max_pixels' => GMAPS_SM_IMAGE_MAX_SIZE,
    '#description' => t('Size of the image file.'),
  );
  $form['accordion']['map']['maptype'] = array('#type' => 'select',
    '#title' => t('Map type'),
    '#default_value' => $map->maptype,
    '#options' => _gmaps_static_map_maptypes(),
  );
  $form['accordion']['map']['center'] = array('#type' => 'gmaps_point',
    '#title' => t('Default center'),
    '#default_value' => $map->center,
    '#required' => TRUE,
    '#map_type' => GMAPS_MAP_EARTH,
    '#use_map' => TRUE,
    '#enable_pov' => FALSE,
  );
  $form['accordion']['map']['zoom'] = array('#type' => 'select',
    '#title' => t('Default zoom'),
    '#default_value' => $map->zoom,
    '#options' => array(-1 => t('Auto')) + range(0, 19),
  );
  $form['accordion']['map']['format'] = array('#type' => 'select',
    '#title' => t('File format'),
    '#default_value' => $map->format,
    '#options' => _gmaps_static_map_format_options(),
  );
  $form['accordion']['map']['fix_viewport'] = array('#type' => 'checkbox',
    '#title' => t('Fix viewport'),
    '#default_value' => $map->fix_viewport,
    '#description' => t('When enabled only content, that exists within the center bounds will be added to the map, otherwise the viewport will depend on the content bounds.'),
  );
  $form['accordion']['map']['frame'] = array('#type' => 'checkbox',
    '#title' => t('Add frame'),
    '#default_value' => $map->frame,
    '#description' => t('The frame consists of a 5 pixel, 55% opacity blue border.'),
  );
  
  $form['accordion']['map']['content'] = array('#type' => 'fieldset',
    '#title' => t('Content display settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['accordion']['map']['content']['ajax'] = array('#type' => 'checkbox',
    '#title' => t('Enable AJAX content loading'),
    '#default_value' => $map->content['ajax'],
  );
  $form['accordion']['map']['content']['icon'] = array('#type' => 'select',
    '#title' => t('Icon style display'),
    '#default_value' => $map->content['icon'],
    '#options' => array('' => t('Disabled'), 'before' => t('Before title'), 'after' => t('After title'), 'replace' => t('Replace title')),
  );
  $form['accordion']['map']['content']['style'] = array('#type' => 'select',
    '#title' => t('List style'),
    '#default_value' => $map->content['style'],
    '#options' => _gmaps_map_tab_styles(),
  );
  $form['accordion']['map']['content']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $map->content['collapsible'],
  );
  $form['accordion']['map']['content']['collapsed'] = array('#type' => 'checkbox',
    '#title' => t('Collapsed'),
    '#default_value' => $map->content['collapsed'],
  );
  $form['accordion']['map']['content']['tabs_nav'] = array('#type' => 'select',
    '#title' => t('Tabs navigation position'),
    '#default_value' => $map->content['tabs_nav'],
    '#options' => array(
      GMAPS_TABS_NAV_TOP => t('Top'),
      GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
      GMAPS_TABS_NAV_LEFT => t('Left'),
      GMAPS_TABS_NAV_RIGHT => t('Right'),
    ),
  );
  $form['accordion']['map']['content']['grid'] = array('#type' => 'fieldset',
    '#title' => t('Grid settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map']['content']['grid']['columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of columns'),
    '#default_value' => $map->content['grid']['columns'],
    '#required' => TRUE,
  );
  $form['accordion']['map']['content']['grid']['alignment'] = array(
    '#type' => 'radios',
    '#title' => t('Alignment'),
    '#options' => array(GMAPS_GRID_ALIGN_HOR => t('Horizontal'), GMAPS_GRID_ALIGN_VER => t('Vertical')),
    '#default_value' => $map->content['grid']['alignment'],
    '#description' => t('Horizontal alignment will place items starting in the upper left and moving right. Vertical alignment will place items starting in the upper left and moving down.'),
  );
  $form['accordion']['map']['content']['ahah'] = array('#type' => 'fieldset',
    '#title' => t('AHAH settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map']['content']['ahah']['progress'] = array('#type' => 'select',
    '#title' => t('Progress style'),
    '#default_value' => $map->content['ahah']['progress'],
    '#options' => array('' => t('Auto'), 'bar' => t('Progress bar'), 'throbber' => t('Throbber')),
  );
  $form['accordion']['map']['content']['ahah']['message'] = array('#type' => 'textfield',
    '#title' => t('Progress message'),
    '#default_value' => $map->content['ahah']['message'],
    '#description' => t('<strong>TRANSLATABLE</strong>. Leave empty for none.'),
  );
  $form['accordion']['map']['content']['ahah']['effect'] = array('#type' => 'textfield',
    '#title' => t('Effect'),
    '#default_value' => $map->content['ahah']['effect'],
    '#description' => t('Leave empty for none.'),
  );
  $form['accordion']['map']['content']['content'] = array('#type' => 'fieldset',
    '#title' => t('Content tabs settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map']['content']['content']['style'] = array('#type' => 'select',
    '#title' => t('Render tabs as'),
    '#default_value' => $map->content['content']['style'],
    '#options' => _gmaps_map_tab_styles(),
  );
  $form['accordion']['map']['content']['content']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $map->content['content']['collapsible'],
  );
  $form['accordion']['map']['content']['content']['collapsed'] = array('#type' => 'checkbox',
    '#title' => t('Collapsed'),
    '#default_value' => $map->content['content']['collapsed'],
  );
  $form['accordion']['map']['content']['content']['tabs_nav'] = array('#type' => 'select',
    '#title' => t('Tabs navigation position'),
    '#default_value' => $map->content['content']['tabs_nav'],
    '#options' => array(
      GMAPS_TABS_NAV_TOP => t('Top'),
      GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
      GMAPS_TABS_NAV_LEFT => t('Left'),
      GMAPS_TABS_NAV_RIGHT => t('Right'),
    ),
  );
  $form['accordion']['map']['content']['content']['grid'] = array('#type' => 'fieldset',
    '#title' => t('Grid settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map']['content']['content']['grid']['columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of columns'),
    '#default_value' => $map->content['content']['grid']['columns'],
    '#required' => TRUE,
  );
  $form['accordion']['map']['content']['content']['grid']['alignment'] = array(
    '#type' => 'radios',
    '#title' => t('Alignment'),
    '#options' => array(GMAPS_GRID_ALIGN_HOR => t('Horizontal'), GMAPS_GRID_ALIGN_VER => t('Vertical')),
    '#default_value' => $map->content['content']['grid']['alignment'],
    '#description' => t('Horizontal alignment will place items starting in the upper left and moving right. Vertical alignment will place items starting in the upper left and moving down.'),
  );
  
  
  //optional settings
  $form['accordion']['ov_data'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Overlay settings'),
    '#id' => 'overlay',
    '#tree' => TRUE,
  );
  $marker = &$map->ov_data['marker'];
  $form['accordion']['ov_data']['marker'] = array('#type' => 'fieldset',
    '#title' => t('Marker settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['marker']['style'] = array('#type' => 'gmaps_static_marker_style',
    '#title' => t('Style'),
    '#default_value' => $marker['style'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['marker']['style_sticky'] = array('#type' => 'gmaps_static_marker_style',
    '#title' => t('Sticky style'),
    '#default_value' => $marker['style_sticky'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['marker']['protected'] = array('#type' => 'gmaps_static_marker_protected',
    '#title' => t('Protected style'),
    '#default_value' => $marker['protected'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['marker']['protected_sticky'] = array('#type' => 'gmaps_static_marker_protected',
    '#title' => t('Sticky protected style'),
    '#default_value' => $marker['protected_sticky'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['marker']['labels'] = array('#type' => 'select',
    '#title' => t('Labels'),
    '#default_value' => $marker['labels'],
    '#options' => array(
      '' => t('Disabled'),
      'numeric' => t('Numeric sequence'),
      'letter' => t('Letters'),
      'both' => t('Numbers and letters'),
      'static' => t('Static'),
    ),
    '#description' => t('When set to static, then labels assigned to markers will be used.'),
  );
  $form['accordion']['ov_data']['marker']['center_first'] = array('#type' => 'checkbox',
    '#title' => t('Center map to first marker'),
    '#default_value' => $marker['center_first'],
    '#description' => t('Has effect only when fix viewport is enabled.'),
  );
  $form['accordion']['ov_data']['marker']['route'] = array('#type' => 'fieldset',
    '#title' => t('Route'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['marker']['route']['enabled'] = array('#type' => 'checkbox',
    '#title' => t('Enable routes'),
    '#default_value' => $marker['route']['enabled'],
  );
  $form['accordion']['ov_data']['marker']['route']['style'] = array('#type' => 'gmaps_static_path_style',
    '#title' => t('Style'),
    '#default_value' => $marker['route']['style'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $path = &$map->ov_data['path'];
  $form['accordion']['ov_data']['path'] = array('#type' => 'fieldset',
    '#title' => t('Path settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['path']['style'] = array('#type' => 'gmaps_static_path_style',
    '#title' => t('Style'),
    '#default_value' => $path['style'],
    '#required' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['ov_data']['path']['style_sticky'] = array('#type' => 'gmaps_static_path_style',
    '#title' => t('Sticky style'),
    '#default_value' => $path['style_sticky'],
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['button']['submit'] = array('#type' => 'submit',
    '#value' => $clone ? t('Clone') : t('Save'),
  );
  if (!$clone && isset($map->mid) && $map->mid > 0) {
    $form['button']['clone'] = array('#type' => 'submit',
      '#value' => t('Clone'),
      '#submit' => array('_gmaps_static_map_form_clone_submit'),
    );
    if ($map->locked && $map->modified) {
      $form['button']['reset'] = array('#type' => 'submit',
        '#value' => t('Reset'),
        '#submit' => array('_gmaps_static_map_form_reset_submit'),
      );
    }
    if (!$map->locked) {
      $form['button']['delete'] = array('#type' => 'submit',
        '#value' => t('Delete'),
        '#submit' => array('_gmaps_static_map_form_delete_submit'),
      );
    }
  }

  return $form;
}

function _gmaps_static_map_form_validate($form, &$form_state) {
  $map = &$form_state['values'];
  $map['mid'] = $form['#gmaps_clone'] ? 0 : $form['#gmaps_static_map']->mid;
  $map['default_id'] = (!$form['#gmaps_clone'] && $form['#gmaps_static_map']->default_id) ? $form['#gmaps_static_map']->default_id : md5(uniqid(mt_rand(), true));
  
  $exists = db_fetch_object(db_query("SELECT * FROM {gmaps_static_maps} m WHERE m.mid != %d AND m.name = '%s'", $map['mid'], $map['name']));
  if ($exists) {
    form_set_error('name', t('The name %name is already taken.', array('%name' => $map['name'])));
  }
  
  if (intval($map['throttle_lifetime']) != $map['throttle_lifetime'] || $map['throttle_lifetime'] < 0) {
    form_set_error('throttle_lifetime', t('Throttle cache lifetime must be an integer greater than or equal to zero.'));
  }
  
  $grid = $map['content']['grid'];
  if (intval($grid['columns']) != $grid['columns'] || $grid['columns'] < 1) {
    form_set_error('content][grid][columns', t('Content grid columns must be an integer greater than or equal to one.'));
  }
  $grid = $map['content']['content']['grid'];
  if (intval($grid['columns']) != $grid['columns'] || $grid['columns'] < 1) {
    form_set_error('content][content][grid][columns', t('Content tabs grid columns must be an integer greater than or equal to one.'));
  }
}

function _gmaps_static_map_form_submit($form, &$form_state) {
  $original_map = $form['#gmaps_static_map'];
  $map = &$form_state['values'];

  if ($original_map->locked) {
    $map['modified'] = 1;
  }

  $save = drupal_write_record('gmaps_static_maps', $map, $original_map->mid ? 'mid' : array());
  if ($save) {
    if ($save == SAVED_NEW) {
      drupal_set_message(t('The static map has been created.'));
    }
    else {
      drupal_set_message(t('The static map has been updated.'));
      $form_state['redirect'] = 'admin/gmaps/static-maps';
    }
  }
  if ($original_map->mid) {
    gmaps_static_map_cache_clear_all('%:b.'. $original_map->mid .':', NULL, TRUE);
    cache_clear_all();
  }
}

function _gmaps_static_map_form_submit_clone($form, &$form_state) {
  $map = $form['#gmaps_static_map'];
  $values = &$form_state['values'];
  
  if (_gmaps_static_map_clone($map, $values)) {
    drupal_set_message(t('The static map has been cloned.'));
  }
  $form_state['redirect'] = 'admin/gmaps/static-maps';
}

function _gmaps_static_map_clone(&$map, $values) {
  if (!isset($values['name']) || empty($values['name'])) {
    $values['name'] = $map->name .' - '. md5(uniqid(mt_rand(), true));
  }
  if (!isset($values['default_id']) || empty($values['default_id'])) {
    $values['default_id'] = md5(uniqid(mt_rand(), true));
  }
  
  $map = gmaps_extend($map, $values);
  $map->mid = 0;

  return drupal_write_record('gmaps_static_maps', $map);
}

/**
 * Submit handler for the clone button.
 * Redirects to the clone page.
 */
function _gmaps_static_map_form_clone_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/static-map/'. $form['#gmaps_static_map']->mid .'/clone';
}

/**
 * Submit handler for the reset button.
 * Redirects to the reset page.
 */
function _gmaps_static_map_form_reset_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/static-map/'. $form['#gmaps_static_map']->mid .'/reset';
}

/**
 * Submit handler for the delete button.
 * Redirects to the delete page.
 */
function _gmaps_static_map_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/static-map/'. $form['#gmaps_static_map']->mid .'/delete';
}

/**
 * Executes the map reset form.
 */
function _gmaps_static_map_reset_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $map = $form['#gmaps_static_map'];

    $default_maps = _gmaps_static_map_get_default_maps();
    $base_map = $default_maps[$map->default_id];
    $map = (object)gmaps_extend($base_map, array(
      'mid' => $map->mid,
      'modified' => 0,
      'locked' => 1,
    ));
    

    if (!db_result(db_query("SELECT 1 FROM {gmaps_static_maps} WHERE name = '%s' AND mid != %d", $base_map['name'], $map->mid))) {
      $map->name = $base_map['name'];
    }

    if (drupal_write_record('gmaps_static_maps', $map, 'mid')) {
      drupal_set_message(t('The static map has been reseted.'));
    }
    
    gmaps_static_map_cache_clear_all('%:b.'. $map->mid .':', NULL, TRUE);
    cache_clear_all();
  }
  
  $form_state['redirect'] = 'admin/gmaps/static-maps';
}

/**
 * Present a form for deleting a map.
 */
function _gmaps_static_map_delete_form($form_state = NULL, $map) {

  $form = array();
  $form['#gmaps_static_map'] = $map;
  $form['#submit'][] = '_gmaps_static_map_delete_form_submit';

  $output = confirm_form($form,
    t('Are you sure you want to delete the static map %map?', array('%map' => t($map->name))),
    'admin/gmaps/static-maps',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );

  return $output;
}

function _gmaps_static_map_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $map = &$form['#gmaps_static_map'];

    db_query("DELETE FROM {gmaps_static_maps} WHERE mid = %d", $map->mid);
    drupal_set_message(t('The static map has been deleted.'));
    
    gmaps_static_map_cache_clear_all('%:b.'. $map->mid .':', NULL, TRUE);
    cache_clear_all();
  }

  $form_state['redirect'] = 'admin/gmaps/static-maps';
}

function _gmaps_static_map_get_default_maps() {
  $maps = array();
  $defs = _gmaps_static_map_get_defaults();

  foreach (module_implements('gmaps_static_map_info') as $module) {
    $function = $module .'_gmaps_static_map_info';
    $result = $function('default_maps');
    if ($result) {
      foreach($result as $default_id => $map) {
        $map = gmaps_extend($defs, $map);
        
        $map['mid'] = 0;
        $map['default_id'] = $default_id;
        $map['modified'] = 0;
        $map['locked'] = 1;
        
        $maps[$default_id] = $map;
      }
    }
  }

  return $maps;
}

function _gmaps_static_map_install_default_maps() {
  $maps =  _gmaps_static_map_get_default_maps();

  if ($maps) {
    //skip previously installed maps
    $result = db_query("SELECT default_id FROM {gmaps_static_maps} WHERE locked = 1");
    while($rec = db_fetch_object($result)) {
      unset($maps[$rec->default_id]);
    }
    
    foreach($maps as $default_id => $map) {
      $values = array(
        'name' => $map['name'],
        'default_id' => $map['default_id'],
      );
      $map = (object)$map;
      $maps[$default_id] = _gmaps_static_map_clone($map, $values);
    }
  }

  return $maps;
}

function _gmaps_static_map_maptypes() {
  return array(
    'roadmap' => t('Roadmap'),
    'mobile' => t('Mobile'),
    'satellite' => t('Satellite'),
    'terrain' => t('Terrain'),
    'hybrid' => t('Hybrid'),
  );
}

function _gmaps_static_map_format_options() {
  static $formats;
  
  if (is_null($formats)) {
    $formats = array();
    foreach(_gmaps_static_map_formats() as $key => $data) {
      $formats[$key] = $data['name'];
    }
  }
  
  return $formats;
}