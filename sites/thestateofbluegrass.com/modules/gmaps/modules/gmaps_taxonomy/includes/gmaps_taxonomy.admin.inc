<?php
// $Id: gmaps_taxonomy.admin.inc,v 1.1.2.7 2009/11/23 13:23:48 xmarket Exp $

/**
 * @file
 * GMaps Taxonomy settings.
 */

/********************
 * Public functions *
 ********************/

/**
 * Admin settings form.
 */
function gmaps_taxonomy_admin_settings() {
  $location_terms = variable_get('gmaps_taxonomy_location_terms', 0);
  $letter_terms = variable_get('gmaps_taxonomy_letter_terms', 0);
  
  $form = array();

  $form['terms'] = array('#type' => 'fieldset',
    '#title' => t('Terms'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['terms']['gmaps_taxonomy_location_terms'] = array('#type' => 'select',
    '#title' => t('Location terms'),
    '#default_value' => variable_get('gmaps_taxonomy_location_terms', 0),
    '#options' => array(
      0 => t('Disabled'),
      GMAPS_TAXONOMY_LOCATION_TERMS_SIMULATED => t('Simulated (GMaps 1.x like)'),
      GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED => t('Integrated (GCG like)'),
    ),
  );
  $vids = variable_get('gmaps_taxonomy_location_vids', array());
  if (empty($vids)) {
    $form['terms']['gmaps_taxonomy_location_vocabulary'] = array('#type' => 'select',
      '#title' => t('Location vocabularies'),
      '#default_value' => variable_get('gmaps_taxonomy_location_vocabulary', GMAPS_TAXONOMY_LOCATION_VOCABULARY_GLOBAL),
      '#options' => array(
        0 => t('One global'),
        1 => t('Per continent'),
        2 => t('Per country'),
      ),
    );
  }
  else {
    $options = array(
      0 => t('One global'),
      1 => t('Per continent'),
      2 => t('Per country'),
    );
    $form['terms']['gmaps_taxonomy_location_vocabulary'] = array('#type' => 'item',
      '#title' => t('Location vocabularies'),
      '#value' => $options[variable_get('gmaps_taxonomy_location_vocabulary', GMAPS_TAXONOMY_LOCATION_VOCABULARY_GLOBAL)],
    );
  }
  $form['terms']['gmaps_taxonomy_location_continent_term'] = array('#type' => 'checkbox',
    '#title' => t('Add continent terms to global vocabulary'),
    '#default_value' => variable_get('gmaps_taxonomy_location_continent_term', 1),
  );
  $view_opts = array(
    'raw' => t('Raw'),
    'plain' => t('Plain text'),
    'themed' => t('Themed'),
    'render' => t('Render'),
  );
  $form['terms']['gmaps_taxonomy_location_term_view'] = array('#type' => 'select',
    '#title' => t('Location term view'),
    '#default_value' => variable_get('gmaps_taxonomy_location_term_view', 'raw'),
    '#options' => $view_opts,
    '#description' => t('If Raw selected, each term will display only the underlying part value.'),
  );
  $form['terms']['gmaps_taxonomy_location_term_view_country'] = array('#type' => 'select',
    '#title' => t('Country term view'),
    '#default_value' => variable_get('gmaps_taxonomy_location_term_view_country', 'raw'),
    '#options' => $view_opts,
    '#description' => t('If Raw selected, only the country name will be displayed.'),
  );
  $form['terms']['gmaps_taxonomy_location_handler'] = array('#type' => 'select',
    '#title' => t('Location listing handler'),
    '#default_value' => variable_get('gmaps_taxonomy_location_handler', 'gmaps'),
    '#options' => array('gmaps' => t('GMaps Taxonomy'), 'taxonomy' => t('Taxonomy')),
  );
  $form['terms']['gmaps_taxonomy_term_page_overwrite'] = array('#type' => 'checkbox',
    '#title' => t('Enable taxonomy/term page overwrite'),
    '#default_value' => variable_get('gmaps_taxonomy_term_page_overwrite', 0),
    '#access' => ($location_terms || $letter_terms),
  );
  if (module_exists('pathauto') && file_exists(drupal_get_path('module', 'pathauto') .'/i18n-ascii.txt')) {
    $form['terms']['gmaps_taxonomy_letter_terms'] = array('#type' => 'checkbox',
      '#title' => t('Enable locality first letter terms'),
      '#default_value' => variable_get('gmaps_taxonomy_letter_terms', 0),
    );
    $form['terms']['gmaps_taxonomy_letter_term_view'] = array('#type' => 'textfield',
      '#title' => t('Letter term view'),
      '#default_value' => variable_get('gmaps_taxonomy_letter_term_view', 'Locality: @letter'),
      '#description' => t('Leave empty to show the letter only. <strong>@letter</strong> will be replaced by the term name.'),
    );
    $form['terms']['gmaps_taxonomy_letter_term_show'] = array('#type' => 'checkbox',
      '#title' => t('Show letter terms'),
      '#default_value' => variable_get('gmaps_taxonomy_letter_term_show', 0),
    );
  }
  else {
    $form['terms']['gmaps_taxonomy_letter_terms'] = array('#type' => 'item',
      '#title' => t('Locality first letter terms'),
      '#value' => t('If you enable the pathauto module and configure its i18n-ascii.txt file properly, you will be able to categorize content by locality first letters.'),
    );
  }
  
  if ($location_terms == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED || $letter_terms) {
    $form['terms']['cron'] = array('#type' => 'fieldset',
      '#title' => t('Cron settings'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#description' => t('You can auto-generate location terms and/or locality first letter terms for existing content during cron run.'),
    );
    if ($location_terms == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED) {
      $form['terms']['cron']['gmaps_taxonomy_location_cron'] = array('#type' => 'textfield',
        '#title' => t('Number of nodes to check for location terms on each run'),
        '#default_value' => variable_get('gmaps_taxonomy_location_cron', 0),
        '#description' => t('Leave empty to disable term genaration.'),
      );
    }
    if ($letter_terms) {
      $form['terms']['cron']['gmaps_taxonomy_letter_cron'] = array('#type' => 'textfield',
        '#title' => t('Number of nodes to check for letter terms on each run'),
        '#default_value' => variable_get('gmaps_taxonomy_letter_cron', 0),
        '#description' => t('Leave empty to disable term genaration.'),
      );
    }
    if ($location_terms == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED && variable_get('gmaps_taxonomy_location_cron', 0)) {
      $form['terms']['cron']['gmaps_taxonomy_location_cron_reset'] = array('#type' => 'submit',
        '#value' => t('Reset location term index'),
        '#submit' => array('_gmaps_taxonomy_location_cron_submit_reset'),
      );
    }
    if ($letter_terms && variable_get('gmaps_taxonomy_letter_cron', 0)) {
      $form['terms']['cron']['gmaps_taxonomy_letter_cron_reset'] = array('#type' => 'submit',
        '#value' => t('Reset letter term index'),
        '#submit' => array('_gmaps_taxonomy_letter_cron_submit_reset'),
      );
    }
  }
  
  $user_masks = array(
    GMAPS_CACHE_NO_USER => t('Skip user'),
    GMAPS_CACHE_PER_USER => t('Per user'),
    GMAPS_CACHE_PER_ROLE => t('Per role'),
  );
  
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_links_cache');
  $form['gmaps_taxonomy_links_cache'] = array('#type' => 'fieldset',
    '#title' => t('Links cache'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['gmaps_taxonomy_links_cache']['enabled'] = array('#type' => 'checkbox',
    '#title' => t('Enable link cache'),
    '#default_value' => $settings['enabled'],
  );
  $form['gmaps_taxonomy_links_cache']['expires'] = array('#type' => 'textfield',
    '#title' => t('Link cache expires'),
    '#default_value' => $settings['expires'],
    '#description' => t("Relative seconds from the time of the last link generation. For example, 21600 for 6 hours. Leave it empty or set to <strong>0 (zero) for TEMPORARY caching.</strong>"),
  );
  $form['gmaps_taxonomy_links_cache']['user_mask'] = array('#type' => 'select',
    '#title' => t('User in link cache ID'),
    '#default_value' => $settings['user_mask'],
    '#options' => $user_masks,
    '#description' => t('If there are privacy enabled fields providing location terms, you must select Per user, otherwise you can set it to Skip user..'),
  );
  
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page');
  $form['gmaps_taxonomy_page'] = array('#type' => 'fieldset',
    '#title' => t('Location page'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['gmaps_taxonomy_page']['page'] = array('#type' => 'textfield',
    '#title' => t('Page length'),
    '#default_value' => $settings['page'],
    '#required' => TRUE,
  );
  $form['gmaps_taxonomy_page']['feed'] = array('#type' => 'textfield',
    '#title' => t('Feed length'),
    '#default_value' => $settings['feed'],
    '#required' => TRUE,
  );
  $form['gmaps_taxonomy_page']['order'] = array('#type' => 'textfield',
    '#title' => t('SQL ORDER BY clause'),
    '#default_value' => $settings['order'],
    '#description' => t("Leave empty for default 'n.sticky DESC, n.created DESC'. Available aliases are n (node) and gca (gmaps_content_address)."),
  );
  $form['gmaps_taxonomy_page']['strict'] = array('#type' => 'checkbox',
    '#title' => t('Use strict filter'),
    '#default_value' => $settings['strict'],
    '#description' => t('If selected, empty argument parts will match only empty parts, rather than everything.'),
  );
  
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_page_cache');
  $form['gmaps_taxonomy_page_cache'] = array('#type' => 'fieldset',
    '#title' => t('Location page cache'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );
  $form['gmaps_taxonomy_page_cache']['result'] = array('#type' => 'checkbox',
    '#title' => t('Enable result cache'),
    '#default_value' => $settings['result'],
  );
  $form['gmaps_taxonomy_page_cache']['result_expires'] = array('#type' => 'textfield',
    '#title' => t('Result cache expires'),
    '#default_value' => $settings['result_expires'],
    '#description' => t("Relative seconds from the time of database query. For example, 21600 for 6 hours. Leave it empty or set to <strong>0 (zero) for TEMPORARY caching.</strong>"),
  );
  $form['gmaps_taxonomy_page_cache']['result_user_mask'] = array('#type' => 'select',
    '#title' => t('User in result cache ID'),
    '#default_value' => $settings['result_user_mask'],
    '#options' => $user_masks,
    '#description' => t('If there is only one registered user, the admin one, then you can select Skip user, if there are any node access related modules installed, you must select Per user.'),
  );
  $form['gmaps_taxonomy_page_cache']['output'] = array('#type' => 'checkbox',
    '#title' => t('Enable output cache'),
    '#default_value' => $settings['output'],
  );
  $form['gmaps_taxonomy_page_cache']['output_expires'] = array('#type' => 'textfield',
    '#title' => t('Output cache expires'),
    '#default_value' => $settings['output_expires'],
    '#description' => t("Relative seconds from the time of page rendering. For example, 21600 for 6 hours. Leave it empty or set to <strong>0 (zero) for TEMPORARY caching.</strong>"),
  );
  $form['gmaps_taxonomy_page_cache']['output_user_mask'] = array('#type' => 'select',
    '#title' => t('User in output cache ID'),
    '#default_value' => $settings['output_user_mask'],
    '#options' => $user_masks,
  );
  $form['gmaps_taxonomy_page_cache']['cids'] = array('#type' => 'fieldset',
    '#title' => t('Optional cache ID parts for output cache'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('If there are any node access related modules installed or any of the following options is selected, that is highly recommended to set the output expiration time to very small, or completely disable the output cache.'),
  );
  $form['gmaps_taxonomy_page_cache']['cids']['geoip'] = array('#type' => 'checkbox',
    '#title' => t('Add geoip to cache ID'),
    '#default_value' => $settings['cids']['geoip'],
    '#access' => module_exists('geoip'),
  );
  $form['gmaps_taxonomy_page_cache']['cids']['refloc'] = array('#type' => 'checkbox',
    '#title' => t('Add reference location to cache ID'),
    '#default_value' => $settings['cids']['refloc'],
  );
  $form['gmaps_taxonomy_page_cache']['node_submit_clear'] = array('#type' => 'checkbox',
    '#title' => t('Clear caches on node submission'),
    '#default_value' => $settings['node_submit_clear'],
  );
  
  $settings = gmaps_taxonomy_get_settings('gmaps_taxonomy_menu');
  $form['gmaps_taxonomy_menu'] = array('#type' => 'fieldset',
    '#title' => t('Location navigation menu'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#description' => t('This feature requires cron support.'),
  );
  $form['gmaps_taxonomy_menu']['enabled'] = array('#type' => 'checkbox',
    '#title' => t('Enable location navigation menu'),
    '#default_value' => $settings['enabled'],
  );
  
  $levels = array('continent' => 'Continent') + _gmaps_address_get_parts();
  $levels = array_map('t', $levels);
  $form['gmaps_taxonomy_menu']['levels'] = array('#type' => 'checkboxes',
    '#title' => t('Enabled levels'),
    '#default_value' => $settings['levels'],
    '#options' => $levels,
    '#description' => t('If none selected all levels, except thoroughfare will be included.'),
  );
  
  $form['gmaps_taxonomy_menu']['countries'] = array('#type' => 'select',
    '#title' => t('Enabled countries'),
    '#default_value' => $settings['countries'],
    '#options' => gmaps_get_countries('names'),
    '#description' => t('If none selected all countries will be included.'),
    '#multiple' => TRUE,
    '#size' => 5,
  );
  $form['gmaps_taxonomy_menu']['delay'] = array('#type' => 'textfield',
    '#title' => t('Menu re-build delay'),
    '#default_value' => $settings['delay'],
    '#required' => TRUE,
    '#size' => 16,
    '#field_suffix' => t('seconds'),
    '#description' => t('Minimum time to wait between each menu re-build.'),
  );
  $form['gmaps_taxonomy_menu']['set_customized'] = array('#type' => 'checkbox',
    '#title' => t('Set cutomized flag of menu items to FALSE'),
    '#default_value' => $settings['set_customized'],
    '#description' => t('The menu systems sets the customized flag of menu items to TRUE when you change anything to an item, which in turn makes them untranslatable. Enabling this feature, you can eliminate that behavior and make them translatable after each menu rebuild.'),
  );
  if ($settings['enabled']) {
    $form['gmaps_taxonomy_menu']['rebuild'] = array('#type' => 'submit',
      '#value' => t('Re-build location navigation menu'),
      '#submit' => array('_gmaps_taxonomy_menu_submit_rebuild'),
    );
  }
  
  $form['seo'] = array('#type' => 'fieldset',
    '#title' => t('SEO'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $seo = variable_get('gmaps_taxonomy_disallow_robots', FALSE);
  if (module_exists('robotstxt')) {
    $form['seo']['gmaps_taxonomy_disallow_robots'] = array('#type' => 'checkbox',
      '#title' => t('Disallow indexing of location taxonomy pages'),
      '#default_value' => $seo,
    );
  }
  else {
    $form['seo']['#collapsible'] = FALSE;
    $form['seo']['#description'] = t('If you enable the robotstxt module, you can disallow indexing of location taxonomy pages.');
    $form['seo']['gmaps_taxonomy_disallow_robots'] = array('#type' => 'value', '#value' => FALSE);
  }
  
  $form['#submit'][] = '_gmaps_taxonomy_admin_settings_presave';
  
  $form = system_settings_form($form);
  
  $form['#validate'][] = '_gmaps_taxonomy_admin_settings_validate';
  $form['#submit'][] = '_gmaps_taxonomy_admin_settings_submit';
  
  return $form;
}




/**********************
 * Internal functions *
 **********************/


function _gmaps_taxonomy_admin_settings_validate($form, &$form_state) {
  $values = $form_state['values']['gmaps_taxonomy_page'];
  if (empty($values['page']) || intval($values['page']) != $values['page'] || $values['page'] <= 0) {
    form_set_error('gmaps_taxonomy_page][page', t('Page length must be an integer greater than 0 (zero).'));
  }
  if (empty($values['feed']) || intval($values['feed']) != $values['feed'] || $values['feed'] <= 0) {
    form_set_error('gmaps_taxonomy_page][feed', t('Feed length must be an integer greater than 0 (zero).'));
  }
  
  $values = $form_state['values']['gmaps_taxonomy_links_cache'];
  if (!empty($values['expires']) && (intval($values['expires']) != $values['expires'] || $values['expires'] < 0)) {
    form_set_error('gmaps_taxonomy_links_cache][expires', t('The link cache expiration value must be an integer greater than or equal to 0 (zero).'));
  }
  
  $values = $form_state['values']['gmaps_taxonomy_page_cache'];
  if (!empty($values['result_expires']) && (intval($values['result_expires']) != $values['result_expires'] || $values['result_expires'] < 0)) {
    form_set_error('gmaps_taxonomy_page_cache][result_expires', t('The result expiration value must be an integer greater than or equal to 0 (zero).'));
  }
  if (!empty($values['output_expires']) && (intval($values['output_expires']) != $values['output_expires'] || $values['output_expires'] < 0)) {
    form_set_error('gmaps_taxonomy_page_cache][output_expires', t('The output expiration value must be an integer greater than or equal to 0 (zero).'));
  }
  
  $values = $form_state['values']['gmaps_taxonomy_menu'];
  if (intval($values['delay']) != $values['delay'] || $values['delay'] < 0) {
    form_set_error('gmaps_taxonomy_menu][delay', t('Menu re-build delay must be an integer greater than or equal to 0 (zero).'));
  }

  $values = $form_state['values']['gmaps_taxonomy_location_cron'];
  if (!empty($values) && (intval($values) != $values || $values < 0)) {
    form_set_error('gmaps_taxonomy_location_cron', t('The cron limit for location terms must be an integer greater than or equal to 0 (zero).'));
  }

  $values = $form_state['values']['gmaps_taxonomy_letter_cron'];
  if (!empty($values) && (intval($values) != $values || $values < 0)) {
    form_set_error('gmaps_taxonomy_letter_cron', t('The cron limit for letter terms must be an integer greater than or equal to 0 (zero).'));
  }
}

function _gmaps_taxonomy_admin_settings_presave($form, &$form_state) {
  $form_state['values']['gmaps_taxonomy_menu']['levels'] = array_keys(array_filter($form_state['values']['gmaps_taxonomy_menu']['levels']));
  $form_state['values']['gmaps_taxonomy_menu']['countries'] = array_keys(array_filter($form_state['values']['gmaps_taxonomy_menu']['countries']));
}

function _gmaps_taxonomy_admin_settings_submit($form, &$form_state) {
  variable_del('gmaps_taxonomy_location_cron_reset');
  variable_del('gmaps_taxonomy_letter_cron_reset');
  
  gmaps_taxonomy_cache_clear_all('*');
  
  $menu = gmaps_taxonomy_get_settings('gmaps_taxonomy_menu');
  $enabled = $form_state['values']['gmaps_taxonomy_menu']['enabled'];
  
  unset($menu['rebuild']);
  variable_set('gmaps_taxonomy_menu', $menu);
  
  if (!$enabled) {
    db_query("DELETE FROM {menu_links} WHERE module = 'gmaps_taxonomy'");
    cache_clear_all('%:'. GMAPS_TAXONOMY_MENU_NAME .':', 'cache_menu', TRUE);
  }
  
  menu_rebuild();
  menu_enable();
}

function _gmaps_taxonomy_menu_submit_rebuild() {
  _gmaps_call_func('gmaps_taxonomy', 'cron', '_gmaps_taxonomy_menu_rebuild');
}

function _gmaps_taxonomy_location_cron_submit_reset() {
  variable_del('gmaps_taxonomy_location_cron_last_nid');
}

function _gmaps_taxonomy_letter_cron_submit_reset() {
  variable_del('gmaps_taxonomy_letter_cron_last_nid');
}

/**
 * Theme the menu overview form into a table.
 * Overwrite of theme_menu_overview_form.
 *
 * @ingroup themeable
 */
function theme_location_navigation_menu_overview_form($form) {
  //tree rearrange is disabled
  //drupal_add_tabledrag('menu-overview', 'match', 'parent', 'menu-plid', 'menu-plid', 'menu-mlid', TRUE, MENU_MAX_DEPTH - 1);
  drupal_add_tabledrag('menu-overview', 'order', 'sibling', 'menu-weight');

  $header = array(
    t('Menu item'),
    array('data' => t('Enabled'), 'class' => 'checkbox'),
    array('data' => t('Expanded'), 'class' => 'checkbox'),
    t('Weight'),
    array('data' => t('Operations'), 'colspan' => '3'),
  );

  $rows = array();
  foreach (element_children($form) as $mlid) {
    if (isset($form[$mlid]['hidden'])) {
      $element = &$form[$mlid];
      // Build a list of operations.
      $operations = array();
      foreach (element_children($element['operations']) as $op) {
        $operations[] = drupal_render($element['operations'][$op]);
      }
      while (count($operations) < 2) {
        $operations[] = '';
      }

      // Add special classes to be used for tabledrag.js.
      $element['plid']['#attributes']['class'] = 'menu-plid';
      $element['mlid']['#attributes']['class'] = 'menu-mlid';
      $element['weight']['#attributes']['class'] = 'menu-weight';

      // Change the parent field to a hidden. This allows any value but hides the field.
      $element['plid']['#type'] = 'hidden';

      $row = array();
      $row[] = theme('indentation', $element['#item']['depth'] - 1) . drupal_render($element['title']);
      $row[] = array('data' => drupal_render($element['hidden']), 'class' => 'checkbox');
      $row[] = array('data' => drupal_render($element['expanded']), 'class' => 'checkbox');
      $row[] = drupal_render($element['weight']) . drupal_render($element['plid']) . drupal_render($element['mlid']);
      $row = array_merge($row, $operations);

      $row = array_merge(array('data' => $row), $element['#attributes']);
      $row['class'] = !empty($row['class']) ? $row['class'] .' draggable' : 'draggable';
      $rows[] = $row;
    }
  }
  $output = '';
  if ($rows) {
    $output .= theme('table', $header, $rows, array('id' => 'menu-overview'));
  }
  $output .= drupal_render($form);
  return $output;
}

function _gmaps_taxonomy_content_field_edit_form_submit($form, &$form_state) {
  $old = &$form['#field']['gmaps']['meta']['taxonomy'];
  $new = &$form_state['values']['gmaps']['meta']['taxonomy'];
  
  if ($old['location'] != $new['location'] || $old['letter'] != $new['letter']) {
    $location = variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED;
    $letter = variable_get('gmaps_taxonomy_letter_terms', 0);
    $locations = $letters = array();
    foreach (content_types() as $type_name => $type) {
      foreach($type['fields'] as $field_name => $field) {
        if (in_array($field['type'], array('gmaps_address', 'gmaps_anp'))) {
          if ($location && $field['widget']['gmaps']['meta']['taxonomy']['location']) {
            $locations[$type_name] = 1;
          }
          if ($letter && $field['widget']['gmaps']['meta']['taxonomy']['letter']) {
            $letters[$type_name] = 1;
          }
        }
        if ($locations[$type_name] && $letters[$type_name]) {
          break;
        }
      }
    }
    if ($location) {
      foreach(variable_get('gmaps_taxonomy_location_vids', array()) as $vid) {
        $voc = taxonomy_vocabulary_load($vid);
        $voc->nodes = $locations;
        $voc = (array)$voc;
        taxonomy_save_vocabulary($voc);
      }
    }
    if ($letter) {
      if ($vid = variable_get('gmaps_taxonomy_letter_vid', 0)) {
        $voc = taxonomy_vocabulary_load($vid);
        $voc->nodes = $letters;
        $voc = (array)$voc;
        taxonomy_save_vocabulary($voc);
      }
    }
  }
}

function _gmaps_taxonomy_form_vocabulary_alter(&$form, &$form_state) {
  $location = variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED;
  $vids = variable_get('gmaps_taxonomy_location_vids', array());
  
  $letter = variable_get('gmaps_taxonomy_letter_terms', 0);
  $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
  
  if (($location && in_array($form['vid']['#value'], $vids)) || ($letter && $form['vid']['#value'] == $vid)) {
    foreach (content_types() as $type_name => $type) {
      foreach($type['fields'] as $field_name => $field) {
        $gmaps = &$field['widget']['gmaps']['meta']['taxonomy'];
        if (in_array($field['type'], array('gmaps_address', 'gmaps_anp')) && (($location && $gmaps['location']) || ($letter && $gmaps['letter']))) {
          unset($form['content_types']['nodes']['#options'][$type_name]);
        }
      }
    }
    $form['settings']['tags'] = array('#type' => 'value', '#value' => 0);
    $form['settings']['multiple'] = array('#type' => 'value', '#value' => 1);
    $form['hierarchy'] = array('#type' => 'value', '#value' => $location ? 1 : 0);
  }
  if (!$location && in_array($form['vid']['#value'], $vids)) {
    $form['gmaps_taxonomy_delete_locations'] = array(
      '#type' => 'submit',
      '#value' => t('Delete all GMaps Taxonomy location vocabularies'),
      '#submit' => array('_gmaps_taxonomy_delete_locations_submit'),
    );
  }
}

function _gmaps_taxonomy_form_term_alter(&$form, &$form_state) {
  $term = &$form['#term'];
  $voc = &$form['#vocabulary'];
  
  if ($term['tid']) {
    $location = variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED;
    $vids = variable_get('gmaps_taxonomy_location_vids', array());
    
    $letter = variable_get('gmaps_taxonomy_letter_terms', 0);
    $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
    
    if (($location && in_array($voc['vid'], $vids)) || ($letter && $voc['vid'] == $vid)) {
      $form['identification']['name_display'] = array('#type' => 'item',
        '#title' => $form['identification']['name']['#title'],
        '#value' => $form['identification']['name']['#default_value'],
        '#weight' => -1,
      );
      $form['identification']['name'] = array('#type' => 'value', '#value' => $form['identification']['name']['#default_value']);
    }
    
    if ($location && in_array($voc['vid'], $vids)) {
      $form['advanced']['parent'] = array('#type' => 'value', '#value' => $form['advanced']['parent']['#default_value']);
      /*$form['advanced']['parent']['#multiple'] = 0;
      $form['advanced']['parent']['#size'] = 1;*/
    }
    else if ($letter && $voc['vid'] == $vid) {
      $form['advanced']['parent'] = array('#type' => 'value', '#value' => 0);
    }
  }
}

function _gmaps_taxonomy_overview_terms_alter(&$form, &$form_state) {
  $location = variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED;
  $vids = variable_get('gmaps_taxonomy_location_vids', array());
  $voc = &$form['#vocabulary'];
  
  if ($location && in_array($voc['vid'], $vids)) {
    unset($form['#parent_fields'], $form['submit'], $form['reset_alphabetical']);
  }
}

function _gmaps_taxonomy_taxonomy($op, $type, $array = NULL) {
  if ($type == 'vocabulary') {
    $location = variable_get('gmaps_taxonomy_location_terms', 0) == GMAPS_TAXONOMY_LOCATION_TERMS_INTEGRATED;
    $vids = variable_get('gmaps_taxonomy_location_vids', array());
    
    $letter = variable_get('gmaps_taxonomy_letter_terms', 0);
    $vid = variable_get('gmaps_taxonomy_letter_vid', 0);
    
    if ($op == 'update') {
      if (($location && in_array($array['vid'], $vids)) || ($letter && $array['vid'] == $vid)) {
        foreach (content_types() as $type_name => $type) {
          foreach($type['fields'] as $field_name => $field) {
            $gmaps = &$field['widget']['gmaps']['meta']['taxonomy'];
            if (in_array($field['type'], array('gmaps_address', 'gmaps_anp')) && (($location && $gmaps['location']) || ($letter && $gmaps['letter']))) {
              $array['nodes'][$type_name] = $type_name;
            }
          }
        }
        db_query("DELETE FROM {vocabulary_node_types} WHERE vid = %d", $array['vid']);
        foreach ($array['nodes'] as $type => $selected) {
          db_query("INSERT INTO {vocabulary_node_types} (vid, type) VALUES (%d, '%s')", $array['vid'], $type);
        }
      }
    }
    else if ($op == 'delete') {
      if ($idx = array_search($array['vid'], $vids)) {
        unset($vids[$idx]);
        if (empty($vids)) {
          variable_del('gmaps_taxonomy_location_vids');
        } else {
          variable_set('gmaps_taxonomy_location_vids', $vids);
        }
      }
      else if ($array['vid'] == $vid) {
        variable_del('gmaps_taxonomy_letter_vid');
      }
    }
  }
  else if ($type == 'term') {
    
  }
}

function _gmaps_taxonomy_delete_locations_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/content/taxonomy/gmaps-delete';
}

function gmaps_taxonomy_delete_location_vocabularies() {
  return confirm_form(array(),
                  t('Are you sure you want to delete all GMaps Taxonomy location vocabularies?'),
                  'admin/content/taxonomy',
                  t('Deleting a vocabulary will delete all the terms in it. This action cannot be undone.'),
                  t('Delete'),
                  t('Cancel'));
}

function gmaps_taxonomy_delete_location_vocabularies_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    foreach(variable_get('gmaps_taxonomy_location_vids', array()) as $vid) {
      taxonomy_del_vocabulary($vid);
    }
    variable_del('gmaps_taxonomy_location_vids');
    drupal_set_message(t('Deleted all GMaps Taxonomy location vocabularies.'));
    watchdog('taxonomy', 'Deleted all GMaps Taxonomy location vocabularies.', array(), WATCHDOG_NOTICE);
  }
  $form_state['redirect'] = 'admin/content/taxonomy';
}

function _gmaps_taxonomy_form_menu_edit_item_alter(&$form, &$form_state) {
  $item = $form['menu']['#item'];
  
  $form['menu']['_path']['#description'] = l($item['title'], $item['href'], $item['options']);
  
  $form['menu']['link_title'] = array('#type' => 'value', '#value' => $form['menu']['link_title']['#default_value']);
  $form['menu']['parent'] = array('#type' => 'value', '#value' => $form['menu']['parent']['#default_value']);
}

