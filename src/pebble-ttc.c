#include <pebble.h>

static Window *s_main_window;
static SimpleMenuLayer *s_simple_menu_layer;

enum {
  KEY_MENU_SECTION_COUNT = 0,
  KEY_MENU_STRING_BUFFER_SIZE = 1,
  KEY_MENU_SECTION_ITEMS_COUNT = 2,
  KEY_MENU_SECTION_TITLE = 3,
  KEY_MENU_SHOW = 4,
  KEY_MENU_ITEM_TITLE_1 = 11,
  KEY_MENU_ITEM_TITLE_2 = 12,
  KEY_MENU_ITEM_TITLE_3 = 13,
  KEY_MENU_ITEM_TITLE_4 = 14,
  KEY_MENU_ITEM_TITLE_5 = 15,
  KEY_MENU_ITEM_TITLE_6 = 16,
  KEY_MENU_ITEM_TITLE_7 = 17,
  KEY_MENU_ITEM_TITLE_8 = 18,
  KEY_MENU_ITEM_TITLE_9 = 19,
  KEY_MENU_ITEM_TITLE_10 = 20,
  KEY_MENU_ITEM_SUBTITLE_1 = 21,
  KEY_MENU_ITEM_SUBTITLE_2 = 22,
  KEY_MENU_ITEM_SUBTITLE_3 = 23,
  KEY_MENU_ITEM_SUBTITLE_4 = 24,
  KEY_MENU_ITEM_SUBTITLE_5 = 25,
  KEY_MENU_ITEM_SUBTITLE_6 = 26,
  KEY_MENU_ITEM_SUBTITLE_7 = 27,
  KEY_MENU_ITEM_SUBTITLE_8 = 28,
  KEY_MENU_ITEM_SUBTITLE_9 = 29,
  KEY_MENU_ITEM_SUBTITLE_10 = 30
};

int MIN_KEY = 0;
int MAX_KEY = 30;

// Dynamically allocated menu data structures
static int s_menu_sections_count;
static SimpleMenuSection *s_menu_sections;
static char *s_menu_string_buffer;

// Counters and pointers used during menu population
static char *s_menu_string_buffer_pos;
static int s_menu_current_section_index;
static int s_menu_current_item_index;
static char* s_menu_current_item_title;
static SimpleMenuItem *s_menu_current_section_items;

static void main_window_load(Window *window) {

}

static void main_window_unload(Window *window) {

}


static char* save_string_in_buffer(char* s) {
  char* stored_string = strcpy(s_menu_string_buffer_pos, s);
  s_menu_string_buffer_pos += strlen(s) + 1;

  return stored_string;
}

// Communication with phone

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  for(int key = MIN_KEY; key <= MAX_KEY; key++) {
    Tuple *tuple = dict_find(iterator, key);
    if (tuple == NULL) {
      continue;
    }
    switch (tuple->key) {
      case KEY_MENU_SECTION_COUNT:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Building Menu");
        s_menu_sections_count = (int)tuple->value->int32;
        s_menu_sections = (SimpleMenuSection *)malloc(s_menu_sections_count * sizeof(SimpleMenuSection));
        s_menu_current_section_index = -1;
        break;
      case KEY_MENU_STRING_BUFFER_SIZE:
        s_menu_string_buffer = (char *)malloc((int)tuple->value->int32 * sizeof(char));
        s_menu_string_buffer_pos = s_menu_string_buffer;
        break;
      case KEY_MENU_SECTION_ITEMS_COUNT:
        s_menu_current_section_items = (SimpleMenuItem *)malloc((int)tuple->value->int32 * sizeof(SimpleMenuItem));
        s_menu_sections[++s_menu_current_section_index] = (SimpleMenuSection) {
          .num_items = (int)tuple->value->int32,
          .items = s_menu_current_section_items,
        };
        s_menu_current_item_index = 0;
        break;
      case KEY_MENU_SECTION_TITLE:
        s_menu_sections[s_menu_current_section_index].title = save_string_in_buffer(tuple->value->cstring);
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
        s_menu_current_item_title = save_string_in_buffer(tuple->value->cstring);
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
        s_menu_current_section_items[s_menu_current_item_index] = (SimpleMenuItem) {
          .title = s_menu_current_item_title,
          .subtitle = save_string_in_buffer(tuple->value->cstring)
        };
        s_menu_current_item_index++;
        break;
      case KEY_MENU_SHOW:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Showing Menu");
        Layer *window_layer = window_get_root_layer(s_main_window);
        GRect bounds = layer_get_frame(window_layer);
        s_simple_menu_layer = simple_menu_layer_create(bounds, s_main_window, s_menu_sections, s_menu_current_section_index + 1, NULL);
        layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
        break;
    }
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void menu_destroy() {
  simple_menu_layer_destroy(s_simple_menu_layer);
  for (int i = 0; i < s_menu_sections_count; i++) {
    free((void *)s_menu_sections[i].items);
  }
  free(s_menu_sections);
  free(s_menu_string_buffer);

}

void initialize_communication_with_phone() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  const int inbox_size = 2048;
  const int outbox_size = 2048;
  app_message_open(inbox_size, outbox_size);
}


void init() {
  initialize_communication_with_phone();

  s_main_window = window_create();
  // window_set_window_handlers(s_main_window, (WindowHandlers) {
  //   .load = main_window_load,
  //   .unload = main_window_unload,
  // });
  window_stack_push(s_main_window, true);
}

void deinit() {
  menu_destroy();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();

  return 0;
}
