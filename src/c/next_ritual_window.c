#include <pebble.h>
#include "next_ritual_window.h"
#include "main.h"


Window *next_ritual_window;
TextLayer *nextRitualText, *next_ritual_countdown_text_layer;
time_t wakeup_timestamp = 0;
time_t next_ritual_time;
char next_ritual_countdown_text[32];
char r_len[9] = "000:000";


static void timer_handler(void *data) {
  int countdown = wakeup_timestamp - time(NULL);
  if (countdown < 0) {
    int hours = (-1)*countdown/60/60;
    int minutes = (-1)*countdown/60 - hours*60;
    if (minutes < 10){
      snprintf(next_ritual_countdown_text, sizeof(next_ritual_countdown_text), "-%d:0%d", hours, minutes);
    } else {
      snprintf(next_ritual_countdown_text, sizeof(next_ritual_countdown_text), "-%d:%d", hours, minutes);
    }
  } else {
    int hours = countdown/60/60;
    int minutes = countdown/60 - hours*60;
    if (minutes < 10){
      snprintf(next_ritual_countdown_text, sizeof(next_ritual_countdown_text), "%d:0%d", hours, minutes);
    } else {
      snprintf(next_ritual_countdown_text, sizeof(next_ritual_countdown_text), "%d:%d", hours, minutes);
    }
  }

  layer_mark_dirty(text_layer_get_layer(next_ritual_countdown_text_layer));
  app_timer_register(29000, timer_handler, data);
}


void next_ritual_window_show(time_t next_time){
  next_ritual_time = next_time;

  int start_time = routine.goal_time[0]*60*60 + routine.goal_time[1]*60 - routine.routine_length;
  int hours =  start_time/60/60;
  int minutes = (start_time/60)%60;
  if (minutes < 10) {
    snprintf(r_len, sizeof(r_len), "%d:0%d", hours, minutes);
  } else {
    snprintf(r_len, sizeof(r_len), "%d:%d", hours, minutes);
  }

  window_stack_push(next_ritual_window_get_window(), true);
}


void next_ritual_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  nextRitualText = text_layer_create(GRect(0, 0, 144, 95));
  // text_layer_set_text(nextRitualText, "Next evening routine starts in:");
  text_layer_set_text(nextRitualText, "Next morning routine starts in:");
  text_layer_set_font(nextRitualText, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(nextRitualText, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(nextRitualText));

  next_ritual_countdown_text_layer = text_layer_create(GRect(0, 95, 144, 130));
  text_layer_set_text(next_ritual_countdown_text_layer, next_ritual_countdown_text);
  text_layer_set_font(next_ritual_countdown_text_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(next_ritual_countdown_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(next_ritual_countdown_text_layer));

  next_ritual_countdown_text_layer = text_layer_create(GRect(0, 130, 144, 168));
  text_layer_set_text(next_ritual_countdown_text_layer, r_len);
  text_layer_set_font(next_ritual_countdown_text_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));
  text_layer_set_text_alignment(next_ritual_countdown_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(next_ritual_countdown_text_layer));

  wakeup_timestamp = next_ritual_time;
  app_timer_register(0, timer_handler, NULL);
}


void next_ritual_window_unload(Window *window) {
  text_layer_destroy(next_ritual_countdown_text_layer);
  text_layer_destroy(nextRitualText);
}


void next_ritual_window_create() {
  next_ritual_window = window_create();
  window_set_window_handlers(next_ritual_window, (WindowHandlers){
    .load = next_ritual_window_load,
    .unload = next_ritual_window_unload
  });
}


void next_ritual_window_destroy() {
  window_destroy(next_ritual_window);
}


Window *next_ritual_window_get_window() {
  return next_ritual_window;
}
