// External API calls

function getRoutes(lat, lon, callback) {
  var url = 'http://pebblettc.chester.me/menu' +
              '?lat=' + lat +
              '&lon=' + lon;
  xhrRequest(url, 'GET', callback);
};

function getPredictions(index, callback) {
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

module.exports.getRoutes = getRoutes;
module.exports.getPredictions = getPredictions
