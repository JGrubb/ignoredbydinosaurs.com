// $Id: gmaps-devel-geocoder.js,v 1.1.2.4 2009/09/14 07:58:00 xmarket Exp $

Drupal.behaviors.gmapsDevelAttachGeocoder = function(context) {
  var rawResult = $('#gmaps-devel-geocoder-raw-result', context);
  
  if (rawResult.hasClass('gmaps-devel-processed')) {
    return;
  }
  rawResult.addClass('gmaps-devel-processed');
  new GMapsDevelGeocoder(rawResult, $('#gmaps-devel-geocoder-result', context));
};

GMapsDevelGeocoder = function(rawResult, result) {
  
  var geocoder = Drupal.gmaps.geocoder.getElement($('.gmaps-devel-geocoder')[0].id);
  
  var rawCallback = function(placemark) {
    rawResult.text(prepareCode(placemark.toSource()));
  };

  var callback = function(placemark) {
    result.text(prepareCode(placemark.toSource()));
  };
  
  var prepareCode = function(code) {
    code = code.replace(/\(/g, "(\n ");
    code = code.replace(/\{/g, "{\n  ");
    code = code.replace(/\",/g, "\",\n");
    code = code.replace(/\d,/g, "$&\n");
    code = code.replace(/\},/g, "},\n");
    
    return code;
  };
  
  if (geocoder) {
    geocoder.setRawCallback(rawCallback).setCallback(callback);
  }
};
