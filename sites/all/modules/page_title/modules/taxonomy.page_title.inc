<?php

/**
 * @file
 * Taxonomy implementations of the page title hooks
 */


/**
 * Implementation of hook_page_title_alter().
 */
function taxonomy_page_title_alter(&$title) {
  $menu_item = menu_get_item();

  // NOTE: We user $menu_item['map'] here instead of $menu_item['page_arguments']
  //       as Views can interrupt the path ad moves the argument value from 0 to 2.
  //       See issue: http://drupal.org/node/1203768

  // If we're looking at a taxonomy term page, get the term title
  if ( !strncmp($menu_item['path'], 'taxonomy/term/%', 15) &&
       ($term = taxonomy_get_term($menu_item['map'][2])) &&
       variable_get('page_title_vocab_'. $term->vid .'_showfield', 0) &&
       ($term_title = page_title_load_title($term->tid, 'term')) ) {
    $title = $term_title;
  }
}


/**
 * Implementation of hook_page_title_pattern_alter().
 */
function taxonomy_page_title_pattern_alter(&$pattern, &$types) {
  $menu_item = menu_get_item();

  // NOTE: We user $menu_item['map'] here instead of $menu_item['page_arguments']
  //       as Views can interrupt the path ad moves the argument value from 0 to 2.
  //       See issue: http://drupal.org/node/1203768

  // Taxonomy Term Page
  if ( !strncmp($menu_item['path'], 'taxonomy/term/%', 15) &&
       ($term = taxonomy_get_term($menu_item['map'][2])) ) {
    $types['taxonomy'] = $term; 
    $pattern = variable_get('page_title_vocab_'. $types['taxonomy']->vid, '');
  }
}


/**
 * Implementation of hook_page_title_settings().
 */
function taxonomy_page_title_settings() {
  $settings = array();

  $vocabs = taxonomy_get_vocabularies();
  foreach ($vocabs as $vocab) {
    $settings['page_title_vocab_'. $vocab->vid] = array(
      'label' => 'Vocabulary - %vocab_name',
      'label arguments' => array('%vocab_name' => $vocab->name),
      'scopes' => array('global', 'taxonomy'),
      'show field' => TRUE,
      'description' => 'This pattern will be used for all %vocab_name term pages',
      'description arguments' => array('%vocab_name' => $vocab->name),
    );
  }

  return $settings;
}
