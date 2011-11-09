// $Id: gmaps-anp-element.js,v 1.1.2.5 2010/03/18 10:52:16 xmarket Exp $

Drupal.behaviors.gmapsAnpElement = function(context) {
  $('.form-gmaps-anp', context).each(function(index, element) {
    var handler, init = function() {Drupal.gmaps.anp.getElement(element.id, context, true);};
    if (handler = Drupal.gmaps.getLazyInitHandler('#'+ element.id, context)) {
      handler.attach('#'+ element.id, init, context);
    }
    else {
      setTimeout(init, 5);
    }
  });
};

/**
 * Global AnP object.
 */
Drupal.gmaps.anp = Drupal.gmaps.anp || new (function() {
  var self = this;
  this.elements = {};
  
  this.behaviors = {};

  this.getElement = function(id, context, refresh) {
    if (self.elements[id]) {
      if (refresh) {
        self.elements[id] = null;
      }
      else {
        return self.elements[id];
      }
    }
    context = context || document;
    var element = $('#'+ id +':not(.gmaps-anp-processed)', context);
    if (element.length) {
      element = $(element[0]);
      element.addClass('gmaps-anp-processed');
      self.elements[id] = new GMapsAnpElement(element);
    }
    return self.elements[id];
  };
  
  this.attachBehaviors = function(anp) {
    if (anp) {
      $.each(self.behaviors, function() {
        this(anp);
      });
    }
  };
})();

GMapsAnpElement = function (item) {
  var self = this;
  var geocoder = $('.form-gmaps-geocoder', item);
  geocoder = geocoder.length ? Drupal.gmaps.geocoder.getElement(geocoder[0].id, item) : null;
  
  var address = $('.form-gmaps-address', item);
  address = Drupal.gmaps.address.getElement(address[0].id, item);
  
  var point = $('.form-gmaps-point', item);
  point = Drupal.gmaps.point.getElement(point[0].id, item);

  this.clearValues = function() {
    address.clearValues();
    point.clearValues();
  };
  
  this.setValues = function(placemark) {
    if (address.checkCountry(placemark.address.country)) {
      address.setValues(placemark.address);
      point.setValues(placemark.point);
    }
    else if (geocoder) {
      geocoder.showError(Drupal.t('Invalid country.'));
    }
  };
  
  this.getValues = function() {
    var anp = {
      'address': address.getValues(),
      'point': point.getValues()
    };
    return anp;
  };
  
  this.getLatLng = function() {
    if (typeof(GLatLng) != 'undefined') {
      return point.getLatLng();
    }
  };
  
  if (geocoder) {
    geocoder.setCallback(this.setValues);
    point.setGeocoder(geocoder);
  }
  
  Drupal.gmaps.anp.attachBehaviors(this);
};
