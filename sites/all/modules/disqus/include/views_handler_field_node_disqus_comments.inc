<?php
/**
* Field handler to present the number of Disqus comments on a node.
*/
class views_handler_field_node_disqus_comments extends views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
  }

  function query() {
    // Override parent::query() without altering query.
  }

  /**
   * When rendering the field.
   */
  function render($values) {
    // Ensure Disqus comments are available on the node user has access to edit this node.
    $node = node_load($values->nid);
    if (user_access('view disqus comments') && isset($node->disqus)) {
      return theme('disqus_comments_num', $node->disqus['domain'], $node->disqus['url']);
    }
  }
}