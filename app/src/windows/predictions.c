#include "predictions.h"
#include "../layers/info.h"
#include <pebble.h>
#include <stdio.h>

enum {
  // Inbound message keys
  KEY_PREDICTION_DIRECTION_COUNT = 200,
  KEY_PREDICTION_TTC_ALERT_COUNT = 201,
  KEY_PREDICTION_TITLE           = 202,
  KEY_PREDICTION_SECONDS_1       = 203,
  KEY_PREDICTION_SECONDS_2       = 204,
  KEY_PREDICTION_SECONDS_3       = 205,
  KEY_PREDICTION_SECONDS_4       = 206,
  KEY_PREDICTION_SECONDS_5       = 207,
  KEY_PREDICTION_TTC_ALERT       = 208,
  KEY_PREDICTION_SHOW            = 209,
};

static Window *s_predictions_window;

static char **s_directions;
static char **s_ttc_alerts;

static int s_direction_index;

static char* strdup(const char* str);

void predictions_window_inbox_received(DictionaryIterator *iterator, void *context) {
  for(int key = KEY_PREDICTION_DIRECTION_COUNT; key <= KEY_PREDICTION_SHOW; key++) {
    Tuple *tuple = dict_find(iterator, key);
    if (tuple == NULL) {
      continue;
    }
    switch (tuple->key) {
      case KEY_PREDICTION_DIRECTION_COUNT:
        s_directions = malloc((int)tuple->value->int32 * sizeof(char*));
        s_direction_index = -1;
        break;
      case KEY_PREDICTION_TTC_ALERT_COUNT:
        break;
      case KEY_PREDICTION_TITLE:
        // s_directions[++s_direction_index] = strdup(tuple->value->cstring);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", s_directions[s_direction_index]);
        break;
      case KEY_PREDICTION_SECONDS_1:
      case KEY_PREDICTION_SECONDS_2:
      case KEY_PREDICTION_SECONDS_3:
      case KEY_PREDICTION_SECONDS_4:
      case KEY_PREDICTION_SECONDS_5:
      case KEY_PREDICTION_TTC_ALERT:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "text: %d", (int)tuple->value->int32);
        // s_directions[++s_direction_index] = strdup(tuple->value->cstring);
        break;
        break;
      case KEY_PREDICTION_SHOW:
        predictions_window_make_visible(PRED_MODE_PREDICTIONS);
        break;
    }
  }
}

void predictions_window_make_visible(int mode) {
  if (!s_predictions_window) {
    s_predictions_window = window_create();
    // window_set_window_handlers(s_predictions_window, (WindowHandlers) {
    //   .appear = predictions_window_appear,
    //   .disappear = predictions_window_disappear,
    // });
  }
  if (window_stack_get_top_window() != s_predictions_window) {
    window_stack_push(s_predictions_window, true);
  }
  if (mode == PRED_MODE_LOADING) {
    info_show("LOADING PREDICTIONS...");
  } else if (mode == PRED_MODE_PREDICTIONS) {
    // ensure we got the layer initialized and loaded with data
    info_hide();
  }
}

void predictions_window_disappear() {
  // free_sections_and_items_arrays();
  free(s_directions);
}

static char* strdup(const char* str)
{
  return strcpy(malloc(strlen(str) + 1),str);
}

void build_ui_and_show() {


}


