#pragma once

#include <pebble.h>

typedef struct {
  char* text;
  bool is_prediction;
  int times_count;
  int times[5];
} DisplayableItem;

void predictions_layer_init(Window *window);
void predictions_layer_update(DisplayableItem *items, int count, bool reset_scroll);
