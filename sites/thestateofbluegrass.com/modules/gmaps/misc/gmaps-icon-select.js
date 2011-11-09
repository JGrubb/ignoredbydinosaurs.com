// $Id: gmaps-icon-select.js,v 1.1.2.5 2009/09/14 07:57:59 xmarket Exp $

Drupal.behaviors.gmapsIconSelectElement = function(context) {
  $('select.form-gmaps-icon-select:not(.gmaps-icon-select-processed)', context).each(function (index, domElement) {
    setTimeout(function() {
      var select = $(domElement);
      new GMapsIconSelectElement(select);
      select.addClass('gmaps-icon-select-processed');
    }, 5);
  });
};

/**
 * Icon select object
 * 
 * @param select
 *  jQuery object of the "select" dom element
 */
GMapsIconSelectElement = function(select) {
  var gis = this;
  var parent = $(select).parent('div.gis-preview-wrapper');
  var img = $('img.gis-preview', parent);
  
  var preview = function(e) {
    img.hide();
    
    if (typeof(Drupal.settings.gmaps) == 'undefined' || typeof(Drupal.settings.gmaps.iconSelect) == 'undefined' ||
        typeof(Drupal.settings.gmaps.iconSelect[select[0].id]) == 'undefined') {
      return;
    }
    
    var attr = Drupal.settings.gmaps.iconSelect[select[0].id][select.fieldValue()[0]];
    
    if (typeof(attr) == 'undefined' || typeof(attr.src) == 'undefined') {
    }
    else {
      img.attr('src', attr.src);
      img.css({'width': attr.width, 'height': attr.height});
      img.show();
    }
  };
  
  select.change(preview).keyup(preview);
  
  preview();
};


