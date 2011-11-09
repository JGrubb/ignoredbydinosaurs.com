The GeoNames contrib project is a collection of modules to integrate with
the webservers and data provided by GeoNames http://www.geonames.org/



=======================================================================
GeoNames Wikipedia Search: Integrate Wikipedia Results into Search
=======================================================================

Creates a new search type "Wikipedia" that includes results from
http://www.geonames.org/export/wikipedia-webservice.html#wikipediaSearch

The results provided by the module also include all values from the XML, including
coordinates and a thumbnail image.

            [entry] => Array
                (
                    [lang] => en
                    [title] => Stock Exchange Tower
                    [summary] => at 125 Old Broad Street. Standing at 103 m (339 feet) tall, with 26 floors, the tower was completed in 1970. It served as the headquarters and offices for the London Stock Exchange until its departure for new premises in Paternoster Square, in July 2004. ''Face to face'' trading was conducted on the trading floor of the exchange, until it was abolished in favour of electronic trading (...)
                    [feature] => 
                    [population] => 0
                    [elevation] => 0
                    [lat] => 51.5144
                    [lng] => -0.0867
                    [wikipediaUrl] => http://en.wikipedia.org/wiki/Stock_Exchange_Tower
                    [thumbnailImg] => http://www.geonames.org/img/wikipedia/11000/thumb-10323-100.jpg
                )
           
These could be themed something like

/**
 * Include a thumbnail in wikipedia search results (provided by geonames/wikipedia_search.module)
 */
function yourmodule_search_item($item, $type) {
  switch ($item['type']) {
    case 'wikipedia':
      // Add coordinates to the extra information
      $entry = $item['entry'];
      $item['extra'] = array('LAT:' . $entry['lat'], 'LONG:' . $entry['lng']);
      // Add a thumbnail image to the results
      if ($entry['thumbnailImg']) {
        $img_html = theme('image', $entry['thumbnailImg'], $item['title'], $item['title'], NULL, FALSE);
        $link_html = l($img_html, $item['link'], array(), NULL, NULL, FALSE, TRUE);
        $item['snippet'] = $link_html . $item['snippet'];
      }
      $html = theme_search_item($item, $type);
      break;
   default:
     $html = theme_search_item($item, $type);
     break;
  }
  return $html;
}

