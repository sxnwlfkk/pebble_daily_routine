#pragma once
#include <pebble.h>


#define ITEM_PER_ROUTINE 30
#define ONE_DAY 86400

#define SETTINGS_KEY 310
#define WK_KEY1 311
#define WK_KEY2 312
#define WK_KEY3 313
#define WK_KEY4 314
#define WK_KEY5 315
#define WK_KEY6 316
#define WK_KEY7 317
#define WK_KEY8 318


typedef struct Item {
  char name[20];
  int time;
  int remaining_time;
  bool pre_carry_stage;
  time_t timer_timestamp;
  time_t carry_timer_timestamp;
} Item;


typedef struct Routine {
  char routine_name[20];
  int num_of_items;
  int weekdays[7];
  bool wakeup_on_start;
  int goal_time[2];
  int routine_length;
  int carry_time;
  int current_item;
  bool ended;
  time_t finish_time;
} Routine;

Routine routine;
Item current_item;
char item_names[10][30];
int item_times[10];


void open_starting_window();


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
