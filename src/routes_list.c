#include "routes_list.h"
#include <pebble.h>
#include "string_buffer.h"

static void initialize_sections_array(int section_count);
static void initialize_current_session(int items_count);
static void save_current_section_title(char * title);
static void save_current_item_title(char* title);
static void save_current_item_subtitle(char* subtitle);
static void show_list();
static void free_sections_and_items_arrays();

enum {
  KEY_MENU_SECTION_COUNT       = 100,
  KEY_MENU_STRING_BUFFER_SIZE  = 101,
  KEY_MENU_SECTION_ITEMS_COUNT = 102,
  KEY_MENU_SECTION_TITLE       = 103,
  KEY_MENU_SHOW                = 104,
  KEY_MENU_ITEM_TITLE_1        = 111,
  KEY_MENU_ITEM_TITLE_2        = 112,
  KEY_MENU_ITEM_TITLE_3        = 113,
  KEY_MENU_ITEM_TITLE_4        = 114,
  KEY_MENU_ITEM_TITLE_5        = 115,
  KEY_MENU_ITEM_TITLE_6        = 116,
  KEY_MENU_ITEM_TITLE_7        = 117,
  KEY_MENU_ITEM_TITLE_8        = 118,
  KEY_MENU_ITEM_TITLE_9        = 119,
  KEY_MENU_ITEM_TITLE_10       = 120,
  KEY_MENU_ITEM_SUBTITLE_1     = 121,
  KEY_MENU_ITEM_SUBTITLE_2     = 122,
  KEY_MENU_ITEM_SUBTITLE_3     = 123,
  KEY_MENU_ITEM_SUBTITLE_4     = 124,
  KEY_MENU_ITEM_SUBTITLE_5     = 125,
  KEY_MENU_ITEM_SUBTITLE_6     = 126,
  KEY_MENU_ITEM_SUBTITLE_7     = 127,
  KEY_MENU_ITEM_SUBTITLE_8     = 128,
  KEY_MENU_ITEM_SUBTITLE_9     = 129,
  KEY_MENU_ITEM_SUBTITLE_10    = 130
};

static Window *s_routes_list_window;
static SimpleMenuLayer *s_simple_menu_layer;


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
}

void routes_list_deinit() {
  simple_menu_layer_destroy(s_simple_menu_layer);
  free_sections_and_items_arrays();
  string_buffer_deinit();
  window_destroy(s_routes_list_window);
}

void routes_list_inbox_received(DictionaryIterator *iterator, void *context) {
  // To ensure all data structures are initialized before usage and to keep
  // track of the current section / item, keys must be processed in order
  for(int key = KEY_MENU_SECTION_COUNT; key <= KEY_MENU_ITEM_SUBTITLE_10; key++) {
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
        initialize_current_session((int)tuple->value->int32);
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
        save_current_item_subtitle(tuple->value->cstring);
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

static void initialize_current_session(int items_count) {
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

static void save_current_item_subtitle(char* subtitle) {
  s_menu_current_section_items[s_menu_current_item_index] = (SimpleMenuItem) {
    .title = s_menu_current_item_title,
    .subtitle = string_buffer_store(subtitle)
  };
  s_menu_current_item_index++;
}

static void show_list() {
  Layer *window_layer = window_get_root_layer(s_routes_list_window);
  GRect bounds = layer_get_frame(window_layer);
  s_simple_menu_layer = simple_menu_layer_create(bounds, s_routes_list_window, s_menu_sections, s_menu_current_section_index + 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void free_sections_and_items_arrays() {
  for (int i = 0; i < s_menu_sections_count; i++) {
    free((void *)s_menu_sections[i].items);
  }
  free(s_menu_sections);
}
