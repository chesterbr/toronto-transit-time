#include "routes.h"
#include "predictions.h"
#include "../layers/splash.h"
#include "../layers/routes.h"
#include "../modules/string_buffer.h"
#include "../modules/bluetooth.h"
#include <pebble.h>

static void routes_window_load(Window *window);
static void routes_window_unload(Window *window);
static void initialize_sections_array(int section_count);
static void initialize_session_struct_and_items_array(int items_count);
static void save_current_section_title(char * title);
static void save_current_item_title(char* title);
static void build_menu_item_using_title_and_subtitle(char* subtitle);
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);
static void free_sections_and_items_arrays(void);

enum {
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
};

static Window *s_routes_list_window;

// Dynamically allocated menu data structures
static int s_menu_sections_count;
static MenuSection *s_menu_sections;

// Counters and pointers used during menu population
static int s_menu_current_section_index;
static int s_menu_current_item_index;
static char* s_menu_current_item_title;
static MenuItem *s_menu_current_section_items;

void routes_window_init(void) {
  s_routes_list_window = window_create();
  window_set_window_handlers(s_routes_list_window, (WindowHandlers) {
    .load = routes_window_load,
    .unload = routes_window_unload
  });
  window_stack_push(s_routes_list_window, true);
}

static void routes_window_load(Window *window) {
  // TODO remove this if we end up not using
}

static void routes_window_unload(Window *window) {
  free_sections_and_items_arrays();
  string_buffer_deinit();
  window_destroy(s_routes_list_window);
}

void routes_window_inbox_received(DictionaryIterator *iterator, void *context) {
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
        routes_layer_init(s_routes_list_window, s_menu_sections, s_menu_sections_count, &menu_select_callback);
        splash_hide();
        break;
    }
  }
}

// Private

static void initialize_sections_array(int section_count) {
  s_menu_sections_count = section_count;
  s_menu_sections = (MenuSection *)malloc(s_menu_sections_count * sizeof(MenuSection));
  s_menu_current_section_index = -1;
}

static void initialize_session_struct_and_items_array(int items_count) {
  s_menu_current_section_items = (MenuItem *)malloc(items_count * sizeof(MenuItem));
  s_menu_sections[++s_menu_current_section_index] = (MenuSection) {
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
  s_menu_current_section_items[s_menu_current_item_index] = (MenuItem) {
    .title = s_menu_current_item_title,
    .subtitle = string_buffer_store(subtitle)
  };
  s_menu_current_item_index++;
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  if (cell_index->section == s_menu_sections_count - 1) {
    return;
  }
  predictions_window_make_visible(PRED_MODE_LOADING);
  bluetooth_request_predictions(cell_index->section, cell_index->row);
}

static void free_sections_and_items_arrays(void) {
  for (int i = 0; i < s_menu_sections_count; i++) {
    free((void *)s_menu_sections[i].items);
  }
  free(s_menu_sections);
}
