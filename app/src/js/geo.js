function getLocation(callback) {
  navigator.geolocation.getCurrentPosition(
    callback,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

module.exports.getLocation = getLocation;

// Private

function locationError(err) {
  console.log('Error requesting location!');
}

