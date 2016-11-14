#include <pebble.h>
#include "ritual_item_window.h"
#include "ritual_end_window.h"
#include "button_handlers.h"
#include "main.h"

Window *ritual_itemWindow;
TextLayer *ritual_item_text_layer;
TextLayer *ritual_carry_text_layer;
int half_time;
int fifth_time;
char plus = 43;
char item_name[30];
char item_time_string[] = " 00:00 ";
char carry_time_string[] = " 00:00 ";
char item_out_of_max[] = "[00/00]";
bool vibrated = false;


// Time functions //
time_t make_time_from_int(int seconds) {
  time_t timer_time = time(NULL);
  return timer_time + (time_t)seconds;
}

int make_int_from_time(time_t timer_time) {
  int seconds = timer_time - time(NULL);
  return seconds;
}


// Show ritual_item_window //
void ritual_item_window_show(){

  // Count vibration times //
  half_time = current_item.remaining_time / 2;
  fifth_time = current_item.remaining_time / 5;

  // Copy name string //
  strncpy(item_name, current_item.name, sizeof(item_name));

  // Copy time string. Can only be positive or zero //
  if (current_item.remaining_time == 0) {
    snprintf(item_time_string, sizeof(item_time_string), "0:00");
  } else {
    snprintf(item_time_string, sizeof(item_time_string), "%d:%d", current_item.remaining_time/60, current_item.remaining_time%60);
  }

  // Copy carry_time string //
  // If positive //
  if (settings.carry_time >= 0) {
    if (settings.carry_time % 60 > 9) {
      snprintf(carry_time_string, sizeof(carry_time_string), "%d:%d", settings.carry_time/60, settings.carry_time%60);
    } else {
      snprintf(carry_time_string, sizeof(carry_time_string), "%d:0%d", settings.carry_time/60, settings.carry_time%60);
    }
  // If negative //
  } else {
    int minutes = ((-1) * settings.carry_time) / 60;
    int seconds = ((-1) * settings.carry_time) % 60;
    if (seconds > 9) {
      snprintf(carry_time_string, sizeof(carry_time_string), "-%d:%d", minutes, seconds);
    } else {
      snprintf(carry_time_string, sizeof(carry_time_string), "-%d:0%d", minutes, seconds);
    }
  }

  // Copy X/Y item string //
  snprintf(item_out_of_max, sizeof(item_out_of_max), "[%d/%d]", settings.current_item+1, num_of_items);

  window_stack_push(ritual_item_window_get_window(), true);
}


// Timer function //
static void timer_handler(void *data) {

  // Countdown from remaining time //
  if (current_item.remaining_time > 0) {

    // Count and render time strings //
    current_item.remaining_time = current_item.timer_timestamp - time(NULL);

    // Vibrate //
    if ((current_item.remaining_time == half_time || current_item.remaining_time == fifth_time) &&
          !vibrated) {
      vibes_double_pulse();
      vibrated = true;
    }
    if (current_item.remaining_time == half_time-2 || current_item.remaining_time == fifth_time-2) {
      vibrated = false;
    }

    int minutes = current_item.remaining_time / 60;
    int seconds = current_item.remaining_time % 60;
    if (seconds < 10){
      snprintf(item_time_string, sizeof(item_time_string), "%d:0%d", minutes, seconds);
    } else {
      snprintf(item_time_string, sizeof(item_time_string), "%d:%d", minutes, seconds);
    }

    layer_mark_dirty(text_layer_get_layer(ritual_item_text_layer));

  // Setting up countdown from carry time //
  } else if (current_item.remaining_time == 0 && current_item.pre_carry_stage) {
      vibes_double_pulse();
      current_item.carry_timer_timestamp = make_time_from_int(settings.carry_time);
      current_item.pre_carry_stage = false;

  // Countdown from carry time //
  } else {
    settings.carry_time = (int)(current_item.carry_timer_timestamp - time(NULL));

    if (settings.carry_time == 0) {
      vibes_double_pulse();
    }

    if (settings.carry_time == 0 && !vibrated) {
      vibes_double_pulse();
      vibrated = true;
    }

    if (settings.carry_time < 0) {
      int minutes = ((-1) * settings.carry_time) / 60;
      int seconds = ((-1) * settings.carry_time) % 60;
      if (seconds < 10) {
        snprintf(carry_time_string, sizeof(carry_time_string), "-%d:0%d", minutes, seconds);
      } else {
        snprintf(carry_time_string, sizeof(carry_time_string), "-%d:%d", minutes, seconds);
      }
    } else {
      int minutes = settings.carry_time / 60;
      int seconds = settings.carry_time % 60;
      if (seconds < 10) {
        snprintf(carry_time_string, sizeof(carry_time_string), "%d:0%d", minutes, seconds);
      } else {
        snprintf(carry_time_string, sizeof(carry_time_string), "%d:%d", minutes, seconds);
      }
    }
    layer_mark_dirty(text_layer_get_layer(ritual_carry_text_layer));
  }
  if (current_item.remaining_time == 0) {
    app_timer_register(0, timer_handler, data);
  } else {
  app_timer_register(1000, timer_handler, data);
  }
}


void ritual_item_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  ritual_item_text_layer = text_layer_create(GRect(0, 0, 144, 30));
  text_layer_set_text(ritual_item_text_layer, item_out_of_max);
  text_layer_set_font(ritual_item_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(ritual_item_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_item_text_layer));

  ritual_item_text_layer = text_layer_create(GRect(0, 30, 144, 60));
  text_layer_set_text(ritual_item_text_layer, item_name);
  text_layer_set_font(ritual_item_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(ritual_item_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_item_text_layer));

  ritual_item_text_layer = text_layer_create(GRect(0, 70, 144, 120));
  text_layer_set_text(ritual_item_text_layer, item_time_string);
  text_layer_set_font(ritual_item_text_layer, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
  text_layer_set_text_alignment(ritual_item_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_item_text_layer));

  ritual_carry_text_layer = text_layer_create(GRect(0, 115, 144, 168));
  text_layer_set_text(ritual_carry_text_layer, carry_time_string);
  text_layer_set_font(ritual_carry_text_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));
  text_layer_set_text_alignment(ritual_carry_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ritual_carry_text_layer));

  vibrated = false;
  current_item.pre_carry_stage = true;
  current_item.timer_timestamp = make_time_from_int(current_item.remaining_time);
  app_timer_register(0, timer_handler, NULL);
}

void ritual_item_window_unload(Window *window) {
  text_layer_destroy(ritual_item_text_layer);
  text_layer_destroy(ritual_carry_text_layer);
  ritual_itemWindow = NULL;
}

void ritual_item_window_disappear(Window *window) {
  save_state();
  write_curr_item(settings.item_keys[settings.current_item]);
}


void ritual_item_window_create() {
  ritual_itemWindow = window_create();
  window_set_window_handlers(ritual_itemWindow, (WindowHandlers){
    .load = ritual_item_window_load,
    .unload = ritual_item_window_unload,
    .disappear = ritual_item_window_disappear
  });
}

void ritual_item_window_destroy() {
  window_destroy(ritual_itemWindow);
}

Window *ritual_item_window_get_window() {
  return ritual_itemWindow;
}
