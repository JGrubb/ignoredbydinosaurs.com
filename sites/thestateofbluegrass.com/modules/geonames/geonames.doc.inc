<?php

/**
 * @file
 * The online help for GeoNames. TODO: convert to advanced help etc?
 */

/**
 * Documentation pages is a form, stupid but it works for now :))
 */
function geonames_docs() {
  $page = '<div>';
  $page .= '<div class="messages">'. t('Syntax: $result = geonames_query($service, $query, $options)') .'</div>';
  $page .= '</div>';

  $element = array(
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Parameter: $service (string)'),
    '#value' => '<div>The "Usage Instructions for Installed Services" below, lists all the currently installed services available on your system. Please look there for details on the various services.</div>',
  );
  $page .= theme('fieldset', $element);

  $element = array(
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Parameter: $query (associative array)'),
    '#value' => '<div>Queries (query parameters) are defined as associative arrays, as the one above. For a list of allowed query parameters, please see the specific service under "Usage Instructions for Installed Services".'.'<div class="messages">'. t('Example: $query = array(\'lat\' => 59.95, \'lng\' => 10.77)') .'</div>'.'This query can be used with all services that require the parameters lat and lng, for example Find Nearby Place Name.<br /><br />
        The query parameters are usually integers (numbers) or strings (text), but for a few services they are allowed to be arrays (multiple values) in order to be able to pass several values of the same type (for instance countries or featurecodes), in the example below we pass on two countries.'.'<div class="messages">'. t('Example: $query = array(\'name\' => \'Holmen\', \'country\' => array(\'NO\',\'SE\'))') .'</div>If you use this query with the \'search\' service (GeoNames Fulltext Search), you will find places with the name <i>Holmen</i> in <i>Norway</i> or <i>Sweden</i></div>',
    );
  $page .= theme('fieldset', $element);

  $element = array(
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Parameter: $options (associative array) - optional'),
    '#value' => t('<div>You may use the $option parameter to modify the results. Limit the number of fields returned, order the columns, or sort the rows. The options array has three optional properties: columns, sortby and sortorder (all of them may be used independently).<br /><br /><b>columns</b> is an array of fieldnames ') .'<div class="messages">'. t('Example: $options '."= array('columns' => array('countryname', 'countrycode') )") .'</div>'. t('By passing this option your results will only contain the two columns specified (countryname and countrycode). The result object will also be arranged with your results in the specified order.') . t('<br /><br /><b>sortby</b> is a fieldname (string) ') .'<div class="messages">'. t('Example: $options '."= array('sortby' => 'countryname')") .'</div>'. t('By passing this option your results will be ordered by the countryname.') . t('<br /><br /><b>sortorder</b> is either ASC or DESC (string) ') .'<div class="messages">'. t('Example: $options '."= array('sortorder' => 'DESC')") .'</div>'. t('By passing this option your results will be returned in a descending order (Your list of Countries would start with the letter Z).') . t('<br /><br /><div class="messages">') . t('Multiple Options Example: $options '."= array('sortby' => 'countryname', 'sortorder' => 'DESC')") .'</div>'. t('This option would return your results ordered by Country Name, descending.</div>'),
  );
  $page .= theme('fieldset', $element);

  $element = array(
      '#collapsible' => TRUE,
        '#collapsed' => TRUE,
    '#title' =>  t('Result: $result (object)'),
        '#value' => '<div>The results object will look something like this: <br />
<pre>
$result->results = array(
  [0] = array(\'name\' => \'Oslo\', ...)
  [1] = array(\'name\' => \'Nydalen\', ...)
  ...
)
$result->total_results_count = integer, number of total results
$result->service = string service name
$result->request = array (
  \'url\' = URL sent to GeoNames server
  \'bytes\' = size of data requested from GeoNames server
  \'seconds\' = time used for request to GeoNames server
  \'cached\' = if the result is loaded from the cache, this variable is set to the cache type (result or data)
)
$result->query = array(
  \'lat\' = 23
  \'lng\' = 34
  \'maxrows\' = 10
  ...
)
$result->pager = array() - associative array, only available for some services
$result->standalone = if set to <i>no</i>, the GeoNames commercial service is used
</pre></code>
</div>',
    );
  $page .= theme('fieldset', $element);

  $page .= '<br /><div>'. t('In the usage instructions below, <b>bold</b> parameters are') .' <b>'. t('mandatory') .'</b>'. t('. Parameters that are structured like \'key=value\' are default values for parameters, and may be overridden. Items in') .' <i>'. t('italics') .'</i> '. t('are') .' <i>'. t('optional parameters') .'</i>. '. t('Services with multiple bold parameters may be one out of two types; single or all - read the instructions carefully.') .'<br /><br /><h2>'. t('Usage Instructions for Installed Services') .'</h2></div>';

  $services = geonames_config(); // geonames_config('services', 'keys');
    // This function used to return the full configuration........ look into how to fix it (fix in other version as well)!

  foreach ($services as $service => $sc) { // service configuration
    $usage = sprintf("<div class=messages>Usage: \$result = geonames_query('%s', \$query, <i>\$options</i>)</div>", $service);

    // Compile the $query part... this is tricky!
    // First, check if the required parameters is an array of arrays (how many times do we have to loop?)
    // count variations of required parameters combinations
    $variations = (is_array($sc['required_parameters'][0])) ? count($sc['required_parameters']) : 1;

    $usage_query = '';
    $loops = 0;
    while ($loops < $variations) {

      // Get $rp
      $rp = ($variations > 1) ? $sc['required_parameters'][$loops] : $sc['required_parameters'];

      $usage_query .= ($usage_query) ? '<b>or</b><div style="height: 8px"></div>' : '';
      $usage_query .= '$query = array(';

      $numreqpar = 0; // Count number of request parameters -- used for displaying various strings later
      if ($rp) {
        foreach ($rp as $pmtr) {
          $numreqpar++;
          $usage_query .= '<b>'. $pmtr .'</b>, ';
        }
      }

      if ($sc['query_defaults']) {
        foreach ($sc['query_defaults'] as $key => $val) {
          if ($key != 'type') {
            $usage_query .= $key .'='. $val .', ';
          }
        }
      }

      if (is_array($sc['allowed_parameters'])) {
        foreach ($sc['allowed_parameters'] as $key => $val) {
          if ((!is_array($rp) || (is_array($rp) && !in_array($key, $rp))) && (!is_array($sc['query_defaults'] || (is_array($sc['query_defaults']) && !in_array($key, array_keys($sc['query_defaults'])))))) {
            if ($key != 'type') {
              $usage_query .= '<i>'. $key .'</i>, ';
            }
          }
        }
      }

      $query_comments = '';
      if (is_array($sc['array_parameters'])) {
        // Some of the parameters are allowed to be arrays...
        foreach ($sc['array_parameters'] as $arrpar) {
          $query_comments .= sprintf('<li> %s is allowed to be an array of strings</li>', $arrpar);
        }
      }

      // strip the last ', ' from the querystring
      if (strstr($usage_query, ',')) {
        $usage_query = drupal_substr($usage_query, 0, drupal_strlen($usage_query) - 2);
      }
      if ($sc['required_parameters_type'] == 'single') {
        if ($numreqpar > 1) {
          $query_comments .= '<li> just one of the bold parameters is required </li>';
        }
      }
      if ($sc['required_parameters_type'] == 'all') {
        if ($numreqpar == 2) {
          $query_comments .= '<li> both bold parameters are required </li>';
        }
        elseif ($numreqpar > 2) {
          $query_comments .= '<li> all bold parameters are required </li>';
        }
      }

      // If no comments, add some space with br
      $query_comments = ($query_comments) ? sprintf("<ul>%s</ul>", $query_comments) : '<div style="height: 6px"></div>';

      $usage_query .= ($usage_query) ? ')'. $query_comments : '';

      $loops++;

    } // End while loop

    $resultrows = '<div>$result->results = array(';

    if ($sc['columns']) {
      // Columns are defined -- use those
      foreach ($sc['columns'] as $a) {
        $resultrows .= $a .', ';
      }
      $resultrows = drupal_substr($resultrows, 0, drupal_strlen($resultrows)-2) .')';
    }
    else {
      // Probably short/medium/long/full syntax... TODO this does not work... DEBUG
      $gf = geonames_fields('full', TRUE);
      foreach ($gf as $f) {
        $resultrows .= $f .', ';
      }
      $resultrows = drupal_substr($resultrows, 0, drupal_strlen($resultrows)-2) .')';
      $resultrows .= ' -- FULL style';
    }
    $resultrows .= '</div>';

    $element = array(
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t($sc['service_full_name']),
      '#value' => '<div>Purpose: '. $sc['description'] . $usage . $usage_query . $resultrows .'</div>',
    );
    $page .= theme('fieldset', $element);

  }

  return $page;
}
