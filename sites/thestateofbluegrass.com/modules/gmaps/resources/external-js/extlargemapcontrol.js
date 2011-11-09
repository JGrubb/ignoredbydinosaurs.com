/**
 * @name ExtLargeMapControl
 * @version 1.2
 * @author Masashi, Bjorn Brala
 * @fileoverview Creates a control with buttons to pan in four directions,
 * and zoom in and zoom out, and a zoom slider. The UI is based on the 
 * LargeMapControl from Google Maps (circa December 2008), but it does not
 * have any integration with Street View.
 */


/*global GKeyboardHandler, GDraggableObject*/

/**
 * @name ExtLargeMapControlOptions
 * @class This class represents optional arguments to the control.
 * @property {String} [zoomInBtnTitle="zoom in"] Specifies tooltip for 
 * zoom in button.
 * @property {String} [zoomOutBtnTitle="zoom out"] Specifies tooltip for 
 * zoom out button button.
 * @property {String} [moveNorthBtnTitle="north"] Specifies tooltip for 
 * pan north button.
 * @property {String} [moveSouthBtnTitle="south"] Specifies tooltip for 
 * pan south button.
 * @property {String} [moveEastBtnTitle="east"] Specifies tooltip for 
 * pan east button.
 * @property {String} [moveWestBtnTitle="west"] Specifies tooltip for 
 * pan west button.
 * @property {String} [returnBtnTitle="home position"] Specifies tooltip for 
 * center button that returns user to original location.
 * @property {String} [type] If set to "small", the control will consist of
 * only the zoom in/zoom out buttons.
 */

/**
 * @desc Creates an ExtLargeMapControl, with optional configuration settings.
 * @param {ExtLargeMapControlOptions} opts
 * @constructor
 */    
function ExtLargeMapControl(opts) {
  this.sliderStep = 9;
  this.imgSrc = "http://maps.google.com/mapfiles/mapcontrols3d.png";
  this.imgSmallSrc = "http://maps.google.com/mapfiles/szc3d.png";
  
  this.divTbl = {};
  this.divTbl.container = { "left" : 0, "top" : 0, "width" : 59};
  this.divTbl.topArrowBtn = { "left" : 20, "top" : 0, "width" : 18, "height" : 18};
  this.divTbl.leftArrowBtn = { "left" : 0, "top" : 20};
  this.divTbl.rightArrowBtn = { "left" : 40, "top" : 20};
  this.divTbl.bottomArrowBtn = { "left" : 20, "top" : 40};
  this.divTbl.centerBtn = { "left" : 20, "top" : 20};
  this.divTbl.zoomSlideBarContainer = { "left" : 19, "top" : 86, "width" : 22};
  this.divTbl.zoomSliderContainer = { "left" : 0, "top" : 0, "width" : 22, "height" : 14};
  this.divTbl.zoomSliderContainerImg = { "left" : 0, "top" : -384, "width" : 22, "height" : 14};
  this.divTbl.zoomOutBtnContainer = { "left" : 0, "top" : 0, "width" : 59, "height" : 23};
  this.divTbl.zoomOutBtnContainerImg = { "left" : 0, "top" : -360, "width" : 59, "height" : 23};

  opts = opts || {};
  this.zoomInBtnTitle = opts.zoomInBtnTitle || "zoom in";
  this.zoomOutBtnTitle = opts.zoomOutBtnTitle || "zoom out";
  this.moveNorthBtnTitle = opts.moveNorthBtnTitle || "north";
  this.moveSouthBtnTitle = opts.moveSouthBtnTitle || "south";
  this.moveEastBtnTitle = opts.moveEastBtnTitle || "east";
  this.moveWestBtnTitle = opts.moveWestBtnTitle || "west";
  this.homeBtnTitle = opts.homeBtnTitle || "home position";
  this.opts = opts;
  
  this.divSmallTbl = {};
  this.divSmallTbl.container = { "left" : 0, "top" : 0, "width" : 19, "height" : 42};
  this.divSmallTbl.zoomInBtn = { "left" : 0, "top" : 0, "width" : 19, "height" : 21};
  this.divSmallTbl.zoomOutBtnImg = { "left" : 0, "top" : -21, "width" : 19, "height" : 21};
  this.divSmallTbl.zoomOutBtn = { "left" : 0, "top" : 21, "width" : 19, "height" : 21};

}


/**
 * @private
 */
ExtLargeMapControl.prototype = new GControl();


/**
 * @desc Initialize the map control
 * @private
 */
ExtLargeMapControl.prototype.initialize = function (map) {

  this._map = map;

  GEvent.addListener(map, 'maptypechanged', GEvent.callback(this, this._updateZoomSliderRange));

  var _handleList = {};
  
  this._keyboardhandler = new GKeyboardHandler(map);
  var agt = navigator.userAgent.toLowerCase();
  
  this._is_ie    = ((agt.indexOf("msie") !== -1) && (agt.indexOf("opera") === -1));
  this._is_ie67  = (agt.indexOf("msie 6") !== -1 || agt.indexOf("msie 7"));
  this._is_ie8   = (agt.indexOf("msie 8") !== -1);
  this._is_gecko = (agt.indexOf('gecko') !== -1);
  this._is_opera = (agt.indexOf("opera") !== -1);

  //common image
  var commonImg = new Image();
  commonImg.src = this.imgSrc;

  var container;
  var zoomOutBtn;
  var zoomInBtn; 
  if (this.opts.type === "small") {
    // create container
    container = document.createElement("div");
    container.style.left = this.divSmallTbl.container.left + "px";
    container.style.top = this.divSmallTbl.container.top + "px";
    container.style.width = this.divSmallTbl.container.width + "px";
    container.style.height = this.divSmallTbl.container.height + "px";
    container.style.position = "absolute";
    container.style.overflow = "hidden";
    this._container = container;
    
    //zoom up button
    zoomInBtn = this.makeImgDiv_(this.imgSmallSrc, this.divSmallTbl.zoomInBtn);
    zoomInBtn.style.cursor = "pointer";
    zoomInBtn.title = this.zoomInBtnTitle;
    container.appendChild(zoomInBtn); 

    //zoom down button
    zoomOutBtn = this.makeImgDiv_(this.imgSmallSrc, this.divSmallTbl.zoomOutBtnImg);
    zoomOutBtn.style.cursor = "pointer";
    zoomOutBtn.style.overflow = "hidden";
    zoomOutBtn.style.position = "absolute";
    zoomOutBtn.style.left = this.divSmallTbl.zoomOutBtn.left + "px";
    zoomOutBtn.style.top = this.divSmallTbl.zoomOutBtn.top + "px";
    zoomOutBtn.style.width = this.divSmallTbl.zoomOutBtn.width + "px";
    zoomOutBtn.style.height = this.divSmallTbl.zoomOutBtn.height + "px";
    zoomOutBtn.title = this.zoomOutBtnTitle;
    container.appendChild(zoomOutBtn); 

    // events
    GEvent.bindDom(zoomOutBtn, "click", this, this._eventZoomOut);
    GEvent.bindDom(zoomInBtn, "click", this, this._eventZoomIn);
  } else {
    // calculation of controller size
    var currentMapType = map.getCurrentMapType();
    var minZoom = parseInt(currentMapType.getMinimumResolution(), 10);
    var maxZoom = parseInt(map.getCurrentMapType().getMaximumResolution(), 10);
    this._maxZoom = maxZoom;
    this._step = this.sliderStep;
    var ctrlHeight = (86 + 5) + (maxZoom - minZoom + 1) * this.sliderStep + 5;

    // create container
    container = this.makeImgDiv_(this.imgSrc, this.divTbl.container);
    container.style.height = (ctrlHeight + this.sliderStep + 2) + "px";
    _handleList.container = container;
    this._container = container;

    //top arrow button
    var topBtn = this.makeImgDiv_(this.imgSrc, this.divTbl.topArrowBtn);
    topBtn.style.cursor = "pointer";
    topBtn.style.left = "20px";
    topBtn.style.top = "0px";
    topBtn.title = this.moveNorthBtnTitle;
    container.appendChild(topBtn); 

    //left arrow button
    var leftBtn = topBtn.cloneNode(true);
    leftBtn.style.left = this.divTbl.leftArrowBtn.left + "px";
    leftBtn.style.top = this.divTbl.leftArrowBtn.top + "px";
    leftBtn.title = this.moveWestBtnTitle;
    container.appendChild(leftBtn); 

    //right arrow button
    var rightBtn = topBtn.cloneNode(true);
    rightBtn.style.left = this.divTbl.rightArrowBtn.left + "px";
    rightBtn.style.top = this.divTbl.rightArrowBtn.top + "px";
    rightBtn.title = this.moveEastBtnTitle;
    container.appendChild(rightBtn); 

    //bottom arrow button
    var bottomBtn = topBtn.cloneNode(true);
    bottomBtn.style.left = this.divTbl.bottomArrowBtn.left + "px";
    bottomBtn.style.top = this.divTbl.bottomArrowBtn.top + "px";
    bottomBtn.title = this.moveSouthBtnTitle;
    container.appendChild(bottomBtn); 

    //center button
    var homeBtn = topBtn.cloneNode(true);
    homeBtn.style.left = this.divTbl.centerBtn.left + "px";
    homeBtn.style.top = this.divTbl.centerBtn.top + "px";
    homeBtn.title = this.homeBtnTitle;
    container.appendChild(homeBtn); 

    _handleList.topBtn = topBtn;
    _handleList.leftBtn = leftBtn;
    _handleList.rightBtn = rightBtn;
    _handleList.bottomBtn = bottomBtn;
    _handleList.homeBtn = homeBtn;


    // zoom slider container
    var zoomSlideBarContainer = document.createElement("div");
    zoomSlideBarContainer.style.position  = "absolute";
    zoomSlideBarContainer.style.left = this.divTbl.zoomSlideBarContainer.left + "px";
    zoomSlideBarContainer.style.top = this.divTbl.zoomSlideBarContainer.top + "px";
    zoomSlideBarContainer.style.width = this.divTbl.zoomSlideBarContainer.width + "px";
    zoomSlideBarContainer.style.height = ((maxZoom - minZoom + 1) * this.sliderStep) + "px";
    zoomSlideBarContainer.style.overflow = "hidden";
    zoomSlideBarContainer.style.cursor = "pointer";
    container.appendChild(zoomSlideBarContainer); 
    _handleList.slideBar = zoomSlideBarContainer;

    // zoom slider Button
    var zoomLevel = map.getZoom();
    var zoomSliderContainer = this.makeImgDiv_(this.imgSrc, this.divTbl.zoomSliderContainerImg);
    
    zoomSliderContainer.style.top = ((maxZoom - zoomLevel) * this.sliderStep + 1) + "px";
    zoomSliderContainer.style.left = this.divTbl.zoomSliderContainer.left + "px";
    zoomSliderContainer.style.width = this.divTbl.zoomSliderContainer.width + "px";
    zoomSliderContainer.style.height = this.divTbl.zoomSliderContainer.height + "px";

    zoomSlideBarContainer.cursor = "url(http://maps.google.com/mapfiles/openhand.cur), default";
    zoomSlideBarContainer.appendChild(zoomSliderContainer); 
    _handleList.slideBarContainer = zoomSliderContainer;



    //zoomOut Btn container
    var zoomOutBtnContainer = this.makeImgDiv_(this.imgSrc, this.divTbl.zoomOutBtnContainerImg);
    zoomOutBtnContainer.style.top = (86 + (maxZoom - minZoom + 1) * this.sliderStep) + "px";
    zoomOutBtnContainer.style.left = this.divTbl.zoomOutBtnContainer.left + "px";
    zoomOutBtnContainer.style.width = this.divTbl.zoomOutBtnContainer.width + "px";
    zoomOutBtnContainer.style.height = this.divTbl.zoomOutBtnContainer.height + "px";

    zoomOutBtnContainer.cursor = "url(http://maps.google.com/mapfiles/openhand.cur), default";
    container.appendChild(zoomOutBtnContainer); 
    _handleList.zoomOutBtnContainer = zoomOutBtnContainer;


    //zoomOut button
    zoomOutBtn = document.createElement("div");
    zoomOutBtn.style.position = "absolute";
    zoomOutBtn.style.left = "20px";
    zoomOutBtn.style.top = (91 + (maxZoom - minZoom + 1) * this.sliderStep) + "px";
    zoomOutBtn.style.width = "18px";
    zoomOutBtn.style.height = "23px";
    zoomOutBtn.style.cursor = "pointer";
    zoomOutBtn.style.overflow = "hidden";
    zoomOutBtn.title = this.zoomOutBtnTitle;
    container.appendChild(zoomOutBtn); 
    _handleList.zoomOutBtn = zoomOutBtn;

    //zoomIn button
    zoomInBtn = document.createElement("div");
    zoomInBtn.style.position = "absolute";
    zoomInBtn.style.left = "20px";
    zoomInBtn.style.top = "65px";
    zoomInBtn.style.width = "18px";
    zoomInBtn.style.height = "23px";
    zoomInBtn.style.cursor = "pointer";
    zoomInBtn.style.overflow = "hidden";
    zoomInBtn.title = this.zoomInBtnTitle;
    container.appendChild(zoomInBtn); 
    _handleList.zoomInBtn = zoomInBtn;

    // events
    GEvent.bindDom(_handleList.topBtn, "click", this, this._eventTop);
    GEvent.bindDom(_handleList.leftBtn, "click", this, this._eventLeft);
    GEvent.bindDom(_handleList.rightBtn, "click", this, this._eventRight);
    GEvent.bindDom(_handleList.bottomBtn, "click", this, this._eventBottom);
    GEvent.bindDom(_handleList.homeBtn, "click", this, this._eventHome);
    GEvent.bindDom(_handleList.zoomOutBtn, "click", this, this._eventZoomOut);
    GEvent.bindDom(_handleList.zoomInBtn, "click", this, this._eventZoomIn);
    GEvent.bindDom(_handleList.slideBar, "click", this, this._eventSlideBar);
    GEvent.bind(map, "zoomend", this, this._eventZoomEnd);

    var drgOpt = {
      container : _handleList.slideBar
    };
    var drgCtrl = new GDraggableObject(_handleList.slideBarContainer, drgOpt);
    GEvent.bindDom(drgCtrl, "dragend", this, this._eventSlideDragEnd);
    this._slider =  drgCtrl;

    //set current slider position
    this._eventZoomEnd(map.getZoom(), map.getZoom());
  }

  // Save DOM element reference in the object.
  this._handleList = _handleList;

  map.getContainer().appendChild(container);
  
  return container;
};

/**
 * Update Zoomslider to ajust to Max and Min resolution on the current maptype
 * @private
**/
ExtLargeMapControl.prototype._updateZoomSliderRange = function (setMaxZoom) {
  
  
  var minZoom = parseInt(this._map.getCurrentMapType().getMinimumResolution(), 10);
  var maxZoom = parseInt(this._map.getCurrentMapType().getMaximumResolution(), 10);
  if (this.isNull(setMaxZoom) === false) {
    maxZoom = setMaxZoom;
  } else {
    this._maxZoom = maxZoom;
  }
  var ctrlHeight = (86 + 5) + (maxZoom - minZoom + 1) * this.sliderStep + 5;
  
  if (this.isNull(this._handleList) === true) {
    return;
  }

  // Update DOM elements to ajust to current Resolution range.
  this._handleList.container.style.height = (ctrlHeight + this.sliderStep + 2) + "px";
  this._handleList.slideBar.style.height = ((maxZoom - minZoom + 1) * this.sliderStep) + "px";
  this._handleList.zoomOutBtnContainer.style.top = (86 + (maxZoom - minZoom + 1) * this.sliderStep) + "px";
  this._handleList.zoomOutBtn.style.top = (91 + (maxZoom - minZoom + 1) * this.sliderStep) + "px";
  this._handleList.slideBarContainer.style.top = ((maxZoom - this._map.getZoom()) * this.sliderStep + 1) + "px";

};


/**
 * @private
 */
ExtLargeMapControl.prototype._eventTop = function () {
  this._map.panDirection(0, 1);
};


/**
 * @private
 */
ExtLargeMapControl.prototype._eventLeft = function () {
  this._map.panDirection(1, 0);
};

/**
 * @private
 */
ExtLargeMapControl.prototype._eventRight = function () {
  this._map.panDirection(-1, 0);
};

/**
 * @private
 */
ExtLargeMapControl.prototype._eventBottom = function () {
  this._map.panDirection(0, -1);
};

/**
 * @private
 */
ExtLargeMapControl.prototype._eventZoomOut = function () {
  this._map.zoomOut();
};

/**
 * @private
 */
ExtLargeMapControl.prototype._eventZoomIn = function () {
  this._map.zoomIn();
};


/**
 * @private
 */
ExtLargeMapControl.prototype._eventSlideBar = function (e) {
  var map = this._map;
  //calculate zoomlevel
  var mouseY = e.clientY;
  var slideStep = this._step;
  var maxZoom = this._maxZoom;
  var container = this._container;

  //set new zoomLevel
  var ctrlPos = this._getDomPosition(container);
  mouseY -= (ctrlPos.y + 91);
  var zoomLevel = Math.floor(maxZoom - (mouseY / slideStep));
  zoomLevel = zoomLevel < 0 ? 0 : zoomLevel;
  map.setZoom(zoomLevel);  
};

/**
  * @private
 */
ExtLargeMapControl.prototype._getDomPosition = function (that) {
  var targetEle = that;
  var pos = { x : 0, y : 0 };
  
  while (targetEle) {
    pos.x += targetEle.offsetLeft; 
    pos.y += targetEle.offsetTop; 
    targetEle = targetEle.offsetParent;

    if (targetEle && this._is_ie) {
      pos.x += (parseInt(ExtLargeMapControl.getElementStyle(targetEle, 
          "borderLeftWidth", "border-left-width"), 10) || 0);
      pos.y += (parseInt(ExtLargeMapControl.getElementStyle(targetEle, 
          "borderTopWidth", "border-top-width"), 10) || 0);
    }
  }

  if (this._is_gecko) {
    var bd = document.getElementsByTagName("BODY")[0];
    pos.x += 2 * (parseInt(ExtLargeMapControl.getElementStyle(bd, 
        "borderLeftWidth", "border-left-width"), 10) || 0);
    pos.y += 2 * (parseInt(ExtLargeMapControl.getElementStyle(bd, 
        "borderTopWidth", "border-top-width"), 10) || 0);
  }
  return pos;
};


/**
 * @private
 */
ExtLargeMapControl.getElementStyle = function (targetElm, IEStyleProp, CSSStyleProp) {
  var elem = targetElm;
  if (elem.currentStyle) {
    return elem.currentStyle[IEStyleProp];
  } else if (window.getComputedStyle) {
    var compStyle = window.getComputedStyle(elem, "");
    return compStyle.getPropertyValue(CSSStyleProp);
  }
};


/**
 * @private
 */
ExtLargeMapControl.prototype._eventSlideDragEnd = function (e) {
  //calculate zoomlevel
  var maxZoom = this._maxZoom;
  var mouseY = this._slider.top;
  var step = this._step;

  //set new zoomLevel
  var zoomLevel = Math.floor(maxZoom - (mouseY / step));
  zoomLevel = zoomLevel < 0 ? 0 : zoomLevel;
  this._map.setZoom(zoomLevel);
};


/**
 * @private
 */
ExtLargeMapControl.prototype._eventHome = function () {
  this._map.returnToSavedPosition();
};


/**
 * @private
 */
ExtLargeMapControl.prototype._eventZoomEnd = function (oldZoom, newZoom) {
  var maxZoom = this._maxZoom;
  if (newZoom < maxZoom) {
    this._updateZoomSliderRange();
  } else {
    this._updateZoomSliderRange(newZoom);
    maxZoom = newZoom;
  }
  var step = this._step;
  this._slider.moveTo(new GPoint(0, (maxZoom - newZoom) * step));
};

/**
 * @private
 * @ignore
 */
ExtLargeMapControl.prototype.copy = function () {
  return new ExtLargeMapControl(this.latlng_, this.opts);
};


/**
 * @private
 * @ignore
 */
ExtLargeMapControl.prototype.getDefaultPosition = function () {
  return new GControlPosition(G_ANCHOR_TOP_LEFT, new GSize(10, 10));
};


/**
 * @private
 * @ignore
 */
ExtLargeMapControl.prototype.selectable = function () {
  return false;
};

/**
 * @private
 * @ignore
 */
ExtLargeMapControl.prototype.printable = function () {
  return true;
};

/**
 * @private
 * @desc      detect null,null string and undefined
 * @param     value
 * @return    true  :  value is nothing
 *            false :  value is not nothing
 */
ExtLargeMapControl.prototype.isNull = function (value) {
  if (!value && value !== 0 ||
     value === undefined ||
     value === "" ||
     value === null ||
     typeof value === "undefined") {
    return true;
  }
  return false;
};

/**
 * @private
 * @desc      create div element with PNG image
 */
ExtLargeMapControl.prototype.makeImgDiv_ = function (imgSrc, params) {
  var imgDiv = document.createElement("div");
  imgDiv.style.position = "absolute";
  imgDiv.style.overflow = "hidden";
  
  if (params.width) {
    imgDiv.style.width = params.width + "px";
  }
  if (params.height) {
    imgDiv.style.height = params.height + "px";
  }
  
  
  var img = null;
  if (!this._is_ie || this._is_ie8) {
    img = new Image();
    img.src = imgSrc;
  } else {
    img = document.createElement("div");
    if (params.width) {
      img.style.width = params.width + "px";
    }
    if (params.height) {
      img.style.height = params.height + "px";
    }
  }
  img.style.position = "relative";
  img.style.left = params.left + "px";
  img.style.top =  params.top + "px";
  img.style.filter = "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + imgSrc + "')";
  imgDiv.appendChild(img);
  return imgDiv;
};

