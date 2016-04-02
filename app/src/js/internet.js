// External API calls
// All returns are JavaScript objects

function getRoutes(lat, lon, callback) {
  var url = 'http://pebblettc.chester.me/menu' +
              '?lat=' + lat +
              '&lon=' + lon;
  xhrRequest(url, 'GET', function(json_routes) {
    routes = JSON.parse(json_routes);
    callback(routes);
  });
};

function getPredictions(url, callback) {
  xhrRequest(url, 'GET', function(xml_predictions) {
    predictions = ttcXmlToJson(xml_predictions).body.predictions;
    callback(predictions);
  });
};

module.exports.getRoutes = getRoutes;
module.exports.getPredictions = getPredictions

// Private

// TODO error handling
function xhrRequest(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// This parser will only work with the TTC XML (all values are strings, no comments,
// no tags inside strings, no malicious code, etc.). DON'T use with generic XML.
function ttcXmlToJson(xml) {
  // Remove <?xml.. ?> header and expand self-closing tags (<tag/> » <tag></>)
  xml = xml.replace(/<\?.*\?>/g,'').replace(/\/>/g, '></>');

  // Split tags in a list without delimiters ("<" and ">"), that is:
  //   '<tag foo=1><other_tag></other_tag></tag>'
  // becomes
  //   ['tag foo=1', 'other_tag', '/other_tag', '/tag']
  xml = ">" + xml + "<";
  tags = xml.split(/>\s*</);

  // Transform each element of the list in JSON text, like:
  //   'tag a="b" c="d"'  »»becomes»»  '"tag": {"a": "b","c": "d"',
  //   '/tag'             »»becomes»»  '}'
  jsonText = "{";
  uniqueSuffix = 1;
  for (i = 0; i < tags.length; i++) {
    tag = tags[i].trim();
    if (tag == "") { continue; }

    // Closing tag
    if (tag[0] == "/") {
      jsonText = jsonText + "},";
      continue;
    }

    splitPos = (tag + ' ').indexOf(' ');
    tagName = tag.substring(0, splitPos);
    tagAttrs = tag.substring(splitPos + 1);

    // These tags may appear more than once
    if (tagName =='direction' || tagName == 'message' || tagName == 'prediction') {
      tagName = tagName + "_" + (uniqueSuffix++);
    }

    // Opening tag
    jsonAttrs = '"' + tagAttrs.replace(/=/g, '":').replace(/" /g,'","');
    jsonText = jsonText + '"' + tagName + '":{' + jsonAttrs + ",";
  }

  // Get rid of trailing commas in hashes and close the
  // top-level object, making the string a parse-able JSON
  jsonText = jsonText.replace(/,}/g, '}');
  jsonText = jsonText.slice(0, -1) + "}";

  return JSON.parse(jsonText);
}
