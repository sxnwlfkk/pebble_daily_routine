#include <pebble.h>


void appmessage_setup() {
  
  // Largest expected inbox and outbox message sizes
  const uint32_t inbox_size = 64;
  const uint32_t outbox_size = 256;

  // Open AppMessage
  app_message_open(inbox_size, outbox_size);

}
