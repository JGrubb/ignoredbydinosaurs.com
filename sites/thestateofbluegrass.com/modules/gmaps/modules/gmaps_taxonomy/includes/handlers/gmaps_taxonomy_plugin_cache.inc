<?php
// $Id: gmaps_taxonomy_plugin_cache.inc,v 1.1.2.1 2009/11/23 13:23:48 xmarket Exp $

/**
 * Caching plugin that uses GMaps Taxonomy page cache settings.
 */
class gmaps_taxonomy_plugin_cache extends views_plugin_cache {
  function summary_title() {
    $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
    $title = array();
    
    if ($settings['result']) {
      $title[] = empty($settings['result_expires']) ? t('Temporary') : format_interval($settings['result_expires'], 1);
    }
    else {
      $title[] = t('None');
    }
    
    if ($settings['output']) {
      $title[] = empty($settings['output_expires']) ? t('Temporary') : format_interval($settings['output_expires'], 1);
    }
    else {
      $title[] = t('None');
    }
    
    return implode('/', $title);
  }

  function cache_start() {
    if (!gmaps_agent_is_search_engine()) {
      parent::cache_start();
    }
  }

  function cache_set($type) {
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
          $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
          cache_set($cid, $data, GMAPS_TAXONOMY_CACHE_TABLE_RESULT, empty($settings['result_expires']) ? CACHE_TEMPORARY : (time() + $settings['result_expires']));
        }
        break;
      case 'output':
        if ($cid = $this->get_output_key()) {
          $this->gather_headers();
          $this->storage['output'] = $this->view->display_handler->output;
          
          $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
          cache_set($cid, $this->storage, GMAPS_TAXONOMY_CACHE_TABLE_PAGE, empty($settings['output_expires']) ? CACHE_TEMPORARY : (time() + $settings['output_expires']));
        }
        break;
    }
  }

  function cache_get($type) {
    $cutoff = $this->cache_expire($type);
    switch ($type) {
      case 'query':
        // Not supported currently, but this is certainly where we'd put it.
        return FALSE;
      case 'results':
        // Values to set: $view->result, $view->total_rows, $view->execute_time,
        // $view->pager['current_page'].
        if (($cid = $this->get_results_key()) && ($cache = cache_get($cid, GMAPS_TAXONOMY_CACHE_TABLE_RESULT))) {
          $this->view->result = $cache->data['result'];
          $this->view->total_rows = $cache->data['total_rows'];
          $this->view->pager = $cache->data['pager'];
          $this->view->execute_time = 0;
          return TRUE;
        }
        return FALSE;
      case 'output':
        if (($cid = $this->get_output_key()) && ($cache = cache_get($cid, GMAPS_TAXONOMY_CACHE_TABLE_PAGE))) {
          $this->storage = $cache->data;
          $this->view->display_handler->output = $cache->data['output'];
          $this->restore_headers();
          return TRUE;
        }
        return FALSE;
    }
  }

  function get_results_key() {
    if (!isset($this->_results_key)) {
      $this->_results_key = ($cid = _gmaps_taxonomy_get_page_cache_id(TRUE)) ? $cid : FALSE;
    }

    return $this->_results_key;
  }

  function get_output_key() {
    if ($this->display->display_plugin != 'page') {
      return FALSE;
    }
    if (!isset($this->_output_key)) {
      $this->_output_key = ($cid = _gmaps_taxonomy_get_page_cache_id(FALSE)) ? $cid : FALSE;
    }

    return $this->_output_key;
  }

  function cache_flush() {
    cache_clear_all('*', GMAPS_TAXONOMY_CACHE_TABLE_RESULT, TRUE);
    cache_clear_all('*', GMAPS_TAXONOMY_CACHE_TABLE_PAGE, TRUE);
  }
}
