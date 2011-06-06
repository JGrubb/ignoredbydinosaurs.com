<?php

/**
* Implementation of hook_views_data_alter.
*/
function disqus_views_data_alter(&$data) {
  // Number of Disqus comments made on the given node.
  $data['node']['disqus_comments'] = array(
    'field' => array(
      'title' => t('Disqus: Comments'),
      'help' => t('The number of Disqus comments made on the node. Note that this will not work in the preview.'),
      'handler' => 'views_handler_field_node_disqus_comments',
    ),
  );
}

/**
* Implementation of hook_views_handlers().
*/
function disqus_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'disqus') . '/include',
    ),
    'handlers' => array(
      // field handlers
      'views_handler_field_node_disqus_comments' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
