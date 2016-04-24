#include "routes.h"

static MenuLayer *s_menu_layer;
static StatusBarLayer *s_status_bar_layer;

static int s_menu_sections_count;
static MenuSection *s_menu_sections;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data);
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);

void routes_layer_init(Window *window, MenuSection *sections, int sections_count, MenuLayerSelectCallback select_callback) {
  s_menu_sections = sections;
  s_menu_sections_count = sections_count;
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = select_callback,
  });

  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  s_status_bar_layer = status_bar_layer_create();

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar_layer));
}

// Menu layer callbacks

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return s_menu_sections_count;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return s_menu_sections[section_index].num_items;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 16; //MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, s_menu_sections[section_index].title);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  const char* title = s_menu_sections[cell_index->section].items[cell_index->row].title;
  const char* subtitle = s_menu_sections[cell_index->section].items[cell_index->row].subtitle;
  menu_cell_basic_draw(ctx, cell_layer, title, subtitle, NULL);
}

