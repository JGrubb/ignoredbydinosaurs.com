<?php
// $Id: gmaps-static-map-item.tpl.php,v 1.1.2.2 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file gmaps-static-map-item.tpl.php
 *
 * Default theme implementation to display a static map.
 *
 * Available variables:
 * - $smi: the map object.
 * - $title: title attribute
 * - $alt: alt attribute
 * - $content: map content.
 *
 * @See: template_preprocess_gmaps_static_map_item().
 */
?>
<div id="<?php print $smi->id;?>" <?php print drupal_attributes($smi->attributes);?>>
  <?php
    $img_attr = array('class' => 'gmaps-static-map-image');
    if (!$smi->throttle && !$smi->se_agent && !empty($smi->file_path) && file_exists($smi->file_path)) {
      print theme('image', $smi->file_path, $title, $alt, $img_attr);
    }
    else if (!empty($smi->url)) {
      print theme('image', $smi->url, $title, $alt, $img_attr, FALSE);
    }
  ?>
  
  <?php if ($content) :?>
  <div class="gmaps-static-map-content"><?php print $content;?></div>
  <?php endif;?>
</div>
