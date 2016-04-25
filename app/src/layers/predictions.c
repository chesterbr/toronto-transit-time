#include "predictions.h"

static TextLayer *s_text_layer;
static ScrollLayer *s_scroll_layer;
static ContentIndicator *s_indicator;
static Layer *s_indicator_up_layer;
static Layer *s_indicator_down_layer;
static GRect s_bounds;

static char s_predictions_full_text[4000];
static char* s_buffer_pos;
static int s_buffer_size;

static DisplayableItem *items[];
static int items_count;

static void update_contents(DisplayableItem items[], int count);
static void update_scroll_bounds(void);
static void text_reset(void);
static void text_append_number(int number);
static void text_append_str(char* text);
static void text_append(char* text, int number);

static char s_content[] = "Cupcake\n\nDonut\n\nEclair\n\nFroyo\n\nGingerbread\n\nHoneycomb\n\nIce Cream Sandwich\n\nJelly Bean\n\nKitKat\n\nLollipop\n\nMarshmallow\n\n";

void predictions_layer_init(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(window_layer);

  // s_scroll_layer = scroll_layer_create(s_bounds);
  // scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  // scroll_layer_set_shadow_hidden(s_scroll_layer, true);
  // layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  // s_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(
  //   grect_inset(s_bounds, GEdgeInsets(20, 0, 0, 0)),
  //   s_bounds));
  // text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  // scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  s_indicator = content_indicator_create();
  s_indicator_up_layer = layer_create(GRect(s_bounds.origin.x, s_bounds.origin.y,
                                      s_bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
  s_indicator_down_layer = layer_create(GRect(0, s_bounds.size.h - STATUS_BAR_LAYER_HEIGHT,
                                        s_bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
  layer_add_child(window_layer, s_indicator_up_layer);
  layer_add_child(window_layer, s_indicator_down_layer);
  const ContentIndicatorConfig up_config = (ContentIndicatorConfig) {
    .layer = s_indicator_up_layer,
    .times_out = false,
    .alignment = GAlignCenter,
    .colors = {
      .foreground = GColorBlack,
      .background = GColorWhite
    }
  };
  content_indicator_configure_direction(s_indicator, ContentIndicatorDirectionUp,
                                        &up_config);
  const ContentIndicatorConfig down_config = (ContentIndicatorConfig) {
    .layer = s_indicator_down_layer,
    .times_out = false,
    .alignment = GAlignCenter,
    .colors = {
      .foreground = GColorBlack,
      .background = GColorWhite
    }
  };
  content_indicator_configure_direction(s_indicator, ContentIndicatorDirectionDown,
                                        &down_config);

}

void predictions_layer_update(DisplayableItem *items, int count, bool reset_scroll) {
  update_contents(items, count);
  if (reset_scroll) {
    update_scroll_bounds();
    scroll_layer_set_content_offset(s_scroll_layer, GPointZero, false);
  }
}

// Private

static void update_contents(DisplayableItem *items, int count) {
  text_reset();
  for(int i = 0; i < count; i++) {
    if (i > 0) {
      text_append_str("\n_________\n\n");
    }
    text_append_str(items[i].text);
    text_append_str("\n");
    if (items[i].times_count == 0 && items[i].is_prediction) {
      text_append_str("\nNo predictions");
    }
    for (int j = 0; j < items[i].times_count; j++) {
      text_append_str(j == 0 ? "\n" : " / ");
      if (items[i].times[j] >= 60) {
        text_append_number(items[i].times[j] / 60);
        text_append_str("min");
      } else if (items[i].times[j] >= 0) {
        text_append_number(items[i].times[j]);
        text_append_str("s");
      } else {
        text_append_str("DUE");
      }
      items[i].times[j]--;
    }
  }
  text_layer_set_text(s_text_layer, s_predictions_full_text);
}

static void update_scroll_bounds(void) {
  int content_size = 5000;
  text_layer_set_size(s_text_layer, GSize(s_bounds.size.w, content_size));
  scroll_layer_set_content_size(s_scroll_layer, GSize(s_bounds.size.w, content_size));
}

static void text_reset(void) {
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

