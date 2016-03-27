function getLocation(callback) {
var timeout = 15000;
var maximumAge = 60000;
  navigator.geolocation.getCurrentPosition(
    callback,
    locationError,
    {timeout: 15000, maximumAge: 60000}
    {timeout: timeout, maximumAge: maximumAge}
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

module.exports.getLocation = getLocation;
