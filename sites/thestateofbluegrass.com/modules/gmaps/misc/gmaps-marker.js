// $Id: gmaps-marker.js,v 1.1.4.6 2010/03/18 10:52:17 xmarket Exp $

GMapsMarkerActionHandlerLink = function(marker) {
  var toc, fs, acc, accHeader, accPanel, tabs, tabsPanel;
  
  var initialize = function() {
    if (marker.gmaps.options.link) {
      if (marker.gmaps.options.link.substr(0, 1) == '#') {
        toc = marker.gmaps.gmi.getOptions().base.map_data.toc_map;
        if (toc && parseInt(toc.enabled, 10)) {
          if (toc.style == 'fieldset') {
            var fs = $('#'+ marker.gmaps.options.link.substr(1), marker.gmaps.gmi.getContainer()).next().eq(0);
          }
          else if (toc.style == 'accordion') {
            acc = $('.gmaps-accordion', marker.gmaps.gmi.getContainer());
            var toc_id = acc.parent()[0].id;
            
            accHeader = Drupal.settings.gmaps.accordion[toc_id].options.header;
            accPanel = $('#'+ marker.gmaps.options.link.substr(1), acc).nextAll(accHeader).eq(0);
            var panel_id = accPanel.next()[0].id;
            
            if (Drupal.settings.gmaps.accordion[toc_id].ahahPanels && Drupal.settings.gmaps.accordion[toc_id].ahahPanels[panel_id]) {
              if (!Drupal.settings.gmaps.accordion[toc_id].ahahPanels[panel_id].data) {
                Drupal.settings.gmaps.accordion[toc_id].ahahPanels[panel_id].data = {};
              }
              Drupal.settings.gmaps.accordion[toc_id].ahahPanels[panel_id].data.marker = $.extend(true, Drupal.settings.gmaps.accordion[toc_id].ahahPanels[panel_id].data.marker || {}, marker.gmaps.options);
            }
          }
          else if (toc.style == 'tabs') {
            tabs = $('.gmaps-tabs', marker.gmaps.gmi.getContainer());
            var toc_id = tabs.parent()[0].id;
            
            tabsPanel = $('#'+ marker.gmaps.options.link.substr(1), tabs).next().eq(0);
            var panel_id = tabsPanel[0].id;
            
            if (Drupal.settings.gmaps.tabs[toc_id].ahahPanels && Drupal.settings.gmaps.tabs[toc_id].ahahPanels[panel_id]) {
              if (!Drupal.settings.gmaps.tabs[toc_id].ahahPanels[panel_id].data) {
                Drupal.settings.gmaps.tabs[toc_id].ahahPanels[panel_id].data = {};
              }
              Drupal.settings.gmaps.tabs[toc_id].ahahPanels[panel_id].data.marker = $.extend(true, Drupal.settings.gmaps.tabs[toc_id].ahahPanels[panel_id].data.marker || {}, marker.gmaps.options);
            }
          }
        }
      }
    }
  };
  
  var click = function() {
    if (marker.gmaps.options.link) {
      if (marker.gmaps.options.link.substr(0, 1) == '#') {
        if (toc && parseInt(toc.enabled, 10)) {
          if (toc.style == 'fieldset') {
            if (fs.hasClass('collapsed')) {
              $('legend a', fs).click();
            }
            if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
              window.location.hash = marker.gmaps.options.link;
            }
          }
          else if (toc.style == 'accordion') {
            if (!accPanel.hasClass('ui-state-active')) {
              if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
                acc.one('accordionchange', function() {
                  window.location.hash = marker.gmaps.options.link;
                });
              }
              acc.accordion('activate', $(accHeader, acc).index(accPanel));
            }
            else if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
              window.location.hash = marker.gmaps.options.link;
            }
          }
          else if (toc.style == 'tabs') {
            if (tabsPanel.hasClass('ui-tabs-hide')) {
              if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
                tabs.one('tabsshow', function() {
                  window.location.hash = marker.gmaps.options.link;
                });
              }
              tabs.tabs('select', tabsPanel[0].id);
            }
            else if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
              window.location.hash = marker.gmaps.options.link;
            }
          }
          //plain box
          else if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
            window.location.hash = marker.gmaps.options.link;
          }
        }
        else if (!window.location.hash || window.location.hash != marker.gmaps.options.link) {
          window.location.hash = marker.gmaps.options.link;
        }
      }
      else if (marker.gmaps.options.link_target == '_blank') {
        window.open(marker.gmaps.options.link);
      }
      else {
        window.location.href = marker.gmaps.options.link;
      }
    }
  };
  
  this.hover = function(ll) {
    click();
  };

  this.click = function(ll) {
    click();
  };

  this.dblclick = function(ll) {
    click();
  };

  this.rightclick = function(p, dom) {
    click();
  };
  
  initialize();
};

GMapsMarkerActionHandlerSvu = function(marker) {
  var pov = marker.gmaps.point.pov;
  var svu = marker.gmaps.gmi.getSVU();
  
  var showSvu = function() {
    if (svu && pov && parseInt(pov.has_svu, 10) && parseInt(pov.enabled, 10)) {
      svu.setLatLng(marker.gmaps.options.point);
    }
    else {
      $.each(marker.gmaps.action, function(e, refs) {
        if (refs.handler instanceof GMapsMarkerActionHandlerSvu) {
          GEvent.removeListener(marker.gmaps.action[e].listener);
          delete marker.gmaps.action[e];
        }
      });
    }
  };
  
  this.hover = function(ll) {
    showSvu();
  };

  this.click = function(ll) {
    showSvu();
  };

  this.dblclick = function(ll) {
    showSvu();
  };

  this.rightclick = function(p, dom) {
    showSvu();
  };
};

GMapsMarkerActionHandlerBlowup = function(marker) {
  var opts = {};
  
  var initialize = function() {
    var gmapsOpts = (marker.gmaps.options.iw_data && marker.gmaps.options.iw_data.google) || {};
    gmapsOpts.blowup = gmapsOpts.blowup || {};
    if (gmapsOpts.blowup.type && typeof(marker.gmaps.gmi.maptypes[gmapsOpts.blowup.type]) != 'undefined') {
      opts.mapType = marker.gmaps.gmi.maptypes[gmapsOpts.blowup.type];
    }
    if (parseInt(gmapsOpts.blowup.zoom) > -1) {
      opts.zoomLevel = parseInt(gmapsOpts.blowup.zoom);
    }
  };
  
  var showBlowup = function() {
    marker.showMapBlowup(opts);
  };
  
  this.hover = function(ll) {
    showBlowup();
  };

  this.click = function(ll) {
    showBlowup();
  };

  this.dblclick = function(ll) {
    showBlowup();
  };

  this.rightclick = function(p, dom) {
    showBlowup();
  };
  
  initialize();
};

GMapsMarkerActionHandlerContent = function(marker) {
  var iwHandler, content;
  
  var initialize = function() {
    if (marker.gmaps.gmi.gmap2.infoWindowEnabled() && marker.gmaps.gmi.iwHandlers[marker.gmaps.options.basic.info_window]) {
      iwHandler = marker.gmaps.gmi.iwHandlers[marker.gmaps.options.basic.info_window];
      content = $.extend(true, {}, marker.gmaps.options.content);
    }
  };
  
  var showContent = function() {
    marker.gmaps.gmi.bulletinBoardHide();
    if (iwHandler) {
      if (content.ajax != null && content.ajax.url != null && content.ajax.url.length) {
        var mapOpts = $.extend(true, {}, marker.gmaps.gmi.getOptions());
        delete mapOpts.overlays;
        var data = $.extend(true, {'marker': marker.gmaps.options, 'gmi': mapOpts}, content.ajax.data ? content.ajax.data : {});
        content.ajax.data = Drupal.gmaps.flatten(data);
        Drupal.gmaps.map.loadContent(content, function(c){content = c; iwHandler.openMarkerInfoWindow(marker, content);}, marker.gmaps.gmi, marker.getLatLng());
      }
      else {
        iwHandler.openMarkerInfoWindow(marker, content);
      }
    }
  };
  
  this.hover = function(ll) {
    showContent();
  };

  this.click = function(ll) {
    showContent();
  };

  this.dblclick = function(ll) {
    showContent();
  };

  this.rightclick = function(p, dom) {
    showContent();
  };
  
  initialize();
};

Drupal.gmaps.marker = Drupal.gmaps.marker || new (function() {
  var self = this;
  
  this.hooks = {
    action: {
      hover: {
        link: GMapsMarkerActionHandlerLink,
        svu: GMapsMarkerActionHandlerSvu,
        blowup: GMapsMarkerActionHandlerBlowup,
        content: GMapsMarkerActionHandlerContent
      },
      click: {
        link: GMapsMarkerActionHandlerLink,
        svu: GMapsMarkerActionHandlerSvu,
        blowup: GMapsMarkerActionHandlerBlowup,
        content: GMapsMarkerActionHandlerContent
      },
      dblclick: {
        link: GMapsMarkerActionHandlerLink,
        svu: GMapsMarkerActionHandlerSvu,
        blowup: GMapsMarkerActionHandlerBlowup,
        content: GMapsMarkerActionHandlerContent
      },
      rightclick: {
        link: GMapsMarkerActionHandlerLink,
        svu: GMapsMarkerActionHandlerSvu,
        blowup: GMapsMarkerActionHandlerBlowup,
        content: GMapsMarkerActionHandlerContent
      }
    }
  };
  
  var parseOpts = function(gmapsOpts) {
    gmapsOpts = gmapsOpts || {};
    var o = {};
    o.clickable = parseInt(gmapsOpts.clickable, 10);
    o.draggable = parseInt(gmapsOpts.draggable, 10);
    o.hide = parseInt(gmapsOpts.hide, 10);
    o.autoPan = parseInt(gmapsOpts.autopan, 10);
    o.dragCrossMove = parseInt(gmapsOpts.dcmove, 10);
    o.bouncy = parseInt(gmapsOpts.bouncy, 10);
    o.bounceGravity = parseFloat(gmapsOpts.gravity, 10);
    return o;
  };
  
  var parseTrackerOpts = function(gmapsOpts) {
    var o = {};
    o.iconScale = parseFloat(gmapsOpts.scale, 10);
    o.padding = parseInt(gmapsOpts.padding, 10);
    o.color = gmapsOpts.color;
    o.weight = parseInt(gmapsOpts.weight, 10);
    o.length = parseInt(gmapsOpts.length, 10);
    o.opacity = parseFloat(gmapsOpts.opacity, 10);
    o.updateEvent = gmapsOpts.update_event;
    o.quickPanEnabled = parseInt(gmapsOpts.quick_pan, 10) ? true : false;
    o.panEvent = gmapsOpts.pan_event;
    return o;
  };
  
  var createClusterer = function(gmi, markers, opts) {
    var o = {
      gridSize: parseInt(opts.gridsize, 10),
      styles: []
    };
    if (parseInt(opts.maxzoom, 10) > -1) {
      o.maxZoom = parseInt(opts.maxzoom, 10);
    }
    $.each(opts.styles, function() {
      if (parseInt(this.icon, 10)) {
        var s = {};
        var icon = Drupal.gmaps.icon.getIcon(this.icon);
        if (icon) {
          s.url = icon.image;
          s.width = icon.iconSize.width;
          s.height = icon.iconSize.height;
          if (this.color && this.color.length) {
            s.opt_textColor = this.color;
          }
          if (this.anchor) {
            s.opt_anchor = this.anchor;
          }
          o.styles.push(s);
        }
      }
    });
    
    return new MarkerClusterer(gmi.gmap2, markers, o);
  };
  
  this.createMarker = function(gmi, opts) {
    var o = parseOpts(opts.options), marker, point;
    
    o.icon = parseInt(opts.basic.icon, 10) ? Drupal.gmaps.icon.getIcon(parseInt(opts.basic.icon, 10)) : G_DEFAULT_ICON;
    
    if (opts.title) {
      o.title = opts.title;
    }
    
    point = new GLatLng(parseFloat(opts.point.latitude), parseFloat(opts.point.longitude));
    if (opts.labeled && opts.labeled.enabled) {
      o.labelText = opts.labeled.label;
      o.labelClass = opts.labeled['class'];
      o.labelOffset = new GSize(parseInt(opts.labeled.offset.x, 10), parseInt(opts.labeled.offset.y, 10));
      marker = new LabeledMarker(point, o);
    }
    else {
      marker = new GMarker(point, o);
    }
    
    return marker;
  };
  
  this.prepareMarker = function(marker, gmi, opts) {
    marker.gmaps = {
      gmi: gmi,
      options: $.extend(true, {}, opts)
    };
    marker.gmaps.point = marker.gmaps.options.point;
    
    if (opts.action) {
      marker.gmaps.action = {};
      $.each(opts.action, function(e, todo) {
        if (todo && Drupal.gmaps.marker.hooks.action[e][todo]) {
          marker.gmaps.action[e] = {handler: new (Drupal.gmaps.marker.hooks.action[e][todo])(marker)};
          if (e == 'rightclick') {
            marker.gmaps.action[e].listener = GEvent.addListener(gmi.gmap2, 'singlerightclick', function(p, dom, ov) {
              if (ov === marker) {
                if (marker.gmaps.rightclickTimer) {
                  marker.gmaps.rightclickTimer = clearTimeout(marker.gmaps.rightclickTimer);
                }
                marker.gmaps.rightclickTimer = setTimeout(function() {
                  marker.gmaps.rightclickTimer = clearTimeout(marker.gmaps.rightclickTimer);
                  marker.gmaps.action[e].handler[e](p, dom);
                }, 300);
              }
            });
          }
          else if (e == 'click') {
            marker.gmaps.action[e].listener = GEvent.addListener(marker, e, function(ll) {
              if (marker.gmaps.clickTimer) {
                marker.gmaps.clickTimer = clearTimeout(marker.gmaps.clickTimer);
              }
              marker.gmaps.clickTimer = setTimeout(function() {
                marker.gmaps.clickTimer = clearTimeout(marker.gmaps.clickTimer);
                marker.gmaps.action[e].handler[e](ll);
              }, 300);
            });
          }
          else if (e == 'dblclick') {
            marker.gmaps.action[e].listener = GEvent.addListener(marker, e, function(ll) {
              if (marker.gmaps.clickTimer) {
                marker.gmaps.clickTimer = clearTimeout(marker.gmaps.clickTimer);
              }
              marker.gmaps.action[e].handler[e](ll);
            });
          }
          else if (e == 'hover') {
            marker.gmaps.action[e].listener = GEvent.addListener(marker, 'mouseover', function(ll) {
              marker.gmaps.action[e].handler[e](ll);
            });
          }
        }
      });
    }
    
    if (opts.tracker && parseInt(opts.tracker.enabled, 10)) {
      marker.gmaps.tracker = new MarkerTracker(marker, gmi.gmap2, parseTrackerOpts(opts.tracker));
      marker.gmaps.trackerListener = GEvent.addListener(marker, 'visibilitychanged', function (isVisible) {
        if (isVisible) {
          marker.gmaps.tracker.enable();
        }
        else {
          marker.gmaps.tracker.disable();
        }
      });
    }
    
    return marker;
  };
  
  this.addContent = function(gmi, content) {
    if (!content);
    
    var markers = [];
    $.each(content, function(id, opts) {
      var marker = self.createMarker(gmi, opts);
      markers.push(self.prepareMarker(marker, gmi, opts));
    });
    
    if (markers.length) {
      if (!gmi.overlays.marker) {
        gmi.overlays.marker = {};
      }
      var ovData = gmi.getOptions().base.ov_data || {};
      var opts = ovData.marker || {}, route = [], b;
      var first_zoomed = false;
      
      if (!gmi.gmaps.marker) {
        gmi.gmaps.marker = {};
      }
      
      var clustered = [], managed = [];
      $.each(markers, function() {
        if (gmi.overlays.marker[this.gmaps.options.id]) {
          gmi.gmap2.removeOverlay(gmi.overlays.marker[this.gmaps.options.id]);
        }
        gmi.overlays.marker[this.gmaps.options.id] = this;
        
        if (opts && opts.route && parseInt(opts.route.enabled, 10) && (!this.gmaps.options.manager || !parseInt(this.gmaps.options.manager.skip_route, 10))) {
          route.push(this.getLatLng());
        }
        
        var direct = false;
        if (opts && opts.manager && opts.manager.manager && opts.manager.manager.length && (!this.gmaps.options.manager || !parseInt(this.gmaps.options.manager.skip_manager, 10))) {
          if (opts.manager.manager == 'clusterer') {
            clustered.push(this);
          }
          else {
            managed.push(this);
          }
          if (gmi.getOptions().base.methods.auto_center_zoom || (gmi.getOptions().base.methods.constructor == Array && $.inArray('auto_center_zoom', gmi.getOptions().base.methods.methods))) {
            var bounds = Drupal.gmaps.map.getOverlayBounds(this);
            
            if (bounds) {
              if (!gmi.gmap2.gmaps.aczBounds) {
                gmi.gmap2.gmaps.aczBounds = bounds;
              }
              else {
                gmi.gmap2.gmaps.aczBounds.extend(bounds.getSouthWest());
                gmi.gmap2.gmaps.aczBounds.extend(bounds.getNorthEast());
              }
            }
          }
        }
        else {
          gmi.gmap2.addOverlay(this);
        }
        
        //zoom to first
        if (!first_zoomed) {
          first_zoomed = true;
          if (opts.misc && parseInt(opts.misc.zoom_first, 10) && (b = Drupal.gmaps.map.getOverlayBounds(this, true))) {
            gmi.gmap2.setZoom(gmi.gmap2.getBoundsZoomLevel(b))
          }
        }
        
        if (this.gmaps.tracker) {
          if (this.isHidden()) {
            this.gmaps.tracker.disable();
          }
          else {
            this.gmaps.tracker.enable();
          }
        }
      });
      
      if (route.length) {
        if (gmi.gmaps.marker.route) {
          var i = gmi.gmaps.marker.route.getVertexCount();
          $.each(route, function(j, latlng) {
            gmi.gmaps.marker.route.insertVertex(i+j, latlng);
          });
        }
        else {
          gmi.gmaps.marker.route = new GPolyline(route, opts.route.color, parseInt(opts.route.weight, 10), parseFloat(opts.route.opacity, 10), {clickable: false, geodesic: parseInt(opts.route.geodesic, 10)});
          gmi.gmap2.addOverlay(gmi.gmaps.marker.route);
        }
      }
      
      if (clustered.length) {
        if (gmi.gmaps.marker.clusterer) {
          gmi.gmaps.marker.clusterer.addMarkers(clustered);
        }
        else {
          gmi.gmaps.marker.clusterer = createClusterer(gmi, clustered, opts.manager.clusterer);
        }
      }

      if (managed.length) {
        if (!gmi.gmaps.marker.manager) {
          var o = {
              trackMarkers: parseInt(opts.manager.mm.trackmarkers, 10),
              borderPadding: parseInt(opts.manager.mm.padding, 10),
              maxZoom: parseInt(opts.manager.mm.maxzoom, 10)
          };
          if (o.maxZoom < 0) delete o.maxZoom;
          gmi.gmaps.marker.manager = opts.manager.manager == 'google' ? new GMarkerManager(gmi.gmap2, o) : new MarkerManager(gmi.gmap2, o);
        }
        $.each(managed, function(i, marker) {
          var min, max;
          if (marker.gmaps.options.manager) {
            min = parseInt(marker.gmaps.options.manager.minzoom, 10);
            max = parseInt(marker.gmaps.options.manager.maxzoom, 10);
          }
          min = min || 0;
          max = (max && max > -1) ? max : null;
          gmi.gmaps.marker.manager.addMarker(this, min, max);
        });
        gmi.gmaps.marker.manager.refresh();
      }
    }
  };
})();

Drupal.gmaps.map.hooks.overlay.marker = Drupal.gmaps.marker;
