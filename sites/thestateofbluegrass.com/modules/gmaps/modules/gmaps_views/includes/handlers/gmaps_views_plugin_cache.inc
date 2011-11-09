<?php
// $Id: gmaps_views_plugin_cache.inc,v 1.1.2.1 2009/12/03 12:29:46 xmarket Exp $

/**
 * Cache plugin to use with GMapas Map and GMaps static map style plugins.
 */
class gmaps_views_plugin_cache extends views_plugin_cache implements gmaps_views_cache_map, gmaps_views_cache_static_map {
  function option_defaults(&$options) {
    $options['results'] = array(
      'enabled' => 1,
      'expires' => -1,
      'user_mask' => GMAPS_CACHE_PER_ROLE,
    );
    $options['output'] = array(
      'enabled' => 1,
      'expires' => -1,
      'user_mask' => GMAPS_CACHE_PER_ROLE,
    );
  }
  
  function options_form(&$form, &$form_state) {

    $form['results'] = array('#type' => 'fieldset',
      '#title' => t('Query results'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );
    $form['results'] += $this->_options_form($this->options['results']);
    
    $form['output'] = array('#type' => 'fieldset',
      '#title' => t('Rendered output'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );
    $form['output'] += $this->_options_form($this->options['output']);
  }

  function _options_form($options) {
    $user_masks = array(
      GMAPS_CACHE_NO_USER => t('Skip user'),
      GMAPS_CACHE_PER_USER => t('Per user'),
      GMAPS_CACHE_PER_ROLE => t('Per role'),
    );
    
    $form = array();
    
    $form['enabled'] = array('#type' => 'checkbox',
      '#title' => t('Enabled'),
      '#default_value' => $options['enabled'],
    );
    $form['expires'] = array('#type' => 'textfield',
      '#title' => t('Expires'),
      '#default_value' => $options['expires'],
      '#description' => t('Relative seconds from the time of map rendering. For example 21600 for six hours. Leave it empty or set to <strong>0 (zero) for PERMANENT caching</strong>, or to <strong>-1 for TEMPORARY caching</strong>.'),
    );
    $form['user_mask'] = array('#type' => 'select',
      '#title' => t('User in cache ID'),
      '#default_value' => $options['user_mask'],
      '#options' => $user_masks,
    );
    
    return $form;
  }

  function summary_title() {
    $title = array(
      $this->_summary_title($this->options['results']),
      $this->_summary_title($this->options['output']),
    );
    
    return implode('/', $title);
  }

  function _summary_title($options) {
    $title = t('None');
    
    if ($options['enabled']) {
      if (empty($options['expires'])) {
        $title = t('Permanent');
      }
      else if ($options['expires'] < 0) {
        $title = t('Temporary');
      }
      else {
        $title = format_interval($options['expires'], 1);
      }
    }
    
    return $title;
  }

  function output_is_cachable() {
    $map_interface = 'gmaps_views_map';
    $smap_interface = 'gmaps_views_static_map';
    if (($this->view->style_plugin instanceof $map_interface) || ($this->view->style_plugin instanceof $smap_interface)) {
      return FALSE;
    }
    return TRUE;
  }
  
  function cache_table($type) {
    if ($this->definition['static maps api']) {
      return $type == 'results' ? 'cache_gv_smap_result' : 'cache_gv_smap';
    }
    else {
      return $type == 'results' ? 'cache_gv_map_result' : 'cache_gv_map';
    }
  }
  
  function cache_start() {
    $this->output_is_cachable();
    if (!gmaps_agent_is_search_engine() && $this->output_is_cachable()) {
      parent::cache_start();
    }
  }

  function cache_expire($type) {
    if ($this->options[$type]['enabled']) {
      $expires = $this->options[$type]['expires'];
      
      if (empty($expires)) {
        return CACHE_PERMANENT;
      }
      else if ($expires < 0) {
        return CACHE_TEMPORARY;
      }
      else {
        return time() + $expires;
      }
    }
    else {
      return FALSE;
    }
  }
  
  function cache_set($type) {
    if (!$this->options[$type]['enabled']) {
      return;
    }
    if (gmaps_agent_is_search_engine()) {
      return;
    }
    switch ($type) {
      case 'query':
        // Not supported currently, but this is certainly where we'd put it.
        break;
      case 'results':
        if ($cid = $this->get_results_key()) {
          $data = array(
            'result' => $this->view->result,
            'total_rows' => $this->view->total_rows,
            'pager' => $this->view->pager,
          );
          cache_set($cid, $data, $this->cache_table($type), $this->cache_expire($type));
        }
        break;
      case 'output':
        if ($this->output_is_cachable() && ($cid = $this->get_output_key())) {
          $this->gather_headers();
          $this->storage['output'] = $this->view->display_handler->output;
          
          $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
          cache_set($cid, $this->storage, $this->cache_table($type), $this->cache_expire($type));
        }
        break;
    }
  }

  function cache_get($type) {
    if (!$this->options[$type]['enabled']) {
      return FALSE;
    }
    switch ($type) {
      case 'query':
        // Not supported currently, but this is certainly where we'd put it.
        return FALSE;
      case 'results':
        // Values to set: $view->result, $view->total_rows, $view->execute_time,
        // $view->pager['current_page'].
        if (($cid = $this->get_results_key()) && ($cache = cache_get($cid, $this->cache_table($type)))) {
          $this->view->result = $cache->data['result'];
          $this->view->total_rows = $cache->data['total_rows'];
          $this->view->pager = $cache->data['pager'];
          $this->view->execute_time = 0;
          return TRUE;
        }
        return FALSE;
      case 'output':
        if ($this->output_is_cachable() && ($cid = $this->get_output_key()) && ($cache = cache_get($cid, $this->cache_table($type)))) {
          $this->storage = $cache->data;
          $this->view->display_handler->output = $cache->data['output'];
          $this->restore_headers();
          return TRUE;
        }
        return FALSE;
    }
  }

  function get_results_key() {
    global $user;

    if (!isset($this->_results_key)) {
      if ($user->uid == 1) {
        $this->_results_key = FALSE;
      }
      else {
        $key_data = array(
          'build_info' => $this->view->build_info,
          'roles' => array_keys($user->roles),
        );
        foreach (array('exposed_info', 'page', 'sort', 'order') as $key) {
          if (isset($_GET[$key])) {
            $key_data[$key] = $_GET[$key];
          }
        }
  
        $this->_results_key = $this->view->name . ':' . $this->display->id . ':' . md5(serialize($key_data));
      }
    }

    return $this->_results_key;
  }

  function get_output_key() {
    global $user;
    if (!isset($this->_output_key)) {
      if ($user->uid == 1) {
        $this->_output_key = FALSE;
      }
      else {
        $key_data = array(
          'result' => $this->view->result,
          'roles' => array_keys($user->roles),
        );
  
        $this->_output_key = $this->view->name . ':' . $this->display->id . ':' . md5(serialize($key_data));
      }
    }

    return $this->_output_key;
  }

  function cache_flush() {
    gmaps_map_cache_clear_all($this->view->name . ':', 'cache_gv_map', TRUE);
    gmaps_static_map_cache_clear_all($this->view->name . ':', 'cache_gv_smap', TRUE);
    cache_clear_all($this->view->name . ':', 'cache_gv_map_result', TRUE);
    cache_clear_all($this->view->name . ':', 'cache_gv_smap_result', TRUE);
  }
  
  function get_map_cache_options() {
    if (!$this->definition['static maps api'] && $this->options['output']['enabled']) {
      return array(
        'cid' => $this->get_output_key(),
        'expires' => $this->options['output']['expires'],
        'table' => $this->cache_table('output'),
        'user_mask' => $this->options['output']['user_mask'],
      );
    }
    return array();
  }
  
  function get_static_map_cache_options() {
    if ($this->definition['static maps api'] && $this->options['output']['enabled']) {
      return array(
        'cid' => $this->get_output_key(),
        'expires' => $this->options['output']['expires'],
        'table' => $this->cache_table('output'),
        'user_mask' => $this->options['output']['user_mask'],
      );
    }
    return array();
  }
}
