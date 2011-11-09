/**
 * MarkerTracker v1.0
 * Copyright (c) 2008 Dan Rummel
 *
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
 *
 *
 * Author: Dan Rummel (www.seero.com)
 *
 *  This ulitily displays directional indicators for "important" markers
 *  on that are out of your maps view.
 */


/**
 *  Creates a MarkerTracker for the given marker and displays it ont he map as needed.
 *
 * @constructor
 * @param {Map} map The map that will display the MarkerTracker. 
 * @param {GMarker} marker the marker to be tracked.
 * @param {Object} opts? Object that contains the options for coustomizing the 
 *                  look and behavior of arrows:
 *   {Number} iconScale Scales the icon size by this value, 0 = no icon.
 *   {Number} padding The padding between the arrow and the edge of the map.
 *   {String} color Color of the arrow.
 *   {Number} weight Thickness of the lines that make up the arrows.
 *   {Number} length length of the arrow.
 *   {Number} opacity opacity of the arrow.
 *   {String} updateEvent The GMap2 event name that triggers the arrows to update.
 *   {String} panEvent The GMarker event name that triggers a quick zoom to the tracked marker.
 *   {Boolean} quickPanEnabled The GMarker event name that triggers a quick zoom to the tracked marker.
 */

function MarkerTracker(marker, map, opts) {
  this.map_ = map;
  this.marker_ = marker;
  this.enabled_ = true;
  this.arrowDisplayed_ = false;
  this.arrow_ = null;
  this.oldArrow_ = null;
  this.control_ = null;
  
  // setup the options
  opts = opts || {};
  this.iconScale_ = MarkerTracker.DEFAULT_ICON_SCALE_;
  if (opts.iconScale != undefined ) {
    this.iconScale_ = opts.iconScale;
  }
  this.padding_ = MarkerTracker.DEFAULT_EDGE_PADDING_;
  if (opts.padding != undefined ) {
    this.padding_ = opts.padding;
  }
  this.color_ = MarkerTracker.DEFAULT_ARROW_COLOR_;
  if (opts.color != undefined ) {
    this.color_ = opts.color;
  }
  this.weight_ = MarkerTracker.DEFAULT_ARROW_WEIGHT_;
  if (opts.weight != undefined ) {
    this.weight_ = opts.weight;
  }
  this.length_ = MarkerTracker.DEFAULT_ARROW_LENGTH_;
  if (opts.length != undefined ) {
    this.length_ = opts.length;
  }
  this.opacity_ = MarkerTracker.DEFAULT_ARROW_OPACITY_;
  if (opts.opacity != undefined ) {
    this.opacity_ = opts.opacity;
  }
  this.updateEvent_ = MarkerTracker.DEFAULT_UPDATE_EVENT_;
  if (opts.updateEvent != undefined ) {
    this.updateEvent_ = opts.updateEvent;
  }
  this.panEvent_ = MarkerTracker.DEFAULT_PAN_EVENT_;
  if (opts.panEvent != undefined ) {
    this.panEvent_ = opts.panEvent;
  }
  this.quickPanEnabled_ = MarkerTracker.DEFAULT_QUICK_PAN_ENABLED_;
  if (opts.quickPanEnabled != undefined ) {
    this.quickPanEnabled_ = opts.quickPanEnabled;
  }
  
  //replicate a different sized icon 
  var babyIcon = new GIcon(marker.getIcon());
  babyIcon.iconSize = new GSize( 
    marker.getIcon().iconSize.width * this.iconScale_,
    marker.getIcon().iconSize.height * this.iconScale_ );
  babyIcon.iconAnchor = new GPoint( 
    marker.getIcon().iconAnchor.x * this.iconScale_,
    marker.getIcon().iconAnchor.y * this.iconScale_/2);
  // kill the shadow
  babyIcon.shadow = null;
  this.babyMarker_ = new GMarker(new GPoint(0, 0), babyIcon);
  
  //bind the update task to the event trigger
  GEvent.bind(this.map_, this.updateEvent_, this, this.updateArrow_ );
  //update the arrow if the marker moves
  GEvent.bind(this.marker_, 'changed', this, this.updateArrow_ );
  if (this.quickPanEnabled_) {
    GEvent.bind(this.babyMarker_, this.panEvent_, this, this.panToMarker_ );
  }
  
  //do an inital check
  this.updateArrow_();
};


//Default Arrow Constants
MarkerTracker.DEFAULT_EDGE_PADDING_ = 25;
MarkerTracker.DEFAULT_ICON_SCALE_ = 0.6;
MarkerTracker.DEFAULT_ARROW_COLOR_ = '#ff0000';
MarkerTracker.DEFAULT_ARROW_WEIGHT_ = 20;
MarkerTracker.DEFAULT_ARROW_LENGTH_ = 20;
MarkerTracker.DEFAULT_ARROW_OPACITY_ = 0.8;
MarkerTracker.DEFAULT_UPDATE_EVENT_ = 'move';
MarkerTracker.DEFAULT_PAN_EVENT_ = 'click';
MarkerTracker.DEFAULT_QUICK_PAN_ENABLED_ = true;

//Default Control Constants

/**
 *  Disables the marker tracker.
 */
MarkerTracker.prototype.disable = function() {
  this.enabled_ = false;
  this.updateArrow_();
};

/**
 *  Enables the marker tracker.
 */
MarkerTracker.prototype.enable = function() {
  this.enabled_ = true;
  this.updateArrow_();
};

/**
 *  Called on on the trigger event to update the arrow. Primary function is to
 *  check if the parent marker is in view, if not draw the tracking arrow.
 */

MarkerTracker.prototype.updateArrow_ = function() {
  if(!this.map_.getBounds().containsLatLng(this.marker_.getLatLng()) && this.enabled_) {
    this.drawArrow_();
  } else if(this.arrowDisplayed_) {
    this.hideArrow_();
  }
};



/**
 *  Draws or redraws the arrow as needed, called when the parent marker is
 *  not with in the map view.
 */

MarkerTracker.prototype.drawArrow_ = function() {

  //convert to pixels
  var bounds = this.map_.getBounds();
  var SE = this.map_.fromLatLngToDivPixel(bounds.getSouthWest());
  var NE = this.map_.fromLatLngToDivPixel(bounds.getNorthEast());
  //include the padding while deciding on the arrow location
  var minX =  SE.x + this.padding_;
  var minY =  NE.y + this.padding_;
  var maxX =  NE.x - this.padding_;
  var maxY =  SE.y - this.padding_;
  
  // find the geometric info for the marker realative to the center of the map
  var center = this.map_.fromLatLngToDivPixel(this.map_.getCenter());
  var loc = this.map_.fromLatLngToDivPixel(this.marker_.getLatLng());
  
  //get the slope of the line
  var m = (center.y-loc.y) / (center.x-loc.x);
  var b = (center.y - m*center.x);
  
  // end the line within the bounds
  if ( loc.x < maxX && loc.x > minX ) {
    var x = loc.x;
  } else if (center.x > loc.x) {
    var x = minX; 
  } else {
    var x = maxX;
  }

  //calculate y and check boundaries again  
  var y = m * x + b;
  if( y > maxY ) {
    y = maxY;
    x = (y - b)/m;
  } else if(y < minY) {
    y = minY;
    x = (y - b) / m;
  }
  
  // get the proper angle of the arrow
  var ang = Math.atan(-m);
  if(x > center.x ) {
    ang = ang + Math.PI; 
  } 
  
  // define the point of the arrow
  var arrowLoc = this.map_.fromDivPixelToLatLng(new GPoint(x, y));
  
  // left side of marker is at -1,1
  var arrowLeft = this.map_.fromDivPixelToLatLng( 
            this.getRotatedPoint_(((-1) * this.length_), this.length_, ang, x, y) );
            
  // right side of marker is at -1,-1
  var arrowRight = this.map_.fromDivPixelToLatLng( 
            this.getRotatedPoint_(((-1)*this.length_), ((-1)*this.length_), ang, x, y));
  
  
  var center = this.map_.getCenter();
  var loc = this.marker_.getLatLng();
  
  this.oldArrow_ = this.arrow_;
  this.arrow_ = new GPolyline([arrowLeft, arrowLoc, arrowRight],
                this.color_, this.weight_, this.opacity_) ;
  this.map_.addOverlay(this.arrow_);
  
  // move the babyMarker to -1,0
  this.babyMarker_.setLatLng(this.map_.fromDivPixelToLatLng( 
            this.getRotatedPoint_(((-2)*this.length_), 0, ang, x, y)));
          
  if (!this.arrowDisplayed_) {
    this.map_.addOverlay(this.babyMarker_);
    this.arrowDisplayed_ = true;
  }
  if (this.oldArrow_) {
    this.map_.removeOverlay(this.oldArrow_);
  }
};



/**
 *  Hides the arrows.
 */
 
MarkerTracker.prototype.hideArrow_ = function() {
  this.map_.removeOverlay(this.babyMarker_);
  if(this.arrow_) {
    this.map_.removeOverlay(this.arrow_);
  }
  if(this.oldArrow_) {
    this.map_.removeOverlay(this.oldArrow_);
  }
  this.arrowDisplayed_ = false;
};


/**
 *  Pans the map to the parent marker.
 */

MarkerTracker.prototype.panToMarker_ = function() {
  this.map_.panTo(this.marker_.getLatLng());
};

/**
 *  This applies a counter-clockwise rotation to any point.
 *  
 * @param {Number} x The x value of the point.
 * @param {Number} y The y value of the point.
 * @param {Number} ang The counter clockwise angle of rotation.
 * @param {Number} xoffset Adds a position offset to the x position.
 * @param {Number} yoffset Adds a position offset to the y position.
 * @return {GPoint} A rotated GPoint.
 */

MarkerTracker.prototype.getRotatedPoint_ = function(x, y, ang, xoffset, yoffset) {
  var newx = y * Math.sin(ang) - x * Math.cos(ang) + xoffset;
  var newy = x * Math.sin(ang) + y * Math.cos(ang) + yoffset;
  var rotatedPoint = new GPoint(newx, newy);
  return(rotatedPoint);
};





