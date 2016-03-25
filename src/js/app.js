
// External APIs

function api_get_routes_for_location(lat, lon, callback) {
  var url = 'http://pebblettc.chester.me/menu' +
              '?lat=' + lat +
              '&lon=' + lon;
  xhrRequest(url, 'GET', callback);
}

function xhrRequest(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// Asynchronous Message Queue

var message_queue = [];
var current_message;
var current_item_ord;

var MAX_ITEMS_PER_MESSAGE = 10;

function enqueue_messages(stops) {
  new_message();
  append_to_message('section_count', stops.length);
  append_to_message('string_buffer_size', stringBufferSize(stops));

  stops.forEach(function(stop) {
    new_message();
    routes = stop['routes'];
    append_to_message('section_items_count', routes.length);
    append_to_message('section_title', stop['stop']);
    routes.forEach(function(route) {
      append_to_message('item_title', route['route']);
      append_to_message('item_subtitle', route['direction']);
      // TODO store URL for subsequent call requests from Pebble
    });
  });

  append_to_message('show', 0);
}

function dispatch_messages() {
  enqueue_current_message_if_any();
  dispatch_until_queue_is_empty();
}

function dispatch_until_queue_is_empty() {
  var dict = message_queue.shift();
  if (dict) {
    console.log(JSON.stringify(dict));
    Pebble.sendAppMessage(dict, function(e) {
      dispatch_until_queue_is_empty();
    }, function(e) {
      console.log('Error sending message to Pebble!' + JSON.stringify(e));
    });
  }
}

function new_message() {
  enqueue_current_message_if_any();
  current_message = {}
  current_item_ord = 1;
}

function append_to_message(type, value) {
  var key = 'KEY_MENU_' + type.toUpperCase();
  if (type == 'item_title') {
    key += '_' + current_item_ord;
  } else if (type == 'item_subtitle') {
    key += '_' + current_item_ord++;
  }
  current_message[key] = value;

  if (current_item_ord > MAX_ITEMS_PER_MESSAGE) {
    new_message();
  }
}

function enqueue_current_message_if_any() {
  if (current_message && Object.keys(current_message).length > 0) {
    message_queue.push(current_message);
  }
}

function stringBufferSize(stops) {
  var size = 0;
  stops.forEach(function(stop) {
    routes = stop['routes'];
    size += stop['stop'].length + 1;
    routes.forEach(function(route) {
      size += route['route'].length + 1;
      size += route['direction'].length + 1;
    });
  });
  return size;
}

// Trigger location, api, dispatch

Pebble.addEventListener('ready',
  function(e) {
    getMenu();
  }
);

function getMenu() {
  console.log("getting position");
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

function locationSuccess(pos) {
  console.log("got location");

  api_get_routes_for_location(pos.coords.latitude, pos.coords.longitude,
    function(response) {
      enqueue_messages(JSON.parse(response));
      dispatch_messages();
    }
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

