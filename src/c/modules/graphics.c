#include "graphics.h"
#include "config.h"

#if defined(PBL_PLATFORM_APLITE) || defined(PBL_PLATFORM_BASALT) || defined(PBL_PLATFORM_CHALK) || defined(PBL_PLATFORM_DIORITE)
#define TOP_RIGHT 72
#define BOT_RIGHT 240
#define BOT_LEFT 384
#define TOP_LEFT 552
#else
#define TOP_RIGHT 72
#define BOT_RIGHT 240
#define BOT_LEFT 384
#define TOP_LEFT 552
#endif

#define MULT_X(a, b) (1000 * a / b)
#define DIV_X(a) (a / 1000)
#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)

static Window *s_window;

#if defined(PBL_RECT)
static int get_rect_perimeter()
{
  const GRect window_bounds = layer_get_bounds(window_get_root_layer(s_window));
  return (window_bounds.size.w + window_bounds.size.h) * 2;
}
#endif

/*
 * The progress is drawn according to progress through the following perimeter zones
 *
 * e       a       b
 *   -------------
 *   |           |
 *   |           |
 *   |           |
 *   |           |
 *   |           |
 *   |           |
 *   |           |
 *   -------------
 * d               c */
static GPoint steps_to_point(int current_steps, int day_average_steps, GRect frame)
{
#if defined(PBL_RECT)
  const int rect_perimeter = get_rect_perimeter();

  // Limits calculated from length along perimeter starting from 'a'
  const int limit_b = day_average_steps * TOP_RIGHT / rect_perimeter;
  const int limit_c = day_average_steps * BOT_RIGHT / rect_perimeter;
  const int limit_d = day_average_steps * BOT_LEFT / rect_perimeter;
  const int limit_e = day_average_steps * TOP_LEFT / rect_perimeter;

  if (current_steps <= limit_b)
  {
    // We are in between zone a <-> b
    return GPoint(frame.origin.x + DIV_X(frame.size.w * (500 + (500 * current_steps / limit_b))),
                  frame.origin.y);
  }
  else if (current_steps <= limit_c)
  {
    // We are in between zone b <-> c
    return GPoint(frame.origin.x + frame.size.w,
                  frame.origin.y + DIV_X(frame.size.h * MULT_X((current_steps - limit_b), (limit_c - limit_b))));
  }
  else if (current_steps <= limit_d)
  {
    // We are in between zone c <-> d
    return GPoint(frame.origin.x + DIV_X(frame.size.w * (1000 - MULT_X((current_steps - limit_c), (limit_d - limit_c)))),
                  frame.origin.y + frame.size.h);
  }
  else if (current_steps <= limit_e)
  {
    // We are in between zone d <-> e
    return GPoint(frame.origin.x,
                  frame.origin.y + DIV_X(frame.size.h * (1000 - MULT_X((current_steps - limit_d), (limit_e - limit_d)))));
  }
  else
  {
    // We are in between zone e <-> 0
    return GPoint(frame.origin.x + DIV_X(frame.size.w / 2 * MULT_X((current_steps - limit_e), (day_average_steps - limit_e))),
                  frame.origin.y);
  }
#elif defined(PBL_ROUND)
  // Simply a calculated point on the circumference
  const int angle = DIV_X(360 *
                          MULT_X(current_steps, day_average_steps));
  return gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(angle));
#endif
}

#if defined(PBL_RECT)
static GPoint inset_point(GPoint outer_point, int inset_amount)
{
  const GSize display_size = layer_get_bounds(window_get_root_layer(s_window)).size;
  // Insets the given point by the specified amount
  return (GPoint){
      .x = MAX(inset_amount - 1, MIN(outer_point.x, display_size.w - inset_amount)),
      .y = MAX(inset_amount - 1, MIN(outer_point.y, display_size.h - inset_amount))};
}
#endif

void graphics_draw_outer_dots(GContext *ctx, GRect bounds)
{
  const GRect inset_bounds = grect_inset(bounds, GEdgeInsets(6));

#if defined(PBL_RECT)
  const int rect_perimeter = get_rect_perimeter();
  const uint16_t quarter_perimeter = rect_perimeter / 4;
  const int dot_radius = 2;

  for (int i = 0; i <= rect_perimeter; i += quarter_perimeter)
  {
    // Put middle dots on each side of screen
    GPoint middle = steps_to_point(i, rect_perimeter, inset_bounds);
    //graphics_context_set_fill_color(ctx, GColorDarkGray);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, middle, dot_radius);

    // Puts two dots between each middle dot
    const int range = 36;
    for (int j = -range; j <= range; j += 2 * range)
    {
      GPoint sides = middle;
      if (i == quarter_perimeter || i == (3 * quarter_perimeter))
      {
        sides.y = middle.y + j;
      }
      else
      {
        sides.x = middle.x + j;
      }
      graphics_fill_circle(ctx, sides, dot_radius);
    }
  }
#elif defined(PBL_ROUND)
  // Outer dots placed along inside circumference
  const int num_dots = 12;
  for (int i = 0; i < num_dots; i++)
  {
    GPoint pos = gpoint_from_polar(inset_bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i * 360 / num_dots));

    const int dot_radius = 2;
    graphics_context_set_fill_color(ctx, GColorDarkGray);
    graphics_fill_circle(ctx, pos, dot_radius);
  }
#endif
}

void graphics_fill_outer_ring(GContext *ctx, int32_t current_steps,
                              int fill_thickness, GRect frame, GColor color)
{
  graphics_context_set_fill_color(ctx, color);
#if defined(PBL_PLATFORM_APLITE) || defined(PBL_PLATFORM_DIORITE)
  /// Override color
  color = GColorWhite;
#endif                                          // PBL_
  const int daily_goal = config_get_daily_goal(); // data_get_daily_average();
  if (daily_goal == 0)
  {
    // Do not draw
    return;
  }

#if defined(PBL_RECT)
  const GRect outer_bounds = frame;

  const GPoint start_outer_point = steps_to_point(0, daily_goal, outer_bounds);
  const GPoint start_inner_point = inset_point(start_outer_point, fill_thickness);
  const GPoint end_outer_point = steps_to_point(current_steps, daily_goal, outer_bounds);
  const GPoint end_inner_point = inset_point(end_outer_point, fill_thickness);

  GPath path = (GPath){
      .points = (GPoint *)malloc(sizeof(GPoint) * 20),
      .num_points = 0};

  const int rect_perimeter = get_rect_perimeter();
  const int32_t corners[6] = {0,
                              daily_goal * TOP_RIGHT / rect_perimeter,
                              daily_goal * BOT_RIGHT / rect_perimeter,
                              daily_goal * BOT_LEFT / rect_perimeter,
                              daily_goal * TOP_LEFT / rect_perimeter,
                              daily_goal};

  // Start the path with start_outer_point
  path.points[path.num_points++] = start_outer_point;

  // Loop through and add all the corners between start and end
  for (uint16_t i = 0; i < ARRAY_LENGTH(corners); i++)
  {
    if (corners[i] > 0 && corners[i] < current_steps)
    {
      path.points[path.num_points++] = steps_to_point(corners[i], daily_goal,
                                                      outer_bounds);
    }
  }

  path.points[path.num_points++] = end_outer_point;
  path.points[path.num_points++] = end_inner_point;

  // Loop though backwards and add all the corners between end and start
  for (int i = ARRAY_LENGTH(corners) - 1; i >= 0; i--)
  {
    if (corners[i] > 0 && corners[i] < current_steps)
    {
      path.points[path.num_points++] = inset_point(steps_to_point(corners[i],
                                                                  daily_goal,
                                                                  outer_bounds),
                                                   fill_thickness);
    }
  }

  // Add start_inner_point
  path.points[path.num_points++] = start_inner_point;

  gpath_draw_filled(ctx, &path);
  graphics_context_set_stroke_color(ctx, color);
  gpath_draw_outline(ctx, &path);
  free(path.points);
  /// Add round end point.
  graphics_context_set_fill_color(ctx, color);
  GPoint center;
  center.x = (end_outer_point.x + end_inner_point.x) / 2;
  center.y = (end_outer_point.y + end_inner_point.y) / 2;
  graphics_fill_circle(ctx, center, 5);
#elif defined(PBL_ROUND)
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, fill_thickness,
                       DEG_TO_TRIGANGLE(0),
                       DEG_TO_TRIGANGLE(360 * current_steps / daily_goal));
#endif
}
/// This method draws the yellow tick
/// This should be current and goal
void graphics_fill_goal_line(GContext *ctx, int currentGoal,
                             int line_length, int line_width, GRect frame, GColor color){
    const int goal = config_get_daily_goal();
  graphics_context_set_stroke_color(ctx, color);
  const GPoint line_outer_point = steps_to_point(currentGoal, goal, frame);

#if defined(PBL_RECT)
  GPoint line_inner_point = inset_point(line_outer_point, line_length);
#elif defined(PBL_ROUND)
  GRect inner_bounds = grect_inset(frame, GEdgeInsets(line_length));
  GPoint line_inner_point = steps_to_point(currentGoal, goal, inner_bounds);
#endif

  graphics_context_set_stroke_width(ctx, line_width);
  graphics_draw_line(ctx, line_inner_point, line_outer_point);
}

void graphics_draw_weather(GContext *ctx, GRect bounds, GColor color, GBitmap *bitmap){
  GRect weather_text_box = bounds;
    char temp_buffer [5];
    if (data_get_temp(true) != -278){
      snprintf(temp_buffer, sizeof(temp_buffer), "%d %c", data_get_temp(config_getWeather().flgCelcius),config_getWeather().flgCelcius?'C':'F');
    }
    else{
      snprintf(temp_buffer, sizeof(temp_buffer), "Sync");
    }
    //shoe_bitmap_box.size = gbitmap_get_bounds(data_get_green_shoe()).size;
    int text_width = graphics_text_layout_get_content_size(temp_buffer,
                                                           data_get_font(FontSizeSmall), weather_text_box, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter)
                         .w;
    const int font_height = 14;
    const int padding = 5;
    // weather_text_box.size = GSize(text_width, font_height);
    // weather_text_box.origin.x=
    // weather_text_box.origin.y=
    //const int combined_width = shoe_bitmap_box.size.w + padding + text_width;
    const int combined_width = padding + text_width;
    //steps_text_box.origin.x = (bounds.size.w / 2) - (combined_width / 2);
    //steps_text_box.origin.y = PBL_IF_RECT_ELSE(56, 60);
    //shoe_bitmap_box.origin.x = (bounds.size.w / 2) + (combined_width / 2) - shoe_bitmap_box.size.w;
    //shoe_bitmap_box.origin.y = PBL_IF_RECT_ELSE(60, 65);
#if defined(PBL_PLATFORM_APLITE) || defined(PBL_PLATFORM_DIORITE)
    /// Override color
    color = GColorWhite;
#endif // PBL_
    graphics_context_set_text_color(ctx, color);
    graphics_draw_text(ctx, temp_buffer, data_get_font(FontSizeSmall),
                       bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    //graphics_draw_bitmap_in_rect(ctx, bitmap, shoe_bitmap_box);
  
}

void graphics_draw_steps_value(GContext *ctx, GRect bounds, GColor color, GBitmap *bitmap)
{
  GRect steps_text_box = bounds;
  GRect shoe_bitmap_box = bounds;
  const char *steps_buffer = data_get_current_steps_buffer();

  shoe_bitmap_box.size = gbitmap_get_bounds(data_get_green_shoe()).size;

  int text_width = graphics_text_layout_get_content_size(steps_buffer,
                                                         data_get_font(FontSizeMediumL), steps_text_box, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter)
                       .w;
  const int font_height = 14;
  const int padding = 5;
  steps_text_box.size = GSize(text_width, font_height);
  const int combined_width = shoe_bitmap_box.size.w + padding + text_width;

  steps_text_box.origin.x = (bounds.size.w / 2) - (combined_width / 2);
  steps_text_box.origin.y = PBL_IF_RECT_ELSE(48, 53);
  shoe_bitmap_box.origin.x = (bounds.size.w / 2) + (combined_width / 2) - shoe_bitmap_box.size.w;
  shoe_bitmap_box.origin.y = PBL_IF_RECT_ELSE(60, 65);

#if defined(PBL_PLATFORM_APLITE) || defined(PBL_PLATFORM_DIORITE)
  /// Override color
  color = GColorWhite;
#endif
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, steps_buffer, data_get_font(FontSizeMediumL),
                     steps_text_box, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

  graphics_draw_bitmap_in_rect(ctx, bitmap, shoe_bitmap_box);
}
/// Added to support BLE and Battery
void graphics_draw_status_icons(GContext *ctx, GPoint xy, GBitmap *bitmapBaterry, GBitmap *bitmapBLE, unsigned int charge)
{
  GRect bleBox, batteryBox;
  /// get size
  bleBox.size = gbitmap_get_bounds(bitmapBLE).size;
  batteryBox.size = gbitmap_get_bounds(bitmapBaterry).size;
  bleBox.origin = xy;
/// Screen width 144
/// Should be right aligned with 15 px margin
#if defined(PBL_RECT)
  xy.x = 140 - 15 - batteryBox.size.w;
#elif defined(PBL_ROUND)
  xy.x = 140 - 20 - batteryBox.size.w;
#endif
  batteryBox.origin = xy;
  graphics_draw_bitmap_in_rect(ctx, bitmapBLE, bleBox);
  graphics_draw_bitmap_in_rect(ctx, bitmapBaterry, batteryBox);
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_rect(ctx, GRect(batteryBox.origin.x + 7, batteryBox.origin.y + 4, (uint8_t)((charge / 100.0) * 11.0), 4), 0, GCornerNone);
}

void graphics_set_window(Window *window)
{
  s_window = window;
}
