<?php
// $Id: gmaps-length-multipart.tpl.php,v 1.1.2.1 2009/01/20 13:16:18 xmarket Exp $

/**
 * @file gmaps-length.tpl.php
 * 
 * Default theme implementation to display a length.
 *
 * Available variables:
 * - $parts: array of themed gmaps_lengths
 * - $group_css: The css-compatible unit group name.
 */
?>
<span class="gmaps-length-multipart gmaps-length-multipart-<?php print $group_css ?>"><?php
  foreach ($parts as $i => $part) :?>
<span class="gmaps-length-part-<?php print ($i == 0 ? 'first' : ($i == count($parts) -1 ? 'last' : 'middle'));?>"><?php print $part; ?></span>
<?php endforeach;?>
</span>
