#pragma once
#include <pebble.h>

void init();
void deinit();
void first_setup();
void setup();
void reset();

 void routine_setup(int id, int len, char * routine_name,
                    char ** name_array, int * time_array,
                    int wakeup, int * goal);
