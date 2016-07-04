Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');
  // Import the Clay package
    var Clay = require('pebble-clay');
    // Load our Clay configuration file
    var clayConfig = require('./config');
    // Initialize Clay
    var clay = new Clay(clayConfig);
});