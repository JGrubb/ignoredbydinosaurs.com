// $Id: gmaps-icon.js,v 1.1.2.4 2010/03/18 10:52:16 xmarket Exp $

/**
 * Credits
 * The original createMarkerIcon(), createLabeledMarkerIcon() and createFlatIcon()
 * were written by Pamela Fox as part of the MapIconMaker project.
 * See: http://code.google.com/p/gmaps-utility-library/
 */

/**
 * Required when used on admin pages.
 */
Drupal.gmaps = Drupal.gmaps || {};

Drupal.gmaps.icon = Drupal.gmaps.icon || new (function(){
  var self = this;
  this.icons = {};

  this.createIcon = function(opts) {
    if (this.hooks[opts.type]) {
      var icon = this.hooks[opts.type](opts);
      return icon || G_DEFAULT_ICON;
    }
  };
  
  this.getIcon = function(id, refresh) {
    id = 'icon_'+ id;
    if (self.icons[id]) {
      if (refresh) {
        self.icons[id] = null;
      }
      else {
        return self.icons[id];
      }
    }
    if (Drupal.settings.gmaps.icon && Drupal.settings.gmaps.icon[id]) {
      self.icons[id] = self.createIcon(Drupal.settings.gmaps.icon[id]);
    }
    return self.icons[id];
  };
  
  this.escapeLabel = function (text) {
    if (text === undefined) {
      return "";
    }
    text = text.replace(/@/, "@@");
    text = text.replace(/\\/, "@\\");
    text = text.replace(/'/, "@'");
    text = text.replace(/\[/, "@[");
    text = text.replace(/\]/, "@]");
    return Drupal.encodeURIComponent(text);
  };
  
  this.setDragCross = function(icon, opts) {
    if (opts.maxheight && opts.maxheight.length) {
      icon.maxHeight = opts.maxheight;
    }
    
    if (opts.drag_cross_image && opts.drag_cross_image.length) {
      icon.dragCrossImage = opts.drag_cross_image;
      icon.dragCrossSize = new GSize(opts.drag_cross_size.width, opts.drag_cross_size.height);
      icon.dragCrossAnchor = new GPoint(opts.drag_cross_anchor.x, opts.drag_cross_anchor.y);
    }
  };
  
  this.createCustomIcon = function(opts) {
    var icon = new GIcon(G_DEFAULT_ICON, opts.icon);
    
    if (opts.icon_size) {
      icon.iconSize = new GSize(opts.icon_size.width, opts.icon_size.height);
    }
    if (opts.icon_anchor) {
      icon.iconAnchor = new GPoint(opts.icon_anchor.x, opts.icon_anchor.y);
    }

    if (opts.shadow && opts.shadow.length) {
      icon.shadow = opts.shadow;
    }
    if (opts.shadow_size) {
      icon.shadowSize = new GSize(opts.shadow_size.width, opts.shadow_size.height);
    }

    if (opts.info_window_anchor) {
      icon.infoWindowAnchor = new GPoint(opts.info_window_anchor.x, opts.info_window_anchor.y);
    }

    if (opts.print_image && opts.print_image.length) {
      icon.printImage = opts.print_image;
    }
    if (opts.moz_print_image && opts.moz_print_image.length) {
      icon.mozPrintImage = opts.moz_print_image;
    }
    if (opts.print_shadow && opts.print_shadow.length) {
      icon.printShadow = opts.print_shadow;
    }
    if (opts.transparent && opts.transparent.length) {
      icon.transparent = opts.transparent;
    }

    if (opts.imagemap && opts.imagemap.length) {
      var imap = opts.imagemap.split(/\s*,\s*/);
      for (var i = 0; i < imap.length; i++) {
        imap[i] = parseInt(imap[i]);
      }
      icon.imageMap = imap;
    }
    
    self.setDragCross(icon, opts);
    
    return icon;
  };
  
  this.createMarkerIcon = function(opts) {
    if (opts.data.cache && !opts.throttle) {
      return self.createCustomIcon(opts);
    }  
    
    var icon = new GIcon(G_DEFAULT_ICON);
    
    var width = opts.icon_size.width || 32;
    var height = opts.icon_size.height || 32;
    var primaryColor = opts.data.primary_color || "#ff0000";
    var strokeColor = opts.data.stroke_color || "#000000";
    var cornerColor = opts.data.corner_color || "#ffffff";
     
    var baseUrl = "http://chart.apis.google.com/chart?cht=mm";
    var iconUrl = baseUrl + "&chs=" + width + "x" + height + 
        "&chco=" + cornerColor.replace("#", "") + "," + 
        primaryColor.replace("#", "") + "," + 
        strokeColor.replace("#", "");
    
    icon.image = iconUrl + "&ext=.png";
    icon.iconSize = new GSize(width, height);
    icon.shadowSize = new GSize(Math.floor(width * 1.6), height);
    icon.iconAnchor = new GPoint(width / 2, height);
    icon.infoWindowAnchor = new GPoint(width / 2, Math.floor(height / 12));
    icon.printImage = iconUrl + "&chof=gif";
    icon.mozPrintImage = iconUrl + "&chf=bg,s,ECECD8" + "&chof=gif";
    icon.transparent = iconUrl + "&chf=a,s,ffffff11&ext=.png";

    icon.imageMap = [
      width / 2, height,
      (7 / 16) * width, (5 / 8) * height,
      (5 / 16) * width, (7 / 16) * height,
      (7 / 32) * width, (5 / 16) * height,
      (5 / 16) * width, (1 / 8) * height,
      (1 / 2) * width, 0,
      (11 / 16) * width, (1 / 8) * height,
      (25 / 32) * width, (5 / 16) * height,
      (11 / 16) * width, (7 / 16) * height,
      (9 / 16) * width, (5 / 8) * height
    ];
    for (var i = 0; i < icon.imageMap.length; i++) {
      icon.imageMap[i] = parseInt(icon.imageMap[i]);
    }

    self.setDragCross(icon, opts);
    
    return icon;
  };
  
  this.createLabeledMarkerIcon = function (opts) {
    if (opts.data.cache && !opts.throttle) {
      return self.createCustomIcon(opts);
    }  
    
    var icon = new GIcon(G_DEFAULT_ICON);
    
    var pinStyle = opts.data.pin_style || 'pin';
    var label = self.escapeLabel(opts.label) || "";
    var labelCut = opts.data.label_cut || 2;
    var width = 21;
    var height = 34;
    if (pinStyle == 'pin_star') {
      width = 23;
      height = 39;
    }  
    else if (pinStyle == 'pin_sleft' || pinStyle == 'pin_sright') {
      width = 23;
      height = 33;
    }  
    var primaryColor = opts.data.primary_color || "#FF0000";
    var strokeColor = opts.data.stroke_color || "#000000";
    var labelColor = opts.data.label_color || "#000000";
    var starPrimaryColor = opts.data.star_primary_color || "#FFFF00";
    var starStrokeColor = opts.data.star_stroke_color || "#0000FF";
    
    var baseUrl = "http://chart.apis.google.com/chart?cht=d&chdp=mapsapi&chl=";
    var iconUrl = baseUrl + pinStyle + "'i\\" + "'[" + label + 
        "'-" + labelCut + "'f\\"  + "hv'a\\]" + "h\\]o\\" + 
        primaryColor.replace("#", "")  + "'fC\\" + 
        labelColor.replace("#", "")  + "'tC\\" + 
        strokeColor.replace("#", "")  + "'eC\\";
    if (pinStyle == 'pin_star') {
      iconUrl += starPrimaryColor.replace("#", "") + "'1C\\" + 
          starStrokeColor.replace("#", "") + "'0C\\";
    }

    icon.image = iconUrl + "Lauto'f\\&ext=.png";
    icon.iconSize = new GSize(width, height);
    icon.shadowSize = new GSize(Math.floor(width * 1.6), height);
    icon.iconAnchor = new GPoint(width / 2, height);
    icon.infoWindowAnchor = new GPoint(width / 2, Math.floor(height / 12));
    icon.printImage = iconUrl + "&chof=gif";
    icon.mozPrintImage = iconUrl + "&chf=bg,s,ECECD8" + "&chof=gif";
    icon.transparent = iconUrl + "Lauto'f\\&chf=a,s,ffffff11&ext=.png";

    icon.imageMap = [
      width / 2, height,
      (7 / 16) * width, (5 / 8) * height,
      (5 / 16) * width, (7 / 16) * height,
      (7 / 32) * width, (5 / 16) * height,
      (5 / 16) * width, (1 / 8) * height,
      (1 / 2) * width, 0,
      (11 / 16) * width, (1 / 8) * height,
      (25 / 32) * width, (5 / 16) * height,
      (11 / 16) * width, (7 / 16) * height,
      (9 / 16) * width, (5 / 8) * height
    ];
    for (var i = 0; i < icon.imageMap.length; i++) {
      icon.imageMap[i] = parseInt(icon.imageMap[i]);
    }

    self.setDragCross(icon, opts);
    
    return icon;
  };
  
  this.createFlatIcon = function (opts) {
    if (opts.data.cache && !opts.throttle) {
      return self.createCustomIcon(opts);
    }  
    
    var icon = new GIcon(G_DEFAULT_ICON);
    
    var width = opts.icon_size.width || 32;
    var height = opts.icon_size.height || 32;
    var labelColor = opts.data.label_color || "#000000";
    var primaryColor = opts.data.primary_color || "#FF0000";
    var shadowFromColor = opts.data.shadow_from_color || "#000000";
    var shadowToColor = opts.data.shadow_to_color || "#000000";
    var labelSize = opts.data.label_size || 0;
    var shadowFromTransparency = opts.data.shadow_from_transparency || "FF";
    var shadowToTransparency = opts.data.shadow_to_transparency || "01";
    var shape = opts.data.shape ||  "circle";
    var shapeCode = (shape === "circle") ? "it" : "itr";
    var label = Drupal.encodeURIComponent(opts.label) || "";

    var baseUrl = "http://chart.apis.google.com/chart?cht=" + shapeCode;
    var iconUrl = baseUrl + "&chs=" + width + "x" + height + 
        "&chco=" + primaryColor.replace("#", "") + "," + 
        shadowFromColor.replace("#", "") + shadowFromTransparency + "," + shadowToColor.replace("#", "") + shadowToTransparency +
        "&chl=" + label + "&chx=" + labelColor.replace("#", "") + 
        "," + labelSize;

    icon.image = iconUrl + "&chf=bg,s,00000000&ext=.png";
    icon.iconSize = new GSize(width, height);
    icon.shadowSize = new GSize(0, 0);
    icon.iconAnchor = new GPoint(width / 2, height / 2);
    icon.infoWindowAnchor = new GPoint(width / 2, height / 2);
    icon.printImage = iconUrl + "&chof=gif";
    icon.mozPrintImage = iconUrl + "&chf=bg,s,ECECD8&chof=gif";
    icon.transparent = iconUrl + "&chf=a,s,ffffff01&ext=.png";
    icon.imageMap = []; 
    if (shapeCode === "itr") {
      icon.imageMap = [0, 0, width, 0, width, height, 0, height];
    } else {
      var polyNumSides = 8;
      var polySideLength = 360 / polyNumSides;
      var polyRadius = Math.min(width, height) / 2;
      for (var a = 0; a < (polyNumSides + 1); a++) {
        var aRad = polySideLength * a * (Math.PI / 180);
        var pixelX = polyRadius + polyRadius * Math.cos(aRad);
        var pixelY = polyRadius + polyRadius * Math.sin(aRad);
        icon.imageMap.push(parseInt(pixelX), parseInt(pixelY));
      }
    }

    self.setDragCross(icon, opts);
    
    return icon;
  };
  
  this.createIconicMarkerIcon = function (opts) {
    if (opts.data.cache && !opts.throttle) {
      return self.createCustomIcon(opts);
    }  
    
    var icon = new GIcon(G_DEFAULT_ICON);
    
    var pinStyle = opts.data.pin_style || 'pin';
    var pinIcon = opts.data.pin_icon || 'home';
    var width = 21;
    var height = 34;
    if (pinStyle == 'pin_star') {
      width = 23;
      height = 39;
    }  
    else if (pinStyle == 'pin_sleft' || pinStyle == 'pin_sright') {
      width = 23;
      height = 33;
    }  
    var primaryColor = opts.data.primary_color || "#FF0000";
    var starColor = opts.data.star_color || "#FFFF00";
    
    var baseUrl = "http://chart.apis.google.com/chart?chst=d_map_xpin_icon&chld=";
    var shadowBaseUrl = "http://chart.apis.google.com/chart?chst=d_map_xpin_shadow&chld=";
    var iconUrl = baseUrl + pinStyle + "|" + pinIcon + "|" +
        primaryColor.replace("#", "") + "|" + starColor.replace("#", "");

    icon.image = iconUrl + "&ext=.png";
    icon.iconSize = new GSize(width, height);
    icon.shadow = shadowBaseUrl + pinStyle + "&ext=.png";
    icon.shadowSize = new GSize(Math.floor(width * 1.6), height);
    icon.iconAnchor = new GPoint(width / 2, height);
    icon.infoWindowAnchor = new GPoint(width / 2, Math.floor(height / 12));
    icon.printImage = iconUrl + "&chof=gif";
    icon.mozPrintImage = iconUrl + "&chf=bg,s,ECECD8" + "&chof=gif";
    icon.transparent = iconUrl + "&chf=a,s,ffffff11&ext=.png";

    icon.imageMap = [
      width / 2, height,
      (7 / 16) * width, (5 / 8) * height,
      (5 / 16) * width, (7 / 16) * height,
      (7 / 32) * width, (5 / 16) * height,
      (5 / 16) * width, (1 / 8) * height,
      (1 / 2) * width, 0,
      (11 / 16) * width, (1 / 8) * height,
      (25 / 32) * width, (5 / 16) * height,
      (11 / 16) * width, (7 / 16) * height,
      (9 / 16) * width, (5 / 8) * height
    ];
    for (var i = 0; i < icon.imageMap.length; i++) {
      icon.imageMap[i] = parseInt(icon.imageMap[i]);
    }

    self.setDragCross(icon, opts);
    
    return icon;
  };
  
  this.createScaledMarkerIcon = function (opts) {
    if (opts.data.cache && !opts.throttle) {
      return self.createCustomIcon(opts);
    }  
    
    var icon = new GIcon(G_DEFAULT_ICON);
    
    var scale = opts.data.scale || 0.5;
    var rotation = opts.data.rotation || 0;
    var labelSize = opts.data.label_size || 10;
    var labelStyle = opts.data.label_style || '_';

    var iconSize = opts.icon_size || {'width': null, 'height': null};
    var width = iconSize.width || Math.ceil(37 * scale);
    var height = iconSize.height || Math.ceil(66 * scale);

    var primaryColor = opts.data.primary_color || "#FF0000";
    var label = "";
    if (opts.label) {
      var labels = opts.label.split('|');
      for(var i in labels) {
        labels[i] = Drupal.encodeURIComponent(labels[i]);
      }
      label = labels.join('|');
    }

    
    var baseUrl = "http://chart.apis.google.com/chart?chst=d_map_spin&chld=";
    var iconUrl = baseUrl + scale + "|" + rotation + "|" +
        primaryColor.replace("#", "") + "|" + labelSize + "|" + labelStyle + "|" + label;

    icon.image = iconUrl + "&ext=.png";
    icon.iconSize = new GSize(width, height);
    icon.shadowSize = new GSize(0, 0);
    icon.iconAnchor = new GPoint(width / 2, height);
    icon.infoWindowAnchor = new GPoint(width / 2, Math.floor(height / 12));
    icon.printImage = iconUrl + "&chof=gif";
    icon.mozPrintImage = iconUrl + "&chf=bg,s,ECECD8" + "&chof=gif";
    icon.transparent = iconUrl + "&chf=a,s,ffffff11&ext=.png";

    icon.imageMap = [
      width / 2, height,
      (7 / 16) * width, (5 / 8) * height,
      (5 / 16) * width, (7 / 16) * height,
      (7 / 32) * width, (5 / 16) * height,
      (5 / 16) * width, (1 / 8) * height,
      (1 / 2) * width, 0,
      (11 / 16) * width, (1 / 8) * height,
      (25 / 32) * width, (5 / 16) * height,
      (11 / 16) * width, (7 / 16) * height,
      (9 / 16) * width, (5 / 8) * height
    ];
    for (var i = 0; i < icon.imageMap.length; i++) {
      icon.imageMap[i] = parseInt(icon.imageMap[i]);
    }

    self.setDragCross(icon, opts);
    
    return icon;
  };

  this.hooks = {
    'custom': this.createCustomIcon,
    'marker': this.createMarkerIcon,
    'labeled_marker': this.createLabeledMarkerIcon,
    'flat': this.createFlatIcon,
    'iconic_marker': this.createIconicMarkerIcon,
    'scaled_marker': this.createScaledMarkerIcon
  };

})();
