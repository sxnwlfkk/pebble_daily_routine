#pragma once
#include <pebble.h>


#define SETTINGS_KEY 99
#define ONE_DAY 86400
#define WK_KEY1 90
#define WK_KEY2 91
#define WK_KEY3 92
#define WK_KEY4 93
#define WK_KEY5 94
#define WK_KEY6 95
#define WK_KEY7 96
#define WK_KEY8 97
#define num_of_items 10


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
  int item_keys[num_of_items];
  int routine_length;
  int carry_time;
  int current_item;
  bool on_time;
  time_t finish_time;
} Settings;

Settings settings;
Item current_item;
char item_names[num_of_items][30];
int item_times[num_of_items];


void open_starting_window();

void init();
void deinit();
void first_setup();
void setup();
void reset();


time_t calculate_next_ritual();
void distribute_carry_loss();
int calculate_first_carry();

char make_into_time(int *first, int *second);
void write_curr_item(int key);
void load_curr_item(int key);
void save_state();
void load_state();


 // Logging //

void log_settings_dump();
void log_formatted_time(time_t time_utc);
