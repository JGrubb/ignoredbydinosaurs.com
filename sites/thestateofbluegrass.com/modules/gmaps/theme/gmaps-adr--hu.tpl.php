<?php
// $Id: gmaps-adr--hu.tpl.php,v 1.1.2.2 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file gmaps-adr--hu.tpl.php
 *
 * Default theme implementation to display a Hungarian address.
 *
 * Available variables:
 * - $links: an address object, whose values are 'link' arrays.
 * - $adr: The address object.
 * - $content: rendered version of $links.
 *
 * @See: template_preprocess_gmaps_adr().
 */
?>
<span class="gmaps-adr gmaps-adr-hu adr"><?php
  if (!empty($content->locality)) {
    print '<span class="gmaps-adr-row">'. $content->locality .'</span>';
  }
  if (!empty($content->deplocality) || !empty($content->thoroughfare)) {
    print '<span class="gmaps-adr-row">';
    if (!empty($content->deplocality)) {
      print $content->deplocality;
    }
    if (!empty($content->thoroughfare)) {
      print (empty($content->deplocality) ? '' : ', ') . $content->thoroughfare;
    }
    print  '</span>';
  }
  if (!empty($content->postalcode)) {
    print '<span class="gmaps-adr-row">'. $content->postalcode .'</span>';
  }
  print '<span class="gmaps-adr-row gmaps-adr-row-last">'. $content->country .'</span>';
?></span>
