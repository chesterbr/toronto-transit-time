#include "predictions.h"

static TextLayer *s_text_layer;
static ScrollLayer *s_scroll_layer;
static GRect s_bounds;

static char s_predictions_full_text[4000];
static char* s_buffer_pos;
static int s_buffer_size;

static void update_contents(DisplayableItem items[], int count);
static void update_scroll_bounds();
static void text_reset();
static void text_append_number(int number);
static void text_append_str(char* text);
static void text_append(char* text, int number);

void predictions_layer_init(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(window_layer);

  s_scroll_layer = scroll_layer_create(s_bounds);
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  s_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(
    grect_inset(s_bounds, GEdgeInsets(20, 0, 0, 0)),
    s_bounds));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));
}

void predictions_layer_update(DisplayableItem *items, int count) {
  update_contents(items, count);
  update_scroll_bounds();
}

// Private

static void update_contents(DisplayableItem *items, int count) {
  text_reset();
  for(int i = 0; i < count; i++) {
    text_append_str(items[i].text);
    text_append_str("\n");
    for (int j = 0; j < items[i].times_count; j++) {
      text_append_str(j == 0 ? "\n" : " / ");
      text_append_number(items[i].times[j]);
      items[i].times[j]--;
    }
    text_append_str("\n\n");
  }
  text_layer_set_text(s_text_layer, s_predictions_full_text);
}

static void update_scroll_bounds() {
  GSize content_size = text_layer_get_content_size(s_text_layer);
  content_size.h = 5000;
  text_layer_set_size(s_text_layer, content_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(s_bounds.size.w, content_size.h));
}

static void text_reset() {
  s_buffer_size = 4000;
  s_buffer_pos = s_predictions_full_text;
}

static void text_append_number(int number) {
  text_append(NULL, number);
}

static void text_append_str(char* text) {
  text_append(text, 0);
}

static void text_append(char* text, int number) {
  int appended;
  if (text != NULL) {
    appended = snprintf(s_buffer_pos, s_buffer_size, "%s", text);
  } else {
    appended = snprintf(s_buffer_pos, s_buffer_size, "%d", number);
  }
  s_buffer_size -= appended;
  s_buffer_pos += appended;
}

