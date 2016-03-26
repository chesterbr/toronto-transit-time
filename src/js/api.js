// External API calls

function getRoutesForLocation(lat, lon, callback) {
  var url = 'http://pebblettc.chester.me/menu' +
              '?lat=' + lat +
              '&lon=' + lon;
  xhrRequest(url, 'GET', callback);
};

function getPredictionsForRouteIndex(index, callback) {
  // TODO implement
};

function xhrRequest(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

module.exports.getRoutesForLocation = getRoutesForLocation;
module.exports.getPredictionsForRouteIndex = getPredictionsForRouteIndex
