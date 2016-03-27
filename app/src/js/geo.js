var TIMEOUT = 15000;
var MAXIMUM_AGE = 60000;

function getCoordinates(callback) {
  navigator.geolocation.getCurrentPosition(
    function(position) {
      callback(position.coords.latitude, position.coords.longitude)
    },
    locationError,
    {timeout: TIMEOUT, maximumAge: MAXIMUM_AGE}
  );
};

function locationError(err) {
  console.log('Error requesting location!');
};

module.exports.getCoordinates = getCoordinates;
