<?php
// $Id: gmaps-length.tpl.php,v 1.1.2.1 2009/01/20 13:16:18 xmarket Exp $

/**
 * @file gmaps-length.tpl.php
 * 
 * Default theme implementation to display a length.
 *
 * Available variables:
 * - $length: The value.
 * - $abbr: The abbreviation to display.
 * - $unit: The unit object.
 * - $group_css: The css-compatible unit group name.
 * - $unit_key_css: The css-compatible unit key.
 */
?>
<span class="gmaps-length gmaps-length-<?php print $group_css ?> gmaps-length-<?php print $unit_key_css ?>"><?php print $length ?><abbr title="<?php print $unit->title; ?>"><?php print $abbr ?></abbr></span>