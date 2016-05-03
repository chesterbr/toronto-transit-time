Internet = require('internet'); // Communication with back-ends
Watch    = require('watch');    // Communication with watch
Geo      = require('geo');      // Geolocation

// Main events

Watch.addEventListener('ready', onPebbleReady);
Watch.addEventListener('route_selected', onRouteSelected);

function onPebbleReady(e) {
  openRoutesList();
};

function onRouteSelected(uri) {
  openPredictions(uri);
}

// Routes window flow

function openRoutesList() {
  Watch.displayTextFindingLocation();
  Geo.getCoordinates(onCoordinatesSuccess, onCoordinatesError);
};

function onCoordinatesSuccess(latitude, longitude) {
  Watch.displayTextFindingStops();
  Internet.getRoutes(latitude, longitude, onRoutesSuccess, onRoutesError);
};

function onRoutesSuccess(routes) {
  Watch.sendRoutes(routes);
};

// Predictions window flow

function openPredictions(uri) {
  Internet.getPredictions(uri, onPredictionsSuccess, onPredictionsError);
}

function onPredictionsSuccess(predictions) {
  Watch.sendPredictions(predictions);
}

// Error flows

function onCoordinatesError() {
  Watch.displayTextErrorLocation();
}

function onRoutesError() {
  Watch.displayTextErrorStops();
}

function onPredictionsError() {
  Watch.displayTextErrorPredictions();
}

