var TIMEOUT = 15000;
var MAXIMUM_AGE = 60000;

function getCoordinates(callback) {
  var onSuccess, onError, options;

  onSuccess = function(position) {
    callback(position.coords.latitude, position.coords.longitude);
  };

  onError = function(err) {
    console.log('Error requesting location!');
  };

  options = {timeout: TIMEOUT, maximumAge: MAXIMUM_AGE}

  navigator.geolocation.getCurrentPosition(onSuccess, onError, options);
};

module.exports.getCoordinates = getCoordinates;
