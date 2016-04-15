#pragma once

#include <pebble.h>

void splash_show(char* message);
void splash_hide(void);

void splash_layer_inbox_received(DictionaryIterator *iterator, void *context);
