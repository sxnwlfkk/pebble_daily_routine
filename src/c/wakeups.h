#pragma once


void schedule_wakeup(int key, time_t w_time, int offset, int reason);
void wakeup_handler();
void wu_check_next_start_time();
void schedule_end_wakeup(time_t end_time);
void cancel_end_wakeup();
WakeupId id;
