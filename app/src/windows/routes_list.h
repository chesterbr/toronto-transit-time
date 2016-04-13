#pragma once

#include <pebble.h>

void routes_list_init(void);
void routes_list_deinit(void);

void routes_list_inbox_received(DictionaryIterator *iterator, void *context);
