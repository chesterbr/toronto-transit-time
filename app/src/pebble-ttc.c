#include <pebble.h>
#include "windows/routes_list.h"
#include "windows/predictions.h"

static void app_init();
static void app_deinit();

static void initialize_communication_with_phone();
static void inbox_received_callback(DictionaryIterator *iterator, void *context);
static void inbox_dropped_callback(AppMessageResult reason, void *context);
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

int main(void) {
  app_init();
  app_event_loop();
  app_deinit();

  return 0;
}

static void app_init() {
  initialize_communication_with_phone();
  routes_list_init();
  // prediction_screen_init();
}

static void app_deinit() {
  routes_list_deinit();
}

static void initialize_communication_with_phone() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  const int inbox_size = 2048;
  const int outbox_size = 2048;
  app_message_open(inbox_size, outbox_size);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  routes_list_inbox_received(iterator, context);
  predictions_window_inbox_received(iterator, context);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

