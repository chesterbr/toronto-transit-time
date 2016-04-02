// Asynchronous communication with Pebble

var stopsAndRoutes;
var routeSelectedCallback;

function addEventListener(event, callback) {
  if (event == "route_selected") {
    routeSelectedCallback = callback;
  } else {
    Pebble.addEventListener(event, callback);
  }
}

function sendRoutes(routes) {
  stopsAndRoutes = routes;
  buildMenuMessages();
  dispatchMessages();
}

function sendPredictions(predictions) {
  console.log(JSON.stringify(predictions));
  buildPredictionMessages();
  dispatchMessages();
}

module.exports.addEventListener = addEventListener;
module.exports.sendRoutes = sendRoutes;
module.exports.sendPredictions = sendPredictions;

Pebble.addEventListener('appmessage', processIncomingMessage);

// Stops and routes (menu)

function buildMenuMessages() {
  appendToMessage('menu_section_count', stopsAndRoutes.length);
  appendToMessage('menu_string_buffer_size', calcBufferSizeForMenuStrings());

  stopsAndRoutes.forEach(function(stop) {
    routes = stop['routes'];
    appendToMessage('menu_section_items_count', routes.length);
    appendToMessage('menu_section_title', stop['stop']);
    routes.forEach(function(route) {
      appendToMessage('menu_item_title', route['route']);
      appendToMessage('menu_item_subtitle', route['direction']);
    });
    enqueueMessage();
  });

  appendToMessage('menu_show', 0);
  enqueueMessage();
}

function calcBufferSizeForMenuStrings() {
  var size = 0;
  stopsAndRoutes.forEach(function(stop) {
    routes = stop['routes'];
    size += stop['stop'].length + 1;
    routes.forEach(function(route) {
      size += route['route'].length + 1;
      size += route['direction'].length + 1;
    });
  });
  return size;
}

// Predictions (screen)

function buildPredictionMessages() {
  // When there are no predictions, this field appears, just send and be done.
  if (predictions.dirTitleBecauseNoPredictions) {
    appendToMessage('prediction_title', predictions.dirTitleBecauseNoPredictions);
    appendToMessage('prediction_show', 0);
    enqueueMessage();
    return;
  }

  // Each direction goes on its own message (with a set of prediction times)
  forEach(predictions, function(direction) {
    appendToMessage('prediction_title', direction.title);
    forEach(direction, function(prediction) {
      appendToMessage('prediction_epoch_time', prediction.epochTime);
    });
    enqueueMessage();
  });

  // TTC alerts are also named "message" - don't mix up with our message
  var ttcAlerts = "";
  forEach(predictions, function(message) {
    ttcAlerts = ttcAlerts + message.text + " ";
  });
  if (ttcAlerts.trim() != "") {
    appendToMessage('prediction_messages', ttcAlerts.trim());
  }
  appendToMessage('prediction_show', 0);
  enqueueMessage();
}

function forEach(obj, callback) {
  // Needed because tags that appear multiple times on the TTC API XML
  // are repesented like sometag_1, sometag_2, etc.
  // The callback *argument* name is the tag name.
  var tagName = callback.toString().match(/function\s.*?\(([^)]*)\)/)[1];
  Object.keys(obj).forEach(function(key) {
    if (key.indexOf(tagName + "_") == 0) {
      callback(obj[key]);
    }
  });
}

// Outgoing message queuing/dispatching (for all screens)

var MAX_ITEMS_PER_MESSAGE = 10;

var messageQueue = [];
var message = {};
var keySuffix = 1;

function appendToMessage(type, value) {
  var key = 'KEY_' + type.toUpperCase();
  if (type == 'menu_item_title') {
    key += '_' + keySuffix;
  } else if (type == 'menu_item_subtitle' || type == 'prediction_epoch_time') {
    key += '_' + keySuffix++;
  }
  message[key] = value;

  if (keySuffix > MAX_ITEMS_PER_MESSAGE) {
    enqueueMessage();
  }
}

function enqueueMessage() {
  if (message && Object.keys(message).length > 0) {
    messageQueue.push(message);
  }
  message = {}
  keySuffix = 1;
}

function dispatchMessages() {
  var dict = messageQueue.shift();
  if (dict) {
    Pebble.sendAppMessage(dict, function(e) {
      dispatchMessages();
    }, function(e) {
      console.log('Error sending message to Pebble!' + JSON.stringify(e));
    });
  }
}

// Incoming message processing

function processIncomingMessage(message) {
  stop = message.payload.KEY_MENU_SELECTED_SECTION;
  route = message.payload.KEY_MENU_SELECTED_ITEM;
  if (typeof(stop) == "number") {
    routeSelectedCallback(stopsAndRoutes[stop].routes[route].uri);
  }
}
