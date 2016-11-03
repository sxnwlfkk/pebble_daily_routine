#include <pebble.h>


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
// Item item_array[11];
Item current_item;

void load_state();
