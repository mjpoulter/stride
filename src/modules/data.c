#include "data.h"
#include <pebble-events/pebble-events.h>

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
static Window *win;
static int tempC;
static int tempF;
static int s_current_steps;
static char s_current_steps_buffer[8];

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
  // const struct tm *time_now = util_get_tm();
  s_current_steps = health_service_sum_today(HealthMetricStepCount);
  persist_write_int(AppKeyCurrentSteps, s_current_steps);
  data_update_steps_buffer();
}

void data_reload_averages() {
  app_timer_register(LOAD_DATA_DELAY, load_health_data_handler, NULL);
}

void data_init() {
  /// Init Communication
  APP_LOG(APP_LOG_LEVEL_DEBUG,"data_init");
  /// Weather init opens communication
  generic_weather_init();
  /// We register to the inbox after the weather initialize everything
  events_app_message_register_inbox_received(prv_inbox_received_handler,NULL);
  events_app_message_register_inbox_dropped(inbox_dropped_callback,NULL);
  // Load resources
  /// @TODO: Quick patch. We need to do a refactor to have a more neat image managing
  #if defined(PBL_PLATFORM_APLITE) || defined(PBL_PLATFORM_DIORITE) 
  s_green_shoe = gbitmap_create_with_resource(RESOURCE_ID_WHITE_SHOE_LOGO);
  s_blue_shoe = s_green_shoe;
  s_red_shoe = s_green_shoe;
  #else
  s_green_shoe = gbitmap_create_with_resource(RESOURCE_ID_GREEN_SHOE_LOGO);
  s_blue_shoe = gbitmap_create_with_resource(RESOURCE_ID_BLUE_SHOE_LOGO);
  s_red_shoe = gbitmap_create_with_resource(RESOURCE_ID_RED_SHOE_LOGO);
  #endif
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
  // return 14100;
  // return 10500;
  return s_current_steps;
}

// int data_get_daily_goal() {
//   return dailyGoal;
// }

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

// void setTimeOfDay(struct tm* tm){
//   /// start the walking day at 8 am
//   if(tm->tm_hour<stDailyGoal.startHr){
//     dailyStepsPercentage=0;
//   }
//   else if(tm->tm_hour==stDailyGoal.startHr && tm->tm_min<stDailyGoal.startMin){
//       dailyStepsPercentage=0;
//   }
//   else if(tm->tm_hour>stDailyGoal.endHr){
//     dailyStepsPercentage=100;
//   }
//   else if(tm->tm_hour==stDailyGoal.endHr && tm->tm_min>stDailyGoal.endMin){
//     dailyStepsPercentage=100;
//   }
//   else{

//      dailyStepsPercentage = 100.*((tm->tm_hour-stDailyGoal.startHr)*60.+tm->tm_min)/(stDailyGoal.walkTime);
//   }
// }

// int getDailyStepsPercentage(){
//   return dailyStepsPercentage;
// }

// int getCurrentDailySteps(){
//   // return data_get_daily_goal()*.25;
//   return dailyStepsPercentage*data_get_daily_goal()/100.;
// }

void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message received");
  Tuple *comm_ready = dict_find(iter, MESSAGE_KEY_JSReady);
  
  // Tuple *weather_units = dict_find(iter, MESSAGE_KEY_tempUnits);
  // Tuple *weather_temp = dict_find(iter, MESSAGE_KEY_showTemp);
  // Tuple *weather_show = dict_find(iter, MESSAGE_KEY_showWeather);
  
  // Tuple *daily_goal_tick = dict_find(iter, MESSAGE_KEY_goalTick);
  // Tuple *daily_goal_steps = dict_find(iter, MESSAGE_KEY_dailySteps);
  // Tuple *daily_goal_start = dict_find(iter, MESSAGE_KEY_dailyStart);
  // Tuple *daily_goal_end = dict_find(iter, MESSAGE_KEY_dailyEnd);
  
  if(comm_ready) {
    APP_LOG(APP_LOG_LEVEL_DEBUG,"[%ld][%ld][%ld][%ld]",MESSAGE_KEY_showWeather,MESSAGE_KEY_tempUnits,MESSAGE_KEY_showTemp,MESSAGE_KEY_goalTick);
    bool flgComm_ready = comm_ready->value->int32 == 1;
    APP_LOG(APP_LOG_LEVEL_DEBUG,flgComm_ready?"Comm ready":"Comm not ready");
    if (flgComm_ready){
      APP_LOG(APP_LOG_LEVEL_DEBUG,"Weather init");
      weather_init();
      if(generic_weather_fetch(&weather_callback)){
        APP_LOG(APP_LOG_LEVEL_DEBUG,"Weather fetched!");
      }
      else{
        APP_LOG(APP_LOG_LEVEL_DEBUG,"Weather fetch fail!");
      }
    }
  }

  
  // if(daily_goal_tick) {
  //   flgDailyGoalShow = daily_goal_tick->value->int32 == 1;
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,flgDailyGoalShow?"Daily goal true":"Daily goal false");
  // }
  
  // if(daily_goal_steps) {
  //   dailyGoal = daily_goal_steps->value->int32 ;
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,"Daily goal :%d",atoi(daily_goal_steps->value->cstring));
  // }
  // if(daily_goal_start){
  //   getHourAndMinutes(daily_goal_start->value->cstring ,&stDailyGoal.startHr,&stDailyGoal.startMin);
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,"Daily goal start %d:%d",stDailyGoal.startHr,stDailyGoal.startMin);
  // }
  // if(daily_goal_end){
  //   getHourAndMinutes(daily_goal_end->value->cstring ,&stDailyGoal.endHr,&stDailyGoal.endMin);
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,"Daily goal end (%s) %d:%d",daily_goal_end->value->cstring,stDailyGoal.endHr,stDailyGoal.endMin);
  // } 
  // if(weather_units){
  //   flgWeatherCelcius = (weather_units->value->int8 == 'c');
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,"Use celcius:[%s],[%u],[%c]",flgWeatherCelcius?"YES":"NO",weather_units->value->uint8,weather_units->value->uint8);
  // } 
  // if(weather_temp){
  //   flgWeatherTemperature = (weather_temp->value->int32 == 1);
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,"Show temperature:[%s]",flgWeatherTemperature?"YES":"NO");
  // } 
  // if(weather_show){
  //   flgWeatherShow = (weather_show->value->int32 == 1);
  //   APP_LOG(APP_LOG_LEVEL_DEBUG,"Show weather:[%s]",flgWeatherShow?"YES":"NO");
  // } 
  // /// review minutes and midnight
  // stDailyGoal.walkTime=(stDailyGoal.endHr-stDailyGoal.startHr)*60+(stDailyGoal.endMin-stDailyGoal.startMin);
  // /// redraw this should be move to main
  main_window_redraw();
  layer_mark_dirty(window_get_root_layer(win));
};

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}



void storeRootWindow(Window* w){
  win=w;
}

void weather_init(){
    //generic_weather_init();
    generic_weather_set_provider(GenericWeatherProviderOpenWeatherMap);
    generic_weather_set_api_key("5203d95b1a7973943dee2ca61eb050f9");
    generic_weather_set_feels_like(false); 
}

void weather_callback(GenericWeatherInfo *info, GenericWeatherStatus status){
        switch (status){
          case GenericWeatherStatusAvailable:
          APP_LOG(APP_LOG_LEVEL_DEBUG,"Weather ready [%d]",info->temp_c);
          tempC = info->temp_c;
          tempF = info->temp_f;
          break;
        default:
          APP_LOG(APP_LOG_LEVEL_DEBUG,"Weather not ready");
          tempC = -278;
          tempF = -460;
          break;
        }
}

int data_get_temp(bool celcius){
  if (celcius){
    return tempC;
  } else{
    return tempF;
  }
}
