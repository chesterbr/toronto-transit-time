#pragma once

#include <pebble.h>

enum {
  PRED_MODE_LOADING,
  PRED_MODE_PREDICTIONS
};

void predictions_window_inbox_received(DictionaryIterator *iterator, void *context);
void predictions_window_make_visible(int mode);

