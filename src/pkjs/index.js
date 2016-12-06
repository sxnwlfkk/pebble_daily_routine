
// Clay support //
var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig);

var routine_dict = {
  'Operation': 1,
  'Routine_Id': 0,
  'Routine_Name': "Test Routine",
  'Routine_Item_No': 5,
  'Wakeup_On_Start': 0,
}


Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');

  // Update s_js_ready on watch
  // Pebble.sendAppMessage({'JSReady': 1});
  // Sending test dict
  Pebble.sendAppMessage(routine_dict);
  console.log('Sent test dictionary.');
});
