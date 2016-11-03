#include <pebble.h>
#include "next_ritual_window.h"
#include "main.h"

Window *nextRitualWindow;
TextLayer *nextRitualText, *next_ritual_countdown_text_layer;
time_t wakeup_timestamp = 0;
time_t next_ritual_time;
char next_ritual_countdown_text[32];

time_t countdown_time() {
  float hours = 15;
  int seconds = hours * 60 * 60;
  time_t cnt_time = time(NULL) + (time_t)seconds;
  return cnt_time;
}


static void timer_handler(void *data) {
  
  int countdown = wakeup_timestamp - time(NULL);
  int hours = countdown/60/60;
  int minutes = countdown/60 - hours*60;
  if (minutes < 10){
    snprintf(next_ritual_countdown_text, sizeof(next_ritual_countdown_text), "%d:0%d", hours, minutes);
  } else {
    snprintf(next_ritual_countdown_text, sizeof(next_ritual_countdown_text), "%d:%d", hours, minutes);
  }
  layer_mark_dirty(text_layer_get_layer(next_ritual_countdown_text_layer));
  app_timer_register(30000, timer_handler, data);
}

void next_ritual_window_show(time_t next_time){
  next_ritual_time = next_time;
  
  window_stack_push(next_ritual_window_get_window(), true);
}

void next_ritual_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  nextRitualText = text_layer_create(GRect(0, 0, 144, 100));
  text_layer_set_text(nextRitualText, "Next morning routine starts in:");
  text_layer_set_font(nextRitualText, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(nextRitualText, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(nextRitualText));
  
  next_ritual_countdown_text_layer = text_layer_create(GRect(0, 110, 144, 168));
  text_layer_set_text(next_ritual_countdown_text_layer, next_ritual_countdown_text);
  text_layer_set_font(next_ritual_countdown_text_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
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
  nextRitualWindow = window_create();
  window_set_window_handlers(nextRitualWindow, (WindowHandlers){
    .load = next_ritual_window_load,
    .unload = next_ritual_window_unload
  });
}

void next_ritual_window_destroy() {
  window_destroy(nextRitualWindow);
}

Window *next_ritual_window_get_window() {
  return nextRitualWindow;
}