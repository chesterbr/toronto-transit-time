function getLocation(callback, errorCallback) {
  navigator.geolocation.getCurrentPosition(
    callback,
    errorCallback,
    {timeout: 10000, maximumAge: 60000}
  );
}

module.exports.getLocation = getLocation;
