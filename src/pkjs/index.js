
// Clay support //
var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig);

// Version //
var phone_version = 2;
var watch_version = 0;

var routine_dict_1 = {
  'Operation': 1,
  'Routine_Id': 0,
  'Routine_Title': "Test Routine",
  'Routine_Item_No': 5,
  'Routine_Names': "First|Second|Third|Fourth|Fifth",
  'Routine_Times': "60|120|360|20|45",
  'Wakeup_On_Start': 1,
  'Goal_1': 7,
  'Goal_2': 30,
}

var routine_dict_2 = {
  'Operation': 1,
  'Routine_Id': 1,
  'Routine_Title': "Test Routine 2",
  'Routine_Item_No': 5,
  'Routine_Names': "Elso|Masodik|Harmadik|Negyedik|Otodik",
  'Routine_Times': "60|120|360|20|45",
  'Wakeup_On_Start': 1,
  'Goal_1': 8,
  'Goal_2': 30,
}

var queue = {
  1: routine_dict_1,
  2: routine_dict_2
}

// Signal ready to the watch
Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready!');

  // Update s_js_ready on watch
  Pebble.sendAppMessage({'JSReady': 1});
});

// Get AppMessage events
Pebble.addEventListener('appmessage', function(e) {
  // Get the dictionary from the message
  var dict = e.payload;
  console.log('Got message: ' + JSON.stringify(dict));
  if (dict['Version'] == 0 || dict['Version']) {
    watch_version = dict['Version'];
    if (watch_version != phone_version) {
        // Sending test dict
        Pebble.sendAppMessage(queue[watch_version+1]);
        console.log('Sent test dictionary.');
    }
  } else {
      console.log('No key in dictionary.');
  }
});
