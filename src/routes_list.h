#include <pebble.h>

void routes_list_init();
void routes_list_deinit();

void routes_list_inbox_received(DictionaryIterator *iterator, void *context);
