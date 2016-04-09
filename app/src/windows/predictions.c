#include "predictions.h"
#include "../layers/info.h"
#include <pebble.h>
#include <stdio.h>

enum {
  // Inbound message keys
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
static TextLayer *s_text_layer;

typedef struct {
  char* text;
  bool is_prediction;
  int times_count;
  int times[5];
} DisplayableItem;

static DisplayableItem s_displayable_items[10];
static int s_displayable_items_count;
static char s_predictions_full_text[4000];

static char* strdup(const char* str);

void predictions_window_inbox_received(DictionaryIterator *iterator, void *context) {
  for(int key = KEY_PREDICTION_TITLE; key <= KEY_PREDICTION_SHOW; key++) {
    Tuple *tuple = dict_find(iterator, key);
    if (tuple == NULL) {
      continue;
    }
    switch (tuple->key) {
      case KEY_PREDICTION_TITLE:
      case KEY_PREDICTION_TTC_ALERT:
        s_displayable_items_count++;
        s_displayable_items[s_displayable_items_count].text = strdup(tuple->value->cstring);
        s_displayable_items[s_displayable_items_count].times_count = 0;
        s_displayable_items[s_displayable_items_count].is_prediction = tuple->key == KEY_PREDICTION_TITLE;
        break;
      case KEY_PREDICTION_SECONDS_1:
      case KEY_PREDICTION_SECONDS_2:
      case KEY_PREDICTION_SECONDS_3:
      case KEY_PREDICTION_SECONDS_4:
      case KEY_PREDICTION_SECONDS_5:
        s_displayable_items[s_displayable_items_count].times[tuple->key-KEY_PREDICTION_SECONDS_1] = tuple->value->int32;
        s_displayable_items[s_displayable_items_count].times_count++;
        break;
      case KEY_PREDICTION_SHOW:
        s_displayable_items_count++;
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

    Layer *window_layer = window_get_root_layer(s_predictions_window);
    GRect bounds = layer_get_bounds(window_layer);

    s_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(
      grect_inset(bounds, GEdgeInsets(20, 0, 0, 0)),
      bounds));
    text_layer_set_text(s_text_layer, "Ready.");
    text_layer_set_text_alignment(s_text_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  }
  if (window_stack_get_top_window() != s_predictions_window) {
    window_stack_push(s_predictions_window, true);
  }
  if (mode == PRED_MODE_LOADING) {
    s_displayable_items_count = -1;
    info_show("LOADING PREDICTIONS...");
  } else if (mode == PRED_MODE_PREDICTIONS) {
    // ensure we got the layer initialized and loaded with data
    char* pos = s_predictions_full_text;
    for(int i = 0; i < s_displayable_items_count; i++) {
      APP_LOG(APP_LOG_LEVEL_INFO, "i = %d\n", i);
      APP_LOG(APP_LOG_LEVEL_INFO, "snprintf pos = %d len=%d",(int) pos, strlen(s_predictions_full_text));
      DisplayableItem item = s_displayable_items[i];
      int n = snprintf(pos, 4000, "%s\n", item.text);
      pos += n;
      APP_LOG(APP_LOG_LEVEL_INFO, "snprintf pos = %d len=%d",(int) pos, strlen(s_predictions_full_text));
      for (int j = 0; j < item.times_count; j++) {
        pos += snprintf(pos, 4000, "%ds /", item.times[j]);
        APP_LOG(APP_LOG_LEVEL_INFO, "snprintf pos = %d len=%d",(int) pos, strlen(s_predictions_full_text));
      }
      pos += snprintf(pos, 4000, "\n\n");
      APP_LOG(APP_LOG_LEVEL_INFO, "snprintf pos = %d len=%d",(int) pos, strlen(s_predictions_full_text));
      APP_LOG(APP_LOG_LEVEL_INFO, "i at the end = %d\n", i);
      APP_LOG(APP_LOG_LEVEL_INFO, "count now = %d\n", s_displayable_items_count);
      APP_LOG(APP_LOG_LEVEL_INFO, "text now = %s\n", item.text);
    }

    APP_LOG(APP_LOG_LEVEL_INFO, "snprintf full_text = %d\n", (int)s_predictions_full_text);

    APP_LOG(APP_LOG_LEVEL_INFO, "%s", s_predictions_full_text);

    text_layer_set_text(s_text_layer, s_predictions_full_text);

    info_hide();
  }
}

void predictions_window_disappear() {
  // free_sections_and_items_arrays();
  for(int i = 0; i < s_displayable_items_count; i++) {
    free(s_displayable_items[i].text);
  }
}

static char* strdup(const char* str)
{
  return strcpy(malloc(strlen(str)*sizeof(char) + 1),str);
}

void build_ui_and_show() {


}


