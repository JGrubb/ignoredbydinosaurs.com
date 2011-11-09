<?php
// $Id: gmaps-views-view-map.tpl.php,v 1.1.2.1 2009/12/03 12:29:46 xmarket Exp $
/**
 * @file gmaps-views-view-map.tpl.php
 * View template to display a map item.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php if (!empty($gmaps_map_item)): ?>
  <div><?php print $gmaps_map_item; ?></div>
<?php endif; ?>
