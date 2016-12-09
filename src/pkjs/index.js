
// Clay support //
var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig);

var routine_dict = {
  'Operation': 1,
  'Routine_Id': 0,
  'Routine_Title': "Test Routine",
  'Routine_Item_No': 5,
  'Routine_Names': "First|Second|Third|Fourth|Fifth",
  'Routine_Times': "60|120|360|20|45",
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
