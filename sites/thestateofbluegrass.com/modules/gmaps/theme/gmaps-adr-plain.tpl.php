<?php
// $Id: gmaps-adr-plain.tpl.php,v 1.1.2.3 2009/09/18 13:53:12 xmarket Exp $

/**
 * @file gmaps-adr-plain.tpl.php
 *
 * Default theme implementation to display an address as string.
 *
 * Available variables:
 * - $adr: The address object.
 */

print theme('gmaps_adr_link_plain', $adr)
?>
