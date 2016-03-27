var timeout = 15000;
var maximumAge = 60000;

function getCoordinates(callback) {
  navigator.geolocation.getCurrentPosition(
    function(position) {
      callback(position.coords.latitude, position.coords.longitude)
    },
    locationError,
    {timeout: timeout, maximumAge: maximumAge}
  );
};

function locationError(err) {
  console.log('Error requesting location!');
};

module.exports.getLocation = getCoordinates;
