var GenericWeather = require('pebble-generic-weather');
var genericWeather = new GenericWeather();

Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');
  /// Notify as soon as possible that JS is ready
  Pebble.sendAppMessage({'JSReady': 1});
  // Import the Clay package
    var Clay = require('pebble-clay');
    // Load our Clay configuration file
    var clayConfig = require('./config');
    // Initialize Clay
    var clay = new Clay(clayConfig);
});

Pebble.addEventListener('appmessage', function(e) {

  genericWeather.appMessageHandler(e);
});