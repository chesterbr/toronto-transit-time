// Asynchronous communication with Pebble

var MAX_TIMES_FOR_DIRECITON = 3;
var ABOUT_SECTION_TITLE = 'Toronto Transit';
var ABOUT_ITEM_TITLE = 'by @chesterbr';
var ABOUT_ITEM_SUBTITLE = 'http://chester.me';

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

// These codes match the ones on splash.c
module.exports.displayTextFindingLocation  = function() { displayText(0) };
module.exports.displayTextErrorLocation    = function() { displayText(1) };
module.exports.displayTextFindingStops     = function() { displayText(2) };
module.exports.displayTextErrorStops       = function() { displayText(3) };
module.exports.displayTextErrorPredictions = function() { displayText(4) };

// Incoming messages

Pebble.addEventListener('appmessage', processIncomingMessage);

function processIncomingMessage(message) {
  stop = message.payload.KEY_REQUESTED_PREDICTION_SECTION;
  route = message.payload.KEY_REQUESTED_PREDICTION_ITEM;
  if (typeof(stop) == "number") {
    routeSelectedCallback(stopsAndRoutes[stop].routes[route].uri);
  }
}

// Messages to routes window

function buildMenuMessages() {
  appendToMessage('menu_section_count', stopsAndRoutes.length + 1);
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

  appendToMessage('menu_section_items_count', 1);
  appendToMessage('menu_section_title', ABOUT_SECTION_TITLE);
  appendToMessage('menu_item_title', ABOUT_ITEM_TITLE);
  appendToMessage('menu_item_subtitle', ABOUT_ITEM_SUBTITLE);
  enqueueMessage();

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
  size += ABOUT_SECTION_TITLE.length + 1;
  size += ABOUT_ITEM_TITLE.length + 1;
  size += ABOUT_ITEM_SUBTITLE.length + 1;
  return size;
}

// Messages to predictions window

function buildPredictionMessages() {
  // A prediction for a stop/route can have any number of "direction"s (usually
  // variations of the same line) and TTC "message"s (e.g., for construction)
  var directions = values(predictions, 'direction');
  var ttcAlerts = values(predictions, 'message');

  appendToMessage('prediction_stop_address', predictions.stopTitle);
  if (directions.length == 0) {
    appendToMessage('prediction_route_text', predictions.dirTitleBecauseNoPredictions);
    enqueueMessage();
  } else {
    directions.forEach(function(direction) {
      appendToMessage('prediction_route_text', direction.title);
      var times = values(direction, 'prediction');
      times.slice(0, MAX_TIMES_FOR_DIRECITON).forEach(function(time) {
        appendToMessage('prediction_seconds', parseInt(time.seconds));
      });
      enqueueMessage();
    });
  }

  ttcAlerts.forEach(function(alert) {
    appendToMessage('prediction_ttc_alert', alert.text);
    enqueueMessage();
  });

  appendToMessage('prediction_show', 0);
  enqueueMessage();
}

function values(obj, tagName) {
  // Needed because tags that appear multiple times on the TTC API XML
  // are represented as sometag_1, sometag_2, etc.
  return Object.keys(obj).sort().filter(function (key) {
    return key.indexOf(tagName + "_") == 0;
  }).map(function(key) {
    return obj[key];
  });
}

// Messages to splash layer

function displayText(textKey) {
  appendToMessage('splash_with_text', textKey);
  enqueueMessage();
  dispatchMessages();
}

// Outgoing message queuing/dispatching (for all windows/layers)

var MAX_ITEMS_PER_MESSAGE = 10;

var messageQueue = [];
var message = {};
var keySuffix = 1;

function appendToMessage(type, value) {
  var key = 'KEY_' + type.toUpperCase();
  if (type == 'menu_item_title') {
    key += '_' + keySuffix;
  } else if (type == 'menu_item_subtitle' || type == 'prediction_seconds') {
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
