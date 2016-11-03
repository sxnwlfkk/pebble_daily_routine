#include <pebble.h>
#include "next_ritual_window.h"
#include "ritual_start_window.h"
#include "ritual_end_window.h"
#include "ritual_item_window.h"
#include "main.h"

#define SETTINGS_KEY 99
#define ITEMS_KEY 100
#define ONE_DAY 86400


// Settings //

Settings settings = {
  .weekdays = {1, 1, 1, 1, 1, 0, 0},
  .goal_time = {21,07},
  .carry_time = 0,
  .current_item = -1
};

// Items //
char item_names[11][20] = {"Elso", "Masodik", "Harmadik", 
                           "Negyedik", "Otodik", "Hatodik", 
                           "Hetedik", "Nyolcadik", "Kilencedik", 
                           "Tizedik", "Freetime"};
int item_times[11] = {10, 200, 500, 400, 600, 150, 250, 350, 450, 50, 0};


// Functions //

/* Wrappers for reading/writing to/from memory. */
void load_item_array() {
  persist_read_data(ITEMS_KEY, &item_array, sizeof(item_array));
}

void write_item_array(int key) {
  persist_write_data(ITEMS_KEY, &item_array, sizeof(item_array));
}

void load_curr_item(int key) {
  current_item = &item_array[key];
}

void write_everything() {
  persist_write_data(ITEMS_KEY, &item_array, sizeof(item_array));
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}


/* Calculates the next occurence of morning routine. */
time_t calculate_next_ritual() {
  int daily_time = (settings.goal_time[0] * 60 * 60 + settings.goal_time[1] * 60) - settings.routine_length;
  if (time_start_of_today() + (time_t)daily_time > time(NULL))
    return time_start_of_today() + daily_time;
  return time_start_of_today() + ONE_DAY + daily_time;
}

/* If user went to negative carry, distribute the loss between
   the remaining items proportional to their remaining times */
void distribute_carry_loss() {
  int total_remaining_time = 0;
  
  // Count remaining time //
  for (int i = settings.current_item+1; i < 10; i++) {
    total_remaining_time += item_array[i].remaining_time;
  }
  
  // Remove proportional loss from items //
  int proportional_loss;
  for (int i = settings.current_item+1; i < 10; i++) {
    proportional_loss = (int) ((float)settings.carry_time * ((float)item_array[i].remaining_time / (float)total_remaining_time));
    item_array[i].remaining_time += proportional_loss; // Should be always negative
  }
  
  // Set carry_time to zero after all this //
  settings.carry_time = 0;
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
  int carry = (int)(calculate_next_ritual() - ONE_DAY - time(NULL));
  return carry;
}


// Button handlers //


// Start ritual window down button handler //

static void deinit(void);
static void start_window_down_click_handler(ClickRecognizerRef recognizer, void *context);
static void start_window_up_click_handler(ClickRecognizerRef recognizer, void *context);
static void back_button_handler(ClickRecognizerRef recognizer, void *context);

static void start_window_click_config_provider(void *context) {
  ButtonId id_down = BUTTON_ID_DOWN; // Down button
  ButtonId id_up = BUTTON_ID_UP; // Up button
  ButtonId id_back = BUTTON_ID_BACK; // Back button
  
  window_single_click_subscribe(id_down, start_window_down_click_handler);
  window_single_click_subscribe(id_up, start_window_up_click_handler);
  window_single_click_subscribe(id_back, back_button_handler);
}

static void end_window_click_config_provider(void *context) {
  ButtonId id_back = BUTTON_ID_BACK; // Back button
  
  window_single_click_subscribe(id_back, back_button_handler);
}

static void back_button_handler(ClickRecognizerRef recognizer, void *context) {
  deinit();
}


static void start_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (settings.current_item == 0)
    return;
  
  settings.current_item--;
  load_curr_item(settings.current_item);
  
  ritual_item_window_create();
  window_set_click_config_provider(ritual_itemWindow, start_window_click_config_provider);
  ritual_item_window_show();
}


static void start_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
 
  // If first run, calculate carry time //
  if (settings.current_item == -1) {
    // settings.carry_time = calculate_first_carry();
    //settings.carry_time = 60;
  } else {
    // If time left, add to carry time
    if (current_item->remaining_time > 0) {
      settings.carry_time += current_item->remaining_time;
    }
    
    if (settings.carry_time < 0)
      distribute_carry_loss();
    
    // Set remaining time in current item to 0, it will come in handy, if we go backwards
    current_item->remaining_time = 0;
  }
  
  settings.current_item++;
  if (settings.current_item < 11) {
    load_curr_item(settings.current_item);
  
    ritual_item_window_create();
    window_set_click_config_provider(ritual_itemWindow, start_window_click_config_provider);
    ritual_item_window_show(); 
 } else {
    ritual_end_window_create();
    window_set_click_config_provider(ritual_endWindow, end_window_click_config_provider);
    ritual_end_window_show("7.30");
  }
  
}


// Next ritual window down button handler//

static void next_ritual_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  load_curr_item(0);
  ritual_start_window_create();
  window_set_click_config_provider(ritual_startWindow, start_window_click_config_provider);
  ritual_start_window_show();
}

static void next_ritual_window_click_config_provider(void *context) {
  ButtonId id = BUTTON_ID_DOWN; // Down button
  
  window_single_click_subscribe(id, next_ritual_window_down_click_handler);
}


/* Setup after first run. */
void setup() {
  int sum_time = 0; // Routine length
  
  // Construct the items and write to memory //
  for (int i = 0; i<11; i++) {
    sum_time += item_times[i];
    strncpy(item_array[i].name, item_names[i], sizeof(item_array[i].name));
    // Copy minutes and seconds. Should be converted to seconds only //
    item_array[i].time = item_times[i];
    item_array[i].remaining_time = item_array[i].time;
  } 
  settings.routine_length = sum_time;
}


static void init(void){
  /* If there are settings in memory, load them. If there aren't,
  it's the first run. Run setup, write it to memory then load. */
  if (persist_exists(SETTINGS_KEY)) {
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
    persist_read_data(ITEMS_KEY, &item_array, sizeof(item_array));
  } else {
    setup();
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
    persist_write_data(ITEMS_KEY, &item_array, sizeof(item_array));
  } 

}


static void deinit(void){
  
  /* Write persistent data. */
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  persist_write_data(ITEMS_KEY, &item_array, sizeof(item_array));
  
  /* Close windows if opened. */
  if (window_is_loaded(nextRitualWindow)) {
    next_ritual_window_destroy();
  }
  if (window_is_loaded(ritual_itemWindow)) {
    ritual_item_window_destroy();
  }
  if (window_is_loaded(ritual_startWindow)) {
    ritual_start_window_destroy();
  }
  if (window_is_loaded(ritual_endWindow)) {
    ritual_end_window_destroy();
  }
}


int main() 
{
  init();
  
  // If routine is not in progress //
  if (settings.current_item == -1) {
    
    // If next routine starts in more than five minutes //
    if (calculate_next_ritual() - 300 > time(NULL)) {
      next_ritual_window_create();
      window_set_click_config_provider(nextRitualWindow, next_ritual_window_click_config_provider);
      next_ritual_window_show(calculate_next_ritual());
    // If next routine starts in less than five minutes //
    } else {
      load_curr_item(0);
      ritual_start_window_create();
      window_set_click_config_provider(nextRitualWindow, start_window_click_config_provider);
      ritual_start_window_show();
    }
    
  // If routine is in progress, load state and continue, where left off //
  } else {
    load_curr_item(settings.current_item);
    ritual_item_window_create();
    window_set_click_config_provider(ritual_itemWindow, start_window_click_config_provider);
    
    // Check how much time elapsed since the closing of the app //
    // If there is remaining time //
    if (current_item->remaining_time != 0) {
      int elapsed_t = (int)(current_item->timer_timestamp - time(NULL));
      // If the elapsed time is greater than the remaining time //
      if (elapsed_t <= 0) {
        settings.carry_time += (current_item->remaining_time + elapsed_t);
        current_item->remaining_time = 0;
        
      // If the elapsed time is smaller than the remaining time //
      } else {
        current_item->remaining_time = elapsed_t;
      }
    // If there isn't any time left, carry time should take it //
    } else {
      settings.carry_time = (int)(current_item->carry_timer_timestamp - time(NULL));
    }
    
    ritual_item_window_show();    
  }
  
  app_event_loop();
  deinit();
}