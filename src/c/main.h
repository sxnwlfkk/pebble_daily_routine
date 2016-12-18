#pragma once
#include <pebble.h>


#define ITEM_PER_ROUTINE 30
#define ROUTINE_MAX_NUMBER 10
#define ONE_DAY 86400

#define SETTINGS_KEY 910
#define WK_KEY1 911
#define WK_KEY2 912
#define WK_KEY3 913
#define WK_KEY4 914
#define WK_KEY5 915
#define WK_KEY6 916
#define WK_KEY7 917
#define WK_KEY8 918
#define MENU_KEY 919

typedef struct MenuData {
  char routine_names[ROUTINE_MAX_NUMBER][20];
  int routine_length[ROUTINE_MAX_NUMBER];
  int start_times[ROUTINE_MAX_NUMBER];
} MenuData;

typedef struct AppSettings {
  int no_of_rutines;
  int set_routines[10];
  int current_routine;
  int version;
} AppSettings;

typedef struct Item {
  char name[20];
  int z_time;
  int remaining_time;
  bool pre_carry_stage;
  time_t timer_timestamp;
  time_t carry_timer_timestamp;
} Item;

typedef struct Routine {
  char name[20];
  int num_of_items;
  int item_keys[30];
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
AppSettings app_settings;
MenuData menu_data;
char item_names[10][30];
int item_times[10];


void open_starting_window();


time_t calculate_next_ritual();
void distribute_carry_loss();
int calculate_first_carry();

char make_into_time(int *first, int *second);
void save_menu_data();
void load_menu_data();
void save_curr_item(int key);
void load_curr_item(int key);
void save_routine(int key);
void load_routine(int key);
void save_app_settings();
void load_app_settings();
void save_state();
void load_state();


 // Logging //

void log_settings_dump();
void log_formatted_time(time_t time_utc);
