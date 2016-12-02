#include <pebble.h>
#include "main.h"
#include "next_ritual_window.h"
#include "ritual_start_window.h"
#include "ritual_item_window.h"
#include "ritual_end_window.h"
#include "button_handlers.h"
#include "app_setup.h"
#include "wakeups.h"


//////////////////////////
// Start Window Buttons //
//////////////////////////

static void start_window_back_button_handler(ClickRecognizerRef recognizer, void *context) {
  deinit();
}


static void start_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // The zeroing is because the bug, which makes -14342231 from zero upon
  // restart. //
  current_item.remaining_time = current_item.time;
  routine.carry_time = 0;
  routine.finish_time = time(NULL) + routine.routine_length;

  routine.current_item++;
  load_curr_item(routine.item_keys[routine.current_item]);

  // Schedule wakeup at routine length from button press
  schedule_end_wakeup(time(NULL) + routine.routine_length);

  ritual_item_window_create();
  window_set_click_config_provider(ritual_item_window, item_window_click_config_provider);
  ritual_item_window_show();
}


static void start_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // This is also because the bug. //
  current_item.remaining_time = current_item.time;
  // routine.carry_time = 0;

  routine.carry_time = calculate_first_carry();

  // Why did I made this? It's not used anywhere else in the project//
  routine.finish_time = time_start_of_today() +
                             routine.goal_time[0]*60 +
                             routine.goal_time[1]*60*60;
  if (routine.carry_time < 0)
    distribute_carry_loss();

  routine.current_item++;
  load_curr_item(routine.item_keys[routine.current_item]);

  // Schedule wakeup at designated end point
  schedule_end_wakeup(calculate_next_ritual());

  ritual_item_window_create();
  window_set_click_config_provider(ritual_item_window, item_window_click_config_provider);
  ritual_item_window_show();
}


void start_window_click_config_provider(void *context) {
  ButtonId id_down = BUTTON_ID_DOWN; // Down button
  ButtonId id_up = BUTTON_ID_UP; // Up button
  ButtonId id_back = BUTTON_ID_BACK; // Back button

  if (routine.wakeup_on_start) {
    window_single_click_subscribe(id_down, start_window_down_click_handler);
  } else {
    window_single_click_subscribe(id_down, start_window_up_click_handler);
  }
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


static void item_window_select_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  return;
}


static void item_window_select_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  reset();
  cancel_end_wakeup();
  wu_check_next_start_time();
  save_state();
  init();
  open_starting_window();
}


static void item_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (routine.current_item <= 0)
    return;

  write_curr_item(routine.item_keys[routine.current_item]);

  routine.current_item -= 1;

  load_curr_item(routine.item_keys[routine.current_item]);

  ritual_item_window_create();
  window_set_click_config_provider(ritual_item_window, item_window_click_config_provider);
  ritual_item_window_show();
}


static void item_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // The bug again. //
  if (current_item.remaining_time < 0 && routine.current_item == -1) {
    current_item.remaining_time = current_item.time;
  }
  if (routine.carry_time < 0 && routine.current_item == -1) {
    routine.carry_time = 0;
  }

  // If time left, add to carry time
  if (current_item.remaining_time > 0) {
    routine.carry_time += current_item.remaining_time;

    // Set remaining time in current item to 0, it will come in handy, if we go backwards
    current_item.remaining_time = 0;
  }

  write_curr_item(routine.item_keys[routine.current_item]);

  if (routine.carry_time < 0)
    distribute_carry_loss();

  routine.current_item++;
  if (routine.current_item < routine.num_of_items) {
    load_curr_item(routine.item_keys[routine.current_item]);

    ritual_item_window_create();
    window_set_click_config_provider(ritual_item_window, item_window_click_config_provider);
    ritual_item_window_show();
 } else {
   // Set up end window, reset all progress data
   cancel_end_wakeup();
   ritual_end_window_create();
   window_set_click_config_provider(ritual_end_window, end_window_click_config_provider);
   ritual_end_window_show();
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
  window_long_click_subscribe(id_select, delay_ms, item_window_select_down_click_handler, NULL);
}


//////////////////////////////////
/// Next Ritual Window Buttons ///
//////////////////////////////////

// Next ritual window down button handler//
static void next_ritual_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  load_curr_item(routine.item_keys[0]);

  ritual_start_window_create();
  window_set_click_config_provider(ritual_start_window, start_window_click_config_provider);
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
