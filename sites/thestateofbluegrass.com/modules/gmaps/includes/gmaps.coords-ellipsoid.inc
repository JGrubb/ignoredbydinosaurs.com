<?php
// $Id: gmaps.coords-ellipsoid.inc,v 1.1.2.6 2009/10/24 16:20:10 xmarket Exp $

/**
 * @file
 * Coordinate processing functions for Ellipsoid(Geoid) bodies.
 */


function _gmaps_coords_line_end_ellipsoid($coords, $bearing, $length, $a, $b) {
  $lat1 = $coords->latitude;
  $lng1 = $coords->longitude;
  $f = ($a - $b) / $a; //1/298.257223563;
  $sinAlpha1 = sin($bearing);
  $cosAlpha1 = cos($bearing);

  $tanU1 = (1-$f) * tan(deg2rad($lat1));
  $cosU1 = 1 / sqrt((1 + $tanU1*$tanU1));
  $sinU1 = $tanU1*$cosU1;
  $sigma1 = atan2($tanU1, $cosAlpha1);
  $sinAlpha = $cosU1 * $sinAlpha1;
  $cosSqAlpha = 1 - $sinAlpha*$sinAlpha;
  $uSq = $cosSqAlpha * ($a*$a - $b*$b) / ($b*$b);
  $A = 1 + $uSq/16384*(4096+$uSq*(-768+$uSq*(320-175*$uSq)));
  $B = $uSq/1024 * (256+$uSq*(-128+$uSq*(74-47*$uSq)));

  $sigma = $length / ($b*$A);
  $sigmaP = 2*M_PI;
  while (abs($sigma-$sigmaP) > pow(10, -12)) {
    $cos2SigmaM = cos(2*$sigma1 + $sigma);
    $sinSigma = sin($sigma);
    $cosSigma = cos($sigma);
    $deltaSigma = $B*$sinSigma*($cos2SigmaM+$B/4*($cosSigma*(-1+2*$cos2SigmaM*$cos2SigmaM)-
      $B/6*$cos2SigmaM*(-3+4*$sinSigma*$sinSigma)*(-3+4*$cos2SigmaM*$cos2SigmaM)));
    $sigmaP = $sigma;
    $sigma = $length / ($b*$A) + $deltaSigma;
  }

  $tmp = $sinU1*$sinSigma - $cosU1*$cosSigma*$cosAlpha1;
  $lat2 = atan2($sinU1*$cosSigma + $cosU1*$sinSigma*$cosAlpha1,
      (1-$f)*sqrt($sinAlpha*$sinAlpha + $tmp*$tmp));
  $lambda = atan2($sinSigma*$sinAlpha1, $cosU1*$cosSigma - $sinU1*$sinSigma*$cosAlpha1);
  $C = $f/16*$cosSqAlpha*(4+$f*(4-3*$cosSqAlpha));
  $L = $lambda - (1-$C) * $f * $sinAlpha *
      ($sigma + $C*$sinSigma*($cos2SigmaM+$C*$cosSigma*(-1+2*$cos2SigmaM*$cos2SigmaM)));

  //$revAz = atan2($sinAlpha, -$tmp);  // final bearing

  return array(rad2deg($lat2), $lng1+rad2deg($L));
}

function _gmaps_coords_latlng_to_cart_ellipsoid($coords, $a, $b) {
  $e = sqrt((pow($a, 2) - pow($b, 2))) / $a;

  $lat = deg2rad($coords->latitude);
  $lon = deg2rad($coords->longitude);
  $h = isset($coords->elevation) ? $coords->elevation : 0;
  $N = $a / pow(1 - (pow($e, 2) * pow(sin($lat), 2)), 0.5);

  $x = ($N + $h) * cos($lat) * cos($lon);

  $y = ($N + $h) * cos($lat) * sin($lon);

  $z = (($N * (pow($b, 2)) / pow($a, 2)) + $h) *sin($lat);

  return array($x, $y, $z);
}

function _gmaps_coords_distance_ellipsoid($coords1, $coords2, $a, $b) {
  //lat1, lon1, lat2, lon2
  $lat1 = $coords1->latitude;
  $lon1 = $coords1->longitude;
  
  $lat2 = $coords2->latitude;
  $lon2 = $coords2->longitude;
  
  $f = ($a - $b) / $a; //1/298.257223563;
  $L = deg2rad($lon2-$lon1);
  $U1 = atan((1-$f) * tan(deg2rad($lat1)));
  $U2 = atan((1-$f) * tan(deg2rad($lat2)));
  
  $sinU1 = sin($U1); $cosU1 = cos($U1);
  $sinU2 = sin($U2); $cosU2 = cos($U2);
  
  $lambda = $L; $lambdaP; $limit = 100;
  do {
    $sinLambda = sin($lambda); $cosLambda = cos($lambda);
    $sinSigma = sqrt(($cosU2*$sinLambda) * ($cosU2*$sinLambda) + 
      ($cosU1*$sinU2-($sinU1*$cosU2*$cosLambda)) * ($cosU1*$sinU2-($sinU1*$cosU2*$cosLambda)));
    // co-incident points
    if ($sinSigma==0) return 0;
    $cosSigma = $sinU1*$sinU2 + ($cosU1*$cosU2*$cosLambda);
    $sigma = atan2($sinSigma, $cosSigma);
    $sinAlpha = ($cosU1*$cosU2*$sinLambda) / $sinSigma;
    $cosSqAlpha = 1 - ($sinAlpha*$sinAlpha);
    $cos2SigmaM = $cosSigma - ((2*$sinU1*$sinU2)/$cosSqAlpha);
    // equatorial line: cosSqAlpha=0 (§6)
    if (is_nan($cos2SigmaM)) {$cos2SigmaM = 0;}
    $C = $f/(16*$cosSqAlpha*(4+$f*(4-3*$cosSqAlpha)));
    $lambdaP = $lambda;
    $lambda = $L + (1-$C) * $f * $sinAlpha *
      ($sigma + $C*$sinSigma*($cos2SigmaM+$C*$cosSigma*(-1+2*$cos2SigmaM*$cos2SigmaM)));
  } while (abs($lambda-$lambdaP) > 1e-12 && --$imit>0);
  // formula failed to converge
  if ($limit==0) {return NULL;}

  $uSq = ($cosSqAlpha * ($a*$a - $b*$b)) / ($b*$b);
  $A = 1 + $uSq/(16384*(4096+$uSq*(-768+$uSq*(320-175*$uSq))));
  $B = $uSq/1024 * (256+$uSq*(-128+$uSq*(74-47*$uSq)));
  $deltaSigma = $B*$sinSigma*($cos2SigmaM + $B/(4*($cosSigma*(-1+2*$cos2SigmaM*$cos2SigmaM)-
    $B/(6*$cos2SigmaM*(-3 + 4*$sinSigma*$sinSigma)*(-3 + 4*$cos2SigmaM*$cos2SigmaM)))));
  $s = $b*$A*($sigma-$deltaSigma);
  
  // initial & final bearings
  //var fwdAz = Math.atan2(cosU2*sinLambda, cosU1*sinU2-sinU1*cosU2*cosLambda);
  //var revAz = Math.atan2(cosU1*sinLambda, -sinU1*cosU2+cosU1*sinU2*cosLambda);
  return $s;
}

function _gmaps_coords_radius_ellipsoid($coords, $a, $b) {
  $lat = deg2rad($coords->latitude);

  $x = cos($lat)/$a;
  $y = sin($lat)/$b;
  return 1 / (sqrt($x*$x + $y*$y));
}
