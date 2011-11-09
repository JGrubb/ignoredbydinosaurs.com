<?php

/*
 * @file
 * Geonames configuration
 */

/**
 * GeoNames Configuration
 *
 * Implement services described at Services described at http://www.geonames.org/export/ws-overview.html
 *
 * @param $a, $b see function
 *
 * @return see bottom of function...
 *
 * TODO: clean up use of t()
 *
 * TODO: provide better defaults:
 * - default service_path to service_name
 * - derive cache result/data prefix from service_name
 */

function geonames_config($a = NULL, $b = NULL) {
  $config['search'] = array(
    'service_full_name'   => 'GeoNames Fulltext Search',
    'description'         => t('Search provides names and information for geographical locations, it is probably the most comprehensive search available from GeoNames and has numereous possible parameter combinations. Note that this service allows some parameters to be arrays: country, featureclass and featurecode.'),
    'service_path'        => 'search',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'seares:',
    'data_cache_prefix'   => 'seadat:',
    'allowed_parameters' => array(
      // our style     // geonames style
      'query'        => 'q',
      'name'         => 'name',
      'name_equals'  => 'name_equals',
      'maxrows'      => 'maxRows',
      'startrow'     => 'startRow',
      'country'      => 'country',
      'admincode1'   => 'adminCode1',
      'admincode2'   => 'adminCode2',
      'admincode3'   => 'adminCode3',
      'featureclass' => 'featureClass',
      'featurecode'  => 'featureCode',
      'lang'         => 'lang',
      'style'        => 'style',
    ),
    'array_parameters'    => array( // Parameters that are allowed to be arrays (or just string)
      'country',
      'featurecode',
      'featureclass',
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      'query',
      'name',
      'name_equals'
    ),
    'required_parameters_type' => 'single',
    'query_defaults' => array(
      'type'         => 'xml',
      'maxrows'      => 15,
      'featureclass' => 'p',
      'startrow'     => 0
    ),
    'pager'          => TRUE,
    'detect_style'   => TRUE,
  );
  $config['postalcodecountryinfo'] = array(
    'service_full_name'   => 'Postal Code Country Info',
    'description'         => 'Find countries for which postal code geocoding is available.',
    'service_path'        => 'postalCodeCountryInfo',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'pccires:',
    'data_cache_prefix'   => 'pccidat:',
    'allowed_parameters'  => array(
      'country',
    ),
    'columns' => array(
      'countrycode',
      'countryname',
      'numpostalcodes',
      'minpostalcode',
      'maxpostalcode',
    ),
  );
  $config['children'] = array(
    'service_full_name'   => 'Place Hierarchy: Children',
    'description'         => t('Find the children of a given geonameId.'),
    'service_path'        => 'children',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'hcres:',
    'data_cache_prefix'   => 'hcdat:',
    'allowed_parameters'  => array(
      // our style        // geonames style
      'geonameid' => 'geonameId',
      'style'     => 'style',
      'lang'      => 'lang',
    ),
    'required_parameters' => array(
      'geonameid',
    ),
    'required_parameters_type' => 'single',
    'detect_style'        => TRUE,
  );
  $config['countrycode'] = array(
    'service_full_name'   => 'Country Code (reverse geocoding)',
    'description'         => t('Find Country Code from latitude and longitude'),
    'service_path'        => 'countrycode',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'ccres:',
    'data_cache_prefix'   => 'ccdat:',
    'allowed_parameters'  => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
      'lang'     => 'lang',
      'type'     => 'type',
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults'      => array(
      'type' => 'xml'
    ),
    'columns' => array('countrycode', 'countryname'),
  );
  $config['countryinfo'] = array(
    'service_full_name'   => 'Country Info (Bounding Box, Capital, Area in square km, Population)',
    'description'         => 'Find countries for which postal code geocoding is available.',
    'service_path'        => 'countryInfo',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'couires:',
    'data_cache_prefix'   => 'couidat:',
    'allowed_parameters' => array(
      //our style  geonames style
      'country' => 'country',
      'lang' => 'lang',
    ),
    'columns' => array(
      'countrycode',
      'countryname',
      'isonumeric',
      'isoalpha3',
      'fipscode',
      'continent',
      'capital',
      'areainsqkm',
      'population',
      'currencycode',
      'languages',
      'geonameid',
      'bboxwest',
      'bboxnorth',
      'bboxsouth',
      'bboxeast',
    ),
  );
  $config['countrysubdivision'] = array(
    'service_full_name'   => 'Country Subdivision (reverse geocoding)',
    'description'         => t('Find Country Code from latitude and longitude'),
    'service_path'        => 'countrySubdivision',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'csres:',
    'data_cache_prefix'   => 'csdat:',
    'allowed_parameters'  => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
      'lang'     => 'lang',
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'columns' => array('countrycode', 'countryname', 'admincode1', 'adminname1'),
  );
  $config['get'] = array(
    'service_full_name'   => 'Get',
    'parser_function'     => 'geonames_parse_get_xml',
    'description'         => 'Retrieve GeoNames record by geonameid',
    'service_path'        => 'get',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'gres:',
    'data_cache_prefix'   => 'gdat:',
    'allowed_parameters'  => array(
      'geonameid' => 'geonameId',
      'lang'      => 'lang',
    ),
    'columns' => array(
      'geoname',
    ),
  );
  $config['gtopo30'] = array(
    'service_full_name'        => 'Elevation - GTOPO30',
    'parser_function'          => 'geonames_parse_elevation_xml',
    'description'              => t('GTOPO30 is a global digital elevation model (DEM) with a horizontal grid spacing of 30 arc seconds (approximately 1 kilometer). GTOPO30 was derived from several raster and vector sources of topographic information.'),
    'service_path'             => 'gtopo30',
    'credit_cost'              => 0,
    'result_cache_prefix'      => 'gtpres:',
    'data_cache_prefix'        => 'gtpdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
      'type'     => 'type',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults'           => array(
      'type' => 'xml'
    ),
  );
  $config['hierarchy'] = array(
    'service_full_name'   => 'Place Hierarchy: Hierarchy',
    'description'         => t('Find all Geonames higher up in the hierarchy of a place name.'),
    'service_path'        => 'hierarchy',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'hires:',
    'data_cache_prefix'   => 'hidat:',
    'allowed_parameters'  => array(
      // our style     // geonames style
      'geonameid'         => 'geonameId',
      'style'             => 'style',
      'lang'              => 'lang',
    ),
    'required_parameters' => array(
      'geonameid',
    ),
    'required_parameters_type' => 'single',
    'detect_style'        => TRUE,
  );
  $config['findnearby'] = array(
    'service_full_name'   => 'Find Nearby (reverse geocoding)',
    'description'         => t('Find Nearby (reverse geocoding)'),
    'service_path'        => 'findNearby',
    'credit_cost'         => 4,
    'result_cache_prefix' => 'nebres:',
    'data_cache_prefix'   => 'nebdat:',
    'allowed_parameters'  => array(
      //our style  geonames style
      'lat'               => 'lat',
      'lng'               => 'lng',
      'radius'            => 'radius',
      'featureclass'      => 'featureClass',
      'featurecode'       => 'featureCode',
      'maxrows'           => 'maxRows',
      'style'             => 'style',
    ),
    'array_parameters' => array(
      'featurecode',
      'featureclass',
    ),
    'required_parameters' => array(
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
  );
  $config['findnearbyplacename'] = array(
    'service_full_name'        => 'Find nearby Place Name (reverse geocoding)',
    'description'              => t('Find nearby place name (reverse geocoding)'),
    'service_path'             => 'findNearbyPlaceName',
    'credit_cost'              => 4,
    'result_cache_prefix'      => 'neares:',
    'data_cache_prefix'        => 'neadat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
      'radius'   => 'radius',
      'maxrows'  => 'maxRows',
      'style'    => 'style',     // Default = MEDIUM
    ),
    'query_defaults' => array(
      'radius' => 20
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
  );
  $config['findnearbypostalcodes'] = array(
    'service_full_name'   => 'Find nearby Postal Codes (reverse geocoding)',
    'description'         => 'Find nearby Postal Codes (reverse geocoding)',
    'service_path'        => 'findNearbyPostalCodes',
    'credit_cost'         => 2,
    'result_cache_prefix' => 'nbpres:',
    'data_cache_prefix'   => 'nbpdat:',
    'allowed_parameters' => array(
      //our style  geonames style
      'lat'        => 'lat',
      'lng'        => 'lng',
      'radius'     => 'radius',
      'postalcode' => 'postalcode',
      'maxrows'    => 'maxRows',      // Geonames default = 5
      'country'    => 'country',      // Geonames default = ALL
      'style'      => 'style',
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      array('lat', 'lng'),
      array('postalcode', 'country')
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'maxrows' => 15,
      'radius'  => 10,
      'style'   => 'medium',
    ),
    'columns' => array(
      'postalcode',
      'name',
      'countrycode',
      'lat',
      'lng',
      'distance',
      'admincode1',
      'admincode2',
      'admincode3',
      'adminname1',
      'adminname2',
      'adminname3',
    ),
  );
  $config['findnearbywikipedia'] = array(
    'service_full_name'   => 'Find nearby Wikipedia Entries (reverse geocoding)',
    'description' => 'Find nearby Wikipedia Entries (reverse geocoding)',
    'service_path'        => 'findNearbyWikipedia',
    'credit_cost'         => 2,
    'result_cache_prefix' => 'wikres:',
    'data_cache_prefix'   => 'wikdat:',
    'allowed_parameters'  => array(
      //our style  geonames style
      'lat'        => 'lat',
      'lng'        => 'lng',
      'radius'     => 'radius',
      'postalcode' => 'postalcode',
      'lang'       => 'lang',         // Geonames default = en
      'maxrows'    => 'maxRows',      // Geonames default = 5
      'country'    => 'country',      // Geonames default = ALL
    ),
    'pager' => FALSE,
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      array('lat', 'lng'),
      array('postalcode', 'country')
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'maxrows' => 15,
      'radius'  => 10
    ),
    'columns' => array(
      'title'        => 'title',
      'summary'      => 'summary',
      'feature'      => 'feature',
      'population'   => 'population',
      'elevation'    => 'elevation',
      'lat'          => 'lat',
      'lng'          => 'lng',
      'wikipediaurl' => 'wikipediaurl',
      'thumbnailimg' => 'thumbnailImg',
      'lang'         => 'lang',
      'distance'     => 'distance',
    ),
  );
  $config['neighbours'] = array(
    'service_full_name'   => 'Place Hierarchy: Neighbours',
    'description'     => t('Returns the neighbours for a given geonameId.'),
    'service_path'        => 'neighbours',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'hnres:',
    'data_cache_prefix'   => 'hndat:',
    'allowed_parameters'  => array(
      // our style     // geonames style
      'geonameid'       => 'geonameId',
      'style'           => 'style',
      'lang'            => 'lang',
    ),
    'required_parameters' => array(
      'geonameid',
    ),
    'required_parameters_type' => 'single',
    'detect_style'   => TRUE,
  );
  $config['postalcodesearch'] = array(
    'service_full_name'   => 'Postal Code Search',
    'description'         => t('Postalcode search provides placenames/postalcodes for locations.'),
    'service_path'        => 'postalCodeSearch',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'pcsres:',
    'data_cache_prefix'   => 'pcsdat:',
    'allowed_parameters' => array(
      // our style     // geonames style
      'postalcode'  => 'postalcode',
      'placename'   => 'placename',
      'maxrows'     => 'maxRows',
      'style'       => 'style',
      'country'     => 'country',   // if not set -- all countries
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      'postalcode',
      'placename',
    ),
    'required_parameters_type' => 'single',
    'query_defaults' => array(
      'maxrows'      => 10,
      'style'        => 'medium',
    ),
    'columns'        => array(
      'postalcode',
      'name',
      'countrycode',
      'lat',
      'lng',
      'admincode1',
      'adminname1',
      'admincode2',
      'adminname2',
      'admincode3',
      'adminname3'
    ),
  );
  $config['siblings'] = array(
    'service_full_name'   => 'Place Hierarchy: Siblings',
    'description'         => t('Returns the siblings for a given geonameId.'),
    'service_path'        => 'siblings',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'hsres:',
    'data_cache_prefix'   => 'hsdat:',
    'allowed_parameters'  => array(
      // our style     // geonames style
      'geonameid'         => 'geonameId',
      'style'             => 'style',
      'lang'              => 'lang',
    ),
    'required_parameters' => array(
      'geonameid',
    ),
    'required_parameters_type' => 'single',
    'detect_style'        => TRUE,
  );
  $config['srtm3'] = array(
    'service_full_name'        => 'Elevation - SRTM3',
    'parser_function'          => 'geonames_parse_elevation_xml',
    'description'              => t('Shuttle Radar Topography Mission (SRTM) elevation data. The dataset covers land areas between 60 degrees north and 56 degrees south. This web service is using SRTM3 data with data points located every 3-arc-second (approximately 90 meters) on a latitude/longitude grid. Result: a single number giving the elevation in meters according to srtm3, ocean areas have been masked as "no data" and have been assigned a value of -32768'),
    'service_path'             => 'srtm3XML',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'strmres:',
    'data_cache_prefix'        => 'strmdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
      'type'     => 'type',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
  );
  $config['timezone'] = array(
    'service_full_name'        => 'Timezone',
    'description'              => t('Find the timezone for a given lat/lng with gmt offset (1. January) and dst offset (1. July)'),
    'service_path'             => 'timezone',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'tzres:',
    'data_cache_prefix'        => 'tzdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'countrycode',
      'countryname',
      'lat',
      'lng',
      'timezoneid',
      'dstoffset',
      'gmtoffset',
      'rawoffset',
      'time',
    ),
  );
  $config['wikipediaboundingbox'] = array(
    'service_full_name'   => 'Wikipedia Articles from Bounding Box',
    'description'         => 'Search Wikipedia entries within the Bounding Box',
    'service_path'        => 'wikipediaBoundingBox',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'wikbres:',
    'data_cache_prefix'   => 'wikbdat:',
    'allowed_parameters' => array(
      //our style  geonames style
      'north'      => 'north',
      'south'      => 'south',
      'east'       => 'east',
      'west'       => 'west',
      'lang'       => 'lang',
      'maxrows'    => 'maxRows',
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      'north',
      'south',
      'east',
      'west',
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'maxrows' => 15,
    ),
    'columns' => array(
      'lang',
      'title',
      'summary',
      'feature',
      'population',
      'elevation',
      'lat',
      'lng',
      'wikipediaurl',
      'thumbnailimg',
    ),
  );
  $config['wikipediasearch'] = array(
    'service_full_name'   => 'Wikipedia Fulltext Search',
    'description'         => 'Search Wikipedia entries in Fulltext',
    'service_path'        => 'wikipediaSearch',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'wiksres:',
    'data_cache_prefix'   => 'wiksdat:',
    'allowed_parameters' => array(
      //our style  geonames style
      'query'      => 'q',
      'title'      => 'title',
      'lang'       => 'lang',         // Geonames default = en
      'maxrows'    => 'maxRows',      // Geonames default = 5
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      array('query'),
      // array('title'),
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'maxrows' => 15,
    ),
    'columns' => array(
      'lang',
      'title',
      'summary',
      'feature',
      'population',
      'elevation',
      'lat',
      'lng',
      'wikipediaurl',
      'thumbnailimg',
    ),
  );
  $config['cities'] = array(
    'service_full_name'   => 'Cities and Placenames',
    'description'         => 'returns a list of cities and placenames in the bounding box, ordered by relevancy (capital/population)',
    'service_path'        => 'cities',
    'credit_cost'         => 1,
    'result_cache_prefix' => 'citiesres:',
    'data_cache_prefix'   => 'citiesdat:',
    'allowed_parameters' => array(
      //our style  geonames style
      'north'      => 'north',
      'south'      => 'south',
      'east'       => 'east',
      'west'       => 'west',
      'lang'       => 'lang',
      'maxrows'    => 'maxRows',
    ),
    'required_parameters' => array( // If you have set query_defaults you should not include the parameter here
      'north',
      'south',
      'east',
      'west',
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'maxrows' => 15,
    ),
    'columns' => array(
      'name',
      'lat',
      'lng',
      'geonameid',
      'countrycode',
      'countryname',
      'fcl',
      'fcode',
    ),
  );
  $config['findNearbyStreets'] = array(
    'service_full_name'        => 'Find nearby Streets',
    'description'              => t('Finds the nearest street for a given lat/lng pair.'),
    'service_path'             => 'findNearbyStreets',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'nearbystreetsres:',
    'data_cache_prefix'        => 'nearbystreetsdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'line',
      'distance',
      'mtfcc',
      'name',
      'fraddl',
      'fraddr',
      'toaddl',
      'toaddr',
      'postalcode',
      'placename',
      'admincode2',
      'adminname2',
      'admincode1',
      'adminname1',
      'countrycode',
    ),
  );
  // The Open Street Map version - reverse geocoding
  $config['findNearbyStreetsOSM'] = array(
    'service_full_name'        => 'Find nearby Streets (Open Street Map)',
    'description'              => t('Finds the nearest street for a given lat/lng pair - OSM.'),
    'service_path'             => 'findNearbyStreetsOSM',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'nearbystreetsosmres:',
    'data_cache_prefix'        => 'nearbystreetsosmdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'name',
      'line',
      'distance',
      'highway',
      'oneway',
      'ref',
    ),
  );
  $config['findNearestIntersection'] = array(
    'service_full_name'        => 'Find nearest Intersection',
    'description'              => t('Finds the nearest street and the next crossing street for a given lat/lng pair.'),
    'service_path'             => 'findNearestIntersection',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'nearbyintres:',
    'data_cache_prefix'        => 'nearbyintdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'street1',
      'street2',
      'lat',
      'lng',
      'distance',
      'postalcode',
      'placename',
      // 'admincode2', // ?
      'adminname2',
      'admincode1',
      'adminname1',
      'countrycode',
    ),
  );
  $config['findNearestIntersectionOSM'] = array(
    'service_full_name'        => 'Find nearest Intersection (Open Street Map)',
    'description'              => t('Finds the nearest street and the next crossing street for a given lat/lng pair.'),
    'service_path'             => 'findNearestIntersectionOSM',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'nearbyintosmres:',
    'data_cache_prefix'        => 'nearbyintosmdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'name',
      'street1',
      'highway1',
      'street2',
      'highway2',
      'lat',
      'lng',
      'distance',
    ),
  );
  $config['findNearestAddress'] = array(
    'service_full_name'        => 'Find nearest Address',
    'description'              => t('Find nearest Address'),
    'service_path'             => 'findNearestAddress',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'nearbyaddrres:',
    'data_cache_prefix'        => 'nearbyaddrdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'street',
      'streetnumber',
      'lat',
      'lng',
      'distance',
      'postalcode',
      'placename',
      'admincode2',
      'adminname2',
      'admincode1',
      'adminname1',
      'countrycode',
    ),
  );
  $config['neighbourhood'] = array(
    'service_full_name'        => 'Neighbourhood / reverse geocoding',
    'description'              => t('The neighbourhood for US cities. Data provided by Zillow under cc-by-sa license.'),
    'service_path'             => 'neighbourhood',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'neighbourhoodres:',
    'data_cache_prefix'        => 'neighbourhooddat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'countrycode',
      'countryname',
      'admincode1',
      'adminname1',
      'admincode2',
      'adminname2',
      'city',
      'name',
    ),
  );
  $config['findNearByWeather'] = array(
    'service_full_name'        => 'Weather Station with most recent weather observation / reverse geocoding',
    'description'              => t('returns a weather station with the most recent weather observation'),
    'service_path'             => 'findNearByWeatherXML',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'nearbyweatherres:',
    'data_cache_prefix'        => 'nearbyweatherdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'observation',
      'stationname',
      'icao',
      'countrycode',
      'elevation',
      'lat',
      'lng',
      'temperature',
      'dewpoint',
      'humidity',
      'clouds',
      'weathercondition',
      'hectopascaltimeter',
      'winddirection',
      'windspeed',
    ),
  );
  $config['ocean'] = array(
    'service_full_name'        => 'Ocean / reverse geocoding',
    'description'              => t('The name of the ocean or sea.'),
    'service_path'             => 'ocean',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'oceanres:',
    'data_cache_prefix'        => 'oceandat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'name',
    ),
  );
  $config['extendedFindNearby'] = array(
    'service_full_name'        => 'Extended Find nearby toponym / reverse geocoding',
    'description'              => t('returns the most detailed information available for the lat/lng query.'),
    'service_path'             => 'extendedFindNearby',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'extendedFindNearbyres:',
    'data_cache_prefix'        => 'extendedFindNearbydat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    // columns depend on where the point is:
    'columns' => array(
      // US
      'street',
      'streetnumber',
      'lat',
      'lng',
      'distance',
      'postalcode',
      'placename',
      'admincode2',
      'adminname2',
      'admincode1',
      'adminname1',
      'countrycode',
      // Other
      'name',
      'lat',
      'lng',
      'geonameid',
      'countrycode',
      'countryname',
      'fcl',
      'fcode',
      // Ocean
      // 'name'
    ),
  );
  $config['astergdem'] = array(
    'service_full_name'        => 'Elevation - Aster Global Digital Elevation Model',
    'parser_function'          => 'geonames_parse_elevation_xml',
    'description'              => t('a single number giving the elevation in meters according to aster gdem.'),
    'service_path'             => 'astergdemXML',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'astergdemres:',
    'data_cache_prefix'        => 'astergdemdat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'lat'      => 'lat',
      'lng'      => 'lng',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'lat',
      'lng'
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml'
    ),
    'columns' => array(
      'lat',
      'lng',
      'astergdem',
    ),
  );
  // eg: http://ws.geonames.org/weatherIcaoXML?formatted=true&ICAO=LSZH&style=full
  $config['weatherIcao'] = array(
    'service_full_name'        => 'Weather Observation',
    // 'parser_function'          => 'geonames_parse_elevation_xml',
    'description'              => t('A single weather observation for the given site.'),
    'service_path'             => 'weatherIcaoXML',
    'credit_cost'              => 1,
    'result_cache_prefix'      => 'weathericaores:',
    'data_cache_prefix'        => 'weathericaodat:',
    'allowed_parameters'       => array(
      //our style  geonames style
      'code'    => 'ICAO', // icaoCode
      'formatted'  => 'formatted',
      'style' => 'style',
    ),
    'required_parameters'      => array( // If you have set query_defaults you should not include the parameter here
      'code',
    ),
    'required_parameters_type' => 'all',
    'query_defaults' => array(
      'type' => 'xml',
      'formatted' => 'true',
      'style' => 'full',
    ),
    'columns' => array(
      'observation',
      'stationname',
      'icao',
      'countrycode',
      'elevation',
      'lat',
      'lng',
      'temperature',
      'dewpoint',
      'humidity',
      'clouds',
      'weathercondition',
      'hectopascaltimeter',
      'winddirection',
      'windspeed',
    ),
  );

  // Prepend all cache prefixes with 'geonames_'
  foreach ($config as $service_name => &$c) {
    $c['service_name'] = $service_name;
    $c['result_cache_prefix'] = 'geonames_'. $c['result_cache_prefix'];
    $c['data_cache_prefix'] = 'geonames_'. $c['data_cache_prefix'];
  }

  $services = array_keys($config);

  switch ($a) {
    case 'services':
      // Return configured services or return full configuration
      return $services;

    case 'all values':
      $key = $b;
      foreach ($config as $c) {
        $values[] = $c[$key];
      }
      return $values;
  } // end switch

  // get a single variable from a service
  $service = $a;
  if (in_array($service, $services)) {
    $service = $a;
    $variable = $b;
    return isset($config[$service][$variable]) ? $config[$service][$variable] : FALSE;
  }

  // if nothing else works, just return the full config ;)
  return $config;
}