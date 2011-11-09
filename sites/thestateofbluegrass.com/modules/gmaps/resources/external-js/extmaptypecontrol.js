/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @name ExtMapTypeControl
 * @version 1.5
 * @copyright 2007 Google
 * @author Pamela Fox, others 
 * Added More button and buttons to save and restore position functionality.
 * This version of ExtMapTypeControl can also be used together with
 * GMapTypeControl, GHierarchicalMapTypeControl, GMenuMapTypeControl,
 * setUIToDefault and all self created map type buttons.
 * @copyright 2009 Wolfgang Pichler
 * @author Wolfgang Pichler (Pil), www.wolfpil.de
 * @fileoverview
 * <p>This class lets you add a control to the map which mimics GMapTypeControl
 * and allows for the addition of a traffic button/traffic key
 * plus a more button to display additional layers
 * and buttons to save the map position and return to it.
 * </p>
 */

/**
 * @name ExtMapTypeControlOptions
 * @class This class represents optional arguments to {@link ExtMapTypeControl}.
 * @desc Either option 'useMapTypes' or 'posRight' should be used.
 * @property {Boolean} [useMapTypes] Controls whether map type buttons are shown.
 * @property {Integer} [posRight] Defines the spacing in pixels of the button from the right map border.
 * @property {Boolean} [showTraffic] Controls whether traffic button is shown.
 * @property {Boolean} [showTrafficKey] Controls whether traffic key is shown.
 * @property {Boolean} [showMore] Controls whether more button is shown.
 * @property {Boolean} [showSave] Controls whether save/return buttons are shown.
 */

/**
 * @desc Creates a control with options specified in {@link ExtMapTypeControlOptions}.
 * @param {ExtMapTypeControlOptions} [opt_opts] Named optional arguments.
 * @constructor
 */
function ExtMapTypeControl(opt_opts) {
  this.options = opt_opts || {};
}

/**
 * Extends GOverlay class from the Google Maps API
 */
ExtMapTypeControl.prototype = new GControl();

/**
 * @desc Initialize the control on the map.
 * Creates the buttons for the map types and appends them to the map.
 * @param {GMap2} map The map that has had this ExtMapTypeControl added to it.
 * @return {DOM Object} Div that holds the map types buttons
 * @private
 */
ExtMapTypeControl.prototype.initialize = function(map) {
 var me = this;
 var extDiv = document.createElement("div");
 var posX;

 if (me.options.useMapTypes) {
  var mapTypes = map.getMapTypes();
  var mapTypeDivs = me.addMapTypeButtons_(map);

  GEvent.addListener(map, "addmaptype", function() {
    var newMapTypes = map.getMapTypes();
    var newMapType = newMapTypes.pop();
    var newMapTypeDiv = me.createButton_(newMapType.getName());
    newMapTypeDiv.setAttribute('title', newMapType.getAlt());
    mapTypes.push(newMapType);
    mapTypeDivs.push(newMapTypeDiv);
    me.resetButtonEvents_(map, mapTypeDivs);
    extDiv.appendChild(newMapTypeDiv);
  });
  GEvent.addListener(map, "removemaptype", function() {
    for (var i = 0; i < mapTypeDivs.length; i++) {
      GEvent.clearListeners(mapTypeDivs[i], "click");
      extDiv.removeChild(mapTypeDivs[i]);
    }
    mapTypeDivs = me.addMapTypeButtons_(map);
    me.resetButtonEvents_(map, mapTypeDivs);
    for (var i = 0; i < mapTypeDivs.length; i++) {
      extDiv.appendChild(mapTypeDivs[i]);
    }
  });

   for (var i = 0; i < mapTypeDivs.length; i++) {
    me.toggleButton_(mapTypeDivs[i].firstChild, false);
    extDiv.appendChild(mapTypeDivs[i]);
    if(map.getCurrentMapType().getName() == mapTypeDivs[i].name) {
     me.toggleButton_(mapTypeDivs[i].firstChild, true);
    }
   }
   map.getContainer().appendChild(extDiv);
   // Sets the proper spaces between the buttons
   posX = 98;
   switch (mapTypes.length) {
    case 2: posX += 72; break;
    case 3: posX += 144; break;
    case 4: posX += 216; break;
   }
 }
  else {
   // If no options were defined, 'posRight: 220' is assumed.
   posX = me.options.posRight || 220;
  }

  /*
  *  Loads SavePosControl when specified as option
  */
  if (me.options.showSave) {
   map.addControl(new SavePosControl(),
    new GControlPosition(G_ANCHOR_TOP_RIGHT, new GSize(7, 31)));
  }

  /*
  *  Loads MoreControl when specified as option
  */
  if (me.options.showMore) {
   var layers = [
    { name: "Photos", obj: new GLayer("com.panoramio.all") },
    { name: "Videos", obj: new GLayer("com.youtube.all") },
    { name: "Wikipedia", obj: new GLayer("org.wikipedia.en") },
    { name: "Webcams", obj: new GLayer("com.google.webcams") }
   ];

   map.addControl(new MoreControl(layers),
    new GControlPosition(G_ANCHOR_TOP_RIGHT, new GSize(posX, 7)));
  }

  /*
  *  Loads TrafficControl when specified as option
  */
  if (me.options.showTraffic) {
    // Moves traffic button to the left if more button should also be displayed
    if(me.options.showMore)
     posX += 94;
    map.addControl(new TrafficControl(me.options),
     new GControlPosition(G_ANCHOR_TOP_RIGHT, new GSize(posX, 7)));
  }
  return extDiv;
};

/**
 * Creates buttons for map types.
 * @param {GMap2} Map object for which to create buttons.
 * @return {Array} Divs containing the buttons.
 * @private
 */
ExtMapTypeControl.prototype.addMapTypeButtons_ = function(map) {
  var me = this;
  var mapTypes = map.getMapTypes();
  var mapTypeDivs = [];
  for (var i = 0; i < mapTypes.length; i++) {
    mapTypeDivs[i] = me.createButton_(mapTypes[i].getName());
    mapTypeDivs[i].name = mapTypes[i].getName();
    mapTypeDivs[i].setAttribute('title', mapTypes[i].getAlt());
  }
  me.resetButtonEvents_(map, mapTypeDivs);
  return mapTypeDivs;
};

/**
 * Ensures that map type button events are assigned correctly.
 * @param {GMap2} Map object for which to reset events.
 * @param {Array} mapTypeDivs Divs containing map type buttons.
 * @private
 */
ExtMapTypeControl.prototype.resetButtonEvents_ = function(map, mapTypeDivs) {
  var me = this;
  var mapTypes = map.getMapTypes();
  for (var i = 0; i < mapTypeDivs.length; i++) {
    var otherDivs = [];
    for (var j = 0; j < mapTypes.length; j++) {
      if (j != i) {
        otherDivs.push(mapTypeDivs[j]);
      }
    }
    me.assignButtonEvent_(mapTypeDivs[i], map, mapTypes[i], otherDivs);
  }
  GEvent.addListener(map, "maptypechanged", function() {
    var divIndex = 0;
    var mapType = map.getCurrentMapType();
    for (var i = 0; i < mapTypes.length; i++) {
      if (mapTypes[i] == mapType) {
        divIndex = i;
      }
    }
    GEvent.trigger(mapTypeDivs[divIndex], "click");
  });
};

/**
 * Creates buttons with text nodes. 
 * @param {String} text Text to display in button
 * @return {DOM Object} The div for the button.
 * @private
 */
ExtMapTypeControl.prototype.createButton_ = function(text) {
  var buttonDiv = document.createElement("div");
  this.setButtonStyle_(buttonDiv);
  buttonDiv.style.cssFloat = "left";
  buttonDiv.style.styleFloat = "left";
  var textDiv = document.createElement("div");
  textDiv.appendChild(document.createTextNode(text));
  textDiv.style.width = "6em";
  buttonDiv.appendChild(textDiv);
  return buttonDiv;
};

/**
 * Assigns events to MapType buttons to change maptype
 * and toggle button styles correctly for all buttons
 * when button is clicked.
 * @param {DOM Object} div Button's div to assign click to
 * @param {GMap2} Map object to change maptype of.
 * @param {Object} mapType GMapType to change map to when clicked
 * @param {Array} otherDivs Array of other button divs to toggle off
 * @private
 */  
ExtMapTypeControl.prototype.assignButtonEvent_ = function(div, map, mapType, otherDivs) {
  var me = this;
  GEvent.addDomListener(div, "click", function() {
    for (var i = 0; i < otherDivs.length; i++) {
      me.toggleButton_(otherDivs[i].firstChild, false);
    }
    me.toggleButton_(div.firstChild, true);
    map.setMapType(mapType);
  });
};

/**
 * Changes style of button to appear on/off depending on boolean passed in.
 * @param {DOM Object} div inner button div to change style of
 * @param {Boolean} boolCheck Used to decide to use on style or off style
 * @private
 */
ExtMapTypeControl.prototype.toggleButton_ = function(div, boolCheck) {
  div.style.fontWeight = boolCheck ? "bold" : "normal";
  div.style.border = boolCheck ? "1px solid #483d8b" : "1px solid #fff";
  var shadow = boolCheck ? "#6495ed" : "#c0c0c0";
  var edges = ["RightColor", "BottomColor"];
   for (var j = 0; j < edges.length; j++) {
    div.style["border" + edges[j]] = shadow;
   }
};

/**
 * Required by GMaps API for controls. 
 * @return {GControlPosition} Default location for map types buttons
 * @private
 */
ExtMapTypeControl.prototype.getDefaultPosition = function() {
  return new GControlPosition(G_ANCHOR_TOP_RIGHT, new GSize(7, 7));
};

/**
 * Sets the proper CSS for the given button element.
 * @param {DOM Object} button Button div to set style for
 * @private
 */
ExtMapTypeControl.prototype.setButtonStyle_ = function(button) {
  button.style.color = "#000000";
  button.style.backgroundColor = "white";
  button.style.font = "small Arial";
  button.style.border = "1px solid black";
  button.style.padding = "0px";
  button.style.margin= "0px";
  button.style.textAlign = "center";
  button.style.fontSize = "12px"; 
  button.style.cursor = "pointer";
};

/**
 * @desc Constructor for TrafficControl.
 * Option hash to decide whether traffic key is shown.
 */
function TrafficControl(opt_opts) {
  this.options = opt_opts;
}

/*
 * It's more efficient to inherit ExtMapTypeControl's prototypes only
 */
function Inherit() {};
Inherit.prototype = ExtMapTypeControl.prototype;
TrafficControl.prototype = new Inherit();

/**
 * Creates the div that holds the traffic button
 * and - if specified - appends the div that holds the traffic key.
 * @param {GMap2} map The map that has had this Control added to it.
 * @return {DOM Object} Div that holds the button.
 * @private 
 */
TrafficControl.prototype.initialize = function(map) {
  var me = this;
  var trafficDiv = me.createButton_("Traffic");
  trafficDiv.setAttribute("title", "Show Traffic");
  trafficDiv.style.visibility = "hidden";
  trafficDiv.style.width = "6em";
  trafficDiv.firstChild.style.cssFloat = "left";
  trafficDiv.firstChild.style.styleFloat = "left";
  me.toggleButton_(trafficDiv.firstChild, false);

  // Sending true makes traffic overlay hidden by default
  var trafficInfo = new GTrafficOverlay({hide: true});
  trafficInfo.hidden = true;

  // Checks whether traffic data is available in viewport,
  // shows and hides the traffic button accordingly.
  GEvent.addListener(trafficInfo, "changed", function(hasTrafficInView) {
   if (hasTrafficInView) {
      trafficDiv.style.visibility = "visible";
   } else {
      trafficDiv.style.visibility = "hidden";
     }
  });
  map.addOverlay(trafficInfo);

  GEvent.addDomListener(trafficDiv.firstChild, "click", function() {
    if (trafficInfo.hidden) {
     trafficInfo.hidden = false;
     trafficInfo.show();
    } else {
     trafficInfo.hidden = true;
     trafficInfo.hide();
    }
    me.toggleButton_(trafficDiv.firstChild, !trafficInfo.hidden);
  });

  /*
  *  Appends traffic key when defined as option
  */
  if(me.options.showTrafficKey) {
   trafficDiv.style.width = "7.8em";
   var keyDiv = document.createElement("div");
   keyDiv.style.width = "1.5em";
   keyDiv.style.position = "absolute";
   keyDiv.style.top = "0px";
   keyDiv.style.right = "0px";
   keyDiv.innerHTML = "?";

   var keyExpandedDiv = document.createElement("div");
   keyExpandedDiv.style.clear = "both";
   keyExpandedDiv.style.padding = "2px";
   var keyInfo = [{"color": "#30ac3e", "text": "&gt; 50 MPH"},
                  {"color": "#ffcf00", "text": "25-50 MPH"},
                  {"color": "#ff0000", "text": "&lt; 25 MPH"},
                  {"color": "#c0c0c0", "text": "No data"}];
    for (var i = 0; i < keyInfo.length; i++) {
      keyExpandedDiv.innerHTML += "<div style='text-align: left'><span style='background-color: " + keyInfo[i].color + "'>&nbsp;&nbsp;</span>"
    +  "<span style='color: " + keyInfo[i].color + "'> " + keyInfo[i].text + " </span>" + "</div>";
    }
    keyExpandedDiv.style.display = "none";

    GEvent.addDomListener(keyDiv, "click", function() {
      if (me.keyExpanded) {
        me.keyExpanded = false;
        keyExpandedDiv.style.display = "none";
      }
       else {
        me.keyExpanded = true;
        keyExpandedDiv.style.display = "block";
       }
       me.toggleButton_(keyDiv, me.keyExpanded);
    });
    me.toggleButton_(keyDiv, me.keyExpanded);
    trafficDiv.appendChild(keyDiv);
    trafficDiv.appendChild(keyExpandedDiv);
  }
  map.getContainer().appendChild(trafficDiv);
  return trafficDiv;
};


/**
 * @desc Constructor for MoreControl.
 * Immutable shared property moved to prototype.
 */
function MoreControl(layers) {
  MoreControl.prototype.layers = layers;
  this.chosen_ = [];
  this.boxes_ = [];
}

/**
* Inherits ExtMapTypeControl's prototypes only
*/
MoreControl.prototype = new Inherit();

/*
 * Primarily creates an outer div that holds 
 * all necessary elements needed for the more button.
 * @param {GMap2} map The map that has had this Control added to it.
 * @return {DOM Object} Div that holds all button elements.
 * @private
 */
MoreControl.prototype.initialize = function(map) {
  var me = this;
  me.map_ = map;
  var outer = document.createElement("div");
  me.moreDiv = me.createButton_("More...");
  me.moreDiv.setAttribute("title", "Show/Hide Layers");
  me.moreDiv.firstChild.style.width = "7em";
  me.toggleButton_(me.moreDiv.firstChild, false);
  outer.appendChild(me.moreDiv);
  outer.appendChild(me.createLayerBox_());

  GEvent.addDomListener(outer, "mouseover", function() {
  if(window.timer) clearTimeout(timer);
   me.layerboxDiv.style.display = "block";
   me.moreDiv.firstChild.style.height = "23px";
  });
  GEvent.addDomListener(outer, "mouseout", function() {
   timer = window.setTimeout(function() {
    me.layerboxDiv.style.display = "none";
    me.moreDiv.firstChild.style.height = "";
   }, 300);
  });
  GEvent.addDomListener(me.moreDiv, "click", function() {
   if(me.chosen_.length > 0 ) {
    /* Makes an independent copy of chosen array since it will be
    *  reset by switchLayer, which might not be useful here
    */
    var copy = me.chosen_.slice();
    for(var i = 0; i < copy.length; i++) {
     var index = parseInt(copy[i]);
     me.switchLayer_(true, me.layers[index].obj);
     me.boxes_[index].checked = true;
    }
   }
   else {
    me.hideAll_();
   }
  });
 map.getContainer().appendChild(outer);
 return outer;
};

/**
 * Primarily creates the outer div that holds the checkboxes.
 * @return {DOM Object} Div that holds all elements underneath the More...Button.
 * @private
 */
MoreControl.prototype.createLayerBox_ = function() {
  var me = this;
  me.layerboxDiv = document.createElement("div");
  // For nested elements position:absolute means relative to its parent
  me.layerboxDiv.style.position = "absolute";
  me.layerboxDiv.style.top = "20px";
  me.layerboxDiv.style.left = "0px";
  me.layerboxDiv.style.marginTop = "-1px";
  me.layerboxDiv.style.font = "small Arial";
  me.layerboxDiv.style.fontSize = "12px";
  me.layerboxDiv.style.padding = "4px";
  me.layerboxDiv.style.width = "120px";
  me.layerboxDiv.style.color = "#000";
  me.layerboxDiv.style.backgroundColor = "#fff";
  me.layerboxDiv.style.border = "1px solid gray";
  me.layerboxDiv.style.borderTopColor = "#e2e2e2";
  me.layerboxDiv.style.cursor = "default";

  var input = [];
  for (var i = 0; i < me.layers.length; i++) {
   input[i] = me.createCheckbox_(i, me.layers[i].name);
   me.layerboxDiv.appendChild(input[i] );
  }

  var ruler = document.createElement("hr");
  ruler.style.width = "92%";
  ruler.style.height = "1px";
  ruler.style.textAlign = "center";
  ruler.style.border = "1px";
  ruler.style.color = "#e2e2e2";
  ruler.style.backgroundColor = "#e2e2e2";
  var boxlink = document.createElement("a");
  boxlink.setAttribute("href", "javascript:void(0)");
  boxlink.style.color = "#a5a5a5";
  boxlink.style.textDecoration = "none";
  boxlink.style.cursor = "default";
  boxlink.style.marginLeft = "33px";
  boxlink.appendChild(document.createTextNode("Hide all"));

  me.layerboxDiv.appendChild(ruler);
  me.layerboxDiv.appendChild(boxlink);

  GEvent.addDomListener(boxlink, "click", function() {
   me.hideAll_();
  });
  me.layerboxDiv.style.display = "none";
  return me.layerboxDiv;
};

/**
 * Creates checkboxes with a click event inside of a div element.
 * @param {Number} nr The array index of the layers array
 * @param {String} name The name of the layer the checkbox belongs to
 * @return {DOM Object} Div that holds the checkbox and its related text node
 * @private
 */
MoreControl.prototype.createCheckbox_ = function(nr, name) {
  var me = this;
  var innerDiv = document.createElement("div");
  var checkbox = document.createElement("input");
  checkbox.setAttribute("type", "checkbox");
  var textSpan = document.createElement("span");
  textSpan.style.marginLeft = "2px";
  textSpan.appendChild(document.createTextNode(name));
  innerDiv.appendChild(checkbox);
  innerDiv.appendChild(textSpan);
  innerDiv.appendChild(document.createElement("br"));
  me.boxes_.push(checkbox);

  GEvent.addDomListener(checkbox, "click", function() {
   me.switchLayer_(this.checked, me.layers[nr].obj);
  });
  return innerDiv;
};

/**
 * Changes style of layerbox to appear on/off depending on passed boolean.
 * @param {DOM Object} elem element to change style of
 * @param {Boolean} boolCheck Used to decide between on or off style
 * @private
 */
MoreControl.prototype.toggleBox_ = function(elem, boolCheck) {
  elem.style.borderWidth = boolCheck ? "2px": "1px";
  elem.style.width = boolCheck ? "119px" : "120px";
};

/**
 * Adds and removes the chosen layers to/from the map.
 * Styles the link inside the layer box and the more button accordingly.
 * @param {Boolean} checked Value of checked or unchecked checkbox
 * @param {Object} layer The GLayer object to add or to remove
 * @private
 */
MoreControl.prototype.switchLayer_ = function(checked, layer) {
  var me = this;
  var link = me.layerboxDiv.lastChild;
  var button = me.moreDiv.firstChild;
  if(checked) {
   me.map_.addOverlay(layer);
   // Resets chosen array
   me.chosen_.length = 0;
   /* 
   *  Toggles the elements
   */
   link.style.color = "#0000cd";
   link.style.textDecoration = "underline";
   link.style.cursor = "pointer";
   me.toggleButton_(button, true);
   me.toggleBox_(me.layerboxDiv, true);
  }
  else {
   me.map_.removeOverlay(layer);
   /*  Resets the elements
    * if all checkboxes were unchecked
   */
   if(!me.checkChecked()) {
    link.style.color = "#a5a5a5";
    link.style.textDecoration = "none";
    link.style.cursor = "default";
    me.toggleButton_(button, false);
    me.toggleBox_(me.layerboxDiv, false);
   }
  }
};

/**
 * Calls switchLayer to remove all displayed layers.
 * Stores index of removed layers in chosen array.
 * @private
 */
MoreControl.prototype.hideAll_ = function() {
  var me = this;
  for(var i = 0; i < me.boxes_.length; i++) {
   if(me.boxes_[i].checked) {
    me.boxes_[i].checked = false;
    me.switchLayer_(false, me.layers[i].obj);
    me.chosen_.push(i);
   }
  }
};

/**
 * Returns true if a checkbox is still checked, otherwise false.
 * @return {Boolean}
 */
MoreControl.prototype.checkChecked = function() {
  var me = this;
  for(var i = 0; i < me.boxes_.length; i++) {
   if(me.boxes_[i].checked) return true;
  }
  return false;
};


/**
 * @desc Constructor for SavePosControl.
 */
function SavePosControl() {};

/**
* Inherits ExtMapTypeControl's prototypes only
*/
SavePosControl.prototype = new Inherit();

/**
 * Creates the buttons for saving position and the back button.
 * @param {GMap2} map The map that has had this Control added to it.
 * @return {DOM Object} Div that holds both buttons.
 * @private
 */
SavePosControl.prototype.initialize = function(map) {
  var me = this;
  var saved = [];
  var saveDiv = document.createElement("div");
  var saveButtonDiv = document.createElement("div");
 
  saveButtonDiv.setAttribute("title", "Save actual position and zoomlevel");
  me.setButtonStyle_(saveButtonDiv);
  // Overwrites a few 'normal' styles of these buttons
  saveButtonDiv.style.width = "7em";
  saveButtonDiv.style.padding = "1px";
  saveButtonDiv.style.marginBottom = "4px";
  saveButtonDiv.style.whiteSpace = "nowrap";
  saveButtonDiv.appendChild(document.createTextNode("Save Position"));
  saveDiv.appendChild(saveButtonDiv);
  var backButtonDiv = document.createElement("div");
  backButtonDiv.setAttribute("title", "Back to saved position");
  me.setButtonStyle_(backButtonDiv);
  backButtonDiv.style.width = "7em";
  backButtonDiv.style.padding = "1px";
  backButtonDiv.appendChild(document.createTextNode("To Saved"));
  saveDiv.appendChild(backButtonDiv);

  GEvent.addDomListener(saveButtonDiv, "click", function() {
   var center = map.getCenter();
   var zoom = map.getZoom();
   saved.splice(0, 2, center, zoom);
   alert("Saved Position: "+ center.toUrlValue()+ "\nZoomlevel: "+ zoom);
  });
  GEvent.addDomListener(backButtonDiv, "click", function() {
   if (saved.length > 0) {
    map.setZoom(saved[1]);
    map.panTo(saved[0]);
   }
  });
 map.getContainer().appendChild(saveDiv);
 return saveDiv;
};
