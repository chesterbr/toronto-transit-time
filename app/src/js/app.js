internet = require('internet'); // Communication with back-ends
watch    = require('watch');    // Communication with watch
geo      = require('geo');      // Geolocation

function openRoutesList(e) {
  geo.getLocation(function(loc) {
    internet.getRoutes(loc.coords.latitude, loc.coords.longitude,
      watch.sendRoutes
    );
  });
}

function openPredictions(route) {
  console.log("callback triggered");
  console.log(route);
  // internet.getPredictions(route,
  //   watch.sendPredictions
  // );
}

watch.addEventListener('ready', openRoutesList);
watch.addEventListener('route_selected', openPredictions);




