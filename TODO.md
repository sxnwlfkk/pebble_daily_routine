# Simple program for pebble, containing my daily rituals

### Should be able to do:
* Persistent time tracking
* Vibrating
* Opening the app when it's start time
* Readable fonts
* Fast, non-freezy

### Policy
* Read the ritual from an easily editable format (name, time allocated)
* Calculate sum time
* Read finish at latest time point
* Lower the time proportionally if user is late
* Carry unused time to next item
* Differentiate between weekdays
* Display in [x/y] format the task progress

### Mechanism / logic
* Read config
  * Calculate alarm time for finish at the latest point
  * This is complie time
* Track time
  * Here starts runtime
* Alarm vibrate, and open app if it's time
* Display current task and count down from available time
  * No need for visual bar
  * Show in the negatives if running over time
  * Optional/future: display carry-over time in separate clock
* On click advance to next task, and carry on the leftover time
  * Update task progress
  * Optional: Ask if user wants to go forward
  * Optional/future: go back, if accidentally advanced the progress
    * In this case, only the leftover time should be carried backwards
* When finished with the obligatory tasks, display leftover time as free time
* When the whole thing ends, display next alarm time, start over
