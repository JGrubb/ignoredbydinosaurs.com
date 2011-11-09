<?php
// $Id: gmaps-adr-plain--hu.tpl.php,v 1.1.2.2 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file gmaps-adr-plain--hu.tpl.php
 *
 * Default theme implementation to display a Hungarian address as string.
 *
 * Available variables:
 * - $adr: The address object.
 */

$nbsp = "\xC2\xA0";
$lines = array();
if (!empty($adr->locality)) {
  $lines[] = str_replace(' ', $nbsp, $adr->locality);
}
if (!empty($adr->deplocality) || !empty($adr->thoroughfare)) {
  $line = '';
  if (!empty($adr->deplocality)) {
    $line .= $adr->deplocality;
  }
  if (!empty($adr->thoroughfare)) {
    $line .= (empty($adr->deplocality) ? '' : ', ') . $adr->thoroughfare;
  }
  $lines[] = str_replace(' ', $nbsp, $line);
}
if (!empty($adr->postalcode)) {
  $lines[] = str_replace(' ', $nbsp, $adr->postalcode);
}
if (!empty($adr->country)) {
  $lines[] = $adr->country;
}

print implode(', ', $lines);
?>
