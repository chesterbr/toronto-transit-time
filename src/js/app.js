Api = require('api');
Geo = require('geo');
Bluetooth = require('bluetooth');

function openRoutesList(e) {
  Geo.getCurrentLocation(function(location) {
    Api.getRoutesForLocation(location.coords.latitude, location.coords.longitude, function(result) {
      Bluetooth.sendRoutes(result);
    })
  });
}

Pebble.addEventListener('ready', openRoutesList);




