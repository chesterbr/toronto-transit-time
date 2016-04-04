#include "predictions.h"
#include <pebble.h>
#include <stdio.h>

enum {
  // Inbound message keys
  KEY_PREDICTION_DIRECTION_COUNT = 200,
  KEY_PREDICTION_TTC_ALERT_COUNT = 201,
  KEY_PREDICTION_TITLE           = 202,
  KEY_PREDICTION_EPOCH_TIME_1    = 203,
  KEY_PREDICTION_EPOCH_TIME_2    = 204,
  KEY_PREDICTION_EPOCH_TIME_3    = 205,
  KEY_PREDICTION_EPOCH_TIME_4    = 206,
  KEY_PREDICTION_EPOCH_TIME_5    = 207,
  KEY_PREDICTION_TTC_ALERT       = 208,
  KEY_PREDICTION_SHOW            = 209,
};

static Window *s_prediction_screen_window;
static ScrollLayer *s_scroll_layer;
static TextLayer *s_text_layer;

static char **s_directions;
static char **s_ttc_alerts;

static int s_direction_index;

void prediction_screen_init() {

}

void prediction_screen_deinit() {
  // free_sections_and_items_arrays();
  free(s_directions);

  window_destroy(s_prediction_screen_window);
}

char* strdup(const char* str)
{
 return strcpy(malloc( strlen(str) + 1),str);
}

void build_ui_and_show() {
  s_prediction_screen_window = window_create();

  // Layer *window_layer = window_get_root_layer(s_prediction_screen_window);

  Layer *window_layer = window_get_root_layer(s_prediction_screen_window);
  GRect bounds = layer_get_bounds(window_layer);

  s_scroll_layer = scroll_layer_create(bounds);

  int content_height = 300;

  for(int i = 0; i < 2; i++) {
    GRect tl_bounds = bounds;
    tl_bounds.origin.y = i*20;
    TextLayer *text_layer = text_layer_create(tl_bounds);
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_text(text_layer, s_directions[i]);
    scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(text_layer));
  };

  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, content_height));
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  window_stack_push(s_prediction_screen_window, true);
}

void prediction_screen_inbox_received(DictionaryIterator *iterator, void *context) {
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
      case KEY_PREDICTION_EPOCH_TIME_1:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %s", s_direction_index, tuple->value->cstring);
        s_directions[++s_direction_index] = strdup(tuple->value->cstring);
        break;
      case KEY_PREDICTION_EPOCH_TIME_2:
      case KEY_PREDICTION_EPOCH_TIME_3:
      case KEY_PREDICTION_EPOCH_TIME_4:
      case KEY_PREDICTION_EPOCH_TIME_5:
      case KEY_PREDICTION_TTC_ALERT:
        break;
      case KEY_PREDICTION_SHOW:
        build_ui_and_show();
    }
  }


}

