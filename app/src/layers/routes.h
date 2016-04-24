#pragma once

#include <pebble.h>

typedef struct {
  const char *title;
  const char *subtitle;
} MenuItem;

typedef struct {
  const char *title;
  const MenuItem *items;
  uint32_t num_items;
} MenuSection;

void routes_layer_init(Window *window, MenuSection *sections, int sections_count, MenuLayerSelectCallback select_callback);
