// $Id: gmaps-geocoder-element.js,v 1.1.2.9 2010/03/18 10:52:16 xmarket Exp $

Drupal.gmaps.geocoder = Drupal.gmaps.geocoder || new (function() {
  var self = this;
  var rc, geocoder;
  this.elements = {};
  
  
  //private
  //use geocoder.parsePlacemark()
  this.parsePlacemark_ = function(placemark) {
    function checkPostalCode() {
      if (typeof(cont.PostalCode) != 'undefined') {
        ret.address.postalcode = cont.PostalCode.PostalCodeNumber;
      }
    }

    var ret = {
      'accuracy': placemark.AddressDetails.Accuracy,
      'search': placemark.address
    };
    if (ret.accuracy == 0) {
      return ret;
    }

    if (typeof(placemark.Point) != 'undefined' && typeof(placemark.Point.coordinates) != 'undefined') {
      ret.point = {
        'latitude': placemark.Point.coordinates[1],
        'longitude': placemark.Point.coordinates[0],
        'elevation': placemark.Point.coordinates[2]
      };

      if (typeof(placemark.ExtendedData) != 'undefined' && typeof(placemark.ExtendedData.LatLonBox) != 'undefined') {
        var bounds = placemark.ExtendedData.LatLonBox;
        ret.point.bounds = {
          'northeast': {
            'latitude': bounds.north,
            'longitude': bounds.east,
            'elevation': 0
          },
          'southwest': {
            'latitude': bounds.south,
            'longitude': bounds.west,
            'elevation': 0
          }
        };
      }
    }
    
    if (typeof(placemark.AddressDetails.Country) == 'undefined') {
      return ret;
    }
    
    ret.address = {
      'country' : placemark.AddressDetails.Country.CountryNameCode
    };

    //the geocoder can suggest a result in a Premise item (accuracy 9)
    if (ret.accuracy > 8) {
      if (typeof(placemark.AddressDetails.Country.Premise) != 'undefined') {
        ret.address.locality = placemark.AddressDetails.Country.Premise.PremiseName;
      }
      return ret;
    }

    var cont = placemark.AddressDetails.Country;
    checkPostalCode();

    if (typeof(cont.AdministrativeArea) != 'undefined') {
      cont = cont.AdministrativeArea;
      ret.address.adminarea = cont.AdministrativeAreaName;
      checkPostalCode();
      if (typeof(cont.SubAdministrativeArea) != 'undefined') {
        cont = cont.SubAdministrativeArea;
        ret.address.subadminarea = cont.SubAdministrativeAreaName;
        checkPostalCode();
      }
    } else if (typeof(cont.SubAdministrativeArea) != 'undefined') {
      cont = cont.SubAdministrativeArea;
      ret.address.adminarea = cont.SubAdministrativeAreaName;
      checkPostalCode();
    }

    if (typeof(cont.Locality) != 'undefined') {
      cont = cont.Locality;
      ret.address.locality = cont.LocalityName;
      checkPostalCode();
      if (typeof(cont.DependentLocality) != 'undefined') {
        cont = cont.DependentLocality;
        ret.address.deplocality = cont.DependentLocalityName;
        checkPostalCode();
        if (typeof(cont.Thoroughfare) != 'undefined') {
          cont = cont.Thoroughfare;
          ret.address.thoroughfare = cont.ThoroughfareName;
          checkPostalCode();
        }
      } else if (typeof(cont.Thoroughfare) != 'undefined') {
        cont = cont.Thoroughfare;
        ret.address.thoroughfare = cont.ThoroughfareName;
        checkPostalCode();
      }
    } else if (typeof(cont.DependentLocality) != 'undefined') {
      cont = cont.DependentLocality;
      ret.address.deplocality = cont.DependentLocalityName;
      checkPostalCode();
      if (typeof(cont.Thoroughfare) != 'undefined') {
        cont = cont.Thoroughfare;
        ret.address.thoroughfare = cont.ThoroughfareName;
        checkPostalCode();
      }
    } else if (typeof(cont.Thoroughfare) != 'undefined') {
      cont = cont.Thoroughfare;
      ret.address.thoroughfare = cont.ThoroughfareName;
      checkPostalCode();
    }

    return ret;
  };

  this.getResultCache = function() {
    if (rc == null) {
      rc = new GMapsCache();
    }
    return rc;
  };

  this.getGeocoder = function() {
    if (geocoder == null) {
      geocoder = new GMapsGeocoderClient(self.getResultCache());
      
    }
    return geocoder;
  };
  
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
    var element = $('#'+ id +':not(.gmaps-geocoder-processed)', context);
    if (element.length) {
      element = $(element[0]);
      element.addClass('gmaps-geocoder-processed');
      self.elements[id] = new GMapsGeocoderElement(element);
    }
    return self.elements[id];
  };

  this.decodeGoogleError = function(response) {
    if (response.Status) {
      switch(response.Status.code) {
      case 400:
      case 601:
        return Drupal.t('Invalid or missing query.');
      case 500:
        return Drupal.t('The Geocoder occured an internal server error.');
      case 602:
        return Drupal.t('No location found.');
      case 603:
        return Drupal.t('Access denied to location.');
      case 610:
        return Drupal.t('Invalid API key.');
      case 620:
        return Drupal.t('Service temporarly unavaliable, because the given key has gone over the requests limit.');
      }
    }
    return null;
  };
})();

/**
 * Replacement of the GClientGeocoder class.
 * Forces all result to be in english using the "hl=en" argument.
 */
GMapsGeocoderClient = function(resultCache) {
  var rc = resultCache || Drupal.gmaps.geocoder.getResultCache();
  var pc = new GMapsCache();
  
  var baseUrl, ll, spn, gl, local = parseInt(Drupal.settings.gmaps.api.settings.geocode_lifetime, 10);
  if (parseInt(Drupal.settings.gmaps.api.settings.geocode_lifetime)) {
    baseUrl = Drupal.settings.basePath + 'gmaps/geocode/';
  }
  else {
    baseUrl = 'http://maps.google.'+ Drupal.settings.gmaps.api.settings.domain +
      '/maps/geo?output=json&oe=utf8&sensor=false&hl=en&key='+ Drupal.settings.gmaps.api.key +'&q=';
  }
  
  var getDomainBias = function() {
    if (gl != null) {
      return '&gl=' + gl;
    }
    return '';
  };
  
  var getViewportBias = function() {
    if (ll != null && spn != null) {
      return '&ll=' + ll + '&spn=' + spn;
    }
    return '';
  };
  
  var getRequestUrl = function(query) {
    return baseUrl + encodeURIComponent(query) + getDomainBias() + getViewportBias();
  };
  
  this.getLocations = function(query, callback) {
    if (!query || !callback) {
      return null;
    }
    
    var request = getRequestUrl(query);

    if (cached = rc.get(request)) {
      callback(cached);
      return null;
    }
    
    $.ajax({
      type: "GET",
      url: request,
      dataType: local ? 'json' : 'jsonp',
      cache: true,
      success: function (response) {
        if (typeof(response['status']) != 'undefined' && response['status'] == 0) {
          if (typeof(response['data']) != 'undefined') {
            callback(response['data']);
          }
        }
        else {
          if (response.Status.code != 200) {
            callback(Drupal.gmaps.geocoder.decodeGoogleError(response));
          }
          else {
            rc.set(request, response.Placemark);
            callback(response.Placemark);
          }
        }
      },
      error: function (xmlhttp) {
        callback(Drupal.ahahError(xmlhttp, request));
      }
    });
  };
  
  this.parsePlacemark = function(placemark) {
    var parsed;
    if (parsed = pc.get(placemark.address)) {
      return parsed;
    }
    
    parsed = Drupal.gmaps.geocoder.parsePlacemark_(placemark);
    pc.set(placemark.address, parsed);
    return parsed;
  };

  this.setViewport = function(bounds) {
    if (!bounds) {
      ll = spn = null;
    }
    else if (bounds instanceof GLatLngBounds) {
      ll = bounds.getCenter().toUrlValue();
      spn = bounds.toSpan().toUrlValue();
    }
    else {
      var spanLat = bounds.northeast.latitude - bounds.southwest.latitude;
      spanLat = (Math.round(spanLat * 1e6)) / 1e6;

      var spanLng = bounds.northeast.longitude - bounds.southwest.longitude;
      spanLng = (Math.round(spanLng * 1e6)) / 1e6;
      
      spn = spanLat.toString() +','+ spanLng.toString();
      
      var llLat = bounds.southwest.latitude + spanLat / 2;
      llLat = (Math.round(llLat * 1e6)) / 1e6;

      var llLng = bounds.southwest.longitude + spanLng / 2;
      llLng = (Math.round(llLng * 1e6)) / 1e6;
      
      ll = llLat.toString() +','+ llLng.toString();
    }
    rc.clearAll();
    pc.clearAll();
  };
  
  this.setBaseCountryCode = function(countryCode) {
    gl = countryCode;
    rc.clearAll();
    pc.clearAll();
  };
  
  this.clearCache = function() {
    rc.clearAll();
    pc.clearAll();
  };
};


/**
 * Helper object to handling multiple geocoder results.
 * 
 * @param popup
 *  (required) jQuery object holding the popup container.
 * @param callback
 *  (required) Optional in the constructor, but required for proper functioning.
 */
GMapsGeocoderPopup = function (popup, callback) {
  popup.hide();

  var gap = this;
  var cb = callback;
  var selected, error;
  var populated = false;
  var accuracyLevels = Drupal.settings.gmaps.geocoder.accuracyLevels;
  var parent = popup.parent();
  var input = $('.form-gmaps-geocoder-query', parent)[0];
  var domain = $('.form-gmaps-geocoder-domain', parent)[0];
  var positioned = false;

  var getError = function() {
    if (error == null) {
      error = $(document.createElement('div')).addClass('error');
    }
    return error;
  };
  
  var setPosition = function() {
    if (!positioned) {
      popup.css({
        'marginTop': input.offsetHeight +'px',
        'width': (input.offsetWidth + domain.offsetWidth - 4) +'px'
      });
      positioned = true;
    }
  };
  
  this.setCallback = function(callback) {
    cb = callback;
  };

  this.populate = function(items) {
    var ul = document.createElement('ul');
    for (i in items) {
      var item = items[i];
      var li = document.createElement('li');
      $(li)
        .html('<div>'+ accuracyLevels[item.AddressDetails.Accuracy] + ' - ' + item.address +'</div>')
        .mousedown(function () { gap.select(this); gap.hide(); })
        .mouseover(function () { gap.highlight(this); })
        .mouseout(function () { gap.unhighlight(this); });
      li.placemark = item;
      $(ul).append(li);
    }
    if (ul.childNodes.length > 0) {
      setPosition();
      popup.empty().append(ul).show();
      populated = true;
    }
    else {
      selected = null;
      gap.hide();
    }
  };

  this.select = function (node) {
    selected = node;
  };

  this.selectDown = function () {
    if (selected && selected.nextSibling) {
      this.highlight(selected.nextSibling);
    }
    else {
      var lis = $('li', popup);
      if (lis.size() > 0) {
        this.highlight(lis[0]);
      }
    }
  };

  this.selectUp = function () {
    if (selected && selected.previousSibling) {
      this.highlight(selected.previousSibling);
    }
    else {
      var lis = $('li', popup);
      if (lis.size() > 0) {
        this.highlight(lis[lis.length - 1]);
      }
    }
  };

  this.highlight = function (node) {
    if (selected) {
      $(selected).removeClass('selected');
    }
    $(node).addClass('selected');
    selected = node;
  };

  this.unhighlight = function (node) {
    $(node).removeClass('selected');
    selected = false;
  };

  this.hide = function (keycode) {
    // Select item if the right key or mousebutton was pressed
    if (populated) {
      if (cb && selected && (!keycode || (keycode && keycode != 46 && keycode != 8 && keycode != 27))) {
        cb(selected.placemark);
      }
      selected = false;
      popup.hide().empty();
      populated = false;
    }
  };
  
  this.isPopulated = function() {
    return populated;
  };
  
  this.showError = function(error) {
    if (error) {
      setPosition();
      popup.append(getError().html(error)).show('normal', function() {setTimeout(function() {popup.hide().empty();}, 2000);});
    }
  };
  
};

GMapsGeocoderElement = function(item, client) {
  var gc = this;
  var cb, rc, ec, map, mapListener, marker, dragstartListener, dragendListener, lastLatLng;
  var inputChanged = false;
  var input = $('.form-gmaps-geocoder-query', item).change(function() {inputChanged = true;});
  var domain = $('.form-gmaps-geocoder-domain', item);
  var search = $('.form-gmaps-geocoder-search', item);
  var options = $('.gmaps-geocoder-options', item);

  var viewport = {'#type': 'checkbox', '#title': Drupal.t('Restrict search to viewport'), '#value': 0};
  viewport = $('<div/>').html(Drupal.render(viewport)).children().eq(0).css({display: 'inline-block'}).hide();
  options.append(viewport);
  var vpi = $(':checkbox', viewport);

  var reverseSearch = {'#type': 'checkbox', '#title': Drupal.t('Enable reverse search'), '#value': 0};
  reverseSearch = $('<div/>').html(Drupal.render(reverseSearch)).children().eq(0).css({display: 'inline-block'}).hide();
  options.append(reverseSearch);
  var rsi = $(':checkbox', reverseSearch);

  var popup = new GMapsGeocoderPopup($('.gmaps-geocoder-popup', item));
  
  client = client || Drupal.gmaps.geocoder.getGeocoder();
  
  item.parents('form').submit(function() {return (!inputChanged && !popup.isPopulated());});
  
  var setGeocoderBaseCountryCode = function(e) {
    client.setBaseCountryCode(domain.fieldValue()[0]);
  };
  
  var setClientViewport = function(e) {
    if (mapListener) {
      mapListener = GEvent.removeListener(mapListener);
    }
    if (vpi.attr('checked') && map) {
      client.setViewport(map.getBounds());
      mapListener = GEvent.addListener(map, 'moveend', function() {
        client.setViewport(map.getBounds());
      });
    }
    else {
      client.setViewport();
    }
  }
  
  var onKeyUp = function (input, e) {
    if (!e) {
      e = window.event;
    }
    switch (e.keyCode) {
      case 27: // esc
        if (popup.isPopulated()) {
          if (lastLatLng) {
            marker.setLatLng(lastLatLng);
            $(item).trigger('searchinterrupt', [lastLatLng]);
            lastLatLng = null;
          }
          popup.select(false);
          popup.hide();
        }
        return true;
      case 40: // down arrow
        if (popup.isPopulated()) {
          popup.selectDown();
          return false;
        }
        return true;
      case 38: // up arrow
        if (popup.isPopulated()) {
          popup.selectUp();
          return false;
        }
        return true;
      case 13: // enter
        lastLatLng = null;
        if (popup.isPopulated()) {
          popup.hide();
          return false;
        }
        else if (inputChanged) {
          gc.doSearch();
          return false;
        }
        return true;
      default: // all other keys
        return true;
    }
  };

  var setStatus = function(status) {
    switch (status) {
    case 'start':
      input.addClass('throbbing');
      //stop throbbing after 10s
      setTimeout(setStatus, 10000);
      break;
    default:
      input.removeClass('throbbing');
      break;
    }
  };
  
  var doCallback = function(placemark) {
    if (placemark.address != null) {
      input.val(placemark.address);
    }
    if (rc) {
      rc(placemark);
    }
    if (cb) {
      cb(client.parsePlacemark(placemark));
    }
  };
  
  var handleResponse = function(response, forcePopup) {
    setStatus();
    if (typeof(response) == 'string') {
      if (ec) {
        ec(response);
      }
      else {
        popup.showError(response);
      }
    }
    else {
      if (response.length == 1 && !forcePopup) {
        lastLatLng = null;
        doCallback(response[0]);
      } else {
        input[0].focus();
        popup.populate(response);
      }
    }
  };
  
  var handleResponsePopup = function(response) {
    handleResponse(response, true);
  };
  
  var setMarkerListener = function() {
    if (dragstartListener) {
      dragstartListener = GEvent.removeListener(dragstartListener);
    }
    if (dragendListener) {
      dragendListener = GEvent.removeListener(dragendListener);
    }
    if (marker && rsi.attr('checked')) {
      dragstartListener = GEvent.addListener(marker, 'dragstart', function(ll) {
        lastLatLng = ll;
      });
      dragendListener = GEvent.addListener(marker, 'dragend', function(ll) {
        gc.doSearch(ll, true);
      });
    }
  };
  
  this.getItem = function() {
    return item;
  };
  
  this.getInput = function() {
    return input;
  };
  
  this.getViewportItem = function() {
    return vpi;
  };
  
  this.getReverseSearchItem = function() {
    return rsi;
  };
  
  this.doSearch = function(query, forcePopup) {
    if (rc || cb) {
      var q;
      if (query) {
        q = (GLatLng != null && query instanceof GLatLng) ? query.toUrlValue() : query;
      }
      q = q || input.fieldValue()[0];
      if (q.length > 0) {
        setStatus('start');
        client.getLocations(q, forcePopup ? handleResponsePopup : handleResponse);
        inputChanged = false;
      }
    }
  };
  
  this.setCallback = function(callback) {
    cb = callback;
    return gc;
  };
  
  this.setRawCallback = function(rawCallback) {
    rc = rawCallback;
    return gc;
  };
  
  this.setErrorCallback = function(errorCallback) {
    ec = errorCallback;
    return gc;
  };
  
  this.setMap = function(gmap2) {
    map = gmap2;

    if (map == null) {
      vpi.removeAttr('checked');
      viewport.hide();
    }
    else {
      //vpi.attr('checked', 'checked');
      viewport.show();
    }
    
    setClientViewport();
    
    return gc;
  };

  this.setMarker = function(m) {
    marker = m;

    if (marker == null) {
      rsi.removeAttr('checked');
      reverseSearch.hide();
    }
    else {
      //rsi.attr('checked', 'checked');
      reverseSearch.show();
    }
    setMarkerListener();
    
    return gc;
  };

  this.showError = function(error) {
    if (error) {
      popup.showError(error);
    }
  };
  
  setGeocoderBaseCountryCode();
  
  this.setCallback();
  this.setMap();
  this.setMarker();
  
  popup.setCallback(doCallback);

  vpi.change(setClientViewport);
  rsi.change(setMarkerListener);
  domain.change(setGeocoderBaseCountryCode).keyup(setGeocoderBaseCountryCode);
  
  search.click(function() {
    gc.doSearch();
  });

  input
    .keyup(function (event) { return onKeyUp(this, event); })
    .blur(function () { popup.select(null); popup.hide(); inputChanged = false;})
    .keydown(function (event) { return (typeof(BUE) != 'undefined' && event.keyCode == 13) ? false : true});

}
