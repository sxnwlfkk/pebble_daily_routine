#pragma once

void next_ritual_window_create();
void next_ritual_window_destroy();
Window *next_ritual_window_get_window();
Window *next_ritual_window;
void next_ritual_window_show(time_t next_time);