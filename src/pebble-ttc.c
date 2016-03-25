#include <pebble-ttc.h>
#include <routes_list.h>

int main(void) {
  app_init();
  app_event_loop();
  app_deinit();

  return 0;
}

void app_init() {
  initialize_communication_with_phone();
  routes_list_init();
}

void app_deinit() {
  routes_list_deinit();
}


void initialize_communication_with_phone() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  const int inbox_size = 2048;
  const int outbox_size = 2048;
  app_message_open(inbox_size, outbox_size);
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  routes_list_inbox_received(iterator, context);
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

