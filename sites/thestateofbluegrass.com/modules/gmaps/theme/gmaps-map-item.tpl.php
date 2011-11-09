<?php
// $Id: gmaps-map-item.tpl.php,v 1.1.2.4 2010/03/04 17:18:38 xmarket Exp $

/**
 * @file gmaps-map-item.tpl.php
 *
 * Default theme implementation to display a map.
 *
 * Available variables:
 * - $gmi: the map object.
 * - $content: TOC map content.
 *
 * @See: template_preprocess_gmaps_map_item().
 */
?>
<div id="<?php print $gmi->id;?>" <?php print drupal_attributes($gmi->attributes);?>>
  <div class="gmaps-map-container-wrapper">
    <div class="gmaps-map-container">
      <span class="loading"><?php print t('Loading...');?></span>
      <noscript>
        <p><?php print t('You should enable javascript to use the map.');?></p></noscript>
    </div>
  </div>
  
  <?php if (isset($gmi->base->map_data['svu']) && isset($gmi->base->map_data['svu']['enabled'])) :?>
  <div class="gmaps-map-svu-wrapper"><div class="gmaps-map-svu"></div></div>
  <?php endif;?>
  
  <div class="gmaps-map-bb"></div>
  
  <?php if ($content) :?>
  <div class="gmaps-map-content"><?php print $content;?></div>
  <?php endif;?>
</div>
