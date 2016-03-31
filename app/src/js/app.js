internet = require('internet'); // Communication with back-end
watch    = require('watch');    // Communication with watch
geo      = require('geo');      // Geolocation

function openRoutesList(e) {
  geo.getLocation(function(loc) {
    internet.getRoutes(loc.coords.latitude, loc.coords.longitude,
      watch.sendRoutes
    );
  });
}

watch.routeSelected = function(route) {
  console.log("callback triggered");
  console.log(route);
  // internet.getPredictions(route,
  //   watch.sendPredictions
  // );
}

Pebble.addEventListener('ready', openRoutesList);




