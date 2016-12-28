#include <pebble.h>
#include "app_setup.h"
#include "main.h"

char ** make_name_array(int len, char *namestr) {
  char **new_array;
  if ((new_array = malloc(len * sizeof(char*))) == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Malloc failed at make_name_array, **new_array.");
  }

  int j = 0;
  for (int i = 0; i < len; i++) {
    if ((new_array[i] = malloc(30 * sizeof(char))) == NULL) {
      APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Malloc failed at make_name_array, **new_array[%d].", i);
    }
    int k = 0;
    while ((new_array[i][k] = namestr[j]) != '|' && new_array[i][k] != '\0') {
      j++;
      k++;
    }
    new_array[i][k] = '\0';
    j++;
  }
  return new_array;
}

int * make_num_array(int len, char *timestr) {
  int *new_times;
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Time string is: %s", timestr);
  if ((new_times = malloc(len * sizeof(int))) == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Malloc failed at make_time_array, **new_times.");
  }

  int j = 0;
  for (int i = 0; i < len; i++) {
    char curr_time[10] = "1";
    int k = 0;
    while ((curr_time[k] = timestr[j]) != '|' && curr_time[k] != '\0') {
      j++;
      k++;
    }
    curr_time[k] = '\0';
    j++;

    new_times[i] = atoi(curr_time);
  }
  return new_times;
}

void destroy_name_array(char ** namearray) {
  free(namearray);
}

void destroy_array(int * array) {
  free(array);
}

uint16_t inbox_new_routine(DictionaryIterator *iter) {

  /* Get routine id */
  int id = 0;
  Tuple *tuple_int_id = dict_find(iter, MESSAGE_KEY_Routine_Id);
  if (tuple_int_id) {
    id = tuple_int_id->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine id is %d", id);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine id failed");
    return 0;
  }

  /* Get routine name */
  char *routine_name;
  Tuple *tuple_str_name = dict_find(iter, MESSAGE_KEY_Routine_Title);
  if (tuple_str_name) {
    routine_name = tuple_str_name->value->cstring;
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine name is %s", routine_name);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine name failed");
    return 0;
  }

  /* Get item number */
  int item_no = 0;
  Tuple *test_int_item_no = dict_find(iter, MESSAGE_KEY_Routine_Item_No);
  if (test_int_item_no) {
    item_no = test_int_item_no->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine item number is %d", item_no);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine item number failed");
    return 0;
  }

  /* Get name array */
  char namestr[256];
  char ** name_array;
  Tuple *test_name_string = dict_find(iter, MESSAGE_KEY_Routine_Names);
  if (test_name_string) {
    strcpy(namestr, test_name_string->value->cstring);
    name_array = make_name_array(item_no, namestr);
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Name string is %s", namestr);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Somehow name generation failed.");
    return 0;
  }

  /* Get time array */
  int *times;
  Tuple *tuple_time_str = dict_find(iter, MESSAGE_KEY_Routine_Times);
  if (tuple_time_str) {
    strcpy(namestr, tuple_time_str->value->cstring);
    times = make_num_array(item_no, namestr);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine time string failed");
    return 0;
  }

  /* Get wakeup int */
  int wakeup = 0;
  int goal_time[2];
  Tuple *tuple_wakeup_int = dict_find(iter, MESSAGE_KEY_Wakeup_On_Start);
  if (tuple_wakeup_int) {
    wakeup = tuple_wakeup_int->value->int32;
    if (wakeup) {
      /* Get goal time */
      Tuple *tuple_goal_int = dict_find(iter, MESSAGE_KEY_Goal_1);
      if (tuple_goal_int) {
        goal_time[0] = tuple_goal_int->value->int32;
      } else {
        return 0;
      }
      tuple_goal_int = dict_find(iter, MESSAGE_KEY_Goal_2);
      if (tuple_goal_int) {
        goal_time[1] = tuple_goal_int->value->int32;
      } else {
        return 0;
      }
    } else {
      goal_time[0] = 0;
      goal_time[1] = 0;
    }
  } else {
    return 0;
  }

  save_state();
  routine_setup(id, item_no, routine_name, name_array, times, wakeup, goal_time);
  load_state();
  app_settings.current_routine = -1;

  destroy_array(times);
  destroy_name_array(name_array);

  return 1;
}

void send_version_to_phone() {
  // Declare the dictionary's iterator
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if(result == APP_MSG_OK) {
    int value = app_settings.version;
    dict_write_int(out_iter, MESSAGE_KEY_Version, &value, sizeof(int), true);

    // Send this message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Error preparing the outbox: %d", (int)result);
  }

}

void inbox_recieved_callback(DictionaryIterator *iter, void *context) {
  /* A new message has been successfully received */

  /* Recieving JSReady message */
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_JSReady);
  if(ready_tuple) {
    /* PebbleKit JS is ready! Safe to send messages */
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "JSReady set to true. Sending version: %d", app_settings.version);
    send_version_to_phone();
  } else {
    app_settings.version += inbox_new_routine(iter);
    send_version_to_phone();
  }


}


void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
  send_version_to_phone();
}


void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  // The message just sent has been successfully delivered
}


void outbox_failed_callback(DictionaryIterator *iter,
                                      AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}


void appmessage_setup() {

  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 1024;
  const uint32_t outbox_size = 64;

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);

  // Register to be notified about inbox received events
  app_message_register_inbox_received(inbox_recieved_callback);

  // Register to be notified about inbox dropped events
  app_message_register_inbox_dropped(inbox_dropped_callback);

  // Register to be notified about outbox sent events
  app_message_register_outbox_sent(outbox_sent_callback);

  // Register to be notified about outbox failed events
  app_message_register_outbox_failed(outbox_failed_callback);

}
