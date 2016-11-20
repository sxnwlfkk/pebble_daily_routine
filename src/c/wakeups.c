#include <pebble.h>
#include "main.h"
#include "button_handlers.h"
#include "ritual_end_window.h"
#include "next_ritual_window.h"

WakeupId id;


int expon(int x, int y) {
  int j;
  for (j = 0; j < y; j++) {
    x *= x;
  }
  return x;
}


////////////////////
// Wakeup Handler //
////////////////////

void wakeup_handler(WakeupId id, int32_t reason) {
  if (reason == 1 && settings.current_item == -1) {
    // Wakeup due to next ritual, and ritual isn't started //
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Wakeup reason: app closed, start time.");
    vibes_double_pulse();
    light_enable_interaction();

    next_ritual_window_create();
    window_set_click_config_provider(nextRitualWindow, next_ritual_window_click_config_provider);
    next_ritual_window_show(calculate_next_ritual() - settings.routine_length);

  } else if (reason == 1) {
    // Wakeup due to next ritual, but the ritual is started //
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Wakeup reason: app open, start time.");
    vibes_double_pulse();
    light_enable_interaction();

  } else if (reason == 2) {
    // Wakeup due to end times //
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Wakeup reason: end time reached.");
    vibes_double_pulse();
    light_enable_interaction();

    ritual_end_window_create();
    window_set_click_config_provider(ritual_endWindow, end_window_click_config_provider);
    ritual_end_window_show();
  }
}


//////////////////////////////
// Schedule wakeup function //
//////////////////////////////

void schedule_wakeup(int key, time_t w_time, int offset, int reason) {
  time_t wakeup_timestamp = w_time + offset;
  int high = offset + 90;
  int low = offset - 90;
  int cookie = reason;
  bool notify_if_missed = false;

  WakeupId id = wakeup_schedule(wakeup_timestamp, cookie, notify_if_missed);

  if (id < 0) {
    for (int i = 1; id < 0 && offset <= high && offset >= low; i++) {
      offset -= expon(-1, i) * i;
      id = wakeup_schedule(wakeup_timestamp + offset, cookie, notify_if_missed);
      if (id >= 0) {
        persist_write_int(key, id);
        break;
      }
    }
  } else {
    persist_write_int(key, id);
  }
}


////////////////////////////////////////////////////////
// Check next_ritual time, and schedule it if not set //
////////////////////////////////////////////////////////

void wu_check_next_start_time() {

  time_t next_time = calculate_next_ritual() - settings.routine_length;

  if (time(NULL) > next_time)
    next_time += ONE_DAY;

  if (persist_exists(WK_KEY1)) {
    // There is a record set, check if valid //

    int w_id = persist_read_int(WK_KEY1);
    time_t wk_time;

    if (!wakeup_query(w_id, &wk_time)) {
      // Previously scheduled but not valid //
      schedule_wakeup(WK_KEY1, next_time, -90, 1);

      APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Previous wakeup isn't valid, scheduling a new one:");
      log_formatted_time(wk_time);
    } else {
      // Scheduled and valid, just log it. //
      APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Previous wakeup time scheduled and valid:");
      log_formatted_time(wk_time);
    }
  } else {
      // Not scheduled, first run //
      schedule_wakeup(WK_KEY1, next_time, -90, 1);

      // Logging //
      int w_id = persist_read_int(WK_KEY1);
      time_t wk_time;

      APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Scheduling wakeup time first. It'll be at:");
      wakeup_query(w_id, &wk_time);
      log_formatted_time(wk_time);
  }
}


////////////////////////////////////////////
// Schedule routine end wakeup and cancel //
////////////////////////////////////////////

void schedule_end_wakeup(time_t end_time) {
  schedule_wakeup(WK_KEY2, end_time, 0, 2);

  // Logging //
  int w_id = persist_read_int(WK_KEY2);
  time_t wk_time;

  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Scheduling end time. It'll be at:");
  wakeup_query(w_id, &wk_time);
  log_formatted_time(wk_time);
}


void cancel_end_wakeup() {
  if (persist_exists(WK_KEY2)) {
    int w_id = persist_read_int(WK_KEY1);
    wakeup_cancel(w_id);
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Cancelled end wakeup.");
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Didn't cancel end wakeup, wasn't any.");
  }
}
