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

/* Generate new men data from app_settings and routines. */
void generate_menu_data() {

}

/* Setup of a routine, with data from app_comm. */
 void routine_setup(int id, int len, char * routine_name,
                    char ** name_array, int * time_array,
                    int wakeup, int * goal) {

  int sum_time = 0; // Routine length
  int zero_key = id * ITEM_PER_ROUTINE + ROUTINE_MAX_NUMBER;

  /* Construct the items and write to memory */
  for (int i = 0; i<len; i++) {
    strncpy(current_item.name, name_array[i], sizeof(current_item.name));
    current_item.z_time = current_item.remaining_time = time_array[i];
    sum_time += current_item.z_time;
    routine.item_keys[i] = zero_key + i;
    save_curr_item(routine.item_keys[i]);
  }

  strncpy(routine.name, routine_name, sizeof(routine.name));
  routine.num_of_items = len;
  routine.routine_length = sum_time;
  routine.carry_time = 0;
  routine.wakeup_on_start = (wakeup != 0) ? true : false;
  routine.current_item = -1;
  routine.ended = false;
  routine.goal_time[0] = *goal;
  routine.goal_time[1] = *(goal+1);
  routine.routine_length = sum_time;

  app_settings.set_routines[app_settings.no_of_rutines += 1] = id;

  strncpy(menu_data.routine_names[id], routine_name, sizeof(menu_data.routine_names));
  menu_data.routine_length[id] = sum_time / 60;
  if (routine.wakeup_on_start) {
    menu_data.start_times[id] = routine.goal_time[0]*60*60 + routine.goal_time[1]*60 - routine.routine_length;
  } else {
    menu_data.start_times[id] = 0;
  }

  save_routine(id);
  save_app_settings();
}

void init_structs() {
  AppSettings app_settings;
  Routine routine;
  Item current_item;
  MenuData menu_data;
}

void first_setup() {
  app_settings.no_of_rutines = 0;
  app_settings.current_routine = -1;
  app_settings.version = 0;
}

 void init(void){
  /* If there are settings in memory, load them. If there aren't,
  it's the first run. Run setup, write it to memory then load. */
  if (persist_exists(SETTINGS_KEY)) {
    init_structs();
    load_state();
  } else {
    init_structs();
    first_setup();
    save_app_settings();
    save_menu_data();
  }

  /* Comms handling */
  appmessage_setup();

  /* Wakueup handling */

  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    /* The app was started by a wakeup event. */
    WakeupId id = 0;
    int32_t reason = 0;

    /* Get details and handle the event appropriately. */
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  }
  wakeup_service_subscribe(wakeup_handler);
  wu_check_next_start_time();
  log_settings_dump();

}


 void deinit(void){
  save_state();
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


/* Call this when the ritual ends */
 void reset() {
  /* Change current_item carry_time, remaining_times to default */
  for (int i=0; i<routine.num_of_items; i++) {
    load_curr_item(routine.item_keys[i]);
    current_item.remaining_time = current_item.z_time;
    current_item.carry_timer_timestamp = 0;
    current_item.timer_timestamp = 0;
    save_curr_item(routine.item_keys[i]);
  }
  routine.carry_time = 0;
  routine.current_item = -1;
}
