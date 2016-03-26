internet = require('internet');
geo = require('geo');
bluetooth = require('bluetooth');

function openRoutesList(e) {
  geo.getLocation(function(loc) {
    internet.getRoutes(loc.coords.latitude, loc.coords.longitude, function(routes) {
      bluetooth.sendRoutes(routes);
    })
  });
}

Pebble.addEventListener('ready', openRoutesList);




