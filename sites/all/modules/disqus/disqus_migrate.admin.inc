<?php

/**
 * Menu callback; Import comments from Disqus to the Drupal comment system.
 */
function disqus_migrate_admin_settings_import() {
  $form = array();
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['file'] = array(
    '#type' => 'file',
    '#title' => t('Import'),
    '#description' => t('The comments.xml file generated in the <a href="@disqustools">Disqus tools administration</a>. You can download this file by clicking on <a href="@commentsxml">comments.xml</a>.', array('@commentsxml' => 'http://disqus.com/migrate/'. variable_get('disqus_domain', '') .'/comments.xml', '@disqustools' => 'http://disqus.com/admin/tools/'. variable_get('disqus_domain', '') .'/')),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  return $form;
}

/**
 * Form submit handler for the import tool.
 */
function disqus_migrate_admin_settings_import_submit($form, &$form_state) {
  // See if the file was successfully saved.
  if ($file = file_save_upload('file')) {
    $simplexml = @simplexml_load_file($file->filepath);
    if ($simplexml === FALSE) {
      form_set_error('file', t('Invalid XML file uploaded.'));
    }
    else {

      // Keep track of the number of comments imported.
      $count = 0;

      foreach($simplexml->article as $article) {
        // Check if the URLs match a Drupal path.
        $url = parse_url($article->url);
        $path = substr($url['path'], strlen(base_path()));
        if (url($path, array('absolute' => TRUE)) == $article->url) {
          
          // Resolve any path aliases.
          if (function_exists('custom_url_rewrite_inbound')) {
            $path = drupal_get_normal_path($path);
          }
  
          // Import any node comments.
          if (substr($path, 0, 4) == 'node') {
            $nid = substr($path, 5, 1);
            // Loop through the comments and import them into Drupal.
            foreach ($article->comments as $thread) {
              foreach($thread->comment as $comment) {
                comment_save(array(
                  'nid' => $nid,
                  'mail' => $comment->email,
                  'homepage' => $comment->url,
                  'name' => $comment->name,
                  'comment' => $comment->message,
                  'subject' => $comment->message,
                  'format' => 0,
                  'cid' => NULL,
                  'pid' => 0,
                  'uid' => 0,
                  'timestamp' => strtotime($comment->date), // Convert the date to a timestamp.
                ));
                $count++;
              }
            }
          }
          
          // No other content type supports Disqus commenting 
        }
      }

      // Display a success message.
      drupal_set_message(t('There were @count comments that were imported. Visit the <a href="@comments">Comments administration</a> to see the imported comments.', array('@comments' => base_path() .'admin/content/comment', '@count' => $count)));
    }
  }
}

/**
 * Menu callback; Export comments from Drupal to the Disqus comment system.
 */
function disqus_migrate_admin_settings_export() {
  $form = array();
  $user_api_key = variable_get('disqus_userapikey', '');

  if (empty($user_api_key)) {
    drupal_set_message(t('A user key must be specified in the general settings to export comments.'), 'error');
    drupal_goto('admin/settings/disqus');
  }
  else {
    $form['disqus_moderated'] = array(
      '#type' => 'checkbox',
      '#title' => t('Include unapproved comments in export.'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Export'),
    ); 
  }
  
  return $form;
}

/**
 * Form submit handler for the export tool.
 */
function disqus_migrate_admin_settings_export_submit($form, &$form_state) {
  // Include the disqus API.
  $user_api_key = variable_get('disqus_userapikey', '');
  $disqus = disqus($user_api_key);

  $node_types = variable_get('disqus_nodetypes', array());
  foreach ($node_types as $type) {
    if ($type) $types[] = "'$type'";
  }
  $types = implode(', ', $types);

  // Retrieve the forum ID.
  $domain = variable_get('disqus_domain', '');
  $forum_id = NULL;
  try {
    $forum_list = $disqus->get_forum_list();
    foreach ($forum_list as $key => $object) {
      if ($object->shortname == $domain) {
        $forum_id = $object->id;
      }
    }
  }
  catch (DisqusException $exception) {
    drupal_set_message(t('There was an error retrieving the available forums from Disqus. Please check your user API key and try again.'), 'error');
    drupal_goto('admin/settings/disqus');
  }

  // Get the forum API key.
  $forum_api_key = NULL;
  try {
    $forum_api_key = $disqus->get_forum_api_key($forum_id);
  }
  catch (DisqusException $exception) {
    drupal_set_message(t('There was an error retrieving the forum API key from Disqus. Please check your user API key, the shortname domain name and then try again.'), 'error');
    return;
  }
  $disqus = disqus($user_api_key, $forum_api_key);

  // Find all comments not already copied to disqus.
  $results = db_query("
    SELECT c.*, u.mail as user_mail, n.title as node_title FROM {comments} c
    INNER JOIN {users} u ON u.uid = c.uid
    INNER JOIN {node} n ON n.nid = c.nid
    LEFT JOIN {disqus_migrate_export} d ON d.cid = c.cid AND d.fid = %d
    WHERE d.did IS NULL AND c.status IN (%s)
    " . (!empty($types) ? "AND n.type IN ($types)" : '') . "
    ORDER BY n.created DESC", $forum_id, ($form_state['values']['disqus_moderated'] ? '0,1' : '0'));

  $count = 0;
  while ($comment = db_fetch_object($results)) {      
    $url = url("node/{$comment->nid}", array('absolute' => TRUE, 'alias' => TRUE));

    // Check if a thread exists for this node on disqus.
    $thread = $disqus->get_thread_by_url($url);
    if (!$nid_thread[$comment->nid]) {
      $thread = $disqus->get_thread_by_url($url);
      if (!$thread) {
        $thread = $disqus->thread_by_identifier("node-{$comment->nid}", $comment->node_title)->thread;      
        $disqus->update_thread($thread->id, array('url' => $url));
        $thread = $disqus->get_thread_by_url($url);
        $nid_thread[$comment->nid] = $thread;
      }
    }
    else {
      $thread = $nid_thread[$comment->nid];
    }

    // If thread still isn't found, skip this comment (for now).
    if (!$thread) {
      continue;
    }

    // Check if this comment has a parent post.
    $parent_post = NULL;
    if ($comment->pid) {
      $parent_post = db_result(db_query("SELECT did FROM {disqus_migrate_export} WHERE cid = %d AND fid = %d", $comment->pid, $forum_id));
    }

    // Create the comment in the thread on disqus.
    $mail = ($comment->mail !== '' ? $comment->mail : $comment->user_mail);
    $name = ($comment->name !== '' ? $comment->name : substr($mail, 0, strpos($mail, '@')));

    // Disqus requires both mail and name. Give them an anonymous mail/name if not provided.
    if (empty($mail)) {
      $mail = t('anon@anonymous.com');
    }
    if (empty($name)) {
      $name = t('Anonymous');
    }

    $post = $disqus->create_post($thread->id, $comment->comment, $name, $mail, array(
      'created_at' => format_date($comment->timestamp, 'custom', 'Y-m-d\TH:i', 0),
      'ip_address' => $comment->ip_address ? $comment->ip_address : '127.0.0.1',
      'parent_post' => $parent_post,
      'state' => $comment->status ? 'unapproved' : 'approved',
    ));

    // Save the export information to the drupal database.
    if ($post->id) {
      db_query("INSERT INTO {disqus_migrate_export} (did, cid, fid) VALUES (%d, %d, %d)", $post->id, $comment->cid, $forum_id);
      $count++;
    }
  }

  // Display a confirmation to the user.
  drupal_set_message(t('!num been exported. You can <a href="@moderate">moderate the comments on Disqus</a>.', array(
    '@moderate' => 'http://' . variable_get('disqus_domain', '') . '.disqus.com',
    '!num' => format_plural($count, "@count comment has", "@count comments have"),
  )));
}
