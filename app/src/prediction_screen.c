#include "prediction_screen.h"
#include <pebble.h>

enum {
  // Inbound message keys
  KEY_PREDICTION_TITLE        = 200,
  KEY_PREDICTION_EPOCH_TIME_1 = 201,
  KEY_PREDICTION_EPOCH_TIME_2 = 202,
  KEY_PREDICTION_EPOCH_TIME_3 = 203,
  KEY_PREDICTION_EPOCH_TIME_4 = 204,
  KEY_PREDICTION_EPOCH_TIME_5 = 205,
  KEY_PREDICTION_MESSAGES     = 206,
  KEY_PREDICTION_SHOW         = 207,
};

void prediction_screen_inbox_received(DictionaryIterator *iterator, void *context) {
  for(int key = KEY_PREDICTION_TITLE; key <= KEY_PREDICTION_SHOW; key++) {
    Tuple *tuple = dict_find(iterator, key);
    if (tuple == NULL) {
      continue;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "message %d with decimal value %d", (int)tuple->key, (int)tuple->value->int32);

    // switch (tuple->key) {
    //   case KEY_MENU_SECTION_COUNT:
    //     initialize_sections_array((int)tuple->value->int32);
    //     break;
    //   case KEY_MENU_STRING_BUFFER_SIZE:
    //     string_buffer_init((int)tuple->value->int32);
    //     break;
    //   case KEY_MENU_SECTION_ITEMS_COUNT:
    //     initialize_session_struct_and_items_array((int)tuple->value->int32);
    //     break;
    //   case KEY_MENU_SECTION_TITLE:
    //     save_current_section_title(tuple->value->cstring);
    //     break;
    //   case KEY_MENU_ITEM_TITLE_1:
    //   case KEY_MENU_ITEM_TITLE_2:
    //   case KEY_MENU_ITEM_TITLE_3:
    //   case KEY_MENU_ITEM_TITLE_4:
    //   case KEY_MENU_ITEM_TITLE_5:
    //   case KEY_MENU_ITEM_TITLE_6:
    //   case KEY_MENU_ITEM_TITLE_7:
    //   case KEY_MENU_ITEM_TITLE_8:
    //   case KEY_MENU_ITEM_TITLE_9:
    //   case KEY_MENU_ITEM_TITLE_10:
    //     save_current_item_title(tuple->value->cstring);
    //     break;
    //   case KEY_MENU_ITEM_SUBTITLE_1:
    //   case KEY_MENU_ITEM_SUBTITLE_2:
    //   case KEY_MENU_ITEM_SUBTITLE_3:
    //   case KEY_MENU_ITEM_SUBTITLE_4:
    //   case KEY_MENU_ITEM_SUBTITLE_5:
    //   case KEY_MENU_ITEM_SUBTITLE_6:
    //   case KEY_MENU_ITEM_SUBTITLE_7:
    //   case KEY_MENU_ITEM_SUBTITLE_8:
    //   case KEY_MENU_ITEM_SUBTITLE_9:
    //   case KEY_MENU_ITEM_SUBTITLE_10:
    //     build_menu_item_using_title_and_subtitle(tuple->value->cstring);
    //     break;
    //   case KEY_MENU_SHOW:
    //     show_list();
    //     break;
    // }
  }


}

