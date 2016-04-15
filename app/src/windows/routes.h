#pragma once

#include <pebble.h>

void routes_window_init(void);
void routes_window_deinit(void);

void routes_window_inbox_received(DictionaryIterator *iterator, void *context);
