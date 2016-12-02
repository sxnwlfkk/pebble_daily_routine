#include <pebble.h>
#include "app_setup.h"
#include "next_ritual_window.h"
#include "ritual_start_window.h"
#include "ritual_end_window.h"
#include "ritual_item_window.h"
#include "button_handlers.h"
#include "wakeups.h"
#include "app_comm.h"
#include "main_window.h"
#include "main.h"


/* Setup after first run. */
 void first_setup() {
  int sum_time = 0; // Routine length

  // Construct the items and write to memory //
  for (int i = 0; i<routine.num_of_items; i++) {
    sum_time += item_times[i];
    strncpy(current_item.name, item_names[i], sizeof(current_item.name));
    // Copy minutes and seconds. Should be converted to seconds only //
    current_item.time = item_times[i];
    current_item.remaining_time = current_item.time;
    write_curr_item(routine.item_keys[i]);
  }
  routine.routine_length = sum_time;
}


// Construct item_array to load from memory into //
 void setup() {
  strncpy(current_item.name, item_names[0], sizeof(current_item.name));
    current_item.time = item_times[0];
  }


 void init(void){
  /* If there are settings in memory, load them. If there aren't,
  it's the first run. Run setup, write it to memory then load. */
  if (persist_exists(SETTINGS_KEY)) {
    setup();
    load_state();
    if (routine.current_item == -1) {
      load_curr_item(routine.item_keys[0]);
    } else {
      load_curr_item(routine.item_keys[routine.current_item]);
    }
  } else {
    first_setup();
    save_state();
  }


  // Wakueup handling //

  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    // The app was started by a wakeup event.
    WakeupId id = 0;
    int32_t reason = 0;

    // Get details and handle the event appropriately
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  }
  wakeup_service_subscribe(wakeup_handler);
  wu_check_next_start_time();
  log_settings_dump();

  // Comms handling
  appmessage_setup();
}


 void deinit(void){
  save_state();
  write_curr_item(routine.item_keys[routine.current_item]);
  wu_check_next_start_time();

  /* Close windows if opened. */
  if (window_is_loaded(next_ritual_window)) {
    next_ritual_window_destroy();
  }
  if (window_is_loaded(ritual_item_window)) {
    ritual_item_window_destroy();
  }
  if (window_is_loaded(ritual_start_window)) {
    ritual_start_window_destroy();
  }
  if (window_is_loaded(ritual_end_window)) {
    ritual_end_window_destroy();
  }
}


// Call this when the ritual ends //
 void reset() {
  // Change current_item carry_time, remaining_times to default
  for (int i=0; i<routine.num_of_items; i++) {
    load_curr_item(routine.item_keys[i]);
    current_item.remaining_time = current_item.time;
    current_item.carry_timer_timestamp = 0;
    current_item.timer_timestamp = 0;
    write_curr_item(routine.item_keys[i]);
  }
  routine.carry_time = 0;
  routine.current_item = -1;
}
