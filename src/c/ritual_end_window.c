#include <pebble.h>
#include "ritual_end_window.h"
#include "main.h"

Window *ritual_endWindow;
TextLayer *ritual_end_text_layer;
char * end_time = "00:00 ";

void ritual_end_window_show(){

  int hours = (time(NULL) - time_start_of_today()) / 60 / 60;
  int minutes = ((time(NULL) - time_start_of_today()) / 60) % 60;

  if (minutes < 10) {
    snprintf(end_time, sizeof("00:00 "), "%i:0%i", hours, minutes);

  // Second is greater than 10 //
  } else {
    snprintf(end_time, sizeof("00:00 "), "%i:%i", hours, minutes);
  }

  window_stack_push(ritual_end_window_get_window(), true);
}

void ritual_end_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  ritual_end_text_layer = text_layer_create(GRect(0, 0, 144, 60));
  text_layer_set_text(ritual_end_text_layer, "Time to go!\nIt's");
  text_layer_set_font(ritual_end_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(ritual_end_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_end_text_layer));

  /* Here the time should be use supplied.*/
  ritual_end_text_layer = text_layer_create(GRect(0, 60, 144, 100));
  text_layer_set_text(ritual_end_text_layer, end_time);
  text_layer_set_font(ritual_end_text_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(ritual_end_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_end_text_layer));

  ritual_end_text_layer = text_layer_create(GRect(0, 100, 144, 168));
  text_layer_set_text(ritual_end_text_layer, "Have a productive day!");
  text_layer_set_font(ritual_end_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(ritual_end_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_end_text_layer));
}

void ritual_end_window_unload(Window *window) {
  text_layer_destroy(ritual_end_text_layer);
}

void ritual_end_window_create() {
  ritual_endWindow = window_create();
  window_set_window_handlers(ritual_endWindow, (WindowHandlers){
    .load = ritual_end_window_load,
    .unload = ritual_end_window_unload
  });
}

void ritual_end_window_destroy() {
  window_destroy(ritual_endWindow);
}

Window *ritual_end_window_get_window() {
  return ritual_endWindow;
}
