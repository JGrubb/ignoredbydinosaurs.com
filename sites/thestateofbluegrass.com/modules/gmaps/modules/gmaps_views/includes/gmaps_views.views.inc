<?php
// $Id: gmaps_views.views.inc,v 1.1.2.4 2010/03/22 14:41:55 xmarket Exp $

/**
 * Implementation of hook_views_plugins
 */
function gmaps_views_views_plugins() {
  return array(
    'module' => 'gmaps_views', // This just tells our themes are elsewhere.
    'style' => array(
      'gmaps_accordion' => array(
        'title' => t('GMaps Accordion'),
        'help' => t('Displays rows as an Accordion element.'),
        'handler' => 'gmaps_views_plugin_style_element',
        'theme' => 'gmaps_views_view_element',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'uses fields' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'element type' => 'accordion',
      ),
      'gmaps_tabs' => array(
        'title' => t('GMaps Tabs'),
        'help' => t('Displays rows as a Tabs element.'),
        'handler' => 'gmaps_views_plugin_style_element',
        'theme' => 'gmaps_views_view_element',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'uses fields' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'element type' => 'tabs',
      ),
      'gmaps_map' => array(
        'title' => t('GMaps Map'),
        'help' => t('Displays rows on an interactive map.'),
        'handler' => 'gmaps_views_plugin_style_map',
        'theme' => 'gmaps_views_view_map',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'static maps api' => FALSE,
      ),
      'gmaps_static_map' => array(
        'title' => t('GMaps Static Map'),
        'help' => t('Displays rows on a static map.'),
        'handler' => 'gmaps_views_plugin_style_map',
        'theme' => 'gmaps_views_view_static_map',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'static maps api' => TRUE,
      ),
    ),
    'row' => array(
      'gmaps_accordion' => array(
        'title' => t('Fields - GMaps Accordion'),
        'help' => t('Displays the fields on an Accordion element with optional templates for each panel.'),
        'handler' => 'gmaps_views_plugin_row_element',
        'theme' => 'gmaps_views_row_element',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'parent' => 'fields',
        'element type' => 'accordion',
      ),
      'gmaps_tabs' => array(
        'title' => t('Fields - GMaps Tabs'),
        'help' => t('Displays the fields on a Tabs element with optional templates for each tab.'),
        'handler' => 'gmaps_views_plugin_row_element',
        'theme' => 'gmaps_views_row_element',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'parent' => 'fields',
        'element type' => 'tabs',
      ),
      'gmaps_map_marker_node' => array(
        'title' => t('Map node marker'),
        'help' => t('GMaps map overlay. Sets the marker content for the standard view of the node.'),
        'handler' => 'gmaps_views_plugin_row_marker_node',
        'theme' => 'gmaps_views_view_row_marker_node',
        'base' => array('node'), // only works with 'node' as base.
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'static maps api' => FALSE,
      ),
      'gmaps_static_map_marker_node' => array(
        'title' => t('Static map node marker'),
        'help' => t('GMaps static map overlay. Sets the marker content for the standard view of the node.'),
        'handler' => 'gmaps_views_plugin_row_marker_node',
        'theme' => 'gmaps_views_view_row_marker_node',
        'base' => array('node'), // only works with 'node' as base.
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'static maps api' => TRUE,
      ),
      'gmaps_map_marker_fields' => array(
        'title' => t('Map fields marker'),
        'help' => t('GMaps map overlay. Sets the marker content for the fields of the display.'),
        'handler' => 'gmaps_views_plugin_row_marker_fields',
        'theme' => 'gmaps_views_view_fields',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'static maps api' => FALSE,
      ),
      'gmaps_static_map_marker_fields' => array(
        'title' => t('Static map fields marker'),
        'help' => t('GMaps static map overlay. Sets the marker content for the fields of the display.'),
        'handler' => 'gmaps_views_plugin_row_marker_fields',
        'theme' => 'gmaps_views_view_fields',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'theme path' => drupal_get_path('module', 'gmaps_views') . '/theme',
        'theme file' => 'theme.inc',
        'static maps api' => TRUE,
      ),
    ),
    'cache' => array(
      'gmaps_views_map' => array(
        'title' => t('GMaps Views map'),
        'help' => t('Cache plugin to use with GMaps Map style plugins.'),
        'handler' => 'gmaps_views_plugin_cache',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'uses options' => TRUE,
        'static maps api' => FALSE,
      ),
      'gmaps_views_static_map' => array(
        'title' => t('GMaps Views static map'),
        'help' => t('Cache plugin to use with GMaps Static Map style plugins.'),
        'handler' => 'gmaps_views_plugin_cache',
        'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
        'uses options' => TRUE,
        'static maps api' => TRUE,
      ),
    ),
  );
}

/**
 * Implementation of hook_views_handlers().
 */
function gmaps_views_views_handlers() {
  $handlers = array(
    'info' => array(
      'path' => drupal_get_path('module', 'gmaps_views') . '/includes/handlers',
    ),
    'handlers' => array(
      'gmaps_views_handler_row_panel' => array(
        'parent' => 'views_handler_field',
      ),
      'gmaps_views_handler_field_marker_sticky' => array(
        'parent' => 'views_handler_field_boolean',
      ),
      'gmaps_views_handler_field_marker_link_node' => array(
        'parent' => 'views_handler_field_node',
      ),
      'gmaps_views_handler_field_marker_link_user' => array(
        'parent' => 'views_handler_field_user',
      ),
      'gmaps_views_handler_field_map_center' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
  
  if (module_exists('taxonomy')) {
    $handlers['handlers']['gmaps_views_handler_field_marker_link_term'] = array(
      'parent' => 'views_handler_field_taxonomy',
    );
  }
  
  return $handlers;
}

/**
 * Implementation of hook_views_data_alter().
 */
function gmaps_views_views_data_alter(&$data) {
  $data['views']['gmaps_row_panel'] = array(
    'title' => t('Panel selector'),
    'help' => t('Panel selector for both GMaps Accordion and GMaps Tabs row style plugins. Fields heavier than this one up to the next panel selector will be displayed on this panel.'),
    'field' => array(
      'handler' => 'gmaps_views_handler_row_panel',
    ),
  );

  $group = t('Map option');
  $help = t('Interactive and static map option.');
  $data['views']['gmaps_map_center_viewloc'] = array(
    'group' => $group,
    'title' => t('Center on view location'),
    'help' => $help,
    'field' => array(
      'handler' => 'gmaps_views_handler_field_map_center',
    ),
  );
  $data['views']['gmaps_map_center_refloc'] = array(
    'group' => $group,
    'title' => t('Center on reference location'),
    'help' => $help,
    'field' => array(
      'handler' => 'gmaps_views_handler_field_map_center',
    ),
  );
  if (module_exists('geoip')) {
    $data['views']['gmaps_map_center_geoip'] = array(
      'group' => $group,
      'title' => t('Center on GeoIP location'),
      'help' => $help,
      'field' => array(
        'handler' => 'gmaps_views_handler_field_map_center',
      ),
    );
  }
  
  $data['node']['marker_sticky'] = array(
    'group' => t('Marker option'),
    'title' => t('Sticky option'),
    'title short' => t('Sticky'),
    'help' => t('Interactive and static marker option.'),
    'field' => array(
      'field' => 'sticky',
      'handler' => 'gmaps_views_handler_field_marker_sticky',
      'click sortable' => TRUE,
    ),
  );
  $data['node']['marker_link'] = array(
    'group' => t('Marker option'),
    'title' => t('Link option'),
    'title short' => t('Node link'),
    'help' => t('Interactive marker option. Sets the marker link options to the node path.'),
    'field' => array(
      'field' => 'nid',
      'handler' => 'gmaps_views_handler_field_marker_link_node',
      'click sortable' => TRUE,
    ),
  );
  $data['users']['marker_link'] = array(
    'group' => t('User marker option'),
    'title' => t('Link option'),
    'title short' => t('User link'),
    'help' => t('Interactive marker option. Sets the marker link options to the user path.'),
    'field' => array(
      'field' => 'uid',
      'handler' => 'gmaps_views_handler_field_marker_link_user',
      'click sortable' => TRUE,
    ),
  );
  
  if (module_exists('taxonomy')) {
    $data['term_data']['marker_link'] = array(
      'group' => t('Term marker option'),
      'title' => t('Link option'),
      'title short' => t('Term link'),
      'help' => t('Interactive marker option. Sets the marker link options to the term path.'),
      'field' => array(
        'field' => 'tid',
        'handler' => 'gmaps_views_handler_field_marker_link_term',
        'click sortable' => TRUE,
      ),
    );
  }
}

function gmaps_views_node_marker_autocomplete() {
  $status = FALSE;
  $data = array();
  if (!empty($_POST)) {
    if (isset($_POST['#accordion'])) {
      $post = $_POST['#accordion']['data'];
    }
    else if (isset($_POST['#tabs'])) {
      $post = $_POST['#tabs']['data'];
    }
    else {
      $post = $_POST;
    }
    
    if (isset($post['gmi'])) {
      $gmi = TRUE;
      $map = $post['gmi'];
    }
    else {
      $map = $post['smi'];
    }
    $marker = $post['marker'];
    if ($view = views_get_view($post['view'])) {
      if ($view->set_display($post['display'])) {
        $row_plugin = $view->display_handler->get_plugin('row');
        $field_alias = $post['field_alias'];
        $row = (object)array($field_alias => $post['nid']);
        
        if (!empty($row_plugin->options['min']['build_mode'])) {
          $row_plugin->view->gmaps_context = 'min';
          if ($output = theme($row_plugin->theme_functions(), $row_plugin->view, $row_plugin->options['min'], $row, $field_alias)) {
            $data['min'] = $output;
          }
          unset($row_plugin->view->gmaps_context);
        }
        
        if (!empty($row_plugin->options['max']['build_mode'])) {
          $row_plugin->view->gmaps_context = 'max';
          if ($output = theme($row_plugin->theme_functions(), $row_plugin->view, $row_plugin->options['max'], $row, $field_alias)) {
            $data['max'] = $output;
          }
          unset($row_plugin->view->gmaps_context);
        }
      
        if (!$gmi || $map['base']['map_data']['toc_map']['enabled']) {
          $style = $gmi ? $map['base']['map_data']['toc_map']['content'] : $map['base']['content']['content'];
          $data = gmaps_overlay_render_content($data, $style, $map['id'] .'-'. $marker['id']);
        }
        
        $status = TRUE;
      }
    }
  }
  gmaps_ajax_json(array('status' => TRUE, 'data' => $data));
  exit();
}



interface gmaps_views_map {
  function get_map_reference_mid();
}

interface gmaps_views_map_option {
  function set_map_option(&$gmi);
}

interface gmaps_views_map_overlay {
  function add_overlay(&$gmi, $row);
}

interface gmaps_views_map_overlay_marker extends gmaps_views_map_overlay {};

interface gmaps_views_map_overlay_option {
  function set_overlay_option(&$gmi, &$overlay, $values);
}

interface gmaps_views_map_overlay_option_marker extends gmaps_views_map_overlay_option {};

interface gmaps_views_map_overlay_option_marker_point extends gmaps_views_map_overlay_option_marker {
  function get_map_type();
};

interface gmaps_views_cache_map {
  function get_map_cache_options();
}


interface gmaps_views_static_map {
  function get_static_map_reference_mid();
}

interface gmaps_views_static_map_option {
  function set_static_map_option(&$smi);
}

interface gmaps_views_static_map_overlay {
  function add_static_overlay(&$smi, $row);
}

interface gmaps_views_static_map_overlay_marker extends gmaps_views_static_map_overlay {};

interface gmaps_views_static_map_overlay_option {
  function set_static_overlay_option(&$smi, &$overlay, $values);
}

interface gmaps_views_static_map_overlay_option_marker extends gmaps_views_static_map_overlay_option {};

interface gmaps_views_static_map_overlay_option_marker_point extends gmaps_views_static_map_overlay_option_marker {};

interface gmaps_views_cache_static_map {
  function get_static_map_cache_options();
}
