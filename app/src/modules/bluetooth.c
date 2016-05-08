#include "bluetooth.h"
#include "../layers/splash.h"
#include <pebble.h>

enum {
  KEY_REQUESTED_PREDICTION_SECTION    = 300,
  KEY_REQUESTED_PREDICTION_ITEM       = 301,
};

static int s_last_predictions_section;
static int s_last_predictions_row;

static void inbox_dropped_callback(AppMessageResult reason, void *context);
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

void bluetooth_initialize(AppMessageInboxReceived received_callback) {
  app_message_register_inbox_received(received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  // We use 1000 bytes mostly because this value counts in "Still allocated"
  // on the log (see http://bit.ly/24ElW41) and a round decimal number helps
  // humans in monitoring that number for potential leaks.
  const int inbox_size = 900;
  const int outbox_size = 100;
  app_message_open(inbox_size, outbox_size);
}

void bluetooth_request_predictions(int section, int row) {
  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if (result == APP_MSG_OK) {
    dict_write_int(out_iter, KEY_REQUESTED_PREDICTION_SECTION, &section, sizeof(section), false);
    dict_write_int(out_iter, KEY_REQUESTED_PREDICTION_ITEM, &row, sizeof(row), false);
    result = app_message_outbox_send();
    if (result != APP_MSG_OK) {
      splash_show("CAN'T FIND PHONE. IS BLUETOOTH ON? \U0001F612");
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
    s_last_predictions_section = section;
    s_last_predictions_row = row;
  } else {
    splash_show("CAN'T FIND PHONE. IS BLUETOOTH ON? \U0001F613");
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

void bluetooth_refresh_predictions(void) {
  bluetooth_request_predictions(s_last_predictions_section, s_last_predictions_row);
}

// Private

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  splash_show("CAN'T FIND PHONE. IS BLUETOOTH ON? \U0001F614");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  splash_show("CAN'T FIND PHONE. IS BLUETOOTH ON? \U0001F616");
}
