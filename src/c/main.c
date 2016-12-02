#include <pebble.h>
#include "next_ritual_window.h"
#include "ritual_start_window.h"
#include "ritual_end_window.h"
#include "ritual_item_window.h"
#include "button_handlers.h"
#include "wakeups.h"
#include "app_comm.h"
#include "main_window.h"
#include "app_setup.h"
#include "main.h"


// Settings //
Routine routine = {
  .weekdays = {1, 1, 1, 1, 1, 0, 0},
  .goal_time = {10,47},
  .item_keys = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109},
  .carry_time = 0,
  .current_item = -1,
  .wakeup_on_start = false,
  .ended = false
};


// Items //
char item_names[routine.num_of_items][30] = {"First", "Second", "Third", "Fourth", "Fifth",
                                     "Sixth", "Seventh", "Eighth", "Ninth", "Tenth"};

int item_times[routine.num_of_items] = {60, 60, 60, 60, 60,
                                60, 60, 60, 60, 60};
Item current_item;


// Logging //

void log_formatted_time(time_t time_utc) {
  char wk_time_str[40];
  strftime(wk_time_str, sizeof(char[40]), "%a %D, %H:%M", gmtime(&time_utc));
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", wk_time_str);
}


void log_settings_dump() {
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "The settings contains:");
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "routine.goal_time = %d:%d", routine.goal_time[0], routine.goal_time[1]);
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Start time should be:");
  time_t next_routine_start = calculate_next_ritual() - routine.routine_length;
  if (time(NULL) > next_routine_start)
    next_routine_start += ONE_DAY;
  log_formatted_time(next_routine_start);
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "routine.carry_time = %d", routine.carry_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "routine.current_item = %d", routine.current_item);
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "routine.current_item = %d", routine.current_item);
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "routine.routine_length = %d", routine.routine_length);
}


// Functions //

 void write_curr_item(int key) {
  persist_write_data(key, &current_item, sizeof(current_item));
}

 void load_curr_item(int key) {
  persist_read_data(key, &current_item, sizeof(current_item));
}

 void save_state() {
  persist_write_data(SETTINGS_KEY, &routine, sizeof(routine));
}

 void load_state() {
  persist_read_data(SETTINGS_KEY, &routine, sizeof(routine));
}


/* Calculates the next occurence of morning routine. */
time_t calculate_next_ritual() {
  int hours = routine.goal_time[0];
  int minutes = routine.goal_time[1];
  int daily_time = time_start_of_today() + (hours*60*60) + (minutes*60);
  if (daily_time > time(NULL))
    return daily_time;
  return ONE_DAY + daily_time;
}


// If user went to negative carry, distribute the loss between
// the remaining items proportional to their remaining times
void distribute_carry_loss() {
  int total_remaining_time = 0;

  // Count remaining time //
  for (int i = routine.current_item+1; i < routine.num_of_items; i++) {
    load_curr_item(routine.item_keys[i]);
    total_remaining_time += current_item.remaining_time;
  }
  if (total_remaining_time != 0) {
    // Remove proportional loss from items //
    int proportional_loss;
    for (int i = routine.current_item+1; i < routine.num_of_items; i++) {
      load_curr_item(routine.item_keys[i]);
      proportional_loss = (int) ((float)routine.carry_time * ((float) current_item.remaining_time / (float)total_remaining_time));
      current_item.remaining_time += proportional_loss; // Should be always negative
      write_curr_item(routine.item_keys[i]);
    }
  load_curr_item(routine.item_keys[routine.current_item]);
  }
  // Set carry_time to zero after all this //
  routine.carry_time = 0;
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
  int carry = (int)(calculate_next_ritual() - time(NULL)) - routine.routine_length;
  return carry;
}

/* Calculates the persistent storage key */
uint16_t current_item_key() {
  return 10 + app_settings.curent_routine * 30 + routine.current_item
}




 void open_starting_window() {
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Open starting window.");

  if (routine.ended) {
    // If routine ended //
    ritual_end_window_create();
    window_set_click_config_provider(ritual_end_window, end_window_click_config_provider);
    ritual_end_window_show();
    routine.ended = false;

  } else if (routine.current_item == -1 && routine.wakeup_on_start) {
  // If routine is not in progress //

    next_ritual_window_create();
    window_set_click_config_provider(next_ritual_window, next_ritual_window_click_config_provider);
    next_ritual_window_show(calculate_next_ritual() - routine.routine_length);

  } else if (routine.current_item == -1 && !routine.wakeup_on_start) {
    load_curr_item(routine.item_keys[0]);

    ritual_start_window_create();
    window_set_click_config_provider(ritual_start_window, start_window_click_config_provider);
    ritual_start_window_show();

  } else {
    // If routine is in progress, load state and continue, where left off //
    ritual_item_window_create();
    window_set_click_config_provider(ritual_item_window, item_window_click_config_provider);

    // Check how much time elapsed since the closing of the app //
    // If there is remaining time //
    if (current_item.remaining_time != 0) {
      int elapsed_t = (int)(current_item.timer_timestamp - time(NULL));
      // If the elapsed time is greater than the remaining time //
      if (elapsed_t <= 0) {
        routine.carry_time += (current_item.remaining_time + elapsed_t);
        current_item.remaining_time = 0;

      // If the elapsed time is smaller than the remaining time //
      } else {
        current_item.remaining_time = elapsed_t;
      }
    // If there isn't any time left, carry time should take it //
    } else {
      routine.carry_time = (int)(current_item.carry_timer_timestamp - time(NULL));
    }
    ritual_item_window_show();
  }
}


int main() {
  init();
  open_starting_window();
  app_event_loop();
  deinit();
}
