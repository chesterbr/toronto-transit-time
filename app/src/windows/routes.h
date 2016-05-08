#pragma once

#include <pebble.h>

typedef void (*RouteSelectedCallback)(int,int);

void routes_window_init(RouteSelectedCallback route_selected_callback);
void routes_window_deinit(void);

void routes_window_set_menu_selected_callback();
void routes_window_inbox_received(DictionaryIterator *iterator, void *context);
