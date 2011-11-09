<?php
// $Id: gmaps-adr.tpl.php,v 1.1.2.2 2009/09/14 07:57:59 xmarket Exp $

/**
 * @file gmaps-adr.tpl.php
 *
 * Default theme implementation to display an address.
 *
 * Available variables:
 * - $links: an address object, whose values are 'link' arrays.
 * - $adr: The address object.
 * - $content: rendered version of $links.
 *
 * @See: template_preprocess_gmaps_adr().
 */
?>
<span class="gmaps-adr gmaps-adr-generic adr"><?php
  if (!empty($content->thoroughfare)) {
    print '<span class="gmaps-adr-row">'. $content->thoroughfare .'</span>';
  }
  if (!empty($content->locality)) {
    print '<span class="gmaps-adr-row">';
    if (!empty($content->deplocality)) {
      print $content->deplocality .', ';
    }
    print $content->locality .'</span>';
  }
  if (!empty($content->adminarea) || !empty($content->postalcode)) {
    print '<span class="gmaps-adr-row">';
    if (!empty($content->adminarea)) {
      if (!empty($content->subadminarea)) {
        print $content->subadminarea .', ';
      }
      print $content->adminarea;
    }
    if (!empty($content->postalcode)) {
      print (empty($content->adminarea) ? '' : ' ') . $content->postalcode;
    }
    print '</span>';
  }
  print '<span class="gmaps-adr-row gmaps-adr-row-last">'. $content->country .'</span>';
?></span>
