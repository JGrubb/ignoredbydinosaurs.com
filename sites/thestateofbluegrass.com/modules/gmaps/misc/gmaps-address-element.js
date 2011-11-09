// $Id: gmaps-address-element.js,v 1.1.2.5 2010/03/18 10:52:17 xmarket Exp $

Drupal.behaviors.gmapsAddressElement = function(context) {
  $('.form-gmaps-address:not(.gmaps-address-embedded)', context).each(function(index, element) {
    var handler, init = function() {Drupal.gmaps.address.getElement(element.id, context, true);};
    if (handler = Drupal.gmaps.getLazyInitHandler('#'+ element.id, context)) {
      handler.attach('#'+ element.id, init, context);
    }
    else {
      setTimeout(init, 5);
    }
  });
};

/**
 * Global Address object.
 * Equivalent of "Drupal" object.
 */
Drupal.gmaps.address = Drupal.gmaps.address || new (function() {
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
    var element = $('#'+ id +':not(.gmaps-address-processed)', context);
    if (element.length) {
      element = $(element[0]);
      element.addClass('gmaps-address-processed');
      self.elements[id] = new GMapsAddressElement(element);
    }
    return self.elements[id];
  };

  this.attachBehaviors = function(adr) {
    if (adr) {
      $.each(self.behaviors, function() {
        this(adr);
      });
    }
  };
})();

GMapsAddressElement = function (item) {
  var ga = this;
  var geocoder = $('.form-gmaps-geocoder', item);
  geocoder = geocoder.length ? Drupal.gmaps.geocoder.getElement(geocoder[0].id, item) : null;
  var country = $('.form-gmaps-address-country', item);
  var countryOptions, currentCountryClass, i, gmi, marker, searchListener;
  
  var parts = {};
  for (i in Drupal.settings.gmaps.address.parts) {
    var k = Drupal.settings.gmaps.address.parts[i];
    parts[k] = $('.form-gmaps-address-'+k, item);
  }
  
  var getCountryOptions = function() {
    if (!countryOptions) {
      countryOptions = {};
      $('option', country).each(function() {
        countryOptions[$(this).val()] = true;
      });
    }
    
    return countryOptions;
  };
  
  var setCountryClass = function() {
    if (!Drupal.settings.gmaps.address.accessAllParts) {
      var oldClass = currentCountryClass;
      if (country.fieldValue() != null && country.fieldValue().length) {
        currentCountryClass = 'gmaps-address-country-'+ country.fieldValue()[0].toLowerCase();
        item.addClass(currentCountryClass);
      }
      if (oldClass && oldClass != currentCountryClass) {
        item.removeClass(oldClass);
      }
    }
  };
  
  var showPoint = function(p) {
    if (marker) {
      if (p) {
        marker.setLatLng(new GLatLng(p.latitude, p.longitude));
        if (bounds = Drupal.gmaps.map.getOverlayBounds(p.bounds ? {gmaps: {point : p}} : marker)) {
          gmi.gmap2.setCenter(bounds.getCenter(), gmi.gmap2.getBoundsZoomLevel(bounds));
        }
        else {
          gmi.gmap2.setCenter(marker.getLatLng());
        }
        marker.show();
      }
      else {
        marker.hide();
        gmi.gmap2.setCenter(new GLatLng(0, 0), 0);
      }
    }
  };
  
  this.checkCountry = function(c) {
    if (getCountryOptions()[c] == null){
      if (geocoder) {
        geocoder.showError(Drupal.t('Invalid country.'));
      }
      return false;
    }
    return true;
  };
  
  this.getItem = function() {
    return item;
  };
  
  this.clearValues = function() {
    for (i in getCountryOptions()) {
      country.val(i);
      setCountryClass();
      break;
    }
    for (i in parts) {
      parts[i].val('');
    }
  };
  
  this.setValues = function(placemark) {
    var adr = (typeof(placemark.address) != 'undefined') ? placemark.address : placemark;
    if (ga.checkCountry(adr.country)) {
      country.val(adr.country);
      setCountryClass();
      for (i in parts) {
        parts[i].val(adr[i] != null ? adr[i] : '');
      }
    }
    else {
      ga.clearValues();
    }
    showPoint(placemark.point);
  };
  
  if (geocoder) {
    geocoder.setCallback(this.setValues);
    
    gmi = $('.gmaps-map-item', item);
    if (gmi.length) {
      $('.gmaps-map-container', gmi).height($('.gmaps-address-gmi', item).height());
      gmi = Drupal.gmaps.map.getMap(gmi[0].id, item);
    }
    else {
      gmi = null;
    }
    if (gmi) {
      geocoder.setMap(gmi.gmap2);
      marker = gmi.overlays.marker.input;
      geocoder.setMarker(marker);
      
      searchListener = GEvent.addListener(gmi.gmap2, 'dblclick', function(ov, ll) {
        geocoder.doSearch(ll, true);
      });
    }
  }
  
  if (!Drupal.settings.gmaps.address.accessAllParts) {
    country.change(setCountryClass).keyup(setCountryClass);
    setCountryClass();
  }

  Drupal.gmaps.address.attachBehaviors(this);
};

