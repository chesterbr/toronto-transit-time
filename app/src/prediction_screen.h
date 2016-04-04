#pragma once

#include <pebble.h>

void prediction_screen_init();
void prediction_screen_deinit();
void prediction_screen_inbox_received(DictionaryIterator *iterator, void *context);
