#include <pebble.h>

bool s_js_ready;

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
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Trying to copy to new arrays.");
    while ((new_array[i][j] = namestr[j]) != '|' || new_array[i][j] != '\0') {
      j++;
    }
    new_array[i][j] = '\0';
    j++;
  }
  return new_array;
}

void destroy_name_array(char ** namearray) {
  free(namearray);
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Memory freed.");
}


void inbox_recieved_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received

  // Recieving JSReady message
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_JSReady);
  if(ready_tuple) {
    // PebbleKit JS is ready! Safe to send messages
    s_js_ready = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "JSReady set to true.");
  }

/*
  Tuple *test_int_tuple = dict_find(iter, MESSAGE_KEY_test_int);
  if(test_int_tuple) {
    // This value was stored as JS Number, which is stored here as int32_t
    int32_t test_int = test_int_tuple->value->int32;
  } */

  int item_no = 0;
  Tuple *test_int_item_no = dict_find(iter, MESSAGE_KEY_Routine_Item_No);
  if (test_int_item_no) {
    item_no = test_int_item_no->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine item number is %d", item_no);
  }

  char namestr[256];
  char ** name_array;
  Tuple *test_name_string = dict_find(iter, MESSAGE_KEY_Routine_Name);
  if (test_name_string) {
    strcpy(namestr, test_name_string->value->cstring);
    name_array = make_name_array(item_no, namestr);
    for (int i = 0; i < item_no; i++) {
      APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Routine item name is %s", name_array[i]);
    }
    destroy_name_array(name_array);
  }
}


void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
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
  const uint32_t inbox_size = 64;
  const uint32_t outbox_size = 256;

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
