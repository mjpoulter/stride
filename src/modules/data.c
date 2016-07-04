#include "data.h"

typedef enum {
  AppKeyCurrentAverage = 0,
  AppKeyDailyAverage,
  AppKeyCurrentSteps
} AppKey;

typedef enum {
  AverageTypeCurrent = 0,
  AverageTypeDaily
} AverageType;

/// added BLE and Battery
static GBitmap *s_blue_shoe, *s_red_shoe,*s_green_shoe, *sbmpBleN,*sbmpBle,*sbmpBatteryCharging,*sbmpBattery;

static GFont s_font_small, s_font_big, s_font_med;

static int s_current_steps;
static char s_current_steps_buffer[8];
static int dailyStepsPercentage;
static bool flgDailyGoalShow=true;

void data_update_steps_buffer() {
  int thousands = s_current_steps / 1000;
  int hundreds = s_current_steps % 1000;
  if(thousands > 0) {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer), "%d,%03d", thousands, hundreds);
  } else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer), "%d", hundreds);
  }
  main_window_redraw();
}

static void load_health_data_handler(void *context) {
  const struct tm *time_now = util_get_tm();
  s_current_steps = health_service_sum_today(HealthMetricStepCount);
  persist_write_int(AppKeyCurrentSteps, s_current_steps);
  data_update_steps_buffer();
}

void data_reload_averages() {
  app_timer_register(LOAD_DATA_DELAY, load_health_data_handler, NULL);
}

void data_init() {
  // Load resources
  s_green_shoe = gbitmap_create_with_resource(RESOURCE_ID_GREEN_SHOE_LOGO);
  s_blue_shoe = gbitmap_create_with_resource(RESOURCE_ID_BLUE_SHOE_LOGO);
  s_red_shoe = gbitmap_create_with_resource(RESOURCE_ID_RED_SHOE_LOGO);
  sbmpBattery= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  sbmpBatteryCharging= gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGER);
  sbmpBle=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLE);
  sbmpBleN=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLEN);
  s_font_small = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_font_med = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_font_big = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  // First time persist
  if(!persist_exists(AppKeyCurrentSteps)) {
    s_current_steps = 0;
  } else {
    s_current_steps = persist_read_int(AppKeyCurrentSteps);
  }
  data_update_steps_buffer();
  // Avoid half-second delay loading the app by delaying API read
  data_reload_averages();
}

void data_deinit() {
  gbitmap_destroy(s_green_shoe);
  gbitmap_destroy(s_blue_shoe);
  gbitmap_destroy(s_red_shoe);
}

int data_get_current_steps() {
  /// Debug
  // return 12000;
  return s_current_steps;
}

int data_get_daily_goal() {
  return 14000;
}

void data_set_current_steps(int value) {
  s_current_steps = value;
}

GFont data_get_font(FontSize size) {
  switch(size) {
    case FontSizeSmall:  return s_font_small;
    case FontSizeMedium: return s_font_med;
    case FontSizeLarge:  return s_font_big;
    default: return s_font_small;
  }
}

GBitmap* data_get_blue_shoe() {
  return s_blue_shoe;
}

GBitmap* data_get_red_shoe() {
  return s_red_shoe;
}

GBitmap* data_get_green_shoe() {
  return s_green_shoe;
}

char* data_get_current_steps_buffer() {
  return s_current_steps_buffer;
}

/// Added to support battery and BLE icons
GBitmap* data_get_BLE(bool isConnected) {
    if (isConnected) return sbmpBle;
    else return sbmpBleN;
}

GBitmap* data_get_Battery(bool isCharging) {
    if (isCharging) return sbmpBatteryCharging;
    else return sbmpBattery;
}

void setTimeOfDay(struct tm* tm){
  /// start the walking day at 8 am
  if(tm->tm_hour<8){
    dailyStepsPercentage=0;
  }
  else{
    dailyStepsPercentage = 100.*(tm->tm_hour-8+tm->tm_min/60.)/16.;
  }
}

int getDailyStepsPercentage(){
  return dailyStepsPercentage;
}

int getCurrentDailySteps(){
  // Debug
  // return data_get_daily_goal()*.25;
  return dailyStepsPercentage*data_get_daily_goal()/100.;
}

bool drawDailyGoal(){
 return flgDailyGoalShow; 
}
  void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *daily_goal_tick = dict_find(iter, MESSAGE_KEY_goalTick);
  if(daily_goal_tick) {
    flgDailyGoalShow = daily_goal_tick->value->int32 == 1;
  }
};