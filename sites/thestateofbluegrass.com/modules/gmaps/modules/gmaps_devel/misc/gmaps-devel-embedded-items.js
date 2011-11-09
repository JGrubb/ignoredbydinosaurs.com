// $Id: gmaps-devel-embedded-items.js,v 1.1.2.3 2009/09/14 07:58:00 xmarket Exp $

GMAPS_DEVEL_PAGE_START_TIME = new Date().getTime();

/**
 * Overwrite to skip lazy init
 */
/*
Drupal.behaviors.gmapsMapItem = function(context) {
  $('.gmaps-map-item:not(.gmaps-map-item-embedded)', context).each(function(index, domElement) {
    setTimeout(function() {Drupal.gmaps.map.getMap(domElement.id, context, false);}, 5);
  });
};
*/


Drupal.gmaps.accordion.behaviors.gmapsDevelEmbeddedItems = function(a) {
  $('#initialized-items').append('<div>Accordion: '+ a.parent()[0].id +' ('+ (new Date().getTime() - GMAPS_DEVEL_PAGE_START_TIME)/1000 +'s)</div>');
};

Drupal.gmaps.tabs.behaviors.gmapsDevelEmbeddedItems = function(t) {
  $('#initialized-items').append('<div>Tabs: '+ t.parent()[0].id +' ('+ (new Date().getTime() - GMAPS_DEVEL_PAGE_START_TIME)/1000 +'s)</div>');
};

Drupal.gmaps.address.behaviors.gmapsDevelEmbeddedItems = function(a) {
  $('#initialized-items').append('<div>Address: '+ a.getItem()[0].id +' ('+ (new Date().getTime() - GMAPS_DEVEL_PAGE_START_TIME)/1000 +'s)</div>');
};

Drupal.gmaps.point.behaviors.gmapsDevelEmbeddedItems = function(p) {
  $('#initialized-items').append('<div>Point: '+ p.getItem()[0].id +' ('+ (new Date().getTime() - GMAPS_DEVEL_PAGE_START_TIME)/1000 +'s)</div>');
};

Drupal.gmaps.map.behaviors.gmapsDevelEmbeddedItems = function(gmi) {
  $('#initialized-items').append('<div>Map: '+ gmi.getOptions().id +' ('+ (new Date().getTime() - GMAPS_DEVEL_PAGE_START_TIME)/1000 +'s)</div>');
};

