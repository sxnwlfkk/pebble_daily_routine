#pragma once
#include <pebble.h>

#define SETTINGS_KEY 99
#define ONE_DAY 86400


typedef struct Item {
  char name[20];
  int time;
  int remaining_time;
  bool pre_carry_stage;
  time_t timer_timestamp;
  time_t carry_timer_timestamp;
} Item;


typedef struct Settings {
  int weekdays[7];
  int goal_time[2];
  int item_keys[11];
  int routine_length;
  int carry_time;
  int current_item;
} Settings;

Settings settings;
Item current_item;
char item_names[11][20];
int item_times[11];


 void open_starting_window();

 void init();
 void deinit();
 void first_setup();
 void setup();
 void reset();


 time_t calculate_next_ritual();
 void distribute_carry_loss();

 void write_curr_item(int key);
 void load_curr_item(int key);
 void save_state();
 void load_state();
