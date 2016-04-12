Internet = require('internet'); // Communication with back-ends
Watch    = require('watch');    // Communication with watch
Geo      = require('geo');      // Geolocation

function openRoutesList(e) {
  Geo.getLocation(function(loc) {
    Internet.getRoutes(loc.coords.latitude, loc.coords.longitude,
      Watch.sendRoutes
    );
  });
}

function openPredictions(uri) {
  Internet.getPredictions(uri,
    Watch.sendPredictions
  );
}

Watch.addEventListener('ready', openRoutesList);
Watch.addEventListener('route_selected', openPredictions);




