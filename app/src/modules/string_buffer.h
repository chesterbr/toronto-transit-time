#pragma once

char* string_buffer_init(int size_in_bytes);
void string_buffer_deinit(void);

char* string_buffer_store(char* s);
