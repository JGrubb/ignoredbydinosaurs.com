<?php

function ibd_node_submitted($node) {
  return t('!username . @datetime',
    array(
      '!username' => theme('username', $node),
      '@datetime' => format_date($node->created),
    ));
}