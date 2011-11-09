<?php
// $Id: theme.inc,v 1.1.2.17 2010/04/01 07:42:10 xmarket Exp $

/**
 * @file
 * Theme preprocess functions for gmaps' themeables.
 */

/**
 * Theme preprocess function for gmaps-length.tpl.php.
 */
function template_preprocess_gmaps_length(&$vars) {
  $vars['group_css'] = strtr($vars['unit']->group['key'], '_', '-');
  $vars['unit_key_css'] = strtr($vars['unit']->key, '_', '-');
}

/**
 * Theme preprocess function for gmaps-length-multipart.tpl.php.
 */
function template_preprocess_gmaps_length_multipart(&$vars) {
  $vars['group_css'] = strtr($vars['group'], '_', '-');
}

/**
 * Display a help icon with a link to view the topic in a popup.
 *
 * @param $module
 *   The module that owns this help topic.
 * @param $topic
 *   The identifier for the topic
 * @param $fragment
 *   link fragment (anchor)
 */
function theme_gmaps_element_help_topic($module, $topic) {
  static $js_added = FALSE;
  $info = advanced_help_get_topic($module, $topic);
  if (!$info) {
    return;
  }
  $text = '<span>' . t('Help') . '</span>';
  $class = 'advanced-help-link gmaps-element-help';

  drupal_add_css(drupal_get_path('module', 'advanced_help') . '/help-icon.css');
  if (user_access('view advanced help popup')) {
    if (!$js_added) {
      //tipically used in #title, where check_plain() removes 'onclick'
      gmaps_add_js();
      drupal_add_js(array('gmaps' => array('elementHelp' => array('width' => $info['popup width'], 'height' => $info['popup height']))), 'setting');
      $js_added = TRUE;
    }
    return l($text, "help/$module/$topic", array(
      'attributes' => array(
        'class' => $class,
        'title' => $info['title'],
        'target' => '_blank',
      ),
      'query' => array('popup' => TRUE),
      'html' => TRUE)
    );
  }
  else {
    return l($text, "help/$module/$topic", array(
      'attributes' => array(
        'class' => $class,
        'title' => $info['title'],
        'target' => '_blank',
      ),
      'html' => TRUE)
    );
  }
}

/**
 * Render a gmaps_icon preview image.
 * 
 * @param mixed $icon
 *  fid, filepath or icon array or object
 * @param mixed $max_size
 *  Any kind of pixel pairs. Defaults to global inline preview size.
 * 
 * @return html
 */
function theme_gmaps_icon_preview($icon, $max_size = NULL) {
  static $default_max = NULL;

  if (is_null($default_max)) {
    $settings = gmaps_get_settings('gmaps_icon_settings');
    $default_max = $settings['inline_preview_size'];
  }

  if (is_numeric($icon)) {
    $file = db_fetch_object(db_query('SELECT * FROM {files} WHERE fid = %d', $icon));
  }
  else if (is_string($icon)) {
    $file = db_fetch_object(db_query("SELECT * FROM {files} WHERE filepath = '%s'", $icon));
  }
  else if (is_array($icon)) {
    $file = (object)$icon;
  }
  else {
    $file = $icon;
  }

  if ($file) {
    //preview
    $info = image_get_info($file->filepath);
    $max = gmaps_pixels_parse($max_size ? $max_size : $default_max);
    if ($max[0] < $info['width']) {
      $info['height'] = round(($max[0] / $info['width']) * $info['height']);
      $info['width'] = $max[0];
    }
    if ($max[1] < $info['height']) {
      $info['width'] = round(($max[1] / $info['height']) * $info['width']);
      $info['height'] = $max[1];
    }
    $img_attr = array('width' => $info['width'], 'height' => $info['height']);
    return theme('image', file_create_url($file->filepath), $file->filename, $file->filename, $img_attr, FALSE);
  }

  return '';
}

function theme_gmaps_references($refs) {
  $output = '';

  foreach($refs as $group => $links) {
    $output .= '<dt>'. $group . '</dt><dd>'. theme('links', $links, array('class' => 'gmaps-reference-links')) .'</dd>';
  }
  return '<dl class="gmaps-references">'. $output .'</dl>';
}

function theme_button_button($element) {
  // Make sure not to overwrite classes.
  $class = 'form-button form-button-button';
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }

  return '<input type="button" id="'. $element['#id'].'" value="'. check_plain($element['#value']) .'" '. drupal_attributes($element['#attributes']) ." />\n";
}

function theme_gmaps_adr_country($code, $name) {
  return $code .' - '. $name;
}

function theme_gmaps_adr_state($code, $name, $adminarea) {
  $output = $code;
  $output .= empty($output) ? ''. $name : (empty($name) ? '' : ' - '. $name);
  return  empty($output) ? $adminarea : $output;
}

function template_preprocess_gmaps_adr(&$vars) {
  $vars['content'] = new stdClass();
  foreach((array)$vars['links'] as $part => $link) {
    if (isset($link['href']) && !empty($link['title'])) {
      $vars['content']->$part = l($link['title'], $link['href'], $link);
    }
    else if (!empty($link['title'])) {
      if (empty($link['html'])) {
        $link['title'] = check_plain($link['title']);
      }
      $span_attributes = '';
      if (isset($link['attributes'])) {
        $span_attributes = drupal_attributes($link['attributes']);
      }
      $vars['content']->$part = '<span'. $span_attributes .'>'. $link['title'] .'</span>';
    }
    else {
      $vars['content']->$part = '';
    }
  }
}

function theme_gmaps_adr_link($links, $adr) {
  $vars = array('links' => $links, 'adr' => $adr);
  template_preprocess_gmaps_adr($vars);
  $content = $vars['content'];

  $rows = array();
  if (!empty($content->thoroughfare)) {
    $rows[] = '<span class="gmaps-adr-link-row">'. $content->thoroughfare .'</span>';
  }
  if (!empty($content->locality)) {
    $row = '<span class="gmaps-adr-link-row">';
    if (!empty($content->deplocality)) {
      $row .= $content->deplocality .', ';
    }
    $rows[] = $row . $content->locality .'</span>';
  }
  if (!empty($content->adminarea) || !empty($content->postalcode)) {
    $row = '<span class="gmaps-adr-link-row">';
    if (!empty($content->adminarea)) {
      if (!empty($content->subadminarea)) {
        $row .= $content->subadminarea .', ';
      }
      $row .= $content->adminarea;
    }
    if (!empty($content->postalcode)) {
      $row .= (empty($content->adminarea) ? '' : ' ') . $content->postalcode;
    }
    $rows[] = $row .'</span>';
  }
  $rows[] = '<span class="gmaps-adr-link-row gmaps-adr-link-row-last">'. $content->country .'</span>';
  
  return '<span class="gmaps-adr-link adr">'. implode(', ', $rows) .'</span>';
}

function theme_gmaps_adr_link_plain($adr) {
  $nbsp = "\xC2\xA0";
  $lines = array();
  if (!empty($adr->thoroughfare)) {
    $lines[] = str_replace(' ', $nbsp, $adr->thoroughfare);
  }
  if (!empty($adr->locality)) {
    $line = '';
    if (!empty($adr->deplocality)) {
      $line = $adr->deplocality .', ';
    }
    $lines[] = str_replace(' ', $nbsp, $line . $adr->locality);
  }
  $line = '';
  if (!empty($adr->adminarea)) {
    if (!empty($adr->subadminarea)) {
      $line = $adr->subadminarea .', ';
    }
    $line .= $adr->adminarea;
  }
  if (!empty($adr->postalcode)) {
    $line .= (empty($adr->adminarea) ? '' : ' ') . $adr->postalcode;
  }
  if (!empty($line)) {
    $lines[] = str_replace(' ', $nbsp, $line);
  }
  if (!empty($adr->country)) {
    $lines[] = str_replace(' ', $nbsp, $adr->country);
  }
  
  //if you replace glue by "\n",
  //add this ruleset to your css: "span.gmaps-adr-plain{ white-space: pre; }",
  //then it will look like as its themed counterpart.
  return implode(', ', $lines);
}

function theme_gmaps_coords_plain($content) {
  return implode(', ', $content);
}

function theme_gmaps_coords($content, $coords, $dms = FALSE) {
  //Standard Geo microformat
  //See microformats: http://microformats.org/wiki/geo
  $mf_classes = (isset($coords->map_type) && $coords->map_type == GMAPS_MAP_EARTH) ? array(' geo', ' latitude', ' longitude') : array();
  $output = '<span class="gmaps-coords '. ($dms ? 'gmaps-coords-dms' : 'gmaps-coords-dd') . $mf_classes[0] .'">';
  $output .= '<abbr class="gmaps-coords-latitude'. $mf_classes[1] .'" title="'. $coords->latitude .'">' . $content['lat'] . '</abbr>, ';
  $output .= '<abbr class="gmaps-coords-longitude'. $mf_classes[2] .'" title="'. $coords->longitude .'">' . $content['lng'] . '</abbr>';
  if (!empty($content['h'])) {
    $output .= ', <abbr class="gmaps-coords-elevation"">' . $content['h'] . '</abbr>';
  }
  $output .= '</span>';
  return $output;
}

function theme_gmaps_coords_cart($content, $coords) {
  //geo microformat support
  $dd = (isset($coords->map_type) && $coords->map_type == GMAPS_MAP_EARTH) ? theme('gmaps_coords', $content, $coords) : '';

  $output = '<span class="gmaps-coords-cart">'. $dd;
  $output .= '<span class="gmaps-coords-cart-x">' . $content['x'] . '</span>, ';
  $output .= '<span class="gmaps-coords-cart-y">' . $content['y'] . '</span>, ';
  $output .= '<span class="gmaps-coords-cart-z">' . $content['z'] . '</span>';
  $output .= '</span>';
  return $output;
}

function theme_gmaps_google_directions($object, $label = NULL, $isAdr = FALSE) {
  $output = '<span class="gmaps-directions gmaps-google-directions"><label>'. t('Google directions') .'</label>';
  $output .= ' <span class="gmaps-directions-from">'. l(t('From here'), gmaps_map_link_google_directions($object, FALSE, $label, $isAdr)) .'</span>';
  $output .= ' <span class="gmaps-directions-to">'. l(t('To here'), gmaps_map_link_google_directions($object, TRUE, $label, $isAdr)) .'</span>';
  $output .= '</span>';

  return $output;
}

function theme_gmaps_mapquest_directions($object) {
  $output = '<span class="gmaps-directions gmaps-mapquest-directions"><label>'. t('MapQuest directions') .'</label>';
  $output .= ' <span class="gmaps-directions-from">'. l(t('From here'), gmaps_map_link_mapquest_directions($object, FALSE)) .'</span>';
  $output .= ' <span class="gmaps-directions-to">'. l(t('To here'), gmaps_map_link_mapquest_directions($object, TRUE)) .'</span>';
  $output .= '</span>';

  return $output;
}

function theme_gmaps_yahoo_directions($object, $isAdr = FALSE) {
  $output = '<span class="gmaps-directions gmaps-yahoo-directions"><label>'. t('Yahoo! directions') .'</label>';
  $output .= ' <span class="gmaps-directions-from">'. l(t('From here'), gmaps_map_link_yahoo_directions($object, FALSE, $isAdr)) .'</span>';
  $output .= ' <span class="gmaps-directions-to">'. l(t('To here'), gmaps_map_link_yahoo_directions($object, TRUE, $isAdr)) .'</span>';
  $output .= '</span>';

  return $output;
}

function _gmaps_accordion_clean_id($id) {
  static $seen_ids = array();

  $id = str_replace(array('][', '_', ' '), '-', $id);

  if (isset($seen_ids[$id])) {
    $id = $id .'-'. $seen_ids[$id]++;
  }
  else {
    $seen_ids[$id] = 1;
  }

  return $id;
}

function _gmaps_accordion_pre_render($element) {
  static $added = FALSE;
  static $effects_added = FALSE;
  
  if (!$added) {
    jquery_ui_add('ui.accordion');
    gmaps_add_js();
    drupal_add_js('document.write(\'<style type="text/css">.gmaps-accordion .gmaps-accordion-panel {display: none;}</style>\');', 'inline');
    $added = TRUE;
  }
  
  $element['#id'] = _gmaps_accordion_clean_id('accordion-'. ($element['#id'] ? $element['#id'] : 'gmaps'));
  
  $element['#required'] = FALSE;
  $element['#sorted'] = TRUE;
  
  if (!isset($element['#accordion'])) {
    $element['#accordion'] = array();
  }
  
  if (!isset($element['#accordion']['active'])) {
    $element['#accordion']['active'] = NULL;
  }
  
  //drupalized shortcuts
  if ($element['#collapsible']) {
    $element['#accordion']['collapsible'] = TRUE;
    if ($element['#collapsed']) {
      $element['#accordion']['active'] = FALSE;
    }
    else if ($element['#accordion']['active'] === FALSE || $element['#accordion']['active'] === NULL) {
      $element['#accordion']['active'] = 0;
    }
  }
  else {
    $element['#collapsed'] = FALSE;
    $element['#accordion']['collapsible'] = FALSE;
  }
  //1.6
  $element['#accordion']['alwaysOpen'] = !$element['#accordion']['collapsible'];
  
  $element['#accordion']['header'] = 'h3.'. $element['#id'];
  $element['#accordion']['selectedClass'] = 'ui-state-active';
  
  //fieldsets are jumping around in IE6 when set to TRUE
  if (!isset($element['#accordion']['autoHeight'])) {
    $element['#accordion']['autoHeight'] = FALSE;
  }
  if (!isset($element['#accordion']['navigation'])) {
    $element['#accordion']['navigation'] = FALSE;
  }
    
  $i = 0; $ahah_panels = array(); $panel_url = 0;
  foreach(element_children($element) as $panel_id) {
    if (isset($element[$panel_id]['#type']) && $element[$panel_id]['#type'] == 'gmaps_accordion_panel') {
      $panel = &$element[$panel_id];
      $panel['#id'] = _gmaps_accordion_clean_id($element['#id'] .'-'. ($panel['#id'] ? $panel['#id'] : 'panel'));
      $panel['#accordion_id'] = $element['#id'];
      
      //synchronize CSS and JS
      if ($panel['#active_panel']) {
        if (is_null($element['#accordion']['active'])) {
          $element['#accordion']['active'] = $i;
        }
        else if ($element['#accordion']['active'] === FALSE || $element['#accordion']['active'] != $i) {
          $panel['#active_panel'] = FALSE;
        }
      }
      else if (!is_null($element['#accordion']['active']) && $element['#accordion']['active'] !== FALSE && $element['#accordion']['active'] == $i) {
        $panel['#active_panel'] = TRUE;
      }
      
      //add AHAH and AJAX settings
      if (isset($panel['#ahah']) && ($ahah = _gmaps_expand_ahah($panel))) {
        $ahah_panels[$panel['#id']] = $ahah;
        $panel_url |= isset($ahah['url']);
      }
      
      if ($i == 0) {
        if (!isset($panel['#attributes'])) {
          $panel['#attributes'] = array();
        }
        $panel['#attributes']['class'] .= ' first';
        $first_panel_id = $panel_id;
      }
      
      $i++;
    }
  }
  if (is_null($element['#accordion']['active'])) {
    $element['#accordion']['active'] = FALSE;
  }
  if ($i > 0) {
    $i--;
    if ($element['#accordion']['active'] !== FALSE && $element['#accordion']['active'] > $i) {
      $element['#accordion']['active'] = $i;
      $panel['#active_panel'] = TRUE;
    }
  }
  $panel['#attributes']['class'] .= ' last';
  
  if ((!$element['#collapsible'] || !$element['#collapsed']) && $element['#accordion']['active'] === FALSE) {
    $element['#accordion']['active'] = 0;
    $element[$first_panel_id]['#active_panel'] = TRUE;
  }
  
  if (!$effects_added && !empty($element['#accordion']['animated']) && $element['#accordion']['animated'] !== 'slide') {
    jquery_ui_add('effects.core');
    $effects_added = TRUE;
  }
  
  $settings = array('options' => $element['#accordion']);
  //set defaults on the element if there are ahah enabled panels
  if (!empty($ahah_panels) && !isset($element['#ahah'])) {
    $element['#ahah'] = array();
  }
  $ahah = _gmaps_expand_ahah($element);
  if (($ahah && isset($ahah['url'])) || $panel_url) {
    if ($ahah) {
      $settings['ahahOptions'] = $ahah;
    }
    if (!empty($ahah_panels)) {
      $settings['ahahPanels'] = $ahah_panels;
    }
  }
  drupal_add_js(array(
    'gmaps' => array(
      'accordion' => array(
        $element['#id'] => $settings,
      ),
    ),
  ), 'setting');
  
  $class = 'form-gmaps-accordion';
  if ($element['#embedded_item']) {
    $class .= ' gmaps-accordion-embedded';
  }
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }
  
  return $element;
}
/**
 * Format a group of form items as an accordion.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used: attributes, title, value, description, children, accordion, id
 * 
 * @return
 *   A themed HTML string representing the form item group.
 *
 * @ingroup themeable
 */
function theme_gmaps_accordion($element) {
  $output = '<div id="'. $element['#id'] .'" '. drupal_attributes($element['#attributes']) .'>' . (!empty($element['#children']) ? ('<div class="gmaps-accordion">'. $element['#children'] .'</div>') : '') .'</div>'. (isset($element['#value']) ? $element['#value'] : '');

  return theme('form_element', $element, $output);
}

/**
 * Format an accordion panel.
 * 
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used: id, title, accordion_id, active_panel, value, description, children
 * 
 * @return
 *   A themed HTML string representing the form item group.
 *
 * @ingroup themeable
 */
function theme_gmaps_accordion_panel($element) {
  $element['#attributes']['class'] = 'gmaps-accordion-header'. ($element['#accordion_id'] ? ' '. $element['#accordion_id'] : '') .
    ((isset($element['#active_panel']) && $element['#active_panel']) ? ' ui-state-active' : '') .' '. $element['#attributes']['class'];
  $output = '<h3 '. drupal_attributes($element['#attributes']) .'><a href="#'. $element['#id'] .'">'. $element['#title'] ."</a></h3>\n";
  //1.6 doesn't support 'ui-accordion-content-active'
  $output .= '<div id="'. $element['#id'] .'" class="gmaps-accordion-panel'. ((isset($element['#active_panel']) && $element['#active_panel']) ? ' ui-accordion-content-active' : '') ."\">\n";
  //$output .= '<div id="'. $element['#id'] .'" class="gmaps-accordion-panel">'."\n";
  
  if (!empty($element['#description'])) {
    $output .= '<div class="description">'. $element['#description'] ."</div>\n";
  }
  $output .= '<div id="'. $element['#id'] .'-content">';
  $output .= (!empty($element['#children']) ? $element['#children'] : '') . (isset($element['#value']) ? $element['#value'] : '');
  $output .= "</div></div>\n";
  
  return $output;
}

function _gmaps_tabs_clean_id($id) {
  static $seen_ids = array();

  $id = str_replace(array('][', '_', ' '), '-', $id);

  if (isset($seen_ids[$id])) {
    $id = $id .'-'. $seen_ids[$id]++;
  }
  else {
    $seen_ids[$id] = 1;
  }

  return $id;
}

function _gmaps_tabs_pre_render($element) {
  static $added = FALSE;
  
  if (!$added) {
    jquery_ui_add('ui.tabs');
    gmaps_add_js();
    drupal_add_js('document.write(\'<style type="text/css">.gmaps-tabs .ui-tabs-hide {display: none;}</style>\');', 'inline');
    $added = TRUE;
  }
  
  $element['#id'] = _gmaps_tabs_clean_id('tabs-'. ($element['#id'] ? $element['#id'] : 'gmaps'));
  
  $element['#required'] = FALSE;
  $element['#sorted'] = TRUE;
  
  if (!isset($element['#nav_position'])) {
    $element['#nav_position'] = GMAPS_TABS_NAV_TOP;
  }
  
  if (!isset($element['#tabs'])) {
    $element['#tabs'] = array();
  }
  
  if (!isset($element['#tabs']['selected']) || !(is_numeric($element['#tabs']['selected']) || is_null($element['#tabs']['selected']))) {
    $element['#tabs']['selected'] = FALSE;
  }
  
  $element['#tabs']['tabSelector'] = 'li.' . $element['#id'] .'-tab';
  $element['#tabs']['tabTemplate'] = '<li class="gmaps-tabs-tab '. $element['#id'] .'-tab"><a href="#{href}"><span>#{label}</span></a></li>';
  $element['#tabs']['panelTemplate'] = '<div class="gmaps-tabs-panel '. $element['#id'] .'-panel"></div>';
  $element['#tabs']['deselectableClass'] = 'ui-tabs-collapsible';
  $element['#tabs']['idPrefix'] = $element['#id'] . '-tab-';

  if ($element['#nav_position'] == GMAPS_TABS_NAV_TOP) {
    if (!isset($element['#collapsible'])) {
      $element['#collapsible'] = FALSE;
    }
  }
  else {
    $element['#collapsible'] = FALSE;
  }
  $element['#tabs']['collapsible'] = $element['#tabs']['deselectable'] = $element['#collapsible'];
  if ($element['#collapsible']) {
    if ($element['#collapsed']) {
      $element['#tabs']['selected'] = NULL;
    }
    else if ($element['#tabs']['selected'] === NULL || $element['#tabs']['selected'] === FALSE) {
      $element['#tabs']['selected'] = 0;
    }
  }
  
  //tabs
  $i = 0; $ahah_panels = array(); $panel_url = 0;
  foreach(element_children($element) as $panel_id) {
    if (isset($element[$panel_id]['#type']) && $element[$panel_id]['#type'] == 'gmaps_tabs_panel') {
      $panel = &$element[$panel_id];
      $panel['#id'] = _gmaps_tabs_clean_id($element['#id'] .'-'. ($panel['#id'] ? $panel['#id'] : 'panel'));
      $panel['#tabs_id'] = $element['#id'];
      
      //synchronize CSS and JS
      if ($panel['#selected_panel']) {
        if ($element['#tabs']['selected'] === FALSE) {
          $element['#tabs']['selected'] = $i;
        }
        else if (is_null($element['#tabs']['selected']) || $element['#tabs']['selected'] != $i) {
          $panel['#selected_panel'] = FALSE;
        }
      }
      else if (!is_null($element['#tabs']['selected']) && $element['#tabs']['selected'] !== FALSE && $element['#tabs']['selected'] == $i) {
        $panel['#selected_panel'] = TRUE;
      }
      
      //add AHAH and AJAX settings
      if (isset($panel['#ahah']) && ($ahah = _gmaps_expand_ahah($panel))) {
        $ahah_panels[$panel['#id']] = $ahah;
        $panel_url |= isset($ahah['url']);
      }
      
            
      if ($i == 0) {
        if (!isset($panel['#attributes'])) {
          $panel['#attributes'] = array();
        }
        $panel['#attributes']['class'] .= ' first';
        $first_panel_id = $panel_id;
      }
      
      $i++;
    }
  }
  
  if ($element['#tabs']['selected'] === FALSE) {
    $element['#tabs']['selected'] = NULL;
  }
  if ($i > 0) {
    $i--;
    if (!is_null($element['#tabs']['selected']) && $element['#tabs']['selected'] > $i) {
      $element['#tabs']['selected'] = $i;
      $panel['#selected_panel'] = TRUE;
    }
  }
  $panel['#attributes']['class'] .= ' last';
  
  if ((!$element['#collapsible'] || !$element['#collapsed']) && is_null($element['#tabs']['selected'])) {
    $element['#tabs']['selected'] = 0;
    $element[$first_panel_id]['#selected_panel'] = TRUE;
  }
  
  $settings = array('options' => $element['#tabs']);
  //set defaults on the element if there are ahah enabled panels
  if (!empty($ahah_panels) && !isset($element['#ahah'])) {
    $element['#ahah'] = array();
  }
  $ahah = _gmaps_expand_ahah($element);
  if (($ahah && isset($ahah['url'])) || $panel_url) {
    if ($ahah) {
      $settings['ahahOptions'] = $ahah;
    }
    if (!empty($ahah_panels)) {
      $settings['ahahPanels'] = $ahah_panels;
    }
  }
  drupal_add_js(array(
    'gmaps' => array(
      'tabs' => array(
        $element['#id'] => $settings,
      ),
    ),
  ), 'setting');
  
  $class = 'form-gmaps-tabs gmaps-tabs-nav-'. $element['#nav_position'];
  if ($element['#embedded_item']) {
    $class .= ' gmaps-tabs-embedded';
  }
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = $class .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = $class;
  }
  
  return $element;
}

/**
 * Format a group of form items as a tabs panel.
 *
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used: attributes, title, value, description, children, accordion, id
 * 
 * @return
 *   A themed HTML string representing the form item group.
 *
 * @ingroup themeable
 */
function theme_gmaps_tabs($element) {
  $output = '<div id="'. $element['#id'] .'" '. drupal_attributes($element['#attributes']) .'><div class="gmaps-tabs">';
  
  //tabs
  $nav = "<ul class=\"gmaps-tabs-nav\">\n";
  $lineend = ($element['#nav_position'] == GMAPS_TABS_NAV_TOP || $element['#nav_position'] == GMAPS_TABS_NAV_BOTTOM) ? "\n" : '';
  foreach(element_children($element) as $panel_id) {
    if (isset($element[$panel_id]['#type']) && $element[$panel_id]['#type'] == 'gmaps_tabs_panel') {
      $panel = &$element[$panel_id];
      $nav .= '<li class="gmaps-tabs-tab '. $element['#id'] .'-tab'. ($panel['#selected_panel'] ? ' ui-tabs-selected' : '') .'"><a href="#'. $panel['#id'] .'"><span>'. $panel['#title'] .'</span></a></li>'. $lineend;
    }
  }
  $nav .= "</ul>\n";
  
  if ($element['#nav_position'] != GMAPS_TABS_NAV_BOTTOM) {
    $output .= $nav;
  }
  $output .= $element['#children'];
  if ($element['#nav_position'] == GMAPS_TABS_NAV_BOTTOM) {
    $output .= $nav;
  }
  if ($element['#nav_position'] == GMAPS_TABS_NAV_LEFT || $element['#nav_position'] == GMAPS_TABS_NAV_RIGHT) {
    $output .= '<div class="gmaps-tabs-clear-nav-'. $element['#nav_position'] .'"></div>';
  }
  $output .= '</div>'. (isset($element['#value']) ? $element['#value'] : '') . "</div>\n";
  
  return theme('form_element', $element, $output);
}

/**
 * Format a tabs panel.
 * 
 * @param $element
 *   An associative array containing the properties of the element.
 *   Properties used: id, title, tabs_id, selected_panel, value, description, children
 * 
 * @return
 *   A themed HTML string representing the form item group.
 *
 * @ingroup themeable
 */
function theme_gmaps_tabs_panel($element) {
  $element['#attributes']['class'] = 'gmaps-tabs-panel '. $element['#tabs_id'] .'-panel' .
    ((!isset($element['#selected_panel']) || !$element['#selected_panel']) ? ' ui-tabs-hide' : '') .' '. $element['#attributes']['class'];
  $output = '<div id="'. $element['#id'] .'" ' . drupal_attributes($element['#attributes']) .">\n";

  if (!empty($element['#description'])) {
    $output .= ' <div class="description">'. $element['#description'] ."</div>\n";
  }
  $output .= '<div id="'. $element['#id'] .'-content">';
  $output .= (!empty($element['#children']) ? $element['#children'] : '') . (isset($element['#value']) ? $element['#value'] : '');
  $output .= "</div></div>\n";
  
  return $output;
}

function template_preprocess_gmaps_map_item(&$vars) {
  global $user;
  
  $content = &$vars['gmi']->content;
  if (!empty($content)) {
    $toc = $vars['gmi']->base->map_data['toc_map'];
    $output = '';
    $panels = array();
    
    $i = 1;
    foreach($content as $id => $item) {
      if (!empty($item['content'])) {
        $anchor = '<a id="'. $vars['gmi']->id .'-'. $id .'"></a>';
        $item['title'] = empty($item['title']) ? t('Item !n', array('!n' => $i)) : filter_xss($item['title'], array());
        $item['content'] = gmaps_overlay_render_content($item['content'], $vars['gmi']->base->map_data['toc_map']['content']);

        if ($item['icon'] && $toc['icon'] && $toc['style'] != 'list') {
          $item['icon'] = is_object($item['icon']) ? $item['icon'] : gmaps_icon_load($item['icon']);
          if ($toc['icon'] == 'before') {
            $item['title'] = theme('gmaps_icon_preview', $item['icon']->icon) .' '. $item['title'];
          }
          else if ($toc['icon'] == 'after') {
            $item['title'] .= ' '. theme('gmaps_icon_preview', $item['icon']->icon);
          }
          else {
            $item['title'] = theme('gmaps_icon_preview', $item['icon']->icon);
          }
          $item['title'] = '<span>'. $item['title'] .'</span>';
        }
        
        if ($toc['style'] == 'plain') {
          $output .= $anchor . $item['content'];
        }
        else if ($toc['style'] == 'list' || $toc['style'] == 'grid') {
          $panels[] = array(
            'data' => $anchor . $item['content'],
            'title' => $item['title'],
            'alt' => $item['title'],
          );
        }
        else if ($toc['style'] == 'box') {
          $output .= $anchor . theme('box', $item['title'], $item['content']);
        }
        else if ($toc['style'] == 'fieldset') {
          $panel = array('#type' => 'fieldset',
            '#prefix' => $anchor,
            '#title' => $item['title'],
            '#value' => $item['content'],
            '#collapsible' => $toc['collapsible'],
            '#collapsed' => $toc['collapsed'],
          );
          $output .= drupal_render($panel);
        }
        else if ($toc['style'] == 'accordion') {
          $panel = array('#type' => 'gmaps_accordion_panel',
            '#id' => $id,
            '#prefix' => $anchor,
            '#title' => $item['title'],
            '#value' => $item['content'],
          );
          if (!empty($item['ahah'])) {
            $panel['#ahah'] = $item['ahah'];
          }
          $panels[] = $panel;
        }
        else if ($toc['style'] == 'tabs') {
          $panel = array('#type' => 'gmaps_tabs_panel',
            '#prefix' => $anchor,
            '#title' => $item['title'],
            '#value' => $item['content'],
          );
          if (!empty($item['ahah'])) {
            $panel['#ahah'] = $item['ahah'];
          }
          $panels[] = $panel;
        }
        
        $i++;
      }
    }
    
    if (!empty($panels)) {
      if ($toc['style'] == 'accordion') {
        $cont = array('#type' => 'gmaps_accordion',
          '#collapsible' => $toc['collapsible'],
          '#collapsed' => $toc['collapsed'],
        );
      }
      else if ($toc['style'] == 'tabs') {
        $cont = array('#type' => 'gmaps_tabs',
          '#collapsible' => $toc['collapsible'],
          '#collapsed' => $toc['collapsed'],
          '#nav_position' => $toc['tabs_nav'],
        );
      }
      else if ($toc['style'] == 'list') {
        $output .= theme('item_list', $panels);
      }
      else if ($toc['style'] == 'grid') {
        $output .= theme('gmaps_grid', $panels, $toc['grid']);
      }
      
      if ($cont) {
        $cont['#id'] = $vars['gmi']->id;
        $cont['#embedded_item'] = TRUE;
        
        $cont['#ahah'] = array();
        if ($toc['ahah']['progress']) {
          $cont['#ahah']['progress']['type'] = $toc['ahah']['progress'];
        }
        if ($toc['ahah']['message']) {
          $cont['#ahah']['progress']['message'] = t($toc['ahah']['message']);
        }
        if ($toc['ahah']['effect']) {
          $cont['#ahah']['effect'] = $toc['ahah']['effect'];
        }
        if (isset($vars['gmi']->toc_ahah)) {
          $cont['#ahah'] = gmaps_extend($cont['#ahah'], $vars['gmi']->toc_ahah);
        }
        if (empty($cont['#ahah'])) {
          unset($cont['#ahah']);
        }
        
        $output .= drupal_render(array_merge($cont, $panels));
      }
    }
    
    $vars['content'] = $output;
  }
  
  $class = 'gmaps-map-item';
  if ($vars['gmi']->embedded) {
    $class .= ' gmaps-map-item-embedded';
  }
  $vars['gmi']->attributes['class'] = $class .' '. $vars['gmi']->attributes['class'];
  
  $vars['template_files'][] = 'gmaps-map-item';
  $vars['template_files'][] = 'gmaps-map-item-type-'. strtr($vars['gmi']->base->type, '_', '-');
  if ($vars['gmi']->base->locked) {
    $vars['template_files'][] = 'gmaps-map-item-default-'. strtr($vars['gmi']->base->default_id, '_', '-');
  }
  $vars['template_files'][] = 'gmaps-map-item-base-'. $vars['gmi']->base->mid;
  $vars['template_files'][] = 'gmaps-map-item-'. strtr($vars['gmi']->id, '_', '-');
}

function theme_gmaps_collapsible_element($element, $class = NULL) {
  if ($element['#collapsible'] && !empty($element['#title'])) {
    $title = $element['#title'];
    $desc = isset($element['#description']) ? $element['#description'] : NULL;
    unset($element['#title'], $element['#description']);
  }
  $output = theme('form_element', $element, '<div id="'. $element['#id'] .'"'. drupal_attributes($element['#attributes']) .'>'. $element['#children'] .'</div>');
  if ($title) {
    $fieldset = array('#type' => 'fieldset',
      '#title' => $title,
      '#collapsible' => TRUE,
      '#collapsed' => $element['#collapsed'],
      '#children' => $output,
      '#attributes' => array('class' => 'gmaps-collapsible-element'. ($class ? ' '. $class : '')),
    );
    if (!is_null($desc)) {
      $fieldset['#description'] = $desc;
    }
    $output = drupal_render($fieldset);
  }
  return $output;
}

function template_preprocess_gmaps_static_map_item(&$vars) {
  $content = &$vars['smi']->content;
  if (!empty($content)) {
    $style = $vars['smi']->base->content;
    $output = '';
    $panels = array();
    
    $i = 1;
    foreach($content as $id => $item) {
      if (!empty($item['content'])) {
        $anchor = '<a id="'. $vars['smi']->id .'-'. $id .'"></a>';
        $item['title'] = empty($item['title']) ? t('Item !n', array('!n' => $i)) : $item['title'];
        $item['content'] = gmaps_overlay_render_content($item['content'], $vars['smi']->base->content['content']);

        if ($item['icon'] && $style['icon'] && $style['style'] != 'list') {
          if ($style['icon'] == 'before') {
            $item['title'] = $item['icon'] .' - '. $item['title'];
          }
          else if ($style['icon'] == 'after') {
            $item['title'] .= ' - '. $item['icon'];
          }
          else {
            $item['title'] = $item['icon'];
          }
          $item['title'] = '<span>'. $item['title'] .'</span>';
        }
        
        if ($style['style'] == 'plain') {
          $output .= $anchor . $item['content'];
        }
        else if ($style['style'] == 'list' || $style['style'] == 'grid') {
          $panels[] = array(
            'data' => $anchor . $item['content'],
            'title' => filter_xss($item['title'], array()),
            'alt' => filter_xss($item['title'], array()),
          );
        }
        else if ($style['style'] == 'box') {
          $output .= $anchor . theme('box', $item['title'], $item['content']);
        }
        else if ($style['style'] == 'fieldset') {
          $panel = array('#type' => 'fieldset',
            '#prefix' => $anchor,
            '#title' => $item['title'],
            '#value' => $item['content'],
            '#collapsible' => $style['collapsible'],
            '#collapsed' => $style['collapsed'],
          );
          $output .= drupal_render($panel);
        }
        else if ($style['style'] == 'accordion') {
          $panel = array('#type' => 'gmaps_accordion_panel',
            '#id' => $id,
            '#prefix' => $anchor,
            '#title' => $item['title'],
            '#value' => $item['content'],
          );
          if (!empty($item['ahah'])) {
            $panel['#ahah'] = $item['ahah'];
          }
          $panels[] = $panel;
        }
        else if ($style['style'] == 'tabs') {
          $panel = array('#type' => 'gmaps_tabs_panel',
            '#prefix' => $anchor,
            '#title' => $item['title'],
            '#value' => $item['content'],
          );
          if (!empty($item['ahah'])) {
            $panel['#ahah'] = $item['ahah'];
          }
          $panels[] = $panel;
        }
        
        $i++;
      }
    }
    
    if (!empty($panels)) {
      if ($style['style'] == 'accordion') {
        $cont = array('#type' => 'gmaps_accordion',
          '#collapsible' => $style['collapsible'],
          '#collapsed' => $style['collapsed'],
        );
      }
      else if ($style['style'] == 'tabs') {
        $cont = array('#type' => 'gmaps_tabs',
          '#collapsible' => $style['collapsible'],
          '#collapsed' => $style['collapsed'],
          '#nav_position' => $style['tabs_nav'],
        );
      }
      else if ($style['style'] == 'list') {
        $output .= theme('item_list', $panels);
      }
      else if ($style['style'] == 'grid') {
        $output .= theme('gmaps_grid', $panels, $style['grid']);
      }
      
      if ($cont) {
        $cont['#id'] = $vars['smi']->id;
        
        $cont['#ahah'] = array();
        if ($style['ahah']['progress']) {
          $cont['#ahah']['progress']['type'] = $style['ahah']['progress'];
        }
        if ($style['ahah']['message']) {
          $cont['#ahah']['progress']['message'] = t($style['ahah']['message']);
        }
        if ($style['ahah']['effect']) {
          $cont['#ahah']['effect'] = $style['ahah']['effect'];
        }
        if (isset($vars['smi']->content_ahah)) {
          $cont['#ahah'] = gmaps_extend($cont['#ahah'], $vars['smi']->content_ahah);
        }
        if (empty($cont['#ahah'])) {
          unset($cont['#ahah']);
        }
        
        $output .= drupal_render(array_merge($cont, $panels));
      }
    }
    
    $vars['content'] = $output;
  }
  
  $class = 'gmaps-static-map-item';
  $vars['smi']->attributes['class'] = $class .' '. $vars['smi']->attributes['class'];
  
  if (isset($vars['smi']->attributes['title'])) {
    $vars['title'] = $vars['smi']->attributes['title'];
    unset($vars['smi']->attributes['title']);
  }
  if (isset($vars['smi']->attributes['alt'])) {
    $vars['alt'] = $vars['smi']->attributes['alt'];
    unset($vars['smi']->attributes['alt']);
  }
  
  $vars['template_files'][] = 'gmaps-static-map-item';
  if ($vars['smi']->base->locked) {
    $vars['template_files'][] = 'gmaps-static-map-item-default-'. strtr($vars['smi']->base->default_id, '_', '-');
  }
  $vars['template_files'][] = 'gmaps-static-map-item-base-'. $vars['smi']->base->mid;
  $vars['template_files'][] = 'gmaps-static-map-item-'. strtr($vars['smi']->id, '_', '-');
}

function theme_gmaps_static_map_marker_icon($smi, $marker, $size, $color, $label = NULL) {
  if ($label) {
    $t_args = array(
      '%size' => $size,
      '%color' => $color,
      '%label' => $label,
    );
    return t('%size %color labeled %label', $t_args);
  }
  else {
    $t_args = array(
      '%size' => $size,
      '%color' => $color,
    );
    return t('%size %color', $t_args);
  }
}

function theme_gmaps_grid($items, $options, $title = NULL) {
  if (empty($items)) {
    return '';
  }
  
  $cols = $options['columns'];
  $attributes = isset($options['attributes']) ? $options['attributes'] : array();
  $header = isset($options['header']) ? $options['header'] : FALSE;
  
  $attributes['class'] = empty($attributes['class']) ? 'gmaps-grid' : ('gmaps-grid '. $attributes['class']);
  
  $output = '<table'. drupal_attributes($attributes) .">\n";
  
  if (isset($title)) {
    $output .= '<caption>'. $title ."</caption>\n";
  }
  $output .= "<tbody>\n";
  
  $rows = array();

  //set up grid
  if ($options['alignment'] == GMAPS_GRID_ALIGN_HOR) {
    $row = array();
    $row_count = 0;
    foreach ($items as $count => $cell) {
      $row[] = $cell;
      $row_count++;
      if (($count + 1) % $cols == 0) {
        $rows[] = $row;
        $row = array();
        $row_count = 0;
      }
    }
    if ($row) {
      // Fill up the last line.
      for ($i = 0; $i < ($cols - $row_count); $i++) {
        $row[] = '';
      }
      $rows[] = $row;
    }
  }
  else {
    $num_rows = floor(count($items) / $cols);
    // The remainders are the 'odd' columns that are slightly longer.
    $remainders = count($items) % $cols;
    $row = 0;
    $col = 0;
    foreach ($items as $count => $cell) {
      $rows[$row][$col] = $cell;
      $row++;

      if (!$remainders && $row == $num_rows) {
        $row = 0;
        $col++;
      }
      else if ($remainders && $row == $num_rows + 1) {
        $row = 0;
        $col++;
        $remainders--;
      }
    }
    for ($i = 0; $i < count($rows[0]); $i++) {
      // This should be string so that's okay :)
      if (!isset($rows[count($rows) - 1][$i])) {
        $rows[count($rows) - 1][$i] = '';
      }
    }
  }
  
  $flip = array('even' => 'odd', 'odd' => 'even');
  $class = 'even';
  foreach ($rows as $row_number => $row) {
    $row_class = 'row-' . ($row_number + 1) .' row-'. $class;
    $class = $flip[$class];
    if ($row_number == 0) {
      $row_class .= ' row-first';
    }
    if (count($rows) == ($row_number + 1)) {
      $row_class .= ' row-last';
    }
    $output .= '<tr class="'. $row_class .'">';
    
    $flip_col = array('even' => 'odd', 'odd' => 'even');
    $class_col = 'even';
    foreach ($row as $col_number => $cell) {
      $cell_class = 'col-' . ($col_number + 1) .' col-'. $class_col;
      $class_col = $flip_col[$class_col];
      if ($col_number == 0) {
        $cell_class .= ' col-first';
      }
      if (count($row) == ($col_number + 1)) {
        $cell_class .= ' col-last';
      }
      
      if (is_array($cell)) {
        if (isset($cell['class'])) {
          $cell['class'] .= ' '. $cell_class;
        }
        else {
          $cell['class'] = $cell_class;
        }
      }
      else {
        $cell = array('data' => $cell, 'class' => $cell_class);
      }
      
      $output .= _theme_table_cell($cell, $header);
    }
    
    $output .= "</tr>\n";
  }
  
  $output .= "</tbody></table>\n";
  return $output;
}
