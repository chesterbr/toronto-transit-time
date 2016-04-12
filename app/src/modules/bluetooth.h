#pragma once

#include <pebble.h>

void bluetooth_initialize(AppMessageInboxReceived received_callback);
void bluetooth_request_predictions(int section, int row);
void bluetooth_refresh_predictions();
