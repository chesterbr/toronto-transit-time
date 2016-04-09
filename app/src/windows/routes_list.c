#include "routes_list.h"
#include "predictions.h"
#include "../layers/info.h"
#include "../modules/string_buffer.h"
#include <pebble.h>

static void initialize_menu_layer();
static void initialize_sections_array(int section_count);
static void initialize_session_struct_and_items_array(int items_count);
static void save_current_section_title(char * title);
static void save_current_item_title(char* title);
static void build_menu_item_using_title_and_subtitle(char* subtitle);
static void show_list();
static void menu_select_callback();
static void free_sections_and_items_arrays();

enum {
  // Inbound message keys
  KEY_MENU_SECTION_COUNT       = 100,
  KEY_MENU_STRING_BUFFER_SIZE  = 101,
  KEY_MENU_SECTION_ITEMS_COUNT = 102,
  KEY_MENU_SECTION_TITLE       = 103,
  KEY_MENU_ITEM_TITLE_1        = 104,
  KEY_MENU_ITEM_SUBTITLE_1     = 105,
  KEY_MENU_ITEM_TITLE_2        = 106,
  KEY_MENU_ITEM_SUBTITLE_2     = 107,
  KEY_MENU_ITEM_TITLE_3        = 108,
  KEY_MENU_ITEM_SUBTITLE_3     = 109,
  KEY_MENU_ITEM_TITLE_4        = 110,
  KEY_MENU_ITEM_SUBTITLE_4     = 111,
  KEY_MENU_ITEM_TITLE_5        = 112,
  KEY_MENU_ITEM_SUBTITLE_5     = 113,
  KEY_MENU_ITEM_TITLE_6        = 114,
  KEY_MENU_ITEM_SUBTITLE_6     = 115,
  KEY_MENU_ITEM_TITLE_7        = 116,
  KEY_MENU_ITEM_SUBTITLE_7     = 117,
  KEY_MENU_ITEM_TITLE_8        = 118,
  KEY_MENU_ITEM_SUBTITLE_8     = 119,
  KEY_MENU_ITEM_TITLE_9        = 120,
  KEY_MENU_ITEM_SUBTITLE_9     = 121,
  KEY_MENU_ITEM_TITLE_10       = 122,
  KEY_MENU_ITEM_SUBTITLE_10    = 123,
  KEY_MENU_SHOW                = 124,

  // Outbound message keys
  KEY_MENU_SELECTED_SECTION    = 125,
  KEY_MENU_SELECTED_ITEM       = 126,
};

static Window *s_routes_list_window;
static MenuLayer *s_menu_layer;
static StatusBarLayer *s_status_bar_layer;

// TODO we're still using the simplemenulayer's data structures. It was a quick hack
// to make it work, but we should convert to our own structures.

// Dynamically allocated menu data structures
static int s_menu_sections_count;
static SimpleMenuSection *s_menu_sections;

// Counters and pointers used during menu population
static int s_menu_current_section_index;
static int s_menu_current_item_index;
static char* s_menu_current_item_title;
static SimpleMenuItem *s_menu_current_section_items;

void routes_list_init() {
  s_routes_list_window = window_create();
  window_stack_push(s_routes_list_window, true);

  initialize_menu_layer();

  info_show("LOADING...");
}

void routes_list_deinit() {
  menu_layer_destroy(s_menu_layer);
  free_sections_and_items_arrays();
  string_buffer_deinit();
  window_destroy(s_routes_list_window);
}

void routes_list_inbox_received(DictionaryIterator *iterator, void *context) {
  // To ensure all data structures are initialized before usage and to keep
  // track of the current section / item, keys must be processed in order
  for(int key = KEY_MENU_SECTION_COUNT; key <= KEY_MENU_SHOW; key++) {
    Tuple *tuple = dict_find(iterator, key);
    if (tuple == NULL) {
      continue;
    }
    switch (tuple->key) {
      case KEY_MENU_SECTION_COUNT:
        initialize_sections_array((int)tuple->value->int32);
        break;
      case KEY_MENU_STRING_BUFFER_SIZE:
        string_buffer_init((int)tuple->value->int32);
        break;
      case KEY_MENU_SECTION_ITEMS_COUNT:
        initialize_session_struct_and_items_array((int)tuple->value->int32);
        break;
      case KEY_MENU_SECTION_TITLE:
        save_current_section_title(tuple->value->cstring);
        break;
      case KEY_MENU_ITEM_TITLE_1:
      case KEY_MENU_ITEM_TITLE_2:
      case KEY_MENU_ITEM_TITLE_3:
      case KEY_MENU_ITEM_TITLE_4:
      case KEY_MENU_ITEM_TITLE_5:
      case KEY_MENU_ITEM_TITLE_6:
      case KEY_MENU_ITEM_TITLE_7:
      case KEY_MENU_ITEM_TITLE_8:
      case KEY_MENU_ITEM_TITLE_9:
      case KEY_MENU_ITEM_TITLE_10:
        save_current_item_title(tuple->value->cstring);
        break;
      case KEY_MENU_ITEM_SUBTITLE_1:
      case KEY_MENU_ITEM_SUBTITLE_2:
      case KEY_MENU_ITEM_SUBTITLE_3:
      case KEY_MENU_ITEM_SUBTITLE_4:
      case KEY_MENU_ITEM_SUBTITLE_5:
      case KEY_MENU_ITEM_SUBTITLE_6:
      case KEY_MENU_ITEM_SUBTITLE_7:
      case KEY_MENU_ITEM_SUBTITLE_8:
      case KEY_MENU_ITEM_SUBTITLE_9:
      case KEY_MENU_ITEM_SUBTITLE_10:
        build_menu_item_using_title_and_subtitle(tuple->value->cstring);
        break;
      case KEY_MENU_SHOW:
        show_list();
        break;
    }
  }
}

// Private

static void initialize_sections_array(int section_count) {
  s_menu_sections_count = section_count;
  s_menu_sections = (SimpleMenuSection *)malloc(s_menu_sections_count * sizeof(SimpleMenuSection));
  s_menu_current_section_index = -1;
}

static void initialize_session_struct_and_items_array(int items_count) {
  s_menu_current_section_items = (SimpleMenuItem *)malloc(items_count * sizeof(SimpleMenuItem));
  s_menu_sections[++s_menu_current_section_index] = (SimpleMenuSection) {
    .num_items = items_count,
    .items = s_menu_current_section_items,
  };
  s_menu_current_item_index = 0;
}

static void save_current_section_title(char * title) {
  s_menu_sections[s_menu_current_section_index].title = string_buffer_store(title);
}

static void save_current_item_title(char* title) {
  s_menu_current_item_title = string_buffer_store(title);
}

static void build_menu_item_using_title_and_subtitle(char* subtitle) {
  s_menu_current_section_items[s_menu_current_item_index] = (SimpleMenuItem) {
    .title = s_menu_current_item_title,
    .subtitle = string_buffer_store(subtitle),
    .callback = menu_select_callback
  };
  s_menu_current_item_index++;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return s_menu_sections_count;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return s_menu_sections[section_index].num_items;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 16; //MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, s_menu_sections[section_index].title);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  menu_cell_basic_draw(ctx, cell_layer, s_menu_sections[cell_index->section].items[cell_index->row].title,
     s_menu_sections[cell_index->section].items[cell_index->row].subtitle,
    NULL);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
   // s_first_menu_items[index].subtitle = "You've hit select here!";
  // layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
  predictions_window_make_visible(PRED_MODE_LOADING);
  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if (result == APP_MSG_OK) {
    dict_write_int(out_iter, KEY_MENU_SELECTED_SECTION, &cell_index->section, sizeof(cell_index->section), false);
    dict_write_int(out_iter, KEY_MENU_SELECTED_ITEM, &cell_index->row, sizeof(cell_index->row), false);
    result = app_message_outbox_send();
    if (result != APP_MSG_OK) {
      info_show("ERROR SENDING DATA TO PHONE");
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    info_show("ERROR TALKING TO PHONE");
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void show_list() {
  Layer *window_layer = window_get_root_layer(s_routes_list_window);
  GRect bounds = layer_get_frame(window_layer);
  bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  menu_layer_set_click_config_onto_window(s_menu_layer, s_routes_list_window);

  s_status_bar_layer = status_bar_layer_create();

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar_layer));
}


static void initialize_menu_layer() {

}

static void free_sections_and_items_arrays() {
  for (int i = 0; i < s_menu_sections_count; i++) {
    free((void *)s_menu_sections[i].items);
  }
  free(s_menu_sections);
}
