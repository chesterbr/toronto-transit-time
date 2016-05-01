var TIMEOUT = 15000;
var MAXIMUM_AGE = 60000;

function getCoordinates(onSuccess, onError) {
  options = { timeout: TIMEOUT, maximumAge: MAXIMUM_AGE }

  onSuccessWrapper = function(position) {
    onSuccess(position.coords.latitude, position.coords.longitude);
  };

  navigator.geolocation.getCurrentPosition(onSuccessWrapper, onError, options);
}

module.exports.getCoordinates = getCoordinates;
