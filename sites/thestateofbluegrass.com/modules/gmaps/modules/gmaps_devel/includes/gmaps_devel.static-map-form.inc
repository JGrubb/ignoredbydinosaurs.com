<?php
// $Id: gmaps_devel.static-map-form.inc,v 1.1.2.3 2009/09/14 07:57:58 xmarket Exp $

/**
 * @file
 * Static map test form.
 */

function gmaps_devel_static_map_form(&$form_state = NULL) {
  $icons = gmaps_get_icons();
  
  $values = $form_state['values'] ? $form_state['values'] : array(
    'base' => 0,
    'count' => 1,
    'path_count' => 0,
    'route' => 0,
    'privacy' => 0,
    'style' => 0,
    'content' => 0,
    'manager' => 0,
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
    '#options' => gmaps_static_map_get_options(),
  );
  
  $form['options']['count'] = array('#type' => 'textfield',
    '#title' => t('Marker count'),
    '#default_value' => $values['count'],
  );
  $form['options']['path_count'] = array('#type' => 'textfield',
    '#title' => t('Path count'),
    '#default_value' => $values['path_count'],
  );
  $form['options']['route'] = array('#type' => 'checkbox',
    '#title' => t('Generate points in route'),
    '#default_value' => $values['route'],
  );
  $form['options']['privacy'] = array('#type' => 'checkbox',
    '#title' => t('Generate privacy'),
    '#default_value' => $values['privacy'],
  );
  $form['options']['style'] = array('#type' => 'checkbox',
    '#title' => t('Generate styles'),
    '#default_value' => $values['style'],
  );
    $form['options']['content'] = array('#type' => 'checkbox',
    '#title' => t('Generate content'),
    '#default_value' => $values['content'],
  );
  $form['options']['manager'] = array('#type' => 'checkbox',
    '#title' => t('Generate managing data'),
    '#default_value' => $values['manager'],
  );
  $form['options']['throttle'] = array('#type' => 'checkbox',
    '#title' => t('Set throttle'),
    '#default_value' => $values['throttle'],
  );
  $form['options']['se_agent'] = array('#type' => 'checkbox',
    '#title' => t('Set search engine'),
    '#default_value' => $values['se_agent'],
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

  $form['#submit'][] = '_gmaps_devel_static_map_form_submit';
  $form['#pre_render'][] = '_gmaps_devel_static_map_form_pre_render';
  
  return $form;
}

function _gmaps_devel_static_map_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function _gmaps_devel_static_map_form_pre_render($form) {
  global $gmaps_throttle;
  $values = $form['#gmaps_values'];
  
  if ($values['base']) {
    if ($values['throttle']) {
      $throttle = $gmaps_throttle;
      $gmaps_throttle = 1;
    }
    
    if ($values['se_agent']) {
      $ua = $_SERVER['HTTP_USER_AGENT'];
      $_SERVER['HTTP_USER_AGENT'] = 'Mediapartners-Google/2.1 ( http://www.googlebot.com/bot.html)';
    }
    
    $cache = $values['cache']['cid'] ? $values['cache'] : array();
    $form['#suffix'] = gmaps_static_map_item_render($values['base'], 'gmaps_devel_static_map_form_smi', array($values), $cache);
    
    if (isset($throttle)) {
      $gmaps_throttle = $throttle;
    }
    
    if (isset($ua)) {
      $_SERVER['HTTP_USER_AGENT'] = $ua;
    }
  }
  
  return $form;
}

function gmaps_devel_static_map_form_smi(&$smi, $values) {
  if (empty($values['count']) || intval($values['count']) != $values['count'] || $values['count'] < 0) {
    return;
  }
  
  $sizes = array_keys(_gmaps_static_marker_sizes());
  $sc = count($sizes);
  $colors = array_keys(_gmaps_static_marker_colors());
  $cc = count($colors);
  $labels = array_map('chr', range(97, 122));
  $lc = count($labels);
  
  $smi_clone = gmaps_extend(array(), $smi);
  unset($smi_clone->overlays, $smi_clone->content, $smi_clone->attributes, $smi_clone->after_process);
  
  $smi->overlays['marker'] = array();
  for ($i = 0; $i < $values['count']; $i++) {
    $marker = array(
      'title' => t('Marker !n', array('!n' => $i+1)),
      'point' => _gmaps_devel_create_marker_point($last_point, GMAPS_MAP_EARTH),
      'label' => mt_rand(0,1) ? $labels[mt_rand(0, $lc-1)] : NULL,
    );
    if ($values['route']) {
      $last_point = (object)gmaps_extend(array(), $marker['point']);
    }
    if ($values['privacy'] && mt_rand(0,1)) {
      $marker['point']->privacy = array('value' => mt_rand(500, 10000), 'unit' => GMAPS_DEFAULT_LENGTH_UNIT);
      $marker['point']->protected = TRUE;
    }
    
    if ($values['style']) {
      $marker['style'] = array(
        'size' => $sizes[mt_rand(0, $sc-1)],
        'color' => $colors[mt_rand(0, $cc-1)],
      );
      $marker['sticky'] = mt_rand(0,1);
    }
    
    if ($values['content']) {
      if ($smi->base->content['ajax']) {
        $smi->content_ahah = array(
          'path' => 'devel/gmaps/maps/marker-content',
          'data' => array('smi' => $smi_clone),
        );
        $marker_clone = gmaps_extend(array(), $marker);
        $marker_clone['id'] = 'm'. $i;
        $marker['content'] = array('ajax' => array('data' => array('marker' => $marker_clone)));
      }
      else {
        $c = _gmaps_devel_create_marker_content();
        if (!empty($c)) {
          $marker['content'] = $c;
        }
      }
    }
    
    if ($values['manager']) {
      $marker['skip_route'] = mt_rand(0,1);
    }
    
    $smi->overlays['marker']['m'. $i] = $marker;
  }

  if ($values['path_count'] && $values['count'] > 1) {
    $smi->overlays['path'] = array();
    for ($i = 0; $i < $values['path_count']; $i++) {
      $last_point = NULL;
      $points = array();
      for ($j = 0; $j < $values['count']; $j++) {
        $points[] = _gmaps_devel_create_marker_point($last_point, GMAPS_MAP_EARTH);
        $last_point = (object)gmaps_extend(array(), $marker['point']);
      }
      $path = array(
        'title' => t('Path !n', array('!n' => $i+1)),
        'path' => (object)array('points' => $points),
      );
      
      if ($values['style']) {
        $path['sticky'] = mt_rand(0,1);
        $path[$path['sticky'] ? 'style_sticky' : 'style'] = array(
          'color' => str_pad(dechex(mt_rand(0, 255)),2,'0',STR_PAD_LEFT).str_pad(dechex(mt_rand(0, 255)),2,'0',STR_PAD_LEFT).str_pad(dechex(mt_rand(0, 255)),2,'0',STR_PAD_LEFT),
          'opacity' => str_pad(dechex(mt_rand(50, 255)),2,'0',STR_PAD_LEFT),
          'weight' => mt_rand(5, 25),
        );
      }
      
      if ($values['content']) {
        if ($smi->base->content['ajax']) {
          $smi->content_ahah = array(
            'path' => 'devel/gmaps/maps/marker-content',
            'data' => array('smi' => $smi_clone),
          );
          $path_clone = gmaps_extend(array(), $path);
          $path_clone['id'] = 'p'. $i;
          $path['content'] = array('ajax' => array('data' => array('marker' => $path_clone)));
        }
        else {
          $c = _gmaps_devel_create_marker_content();
          if (!empty($c)) {
            $path['content'] = $c;
          }
        }
      }
      
      $smi->overlays['path']['p'. $i] = $path;
    }
  }
}
