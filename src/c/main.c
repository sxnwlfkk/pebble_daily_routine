#include <pebble.h>
#include "next_ritual_window.h"
#include "ritual_start_window.h"
#include "ritual_end_window.h"
#include "ritual_item_window.h"
#include "button_handlers.h"
#include "wakeups.h"
#include "main.h"


// Settings //
Settings settings = {
  .weekdays = {1, 1, 1, 1, 1, 0, 0},
  .goal_time = {14,10},
  .item_keys = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114},
  .carry_time = 0,
  .current_item = -1,
  // .on_time = false;
};


// Items //
char item_names[num_of_items][30] = {"First", "Second", "Third",
                           "Fourth", "Fifth", "Sixth",
                           "Seventh", "Eighth", "Ninth",
                           "Tenth", "Eleventh", "Twelfth", "Thirteenth", "Fourteenth" "Freetime"};
int item_times[num_of_items] = {10, 200, 500, 400, 600, 150, 250, 350, 450, 50, 13, 55, 0};
Item current_item;


// Functions //

 void write_curr_item(int key) {
  persist_write_data(key, &current_item, sizeof(current_item));
}

 void load_curr_item(int key) {
  persist_read_data(key, &current_item, sizeof(current_item));
}

 void save_state() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

 void load_state() {
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}


/* Calculates the next occurence of morning routine. */
time_t calculate_next_ritual() {
  int hours = settings.goal_time[0];
  int minutes = settings.goal_time[1];
  int daily_time = time_start_of_today() + (hours*60*60) + (minutes*60);
  if (daily_time > time(NULL))
    return daily_time;
  return ONE_DAY + daily_time;
}


/* If user went to negative carry, distribute the loss between
   the remaining items proportional to their remaining times */
 void distribute_carry_loss() {
  int total_remaining_time = 0;

  // Count remaining time //
  for (int i = settings.current_item+1; i < num_of_items; i++) {
    load_curr_item(settings.item_keys[i]);
    total_remaining_time += current_item.remaining_time;
  }
  if (total_remaining_time != 0) {
    // Remove proportional loss from items //
    int proportional_loss;
    for (int i = settings.current_item+1; i < num_of_items; i++) {
      load_curr_item(settings.item_keys[i]);
      proportional_loss = (int) ((float)settings.carry_time * ((float) current_item.remaining_time / (float)total_remaining_time));
      current_item.remaining_time += proportional_loss; // Should be always negative
      write_curr_item(settings.item_keys[i]);
  }

  load_curr_item(settings.item_keys[settings.current_item]);
}

  // Set carry_time to zero after all this //
  settings.carry_time = 0;
}


// Call this when the ritual ends //
 void reset() {
  // Change current_item carry_time, remaining_times to default
  for (int i=0; i<num_of_items; i++) {
    load_curr_item(settings.item_keys[i]);
    current_item.remaining_time = current_item.time;
    current_item.carry_timer_timestamp = 0;
    current_item.timer_timestamp = 0;
    write_curr_item(settings.item_keys[i]);
  }
  settings.carry_time = 0;
  settings.current_item = -1;
}


// Calculated absolute value //

int abs(int val) {
  if (val < 0) {
    return (-1) * val;
  }
  return val;
}


/* Calculates how much earlier of late the routine is started. */
 int calculate_first_carry() {
  int carry = (int)(calculate_next_ritual() - time(NULL)) - settings.routine_length;
  return carry;
}


/* Setup after first run. */
 void first_setup() {
  int sum_time = 0; // Routine length

  // Construct the items and write to memory //
  for (int i = 0; i<num_of_items; i++) {
    sum_time += item_times[i];
    strncpy(current_item.name, item_names[i], sizeof(current_item.name));
    // Copy minutes and seconds. Should be converted to seconds only //
    current_item.time = item_times[i];
    current_item.remaining_time = current_item.time;
    write_curr_item(settings.item_keys[i]);
  }
  settings.routine_length = sum_time;
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
    if (settings.current_item == -1) {
      load_curr_item(settings.item_keys[0]);
    } else {
      load_curr_item(settings.item_keys[settings.current_item]);
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

}


 void deinit(void){
  save_state();
  write_curr_item(settings.item_keys[settings.current_item]);

  /* Close windows if opened. */
  if (window_is_loaded(nextRitualWindow)) {
    next_ritual_window_destroy();
  }
  if (window_is_loaded(ritual_itemWindow)) {
    ritual_item_window_destroy();
  }
  if (window_is_loaded(ritual_startWindow)) {
    ritual_start_window_destroy();
  }
  if (window_is_loaded(ritual_endWindow)) {
    ritual_end_window_destroy();
  }
}

 void open_starting_window() {
  // If routine is not in progress //
  if (settings.current_item == -1) {

    // If routine is not in progress //
    if (calculate_next_ritual() > time(NULL)) {

      next_ritual_window_create();
      window_set_click_config_provider(nextRitualWindow, next_ritual_window_click_config_provider);
      next_ritual_window_show(calculate_next_ritual() - settings.routine_length);
    }

  // If routine is in progress, load state and continue, where left off //
  } else {
    ritual_item_window_create();
    window_set_click_config_provider(ritual_itemWindow, item_window_click_config_provider);

    // Check how much time elapsed since the closing of the app //
    // If there is remaining time //
    if (current_item.remaining_time != 0) {
      int elapsed_t = (int)(current_item.timer_timestamp - time(NULL));
      // If the elapsed time is greater than the remaining time //
      if (elapsed_t <= 0) {
        settings.carry_time += (current_item.remaining_time + elapsed_t);
        current_item.remaining_time = 0;

      // If the elapsed time is smaller than the remaining time //
      } else {
        current_item.remaining_time = elapsed_t;
      }
    // If there isn't any time left, carry time should take it //
    } else {
      settings.carry_time = (int)(current_item.carry_timer_timestamp - time(NULL));
    }

    ritual_item_window_show();
  }
}


int main()
{
  init();
  open_starting_window();
  app_event_loop();
  deinit();
}
