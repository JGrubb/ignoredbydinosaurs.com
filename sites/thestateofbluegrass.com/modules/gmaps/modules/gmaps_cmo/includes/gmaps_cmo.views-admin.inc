<?php
// $Id: gmaps_cmo.views-admin.inc,v 1.1.2.1 2010/02/18 16:36:22 xmarket Exp $

function _gmaps_cmo_views_data_alter(&$data) {
  $group = t('Marker option');
  
  $part_title = t('Icon options - Content type');
  $part_title_short = t('Icon options');
  $data['node']['gmaps_icon'] = array(
    'group' => $group,
    'title' => $part_title,
    'title short' => $part_title_short,
    'help' =>  t('Content type based interactive and static marker icon and protected icon options.'),
    'field' => array(
      'field' => 'type',
      'handler' => 'gmaps_cmo_handler_marker_icon',
      'click sortable' => FALSE,
    ),
  );
}

