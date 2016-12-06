#include <pebble.h>

bool s_js_ready;


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

  Tuple *test_int_operation = dict_find(iter, MESSAGE_KEY_Operation);
  if (test_int_operation) {
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Operation is set to one");
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
