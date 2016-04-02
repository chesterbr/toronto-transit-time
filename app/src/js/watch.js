// Asynchronous communication with Pebble

var MAX_ITEMS_PER_MESSAGE = 10;

var stops_and_routes;
var routeSelectedCallback;

// These are used only to enqueue the messages
var message_queue = [];
var current_message;
var current_item_ord;

function addEventListener(event, callback) {
  if (event == "route_selected") {
    routeSelectedCallback = callback;
  } else {
    Pebble.addEventListener(event, callback);
  }
}

function sendRoutes(routes) {
  stops_and_routes = routes;
  enqueueMessages();
  dispatchMessages();
}

function sendPredictions(predictions) {
  console.log(JSON.stringify(predictions));


  forEach(predictions, function(direction) {
    console.log(direction.title);
    forEach(direction, function(prediction) {
      console.log(prediction.epochTime);
    });
    forEach(direction, function(message) {
      console.log(message.text);
    });
  });

}

// Tags that can appear multiple times in the TTC API XML are
// represented like tag_1, tag_2, etc., so this code will invoke
// the callback for each value of a tag_XXX property in obj.
//
// Notice that the callback argument name is the tag name.
function forEach(obj, callback) {
  keyPrefix = callback.toString().match(/function\s.*?\(([^)]*)\)/)[1];
  keys = Object.keys(obj).filter(function(key) {
    return key.indexOf(keyPrefix + "_") == 0;
  });
  keys.forEach(function(key) {
    callback(obj[key]);
  });
}


module.exports.addEventListener = addEventListener;
module.exports.sendRoutes = sendRoutes;
module.exports.sendPredictions = sendPredictions;

// Private

Pebble.addEventListener('appmessage', function(message) {
  stop_index = message.payload.KEY_MENU_SELECTED_SECTION;
  route_index = message.payload.KEY_MENU_SELECTED_ITEM;
  if (typeof(stop_index) == "number") {
    routeSelectedCallback(stops_and_routes[stop_index].routes[route_index].uri);
  }
});

function enqueueMessages() {
  new_message();
  append_to_message('section_count', stops_and_routes.length);
  append_to_message('string_buffer_size', stringBufferSize(stops_and_routes));

  stops_and_routes.forEach(function(stop) {
    new_message();
    routes = stop['routes'];
    append_to_message('section_items_count', routes.length);
    append_to_message('section_title', stop['stop']);
    routes.forEach(function(route) {
      append_to_message('item_title', route['route']);
      append_to_message('item_subtitle', route['direction']);
    });
  });

  append_to_message('show', 0);
}

function dispatchMessages() {
  enqueue_current_message_if_any();
  dispatch_until_queue_is_empty();
}

function dispatch_until_queue_is_empty() {
  var dict = message_queue.shift();
  if (dict) {
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
