#include "splash.h"

enum {
  SPLASH_TEXT_FINDING_LOCATION  = 0,
  SPLASH_TEXT_ERROR_LOCATION    = 1,
  SPLASH_TEXT_FINDING_STOPS     = 2,
  SPLASH_TEXT_ERROR_STOPS       = 3,
  SPLASH_TEXT_ERROR_PREDICTIONS = 4
};

const int KEY_SPLASH_WITH_TEXT = 400;

static Layer *s_splash_layer = NULL;
static TextLayer *s_splash_message_layer;
static BitmapLayer *s_splash_streetcar_layer;
static GBitmap *s_splash_streetcar_bitmap;

char *s_splash_message;

static GRect s_splash_bounds; // This will likely move

static void ensure_layer_initialized(void);
static void move_layer_to_top(void);
static void fill_background(Layer *layer, GContext *ctx);

void splash_show(char* message) {
  ensure_layer_initialized();
  text_layer_set_text(s_splash_message_layer, message);
  layer_set_hidden(s_splash_layer, false);
  move_layer_to_top();
}

void splash_hide(void) {
  layer_remove_from_parent(s_splash_layer);
  layer_set_hidden(s_splash_layer, true);
}

void splash_layer_inbox_received(DictionaryIterator *iterator, void *context) {
  Tuple *tuple = dict_find(iterator, KEY_SPLASH_WITH_TEXT);
  if (tuple == NULL) {
    return;
  }
  switch (tuple->value->int32) {
    case SPLASH_TEXT_FINDING_LOCATION:
      splash_show("FINDING WHERE YOU ARE...");
      break;
    case SPLASH_TEXT_ERROR_LOCATION:
      splash_show("CAN'T GET GPS POSITION \U0001F614");
      break;
    case SPLASH_TEXT_FINDING_STOPS:
      splash_show("LOADING NEARBY STOPS...");
      break;
    case SPLASH_TEXT_ERROR_STOPS:
      splash_show("CAN'T DOWNLOAD STOPS \U0001F614");
      break;
    case SPLASH_TEXT_ERROR_PREDICTIONS:
      splash_show("CAN'T DOWNLOAD TTC PREDICTIONS \U0001F614");
      break;
  }
}

/// Private

static void ensure_layer_initialized(void) {
  if (s_splash_layer) { return; }

  Layer *window_layer = window_get_root_layer(window_stack_get_top_window());
  s_splash_bounds = layer_get_bounds(window_layer);

  s_splash_layer = layer_create(s_splash_bounds);
  layer_set_update_proc(s_splash_layer, fill_background);

  s_splash_message_layer = text_layer_create(
    GRect(s_splash_bounds.origin.x, s_splash_bounds.size.h * 0.75,
          s_splash_bounds.size.w, s_splash_bounds.size.h)
  );
  text_layer_set_text_alignment(s_splash_message_layer, GTextAlignmentCenter);

  s_splash_streetcar_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STREETCAR);
  GSize image_size = gbitmap_get_bounds(s_splash_streetcar_bitmap).size;
  GPoint center = grect_center_point(&s_splash_bounds);
  GRect image_frame = GRect(center.x, center.y, image_size.w, image_size.h);
  image_frame.origin.x -= image_size.w / 2;
  image_frame.origin.y = s_splash_bounds.size.h * 0.1;
  s_splash_streetcar_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(s_splash_streetcar_layer, s_splash_streetcar_bitmap);

  layer_add_child(s_splash_layer, text_layer_get_layer(s_splash_message_layer));
  layer_add_child(s_splash_layer, bitmap_layer_get_layer(s_splash_streetcar_layer));
}

static void move_layer_to_top(void) {
  Layer *root_layer = window_get_root_layer(window_stack_get_top_window());
  layer_remove_from_parent(s_splash_layer);
  layer_add_child(root_layer, s_splash_layer);
}

static void fill_background(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, s_splash_bounds, 0, GCornerNone);
}


