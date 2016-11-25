#pragma once

#include <pebble.h>

#include "../modules/data.h"
#include "../modules/graphics.h"
#include "../modules/util.h"
#include "../modules/config.h"

void main_window_push();
void main_window_update_time(struct tm* tick_time);
void main_window_update_ble(bool isConnected);
void main_window_update_battery(bool isCharging,int charge);
void main_window_redraw();
