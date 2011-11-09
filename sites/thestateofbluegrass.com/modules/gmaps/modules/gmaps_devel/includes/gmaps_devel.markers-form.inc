<?php
// $Id: gmaps_devel.markers-form.inc,v 1.1.2.6 2009/11/23 13:23:48 xmarket Exp $

/**
 * @file
 * Multiple markers test form.
 */

function gmaps_devel_markers_form(&$form_state = NULL) {
  $icons = gmaps_get_icons();
  
  $values = $form_state['values'] ? $form_state['values'] : array(
    'base' => gmaps_map_get_mid_for_default('input_earth'),
    'count' => 5,
    'route' => 0,
    'privacy' => 0,
    'icons' => array(),
    'content' => 0,
    'info_window' => 0,
    'manager' => 0,
    'manage_all' => 0,
    'se_agent' => 0,
    'throttle' => 0,
    'cache' => array(
      'expires' => 30,
    ),
  );

  $form = array(
    '#gmaps_values' => $values,
  );
  
  $form['options'] = array('#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['options']['base'] = array('#type' => 'select',
    '#title' => t('Base map'),
    '#default_value' => $values['base'],
    '#options' => gmaps_map_get_options(),
  );
  
  $form['options']['count'] = array('#type' => 'textfield',
    '#title' => t('Marker count'),
    '#default_value' => $values['count'],
  );
  $form['options']['route'] = array('#type' => 'checkbox',
    '#title' => t('Generate points in route'),
    '#default_value' => $values['route'],
  );
  $form['options']['privacy'] = array('#type' => 'checkbox',
    '#title' => t('Generate privacy'),
    '#default_value' => $values['privacy'],
  );
  $form['options']['icons'] = array('#type' => 'gmaps_icon_select',
    '#title' => t('Icons'),
    '#default_value' => $values['icons'],
    '#icons' => $icons,
    '#multiple' => TRUE,
    '#size' => 5,
  );
  $form['options']['content'] = array('#type' => 'checkbox',
    '#title' => t('Generate content'),
    '#default_value' => $values['content'],
  );
  $form['options']['info_window'] = array('#type' => 'checkbox',
    '#title' => t('Generate info windows'),
    '#default_value' => $values['info_window'],
  );
  $form['options']['manager'] = array('#type' => 'checkbox',
    '#title' => t('Generate managing data'),
    '#default_value' => $values['manager'],
  );
  $form['options']['manage_all'] = array('#type' => 'checkbox',
    '#title' => t('Manage all markers'),
    '#default_value' => $values['manage_all'],
  );
  $form['options']['se_agent'] = array('#type' => 'checkbox',
    '#title' => t('Set search engine'),
    '#default_value' => $values['se_agent'],
  );
  $form['options']['throttle'] = array('#type' => 'checkbox',
    '#title' => t('Set throttle'),
    '#default_value' => $values['throttle'],
  );
  $form['options']['cache'] = array('#tree' => TRUE);
  $form['options']['cache']['cid'] = array('#type' => 'textfield',
    '#title' => t('Cache ID'),
    '#default_value' => $values['cache']['cid'],
  );
  $form['options']['cache']['expires'] = array('#type' => 'textfield',
    '#title' => t('Cache expires'),
    '#default_value' => $values['cache']['expires'],
  );
  $form['options']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  $form['#submit'][] = '_gmaps_devel_markers_form_submit';
  $form['#pre_render'][] = '_gmaps_devel_markers_form_pre_render';
  
  return $form;
}

function _gmaps_devel_markers_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function _gmaps_devel_markers_form_pre_render($form) {
  global $gmaps_throttle;
  $values = $form['#gmaps_values'];
  
  if ($values['throttle']) {
    $throttle = $gmaps_throttle;
    $gmaps_throttle = 1;
  }
  
  if ($values['se_agent']) {
    $ua = $_SERVER['HTTP_USER_AGENT'];
    $_SERVER['HTTP_USER_AGENT'] = 'Mediapartners-Google/2.1 ( http://www.googlebot.com/bot.html)';
  }
  
  $cache = $values['cache']['cid'] ? $values['cache'] : array();
  $form['#suffix'] = gmaps_map_item_render($values['base'], 'gmaps_devel_markers_form_gmi', array($values), $cache);
  
  if (isset($throttle)) {
    $gmaps_throttle = $throttle;
  }
  
  if (isset($ua)) {
    $_SERVER['HTTP_USER_AGENT'] = $ua;
  }
  
  return $form;
}

function gmaps_devel_markers_form_gmi(&$gmi, $values) {
  if (empty($values['count']) || intval($values['count']) != $values['count'] || $values['count'] < 0) {
    return;
  }
  
  $info = gmaps_map_get_info($gmi->base->type);
  $info_windows = array_keys($info->info_windows_names);
  $iwc = count($info_windows);
  $ic = count($values['icons']);
  if ($ic) {
    $icons = array_keys($values['icons']);
  }
  $zoom_levels = array_keys(gmaps_map_get_zoom_levels($info));
  $zlc = count($zoom_levels);
  $gmi->overlays['marker'] = array();
  for ($i = 0; $i < $values['count']; $i++) {
    $marker = array(
      'title' => t('Marker !n', array('!n' => $i+1)),
      'point' => _gmaps_devel_create_marker_point($last_point, $gmi->base->type),
      'basic' => array('icon' => $ic ? ($ic == 1 ? $icons[0] : $icons[mt_rand(0, $ic-1)]) : 0),
      'options' => array('draggable' => 0),
    );
    if ($values['route']) {
      $last_point = drupal_clone($marker['point']);
    }
    if ($values['privacy'] && mt_rand(0,1)) {
      $marker['point']->protected = TRUE;
    }
    
    if ($values['content']) {
      if (!in_array('content', $gmi->base->ov_data['marker']['action'])) {
        $marker['action']['click'] = 'content';
      }
      if ($gmi->base->ov_data['marker']['misc']['ajaxcontent']) {
        if ($gmi->base->map_data['toc_map']['enabled']) {
          $gmi->toc_ahah = array(
            'path' => 'devel/gmaps/maps/marker-content',
          );
          $marker['content'] = array('ajax' => TRUE);
        }
        else {
          $marker['content']['ajax'] = array('path' => 'devel/gmaps/maps/marker-content');
        }
      }
      else {
        $c = _gmaps_devel_create_marker_content();
        if (!empty($c)) {
          $marker['content'] = $c;
        }
      }
      if (!empty($marker['content'])) {
        if ($values['info_window']) {
          $marker['basic']['info_window'] = $info_windows[mt_rand(0, $iwc-1)];
        }
        $marker['sticky'] = mt_rand(0,1);
      }
    }
    
    if ($values['manager']) {
      $marker['manager'] = array(
        'skip_route' => mt_rand(0,1),
        'skip_manager' => $values['manage_all'] ? 0 : mt_rand(0,1),
      );
      if (!empty($gmi->base->ov_data['marker']['manager']['manager']) && $gmi->base->ov_data['marker']['manager']['manager'] != 'clusterer') {
        $min = mt_rand(0, $zlc-1);
        $marker['manager']['minzoom'] = $zoom_levels[$min];
        $marker['manager']['maxzoom'] = mt_rand(0, 1) ? -1 : $zoom_levels[mt_rand($min, $zlc-1)];
      } 
    }
    
    $gmi->overlays['marker']['m'. $i] = $marker;
  }
}
