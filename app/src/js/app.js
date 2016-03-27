internet = require('internet');
geo = require('geo');
bluetooth = require('bluetooth');

function openRoutesList(e) {
  geo.getCoordinates(function(latitude, longitude) {
    internet.getRoutes(latitude, longitude, function(routes) {
      bluetooth.sendRoutes(routes);
    })
  });
}

Pebble.addEventListener('ready', openRoutesList);
