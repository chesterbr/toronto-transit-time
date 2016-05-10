#include "predictions.h"
#include "../layers/splash.h"
#include "../layers/predictions.h"
#include "../modules/bluetooth.h"
#include "../modules/util.h"
#include <pebble.h>

const int PREDICTIONS_REFRESH_SECONDS = 15;
const int PREDICTIONS_SCREEN_TIMEOUT_SECONDS = 120;

enum {
  // Inbound message keys
  KEY_PREDICTION_ROUTE_TEXT   = 200,
  KEY_PREDICTION_STOP_ADDRESS = 201,
  KEY_PREDICTION_TTC_ALERT    = 202,
  KEY_PREDICTION_SECONDS_1    = 203,
  KEY_PREDICTION_SECONDS_2    = 204,
  KEY_PREDICTION_SECONDS_3    = 205,
  KEY_PREDICTION_SHOW         = 206,
};

static Window *s_predictions_window;

static char* s_stop_address;
static DisplayableItem s_displayable_items[10];
static int s_displayable_items_count;
static int s_seconds_until_refresh;
static int s_seconds_until_exit;
static bool s_reset_scroll;

static void predictions_window_load(struct Window *window);
static void predictions_window_unload(struct Window *window);
static void update_prediction_times(tm *tick_time, TimeUnits units_changed);
static void config_provider(Window *window);
static void free_predictions_data_structures(void);

void predictions_window_inbox_received(DictionaryIterator *iterator, void *context) {
  for(int key = KEY_PREDICTION_ROUTE_TEXT; key <= KEY_PREDICTION_SHOW; key++) {
    Tuple *tuple = dict_find(iterator, key);
    if (tuple == NULL) {
      continue;
    }
    switch (tuple->key) {
      case KEY_PREDICTION_ROUTE_TEXT:
      case KEY_PREDICTION_TTC_ALERT:
        s_displayable_items_count++;
        s_displayable_items[s_displayable_items_count].text = strdup(tuple->value->cstring);
        s_displayable_items[s_displayable_items_count].times_count = 0;
        s_displayable_items[s_displayable_items_count].is_prediction = tuple->key == KEY_PREDICTION_ROUTE_TEXT;
        break;
      case KEY_PREDICTION_STOP_ADDRESS:
        s_stop_address = strdup(tuple->value->cstring);
        break;
      case KEY_PREDICTION_SECONDS_1:
      case KEY_PREDICTION_SECONDS_2:
      case KEY_PREDICTION_SECONDS_3:
        s_displayable_items[s_displayable_items_count].times[tuple->key - KEY_PREDICTION_SECONDS_1] = tuple->value->int32;
        s_displayable_items[s_displayable_items_count].times_count++;
        break;
      case KEY_PREDICTION_SHOW:
        s_displayable_items_count++;
        s_seconds_until_refresh = PREDICTIONS_REFRESH_SECONDS;
        predictions_window_make_visible(PRED_MODE_PREDICTIONS);
        tick_timer_service_subscribe(SECOND_UNIT, update_prediction_times);
        break;
    }
  }
}

void predictions_window_make_visible(int mode) {
  if (!s_predictions_window) {
    s_predictions_window = window_create();
    window_set_window_handlers(s_predictions_window, (WindowHandlers) {
      .load = predictions_window_load,
      .unload = predictions_window_unload,
    });
    window_set_click_config_provider(s_predictions_window, (ClickConfigProvider)config_provider);
  }
  if (window_stack_get_top_window() != s_predictions_window) {
    window_stack_push(s_predictions_window, true);
  }
  if (mode == PRED_MODE_LOADING) {
    s_displayable_items_count = -1;
    s_reset_scroll = true;
    s_seconds_until_exit = PREDICTIONS_SCREEN_TIMEOUT_SECONDS;
    splash_show("LOADING PREDICTIONS...");
  } else if (mode == PRED_MODE_PREDICTIONS) {
    predictions_layer_update(s_stop_address, s_displayable_items, s_displayable_items_count, s_reset_scroll);
    splash_hide();
  }
}

static void predictions_window_load(struct Window *window) {
  predictions_layer_init(s_predictions_window);
}

static void predictions_window_unload(struct Window *window) {
  predictions_layer_destroy();
  tick_timer_service_unsubscribe();
  free_predictions_data_structures();
}

static void update_prediction_times(tm *tick_time, TimeUnits units_changed) {
  if (--s_seconds_until_exit == 0) {
    window_stack_pop(true);
  } else if (--s_seconds_until_refresh > 0) {
    for(int i = 0; i < s_displayable_items_count; i++) {
      for(int j = 0; j < s_displayable_items[i].times_count; j++) {
        s_displayable_items[i].times[j] -= 1;
      }
    }
    predictions_layer_update(s_stop_address, s_displayable_items, s_displayable_items_count, false);
    layer_mark_dirty(window_get_root_layer(s_predictions_window));
  } else {
    s_displayable_items_count = -1;
    s_reset_scroll = false;
    splash_show("REFRESHING...");
    free_predictions_data_structures();
    tick_timer_service_unsubscribe();
    bluetooth_refresh_predictions();
  }
}

static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, predictions_layer_button_up_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, predictions_layer_button_down_handler);
}

static void free_predictions_data_structures(void) {
  for(int i = 0; i < s_displayable_items_count; i++) {
    free(s_displayable_items[i].text);
  }
  free(s_stop_address);
}
