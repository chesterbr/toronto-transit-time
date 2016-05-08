#include "predictions.h"
#include "modules/util.h"

const int PREDICTION_TEXT_SIZE = 15;
const int PREDICTION_BOX_MARGIN = 4;
const int PREDICTION_BOX_ROUND_CORNER_RADIUS = 12;
const int CONTENT_INDICATOR_HEIGHT = 8;

static TextLayer *s_main_text_layer;
static TextLayer *s_secondary_text_layer;
static TextLayer *s_first_prediction_text_layer;
static TextLayer *s_other_predictions_text_layer;
static ContentIndicator *s_indicator;
static Layer *s_indicator_up_layer;
static Layer *s_indicator_down_layer;
static Layer *s_predictions_layer;
static GRect s_bounds;
static GRect s_predictions_layer_bounds;
static GRect s_first_prediction_text_bounds;
static GRect s_other_predictions_text_bounds;

static char *s_stop_address;
static DisplayableItem *s_items;
static int s_items_count;
static int s_current_item;
static char *s_first_prediction_text;
static char *s_other_predictions_text;

static void update_current_display_item(void);
static void update_up_and_down_content_indicators(void);
static int format_time(char* var, DisplayableItem item, int index);
static void fill_background(Layer *layer, GContext *ctx);

void predictions_layer_init(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(window_layer);

  GRect main_text_bounds = GRect(
    s_bounds.origin.x, CONTENT_INDICATOR_HEIGHT,
    s_bounds.size.w, s_bounds.size.h - 2 * CONTENT_INDICATOR_HEIGHT
  );
  s_main_text_layer = text_layer_create(main_text_bounds);
  text_layer_set_text_alignment(s_main_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_main_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  GRect secondary_text_bounds = GRect(
    s_bounds.origin.x, s_bounds.size.h * 0.60,
    s_bounds.size.w, s_bounds.size.h
  );
  s_secondary_text_layer = text_layer_create(secondary_text_bounds);
  text_layer_set_text_alignment(s_secondary_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_secondary_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));

  s_first_prediction_text_bounds = GRect(
    s_bounds.origin.x + PREDICTION_BOX_MARGIN / 2, s_bounds.size.h * 0.40,
    s_bounds.size.w / 2 , s_bounds.size.h * 0.20
  );
  s_first_prediction_text_layer = text_layer_create(s_first_prediction_text_bounds);
  text_layer_set_text_alignment(s_first_prediction_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_first_prediction_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  s_other_predictions_text_bounds = GRect(
    s_bounds.size.w / 2, s_bounds.size.h * 0.40,
    s_bounds.size.w / 2, s_bounds.size.h * 0.20
  );
  s_other_predictions_text_layer = text_layer_create(s_other_predictions_text_bounds);
  text_layer_set_text_alignment(s_other_predictions_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_other_predictions_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));

  s_predictions_layer_bounds = GRect(
    s_bounds.origin.x, s_bounds.size.h * 0.40,
    s_bounds.size.w, s_bounds.size.h * 0.20
  );
  s_predictions_layer = layer_create(s_predictions_layer_bounds);
  layer_set_update_proc(s_predictions_layer, fill_background);

  s_indicator = content_indicator_create();
  s_indicator_up_layer = layer_create(GRect(s_bounds.origin.x, s_bounds.origin.y,
                                      s_bounds.size.w, CONTENT_INDICATOR_HEIGHT));
  s_indicator_down_layer = layer_create(GRect(0, s_bounds.size.h - CONTENT_INDICATOR_HEIGHT,
                                        s_bounds.size.w, CONTENT_INDICATOR_HEIGHT));

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

  layer_add_child(window_layer, text_layer_get_layer(s_main_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_secondary_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_other_predictions_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_first_prediction_text_layer));
  layer_add_child(window_layer, s_predictions_layer);
  layer_add_child(window_layer, s_indicator_up_layer);
  layer_add_child(window_layer, s_indicator_down_layer);

  text_layer_enable_screen_text_flow_and_paging(s_main_text_layer, 2);
  text_layer_enable_screen_text_flow_and_paging(s_secondary_text_layer, 2);

  // Non-UI init
  s_first_prediction_text = malloc(sizeof(char) * PREDICTION_TEXT_SIZE);
  s_other_predictions_text = malloc(sizeof(char) * PREDICTION_TEXT_SIZE);
}

void predictions_layer_destroy(void) {
  free(s_other_predictions_text);
  free(s_first_prediction_text);
  layer_destroy(s_indicator_down_layer);
  layer_destroy(s_indicator_up_layer);
  content_indicator_destroy(s_indicator);
  layer_destroy(s_predictions_layer);
  text_layer_destroy(s_other_predictions_text_layer);
  text_layer_destroy(s_first_prediction_text_layer);
  text_layer_destroy(s_secondary_text_layer);
  text_layer_destroy(s_main_text_layer);
}

static void fill_background(Layer *layer, GContext *ctx) {
  if (s_items == NULL || !s_items[s_current_item].is_prediction) {
    return;
  }

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 6);

  if (s_items[s_current_item].times_count == 0) {
    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(ctx, "No predictions", font, s_bounds,
      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  } else {
    graphics_draw_rect(ctx, GRect(
      s_predictions_layer_bounds.size.w / 2 - PREDICTION_BOX_MARGIN / 2, 0,
      PREDICTION_BOX_MARGIN, s_predictions_layer_bounds.size.h));
    graphics_draw_round_rect(ctx, GRect(
      PREDICTION_BOX_MARGIN,
      0,
      s_predictions_layer_bounds.size.w / 2 - PREDICTION_BOX_MARGIN,
      s_predictions_layer_bounds.size.h), PREDICTION_BOX_ROUND_CORNER_RADIUS);
    graphics_draw_round_rect(ctx, GRect(
      s_predictions_layer_bounds.size.w / 2 + PREDICTION_BOX_MARGIN,
      0,
      s_predictions_layer_bounds.size.w / 2 - 2 * PREDICTION_BOX_MARGIN,
      s_predictions_layer_bounds.size.h), PREDICTION_BOX_ROUND_CORNER_RADIUS);
  }

  graphics_draw_round_rect(ctx, GRect(
    PREDICTION_BOX_MARGIN,
    0,
    s_predictions_layer_bounds.size.w - 2 * PREDICTION_BOX_MARGIN,
    s_predictions_layer_bounds.size.h), PREDICTION_BOX_ROUND_CORNER_RADIUS);

}

void predictions_layer_update(char *stop_address, DisplayableItem *items, int count, bool reset_scroll) {
  s_stop_address = stop_address;
  s_items = items;
  s_items_count = count;

  if (reset_scroll) {
    s_current_item = 0;
  } else {
    s_current_item = min(s_current_item, count - 1);
  }
  update_current_display_item();
}

void predictions_layer_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_current_item < s_items_count - 1) {
    s_current_item++;
    update_current_display_item();
  }
}

void predictions_layer_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_current_item > 0) {
    s_current_item--;
    update_current_display_item();
  }
}

// Private

static void update_current_display_item(void) {
  DisplayableItem item = s_items[s_current_item];
  text_layer_set_text(s_main_text_layer, item.text);
  text_layer_set_text(s_secondary_text_layer, NULL);
  text_layer_set_text(s_first_prediction_text_layer, NULL);
  text_layer_set_text(s_other_predictions_text_layer, NULL);
  if (item.is_prediction) {
    text_layer_set_text(s_secondary_text_layer, s_stop_address);
    if (item.times_count > 0) {
      format_time(s_first_prediction_text, item, 0);
      int pos = format_time(s_other_predictions_text, item, 1);
      format_time(s_other_predictions_text + pos, item, 2);
      text_layer_set_text(s_first_prediction_text_layer, s_first_prediction_text);
      text_layer_set_text(s_other_predictions_text_layer, s_other_predictions_text);
    }
  }
  update_up_and_down_content_indicators();
}

static void update_up_and_down_content_indicators(void) {
  content_indicator_set_content_available(s_indicator,
    ContentIndicatorDirectionUp, s_current_item > 0);
  content_indicator_set_content_available(s_indicator,
    ContentIndicatorDirectionDown, s_current_item < s_items_count - 1);
}

static int format_time(char* var, DisplayableItem item, int index) {
  if (index >= item.times_count) {
    *var = '\0';
    return 0;
  }
  char* format;
  int value = item.times[index];
  if (value >= 60) {
    format = "%dmin\n";
    value = value / 60;
  } else if (value > 0) {
    format = "%ds\n";
  } else {
    format = "DUE\n";
  }
  return snprintf(var, PREDICTION_TEXT_SIZE, format, value);
}
