#pragma once

#include <pebble.h>

// #include "config.h"
#include "util.h"
#include "../windows/main_window.h"
#include <pebble-generic-weather/pebble-generic-weather.h>

#define LOAD_DATA_DELAY 0

typedef enum {
  FontSizeSmall = 0,
  FontSizeMedium,
  FontSizeLarge
} FontSize;

void data_init();
void data_deinit();
int data_get_current_steps();
void data_set_current_steps(int value);
void data_set_current_average(int value); 
int data_get_temp(bool celcius);
void data_reload_averages();
GFont data_get_font(FontSize size);
GBitmap* data_get_blue_shoe();
GBitmap* data_get_red_shoe();
GBitmap* data_get_green_shoe();
void data_update_steps_buffer();
char* data_get_current_steps_buffer();
GBitmap* data_get_BLE(bool isConnected) ;
GBitmap* data_get_Battery(bool isCharging);
void prv_inbox_received_handler(DictionaryIterator *iter, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context) ;
void storeRootWindow(Window* win);
void weather_init();
void weather_callback(GenericWeatherInfo *info, GenericWeatherStatus status);
void weather_update(struct tm *tick_time, TimeUnits changed);