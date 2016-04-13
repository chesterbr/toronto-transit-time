#include "string_buffer.h"
#include <pebble.h>

static char *s_menu_string_buffer;
static char *s_menu_string_buffer_pos;

char* string_buffer_init(int size) {
  return (
    s_menu_string_buffer =
      s_menu_string_buffer_pos =
        (char *)malloc(size * sizeof(char))
  );
}

void string_buffer_deinit(void) {
  free(s_menu_string_buffer);
}

char* string_buffer_store(char* s) {
  char* stored_string = strcpy(s_menu_string_buffer_pos, s);
  s_menu_string_buffer_pos += strlen(s) + 1;

  return stored_string;
}

