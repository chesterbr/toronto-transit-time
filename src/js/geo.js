function getCurrentLocation(callback) {
  navigator.geolocation.getCurrentPosition(
    callback,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

module.exports.getCurrentLocation = getCurrentLocation;
