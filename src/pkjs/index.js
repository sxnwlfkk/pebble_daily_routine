
// Clay support //
var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig);


Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');

  // Update s_js_ready on watch
  Pebble.sendAppMessage({'JSReady': 1});
});
