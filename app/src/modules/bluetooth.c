#include "bluetooth.h"
#include "../layers/info.h"
#include <pebble.h>

enum {
  KEY_MENU_SELECTED_SECTION    = 125,
  KEY_MENU_SELECTED_ITEM       = 126,
};

static int s_last_predictions_section;
static int s_last_predictions_row;

void bluetooth_request_predictions(int section, int row) {
  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if (result == APP_MSG_OK) {
    dict_write_int(out_iter, KEY_MENU_SELECTED_SECTION, &section, sizeof(section), false);
    dict_write_int(out_iter, KEY_MENU_SELECTED_ITEM, &row, sizeof(row), false);
    result = app_message_outbox_send();
    if (result != APP_MSG_OK) {
      info_show("ERROR SENDING DATA TO PHONE");
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
    s_last_predictions_section = section;
    s_last_predictions_row = row;
  } else {
    info_show("ERROR TALKING TO PHONE");
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

void bluetooth_refresh_predictions() {
  bluetooth_request_predictions(s_last_predictions_section, s_last_predictions_row);
}
