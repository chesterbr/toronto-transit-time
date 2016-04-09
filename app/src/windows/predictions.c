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
static ScrollLayer *s_scroll_layer;
static GRect s_bounds;

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
static void predictions_window_disappear();
static void update_predictions_layer_contents();
static void update_prediction_times(tm *tick_time, TimeUnits units_changed);

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
        tick_timer_service_subscribe(SECOND_UNIT, update_prediction_times);
        break;
    }
  }
}

void predictions_window_make_visible(int mode) {
  if (!s_predictions_window) {
    s_predictions_window = window_create();
    window_set_window_handlers(s_predictions_window, (WindowHandlers) {
      .disappear = predictions_window_disappear,
    });

    Layer *window_layer = window_get_root_layer(s_predictions_window);
    s_bounds = layer_get_bounds(window_layer);

    s_scroll_layer = scroll_layer_create(s_bounds);
    scroll_layer_set_click_config_onto_window(s_scroll_layer, s_predictions_window);
    layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

    s_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(
      grect_inset(s_bounds, GEdgeInsets(20, 0, 0, 0)),
      s_bounds));
    text_layer_set_text(s_text_layer, "Ready.");
    text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
    scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  }
  if (window_stack_get_top_window() != s_predictions_window) {
    window_stack_push(s_predictions_window, true);
  }
  if (mode == PRED_MODE_LOADING) {
    s_displayable_items_count = -1;
    info_show("LOADING PREDICTIONS...");
  } else if (mode == PRED_MODE_PREDICTIONS) {
    // ensure we got the layer initialized and loaded with data
    update_predictions_layer_contents();
    text_layer_set_text(s_text_layer, s_predictions_full_text);

    GSize content_size = text_layer_get_content_size(s_text_layer);
    content_size.h = 5000;
    text_layer_set_size(s_text_layer, content_size);
    scroll_layer_set_content_size(s_scroll_layer, GSize(s_bounds.size.w, content_size.h));

    info_hide();
  }
}

// Private

static void update_predictions_layer_contents() {
    char* pos = s_predictions_full_text;
  for(int i = 0; i < s_displayable_items_count; i++) {
    int n = snprintf(pos, 4000, "%s\n", s_displayable_items[i].text);
    pos += n;
    for (int j = 0; j < s_displayable_items[i].times_count; j++) {
      pos += snprintf(pos, 4000, "%ds /", s_displayable_items[i].times[j]);
      s_displayable_items[i].times[j] -= 1;
    }
    pos += snprintf(pos, 4000, "\n\n");
  }
}

static void predictions_window_disappear() {
  tick_timer_service_unsubscribe();
  for(int i = 0; i < s_displayable_items_count; i++) {
    free(s_displayable_items[i].text);
  }
}

static void update_prediction_times(tm *tick_time, TimeUnits units_changed) {
  update_predictions_layer_contents();
  layer_mark_dirty(window_get_root_layer(s_predictions_window));
}

static char* strdup(const char* str)
{
  return strcpy(malloc(strlen(str) * sizeof(char) + 1), str);
}
