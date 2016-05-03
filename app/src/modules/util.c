#include "util.h"
#include <pebble.h>

int min(int a, int b) {
  return a < b ? a : b;
}

char* strdup(const char* str)
{
  return strcpy(malloc(strlen(str) * sizeof(char) + 1), str);
}

