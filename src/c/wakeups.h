#pragma once


void schedule_wakeup(int key, time_t w_time, int offset, int reason);
void wakeup_handler();
void check_next_start_time();
WakeupId id;
