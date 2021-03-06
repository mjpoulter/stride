#pragma once

#include <pebble.h>

#include "data.h"

void graphics_draw_outer_dots(GContext *ctx, GRect bounds);

void graphics_fill_outer_ring(GContext *ctx, int32_t current_steps,
                              int fill_thickness, GRect frame, GColor color);
/// Draw yellow tick
void graphics_fill_goal_line(GContext *ctx, int currentGoal,int line_length, int line_width,
                                 GRect frame, GColor color);

void graphics_draw_steps_value(GContext *ctx, GRect bounds, GColor color, GBitmap *bitmap);

void graphics_set_window(Window *window);

void graphics_draw_status_icons(GContext *ctx, GPoint cx,GBitmap* bitmapBaterry,GBitmap*bitmapBLE,unsigned int charge);

void graphics_draw_weather(GContext *ctx, GRect bounds, GColor color, GBitmap *bitmap);
