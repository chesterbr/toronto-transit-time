#include "predictions.h"

const int PREDICTION_TEXT_SIZE = 15;

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

static DisplayableItem *s_items;
static int s_items_count;
static int s_current_item;
static char *s_first_prediction_text;
static char *s_other_predictions_text;

static void update_current_display_item();
static int format_time(char* var, DisplayableItem item, int index);
static void fill_background(Layer *layer, GContext *ctx);

void predictions_layer_init(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(window_layer);

  GRect main_text_bounds = GRect(
    s_bounds.origin.x, STATUS_BAR_LAYER_HEIGHT,
    s_bounds.size.w, s_bounds.size.h - 2 * STATUS_BAR_LAYER_HEIGHT
  );
  s_main_text_layer = text_layer_create(main_text_bounds);
  text_layer_set_text_alignment(s_main_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_main_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));

  GRect secondary_text_bounds = GRect(
    s_bounds.origin.x, s_bounds.size.h * 0.65,
    s_bounds.size.w, s_bounds.size.h - STATUS_BAR_LAYER_HEIGHT
  );
  s_secondary_text_layer = text_layer_create(secondary_text_bounds);
  text_layer_set_text_alignment(s_secondary_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_secondary_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));

  s_first_prediction_text_bounds = GRect(
    s_bounds.origin.x, s_bounds.size.h * 0.40,
    s_bounds.size.w / 2 , s_bounds.size.h * 0.20
  );
  s_first_prediction_text_layer = text_layer_create(s_first_prediction_text_bounds);
  text_layer_set_text_alignment(s_first_prediction_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_first_prediction_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  s_other_predictions_text_bounds = GRect(
    s_bounds.size.w / 2 , s_bounds.size.h * 0.40,
    s_bounds.size.w / 2 , s_bounds.size.h * 0.20
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
                                      s_bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
  s_indicator_down_layer = layer_create(GRect(0, s_bounds.size.h - STATUS_BAR_LAYER_HEIGHT,
                                        s_bounds.size.w, STATUS_BAR_LAYER_HEIGHT));

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

static void fill_background(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "draw");

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 6);

  graphics_draw_round_rect(ctx, GRect(
    0,
    0,
    s_predictions_layer_bounds.size.w / 2,
    s_predictions_layer_bounds.size.h), 12);
  graphics_draw_round_rect(ctx, GRect(
    s_predictions_layer_bounds.size.w / 2,
    0,
    s_predictions_layer_bounds.size.w / 2,
    s_predictions_layer_bounds.size.h), 12);
  graphics_draw_round_rect(ctx, GRect(
    0,
    0,
    s_predictions_layer_bounds.size.w,
    s_predictions_layer_bounds.size.h), 12);
}

void predictions_layer_update(DisplayableItem *items, int count, bool reset_scroll) {
  s_items = items;
  s_items_count = count;

  if (reset_scroll) {
    s_current_item = 0;
  }

  update_current_display_item();
}

// Private

static void update_current_display_item() {
  DisplayableItem item = s_items[s_current_item];
  text_layer_set_text(s_main_text_layer, item.text);
  if (item.is_prediction) {
    format_time(s_first_prediction_text, item, 0);
    int pos = format_time(s_other_predictions_text, item, 1);
    format_time(s_other_predictions_text + pos, item, 2);
    text_layer_set_text(s_secondary_text_layer, "TODO put stop address");
    text_layer_set_text(s_first_prediction_text_layer, s_first_prediction_text);
    text_layer_set_text(s_other_predictions_text_layer, s_other_predictions_text);
  } else {
    text_layer_set_text(s_secondary_text_layer, NULL);
    text_layer_set_text(s_first_prediction_text_layer, NULL);
    text_layer_set_text(s_other_predictions_text_layer, NULL);
  }
}

static int format_time(char* var, DisplayableItem item, int index) {
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
