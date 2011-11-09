<?php
// $Id: gmaps.map-admin.inc,v 1.1.2.14 2010/05/06 15:15:07 xmarket Exp $

/**
 * @file
 * Map administration functions.
 */


/************************
 * hook implementations *
 ************************/

/**
 * Implementation of hook_gmaps_map_info().
 */
function gmaps_gmaps_map_info($op, $type = NULL) {
  if ($op == 'types') {
    return array(
      'earth' => array(
        'name' => 'Earth',
        'coords' => array(
          'callbacks' => array(
            'line_end' => '_gmaps_coords_line_end_ellipsoid',
            'latlng_to_cart' => '_gmaps_coords_latlng_to_cart_ellipsoid',
            'distance' => '_gmaps_coords_distance_ellipsoid',
            'radius' => '_gmaps_coords_radius_ellipsoid',
          ),
          'arguments' => array(
            'a' => 6378137,
            'b' => 6356752.3142,
          ),
          'file' => 'includes/gmaps.coords-ellipsoid',
        ),
      ),
      'moon' => array(
        'name' => 'Moon',
        'coords' => array(
          'callbacks' => array(
            'line_end' => '_gmaps_coords_line_end_ellipsoid',
            'latlng_to_cart' => '_gmaps_coords_latlng_to_cart_ellipsoid',
            'distance' => '_gmaps_coords_distance_ellipsoid',
            'radius' => '_gmaps_coords_radius_ellipsoid',
          ),
          'arguments' => array(
            'a' => 1738140,
            'b' => 1735970,
          ),
          'file' => 'includes/gmaps.coords-ellipsoid',
        ),
      ),
      'mars' => array(
        'name' => 'Mars',
        'coords' => array(
          'callbacks' => array(
            'line_end' => '_gmaps_coords_line_end_ellipsoid',
            'latlng_to_cart' => '_gmaps_coords_latlng_to_cart_ellipsoid',
            'distance' => '_gmaps_coords_distance_ellipsoid',
            'radius' => '_gmaps_coords_radius_ellipsoid',
          ),
          'arguments' => array(
            'a' => 3396200,
            'b' => 3376200,
          ),
          'file' => 'includes/gmaps.coords-ellipsoid',
        ),
      ),
      'sky' => array(
        'name' => 'Sky',
        '3D_enabled' => FALSE,
      ),
    );
  }
  else if ($op == 'maptypes') {
    if ($type == GMAPS_MAP_EARTH) {
      return array(
        'normal' => array(
          'name' => 'Normal map',
          'max_zoom' => 17,
        ),
        'satellite' =>  array(
          'name' => 'Satellite map',
          'max_zoom' => 19,
        ),
        'hybrid' =>  array(
          'name' => 'Hybrid map',
          'max_zoom' => 19,
        ),
        'physical' =>  array(
          'name' => 'Physical map',
          'max_zoom' => 15,
        ),
        'satellite_3D' =>  array(
          'name' => '3D Satellite map',
          'max_zoom' => 30,
        ),
        'aerial' => array(
          'name' => 'Aerial map',
          'max_zoom' => 21,
        ),
        'aerial_hybrid' => array(
          'name' => 'Aerial hybrid map',
          'max_zoom' => 21,
        ),
        'mapmaker_normal' => array(
          'name' => 'Mapmaker normal map',
          'max_zoom' => 17,
        ),
        'mapmaker_hybrid' =>  array(
          'name' => 'Mapmaker hybrid map',
          'max_zoom' => 19,
        ),
      );
    }
    else if ($type == 'moon') {
      return array(
        'moon_elevation' =>  array(
          'name' => 'Moon elevation map',
          'max_zoom' => 7,
        ),
        'moon_visible' => array(
          'name' => 'Moon visible map',
          'max_zoom' => 9,
        ),
      );
    }
    else if ($type == 'mars') {
      return array(
        'mars_elevation' => array(
          'name' => 'Mars elevation map',
          'max_zoom' => 8,
        ),
        'mars_visible' => array(
          'name' => 'Mars visible map',
          'max_zoom' => 9,
        ),
        'mars_infrared' => array(
          'name' => 'Mars infrared map',
          'max_zoom' => 12,
        ),
      );
    }
    else if ($type == 'sky') {
      return array(
        'sky_visible' => array(
          'name' => 'Sky visible map',
          'max_zoom' => 19,
        ),
      );
    }
  }
  else if ($op == 'methods') {
    $methods = array(
      'dragging' => array(
        'name' => 'Dragging',
      ),
      'infowindow' => array(
        'name' => 'Info window',
      ),
      'dblclickzoom' => array(
        'name' => 'Double click zoom',
      ),
      'continuouszoom' => array(
        'name' => 'Continous zoom',
      ),
      'scrollwheelzoom' => array(
        'name' => 'Scroll wheel zoom',
      ),
      'keyboardhandler' => array(
        'name' => 'Keyboard handler',
      ),
      'auto_center_zoom' => array(
        'name' => 'Automatic center and zoom',
        'file' => 'includes/gmaps.map-admin-methods'
      ),
      'resize' => array(
        'name' => 'Height resize',
      ),
      'rotation' => array(
        'name' => 'Rotation',
      ),
      'keydragzoom' => array(
        'name' => 'Key drag zoom',
        'file' => 'includes/gmaps.map-admin-methods',
        'js_files' => array(
          'keydragzoom' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'keydragzoom.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'keydragzoom_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      ),
    );
    if ($type == GMAPS_MAP_EARTH) {
      $methods['googlebar'] = array(
        'name' => 'Google bar',
        'file' => 'includes/gmaps.map-admin-methods'
      );
      $methods['adsmanager'] = array(
        'name' => 'AdSense manager',
        'file' => 'includes/gmaps.map-admin-methods'
      );
      $methods['traffic'] = array(
        'name' => 'Traffic',
        'file' => 'includes/gmaps.map-admin-methods'
      );
      $methods['streetview'] = array(
        'name' => 'Street view support',
        'file' => 'includes/gmaps.map-admin-methods'
      );
      $methods['layers'] = array(
        'name' => 'Layers',
        'file' => 'includes/gmaps.map-admin-methods'
      );
    }
    return $methods;
  }
  else if ($op == 'info_windows') {
    return array(
      'google' => array(
        'name' => 'API default info window',
        'file' => 'includes/gmaps.map-admin-info-windows'
      ),
      'bb' => array(
        'name' => 'Bulletin board info window',
        'file' => 'includes/gmaps.map-admin-info-windows'
      ),
      'extended' => array(
        'name' => 'Extended info window',
        'file' => 'includes/gmaps.map-admin-info-windows',
        'js_files' => array(
          'extinfowindow' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'extinfowindow.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'extinfowindow_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      ),
      'tmc' => array(
        'name' => 'Tabbed max content info window',
        'file' => 'includes/gmaps.map-admin-info-windows',
        'js_files' => array(
          'tabbedmaxcontent' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'tabbedmaxcontent.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'tabbedmaxcontent_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      ),
    );
  }
  else if ($op == 'control_types') {
    $control_types = array(
      'maptype' => array(
        'name' => 'Map type',
      ),
      'navigation' => array(
        'name' => 'Navigation',
      ),
      'scale' => array(
        'name' => 'Scale',
      ),
      'overview' => array(
        'name' => 'Overview',
      ),
      'dragzoom' => array(
        'name' => 'Drag zoom',
      ),
    );
    if ($type == GMAPS_MAP_EARTH) {
      $control_types['breadcrumb'] = array(
        'name' => 'Breadcrumb',
      );
    }
    return $control_types;
  }
  else if ($op == 'controls') {
    $controls = array(
      'smallzoom' => array(
        'name' => 'Small zoom control',
        'control_type' => 'navigation',
      ),
      'smallmap' => array(
        'name' => 'Small map control',
        'control_type' => 'navigation',
      ),
      'largemap' => array(
        'name' => 'Large map control',
        'control_type' => 'navigation',
      ),
      'smallzoom3D' => array(
        'name' => 'Small zoom 3D control',
        'control_type' => 'navigation',
      ),
      'largemap3D' => array(
        'name' => 'Large map 3D control',
        'control_type' => 'navigation',
      ),
      'extnav' => array(
        'name' => 'Extended navigation control',
        'control_type' => 'navigation',
        'api_version' => 'largemap3D',
        'file' => 'includes/gmaps.map-admin-controls',
        'js_files' => array(
          'extlargemapcontrol' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'extlargemapcontrol.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'extlargemapcontrol_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      ),
      'scale' => array(
        'name' => 'Scale control',
        'control_type' => 'scale',
      ),
      'maptype' => array(
        'name' => 'Map type control',
        'control_type' => 'maptype',
        'file' => 'includes/gmaps.map-admin-controls',
      ),
      'menumaptype' => array(
        'name' => 'Menu map type control',
        'control_type' => 'maptype',
        'file' => 'includes/gmaps.map-admin-controls',
      ),
      'hierarchical_maptype' => array(
        'name' => 'Hierarchical map type control',
        'control_type' => 'maptype',
      ),
      'overview' => array(
        'name' => 'Overview control',
        'control_type' => 'overview',
        'position' => GMAPS_MAP_CONTROL_POSITION_STATIC,
      ),
      'extdragzoom' => array(
        'name' => 'Extended drag zoom control',
        'control_type' => 'dragzoom',
        'api_version' => FALSE,
        'file' => 'includes/gmaps.map-admin-controls',
        'js_files' => array(
          'extdragzoom' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'dragzoom.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'dragzoom_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      ),
    );
    if ($type == GMAPS_MAP_EARTH) {
      $controls['extmaptype'] = array(
        'name' => 'Extended map type control',
        'control_type' => 'maptype',
        'api_version' => 'maptype',
        'file' => 'includes/gmaps.map-admin-controls',
        'js_files' => array(
          'extmaptypecontrol' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'extmaptypecontrol.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'extmaptypecontrol_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      );
      $controls['breadcrumb'] = array(
        'name' => 'Breadcrumb control',
        'control_type' => 'breadcrumb',
      );
    }
    return $controls;
  }
  else if ($op == 'overlays') {
    $overlays = array(
      'marker' => array(
        'name' => 'Marker',
        'file' => 'includes/gmaps.map-admin-ov-marker',
        'js_files' => array(
          'api' => array(
            'plain' => array('/misc/gmaps-marker.js'),
          ),
          'labeledmarker' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'labeledmarker.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'labeledmarker_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
          'extmarkermanager' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'markermanager.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'markermanager_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
          'markerclusterer' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'markerclusterer.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'markerclusterer_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
          'markertracker' => array(
            'plain' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'markertracker.js'),
            'packed' => array(GMAPS_EXTERNAL_JS_RESOURCE_PATH .'markertracker_packed.js', 'module', 'header', FALSE, TRUE, FALSE),
          ),
        ),
      ),
    );
    return $overlays;
  }
  else if ($op == 'default_maps') {
    $types = gmaps_map_get_types('names');
    $defaults = array();
    
    $input = gmaps_extend(array(), $type);
    
    $input['name'] = 'Input '. $types[$input['type']];
    $input['size'] = gmaps_pixels_parse('0x300', 'wh');
    $input['throttle_support'] = 0;
    $input['map_data']['progressbar']['enabled'] = 0;
    $input['map_data']['toc_map']['enabled'] = 0;
    $input['map_data']['se_support'] = 0;
    
    $input['methods'] = drupal_map_assoc(array('auto_center_zoom', 'dragging', 'resize'));
    $input['controls']['maptype']['control'] = 'menumaptype';
    $input['controls']['navigation']['control'] = 'smallmap';
    
    foreach($input['controls'] as $ct => $ct_data) {
      if (!in_array($ct, array('maptype', 'navigation'))) {
        $input['controls'][$ct]['control'] = '';
      }
    }
    
    $input['control_data']['menumaptype']['shortnames'] = 1;
    
    $input['ov_data']['marker']['misc']['labels'] = '';
    
    $input['ov_data']['marker']['options']['clickable'] = 1;
    $input['ov_data']['marker']['options']['draggable'] = 1;
    $input['ov_data']['marker']['options']['hide'] = 0;
    
    $input['ov_data']['marker']['labeled']['enabled'] = 0;
    $input['ov_data']['marker']['protected_labeled']['enabled'] = 0;
    
    $input['ov_data']['marker']['action']['hover'] = '';
    $input['ov_data']['marker']['action']['click'] = 'content';
    $input['ov_data']['marker']['action']['dblclick'] = '';
    $input['ov_data']['marker']['action']['rightclick'] = '';
    
    $input['ov_data']['marker']['tracker']['enabled'] = 1;
    $input['ov_data']['marker']['route']['enabled'] = 0;
    $input['ov_data']['marker']['manager']['manager'] = '';

    $defaults['input_'. $input['type']] = $input;
    
    return $defaults;
  }
}

/**
 * Implementation of hook_gmaps_map_defaults_hook().
 */
function gmaps_gmaps_map_defaults_earth($info) {
  $map = array(
    'maptypes' => array('normal', 'satellite', 'hybrid'),
    'default_maptype' => 'normal',
    'map_data' => array(
      'svu' => array(
        'enabled' => 1,
        'height' => 300,
        'fullscreen' => 1,
        'resize' => 1,
        'marker' => 1,
        'track' => 1,
        'hide' => 1,
        'userphotos' => 0,
        'photorepos' => array(),
        'icon' => array(
          'type' => 1,
        ),
      ),
    ),
    'methods' => drupal_map_assoc(array('googlebar')),
    'controls' => array(
      'maptype' => array('control' => 'extmaptype'),
      'navigation' => array('control' => 'largemap3D'),
      'breadcrumb' => array('control' => 'breadcrumb'),
    ),
  );
  
  $compass = array('n', 'nne', 'ne', 'ene', 'e', 'ese', 'se', 'sse', 's', 'ssw', 'sw', 'wsw', 'w', 'wnw', 'nw', 'nnw');
  foreach($compass as $bearing) {
    $map['map_data']['svu']['icon'][$bearing] = gmaps_icon_get_icid_for_default('svuman_'. $bearing);
  }
  
  return $map;
}

/**
 * Implementation of hook_gmaps_map_defaults_hook().
 */
function gmaps_gmaps_map_defaults_moon($info) {
  $map = array(
    'maptypes' => array('moon_visible', 'moon_elevation'),
    'default_maptype' => 'moon_visible',
    'controls' => array(
      'maptype' => array('control' => 'maptype'),
      'navigation' => array('control' => 'largemap3D'),
    ),
  );
  return $map;
}

/**
 * Implementation of hook_gmaps_map_defaults_hook().
 */
function gmaps_gmaps_map_defaults_mars($info) {
  $map = array(
    'maptypes' => array('mars_visible', 'mars_elevation', 'mars_infrared'),
    'default_maptype' => 'mars_visible',
    'controls' => array(
      'maptype' => array('control' => 'maptype'),
      'navigation' => array('control' => 'largemap3D'),
    ),
  );
  return $map;
}

/**
 * Implementation of hook_gmaps_map_defaults_hook().
 */
function gmaps_gmaps_map_defaults_sky($info) {
  $map = array(
    'maptypes' => array('sky_visible'),
    'default_maptype' => 'sky_visible',
    'controls' => array(
      'maptype' => array('control' => 'maptype'),
      'navigation' => array('control' => 'largemap3D'),
    ),
  );
  return $map;
}

/**
 * Implementation of hook_form_alter().
 */
function gmaps_form_gmaps_map_form_alter(&$form, &$form_state) {
  $map = $form['#gmaps_map'];
  
  if ($map->default_id == 'input_'. $map->type) {
    //settings
    $f = &$form['accordion']['basic'];
    $f['throttle_support']['#default_value'] = 0;
    $f['throttle_support']['#access'] = FALSE;
    $f['throttle_lifetime']['#default_value'] = 0;
    $f['throttle_lifetime']['#access'] = FALSE;
    $f = &$form['accordion']['map_data'];
    $f['toc_map']['enabled']['#default_value'] = 0;
    $f['toc_map']['enabled']['#access'] = FALSE;
    $f['se_support']['#default_value'] = 0;
    $f['se_support']['#access'] = FALSE;
    $f = &$form['accordion']['map_data']['progressbar'];
    $f['enabled']['#default_value'] = 0;
    $f['enabled']['#access'] = FALSE;

    //methods
    $f = &$form['accordion']['method_panel'];
    $f['methods']['#default_value'] = drupal_map_assoc(array('auto_center_zoom', 'dragging', 'resize'));
    $f['methods']['#access'] = FALSE;
    
    //controls
    $f = &$form['accordion']['control_panel']['controls'];
    $f['maptype']['control']['#default_value'] = 'menumaptype';
    $f['maptype']['control']['#access'] = FALSE;
    $f['maptype']['control_data']['menumaptype']['shortnames']['#default_value'] = 1;
    $f['maptype']['control_data']['menumaptype']['shortnames']['#access'] = FALSE;
    $f['navigation']['control']['#default_value'] = 'smallmap';
    $f['navigation']['control']['#access'] = FALSE;
    
    foreach(element_children($f) as $ct ) {
      if (!in_array($ct, array('maptype', 'navigation'))) {
        $f[$ct]['control']['#default_value'] = '';
        $f[$ct]['control']['#access'] = FALSE;
      }
    }
    
    //overlays - marker
    $f = &$form['accordion']['ov_panel']['ov_data']['marker'];

    //labels
    $f['misc']['labels']['#default_value'] = '';
    $f['misc']['labels']['#access'] = FALSE;
    
    //labeled
    $f['options']['#after_build'][] = 'gmaps_map_form_gmaps_marker_options_after_build';
    
    //labeled
    $f['labeled']['#after_build'][] = 'gmaps_map_form_gmaps_marker_labeled_after_build';
    
    //protected labeled
    $f['protected_labeled']['#after_build'][] = 'gmaps_map_form_gmaps_marker_labeled_after_build';
    
    $f['action']['#access'] = FALSE;
    $f['action']['#after_build'][] = 'gmaps_map_form_gmaps_marker_action_after_build';
    
    //routes
    $f['route']['enabled']['#default_value'] = 0;
    $f['route']['enabled']['#access'] = FALSE;
    
    //manager
    $f['manager']['manager']['#default_value'] = '';
    $f['manager']['manager']['#access'] = FALSE;
    
    //$form['#after_build'][] = 'gmaps_form_gmaps_map_form_after_build';
  }
}

function gmaps_map_form_gmaps_marker_options_after_build($form, &$form_state) {
  form_set_value($form['clickable'], 1, $form_state);
  $form['clickable']['#access'] = FALSE;
  form_set_value($form['draggable'], 1, $form_state);
  $form['draggable']['#access'] = FALSE;
  form_set_value($form['hide'], 0, $form_state);
  $form['hide']['#access'] = FALSE;
  return $form;
}

function gmaps_map_form_gmaps_marker_labeled_after_build($form, &$form_state) {
  form_set_value($form['enabled'], 0, $form_state);
  $form['enabled']['#access'] = FALSE;
  return $form;
}

function gmaps_map_form_gmaps_marker_action_after_build($form, &$form_state) {
  form_set_value($form['hover'], '', $form_state);
  $form['hover']['#access'] = FALSE;
  form_set_value($form['click'], 'content', $form_state);
  $form['click']['#access'] = FALSE;
  form_set_value($form['dblclick'], '', $form_state);
  $form['dblclick']['#access'] = FALSE;
  form_set_value($form['rightclick'], '', $form_state);
  $form['rightclick']['#access'] = FALSE;
  return $form;
}




/********************
 * Public functions *
 ********************/

function gmaps_map_invoke($hook, $type, $op, $map_type, $a1, &$a2 = NULL) {
  static $included = array();
  static $hook_map = NULL;
  
  if (is_null($hook_map)) {
    $info = gmaps_map_get_info($map_type);
    foreach(_gmaps_map_get_hook_map() as $h => $hook_info) {
      $hook_map[$h] = $info->$hook_info['info'];
    }
  }
  $hook_info = $hook_map[$hook][$type];

  if ($hook_info) {
    if (isset($hook_info->file) && !isset($included[$hook_info->file])) {
      module_load_include('inc', $hook_info->module, $hook_info->file);
      $included[$hook_info->file] = TRUE;
    }
  
    $function = $hook_info->module .'_gmaps_map_'. $hook .'_'. $type;
    if (function_exists($function)) {
      return $function($op, $a1, $a2);
    }
  }

  return NULL;
}

function gmaps_map_invoke_all($op, $map_type, $a1, &$a2 = NULL) {
  $info = gmaps_map_get_info($map_type);
  foreach(_gmaps_map_get_hook_map() as $hook => $hook_info) {
    if (isset($info->$hook_info['info'])) {
      foreach($info->$hook_info['info'] as $type => $type_data) {
        gmaps_map_invoke($hook, $type, $op, $map_type, $a1, $a2);
      }
    }
  }
}

/**
 * Map administration page.
 * 
 * @return html
 */
function gmaps_map_admin_overview() {
  _gmaps_map_install_default_maps();

  $output = drupal_get_form('gmaps_map_admin_filter');
    
  $types = gmaps_map_get_types();

  $header = array(
    array('data' => t('Name'), 'field' => 'm.name', 'sort' => 'asc'),
    array('data' => t('Type'), 'field' => 'm.type'),
    array('data' => t('Operations'), 'colspan' => '1')
  );

  $rows = array();
  $path = 'admin/gmaps/map/';

  $sql = 'SELECT m.mid, m.* FROM {gmaps_maps} m';
  $filter = $_SESSION['gmaps_map_admin_filter']['sql'];
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
    $type_name = &$types[$rec->type]->name;
    $name = t($rec->name);
    $rows[] = array(
      l(drupal_strlen($name) > 15 ? drupal_substr($name, 0, 10).'...' : $name, $path . $rec->mid .'/edit', array('attributes' => array('title' => $name))),
      l(drupal_strlen($type_name) > 15 ? drupal_substr($type_name, 0, 10).'...' : $type_name, 'admin/gmaps/map/add/' . $types[$rec->type]->type_url),
      implode(', ', $ops),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No maps available.'), 'colspan' => '3', 'class' => 'message'));
  }

  $output .= theme('table', $header, $rows) . theme('pager', NULL, 50, 0);
  return $output;
}

/**
 * Filter form for map admin overview.
 */
function gmaps_map_admin_filter() {
  $form = array();

  if (empty($_SESSION['gmaps_map_admin_filter'])) {
    $_SESSION['gmaps_map_admin_filter'] = array();
    $_SESSION['gmaps_map_admin_filter']['type'] = 'all';
  }

  $types = array('all' => t('All types')) + gmaps_map_get_types('names');

  $form['filter'] = array('#type' => 'fieldset',
    '#title' => t('Filters'),
    '#collapsible' => TRUE,
    '#collapsed' => !empty($_SESSION['gmaps_map_admin_filter']['sql']),
  );
  $form['filter']['type'] = array(
    '#type' => 'select',
    '#title' => t('Filter by type'),
    '#options' => $types,
    '#default_value' => $_SESSION['gmaps_map_admin_filter']['type'],
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );

  $form['filter']['submit'] = array('#type' => 'submit', '#value' => t('Filter'));
  $form['#redirect'] = FALSE;

  return $form;
}

function gmaps_map_admin_filter_submit($form, &$form_state) {
  $values = &$form_state['values'];
  $_SESSION['gmaps_map_admin_filter']['type'] = $values['type'];

  $filters = $args = array();
  if ($values['type'] != 'all') {
    $filters[] = "m.type = '%s'";
    $args[] = $values['type'];
  }

  if (empty($filters)) {
    $_SESSION['gmaps_map_admin_filter']['sql'] = array();
  }
  else {
    $_SESSION['gmaps_map_admin_filter']['sql'] = array();
    $_SESSION['gmaps_map_admin_filter']['sql']['query'] = ' WHERE '. implode(' AND ', array_values($filters));
    $_SESSION['gmaps_map_admin_filter']['sql']['args'] = $args;
  }
}

/**
 * Map add menu callback.
 */
function gmaps_map_add($type, $type_info) {
  $info = gmaps_map_get_info($type);
  
  $map = _gmaps_map_get_defaults($info, $type_info);
  
  $map = array_merge($map, array(
    'mid' => 0,
    'default_id' => '',
    'type' => $type,
    'name' => '',
    'modified' => 0,
    'locked' => 0,
  ));

  drupal_alter('gmaps_map_defaults', $map);
  
  $map = (object)$map;
  
  return drupal_get_form('gmaps_map_form', &$map, $info);
}

/**
 * Map edit menu callback.
 */
function gmaps_map_edit($map) {
  drupal_set_title(t('Edit map preset %map', array('%map' => t($map->name))));

  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Map presets'), 'admin/gmaps/maps');
  drupal_set_breadcrumb($bc);

  $info = gmaps_map_get_info($map->type);

  return drupal_get_form('gmaps_map_form', &$map, &$info);
}

/**
 * Map clone menu callback.
 */
function gmaps_map_clone($map) {
  drupal_set_title(t('Clone map preset %map', array('%map' => t($map->name))));

  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Map presets'), 'admin/gmaps/maps');
  drupal_set_breadcrumb($bc);

  $info = gmaps_map_get_info($map->type);
  
  $map->mid = 0;
  $map->default_id = '';
  $map->name = '';
  $map->modified = 0;
  $map->locked = 0;


  return drupal_get_form('gmaps_map_form', &$map, &$info, TRUE);
}

/**
 * Map reset confirm form.
 */
function gmaps_map_reset_confirm($form_state = NULL, $map) {
  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Map presets'), 'admin/gmaps/maps');
  drupal_set_breadcrumb($bc);

  $form['#gmaps_map'] = $map;

  $form = confirm_form($form,
    t('Are you sure you want to reset the map preset %title?', array('%title' => t($map->name))),
    'admin/gmaps/maps',
    t('This action cannot be undone.'),
    t('Reset'),
    t('Cancel')
  );

  $form['#submit'][] = '_gmaps_map_reset_submit';

  return $form;
}

/**
 * Map delete menu callback.
 */
function gmaps_map_delete($map) {
  //Add admin overview to the breadcrumb
  $bc = drupal_get_breadcrumb();
  $bc[] = l(t('Map presets'), 'admin/gmaps/maps');
  drupal_set_breadcrumb($bc);

  $refs = gmaps_references_invoke('map', $map);
  if (!empty($refs)) {
    $output = '<p>'. t('Unable to delete this map preset because of the following references. Remove them if you really want to delete the preset.') .'</p>';
    $output .= theme('gmaps_references', $refs);
    $output .= '<p>'. l(t('Cancel'), 'admin/gmaps/maps') .'</p>';
    return $output;
  }

  return drupal_get_form('_gmaps_map_delete_form', $map);
}

/**
 * Map form used on add/edit/clone.
 *
 * @param array $form_state
 * @param object $map
 * @param object $info
 *  Map type info
 * @param boolean $clone
 *
 * @return array
 */
function gmaps_map_form(&$form_state, &$map, &$info, $clone = FALSE) {
  $form = _gmaps_map_form($form_state, $map, $info, $clone);
  if ($clone) {
    $form['#validate'][] = '_gmaps_map_form_validate_main';
    $form['#submit'][] = '_gmaps_map_form_submit_clone';
  }
  else {
    $form['#validate'][] = '_gmaps_map_form_validate_all';
    $form['#submit'][] = '_gmaps_map_form_submit';
  }
  
  //form_alter helper for default icon providers
  if ($map->locked) {
    $data = &$form;
    $data['__drupal_alter_by_ref'] = array(&$form_state);
    drupal_alter('gmaps_default_map_form_'. $map->default_id, $data);
  }
    
  return $form;
}





/**********************
 * Internal functions *
 **********************/

/**
 * Collect and prepare map type info from hook implementations.
 *
 * @return array of arrays
 */
function _gmaps_map_get_types() {
  $types = array();
  foreach (module_implements('gmaps_map_info') as $module) {
    $function = $module .'_gmaps_map_info';
    $result = $function('types');
    if (isset($result)) {
      foreach($result as $key => $info) {
        //'info' conflicts with hook_gmaps_map_info(), 'item' conflict with hook_gmaps_map_item_alter().
        if ($key != 'info' && $key != 'item') {
          $info['type'] = $key;
          $info['module'] = $module;
          $info['type_url'] = strtr($key, '_', '-');
          if (!isset($info['name'])) {
            //type is ascii only
            $info['name'] = ucfirst($info['type']);
          }
          //enabled by default
          $info['3D_enabled'] = (!isset($info['3D_enabled']) || $info['3D_enabled']);
          if (!isset($info['weight'])) {
            $info['weight'] = 0;
          }
          $types[$key] = $info;
        }
      }
    }
  }
  if (!empty($types)) {
    drupal_alter('gmaps_map_types', $types);
    cache_set('gmaps_map_types', $types, 'cache');
  }

  return $types;
}

/**
 * Collect and prepare map type info from hook implementations.
 *
 * @param string $type
 *  ID of the map type.
 * 
 * @return array of arrays
 */
function _gmaps_map_get_info($type) {
  $info = array();
  foreach (module_implements('gmaps_map_info') as $module) {
    $function = $module .'_gmaps_map_info';
    
    //foreach(array('maptypes', 'methods', 'control_types', 'controls', 'overlays') as $key) {
    $context = 'maptypes';
    $result = $function($context, $type);
    if (!empty($result)) {
      if (!isset($info[$context])) {
        $info[$context] = array();
      }
      foreach($result as $key => $data) {
        $data['type'] = $key;
        $data['module'] = $module;
        if (!isset($data['min_zoom'])) {
          $data['min_zoom'] = 0;
        }
        $info[$context][$key] = $data;
      }
    }
    if (empty($info[$context])) {
      return FALSE;
    }

    $context = 'methods';
    $result = $function($context, $type);
    if (!empty($result)) {
      if (!isset($info[$context])) {
        $info[$context] = array();
      }
      foreach($result as $key => $data) {
        $data['type'] = $key;
        $data['module'] = $module;

        $info[$context][$key] = $data;
      }
    }

    $context = 'info_windows';
    $result = $function($context, $type);
    if (!empty($result)) {
      if (!isset($info[$context])) {
        $info[$context] = array();
      }
      foreach($result as $key => $data) {
        $data['type'] = $key;
        $data['module'] = $module;
        
        $info[$context][$key] = $data;
      }
    }

    $context = 'control_types';
    $result = $function($context, $type);
    if (!empty($result)) {
      if (!isset($info[$context])) {
        $info[$context] = array();
      }
      foreach($result as $key => $data) {
        $data['controls'] = array();
        
        $info[$context][$key] = $data;
      }
    }

    $context = 'controls';
    $result = $function($context, $type);
    if (!empty($result)) {
      if (!isset($info[$context])) {
        $info[$context] = array();
      }
      foreach($result as $key => $data) {
        $data['type'] = $key;
        $data['module'] = $module;
        
        if (!isset($data['api_version'])) {
          $data['api_version'] = TRUE;
        }

        $info[$context][$key] = $data;
      }
    }

    $context = 'overlays';
    $result = $function($context, $type);
    if (!empty($result)) {
      if (!isset($info[$context])) {
        $info[$context] = array();
      }
      foreach($result as $key => $data) {
        $data['type'] = $key;
        $data['module'] = $module;

        $info[$context][$key] = $data;
      }
    }
  }
  if (!empty($info)) {
    $info['type'] = $type;
    if (isset($info['controls'])) {
      foreach($info['controls'] as $control => $data) {
        if (isset($info['control_types'][$data['control_type']])) {
          $info['control_types'][$data['control_type']]['controls'][] = $control;
        }
        else {
          unset($info['controls'][$control]);
        }
      }
      if (empty($info['controls'])) {
        unset($info['controls'], $info['control_types']);
      }
    }
    if (isset($info['control_types'])) {
      foreach($info['control_types'] as $ct => $ct_data) {
        if (empty($ct_data['controls'])) {
          unset($info['control_types'][$ct]);
        }
      }
      if (empty($info['control_types'])) {
        unset($info['control_types'], $info['controls']);
      }
    }
    drupal_alter('gmaps_map_info', $info);
    cache_set('gmaps_map_info:'. $type, $info, 'cache');
  }

  return $info;
}

function _gmaps_map_get_defaults($info, $type_info) {
  //core defaults
  $map = array(
    'type' => $info->type,
    'throttle_support' => 1,
    'throttle_lifetime' => 3600,
    'size' => gmaps_pixels_parse('0x200', 'wh'),
    'center' => (object)array(
      'latitude' => 0,
      'longitude' => 0,
      'elevation' => 0,
      'map_type' => $info->type,
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
    'zoom' => -1,
    'map_data' => array(
      'throbber' => gmaps_icon_get_icid_for_default('throbber'),
      'se_support' => 1,
      'progressbar' => array(
        'enabled' => 1,
        'width' => 0,
        'loadstring' => 'Loading...',
      ),
      'toc_map' => array(
        'enabled' => 0,
        'style' => 'accordion',
        'collapsible' => 1,
        'collapsed' => 0,
        'tabs_nav' => GMAPS_TABS_NAV_TOP,
        'grid' => array(
          'columns' => 2,
          'alignment' => GMAPS_GRID_ALIGN_VER,
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
    ),
    'methods' => array('dragging', 'infowindow', 'dblclickzoom', 'auto_center_zoom', 'resize'),
    'method_data' => array(),
    'iw_data' => array(),
    'controls' => array(),
    'control_data' => array(),
    'ov_data' => array(),
  );
  
  if ($info->type == GMAPS_MAP_EARTH) {
    $map['center']->pov = array(
      'yaw' => 0,
      'pitch' => 0,
      'zoom' => 0,
    );
  }
  
  //type defaults
  $function = $type_info->module .'_gmaps_map_defaults_'. $info->type;
  if (function_exists($function) && ($type_map = $function($info))) {
    $map = gmaps_extend($map, $type_map);
  }
  
  foreach(_gmaps_map_get_hook_map() as $hook => $hook_info) {
    if (isset($info->$hook_info['info'])) {
      foreach($info->$hook_info['info'] as $data) {
        $result = gmaps_map_invoke($hook, $data->type, 'defaults', $info->type, $info);
        if ($result) {
          $map[$hook_info['storage']][$data->type] = $result;
        }
      }
    }
  }
  
  return $map;
}

/**
 * Generates the map editing form.
 */
function _gmaps_map_form(&$form_state, &$map, &$info, $clone = FALSE) {
  $form = array();

  $form['#gmaps_map'] = $map;
  $form['#gmaps_map_info'] = $info;
  //should be checked in hook_form_alter implementations
  $form['#gmaps_clone'] = $clone;
  
  $icons = gmaps_get_icons();

  $form['accordion'] = $clone ? array() : array('#type' => 'gmaps_accordion',
    '#id' => 'gmaps-map-form-accordion',
    '#collapsible' => TRUE,
    '#collapsed' => $map->mid,
  );
  
  $types = gmaps_map_get_types('names');
  $form['accordion']['basic'] = $clone ? array() : array('#type' => 'gmaps_accordion_panel',
    '#title' => t('%type settings', array('%type' => $types[$map->type])),
    '#id' => 'basic',
  );
  $form['accordion']['basic']['name'] = array('#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $map->name,
    '#description' => t('<em>TRANSLATABLE.</em> The human-readable name of this map. This text will be displayed in select list. This name must be unique.'),
    '#required' => TRUE,
  );
  $form['accordion']['basic']['throttle_support'] = array('#type' => 'radios',
    '#title' => t('Throttle support'),
    '#default_value' => $map->throttle_support,
    '#options' => array(t('Disabled'), t('Enabled')),
  );
  $form['accordion']['basic']['throttle_lifetime'] = array('#type' => 'textfield',
    '#title' => t('Throttle cache lifetime'),
    '#default_value' => $map->throttle_lifetime,
    '#required' => TRUE,
    '#description' => t('Minimum cache lifetime when throttle is active. Use 0 (zero) to disable cache.'),
    '#size' => 16,
    '#field_suffix' => t('seconds'),
  );
  $form['accordion']['basic']['size'] = array('#type' => 'gmaps_pixels',
    '#title' => t('Map size'),
    '#default_value' => $map->size,
    '#required' => TRUE,
    '#return_value_keys' => 'wh',
    '#description' => t('Size of the map container. Use 0 (zero) for auto sizing.'),
  );
  $form['accordion']['basic']['maptypes'] = array('#type' => 'select',
    '#title' => t('Map types'),
    '#default_value' => $map->maptypes,
    '#required' => TRUE,
    '#options' => $info->maptypes_names,
    '#multiple' => TRUE,
    '#size' => 3,
  );
  $form['accordion']['basic']['default_maptype'] = array('#type' => 'select',
    '#title' => t('Default map type'),
    '#default_value' => $map->default_maptype,
    '#options' => $info->maptypes_names,
  );
  $form['accordion']['basic']['center'] = array('#type' => 'gmaps_point',
    '#title' => t('Default center'),
    '#default_value' => $map->center,
    '#required' => TRUE,
    '#map_type' => $map->type,
    '#use_map' => $map,
    '#enable_pov' => FALSE,
    '#enable_input' => TRUE,
  );
  $form['accordion']['basic']['zoom'] = array('#type' => 'select',
    '#title' => t('Default zoom'),
    '#default_value' => $map->zoom,
    '#options' => array(-1 => t('Auto')) + _gmaps_map_get_zoom_levels($info->maptypes),
    '#description' => t('Auto depends on the bounds of the center.'),
  );
  
  if ($clone) {
    $form['button']['submit'] = array('#type' => 'submit',
      '#value' => t('Clone'),
    );
    return $form;
  }

  //optional settings
  $form['accordion']['map_data'] = array('#type' => 'gmaps_accordion_panel',
    '#title' => t('Optional settings'),
    '#id' => 'settings',
    '#tree' => TRUE,
  );
  $form['accordion']['map_data']['throbber'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Throbber icon'),
    '#default_value' => $map->map_data['throbber'],
    '#icons' => $icons,
  );
  $form['accordion']['map_data']['draggable_cursor'] = array('#type' => 'textfield',
    '#title' => t('Draggable cursor'),
    '#default_value' => $map->map_data['draggable_cursor'],
  );
  $form['accordion']['map_data']['dragging_cursor'] = array('#type' => 'textfield',
    '#title' => t('Dragging cursor'),
    '#default_value' => $map->map_data['dragging_cursor'],
  );
  $form['accordion']['map_data']['bgcolor'] = array('#type' => 'textfield',
    '#title' => t('Background color'),
    '#default_value' => $map->map_data['bgcolor'],
    '#size' => 10,
  );
  $form['accordion']['map_data']['se_support'] = array('#type' => 'checkbox',
    '#title' => t('Enable Search Engine support'),
    '#default_value' => $map->map_data['se_support'],
    '#description' => t('When enabled and the current user-agent is a search engine, then the TOC map behavior is turned on for the request.'),
  );
  $form['accordion']['map_data']['all_files'] = array('#type' => 'checkbox',
    '#title' => t('Add all client files'),
    '#default_value' => $map->map_data['all_files'],
    '#description' => t('Enable this feature when all available functionality required on the client side.'),
  );
  $form['accordion']['map_data']['progressbar'] = array('#type' => 'fieldset',
    '#title' => t('Progress bar'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map_data']['progressbar']['enabled'] = array('#type' => 'checkbox',
    '#title' => t('Enable progress bar'),
    '#default_value' => $map->map_data['progressbar']['enabled'],
  );
  $form['accordion']['map_data']['progressbar']['width'] = array('#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $map->map_data['progressbar']['width'],
    '#required' => TRUE,
    '#size' => 6,
    '#maxlength' => 4,
    '#field_suffix' => 'px',
    '#description' => t('Use 0 (zero) for the default value, which is 176px.'),
  );
  $form['accordion']['map_data']['progressbar']['loadstring'] = array('#type' => 'textfield',
    '#title' => t('Load string'),
    '#default_value' => $map->map_data['progressbar']['loadstring'],
    '#required' => TRUE,
    '#description' => t('<strong>TRANSLATABLE</strong>. Initial message displayed before any update.'),
  );
  $form['accordion']['map_data']['toc_map'] = array('#type' => 'fieldset',
    '#title' => t('Table of contents map'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['enabled'] = array('#type' => 'checkbox',
    '#title' => t('Enable TOC map behavior'),
    '#default_value' => $map->map_data['toc_map']['enabled'],
    '#description' => t('When enabled, the content of the overlays will be shown below the map, and the map itself will act as a TOC navigator.'),
  );
  $form['accordion']['map_data']['toc_map']['icon'] = array('#type' => 'select',
    '#title' => t('Icon display'),
    '#default_value' => $map->map_data['toc_map']['icon'],
    '#options' => array('' => t('Disabled'), 'before' => t('Before title'), 'after' => t('After title'), 'replace' => t('Replace title')),
  );
  $form['accordion']['map_data']['toc_map']['style'] = array('#type' => 'select',
    '#title' => t('List style'),
    '#default_value' => $map->map_data['toc_map']['style'],
    '#options' => _gmaps_map_tab_styles(),
  );
  $form['accordion']['map_data']['toc_map']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $map->map_data['toc_map']['collapsible'],
  );
  $form['accordion']['map_data']['toc_map']['collapsed'] = array('#type' => 'checkbox',
    '#title' => t('Collapsed'),
    '#default_value' => $map->map_data['toc_map']['collapsed'],
  );
  $form['accordion']['map_data']['toc_map']['tabs_nav'] = array('#type' => 'select',
    '#title' => t('Tabs navigation position'),
    '#default_value' => $map->map_data['toc_map']['tabs_nav'],
    '#options' => array(
      GMAPS_TABS_NAV_TOP => t('Top'),
      GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
      GMAPS_TABS_NAV_LEFT => t('Left'),
      GMAPS_TABS_NAV_RIGHT => t('Right'),
    ),
  );
  $form['accordion']['map_data']['toc_map']['grid'] = array('#type' => 'fieldset',
    '#title' => t('Grid settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['grid']['columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of columns'),
    '#default_value' => $map->map_data['toc_map']['grid']['columns'],
    '#required' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['grid']['alignment'] = array(
    '#type' => 'radios',
    '#title' => t('Alignment'),
    '#options' => array(GMAPS_GRID_ALIGN_HOR => t('Horizontal'), GMAPS_GRID_ALIGN_VER => t('Vertical')),
    '#default_value' => $map->map_data['toc_map']['grid']['alignment'],
    '#description' => t('Horizontal alignment will place items starting in the upper left and moving right. Vertical alignment will place items starting in the upper left and moving down.'),
  );
  $form['accordion']['map_data']['toc_map']['ahah'] = array('#type' => 'fieldset',
    '#title' => t('AHAH settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['ahah']['progress'] = array('#type' => 'select',
    '#title' => t('Progress style'),
    '#default_value' => $map->map_data['toc_map']['ahah']['progress'],
    '#options' => array('' => t('Auto'), 'bar' => t('Progress bar'), 'throbber' => t('Throbber')),
  );
  $form['accordion']['map_data']['toc_map']['ahah']['message'] = array('#type' => 'textfield',
    '#title' => t('Progress message'),
    '#default_value' => $map->map_data['toc_map']['ahah']['message'],
    '#description' => t('<strong>TRANSLATABLE</strong>. Leave empty for none.'),
  );
  $form['accordion']['map_data']['toc_map']['ahah']['effect'] = array('#type' => 'textfield',
    '#title' => t('Effect'),
    '#default_value' => $map->map_data['toc_map']['ahah']['effect'],
    '#description' => t('Leave empty for none.'),
  );
  $form['accordion']['map_data']['toc_map']['content'] = array('#type' => 'fieldset',
    '#title' => t('Content tabs settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['content']['style'] = array('#type' => 'select',
    '#title' => t('Render tabs as'),
    '#default_value' => $map->map_data['toc_map']['content']['style'],
    '#options' => _gmaps_map_tab_styles(),
  );
  $form['accordion']['map_data']['toc_map']['content']['collapsible'] = array('#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $map->map_data['toc_map']['content']['collapsible'],
  );
  $form['accordion']['map_data']['toc_map']['content']['collapsed'] = array('#type' => 'checkbox',
    '#title' => t('Collapsed'),
    '#default_value' => $map->map_data['toc_map']['content']['collapsed'],
  );
  $form['accordion']['map_data']['toc_map']['content']['tabs_nav'] = array('#type' => 'select',
    '#title' => t('Tabs navigation position'),
    '#default_value' => $map->map_data['toc_map']['content']['tabs_nav'],
    '#options' => array(
      GMAPS_TABS_NAV_TOP => t('Top'),
      GMAPS_TABS_NAV_BOTTOM => t('Bottom'),
      GMAPS_TABS_NAV_LEFT => t('Left'),
      GMAPS_TABS_NAV_RIGHT => t('Right'),
    ),
  );
  $form['accordion']['map_data']['toc_map']['content']['grid'] = array('#type' => 'fieldset',
    '#title' => t('Grid settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['content']['grid']['columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of columns'),
    '#default_value' => $map->map_data['toc_map']['content']['grid']['columns'],
    '#required' => TRUE,
  );
  $form['accordion']['map_data']['toc_map']['content']['grid']['alignment'] = array(
    '#type' => 'radios',
    '#title' => t('Alignment'),
    '#options' => array(GMAPS_GRID_ALIGN_HOR => t('Horizontal'), GMAPS_GRID_ALIGN_VER => t('Vertical')),
    '#default_value' => $map->map_data['toc_map']['content']['grid']['alignment'],
    '#description' => t('Horizontal alignment will place items starting in the upper left and moving right. Vertical alignment will place items starting in the upper left and moving down.'),
  );
  
  //street view, earth only
  if ($info->type == GMAPS_MAP_EARTH) {
    $form['accordion']['map_data']['svu'] = array('#type' => 'fieldset',
      '#title' => t('Street view'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['accordion']['map_data']['svu']['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enable street view'),
      '#default_value' => $map->map_data['svu']['enabled'],
    );
    $form['accordion']['map_data']['svu']['height'] = array('#type' => 'textfield',
      '#title' => t('Height'),
      '#default_value' => $map->map_data['svu']['height'],
      '#required' => TRUE,
      '#size' => 6,
      '#maxlength' => 3,
      '#field_suffix' => 'px',
    );
    $form['accordion']['map_data']['svu']['fullscreen'] = array('#type' => 'checkbox',
      '#title' => t('Enable full-screen control'),
      '#default_value' => $map->map_data['svu']['fullscreen'],
    );
    $form['accordion']['map_data']['svu']['resize'] = array('#type' => 'checkbox',
      '#title' => t('Enable height resize'),
      '#default_value' => $map->map_data['svu']['resize'],
    );
    $form['accordion']['map_data']['svu']['marker'] = array('#type' => 'checkbox',
      '#title' => t('Enable street view marker'),
      '#default_value' => $map->map_data['svu']['marker'],
    );
    $form['accordion']['map_data']['svu']['track'] = array('#type' => 'checkbox',
      '#title' => t('Track street view marker'),
      '#default_value' => $map->map_data['svu']['track'],
    );
    $form['accordion']['map_data']['svu']['blowup'] = array('#type' => 'checkbox',
      '#title' => t('Show map blowup on click'),
      '#default_value' => $map->map_data['svu']['blowup'],
    );
    $form['accordion']['map_data']['svu']['hide'] = array('#type' => 'checkbox',
      '#title' => t('Hide street view on right click'),
      '#default_value' => $map->map_data['svu']['hide'],
    );
    $form['accordion']['map_data']['svu']['userphotos'] = array('#type' => 'checkbox',
      '#title' => t('Enable user photos viewing mode'),
      '#default_value' => $map->map_data['svu']['userphotos'],
    );
    $form['accordion']['map_data']['svu']['photorepos'] = array('#type' => 'select',
      '#title' => t('Photo repositories'),
      '#default_value' => $map->map_data['svu']['photorepos'],
      '#options' => array(
        'panoramio' => t('Panoramio'),
        'picasa' => t('Picasa'),
        'flickr' => t('Flickr'),
      ),
      '#multiple' => TRUE,
      '#description' => t('Enabled repositories for the user photos viewing mode. All will be enabled if none selected.'),
    );
    
    $form['accordion']['map_data']['svu']['icon'] = array('#type' => 'fieldset',
      '#title' => t('Icons'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['accordion']['map_data']['svu']['icon']['type'] = array('#type' => 'select',
      '#title' => t('Type'),
      '#default_value' => $map->map_data['svu']['icon']['type'],
      '#options' => array(t('Static'), t('Dynamic')),
      '#description' => t('When set to static, than the north icon will be used in all bearing.'),
    );

    $compass = array(
      'n' => t('North (0&deg;)'),
      'nne' => t('North-Northeast'),
      'ne' => t('Northeast'),
      'ene' => t('East-Northeast'),
      'e' => t('East (90&deg;)'),
      'ese' => t('East-Southeast'),
      'se' => t('Southeast'),
      'sse' => t('South-Southeast'),
      's' => t('South (180&deg;)'),
      'ssw' => t('South-Southwest'),
      'sw' => t('Southwest'),
      'wsw' => t('West-Southwest'),
      'w' => t('West (270&deg;)'),
      'wnw' => t('West-Northwest'),
      'nw' => t('Northwest'),
      'nnw' => t('North-Northwest'),
    );
    foreach($compass as $bearing => $label) {
      $form['accordion']['map_data']['svu']['icon'][$bearing] = array('#type' => 'gmaps_icon_select',
        '#title' => $label,
        '#default_value' => $map->map_data['svu']['icon'][$bearing],
        '#icons' => (($map->map_data['svu']['icon']['type'] || $bearing == 'n') ? array() : array(0 => t('Disabled'))) + $icons,
      );
    }
  }
  
  //methods
  if (!empty($info->methods)) {
    $form['accordion']['method_panel'] = array('#type' => 'gmaps_accordion_panel',
      '#title' => t('Methods'),
      '#id' => 'methods',
    );
    $form['accordion']['method_panel']['methods'] = array('#type' => 'select',
      '#title' => t('Allowed methods'),
      '#default_value' => $map->methods,
      '#options' => $info->methods_names,
      '#multiple' => TRUE,
      '#size' => 5,
    );
    foreach($info->methods_names as $method => $name) {
      $method = $info->methods[$method];
      $method_form = gmaps_map_invoke('method', $method->type, 'form', $info->type, $form, $form_state);
      if ($method_form) {
        if (!isset($form['accordion']['method_panel']['method_data'])) {
          $form['accordion']['method_panel']['method_data'] = array('#type' => 'gmaps_accordion',
            '#id' => 'gmaps-map-form-methods',
            '#tree' => TRUE,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
          );
        }
        $form['accordion']['method_panel']['method_data'][$method->type] = array('#type' => 'gmaps_accordion_panel',
          '#title' => t('!title settings', array('!title' => $method->name)),
          '#id' => 'method-'. $method->type,
          '#tree' => TRUE,
        );
        $form['accordion']['method_panel']['method_data'][$method->type] = array_merge($form['accordion']['method_panel']['method_data'][$method->type], $method_form);
      }
    }
  }
  
  //info windows
  if (!empty($info->info_windows_names)) {
    foreach($info->info_windows_names as $iw => $name) {
      $iw = $info->info_windows[$iw];
      $iw_form = gmaps_map_invoke('iw', $iw->type, 'form', $info->type, $form, $form_state);
      if ($iw_form) {
        if (!isset($form['accordion']['iw_panel'])) {
          $form['accordion']['iw_panel'] = array('#type' => 'gmaps_accordion_panel',
            '#title' => t('Info windows'),
            '#id' => 'info-windows',
          );
          $form['accordion']['iw_panel']['iw_data'] = array('#type' => 'gmaps_accordion',
            '#id' => 'gmaps-map-form-iws',
            '#tree' => TRUE,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
          );
        }
        $form['accordion']['iw_panel']['iw_data'][$iw->type] = array('#type' => 'gmaps_accordion_panel',
          '#title' => t('!title settings', array('!title' => $iw->name)),
          '#id' => 'iw-'. $iw->type,
          '#tree' => TRUE,
        );
        $form['accordion']['iw_panel']['iw_data'][$iw->type] = array_merge($form['accordion']['iw_panel']['iw_data'][$iw->type], $iw_form);
      }
    }
  }
  
  //controls
  if (!empty($info->control_types)) {
    $form['accordion']['control_panel'] = array('#type' => 'gmaps_accordion_panel',
      '#title' => t('Controls'),
      '#id' => 'controls',
    );
    $form['accordion']['control_panel']['controls'] = array('#type' => 'gmaps_accordion',
      '#id' => 'gmaps-map-form-controls',
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    foreach($info->control_types as $ct => $ct_data) {
      $form['accordion']['control_panel']['controls'][$ct] = array('#type' => 'gmaps_accordion_panel',
        '#title' => t('!title control', array('!title' => $ct_data->name)),
        '#id' => 'control-type-'. $ct,
      );
      $form['accordion']['control_panel']['controls'][$ct]['control'] = array('#type' => 'select',
        '#title' => t('Control'),
        '#default_value' => $map->controls[$ct]['control'],
        '#options' => array('' => t('Disabled')) + $ct_data->controls_names,
      );
      $form['accordion']['control_panel']['controls'][$ct]['position'] = array('#type' => 'gmaps_control_position',
        '#title' => t('Position'),
        '#default_value' => $map->controls[$ct]['position'],
        '#collapsible' => TRUE,
        '#collapsed' => (empty($map->controls[$ct]['position']) || $map->controls[$ct]['position']['anchor'] == GMAPS_CONTROL_POSITION_DEFAULT),
      );
      foreach($ct_data->controls as $control) {
        $control = $info->controls[$control];
        $control_form = gmaps_map_invoke('control', $control->type, 'form', $info->type, $form, $form_state);
        if ($control_form) {
          if (!isset($form['accordion']['control_panel']['controls'][$ct]['control_data'])) {
            $form['accordion']['control_panel']['controls'][$ct]['control_data'] = array('#type' => 'gmaps_accordion',
              '#id' => 'gmaps-map-form-control-data-'. $ct,
              '#tree' => TRUE,
              '#collapsible' => TRUE,
              '#collapsed' => TRUE,
            );
          }
          $form['accordion']['control_panel']['controls'][$ct]['control_data'][$control->type] = array('#type' => 'gmaps_accordion_panel',
            '#title' => t('!title settings', array('!title' => $control->name)),
            '#id' => 'control-'. $control->type,
            '#tree' => TRUE,
          );
          $form['accordion']['control_panel']['controls'][$ct]['control_data'][$control->type] = array_merge($form['accordion']['control_panel']['controls'][$ct]['control_data'][$control->type], $control_form);
        }
      }
    }
  }
  
  //overlays
  if (!empty($info->overlays_names)) {
    foreach($info->overlays_names as $ov => $name) {
      $ov = $info->overlays[$ov];
      $ov_form = gmaps_map_invoke('ov', $ov->type, 'form', $info->type, $form, $form_state);
      if ($ov_form) {
        if (!isset($form['accordion']['ov_panel'])) {
          $form['accordion']['ov_panel'] = array('#type' => 'gmaps_accordion_panel',
            '#title' => t('Overlays'),
            '#id' => 'overlays',
          );
          $form['accordion']['ov_panel']['ov_data'] = array('#type' => 'gmaps_accordion',
            '#id' => 'gmaps-map-form-overlays',
            '#tree' => TRUE,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
          );
        }
        $form['accordion']['ov_panel']['ov_data'][$ov->type] = array('#type' => 'gmaps_accordion_panel',
          '#title' => t('!title settings', array('!title' => $ov->name)),
          '#id' => 'overlay-'. $ov->type,
          '#tree' => TRUE,
        );
        $form['accordion']['ov_panel']['ov_data'][$ov->type] = array_merge($form['accordion']['ov_panel']['ov_data'][$ov->type], $ov_form);
      }
    }
  }
  
  $form['button']['submit'] = array('#type' => 'submit',
    '#value' => t('Save'),
  );
  if (isset($map->mid) && $map->mid > 0) {
    $form['button']['clone'] = array('#type' => 'submit',
      '#value' => t('Clone'),
      '#submit' => array('_gmaps_map_form_clone_submit'),
    );
    if ($map->locked && $map->modified) {
      $form['button']['reset'] = array('#type' => 'submit',
        '#value' => t('Reset'),
        '#submit' => array('_gmaps_map_form_reset_submit'),
      );
    }
    if (!$map->locked) {
      $form['button']['delete'] = array('#type' => 'submit',
        '#value' => t('Delete'),
        '#submit' => array('_gmaps_map_form_delete_submit'),
      );
    }
  }

  $form['#pre_render'][] = '_gmaps_map_form_pre_render';

  return $form;
}

function _gmaps_map_form_validate_main($form, &$form_state) {
  $map = &$form_state['values'];
  $map['mid'] = $form['#gmaps_map']->mid;
  $map['default_id'] = $form['#gmaps_map']->default_id ? $form['#gmaps_map']->default_id : md5(uniqid(mt_rand(), true));

  $exists = db_fetch_object(db_query("SELECT * FROM {gmaps_maps} m WHERE m.mid != %d AND m.name = '%s'", $map['mid'], $map['name']));
  if ($exists) {
    form_set_error('name', t('The name %name is already taken.', array('%name' => $map['name'])));
  }
  
  if (intval($map['throttle_lifetime']) != $map['throttle_lifetime'] || $map['throttle_lifetime'] < 0) {
    form_set_error('throttle_lifetime', t('Throttle cache lifetime must be an integer greater than or equal to zero.'));
  }
  
  if (!in_array($map['default_maptype'], $map['maptypes'])) {
    form_set_error('default_maptype', t('The default map type must be one of the allowed map types.'));
  }
  
  if (!form_get_errors()) {
    $map['maptypes'] = array_keys($map['maptypes']);
  }
}

function _gmaps_map_form_validate_all($form, &$form_state) {
  $map = &$form_state['values'];
  $map['mid'] = $form['#gmaps_map']->mid;
  $map['type'] = $form['#gmaps_map']->type;
  
  $info = &$form['#gmaps_map_info'];
  
  gmaps_map_invoke_all('validate', $form['#gmaps_map']->type, $form, $form_state);

  _gmaps_map_form_validate_main($form, $form_state);

  $pg = &$map['map_data']['progressbar'];
  if (intval($pg['width']) != $pg['width'] || $pg['width'] < 0) {
    form_set_error('map_data][progressbar][width', t('Progress bar width must be an integer greater than zero.'));
  }

  if ($map['type'] == GMAPS_MAP_EARTH) {
    $svu = &$map['map_data']['svu'];
    if (intval($svu['height']) != $svu['height'] || $svu['height'] <= 0) {
      form_set_error('map_data][svu][height', t('Street view height must be an integer greater than zero.'));
    }
  }
  else {
    unset($map['map_data']['svu']);
  }
  
  $grid = $map['map_data']['toc_map']['grid'];
  if (intval($grid['columns']) != $grid['columns'] || $grid['columns'] < 1) {
    form_set_error('map_data][toc_map][grid][columns', t('Content grid columns must be an integer greater than or equal to one.'));
  }
  $grid = $map['map_data']['toc_map']['content']['grid'];
  if (intval($grid['columns']) != $grid['columns'] || $grid['columns'] < 1) {
    form_set_error('map_data][toc_map][content][grid][columns', t('Content tabs grid columns must be an integer greater than or equal to one.'));
  }
  
  if (!form_get_errors()) {
    if (isset($map['controls'])) {
      foreach($map['controls'] as $ct => $ct_data) {
        //clear position settings when the current control doesn't support it
        if (!empty($ct_data['control']) && $info->controls[$ct_data['control']]->position == GMAPS_MAP_CONTROL_POSITION_STATIC) {
          $map['controls'][$ct]->position['anchor'] = -1;
          $map['controls'][$ct]->position['offset'] = '';
        }
        if (isset($ct_data['control_data'])) {
          if (!isset($map['control_data'])) {
            $map['control_data'] = array();
          }
          foreach($ct_data['control_data'] as $control => $data) {
            $map['control_data'][$control] = $data;
          }
          unset($map['controls'][$ct]['control_data']);
        }
      }
    }
  }
}

function _gmaps_map_form_submit($form, &$form_state) {
  $original_map = $form['#gmaps_map'];
  $map = &$form_state['values'];

  gmaps_map_invoke_all('submit', $form['#gmaps_map']->type, $form, $form_state);

  if ($original_map->locked) {
    $map['modified'] = 1;
  }

  $save = drupal_write_record('gmaps_maps', $map, $original_map->mid ? 'mid' : array());
  if ($save) {
    if ($save == SAVED_NEW) {
      drupal_set_message(t('The map has been created.'));
    }
    else {
      drupal_set_message(t('The map has been updated.'));
      $form_state['redirect'] = 'admin/gmaps/maps';
    }
  }
  
  if ($original_map->mid) {
    gmaps_map_cache_clear_all('%:b.'. $original_map->mid .':', NULL, TRUE);
    cache_clear_all();
  }
}

function _gmaps_map_form_submit_clone($form, &$form_state) {
  $map = $form['#gmaps_map'];
  $values = &$form_state['values'];
  
  if (_gmaps_map_clone($map, $values)) {
    drupal_set_message(t('The map has been cloned.'));
    $form_state['redirect'] = 'admin/gmaps/map/'. $map->mid .'/edit';
  }
  else {
    $form_state['redirect'] = 'admin/gmaps/maps';
  }
}

/**
 * Helper function to allow usage of drupal_add_css() and drupal_add_js() on cached forms.
 *
 * @param array $form
 * 
 * @return array
 */
function _gmaps_map_form_pre_render($form) {
  $map_type = $form['#gmaps_map']->type;
  $info = gmaps_map_get_info($map_type);
  foreach(_gmaps_map_get_hook_map() as $hook => $hook_info) {
    if (isset($info->$hook_info['info'])) {
      foreach($info->$hook_info['info'] as $type => $type_data) {
        if ($altered_form = gmaps_map_invoke($hook, $type, 'pre_render', $map_type, $form)) {
          $form = $altered_form;
        }
      }
    }
  }

  return $form;
}

function _gmaps_map_clone(&$map, $values) {
  foreach(_gmaps_map_get_hook_map() as $hook_info) {
    if (!isset($values[$hook_info['storage']])) {
      $values[$hook_info['storage']] = array();
    }
  }
  
  gmaps_map_invoke_all('clone', $map->type, $map, $values);
  
  if (!isset($values['name']) || empty($values['name'])) {
    $values['name'] = $map->name .' - '. md5(uniqid(mt_rand(), true));
  }
  if (!isset($values['default_id']) || empty($values['default_id'])) {
    $values['default_id'] = md5(uniqid(mt_rand(), true));
  }
  
  $map = gmaps_extend($map, $values);
  $map->mid = 0;

  return drupal_write_record('gmaps_maps', $map);
}

/**
 * Submit handler for the map clone button.
 * Redirects to the clone page.
 */
function _gmaps_map_form_clone_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/map/'. $form['#gmaps_map']->mid .'/clone';
}

/**
 * Submit handler for the map reset button.
 * Redirects to the reset page.
 */
function _gmaps_map_form_reset_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/map/'. $form['#gmaps_map']->mid .'/reset';
}

/**
 * Submit handler for the map delete button.
 * Redirects to the delete page.
 */
function _gmaps_map_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/gmaps/map/'. $form['#gmaps_map']->mid .'/delete';
}

/**
 * Executes the map reset form.
 */
function _gmaps_map_reset_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $map = &$form['#gmaps_map'];

    $default_maps = _gmaps_map_get_default_maps();
    $base_map = (object)$default_maps[$map->default_id];
    

    $skip_fields = array('mid', 'name', 'modified', 'locked');
    $info = gmaps_map_get_info($map->type);
    foreach(_gmaps_map_get_hook_map() as $hook => $hook_info) {
      $skip_fields[] = $hook_info['storage'];
      if (isset($info->$hook_info['info'])) {
        foreach($info->$hook_info['info'] as $type => $type_data) {
          if (!gmaps_map_invoke($hook, $type, 'reset', $map->type, $base_map, $map)) {
            $map->{$hook_info['storage']}[$type] = $base_map->{$hook_info['storage']}[$type];
          }
        }
      }
    }
    
    $map->modified = 0;
    foreach((array)$base_map as $field => $value) {
      if (!in_array($field, $skip_fields)) {
        $map->$field = $value;
      }
    }

    if (!db_result(db_query("SELECT 1 FROM {gmaps_maps} WHERE name = '%s' AND mid != %d", $base_map->name, $map->mid))) {
      $map->name = $base_map->name;
    }

    if (drupal_write_record('gmaps_maps', $map, 'mid')) {
      drupal_set_message(t('The map has been reseted.'));
    }

    gmaps_map_cache_clear_all('%:b.'. $map->mid .':', NULL, TRUE);
    cache_clear_all();
  }
  
  $form_state['redirect'] = 'admin/gmaps/maps';
}

/**
 * Present a form for deleting an map.
 */
function _gmaps_map_delete_form($form_state = NULL, $map) {

  $form = array();
  $form['#gmaps_map'] = $map;
  $form['#submit'][] = '_gmaps_map_delete_form_submit';

  $output = confirm_form($form,
    t('Are you sure you want to delete the map %map?', array('%map' => t($map->name))),
    'admin/gmaps/maps',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );

  return $output;
}

function _gmaps_map_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
   $map = &$form['#gmaps_map'];
    
    gmaps_map_invoke_all('delete', $map->type, $map);
    
    db_query("DELETE FROM {gmaps_maps} WHERE mid = %d", $map->mid);
    drupal_set_message(t('The map has been deleted.'));

    gmaps_map_cache_clear_all('%:b.'. $map->mid .':', NULL, TRUE);
    cache_clear_all();
  }

  $form_state['redirect'] = 'admin/gmaps/maps';
}

function _gmaps_map_get_default_maps() {
  $types = gmaps_map_get_types();
  $maps = array();
  $infos = array();
  $defaults = array();

  foreach (module_implements('gmaps_map_info') as $module) {
    $function = $module .'_gmaps_map_info';
    foreach ($types as $type) {
      if (!isset($infos[$type->type])) {
        $infos[$type->type] = gmaps_map_get_info($type->type);
      }
      if (!isset($defaults[$type->type])) {
        $defaults[$type->type] = _gmaps_map_get_defaults($infos[$type->type], $type);
      }
      
      $result = $function('default_maps', $defaults[$type->type]);
      if ($result) {
        foreach($result as $default_id => $map) {
          $map['mid'] = 0;
          $map['type'] = $type->type;
          $map['default_id'] = $default_id;
          $map['modified'] = 0;
          $map['locked'] = 1;
          
          if ($map['type'] != GMAPS_MAP_EARTH) {
            unset($map['svu']);
          }

          $maps[$default_id] = $map;
        }
      }
    }
  }

  return $maps;
}

function _gmaps_map_install_default_maps($maps = NULL) {
  if (empty($maps)) {
    $maps =  _gmaps_map_get_default_maps();
  }

  if ($maps) {
    //skip previously installed maps
    $result = db_query("SELECT default_id FROM {gmaps_maps} WHERE locked = 1");
    while($rec = db_fetch_object($result)) {
      unset($maps[$rec->default_id]);
    }
    
    foreach($maps as $default_id => $map) {
      $values = array(
        'name' => $map['name'],
        'default_id' => $map['default_id'],
      );
      $map = (object)$map;
      $maps[$default_id] = _gmaps_map_clone($map, $values);
    }
  }

  return $maps;
}
