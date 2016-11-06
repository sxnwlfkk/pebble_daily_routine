#include <pebble.h>
#include "main.h"
#include "next_ritual_window.h"
#include "ritual_start_window.h"
#include "ritual_item_window.h"
#include "ritual_end_window.h"
#include "button_handlers.h"

//////////////////////////
// Start Window Buttons //
//////////////////////////


static void start_window_back_button_handler(ClickRecognizerRef recognizer, void *context) {
  deinit();
}

static void start_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {

  // I don't know what causes this //
  if (current_item.remaining_time < 0 && settings.current_item == -1) {
    current_item.remaining_time = current_item.time;
  }

  if (settings.carry_time < 0 && settings.current_item == -1) {
    settings.carry_time = 0;
  }

  settings.carry_time = 0;

  settings.current_item++;
  load_curr_item(settings.item_keys[settings.current_item]);

  ritual_item_window_create();
  window_set_click_config_provider(ritual_itemWindow, item_window_click_config_provider);
  ritual_item_window_show();

}


static void start_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {

  // I don't know what causes this //
  if (current_item.remaining_time < 0 && settings.current_item == -1) {
    current_item.remaining_time = current_item.time;
  }

  if (settings.carry_time < 0 && settings.current_item == -1) {
    settings.carry_time = 0;
  }

  settings.carry_time = calculate_first_carry();

  if (settings.carry_time < 0)
    distribute_carry_loss();

  settings.current_item++;
  load_curr_item(settings.item_keys[settings.current_item]);

  ritual_item_window_create();
  window_set_click_config_provider(ritual_itemWindow, item_window_click_config_provider);
  ritual_item_window_show();

}



void start_window_click_config_provider(void *context) {
  ButtonId id_down = BUTTON_ID_DOWN; // Down button
  ButtonId id_up = BUTTON_ID_UP; // Up button
  ButtonId id_back = BUTTON_ID_BACK; // Back button

  window_single_click_subscribe(id_down, start_window_down_click_handler);
  window_single_click_subscribe(id_up, start_window_up_click_handler);
  window_single_click_subscribe(id_back, start_window_back_button_handler);
}

///////////////////////////
/// Item Window Buttons ///
///////////////////////////

// Item window handlers //


static void back_button_handler(ClickRecognizerRef recognizer, void *context) {
  deinit();
}


static void item_window_select_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  return;
}


static void item_window_select_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  reset();
  save_state();
  // clear_data_variables();
  init();
  open_starting_window();
}


static void item_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (settings.current_item <= 0)
    return;

  write_curr_item(settings.item_keys[settings.current_item]);

  settings.current_item -= 1;

  load_curr_item(settings.item_keys[settings.current_item]);

  ritual_item_window_create();
  window_set_click_config_provider(ritual_itemWindow, start_window_click_config_provider);
  ritual_item_window_show();
}


static void item_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {

  // I don't know what causes this //
  if (current_item.remaining_time < 0 && settings.current_item == -1) {
    current_item.remaining_time = current_item.time;
  }

  if (settings.carry_time < 0 && settings.current_item == -1) {
    settings.carry_time = 0;
  }

  // If first run, calculate carry time //
  if (settings.current_item == -1) {
    // settings.carry_time = calculate_first_carry();
    //settings.carry_time = 60;
  } else {
    // If time left, add to carry time
    if (current_item.remaining_time > 0) {
      settings.carry_time += current_item.remaining_time;

      // Set remaining time in current item to 0, it will come in handy, if we go backwards
      current_item.remaining_time = 0;
    }

    write_curr_item(settings.item_keys[settings.current_item]);

    if (settings.carry_time < 0)
      distribute_carry_loss();
  }

  settings.current_item++;
  if (settings.current_item < num_of_items) {
    load_curr_item(settings.item_keys[settings.current_item]);

    ritual_item_window_create();
    window_set_click_config_provider(ritual_itemWindow, item_window_click_config_provider);
    ritual_item_window_show();
 } else {
   // Set up end window, reset all progress data
    ritual_end_window_create();
    window_set_click_config_provider(ritual_endWindow, end_window_click_config_provider);
    char time_s[6] = "00:00 ";

    int hours = settings.goal_time[0];
    int minutes = settings.goal_time[1];
    if (minutes < 10) {
      snprintf(time_s, sizeof(time_s), "%i:0%i", hours, minutes);

    // Second is greater than 10 //
    } else {
      snprintf(time_s, sizeof(time_s), "%i:%i", hours, minutes);
    }

    ritual_end_window_show(time_s);
  }

}


// Start and Item Window config provider //

void item_window_click_config_provider(void *context) {
  ButtonId id_down = BUTTON_ID_DOWN; // Down button
  ButtonId id_up = BUTTON_ID_UP; // Up button
  ButtonId id_back = BUTTON_ID_BACK; // Back button
  ButtonId id_select = BUTTON_ID_SELECT; // Select button
  uint16_t delay_ms = 1000;

  window_single_click_subscribe(id_down, item_window_down_click_handler);
  window_single_click_subscribe(id_up, item_window_up_click_handler);
  window_single_click_subscribe(id_back, back_button_handler);
  window_long_click_subscribe(id_select, delay_ms, item_window_select_down_click_handler, item_window_select_up_click_handler);
}


//////////////////////////////////
/// Next Ritual Window Buttons ///
//////////////////////////////////

// Next ritual window down button handler//
static void next_ritual_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  load_curr_item(settings.item_keys[0]);

  ritual_start_window_create();

  window_set_click_config_provider(ritual_startWindow, start_window_click_config_provider);

  ritual_start_window_show();
}


static void next_ritual_window_back_click_handler(ClickRecognizerRef recognizer, void *context) {
  deinit();
}


void next_ritual_window_click_config_provider(void *context) {
  ButtonId id_down = BUTTON_ID_DOWN; // Down button
  ButtonId id_back = BUTTON_ID_BACK; // Back button

  window_single_click_subscribe(id_down, next_ritual_window_down_click_handler);
  window_single_click_subscribe(id_back, next_ritual_window_back_click_handler);

}


//////////////////////////
/// End Window Buttons ///
//////////////////////////

// End Window Button Handlers //
static void end_window_back_button_handler(ClickRecognizerRef recognizer, void *context) {
  reset();
  save_state();
  deinit();
}


// End Window Config Provider //
void end_window_click_config_provider(void *context) {
  ButtonId id_back = BUTTON_ID_BACK; // Back button

  window_single_click_subscribe(id_back, end_window_back_button_handler);
}
