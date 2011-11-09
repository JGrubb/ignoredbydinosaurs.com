<?php
// $Id: gmaps-builtin-icons.inc,v 1.1.4.2 2009/08/20 06:15:10 xmarket Exp $

function _gmaps_builtin_icons() {
  //must return key ready to use in gmaps_icon_config_save
  $icons = array();
  $path = drupal_get_path('module', 'gmaps') . '/resouces/icons/legacy/';
  
  $group = t('Coloured icons');
  $icons[$group][0] = array(
    'name' => 'Red balloon',
    'image' => $path . 'red.png',
  );
  $icons[$group][1] = array(
    'name' => 'Blue balloon',
    'image' => $path . 'blue.png',
  );
  $icons[$group][2] = array(
    'name' => 'Gray balloon',
    'image' => $path . 'gray.png',
  );
  $icons[$group][3] = array(
    'name' => 'Green balloon',
    'image' => $path . 'green.png',
  );
  $icons[$group][4] = array(
    'name' => 'Lightblue balloon',
    'image' => $path . 'lblue.png',
  );
  $icons[$group][5] = array(
    'name' => 'Orange balloon',
    'image' => $path . 'orange.png',
  );
  $icons[$group][6] = array(
    'name' => 'Pink balloon',
    'image' => $path . 'pink.png',
  );
  $icons[$group][7] = array(
    'name' => 'Purple balloon',
    'image' => $path . 'purple.png',
  );
  $icons[$group][8] = array(
    'name' => 'White balloon',
    'image' => $path . 'white.png',
  );
  $icons[$group][9] = array(
    'name' => 'Yellow balloon',
    'image' => $path . 'yellow.png',
  );

  $group = t('Miscellaneous icons');
  $icons[$group][10] = array(
    'name' => 'Drupal balloon',
    'image' => $path . 'drupal.png',
  );
  $icons[$group][11] = array(
    'name' => 'Route start balloon',
    'image' => $path . 'routestart.png',
  );
  $icons[$group][12] = array(
    'name' => 'Route end balloon',
    'image' => $path . 'routeend.png',
  );

  $group = t('Day icons');
  $icons[$group][13] = array(
    'name' => 'Monday balloon',
    'image' => $path . 'marker_monday.png',
  );
  $icons[$group][14] = array(
    'name' => 'Tuesday balloon',
    'image' => $path . 'marker_tuesday.png',
  );
  $icons[$group][15] = array(
    'name' => 'Wednesday balloon',
    'image' => $path . 'marker_wednesday.png',
  );
  $icons[$group][16] = array(
    'name' => 'Thursday balloon',
    'image' => $path . 'marker_thursday.png',
  );
  $icons[$group][17] = array(
    'name' => 'Friday balloon',
    'image' => $path . 'marker_friday.png',
  );
  $icons[$group][18] = array(
    'name' => 'Saturday balloon',
    'image' => $path . 'marker_saturday.png',
  );
  $icons[$group][19] = array(
    'name' => 'Sunday balloon',
    'image' => $path . 'marker_sunday.png',
  );

  $group = t('Numbered icons');
  $icons[$group][20] = array(
    'name' => 'No. 0 balloon',
    'image' => $path . 'number0.png',
  );
  $icons[$group][21] = array(
    'name' => 'No. 1 balloon',
    'image' => $path . 'number1.png',
  );
  $icons[$group][22] = array(
    'name' => 'No. 2 balloon',
    'image' => $path . 'number2.png',
  );
  $icons[$group][23] = array(
    'name' => 'No. 3 balloon',
    'image' => $path . 'number3.png',
  );
  $icons[$group][24] = array(
    'name' => 'No. 4 balloon',
    'image' => $path . 'number4.png',
  );
  $icons[$group][25] = array(
    'name' => 'No. 5 balloon',
    'image' => $path . 'number5.png',
  );
  $icons[$group][26] = array(
    'name' => 'No. 6 balloon',
    'image' => $path . 'number6.png',
  );
  $icons[$group][27] = array(
    'name' => 'No. 7 balloon',
    'image' => $path . 'number7.png',
  );
  $icons[$group][28] = array(
    'name' => 'No. 8 balloon',
    'image' => $path . 'number8.png',
  );
  $icons[$group][29] = array(
    'name' => 'No. 9 balloon',
    'image' => $path . 'number9.png',
  );
  $icons[$group][30] = array(
    'name' => 'No. 10 balloon',
    'image' => $path . 'number10.png',
  );
  $icons[$group][31] = array(
    'name' => 'No. 11 balloon',
    'image' => $path . 'number11.png',
  );
  $icons[$group][32] = array(
    'name' => 'No. 12 balloon',
    'image' => $path . 'number12.png',
  );
  $icons[$group][33] = array(
    'name' => 'No. 13 balloon',
    'image' => $path . 'number13.png',
  );
  $icons[$group][34] = array(
    'name' => 'No. 14 balloon',
    'image' => $path . 'number14.png',
  );
  $icons[$group][35] = array(
    'name' => 'No. 15 balloon',
    'image' => $path . 'number15.png',
  );
  $icons[$group][36] = array(
    'name' => 'No. 16 balloon',
    'image' => $path . 'number16.png',
  );
  $icons[$group][37] = array(
    'name' => 'No. 17 balloon',
    'image' => $path . 'number17.png',
  );
  $icons[$group][38] = array(
    'name' => 'No. 18 balloon',
    'image' => $path . 'number18.png',
  );
  $icons[$group][39] = array(
    'name' => 'No. 19 balloon',
    'image' => $path . 'number19.png',
  );
  $icons[$group][40] = array(
    'name' => 'No. 20 balloon',
    'image' => $path . 'number20.png',
  );
  $icons[$group][41] = array(
    'name' => 'No. 21 balloon',
    'image' => $path . 'number21.png',
  );
  $icons[$group][42] = array(
    'name' => 'No. 22 balloon',
    'image' => $path . 'number22.png',
  );
  $icons[$group][43] = array(
    'name' => 'No. 23 balloon',
    'image' => $path . 'number23.png',
  );
  $icons[$group][44] = array(
    'name' => 'No. 24 balloon',
    'image' => $path . 'number24.png',
  );
  $icons[$group][45] = array(
    'name' => 'No. 25 balloon',
    'image' => $path . 'number25.png',
  );
  $icons[$group][46] = array(
    'name' => 'No. 26 balloon',
    'image' => $path . 'number26.png',
  );

  $group = t('Literal icons');
  $icons[$group][47] = array(
    'name' => 'Letter A balloon',
    'image' => $path . 'letterA.png',
  );
  $icons[$group][48] = array(
    'name' => 'Letter B balloon',
    'image' => $path . 'letterB.png',
  );
  $icons[$group][49] = array(
    'name' => 'Letter C balloon',
    'image' => $path . 'letterC.png',
  );
  $icons[$group][50] = array(
    'name' => 'Letter D balloon',
    'image' => $path . 'letterD.png',
  );
  $icons[$group][51] = array(
    'name' => 'Letter E balloon',
    'image' => $path . 'letterE.png',
  );
  $icons[$group][52] = array(
    'name' => 'Letter F balloon',
    'image' => $path . 'letterF.png',
  );
  $icons[$group][53] = array(
    'name' => 'Letter G balloon',
    'image' => $path . 'letterG.png',
  );
  $icons[$group][54] = array(
    'name' => 'Letter H balloon',
    'image' => $path . 'letterH.png',
  );
  $icons[$group][55] = array(
    'name' => 'Letter I balloon',
    'image' => $path . 'letterI.png',
  );
  $icons[$group][56] = array(
    'name' => 'Letter J balloon',
    'image' => $path . 'letterJ.png',
  );

  return $icons;
}

function _gmaps_bultin_icon_images() {
  $info = _gmaps_builtin_icons();
  $images = array();
  
  foreach($info as $group => $icons) {
    foreach($icons as $builtinid => $icon) {
      $images[$builtinid] = $icon['image'];
    }
  }
  
  return $images;
}
