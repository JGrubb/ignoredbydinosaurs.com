// $Id: gmaps-point-element.js,v 1.1.2.8 2010/03/18 10:52:16 xmarket Exp $

Drupal.behaviors.gmapsPointElement = function(context) {
  $('.form-gmaps-point:not(.gmaps-point-embedded)', context).each(function(index, element) {
    var handler, init = function() {Drupal.gmaps.point.getElement(element.id, context, true);};
    if (handler = Drupal.gmaps.getLazyInitHandler('#'+ element.id, context)) {
      handler.attach('#'+ element.id, init, context);
    }
    else {
      setTimeout(init, 5);
    }
  });
};

/**
 * Global Point object.
 */
Drupal.gmaps.point = Drupal.gmaps.point || new (function() {
  var self = this, rc, elevationService;
  this.elements = {};
  
  this.behaviors = {};

  this.getResultCache = function() {
    if (rc == null) {
      rc = new GMapsCache();
    }
    return rc;
  };

  this.getElevationService = function() {
    if (Drupal.settings.gmaps.elevation.length) {
      elevationService = new GMapsElevationService();
    }
    return elevationService;
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
    var element = $('#'+ id +':not(.gmaps-point-processed)', context);
    if (element.length) {
      element = $(element[0]);
      element.addClass('gmaps-point-processed');
      self.elements[id] = new GMapsPointElement(element);
    }
    return self.elements[id];
  };

  this.attachBehaviors = function(p) {
    if (p) {
      $.each(self.behaviors, function() {
        this(p);
      });
    }
  };
})();

GMapsElevationService = function(resultCache) {
  var rc = resultCache || Drupal.gmaps.point.getResultCache();

  this.getElevation = function(point, callback) {
    if (!point || !callback) {
      return null;
    }
    
    var p = {};
    if (typeof(point) == 'GLatLng') {
      p.latitude = point.lat();
      p.longitude = point.lng();
    }
    else {
      p = point;
    }
    
    var request = 'http://ws.geonames.org/' + Drupal.settings.gmaps.elevation +'JSON?lat='+ p.latitude +'&lng='+ p.longitude;

    if (cached = rc.get(request)) {
      callback(cached);
      return;
    }
    
    $.ajax({
      type: "GET",
      url: request,
      dataType: 'jsonp',
      cache: true,
      success: function (response) {
        if (response.status) {
          callback(Drupal.t(response.status.message));
        }
        else {
          response = response[Drupal.settings.gmaps.elevation];
          rc.set(request, response);
          callback(response);
        }
      },
      error: function (xmlhttp) {
        callback(Drupal.ahahError(xmlhttp, request));
      }
    });
  };

  this.clearCache = function() {
    rc.clearAll();
  };
};

GMapsPointElement = function (item, opts, elevationService) {
  var gp = this, timers = {};
  var geocoder = $('.form-gmaps-geocoder', item);
  geocoder = geocoder.length ? Drupal.gmaps.geocoder.getElement(geocoder[0].id, item) : null;
  
  opts = $.extend(true, {}, opts || Drupal.settings.gmaps.point[item[0].id]);
  opts.input = typeof(opts.input) == 'boolean' ? opts.input : parseInt(opts.input, 10);
  
  var tabs = $('.form-gmaps-tabs', item);
  tabs = tabs.length ? Drupal.gmaps.tabs.getTabs(tabs[0].id, item) : null;
  
  var latitude = $('.form-gmaps-point-latitude:first', item);
  var longitude = $('.form-gmaps-point-longitude:first', item);
  
  var elevation = $('.form-gmaps-point-elevation', item), es, isESSearching = false;
  if (elevation.length) {
    if (Drupal.settings.gmaps.elevation.length) {
      es = elevationService || Drupal.gmaps.point.getElevationService();
    }
  }
  else {
    elevation = $('.gmaps-point-part-elevation input:hidden', item);
  }
  
  var bounds = $('.form-gmaps-point-bounds', item);
  bounds = bounds.length ? bounds : null;
  if (bounds) {
    bounds = {
      'northeast': new GMapsPointElement($('.form-gmaps-point-bounds-northeast', bounds), {input: false}),
      'southwest': new GMapsPointElement($('.form-gmaps-point-bounds-southwest', bounds), {input: false})
    };
  }

  var pov = $('.form-gmaps-point-pov', item);
  pov = pov.length ? pov : null;
  if (pov) {
    pov = {
      hasSVU: $('input.form-gmaps-point-pov-has-svu').eq(0),
      enabled: $('input.form-gmaps-point-pov-enabled').eq(0),
      yaw: $('input.form-gmaps-point-pov-yaw').eq(0),
      pitch: $('input.form-gmaps-point-pov-pitch').eq(0),
      zoom: $('select.form-gmaps-point-pov-zoom').eq(0)
    };
  }
  
  var gmi = $('.gmaps-map-item', item), marker, gmiSVU, searchListener, geocoderRSI, dragendListener,
    neMarker, neDragendListener, swMarker, swDragendListener, svuDragstartListener;
  gmi = gmi.length ? gmi = Drupal.gmaps.map.getMap(gmi[0].id, item, true) : null;
  
  item.parents('form').submit(function() {
    var ret = !isESSearching;
    if (ret && !opts.input) {
      latitude.removeAttr('disabled');
      longitude.removeAttr('disabled');
      if (pov) {
        pov.yaw.removeAttr('disabled');
        pov.pitch.removeAttr('disabled');
        pov.zoom.removeAttr('disabled');
      }
    }
    return ret;
  });

  var setElevation = function(e) {
    setStatus();
    if (isNaN(e)) {
      elevation.val(0);
      if (geocoder) {
        geocoder.showError(e);
      }
    }
    else {
      elevation.val(e);
    }
  };
  
  var doElevationSearch = function() {
    if (es && !isESSearching && elevation.hasClass('gmaps-point-elevation-autocomplete')) {
      var values = gp.getValues();
      if (values.latitude.length && values.longitude.length) {
        setStatus('start');
        es.getElevation(values, setElevation);
        return true;
      }
    }
    return false;
  };
  
  var setStatus = function(status) {
    if (es) {
      switch (status) {
      case 'start':
        elevation.addClass('throbbing');
        isESSearching = true;
        //stop throbbing after 30s
        //geonames free service is often very slow
        setTimeout(setStatus, 30000);
        break;
      default:
        elevation.removeClass('throbbing');
        elevation.removeClass('gmaps-point-elevation-autocomplete');
        isESSearching = false;
        break;
      }
    }
  };
  
  var onKeydown = function(e) {
    if (!e) {
      e = window.event;
    }
    //enter
    if (e.keyCode == 13) {
      return !doElevationSearch();
    }
  };
  
  var elevationAddES = function() {
    if (es) {
      elevation.val('');
      elevation.addClass('gmaps-point-elevation-autocomplete');
    }
  };
  
  var updateMarker = function(p) {
    if (marker) {
      if (p) {
        marker.setLatLng(new GLatLng(p.latitude, p.longitude));
        var b = bounds ? gp.getLatLngBounds() : Drupal.gmaps.map.getOverlayBounds({gmaps: {point : p}}, true);
        if (!marker.gmaps.gpDragging && !marker.gmaps.svuInitialized && b) {
          gmi.gmap2.setCenter(marker.getLatLng(), gmi.gmap2.getBoundsZoomLevel(b));
        }
        else {
          gmi.gmap2.setCenter(marker.getLatLng());
        }
        marker.gmaps.gpDragging = false;
        marker.show();
        if (gmiSVU && !marker.gmaps.svuInitialized) {
          gmiSVU.setLatLng(marker.getLatLng());
        }
      }
      else {
        marker.hide();
        //gmi.gmap2.setCenter(new GLatLng(0, 0), 0);
        if (gmiSVU) {
          gmiSVU.hide();
        }
      }
    }
  };
  
  var setMarkerListener = function() {
    if (dragendListener) {
      dragendListener = GEvent.removeListener(dragendListener);
    }
    if (svuDragstartListener) {
      svuDragstartListener = GEvent.removeListener(svuDragstartListener);
    }
    
    if (marker && (!geocoderRSI || !geocoderRSI.attr('checked'))) {
      dragendListener = GEvent.addListener(marker, 'dragend', function(ll) {
        gp.updateValues(ll, true);
      });
    }
    svuDragstartListener = GEvent.addListener(marker, 'dragstart', function(ll) {
      marker.gmaps.gpDragging = true;
    });
  };
  
  var setBoundLatLng = function(bound, ll) {
    if (bounds) {
      bounds[bound].setLatLng(ll);
      var b = bounds ? gp.getLatLngBounds() : Drupal.gmaps.map.getOverlayBounds(m);
      if (b) {
        gmi.gmap2.setCenter(b.getCenter(), gmi.gmap2.getBoundsZoomLevel(b));
      }
    }
  };
  
  var updateBoundMarker = function(m, ll) {
    if (m) {
      if (ll) {
        m.setLatLng(ll);
        m.show();
      }
      else {
        m.hide();
      }
    }
  };
  
  var onKeyupCoords = function(f, e, m) {
    if (timers[f]) {
      timers[f] = clearTimeout(timers[f]);
    }
    var p = gp.hasValues();
    if (p && (p[f] || p[f] === 0)) {
      if (!e) {
        e = window.event;
      }
      var val = p[f] || 0;
      switch (e.keyCode) {
        case 9: // tab
        case 13: // enter
        case 27: // esc
          return true;
        case 34: // page down
        //case 37: // left arrow
        case 40: // down arrow
          p[f] = val - 0.0001;
          break;
        case 33: // page up
        case 38: // up arrow
        //case 39: // right arrow
          p[f] = val + 0.0001;
          break;
      };
      timers[f] = setTimeout(function() {
        timers[f] = null;
        gp.updateValues(new GLatLng(p.latitude, p.longitude), m ? false : true );
        if (m) {
          updateBoundMarker(m, gp.getLatLng());
        }
      }, 500);
    }
  };
  
  var onKeyupPOV = function(f, e) {
    if (timers[f]) {
      timers[f] = clearTimeout(timers[f]);
    }
    var p = gp.getPOV();
    if (p[f] || p[f] === 0) {
      if (!e) {
        e = window.event;
      }
      var val = p[f] || 0;
      switch (e.keyCode) {
        case 9: // enter
        case 13: // enter
        case 27: // esc
          return true;
        case 34: // page down
        //case 37: // left arrow
        case 40: // down arrow
          p[f] = val - 1;
          break;
        case 33: // page up
        case 38: // up arrow
        //case 39: // right arrow
          p[f] = val + 1;
          break;
      };
      timers[f] = setTimeout(function() {
        timers[f] = null;
        gp.setPOV(p, true);
      }, 500);
    }
  };
  
  var addKeyboardHandlers = function() {
    if (gmi) {
      latitude.keyup(function(event) {onKeyupCoords('latitude', event);});
      longitude.keyup(function(event) {onKeyupCoords('longitude', event);});
      if (pov) {
        pov.yaw.keyup(function(event) {onKeyupPOV('yaw', event);});
        pov.pitch.keyup(function(event) {onKeyupPOV('pitch', event);});
        var func = function(event) {onKeyupPOV('zoom', event);};
        pov.zoom.keyup(func).change(func);
      }
    }
    if (bounds) {
      bounds.northeast.addBoundKeyboardHandlers(neMarker);
      bounds.southwest.addBoundKeyboardHandlers(swMarker);
    }
  };
  
  var initialize = function() {
    if (geocoder) {
      geocoder.setCallback(gp.setValues);
    }
    
    if (es) {
      latitude.change(elevationAddES);
      longitude.change(elevationAddES);
      elevation.keydown(function(event) {return onKeydown(event);});
    }
    
    if (gmi) {
      searchListener = GEvent.addListener(gmi.gmap2, 'dblclick', function(ov, ll) {
        if (geocoder && geocoderRSI && geocoderRSI.attr('checked')) {
          geocoder.doSearch(ll, true);
        }
        else {
          gp.updateValues(ll, true);
        }
      });
      
      marker = gmi.overlays.marker.input;
      
      var b;
      if (b = Drupal.gmaps.map.getOverlayBounds(marker)) {
        gmi.gmap2.setCenter(b.getCenter(), gmi.gmap2.getBoundsZoomLevel(b));
      }
      else {
        gmi.gmap2.setCenter(marker.getLatLng());
      }
      //delete marker.gmaps.point.bounds;

      gp.setGeocoder(geocoder, true);
      
      setMarkerListener();
      
      if (pov && (gmiSVU = gmi.getSVU())) {
        marker.gmaps.svu = {'dragendListener': dragendListener, 'rightclickListener': null};
        gmiSVU.setMarker(marker);
        
        GEvent.addListener(gmiSVU, 'gmapssvunosvu', function() {
          pov.hasSVU.val(0);
        });
        GEvent.addListener(gmiSVU, 'gmapssvulocationset', function(ll) {
          pov.hasSVU.val(1);
        });
        gmiSVU.setLatLng(gp.getLatLng(), gp.getPOV());
        
        if (gmiSVU.panorama.gmaps.initListener) {
          gmiSVU.panorama.gmaps.initListener = GEvent.removeListener(gmiSVU.panorama.gmaps.initListener);
        }
        gmiSVU.panorama.gmaps.initListener = GEvent.addListener(gmiSVU.panorama, "initialized", function(loc) {
          marker.gmaps.svuInitialized = true;
          gp.updateValues(loc.latlng, true);
          gp.setPOV(gmiSVU.panorama.getPOV());
          marker.gmaps.svuInitialized = false;
        });
        //only one listener works......
        if (gmiSVU.panorama.gmaps.yawListener) {
          gmiSVU.panorama.gmaps.yawListener = GEvent.removeListener(gmiSVU.panorama.gmaps.yawListener);
        }
        gmiSVU.panorama.gmaps.yawListener = GEvent.addListener(gmiSVU.panorama, "yawchanged", function(yaw) {
          gp.setPOV(gmiSVU.panorama.getPOV());
        }); 
        gmiSVU.panorama.gmaps.pitchListener = GEvent.addListener(gmiSVU.panorama, "pitchchanged", function(pitch) {
          gp.setPOV(gmiSVU.panorama.getPOV());
        }); 
        gmiSVU.panorama.gmaps.zoomListener = GEvent.addListener(gmiSVU.panorama, "zoomchanged", function(zoom) {
          gp.setPOV(gmiSVU.panorama.getPOV());
        }); 
      }
      
      if (bounds) {
        neMarker = gmi.overlays.marker.ne_bound;
        neDragendListener = GEvent.addListener(neMarker, 'dragend', function(ll) {
          setBoundLatLng('northeast', ll);
        });

        swMarker = gmi.overlays.marker.sw_bound;
        swDragendListener = GEvent.addListener(swMarker, 'dragend', function(ll) {
          setBoundLatLng('southwest', ll);
        });
      }
    }

    if (opts.input) {
      addKeyboardHandlers();
    }
    else {
      latitude.attr('disabled', true);
      longitude.attr('disabled', true);
      if (pov) {
        pov.yaw.attr('disabled', true);
        pov.pitch.attr('disabled', true);
        pov.zoom.attr('disabled', true);
      }
    }
  };
  
  this.setGeocoder = function(g, init) {
    if (geocoder && !init) {
      geocoder.setMap();
      geocoder.setMarker();
      
      geocoderRSI.unbind('change', setMarkerListener);
      geocoderRSI = null;
      geocoder = null;
    }
    if (g) {
      geocoder = g;
      if (gmi) {
        geocoder.setMap(gmi.gmap2);
        geocoder.setMarker(marker);
        
        geocoderRSI = geocoder.getReverseSearchItem();
        geocoderRSI.change(setMarkerListener);
      }
    }
    if (marker) {
      setMarkerListener();
    }
  };
  
  this.getItem = function() {
    return item;
  };
  
  this.hasValues = function() {
    var p = gp.getValues();
    return (
      p.latitude !== null && p.latitude !== '' &&
      p.longitude !== null && p.longitude !== '' &&
      p.elevation !== null && p.elevation !== ''
    ) ? p : null;
  };
  
  this.clearValues = function() {
    latitude.val('');
    longitude.val('');
    elevation.val('');
    if (bounds) {
      bounds.northeast.clearValues();
      bounds.southwest.clearValues();
    }
    updateMarker();
  };
  
  this.setValues = function(placemark) {
    if (!placemark) {
      return;
    }
    var point = (typeof(placemark.point) != 'undefined') ? placemark.point : placemark;
    point.latitude = (Math.round(point.latitude * 1e6)) / 1e6;
    point.longitude = (Math.round(point.longitude * 1e6)) / 1e6;
    latitude.val(point.latitude);
    longitude.val(point.longitude);
    if (bounds && point.bounds) {
      bounds.northeast.setValues(point.bounds.northeast);
      updateBoundMarker(neMarker, bounds.northeast.getLatLng());
      bounds.southwest.setValues(point.bounds.southwest);
      updateBoundMarker(swMarker, bounds.southwest.getLatLng());
    }
    if (es) {
      elevationAddES();
      doElevationSearch();
    }
    else {
      elevation.val(point.elevation);
    }
    updateMarker(point);
  };
  
  this.getValues = function() {
    var point = {
      'latitude': latitude.val(),
      'longitude': longitude.val(),
      'elevation': elevation.val()
    };
    point.latitude = (isNaN(point.latitude) || point.latitude === '') ? null : parseFloat(point.latitude);
    point.longitude = (isNaN(point.longitude) || point.longitude === '') ? null : parseFloat(point.longitude);
    point.elevation = (isNaN(point.elevation) || point.elevation === '') ? null : parseInt(point.elevation, 10);
    if (bounds) {
      point.bounds = {
        'northeast': bounds.northeast.getValues(),
        'southwest': bounds.southwest.getValues()
      };
    }
    return point;
  };
  
  this.setLatLng = function(ll) {
    latitude.val((Math.round(ll.lat() * 1e6)) / 1e6);
    longitude.val((Math.round(ll.lng() * 1e6)) / 1e6);
  };
  
  this.getLatLng = function() {
    if (typeof(GLatLng) != 'undefined') {
      var p = gp.getValues();
      return new GLatLng(p.latitude, p.longitude);
    }
  };
  
  this.getLatLngBounds = function() {
    if (bounds && typeof(GLatLngBounds) != 'undefined') {
      var ne = bounds.northeast.getLatLng();
      var sw = bounds.southwest.getLatLng();
      return new GLatLngBounds(sw, ne);
    }
  };
  
  this.updateValues = function(ll, m) {
    if (!ll) {
      return;
    }
    var p, dlat, dlng;
    if (p = gp.hasValues()) {
      dlat = (Math.round((ll.lat() - p.latitude) * 1e6)) / 1e6;
      dlng = (Math.round((ll.lng() - p.longitude) * 1e6)) / 1e6;
    }
    
    latitude.val((Math.round(ll.lat() * 1e6)) / 1e6);
    longitude.val((Math.round(ll.lng() * 1e6)) / 1e6);
    
    if (bounds) {
      var b = Drupal.gmaps.map.getOverlayBounds(marker ? marker : {gmaps: {point: gp.getValues()}});
      var ne = bounds.northeast.hasValues();
      if (ne && p) {
        ne.latitude = ne.latitude + dlat;
        ne.longitude = ne.longitude + dlng;
        bounds.northeast.setValues(ne);
      }
      else {
        bounds.northeast.setLatLng(b.getNorthEast());
      }
      updateBoundMarker(neMarker, bounds.northeast.getLatLng());

      var sw = bounds.southwest.hasValues();
      if (sw && p) {
        sw.latitude = sw.latitude + dlat;
        sw.longitude = sw.longitude + dlng;
        bounds.southwest.setValues(sw);
      }
      else {
        bounds.southwest.setLatLng(b.getSouthWest());
      }
      updateBoundMarker(swMarker, bounds.southwest.getLatLng());
    }

    if (m) {
      updateMarker(gp.getValues());
    }

    if (es) {
      elevationAddES();
      doElevationSearch();
    }
    else {
      elevation.val(0);
    }
  };
  
  this.setPOV = function(p, svu) {
    if (pov) {
      pov.yaw.val(p.yaw);
      pov.pitch.val(p.pitch);
      pov.zoom.val(p.zoom);
      if (svu) {
        gmiSVU.panorama.setPOV(gp.getPOV());
      }
    }
  };
  
  this.getPOV = function() {
    if (pov) {
      var p = {
        yaw: parseFloat(pov.yaw.val()),
        pitch: parseFloat(pov.pitch.val()),
        zoom: parseInt(pov.zoom.val())
      };
      return p;
    }
    return null;
  };
  
  this.addBoundKeyboardHandlers = function(m) {
    latitude.removeAttr('disabled');
    longitude.removeAttr('disabled');
    if (m) {
      latitude.keyup(function(event) {onKeyupCoords('latitude', event, m);});
      longitude.keyup(function(event) {onKeyupCoords('longitude', event, m);});
    }
  };
  
  initialize();
  
  Drupal.gmaps.point.attachBehaviors(this);
};
