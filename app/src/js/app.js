Internet = require('internet'); // Communication with back-ends
Watch    = require('watch');    // Communication with watch
Geo      = require('geo');      // Geolocation

function openRoutesList(e) {
  Watch.displayTextFindingLocation();
  Geo.getLocation(function(loc) {
    Watch.displayTextFindingStops();
    Internet.getRoutes(loc.coords.latitude, loc.coords.longitude,
      Watch.sendRoutes,
      Watch.displayTextErrorStops
    );
  }, Watch.displayTextErrorLocation);
}

function openPredictions(uri) {
  Internet.getPredictions(uri,
    Watch.sendPredictions,
    Watch.displayTextErrorPredictions
  );
}

Watch.addEventListener('ready', openRoutesList);
Watch.addEventListener('route_selected', openPredictions);




