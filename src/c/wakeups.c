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

  // Wakeup due to next ritual //
  if (reason == 1) {
    vibes_double_pulse();

    next_ritual_window_create();
    window_set_click_config_provider(nextRitualWindow, next_ritual_window_click_config_provider);
    next_ritual_window_show(calculate_next_ritual());
  // Wakeup due to end times //
  } else if (reason == 2) {
    vibes_double_pulse();

    ritual_end_window_create();
    window_set_click_config_provider(ritual_endWindow, end_window_click_config_provider);
    char time_s[5] = "00:00";

    int hours = settings.goal_time[0];
    int seconds = settings.goal_time[1];
    if (seconds < 10) {
      snprintf(time_s, sizeof(time_s), "%d:0%d", hours, seconds);

    // Second is greater than 10 //
    } else {
      snprintf(time_s, sizeof(time_s), "%d:%d", hours, seconds);
    }

    ritual_end_window_show(time_s);
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

void check_next_start_time() {
  if (persist_exists(WK_KEY1)) {
    int w_id = persist_read_int(WK_KEY1);
    if (!wakeup_query(w_id, NULL)) {
      schedule_wakeup(WK_KEY1, (time_t)calculate_next_ritual(), -90, 1);
    }
  }
}
