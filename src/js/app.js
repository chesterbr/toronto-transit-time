// Communications initialization

Pebble.addEventListener("ready",
  function(e) {
    console.log('JS Ready!');
  }
);

// Asynchronous Message Queue

var g_menu_messages = [];

function enqueue_msg(dict) {
  g_menu_messages.push(dict);
}

function send_queued_msgs() {
  var dict = g_menu_messages.shift();
  if (dict) {
    Pebble.sendAppMessage(dict, function(e) {
      send_queued_msgs();
    }, function(e) {
      console.log('Error sending weather info to Pebble!' + JSON.stringify(e));
    });
  }
}

// Location

// Helper for http requests
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


function locationSuccess(pos) {
  console.log("got location");
  var url = 'http://pebblettc.chester.me/menu' +
    '?lat=' + pos.coords.latitude +
    '&lon=' + pos.coords.longitude;

  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);

      var string_buffer_size = 0;
      for (var section_title in json) {
        items = json[section_title];
        string_buffer_size += section_title.length + 1;
        items.forEach(function(item) {
          string_buffer_size += item.title.length + 1;
          string_buffer_size += item.subtitle.length + 1;
        });
      }


      enqueue_msg({
        'KEY_MENU_SECTION_COUNT': Object.keys(json).length,
        'KEY_MENU_STRING_BUFFER_SIZE' : string_buffer_size
      });

      for (var section_title in json) {
        items = json[section_title];
        enqueue_msg({ 'KEY_MENU_SECTION_ITEMS_COUNT' : items.length });
        enqueue_msg({ 'KEY_MENU_SECTION_TITLE' : section_title });
        items.forEach(function(item) {
          enqueue_msg({ 'KEY_MENU_ITEM_TITLE' : item.title });
          enqueue_msg({ 'KEY_MENU_ITEM_SUBTITLE' : item.subtitle });
          // TODO store URL
        });
      }

      enqueue_msg({ 'KEY_MENU_SHOW' : 0 })

      send_queued_msgs();
    }
  );
}



function locationError(err) {
  console.log('Error requesting location!');
}

function getMenu() {
  console.log("getting position");
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    console.log('PebbleKit JS ready!');

    // Get the initial menu
    getMenu();
  }
);

