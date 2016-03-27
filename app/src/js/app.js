internet = require('internet');
geo = require('geo');
bluetooth = require('bluetooth');

function onPebbleReady(e) {
  openRoutesList();
};

function openRoutesList(e) {
  geo.getCoordinates(onCoordinatesSuccess);
};

function onCoordinatesSuccess(latitude, longitude) {
  internet.getRoutes(latitude, longitude, onRoutesSuccess)
};

function onRoutesSuccess(routes) {
  bluetooth.sendRoutes(routes);
};

Pebble.addEventListener('ready', onPebbleReady);
