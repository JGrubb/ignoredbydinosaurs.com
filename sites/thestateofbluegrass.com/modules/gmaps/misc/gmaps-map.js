// $Id: gmaps-map.js,v 1.1.2.11 2010/03/18 10:52:16 xmarket Exp $

GMAPS_MAP_EARTH = 'earth';

GMAPS_MAP_MIN_HEIGHT = 50;
GMAPS_MAP_DEFAULT_ZOOM = 10;
GMAPS_MAP_LAZY_INIT_TIMEOUT = 500;
GMAPS_MAP_DEFAULT_BOUNDS_SPAN = 1;
GMAPS_MAP_SVU_MIN_HEIGHT = 100;
GMAPS_MAP_MAPTYPE_CONTROL = 'maptype';

GMAPS_MAP_ACZ_MODE_BOTH = 0;
GMAPS_MAP_ACZ_MODE_CENTER = 1;
GMAPS_MAP_ACZ_MODE_ZOOM = 2;

GMAPS_CONTROL_POSITION_DEFAULT = -1;

Drupal.behaviors.gmapsMapItem = function(context) {
  if (GBrowserIsCompatible()) {
    $('.gmaps-map-item span.loading', context).show();
    $('.gmaps-map-item:not(.gmaps-map-item-embedded)', context).each(function(index, element) {
      var handler, init = function() {Drupal.gmaps.map.getMap(element.id, context, true, true);};
      if (handler = Drupal.gmaps.getLazyInitHandler('#'+ element.id, context)) {
        handler.attach('#'+ element.id, init, context);
      }
      else {
        setTimeout(init, 5);
      }
    });
  }
};

Drupal.theme.prototype.gmapsInfoWindowContent = function(title, content) {
  var output = (title ? '<h3 class="gmaps-info-window-title">'+ title +'</h3>' : '');
  output += '<div class="gmaps-info-window-content">'+ content +'</div>';
  return output;
};

/**
 * GMap2 extensions
 */

/**
 * AdsManager method
 * @return
 */
GMap2.prototype.enableAdsManager = function() {
  if (!this.gmaps.adsManager) {
    var opts = this.gmaps.gmi.getOptions().base.method_data.adsmanager;
    var admOpts = {'style': G_ADSMANAGER_STYLE_ADUNIT, 'maxAdsOnMap': parseInt(opts.maxads), 'minZoomLevel': parseInt(opts.minzoom)};
    if (opts.channel.length) {
      admOpts.channel = opts.channel;
    }
    if (opts.position) {
      var anchor = opts.position.anchor ? parseInt(opts.position.anchor, 10) : GMAPS_CONTROL_POSITION_DEFAULT;
      if (anchor > GMAPS_CONTROL_POSITION_DEFAULT && opts.position.offset) {
        var offset = new GSize(parseInt(opts.position.offset.width, 10), parseInt(opts.position.offset.height, 10));
        admOpts.position = new GControlPosition(anchor, offset);
      }
    }
    this.gmaps.adsManager = new GAdsManager(this, opts.client, admOpts);
  }
  if (this.gmaps.adsManager) {
    this.gmaps.adsManager.enable();
  }
};
GMap2.prototype.disableAdsManager = function() {
  if (this.gmaps.adsManager) {
    this.gmaps.adsManager.disable();
  }
};

/**
 * AutoCenterZoom method
 */
GMap2.prototype.enableAutoCenterZoom = function() {
  if (!this.gmaps.gmi.isInitialized()) {
    this.gmaps.aczOverlayListener = GEvent.addListener(this, 'addoverlay', this._gmapsAczAddOverlay);
    
    this.gmaps.aczInitListener = GEvent.addListener(this.gmaps.gmi, 'initialized', this._gmapsAczInitialized);
  }
};

GMap2.prototype._gmapsAczAddOverlay = function(ov) {
  var bounds = Drupal.gmaps.map.getOverlayBounds(ov);
  
  if (bounds) {
    if (!this.gmaps.aczBounds) {
      this.gmaps.aczBounds = bounds;
    }
    else {
      this.gmaps.aczBounds.extend(bounds.getSouthWest());
      this.gmaps.aczBounds.extend(bounds.getNorthEast());
    }
  }
};

GMap2.prototype._gmapsAczInitialized = function() {
  GEvent.removeListener(this.gmap2.gmaps.aczOverlayListener);
  GEvent.removeListener(this.gmap2.gmaps.aczInitListener);
  if (this.gmap2.gmaps.aczBounds) {
    var aczOpts = this.gmap2.gmaps.gmi.getOptions().base.method_data;
    if (!aczOpts) {
      aczOpts = {auto_center_zoom: null};
    }
    var opts = aczOpts.auto_center_zoom;
    if (!opts) {
      opts = {mode: GMAPS_MAP_ACZ_MODE_BOTH};
    }
    var zoom = parseInt(this.gmap2.gmaps.gmi.getOptions().base.zoom, 10);
    if (opts.mode == GMAPS_MAP_ACZ_MODE_CENTER) {
      this.gmap2.setCenter(this.gmap2.gmaps.aczBounds.getCenter());
    }
    else if (opts.mode == GMAPS_MAP_ACZ_MODE_ZOOM) {
      if (zoom == -1) {
        this.gmap2.setZoom(this.gmap2.getBoundsZoomLevel(this.gmap2.gmaps.aczBounds));
      }
    }
    else {
      zoom = (zoom == -1) ? this.gmap2.getBoundsZoomLevel(this.gmap2.gmaps.aczBounds) : this.gmap2.getZoom();
      this.gmap2.setCenter(this.gmap2.gmaps.aczBounds.getCenter(), zoom);
    }
    delete this.gmap2.gmaps.aczMethod;
  }
};

/**
 * KeyboardHandler method
 */
GMap2.prototype.enableKeyboardHandler = function() {
  if (!this.gmaps.keyboardHandler) {
    this.gmaps.keyboardHandler = new GKeyboardHandler(this);
  }
};

/**
 * Layers method
 */
GMap2.prototype.enableLayers = function() {
  if (!this.gmaps.layers.length) {
    var gmap2 = this;
    jQuery.each(this.gmaps.gmi.getOptions().base.method_data.layers.layers, function(index, layerId) {
      var layer = new GLayer(layerId);
      gmap2.gmaps.layers[index] = layer;
      gmap2.addOverlay(layer);
    });
  }
};
GMap2.prototype.disableLayers = function() {
  if (this.gmaps.layers.length) {
    var gmap2 = this;
    jQuery.each(this.gmaps.layers, function() {
      gmap2.removeOverlay(this);
    });
    this.gmaps.layers = {};
  }
};

/**
 * Resize method
 */
GMap2.prototype.enableResize = function() {
  if (!this.gmaps.isAutoCenterZoomEnabled) {
    this.gmaps.isAutoCenterZoomEnabled = true;
    var gmap2 = this;  
    var cont = $('.gmaps-map-container-wrapper', this.gmaps.gmi.getContainer()), staticOffset = null;
  
    $(cont).wrap('<div class="resizable-gmaps-map-container"><span></span></div>')
    .parent().append($('<div class="grippie"></div>').mousedown(startDrag));
  
    var grippie = $('div.grippie', $(cont).parent())[0];
    grippie.style.marginRight = (grippie.offsetWidth - $(cont)[0].offsetWidth) +'px';
  }

  function startDrag(e) {
    staticOffset = cont.height() - e.pageY;
    cont.css('opacity', 0.25);
    $(document).mousemove(performDrag).mouseup(endDrag);
    return false;
  }

  function performDrag(e) {
    cont.height(Math.max(32, staticOffset + e.pageY) + 'px');
    $('.gmaps-map-container', cont).height(cont.height() + 'px');
    return false;
  }

  function endDrag(e) {
    $(document).unbind("mousemove", performDrag).unbind("mouseup", endDrag);
    cont.css('opacity', 1);
    var center = gmap2.getCenter();
    gmap2.checkResize();
    gmap2.setCenter(center);
  }
};

/**
 * Street view method
 */
GMap2.prototype.enableStreetView = function() {
  if (!this.gmaps.streetView.overlay) {
    var gmap2 = this;
    this.gmaps.streetView.overlay = new GStreetviewOverlay();
    this.addOverlay(this.gmaps.streetView.overlay);
    
    if (parseInt(this.gmaps.gmi.getOptions().base.method_data.streetview.opensvu)) {
      this.gmaps.streetView.listener = GEvent.addListener(this, 'click', function(ov, ll, ovll) {
        if (!ov && !gmap2.gmaps.streetView.timer) {
          gmap2.gmaps.streetView.timer = setTimeout(function() {
            gmap2.gmaps.streetView.timer = null;
            gmap2.gmaps.gmi.getSVU().setLatLng(ll);
          }, 500);
        }
      });
      if (this.gmaps.gmi.getOptions().base.methods['dblclickzoom']) {
        this.gmaps.streetView.zoomListener = GEvent.addListener(this, 'dblclick', function(ov, ll) {
          if (gmap2.gmaps.streetView.timer) {
            gmap2.gmaps.streetView.timer = clearTimeout(gmap2.gmaps.streetView.timer);
          }
        });
      }
    }
  }
};
GMap2.prototype.disableStreetView = function() {
  if (this.gmaps.streetView.overlay) {
    this.removeOverlay(this.gmaps.streetView.overlay);
    this.gmaps.streetView.overlay = null;
    
    if (this.gmaps.streetView.listener) {
      GEvent.removeListener(this.gmaps.streetView.listener);
      this.gmaps.streetView.listener = null;
    }
    if (this.gmaps.streetView.zoomListener) {
      GEvent.removeListener(this.gmaps.streetView.zoomListener);
      this.gmaps.streetView.zoomListener = null;
    }
  }
};

/**
 * Traffic method
 */
GMap2.prototype.enableTraffic = function() {
  if (!this.gmaps.traffic) {
    this.gmaps.traffic = new GTrafficOverlay({'incidents': parseInt(this.gmaps.gmi.getOptions().base.method_data.traffic.incidents) ? true : false});
    this.addOverlay(this.gmaps.traffic);
  }
};
GMap2.prototype.disableTraffic = function() {
  if (this.gmaps.traffic) {
    this.removeOverlay(this.gmaps.traffic);
    this.gmaps.traffic = null;
  }
};

/**
 * KeyDragZoom method
 */
GMap2.prototype.enableGMapsKeyDragZoom = function() {
  if (this.enableKeyDragZoom) {
    var opts = this.gmaps.gmi.getOptions().base.method_data.keydragzoom;
    this.enableKeyDragZoom({
      key: opts.key,
      border: opts.border
    });
  }
};
GMap2.prototype.disableGMapsKeyDragZoom = function() {
  if (this.disableKeyDragZoom) {
    this.disableKeyDragZoom();
  }
};

/**
 * Reference implementation of MapTypeHandler "interface".
 */
GMapsMapTypeHandler = function() {
  this.createMapType = function(gmi, type) {
    if (type == 'normal') {
      return G_NORMAL_MAP;
    }
    else if (type == 'satellite') {
      return G_SATELLITE_MAP;
    }
    else if (type == 'hybrid') {
      return G_HYBRID_MAP;
    }
    else if (type == 'physical') {
      return G_PHYSICAL_MAP;
    }
    else if (type == 'satellite_3D') {
      return G_SATELLITE_3D_MAP;
    }
    else if (type == 'aerial') {
      return G_AERIAL_MAP;
    }
    else if (type == 'aerial_hybrid') {
      return G_AERIAL_HYBRID_MAP;
    }
    else if (type == 'mapmaker_normal') {
      return G_MAPMAKER_NORMAL_MAP;
    }
    else if (type == 'mapmaker_hybrid') {
      return G_MAPMAKER_HYBRID_MAP;
    }
    else if (type == 'moon_elevation') {
      return G_MOON_ELEVATION_MAP;
    }
    else if (type == 'moon_visible') {
      return G_MOON_VISIBLE_MAP;
    }
    else if (type == 'mars_elevation') {
      return G_MARS_ELEVATION_MAP;
    }
    else if (type == 'mars_visible') {
      return G_MARS_VISIBLE_MAP;
    }
    else if (type == 'mars_infrared') {
      return G_MARS_INFRARED_MAP;
    }
    else if (type == 'sky_visible') {
      return G_SKY_VISIBLE_MAP;
    }
  };
};

/**
 * Reference implementation of ControlHandler "interface".
 */
GMapsControlHandler = function() {
  var extDragZoomOverwritten = false;
  
  var overwriteExtDragZoom = function() {
    if (!extDragZoomOverwritten) {
      extDragZoomOverwritten = true;
      DragZoomControl.prototype.initButton_ = function(buttonContainerDiv) {
        var G = this.globals;
        var buttonDiv = document.createElement('div');
        buttonDiv.innerHTML = G.options.buttonHTML;
        DragZoomUtil.style([buttonDiv], {cursor: 'pointer', zIndex:200});
        $(buttonDiv).addClass('gmaps-map-control-extdragzoom-starting '+ G.gmaps.style.starting);
        $(buttonDiv).addClass('gmaps-map-control-extdragzoom-button '+ G.gmaps.style.button);
        buttonContainerDiv.appendChild(buttonDiv);
        return buttonDiv;
      };
      
      DragZoomControl.prototype.initBackButton_ = function(buttonContainerDiv) {
        var G = this.globals;
        var backButtonDiv = document.createElement('div');
        backButtonDiv.innerHTML = G.options.backButtonHTML;
        DragZoomUtil.style([backButtonDiv], {cursor: 'pointer', zIndex:200});
        $(backButtonDiv).addClass('gmaps-map-control-extdragzoom-starting '+ G.gmaps.style.starting);
        $(backButtonDiv).addClass('gmaps-map-control-extdragzoom-back '+ G.gmaps.style.back);
        buttonContainerDiv.appendChild(backButtonDiv);
        return backButtonDiv;
      };
      
      DragZoomControl.prototype.setButtonMode_ = function(mode){
        var G = this.globals;
        if (mode == 'zooming') {
          G.buttonDiv.innerHTML = G.options.buttonZoomingHTML;
          $(G.buttonDiv).addClass('gmaps-map-control-extdragzoom-zooming '+ G.gmaps.style.zooming);
          $(G.buttonDiv).removeClass('gmaps-map-control-extdragzoom-button '+ G.gmaps.style.button);
        } else {
          G.buttonDiv.innerHTML = G.options.buttonHTML;
          $(G.buttonDiv).addClass('gmaps-map-control-extdragzoom-button '+ G.gmaps.style.button);
          $(G.buttonDiv).removeClass('gmaps-map-control-extdragzoom-zooming '+ G.gmaps.style.zooming);
        }
      };
    }
  };
  
  this.createControl = function(gmi, type) {
    if (type == 'smallzoom') {
      return new GSmallZoomControl();
    }
    else if (type == 'smallmap') {
      return new GSmallMapControl();
    }
    else if (type == 'largemap') {
      return new GLargeMapControl();
    }
    else if (type == 'smallzoom3D') {
      return new GSmallZoomControl3D();
    }
    else if (type == 'largemap3D') {
      return new GLargeMapControl3D();
    }
    else if (type == 'extnav') {
      var opts = gmi.getOptions().base.control_data.extnav;
      var exOpts = {'type': opts.type};
      exOpts.moveEastBtnTitle = opts.title.east;
      exOpts.moveWestBtnTitle = opts.title.west;
      exOpts.moveNorthBtnTitle = opts.title.north;
      exOpts.moveSouthBtnTitle = opts.title.south;
      exOpts.returnBtnTitle = opts.title['return'];
      exOpts.zoomInBtnTitle = opts.title.zoomin;
      exOpts.zoomOutBtnTitle = opts.title.zoomout;
      return new ExtLargeMapControl(exOpts);
    }
    else if (type == 'scale') {
      return new GScaleControl();
    }
    else if (type == 'maptype') {
      return new GMapTypeControl(parseInt(gmi.getOptions().base.control_data.maptype.shortnames) ? true : false);
    }
    else if (type == 'menumaptype') {
      return new GMenuMapTypeControl(parseInt(gmi.getOptions().base.control_data.menumaptype.shortnames) ? true : false);
    }
    else if (type == 'hierarchical_maptype') {
      return new GHierarchicalMapTypeControl();
    }
    else if (type == 'extmaptype') {
      var opts = gmi.getOptions().base.control_data.extmaptype;
      var opt_opts = {
        'showTraffic': parseInt(opts.traffic) ? true : false,
        'showTrafficKey': parseInt(opts.traffickey) ? true : false,
        'showMore': parseInt(opts.more) ? true : false,
        'showSave': parseInt(opts.save) ? true : false
      };
      if (typeof(opts.maptype) == 'undefined' || !opts.maptype || opts.maptype == 'extmaptype') {
        opt_opts.useMapTypes = true;
      }
      else {
        var ct_opts = $.extend(true, {}, gmi.getOptions().base.controls.maptype);
        ct_opts.control = opts.maptype;
        if (typeof(opts.control_data) != 'undefined') {
          gmi.getOptions().base.control_data[opts.maptype] = opts.control_data;
        }
        gmi.addControl('extmaptype_maptype', ct_opts);
        opt_opts.posRight = opts.pos_right ? parseInt(opts.pos_right) : 100;
      }
        
      var control = new ExtMapTypeControl(opt_opts);
      
      if (!opt_opts.useMapTypes) {
        control.gmaps = {'maptype': gmi.controls['extmaptype_maptype']};
      }
      
      return control;
    }
    else if (type == 'overview') {
      return new GOverviewMapControl();
    }
    else if (type == 'extdragzoom') {
      var opts = gmi.getOptions().base.control_data.extdragzoom;
      var boxStyle = {
        'fillColor': opts.boxstyle.fillcolor,
        'border': opts.boxstyle.border,
        'opacity': parseFloat(opts.boxstyle.opacity)
      };
      var others = {
        'buttonHTML': opts.other.button_html,
        'buttonZoomingHTML': opts.other.zooming_html,
        'backButtonHTML': opts.other.back_html,
        'backButtonEnabled': parseInt(opts.other.back_button),
        'overlayRemoveTime': parseInt(opts.other.overlay_time),
        'stickyZoomEnabled': parseInt(opts.other.sticky_zoom),
        'rightMouseZoomOutEnabled': parseInt(opts.other.zoom_out),
        'minDragSize': parseInt(opts.other.dragsize)
      };

      overwriteExtDragZoom();
      
      var control = new DragZoomControl(boxStyle, others);
      control.globals.gmaps = {'style': opts.other.style};

      return control;
    }
    else if (type == 'breadcrumb') {
      var opts = gmi.getOptions().base.control_data.breadcrumb;
      if (!opts) {
        opts = {level: 0};
      }
      opts.level = parseInt(opts.level);
      
      var control = new GNavLabelControl();
      
      if (opts.level > 0) {
        control.setMinAddressLinkLevel(opts.level);
      }
      
      return control;
    }
  };
};

/**
 * Reference implementation of interface "InfoWindowHandler"
 * for API info window
 */
GMapsInfoWindowHandlerGoogle = function(gmi) {
  var self = this, opts = {};
  
  var parseOpts = function(gmapsOpts) {
    var o = {};
    o.maxWidth = parseInt(gmapsOpts.maxwidth, 10);
    o.noCloseOnClick = parseInt(gmapsOpts.nocloseonclick, 10);
    o.pixelOffset = new GSize(parseInt(gmapsOpts.offset.x, 10), parseInt(gmapsOpts.offset.y, 10));
    o.maximized = parseInt(gmapsOpts.maximized, 10);
    if (gmapsOpts.blowup.type && gmapsOpts.blowup.type.length && typeof(gmi.maptypes[gmapsOpts.blowup.type]) != 'undefined') {
      o.mapType = gmi.maptypes[gmapsOpts.blowup.type];
    }
    if (parseInt(gmapsOpts.blowup.zoom, 10) > -1) {
      o.zoomLevel = parseInt(gmapsOpts.blowup.zoom, 10);
    }
    return o;
  };
  
  var initialize = function() {
    opts = parseOpts(gmi.getOptions().base.iw_data.google);
  };
  
  this.openInfoWindow = function(latlng, content, iwOpts) {
    var currentOpts = $.extend(true, {}, opts, iwOpts ? iwOpts : {});
    var minContent, maxContent, tabs = [], opened = false;
    if (typeof(content.max) != 'undefined' && content.max !== null && content.max.length) {
      maxContent = $('<div/>').html(content.max)[0];
      Drupal.attachBehaviors(maxContent);
      currentOpts.maxContent = maxContent;
    }
    if (!currentOpts.maxTitle && content.title) {
      currentOpts.maxTitle = content.title;
    }
    
    if (typeof(content.tabs) != 'undefined') {
      $.each(content.tabs, function(label, tabContent) {
        tabContent = $('<div/>').html(tabContent)[0];
        Drupal.attachBehaviors(tabContent);
        tabs.push(new GInfoWindowTab(label, tabContent));
      });
      if (tabs.length) {
        gmi.gmap2.openInfoWindowTabs(latlng, tabs, currentOpts);
        if (maxContent) opened = true;
      }
    }
    else if (typeof(content.min) != 'undefined' && content.min !== null && content.min.length) {
      delete currentOpts.selectedTab;
      minContent = $('<div/>').html(Drupal.theme('gmapsInfoWindowContent', content.title, content.min))[0];
      Drupal.attachBehaviors(minContent);
      gmi.gmap2.openInfoWindow(latlng, minContent, currentOpts);
      if (maxContent) opened = true;
    }
    else if (maxContent) {
      delete currentOpts.maxContent;
      if (currentOpts.maxTitle) {
        maxContent = $('<div/>').html(Drupal.theme('gmapsInfoWindowContent', currentOpts.maxTitle, content.max))[0];
        delete currentOpts.maxTitle;
      }
      gmi.gmap2.openInfoWindow(latlng, maxContent, currentOpts);
    }
    
    if (opened && currentOpts.maximized) {
      var iw = gmi.gmap2.getInfoWindow();
      var m = GEvent.addListener(gmi.gmap2, 'infowindowopen', function () {
        GEvent.removeListener(m);
        iw.maximize();
      });
    }
  };
  
  this.openMarkerInfoWindow = function(marker, content) {
    var currentOpts;
    if (marker.gmaps.options.iw_data && marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]) {
      currentOpts = $.extend(true, {}, opts, parseOpts(marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]));
    }
    else {
      currentOpts = $.extend(true, {}, opts);
    }
    var minContent, maxContent, tabs = [], opened = false;
    if (typeof(content.max) != 'undefined' && content.max !== null && content.max.length) {
      maxContent = $('<div/>').html(content.max)[0];
      Drupal.attachBehaviors(maxContent);
      currentOpts.maxContent = maxContent;
    }
    if (!currentOpts.maxTitle && content.title) {
      currentOpts.maxTitle = content.title;
    }
    
    if (typeof(content.tabs) != 'undefined') {
      $.each(content.tabs, function(label, tabContent) {
        tabContent = $('<div/>').html(tabContent)[0];
        Drupal.attachBehaviors(tabContent);
        tabs.push(new GInfoWindowTab(label, tabContent));
      });
      if (tabs.length) {
        marker.openInfoWindowTabs(tabs, currentOpts);
        if (maxContent) opened = true;
      }
    }
    else if (typeof(content.min) != 'undefined' && content.min !== null && content.min.length) {
      delete currentOpts.selectedTab;
      minContent = $('<div/>').html(Drupal.theme('gmapsInfoWindowContent', content.title, content.min))[0];
      Drupal.attachBehaviors(minContent);
      marker.openInfoWindow(minContent, currentOpts);
      if (maxContent) opened = true;
    }
    else if (maxContent) {
      delete currentOpts.maxContent;
      if (currentOpts.maxTitle) {
        maxContent = $('<div/>').html(Drupal.theme('gmapsInfoWindowContent', currentOpts.maxTitle, content.max))[0];
        delete currentOpts.maxTitle;
      }
      marker.openInfoWindow(maxContent, currentOpts);
    }
    
    if (opened && currentOpts.maximized) {
      var iw = marker.gmaps.gmi.gmap2.getInfoWindow();
      var m = GEvent.addListener(marker.gmaps.gmi.gmap2, 'infowindowopen', function () {
        GEvent.removeListener(m);
        iw.maximize();
      });
    }
  };
  
  initialize();
};

/**
 * Reference implementation of interface "InfoWindowHandler"
 * for Bulletin board info window
 */
GMapsInfoWindowHandlerBulletinBoard = function(gmi) {
  var self = this, opts;
  
  var parseOpts = function(gmapsOpts) {
    var o = $.extend(true, {}, gmapsOpts);
    o.collapsible = parseInt(o.collapsible, 10);
    o.collapsed = parseInt(o.collapsed, 10);
    o.tabs_state = o.tabs_state || 'max';
    o.maximized = o.maximized ? parseInt(o.maximized, 10) : 0;
    return o;
  };
  
  var initialize = function() {
    opts = parseOpts(gmi.getOptions().base.iw_data.bb);
  };
  
  var showContent = function(content, currentOpts) {
    gmi.gmap2.closeInfoWindow();

    var minContent, maxTitle, maxContent, tabs, hasMin, hasMax;
    
    hasMin = typeof(content.min) != 'undefined' && content.min !== null && content.min.length;
    hasMax = typeof(content.max) != 'undefined' && content.max !== null && content.max.length;
    
    if (!hasMin && hasMax) {
      content.min = content.max;
      currentOpts.tabs_state = 'min';
      delete content.max;
    }
    
    if (typeof(content.tabs) != 'undefined') {
      if (currentOpts.style == 'plain') {
        tabs = Drupal.gmaps.map.renderTabsPlain(content.tabs);
      }
      else if (currentOpts.style == 'list') {
        tabs = Drupal.gmaps.map.renderTabsList(content.tabs);
      }
      else if (currentOpts.style == 'grid') {
        tabs = Drupal.gmaps.map.renderTabsList(content.tabs, currentOpts);
      }
      else if (currentOpts.style == 'box') {
        tabs = Drupal.gmaps.map.renderTabsBox(content.tabs);
      }
      else if (currentOpts.style == 'fieldset') {
        tabs = Drupal.gmaps.map.renderTabsFieldset(content.tabs, currentOpts);
      }
      else if (currentOpts.style == 'accordion') {
        tabs = Drupal.gmaps.map.renderTabsAccordion(content.tabs, currentOpts);
      }
      else if (currentOpts.style == 'tabs') {
        tabs = Drupal.gmaps.map.renderTabsTabs(content.tabs, currentOpts);
      }
      
      if (tabs) {
        currentOpts.tabs = true;
      }
    }
    
    if (hasMin) {
      minContent = Drupal.theme('gmapsInfoWindowContent', content.title, content.min);
      if (tabs && currentOpts.tabs_state == 'min') {
        minContent += tabs;
      }
      if (hasMax) {
        minContent = '<div class="gmaps-bbinfowindow-minimize"></div><div class="gmaps-bbinfowindow-maximize"></div><div id="'+ gmi.getContainer()[0].id +'-bbinfowindow-mincontent">'+ minContent +'</div>';
      }
    }
    
    if (hasMax) {
      maxTitle = currentOpts.maxTitle || content.title;
      maxContent = Drupal.theme('gmapsInfoWindowContent', maxTitle, content.max);
      if (tabs && currentOpts.tabs_state == 'max') {
        maxContent += tabs;
      }
      maxContent = '<div id="'+ gmi.getContainer()[0].id +'-bbinfowindow-maxcontent">'+ maxContent +'</div>';
    }
    else {
      currentOpts.maximized = 0;
    }
    
    var c = (minContent || '') + (maxContent || '');
    if (c.length) {
      c = $('<div/>').html(c)[0];
      gmi.bulletinBoardShow(c);
      var bb = gmi.getBulletinBoard();
      if (hasMax) {
        addStateListeners(bb);
        switchState(currentOpts.maximized ? 'max' : 'min', bb);
      }
      if (gmi.gmap2.gmaps.bbInfoWindowListener == null) {
        gmi.gmap2.gmaps.bbInfoWindowListener = GEvent.addListener(gmi.gmap2, 'infowindowopen', function() {
          gmi.bulletinBoardHide();
        });
      }
      //GEvent.trigger(gmi.gmap2, 'bbinfowindowopen');
    }
  };
  
  var switchState = function(toState, c) {
    var id = '#'+ gmi.getContainer()[0].id;
    if (toState == 'min') {
      $('.gmaps-bbinfowindow-minimize', c).hide();
      $('.gmaps-bbinfowindow-maximize', c).show();
      $(id +'-bbinfowindow-mincontent', c).show();
      $(id +'-bbinfowindow-maxcontent', c).hide();
    }
    else if (toState == 'max') {
      $('.gmaps-bbinfowindow-minimize', c).show();
      $('.gmaps-bbinfowindow-maximize', c).hide();
      $(id +'-bbinfowindow-mincontent', c).hide();
      $(id +'-bbinfowindow-maxcontent', c).show();
    }
  };
  
  var addStateListeners = function(c) {
    $('.gmaps-bbinfowindow-minimize', c).click(function() {
      switchState('min', c);
    });
    $('.gmaps-bbinfowindow-maximize', c).click(function() {
      switchState('max', c);
    });
  };
  
  this.openInfoWindow = function(latlng, content, iwOpts) {
    var currentOpts = $.extend(true, {}, opts, iwOpts ? iwOpts : {});
    showContent(content, currentOpts);
  };
  
  this.openMarkerInfoWindow = function(marker, content) {
    var currentOpts;
    if (marker.gmaps.options.iw_data && marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]) {
      currentOpts = $.extend(true, {}, opts, parseOpts(marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]));
    }
    else {
      currentOpts = $.extend(true, {}, opts);
    }
    showContent(content, currentOpts);
  };
  
  initialize();
};

/**
 * Reference implementation of interface "InfoWindowHandler"
 * for Extended info window
 */
GMapsInfoWindowHandlerExtended = function(gmi) {
  var self = this, opts;
  
  var parseOpts = function(gmapsOpts) {
    var o = $.extend(true, {}, gmapsOpts);
    o.collapsible = parseInt(o.collapsible, 10);
    o.collapsed = parseInt(o.collapsed, 10);
    o.beakOffset = parseInt(o.beak_offset, 10);
    delete o.beak_offset;
    if (o.padding) {
      o.paddingX = parseInt(o.padding.x, 10);
      o.paddingY = parseInt(o.padding.y, 10);
      delete o.padding;
    }
    if (o.max_panning != null) {
      o.maxPanning = parseInt(o.max_panning, 10);
      delete o.max_panning;
    }
    if (o.nocloseonclick != null) {
      o.noCloseOnClick = parseInt(o.nocloseonclick, 10) ? true : false;
      delete o.nocloseonclick;
    }
    return o;
  };
  
  var initialize = function() {
    opts = parseOpts(gmi.getOptions().base.iw_data.extended);
    
    GEvent.addListener(gmi.gmap2, 'extinfowindowopen', function() {
      var iw = gmi.gmap2.getExtInfoWindow();
      var cssClass = opts['class'] ? opts['class'] : 'gmaps-extinfowindow';
      $('#'+ iw.container_.id).addClass(cssClass);
      iw.container_.style.width = '';
      var containerWidth  = iw.getStyle_(document.getElementById(iw.container_.id), 'width');
      iw.container_.style.width = (containerWidth == null ? iw.defaultStyles.containerWidth : containerWidth);
      iw.contentWidth = iw.getDimensions_(iw.container_).width;
      iw.contentDiv_.style.width = iw.contentWidth + 'px';
      $(iw.contentDiv_).addClass(cssClass + '-contents');
      
      for (var i in iw.wrapperParts ) {
        var tempElement = document.createElement('div');
        tempElement.id = 'gmaps_'+ iw.infoWindowId_ + '_' + i;
        tempElement.style.visibility = 'hidden';
        $(tempElement).addClass(cssClass + '-' + i);
        document.body.appendChild(tempElement);
        tempElement = document.getElementById('gmaps_'+ iw.infoWindowId_ + '_' + i);
        var tempWrapperPart = iw.wrapperParts[i];    
        tempWrapperPart.w = parseInt(iw.getStyle_(tempElement, 'width'), 10);
        tempWrapperPart.h = parseInt(iw.getStyle_(tempElement, 'height'), 10);
        document.body.removeChild(tempElement);
      }
      
      for (var i in iw.wrapperParts) {
        var wrapperPartsDiv = iw.wrapperParts[i].domElement;
        $(wrapperPartsDiv).addClass(cssClass +'-'+ i);
        iw.wrapperParts[i].domElement = wrapperPartsDiv;
      }
      
      var trigger = iw.triggerWindowOpenEvent;
      iw.triggerWindowOpenEvent = false;
      iw.redraw(true);
      iw.redraw(true);
      iw.triggerWindowOpenEvent = trigger;
    });
  };
  
  var showContent = function(content, currentOpts, marker, ll) {
    var minContent, maxTitle, maxContent, tabs, hasMin, hasMax;
    currentOpts.tabs_state = currentOpts.tabs_state == null ? 'max' : currentOpts.tabs_state;
    
    hasMin = typeof(content.min) != 'undefined' && content.min !== null && content.min.length;
    hasMax = typeof(content.max) != 'undefined' && content.max !== null && content.max.length;
    
    if (!hasMin && hasMax) {
      content.min = content.max;
      currentOpts.tabs_state = 'min';
      delete content.max;
    }
    
    if (typeof(content.tabs) != 'undefined') {
      if (currentOpts.style == 'plain') {
        tabs = Drupal.gmaps.map.renderTabsPlain(content.tabs);
      }
      else if (currentOpts.style == 'list') {
        tabs = Drupal.gmaps.map.renderTabsList(content.tabs);
      }
      else if (currentOpts.style == 'grid') {
        tabs = Drupal.gmaps.map.renderTabsList(content.tabs, currentOpts);
      }
      else if (currentOpts.style == 'box') {
        tabs = Drupal.gmaps.map.renderTabsBox(content.tabs);
      }
      else if (currentOpts.style == 'fieldset') {
        tabs = Drupal.gmaps.map.renderTabsFieldset(content.tabs, currentOpts);
      }
      else if (currentOpts.style == 'accordion') {
        tabs = Drupal.gmaps.map.renderTabsAccordion(content.tabs, currentOpts);
      }
      else if (currentOpts.style == 'tabs') {
        tabs = Drupal.gmaps.map.renderTabsTabs(content.tabs, currentOpts);
      }
      
      if (tabs) {
        currentOpts.tabs = true;
      }
    }
    
    if (hasMin) {
      minContent = Drupal.theme('gmapsInfoWindowContent', content.title, content.min);
      if (tabs && currentOpts.tabs_state == 'min') {
        minContent += tabs;
      }
    }
    
    if (hasMax) {
      maxTitle = currentOpts.maxTitle || content.title;
      maxContent = Drupal.theme('gmapsInfoWindowContent', maxTitle, content.max);
      if (tabs && currentOpts.tabs_state == 'max') {
        maxContent += tabs;
      }
      currentOpts.maxContent = maxContent;
    }
    
    if (minContent) {
      if (gmi.gmap2.ClickListener_ != null) {
        GEvent.removeListener(gmi.gmap2.ClickListener_);
      }
      if (ll) {
        gmi.gmap2.openExtInfoWindow(marker, currentOpts.cssId, '', currentOpts);
      }
      else {
        marker.openExtInfoWindow(gmi.gmap2, currentOpts.cssId, '', currentOpts);
      }
      
      var iw = gmi.gmap2.getExtInfoWindow();
      if (iw) {
        iw.ajaxUrl_ = minContent;
        
        iw.ajaxRequest_ = function(content) {
          content = content || iw.ajaxUrl_;
          var thisMap = iw.map_;
          var infoWindow = document.getElementById(iw.infoWindowId_ + '_contents');
          var curentState = iw.isMaximized_ ? 'min' : 'max';
          
          var currentContent = $('<div/>').html(content)[0];

          $(infoWindow).empty().append(currentContent);

          Drupal.attachBehaviors(currentContent);
          if (tabs && currentOpts.tabs_state == curentState) {
            addContentListeners(currentContent, currentOpts);
          }

          iw.redraw(true);
          iw.resize();
          GEvent.trigger(thisMap, 'extinfowindowupdate');
        };
        
        iw.ajaxRequest_(iw.ajaxUrl_);
      }
    }
  };
  
  var addContentListeners = function(c, currentOpts) {
    if (currentOpts.style == 'fieldset' && currentOpts.collapsible) {
      $('fieldset.collapsible > legend a', c).click(function() {
        setTimeout(function() {
          gmi.gmap2.getExtInfoWindow().resize();
          gmi.gmap2.getExtInfoWindow().redraw(true);
        },200);
      });
    }
    else if (currentOpts.style == 'accordion') {
      $('.gmaps-accordion', c).bind('accordionchange', function() {
        gmi.gmap2.getExtInfoWindow().resize();
        gmi.gmap2.getExtInfoWindow().redraw(true);
      });
    }
    else if (currentOpts.style == 'tabs') {
      $('.gmaps-tabs', c)
        .bind('tabsshow', function() {
          gmi.gmap2.getExtInfoWindow().resize();
          gmi.gmap2.getExtInfoWindow().redraw(true);
        })
        .bind('tabsselect', function(e, ui) {
          if (!$(ui.panel).hasClass('ui-tabs-hide')) {
            setTimeout(function() {
              gmi.gmap2.getExtInfoWindow().resize();
              gmi.gmap2.getExtInfoWindow().redraw(true);
            }, 200);
          }
        });
    }
  };
  
  this.openInfoWindow = function(latlng, content, iwOpts) {
    var currentOpts = $.extend(true, {}, opts, iwOpts ? iwOpts : {});
    currentOpts.cssId = currentOpts.cssId || gmi.getOptions().id +'-extinfowindow';
    showContent(content, currentOpts, latlng, true);
  };
  
  this.openMarkerInfoWindow = function(marker, content) {
    var currentOpts;
    if (marker.gmaps.options.iw_data && marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]) {
      currentOpts = $.extend(true, {}, opts, parseOpts(marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]));
    }
    else {
      currentOpts = $.extend(true, {}, opts);
    }
    currentOpts.cssId = currentOpts.cssId || gmi.getOptions().id +'-'+ marker.gmaps.options.id +'-extinfowindow';
    showContent(content, currentOpts, marker);
  };
  
  initialize();
};

/**
 * Reference implementation of interface "InfoWindowHandler"
 * for Tabbed Max Content info window
 */
GMapsInfoWindowHandlerTabbedMax = function(gmi) {
  var self = this;
  var opts = {};
  
  var createEl = function (tag, attrs, content, style, parent) {
    var node = content;
    if (!content || (content && typeof content === 'string')) {
      node = document.createElement(tag);
      node.innerHTML = content || ''; 
    }
    if (style) {
      $.extend(true, node.style, style);
    }
    if (attrs) {
      $.extend(true, node, attrs);
    }
    if (parent) {
      parent.appendChild(node);
    }
    return node;
  };
  
  var overwriteTabbedMaxContent = function(tmc) {
    if (tmc.gmapsOverwritten) return;
    tmc.gmapsOverwritten = true;
    
    var callback = function(obj, method, arg){
      return function() {method.call(obj, arg)};
    };
    tmc.onMaximizeClick_ = function () {
      for (var i = 0, ct = this.tabs_.length; i < ct; i++) {
        if (this.tabs_[i].navNode_.clickListener_ == null) {
          this.tabs_[i].navNode_.clickListener_ = callback(tmc, tmc.selectTab, i);
          $(this.tabs_[i].navNode_).click(this.tabs_[i].navNode_.clickListener_);
        }
      }
    };

    tmc.initialize_ = function (sumNode, tabs, opt_maxOptions) {
      GEvent.clearListeners(this.infoWindow_, 'maximizeclick');
      GEvent.clearListeners(this.infoWindow_, 'restoreclick');
      GEvent.bind(this.infoWindow_, 'maximizeclick', this, this.onMaximizeClick_);
      this.tabs_ = tabs;
      this.selectedTab_ = -1;

      opt_maxOptions = opt_maxOptions || {};
      var selectedTab = opt_maxOptions.selectedTab || 0;
      this.cssClass = opt_maxOptions['class'] || 'gmaps-tmcinfowindow';
      var cssId = opt_maxOptions.cssId || 'gmaps-tmc-maxcontent';

      if (this.maxNode_) {
        GEvent.clearNode(this.maxNode_);
        this.maxNode_.innerHTML = '';
      } else {
        this.maxNode_ = createEl('div');
      }
      this.maxNode_.id = cssId;
      $(this.maxNode_).addClass(this.cssClass);
      
      this.summaryNode_ = createEl('div', null, sumNode, null, this.maxNode_);
      $(this.summaryNode_).addClass(this.cssClass +'-summary');
      
      this.navsNode_ = createEl('div', null, null, null, this.maxNode_);
      $(this.navsNode_).addClass(this.cssClass +'-tabbar');
      
      this.contentsNode_ = createEl('div', null, null, null, this.maxNode_);
      var left, node, right;
      if (tabs && tabs.length) {
        // left
        left = createEl('span', null, null, null, this.navsNode_);
        $(left).addClass(this.cssClass +'-tableft');
        for (var i = 0, ct = tabs.length; i < ct; i++) {
          if (i === selectedTab || tabs[i].getLabel() === selectedTab) {
            this.selectedTab_ = i;
          }
          tabs[i].navNode_ = createEl('span', null, tabs[i].getLabel(), this.style_.tabOff, this.navsNode_);//);
          $(tabs[i].navNode_).addClass(this.cssClass +'-taboff');
          node = createEl('div', null, tabs[i].getContentNode(), null, this.contentsNode_);
          $(node).addClass(this.cssClass +'-content');
          node.style.display = 'none';
        }
        // right
        right = createEl('span', null, null, null, this.navsNode_);
        $(right).addClass(this.cssClass +'-tabright');
      }
    };
    
    tmc.selectTab = function (identifier) {
      var trigger = false;
      var hasVisibleTab = false;
      var tab;
      for (var i = 0, ct = this.tabs_.length; i < ct; i++) {
        tab = this.tabs_[i];
        if (i === identifier || tab.getLabel() === identifier) {
          if (tab.getContentNode().style.display === 'none') {
            $(tab.navNode_).addClass(this.cssClass +'-tabon');
            tab.getContentNode().style.display = 'block';
            this.selectedTab_ = i;  
            trigger = true;
          }
          hasVisibleTab = true; 
        } else {
          $(tab.navNode_).removeClass(this.cssClass +'-tabon');
          tab.getContentNode().style.display = 'none';
        }
      }
      // avoid excessive event if clicked on a selected tab.
      if (trigger) {
        /**
         * This event is fired after a tab is selected,
         * passing the selected {@link MaxContentTab} into the callback.
         * @name TabbedMaxContent#selecttab
         * @param {MaxContentTab} selected tab
         * @event
         */
        GEvent.trigger(this, 'selecttab', this.tabs_[this.selectedTab_]);
      }
      if (!hasVisibleTab) {
        this.selectTab(0);
      }
    };
    
    tmc.checkResize = function () {};
  };
  
  
  var parseOpts = function(gmapsOpts) {
    var o = {};
    o.maxWidth = parseInt(gmapsOpts.maxwidth, 10);
    o.noCloseOnClick = parseInt(gmapsOpts.nocloseonclick, 10);
    o.pixelOffset = new GSize(parseInt(gmapsOpts.offset.x, 10), parseInt(gmapsOpts.offset.y, 10));
    o.maximized = parseInt(gmapsOpts.maximized, 10);
    o.maximized = parseInt(gmapsOpts.maximized, 10);
    o['class'] = gmapsOpts['class'];
    return o;
  };
  
  var initialize = function() {
    opts = parseOpts(gmi.getOptions().base.iw_data.tmc);
  };
  
  var prepareContent = function(content, currentOpts, c) {
    if (typeof(content.min) != 'undefined' && content.min !== null && content.min.length) {
      c.minContent = $('<div/>').html(Drupal.theme('gmapsInfoWindowContent', content.title, content.min))[0];
      Drupal.attachBehaviors(c.minContent);
    }
    
    if (typeof(content.max) != 'undefined' && content.max !== null && content.max.length) {
      c.maxContent = $('<div/>').html(content.max)[0];
      Drupal.attachBehaviors(c.maxContent);
    }
    if (!currentOpts.maxTitle && content.title) {
      currentOpts.maxTitle = content.title;
    }
    
    if (typeof(content.tabs) != 'undefined') {
      $.each(content.tabs, function(label, tabContent) {
        tabContent = $('<div/>').html(tabContent)[0];
        Drupal.attachBehaviors(tabContent);
        c.tabs.push(c.minContent ? new MaxContentTab(label, tabContent) : new GInfoWindowTab(label, tabContent));
      });
    }
  };
  
  var maximize = function(currentOpts) {
    if (currentOpts.maxContent && currentOpts.maximized) {
      var iw = gmi.gmap2.getInfoWindow();
      var m = GEvent.addListener(gmi.gmap2, 'infowindowopen', function () {
        GEvent.removeListener(m);
        iw.maximize();
      });
    }
  };
  
  this.openInfoWindow = function(latlng, content, iwOpts) {
    var currentOpts = $.extend(true, {}, opts, iwOpts ? iwOpts : {});
    var c = {minContent: null, maxContent: null, tabs: []};

    prepareContent(content, currentOpts, c);
    
    if (c.tabs.length) {
      if (c.minContent) {
        overwriteTabbedMaxContent(gmi.gmap2.getTabbedMaxContent());
        currentOpts.cssId = currentOpts.cssId || gmi.getOptions().id +'-tmcinfowindow';
        gmi.gmap2.openMaxContentTabs(latlng, c.minContent, c.maxContent, c.tabs, currentOpts);
      }
      else {
        if (c.maxContent) {
          currentOpts.maxContent = c.maxContent;
        }
        gmi.gmap2.openInfoWindowTabs(latlng, c.tabs, currentOpts);
      }
    }
    else if (c.maxContent) {
      if (c.minContent) {
        currentOpts.maxContent = c.maxContent;
        gmi.gmap2.openInfoWindow(latlng, c.minContent, currentOpts);
      }
      else {
        delete currentOpts.maxTitle;
        gmi.gmap2.openInfoWindow(latlng, c.maxContent, currentOpts);
      }
    }
    else {
      delete currentOpts.maxTitle;
      gmi.gmap2.openInfoWindow(latlng, c.minContent, currentOpts);
    }
    maximize(currentOpts);
  };
  
  this.openMarkerInfoWindow = function(marker, content) {
    var currentOpts;
    if (marker.gmaps.options.iw_data && marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]) {
      currentOpts = $.extend(true, {}, opts, parseOpts(marker.gmaps.options.iw_data[marker.gmaps.options.basic.info_window]));
    }
    else {
      currentOpts = $.extend(true, {}, opts);
    }
    var c = {minContent: null, maxContent: null, tabs: []};

    prepareContent(content, currentOpts, c);
    
    if (c.tabs.length) {
      if (c.minContent) {
        overwriteTabbedMaxContent(gmi.gmap2.getTabbedMaxContent());
        currentOpts.cssId = currentOpts.cssId || gmi.getOptions().id +'-'+ marker.gmaps.options.id +'-tmcinfowindow';
        marker.openMaxContentTabs(gmi.gmap2, c.minContent, c.maxContent, c.tabs, currentOpts);
      }
      else {
        if (c.maxContent) {
          currentOpts.maxContent = c.maxContent;
        }
        marker.openInfoWindowTabs(c.tabs, currentOpts);
      }
    }
    else if (c.maxContent) {
      if (c.minContent) {
        currentOpts.maxContent = c.maxContent;
        marker.openInfoWindow(c.minContent, currentOpts);
      }
      else {
        delete currentOpts.maxTitle;
        marker.openInfoWindow(c.maxContent, currentOpts);
      }
    }
    else {
      delete currentOpts.maxTitle;
      marker.openInfoWindow(c.minContent, currentOpts);
    }
    maximize(currentOpts);
  };
  
  initialize();
};

Drupal.gmaps.map = Drupal.gmaps.map || new (function() {
  var self = this, svuClient, loadedContents = {};
  this.maps = {};
  
  this.hooks = {
    'maptype': {},
    'method': {
      'adsmanager': ['disableAdsManager', 'enableAdsManager'],
      'auto_center_zoom': [false, 'enableAutoCenterZoom'],
      'continuouszoom': ['disableContinuousZoom', 'enableContinuousZoom'],
      'dblclickzoom': ['disableDoubleClickZoom', 'enableDoubleClickZoom'],
      'dragging': ['disableDragging', 'enableDragging'],
      'googlebar': ['disableGoogleBar', 'enableGoogleBar'],
      'keyboardhandler': [false, 'enableKeyboardHandler'],
      'layers': ['disableLayers', 'enableLayers'],
      'resize': [false, 'enableResize'],
      'rotation': ['disableRotation', 'enableRotation'],
      'scrollwheelzoom': ['disableScrollWheelZoom', 'enableScrollWheelZoom'],
      'streetview': ['disableStreetView', 'enableStreetView'],
      'traffic': ['disableTraffic', 'enableTraffic'],
      'infowindow': ['disableInfoWindow', 'enableInfoWindow'],
      'keydragzoom': ['disableGMapsKeyDragZoom', 'enableGMapsKeyDragZoom']
    },
    'control': {},
    'info_window': {
      'google': GMapsInfoWindowHandlerGoogle,
      'bb': GMapsInfoWindowHandlerBulletinBoard,
      'extended': GMapsInfoWindowHandlerExtended,
      'tmc': GMapsInfoWindowHandlerTabbedMax
    },
    'overlay': {}
  };
  
  this.hooks.maptype.normal = new GMapsMapTypeHandler();
  this.hooks.maptype = {
    'normal': this.hooks.maptype.normal,
    'satellite': this.hooks.maptype.normal,
    'hybrid': this.hooks.maptype.normal,
    'physical': this.hooks.maptype.normal,
    'satellite_3D': this.hooks.maptype.normal,
    'aerial': this.hooks.maptype.normal,
    'aerial_hybrid': this.hooks.maptype.normal,
    'mapmaker_normal': this.hooks.maptype.normal,
    'mapmaker_hybrid': this.hooks.maptype.normal,
    'moon_elevation': this.hooks.maptype.normal,
    'moon_visible': this.hooks.maptype.normal,
    'mars_elevation': this.hooks.maptype.normal,
    'mars_visible': this.hooks.maptype.normal,
    'mars_infrared': this.hooks.maptype.normal,
    'sky_visible': this.hooks.maptype.normal
  };
  
  this.hooks.control.smallzoom = new GMapsControlHandler();
  this.hooks.control = {
    'smallzoom': this.hooks.control.smallzoom,
    'smallmap': this.hooks.control.smallzoom,
    'largemap': this.hooks.control.smallzoom,
    'smallzoom3D': this.hooks.control.smallzoom,
    'largemap3D': this.hooks.control.smallzoom,
    'extnav': this.hooks.control.smallzoom,
    'scale': this.hooks.control.smallzoom,
    'maptype': this.hooks.control.smallzoom,
    'menumaptype': this.hooks.control.smallzoom,
    'hierarchical_maptype': this.hooks.control.smallzoom,
    'extmaptype': this.hooks.control.smallzoom,
    'overview': this.hooks.control.smallzoom,
    'extdragzoom': this.hooks.control.smallzoom,
    'breadcrumb': this.hooks.control.smallzoom
  };
  
  this.behaviors = {};

  this.getMap = function(id, context, lazy, refresh) {
    if (self.maps[id]) {
      if (refresh) {
        self.maps[id] = null;
      }
      else {
        return self.maps[id];
      }
    }
    
    if (GBrowserIsCompatible()) {
      context = context || document;
      var element = $('#'+ id +':not(.gmaps-map-item-processed)', context);
      if (element.length) {
        element = $(element[0]);
        element.addClass('gmaps-map-item-processed');
        self.maps[id] = new GMapsMapItem(element, null, lazy);
      }
    }
    return self.maps[id];
  };
  
  this.setGoogleBarOptions = function(gmi, mapOpts) {
    var opts = gmi.getOptions();
    if (opts.base.method_data && opts.base.method_data.googlebar) {
      var gb = opts.base.method_data.googlebar; 
    }
    else {
      return;
    }
    mapOpts.googleBarOptions = {};
    var gbOpts = mapOpts.googleBarOptions;
    
    gbOpts.showOnLoad = parseInt(gb.showonload) ? true : false;
    
    gbOpts.style = "old";
    //disabled, because bogus
    /*if (parseInt(gb.newstyle)) {
      gbOpts.style = "new";
    }*/
    
    if (gb.ads && gb.ads.client && gb.ads.client.length) {
      gbOpts.adsOptions = {};
      gbOpts.adsOptions.client = gb.client;
      gbOpts.adsOptions.channel = gb.channel;
      gbOpts.adsOptions.adsafe = gb.safety;
    }
    
    gbOpts.linkTarget = gb.link_target;
    gbOpts.listingTypes = gb.listing_types;
    gbOpts.resultList = gb.result_list == 'bb' ? gmi.getBulletinBoard()[0] : gb.result_list;
    gbOpts.suppressInitialResultSelection = parseInt(gb.suppress_selection) ? true : false;
    gbOpts.suppressZoomToBounds = parseInt(gb.suppress_zoom) ? true : false;
  };
  
  this.createThrobber = function(gmi) {
    var markerOpts = {
      'icon': Drupal.gmaps.icon.getIcon(gmi.getOptions().base.map_data.throbber),
      'clickable': false,
      'draggable': false
    };
    return new GMarker(gmi.gmap2.getCenter(), markerOpts);
  };
  
  this.attachBehaviors = function(gmi) {
    if (gmi) {
      $.each(self.behaviors, function() {
        this(gmi);
      });
    }
  };
  
  this.getStreetviewClient = function() {
    if (!svuClient) {
      svuClient = new GStreetviewClient();
    }
    
    return svuClient;
  };
  
  this.loadContent = function(content, callback, gmi, latlng) {
    if (typeof(content.ajax) == 'undefined' || content.ajax === null || typeof(content.ajax.url) == 'undefined' || content.ajax.url === null || !content.ajax.url.length) {
      delete content.url;
      callback(content);
      return;
    }
    
    var cid = $.md5(content.ajax.url + (content.ajax.data ? (content.ajax.url.match(/\?/) ? "&" : "?") + $.param(content.ajax.data) : ''));
    if (loadedContents[cid]) {
      callback(loadedContents[cid]);
      return;
    }
    
    if (gmi) {
      var ll = latlng || gmi.gmap2.getCenter();
      gmi.showThrobber(ll);
    }
    
    var ajax = {
      type: "POST",
      url: content.ajax.url,
      data: content.ajax.data,
      success: function (response) {
        if (typeof(response.status) != 'undefined' && response.status == 0) {
          if (typeof(response.data) != 'undefined') {
            callback({'title': 'Internal server error', 'min': response.data});
          }
        }
        else {
          //avoid infinite loops
          delete response.data.ajax;
          loadedContents[cid] = response.data;
          callback(loadedContents[cid]);
        }
        if (gmi) {
          gmi.hideThrobber();
        }
      },
      error: function (xmlhttp) {
        callback({'title': 'AJAX/HTTP error', 'min': Drupal.ahahError(xmlhttp, content.ajax.url)});
        if (gmi) {
          gmi.hideThrobber();
        }
      }
    };
    
    Drupal.gmaps.loadAjaxContent(ajax);
  };
  
  this.renderTabsPlain = function(tabs) {
    var output = '';
    $.each(tabs, function(title, content) {
      output += content;
    });
    return '<div class="gmaps-info-window-tabs gmaps-info-window-tabs-box">'+ output +'</div>';
  };
  
  this.renderTabsList = function(tabs) {
    var items = [];
    $.each(tabs, function(title, content) {
      items.push({data: content, title: title, alt: title});
    });
    return items.length ? '<div class="gmaps-info-window-tabs gmaps-info-window-tabs-box">'+ Drupal.theme('itemList', items) +'</div>' : '';
  };
  
  this.renderTabsGrid = function(tabs, opts) {
    var items = [];
    $.each(tabs, function(title, content) {
      items.push({data: content, title: title, alt: title});
    });
    return items.length ? '<div class="gmaps-info-window-tabs gmaps-info-window-tabs-box">'+ Drupal.theme('gmapsGrid', items, opts.grid) +'</div>' : '';
  };
  
  this.renderTabsBox = function(tabs) {
    var output = '';
    $.each(tabs, function(title, content) {
      output += Drupal.theme('box', title, content);
    });
    return '<div class="gmaps-info-window-tabs gmaps-info-window-tabs-box">'+ output +'</div>';
  };
  
  this.renderTabsFieldset = function(tabs, opts) {
    var output = '';
    $.each(tabs, function(title, content) {
      var fs = {
        '#type': 'fieldset',
        '#title': title,
        '#value': content,
        '#collapsible': opts.collapsible,
        '#collapsed': opts.collapsed
      };
      output += Drupal.render(fs);
    });
    return '<div class="gmaps-info-window-tabs gmaps-info-window-tabs-fieldset">'+ output +'</div>';
  };
  
  this.renderTabsAccordion = function(tabs, opts) {
    var accordion = {
      '#type': 'gmaps_accordion',
      '#collapsible': opts.collapsible,
      '#collapsed': opts.collapsed,
      '#attributes': {'class': 'gmaps-info-window-tabs gmaps-info-window-tabs-accordion'}
    };
    var i = 0;
    $.each(tabs, function(title, content) {
      accordion[i] = {
        '#type': 'gmaps_accordion_panel',
        '#title': title,
        '#value': content
      };
      i++;
    });
    return i > 0 ? Drupal.render(accordion) : '';
  };
  
  this.renderTabsTabs = function(tabs, opts) {
    var gmapsTabs = {
      '#type': 'gmaps_tabs',
      '#collapsible': opts.collapsible,
      '#collapsed': opts.collapsed,
      '#nav_position': opts.tabs_nav,
      '#attributes': {'class': 'gmaps-info-window-tabs gmaps-info-window-tabs-tabs'}
    };
    var i = 0;
    $.each(tabs, function(title, content) {
      gmapsTabs[i] = {
        '#type': 'gmaps_tabs_panel',
        '#title': title,
        '#value': content
      };
      i++;
    });
    return i > 0 ? Drupal.render(gmapsTabs) : '';
  };

  this.getOverlayBounds = function(ov, boundsOnly) {
    var bounds, ll, sw, ne, iconSize;
    if (ov.gmaps && ov.gmaps.point) {
      if (ov.gmaps.point.bounds && (sw = ov.gmaps.point.bounds.southwest) && (ne = ov.gmaps.point.bounds.northeast)) {
        if (sw.latitude && sw.longitude && ne.latitude && ne.longitude) {
          bounds = new GLatLngBounds(new GLatLng(sw.latitude, sw.longitude), new GLatLng(ne.latitude, ne.longitude));
        }
      }
      if (!bounds && !boundsOnly) {
        ll = new GLatLng(ov.gmaps.point.latitude, ov.gmaps.point.longitude);
      }
    }
    else if (ov.getLatLng && !boundsOnly) {
      iconSize = ov.getIcon ? ov.getIcon().iconSize : {width: 0, height: 0}
      if (iconSize.width != 0 && iconSize.height != 0 && (!ov.isHidden || !ov.isHidden())) {
        ll = ov.getLatLng();
      }
    }
    
    if (ll) {
      bounds = new GLatLngBounds(new GLatLng(ll.lat() - GMAPS_MAP_DEFAULT_BOUNDS_SPAN, ll.lng() - GMAPS_MAP_DEFAULT_BOUNDS_SPAN), new GLatLng(ll.lat() + GMAPS_MAP_DEFAULT_BOUNDS_SPAN, ll.lng() + GMAPS_MAP_DEFAULT_BOUNDS_SPAN));
    }
    
    return bounds;
  };
  
  //GUnload();
  $(document).unload(function(){GUnload();});
})();

GMapsMapItem = function(container, opts, lazyInit) {
  var self = this;
  opts = opts || Drupal.settings.gmaps.map[container[0].id];
  lazyInit = lazyInit != null ? lazyInit : false;
  
  var bb, i, maptype, throbber, progressbar, initialized = false, svu;
  
  this.gmaps = {};
  this.gmap2 = null;
  this.maptypes = {};
  this.controls = {};
  this.iwHandlers = {};
  this.overlays = {};
  
  this.getOptions = function(){
    return opts;
  };
  
  this.getContainer = function(){
    return container;
  };
  
  this.isInitialized = function(){
    return initialized;
  };
  
  this.getBulletinBoard = function() {
    if (bb == null) {
      bb = $('.gmaps-map-bb:eq(0)', container).hide();
    }
    return bb;
  };
  
  this.bulletinBoardShow = function(content) {
    if (!bb) {
      self.getBulletinBoard();
    }
    if (bb) {
      var c;
      if (content.constructor == 'String') {
        c = $('<div/>').html(content).children();
      }
      else {
        c = $(content).clone();
      }
      bb.hide().empty().append(c);
      bb.show();
      Drupal.attachBehaviors(c);
      $(c).before('<div class="gmaps-map-bb-close"></div>');
      $('.gmaps-map-bb-close', bb).click(function() {self.bulletinBoardHide();});
    }
  };
  
  this.bulletinBoardHide = function() {
    if (bb) {
      bb.html('').hide();
    }
  };
  
  this.getSVU = function(initOpts) {
    if (svu == null && opts.base.type == GMAPS_MAP_EARTH && opts.base.map_data.svu && parseInt(opts.base.map_data.svu.enabled)) {
      svu = new GMapsStreetviewItem(self, initOpts);
    }
    return svu;
  };
  
  this.showThrobber = function(p) {
    if (!throbber) {
      throbber = Drupal.gmaps.map.createThrobber(self);
    }
    if (throbber) {
      if (p instanceof GLatLng) {
        throbber.setLatLng(p);
      }
      else if (p instanceof GLatLngBounds) {
        throbber.setLatLng(p.getCenter());
      }
      else {
        throbber.setLatLng(new GLatLng(p.latitude, p.longitude));
      }
      this.gmap2.addOverlay(throbber);
    }
  };
  
  this.hideThrobber = function() {
    if (throbber) {
      this.gmap2.removeOverlay(throbber);
    }
  };
  
  var createProgressbar = function() {
    if (self.gmap2 && opts.base.map_data.progressbar && parseInt(opts.base.map_data.progressbar.enabled)) {
      var pbOpts = {'loadstring': opts.base.map_data.progressbar.loadstring, 'width': parseInt(opts.base.map_data.progressbar.width)};
      progressbar = new ProgressbarControl(self.gmap2, pbOpts);
    }
  }
  
  this.startProgress = function(ops) {
    if (!progressbar) {
      createProgressbar();
    }
    if (progressbar) {
      progressbar.start(ops);
    }
  };
  
  this.updateProgress = function(step) {
    if (progressbar) {
      progressbar.updateLoader(step);
    }
  };
  
  this.stopProgress = function() {
    if (progressbar) {
      progressbar.remove();
    }
  };
  
  this.openInfoWindow = function(type, latlng, content, iwOpts) {
    self.bulletinBoardHide();
    if (self.gmap2.infoWindowEnabled() && typeof(self.iwHandlers[type]) != 'undefined') {
      if (content.ajax != null && content.ajax.url != null && content.ajax.url.length) {
        var mapOpts = $.extend(true, {}, opts);
        delete mapOpts.overlays;
        var data = $.extend(true, {'info_window': type, 'gmi': mapOpts}, content.ajax.data ? content.ajax.data : {});
        content.ajax.data = Drupal.gmaps.flatten(data);
        Drupal.gmaps.map.loadContent(content, function(c){content = c; self.iwHandlers[type].openInfoWindow(latlng, c, iwOpts);}, self);
      }
      else {
        self.iwHandlers[type].openInfoWindow(latlng, content, iwOpts);
      }
    }
  };
  
  this.addControl = function(id, options) {
    if (options.control.length && Drupal.gmaps.map.hooks.control[options.control]) {
      var control = Drupal.gmaps.map.hooks.control[options.control].createControl(self, options.control);
      if (control) {
        var ctrlPosition = control.getDefaultPosition();
        var anchor = typeof(options.position) != 'undefined' ? parseInt(options.position.anchor, 10) : GMAPS_CONTROL_POSITION_DEFAULT;
        if (anchor > GMAPS_CONTROL_POSITION_DEFAULT) {
          ctrlPosition.anchor = anchor;
          if (typeof(options.position.offset) == 'object' && typeof(options.position.offset.width) != 'undefined') {
            ctrlPosition.offset = new GSize(parseInt(options.position.offset.width, 10), parseInt(options.position.offset.height, 10));
          }
        }
        self.gmap2.addControl(control, ctrlPosition);
        self.controls[id] = control;
      }
    }
  }
  
  this.initialize = function(wereLazy) {
    if (self.gmap2) {
      return;
    }
    
    var cont = $('.gmaps-map-container', container);
    
    //GMap2 options
    var mapOpts = {'mapTypes': []};
    
    //size
    var height = parseInt(opts.base.size.height);
    height = height || Math.max(cont.height(), GMAPS_MAP_MIN_HEIGHT);
    cont.height(height);
    if (parseInt(opts.base.size.width)) {
      container.width(parseInt(opts.base.size.width));
    }
    
    $(opts.base.maptypes).each(function() {
      if (typeof(Drupal.gmaps.map.hooks.maptype[this]) != 'undefined') {
        maptype = Drupal.gmaps.map.hooks.maptype[this].createMapType(self, this);
        if (maptype) {
          mapOpts.mapTypes.push(maptype);
          self.maptypes[this] = maptype;
        }
      }
    });
    
    if (!mapOpts.mapTypes.length) {
      throw Drupal.t('No valid map types had been configured.');
    }
    
    if (opts.base.map_data.draggable_cursor) {
      mapOpts.draggableCursor = opts.base.map_data.draggable_cursor;
    }
    if (opts.base.map_data.dragging_cursor) {
      mapOpts.draggingCursor = opts.base.map_data.dragging_cursor;
    }
    if (opts.base.map_data.bgcolor) {
      mapOpts.backgroundColor = opts.base.map_data.bgcolor;
    }
    
    if (opts.base.methods && opts.base.methods.googlebar) {
      Drupal.gmaps.map.setGoogleBarOptions(self, mapOpts);
    }
    
    //The map
    self.gmap2 = new GMap2(cont[0], mapOpts);
    self.gmap2.gmaps = {'gmi': self, 'adsManager': null, 'isAutoCenterZoomEnabled': false, 'keyboardHandler': null, 'layers': {}, 'streetView': {'overlay': null, 'listener': null, 'timer': null, 'zoomListener': null}, 'traffic': null};
    
    if (typeof(self.maptypes[opts.base.default_maptype]) != 'undefined') {
      self.gmap2.setMapType(self.maptypes[opts.base.default_maptype]);
    }
    var zoom = parseInt(opts.base.zoom, 10);
    if (zoom == -1) {
      if (typeof(opts.base.center.bounds) != 'undefined') {
        zoom = self.gmap2.getBoundsZoomLevel(new GLatLngBounds(new GLatLng(opts.base.center.bounds.southwest.latitude, opts.base.center.bounds.southwest.longitude), new GLatLng(opts.base.center.bounds.northeast.latitude, opts.base.center.bounds.northeast.longitude)));
      }
      else {
        zoom = GMAPS_MAP_DEFAULT_ZOOM;
      }
    }
    self.gmap2.setCenter(new GLatLng(opts.base.center.latitude, opts.base.center.longitude), zoom);
    
    //non-lazy init
    if (!wereLazy) {
      var handler;
      if (handler = Drupal.gmaps.getLazyInitHandler('#'+ container[0].id)) {
        handler.attach('#'+ container[0].id, function() {
          var center = self.gmap2.getCenter();
          self.gmap2.checkResize();
          self.gmap2.setCenter(center);
          //initCenter(self.gmap2);
        });
      }
    }

    //Methods
    if (opts.base.methods) {
      $.each(Drupal.gmaps.map.hooks.method, function(index, callbacks){
        if (opts.base.methods.constructor == Array) {
          index = $.inArray(index, opts.base.methods);
        }
        var cb = opts.base.methods[index] ? callbacks[1] : callbacks[0];
        if (cb && self.gmap2[cb]) {
          self.gmap2[cb]();
        }
      });
    }
    
    //Controls
    if (opts.base.controls) {
      $.each(opts.base.controls, function(index, ct) {
        self.addControl(index, ct)
      });
    }
    
    //Info windows
    if (opts.info_windows) {
      for (var i in opts.info_windows) {
        if (typeof(Drupal.gmaps.map.hooks.info_window[i]) != 'undefined') {
          self.iwHandlers[i] = new (Drupal.gmaps.map.hooks.info_window[i])(self);
        }
      }
    }
    
    //Overlays
    if (opts.overlays) {
      var ovHandler, overlays;
      $.each(opts.overlays, function(type, content) {
        if (ovHandler = Drupal.gmaps.map.hooks.overlay[type]) {
          ovHandler.addContent(self, content);
        }
      });
    }
    
    //TOC map
    if (opts.base.map_data.toc_map && parseInt(opts.base.map_data.toc_map.enabled, 10)) {
      var toc_style = opts.base.map_data.toc_map.style;
      if (toc_style == 'accordion' || toc_style == 'tabs') {
        var toc_gmi = $.extend(true, {}, opts);
        delete toc_gmi.overlays;
        
        if (toc_style == 'tabs' && $('.form-gmaps-tabs:first', container).length) {
          var toc_id = $('.form-gmaps-tabs:first', container)[0].id;
          if (!Drupal.settings.gmaps.tabs[toc_id].ahahOptions) {
            Drupal.settings.gmaps.tabs[toc_id].ahahOptions = {};
          }
          if (!Drupal.settings.gmaps.tabs[toc_id].ahahOptions.data) {
            Drupal.settings.gmaps.tabs[toc_id].ahahOptions.data = {};
          }
          Drupal.settings.gmaps.tabs[toc_id].ahahOptions.data.gmi = toc_gmi;
          self.toc = Drupal.gmaps.tabs.getTabs(toc_id, container);
        }
        else if ($('.form-gmaps-accordion:first', container).length) {
          var toc_id = $('.form-gmaps-accordion:first', container)[0].id;
          if (!Drupal.settings.gmaps.accordion[toc_id].ahahOptions) {
            Drupal.settings.gmaps.accordion[toc_id].ahahOptions = {};
          }
          if (!Drupal.settings.gmaps.accordion[toc_id].ahahOptions.data) {
            Drupal.settings.gmaps.accordion[toc_id].ahahOptions.data = {};
          }
          Drupal.settings.gmaps.accordion[toc_id].ahahOptions.data.gmi = toc_gmi;
          self.toc = Drupal.gmaps.accordion.getAccordion(toc_id, container);
        }
      }
    }
    
    //Behaviors
    Drupal.gmaps.map.attachBehaviors(self);
    
    GEvent.trigger(self, 'initialized');
    initialized = true;
  };
  
  if (GBrowserIsCompatible()) {
    self.initialize(lazyInit);
  }

};

GMapsStreetviewItem = function(gmi, initOpts, client) {
  var self = this, staticOffset, marker, dynamicMarker, isHidden = true, tracker, flashAvailable = true;
  client = client || Drupal.gmaps.map.getStreetviewClient();
  this.wrapper = $($('.gmaps-map-svu-wrapper', gmi.getContainer())[0]);
  this.container = $($('.gmaps-map-svu', gmi.getContainer())[0]);
  this.panorama;
  
  var startDrag = function(e) {
    staticOffset = self.container.height() - e.pageY;
    self.container.css('opacity', 0.25);
    $(document).mousemove(performDrag).mouseup(endDrag);
    return false;
  };

  var performDrag = function(e) {
    self.container.height(Math.max(32, staticOffset + e.pageY) + 'px');
    return false;
  };

  var endDrag = function(e) {
    $(document).unbind("mousemove", performDrag).unbind("mouseup", endDrag);
    self.container.css('opacity', 1);
    self.panorama.checkResize();
  };
  
  var handleError = function(errorCode) {
    if (errorCode == 603) {
      flashAvailable = false;
      self.hide();
      if (marker) {
        gmi.gmap2.removeOverlay(marker);
        if (marker.gmaps.svu.dragendListener) {
          GEvent.removeListener(marker.gmaps.svu.dragendListener);
        }
        if (marker.gmaps.svu.clickListener) {
          GEvent.removeListener(marker.gmaps.svu.clickListener);
        }
        if (marker.gmaps.svu.rightclickListener) {
          GEvent.removeListener(marker.gmaps.svu.rightclickListener);
        }
        if (tracker) {
          tracker.disable();
          delete tracker;
        }
        marker = null;
      }
      if (self.panorama) {
        self.panorama.remove();
        GEvent.removeListener(self.panorama.gmaps.errorListener);
        GEvent.removeListener(self.panorama.gmaps.initListener);
        GEvent.removeListener(self.panorama.gmaps.yawListener);
        self.panorama = null;
      }
      self.wrapper.html('');
      GEvent.trigger(self, 'gmapssvunoflash');
    }
    else if (errorCode == 600) {
      GEvent.trigger(self, 'gmapssvunosvu');
    }
  };
  
  var getYawBearing = function(yaw) {
    var compass = ['n','nne','ne','ene','e','ese','se','sse','s','ssw','sw','wsw','w','wnw','nw','nnw'];
    var SVUMAN_NUM_ICONS = 16;
    var SVUMAN_ANGULAR_RES = 360/SVUMAN_NUM_ICONS;
    if (yaw < 0) {
      yaw += 360;
    }
    var guyImageNum = Math.round(yaw/SVUMAN_ANGULAR_RES) % SVUMAN_NUM_ICONS;
    return compass[guyImageNum];
  };
  
  var getYawIcon = function(yaw) {
    if (dynamicMarker) {
      return Drupal.gmaps.icon.getIcon(gmi.getOptions().base.map_data.svu.icon[getYawBearing(yaw)]);
    }
    else {
      return Drupal.gmaps.icon.getIcon(gmi.getOptions().base.map_data.svu.icon.n);
    }
  };

  var onYawChange = function(newYaw) {
    if (marker) {
      marker.setImage(getYawIcon(newYaw).image);
    }
  };
  
  var prepareInitOpts = function(initOpts) {
    initOpts = initOpts || {};
    
    var opts = gmi.getOptions().base.map_data.svu;
    
    if (typeof(opts.fullscreen) != 'undefined' && opts.fullscreen != null && (typeof(initOpts.enableFullScreen) == 'undefined' || initOpts.enableFullScreen == null)) {
      initOpts.enableFullScreen = parseInt(opts.fullscreen, 10) ? true : false;
    }
    
    initOpts.features = initOpts.features || {};
    if (typeof(opts.userphotos) != 'undefined' && opts.userphotos != null && (typeof(initOpts.features.userPhotos) == 'undefined' || initOpts.features.userPhotos == null)) {
      initOpts.features.userPhotos = parseInt(opts.userphotos) ? true : false;
      
      if (typeof(opts.photorepos) != 'undefined' && opts.photorepos != null && (typeof(initOpts.userPhotosOptions) == 'undefined' || initOpts.userPhotosOptions == null)) {
        var photorepos = new Array();
        $.each(opts.photorepos, function(k, v) {
          photorepos.push(k);
        });
        if (photorepos.length) {
          initOpts.userPhotosOptions = {photoRepositories: photorepos};
        }
      }
    }
    
    if (initOpts.features.userPhotos) {
      initOpts.features.streetView = true;
    }
    
    return initOpts;
  };
  
  var initialize = function() {
    self.container.height(Math.max(parseInt(gmi.getOptions().base.map_data.svu.height), GMAPS_MAP_SVU_MIN_HEIGHT));
    
    self.panorama = new GStreetviewPanorama(self.container[0], prepareInitOpts(initOpts));
    self.panorama.gmaps = {};
    self.panorama.gmaps.errorListener = GEvent.addListener(self.panorama, "error", handleError); 
    self.panorama.gmaps.initListener = GEvent.addListener(self.panorama, "initialized", function(loc) {updateMarker(loc.latlng)}); 
    self.panorama.gmaps.yawListener = GEvent.addListener(self.panorama, "yawchanged", onYawChange); 
    
    if (self.panorama && parseInt(gmi.getOptions().base.map_data.svu.resize)) {
      $(self.container).wrap('<div class="resizable-gmaps-svu-container"><span></span></div>')
      .parent().append($('<div class="grippie"></div>').mousedown(startDrag));
    
      var grippie = $('div.grippie', $(self.container).parent())[0];
      grippie.style.marginRight = (grippie.offsetWidth - $(self.container)[0].offsetWidth) +'px';
    }
  };
  
  var createMarker = function(point, pov) {
    if (!marker) {
      dynamicMarker = parseInt(gmi.getOptions().base.map_data.svu.icon.type);
      var icon = getYawIcon(pov.yaw);
      var markerOpts = {
        'icon': icon,
        'clickable': true,
        'draggable': true,
        'autoPan': true
      };
      marker = new  GMarker(point, markerOpts);
      marker.gmaps = marker.gmaps || {};
      marker.gmaps.svu = {'blowup': false, 'dragendListener': null, 'clickListener': null, 'rightclickListener': null};
      
      marker.gmaps.svu.dragendListener = GEvent.addListener(marker, 'dragend', function(ll) {
        self.setLatLng(ll);
      });
      
      if (parseInt(gmi.getOptions().base.map_data.svu.blowup)) {
        marker.gmaps.svu.clickListener = GEvent.addListener(marker, 'click', function(ll) {
          marker.showMapBlowup();
          marker.gmaps.svu.blowup = true;
        });
      }
      
      if (parseInt(gmi.getOptions().base.map_data.svu.hide)) {
        marker.gmaps.svu.rightclickListener = GEvent.addListener(gmi.gmap2, 'singlerightclick', function(p, dom, ov) {
          if (ov === marker) {
            self.hide();
          }
        });
      }
      
      gmi.gmap2.addOverlay(marker);
      
      if (parseInt(gmi.getOptions().base.map_data.svu.track)) {
        tracker = new MarkerTracker(marker, gmi.gmap2, {'updateEvent': 'moveend'});
      }
    }
  };
  
  var updateMarker = function(point, pov) {
    if (parseInt(gmi.getOptions().base.map_data.svu.marker)) {
      createMarker(point, pov);
      if (marker) {
        if (marker.gmaps.svu.blowup) {
          gmi.gmap2.closeInfoWindow();
          marker.gmaps.svu.blowup = false;
        }
        marker.setLatLng(point);
        marker.setImage(getYawIcon(pov.yaw).image);
      }
    }
  };
  
  var setLatLngCallback = function(ll, point, pov) {
    if (ll) {
      self.show();
      self.panorama.setLocationAndPOV(ll, pov);
      //setTimeout(function() {self.show();}, 1000);
      GEvent.trigger(self, 'gmapssvulocationset', ll);
    }
    else {
      GEvent.trigger(self, 'gmapssvunosvu');
      self.hide();
    }
    updateMarker(point, pov);
  };
  
  this.setLatLng = function(point, pov) {
    if (flashAvailable) {
      self.show();
      var p;
      if (point instanceof GLatLng) {
        p = new GLatLng(point.lat(), point.lng());
      }
      else {
        p = new GLatLng(point.latitude, point.longitude);
        if (!pov && point.pov) pov = $.extend(true, {}, point.pov); 
      }
      client.getNearestPanoramaLatLng(p, function (ll) {setLatLngCallback(ll, p, pov);});
    }
  };
  
  this.show = function() {
    if (flashAvailable && isHidden) {
      self.wrapper.show();
      isHidden = false;
    }
  };
  
  this.hide = function() {
    if (!isHidden) {
      self.wrapper.hide();
      isHidden = true;
    }
  };
  
  this.setMarker = function(m) {
    marker = m;
  };
  
  this.getClient = function() {
    return client;
  };
  
  initialize();
};
