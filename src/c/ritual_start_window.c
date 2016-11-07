#include <pebble.h>
#include "ritual_start_window.h"
#include "main.h"
#include "wakeups.h"

Window *ritual_startWindow;
TextLayer *ritual_start_text_layer;
char first_item_name[30];
char first_item_time_string[] = "00:00";

void ritual_start_window_show() {
  strncpy(first_item_name, current_item.name, sizeof(first_item_name));

  int minutes = current_item.time / 60;
  int seconds = current_item.time % 60;

  if (seconds < 10) {
    snprintf(first_item_time_string, sizeof(first_item_time_string), "%d:0%d", minutes, seconds);
  } else {
    snprintf(first_item_time_string, sizeof(first_item_time_string), "%d:%d", minutes, seconds);
  }

  window_stack_push(ritual_start_window_get_window(), true);
}


void ritual_start_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  ritual_start_text_layer = text_layer_create(GRect(0, 0, 144, 90));
  text_layer_set_text(ritual_start_text_layer, "It's time to start your routine.\nFirst up:");
  text_layer_set_font(ritual_start_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(ritual_start_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_start_text_layer));

  ritual_start_text_layer = text_layer_create(GRect(0, 80, 144, 120));
  text_layer_set_text(ritual_start_text_layer, first_item_name);
  text_layer_set_font(ritual_start_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(ritual_start_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_start_text_layer));

  ritual_start_text_layer = text_layer_create(GRect(0, 120, 144, 168));
  text_layer_set_text(ritual_start_text_layer, first_item_time_string);
  text_layer_set_font(ritual_start_text_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(ritual_start_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_start_text_layer));
}

void ritual_start_window_unload(Window *window) {
 text_layer_destroy(ritual_start_text_layer);
}

void ritual_start_window_create() {
  ritual_startWindow = window_create();
  window_set_window_handlers(ritual_startWindow, (WindowHandlers){
    .load = ritual_start_window_load,
    .unload = ritual_start_window_unload
  });
}

void ritual_start_window_destroy() {
  window_destroy(ritual_startWindow);
}

Window *ritual_start_window_get_window() {
  return ritual_startWindow;
}
