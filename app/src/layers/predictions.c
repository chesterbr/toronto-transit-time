#include "predictions.h"

static TextLayer *s_main_text_layer;
static TextLayer *s_secondary_text_layer;
static TextLayer *s_first_prediction_text_layer;
static TextLayer *s_other_predictions_text_layer;
static ScrollLayer *s_scroll_layer;
static ContentIndicator *s_indicator;
static Layer *s_indicator_up_layer;
static Layer *s_indicator_down_layer;
static Layer *s_predictions_layer;
static GRect s_bounds;
static GRect s_predictions_layer_bounds;
static GRect s_first_prediction_text_bounds;
static GRect s_other_predictions_text_bounds;

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
static void fill_background(Layer *layer, GContext *ctx);

static char s_content[] = "Starting at 2am on March 29 until 5am on April 11 - 504 King streetcars will be on diversion due to work at King St W at Charlotte St. EB: Spadina, Queen and Church. WB: York, Queen and Spadina. Visit www.ttc.ca for details.";

void predictions_layer_init(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_bounds = layer_get_bounds(window_layer);

  // s_scroll_layer = scroll_layer_create(s_bounds);
  // scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
  // scroll_layer_set_shadow_hidden(s_scroll_layer, true);
  // layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

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

  // This is just a test


  content_indicator_set_content_available(s_indicator, ContentIndicatorDirectionUp,
                                          true);
  content_indicator_set_content_available(s_indicator, ContentIndicatorDirectionDown,
                                          true);

  text_layer_set_text(s_main_text_layer, "East - 306 Carlton Blue Night towards Main Street Station");
  text_layer_set_text(s_secondary_text_layer, "College St At Beverley St And Some Extra Stuff");
  text_layer_set_text(s_first_prediction_text_layer, "1min");
  text_layer_set_text(s_other_predictions_text_layer, "69min\n88min");
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
  text_layer_set_text(s_main_text_layer, s_predictions_full_text);
}

static void update_scroll_bounds(void) {
  int content_size = 5000;
  text_layer_set_size(s_main_text_layer, GSize(s_bounds.size.w, content_size));
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

