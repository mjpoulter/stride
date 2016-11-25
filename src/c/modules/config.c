#include "config.h"
#include <pebble-events/pebble-events.h>

static struct stDailyGoal dailyGoal;
static struct stWeather weather;
struct stConfig {
  struct stWeather weather;
  struct stDailyGoal dailyGoal;
};

void config_inbox_received_handler(DictionaryIterator *iter, void *context) ;

struct stDailyGoal config_getDailyGoal(){
    return dailyGoal;
}
struct stWeather config_getWeather(){
  return weather;
}

bool config_init(){
  if( persist_exists(PERSISTENT_CONFIG)){
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Loading persistent configuration");
    struct stConfig config;
    persist_read_data(PERSISTENT_CONFIG, &config, sizeof(struct stConfig));
    dailyGoal=config.dailyGoal;
    weather=config.weather;
  }
  else{
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Loading default configuration");
    dailyGoal.startHr=8;
    dailyGoal.startMin=0;
    dailyGoal.endHr=24;
    dailyGoal.endMin=0;
    dailyGoal.walkTime=16*60;
    dailyGoal.steps=15000;
    weather.flgCelcius=true;
    weather.flgShow=true;
    weather.flgTemperature=true;
    dailyGoal.walkTime=(dailyGoal.endHr==0?24:dailyGoal.endHr-dailyGoal.startHr)*60+(dailyGoal.endMin-dailyGoal.startMin);
    dailyGoal.flgShow=true;
  }
  events_app_message_register_inbox_received(config_inbox_received_handler,NULL);
  return true;
}

/// Needs to renamed and replaced 
int config_get_daily_goal() {
  return dailyGoal.steps;
}


void getHourAndMinutes(char* buffer,int *hr,int *min){
  char number[3]={'\0','\0','\0'};
  number[0]=buffer[0];
  number[1]=buffer[1];
  *hr=atoi(number);
  number[0]=buffer[3];
  number[1]=buffer[4];
  *min=atoi(number);
}

void config_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *weather_units = dict_find(iter, MESSAGE_KEY_tempUnits);
  Tuple *weather_temp = dict_find(iter, MESSAGE_KEY_showTemp);
  Tuple *weather_show = dict_find(iter, MESSAGE_KEY_showWeather);  
  Tuple *daily_goal_tick = dict_find(iter, MESSAGE_KEY_goalTick);
  Tuple *daily_goal_steps = dict_find(iter, MESSAGE_KEY_dailySteps);
  Tuple *daily_goal_start = dict_find(iter, MESSAGE_KEY_dailyStart);
  Tuple *daily_goal_end = dict_find(iter, MESSAGE_KEY_dailyEnd);
  bool msgRec = false;

  if(daily_goal_tick) {
    dailyGoal.flgShow = daily_goal_tick->value->int32 == 1;
    APP_LOG(APP_LOG_LEVEL_DEBUG,dailyGoal.flgShow?"Daily goal true":"Daily goal false");
    msgRec = true;
  }
  
  if(daily_goal_steps) {
    dailyGoal.steps = atoi(daily_goal_steps->value->cstring) ;
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Daily goal :%d",atoi(daily_goal_steps->value->cstring));
    msgRec = true;
  }
  if(daily_goal_start){
    getHourAndMinutes(daily_goal_start->value->cstring ,&dailyGoal.startHr,&dailyGoal.startMin);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Daily goal start %d:%d",dailyGoal.startHr,dailyGoal.startMin);
    msgRec = true;
  }
  if(daily_goal_end){
    getHourAndMinutes(daily_goal_end->value->cstring ,&dailyGoal.endHr,&dailyGoal.endMin);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Daily goal end (%s) %d:%d",daily_goal_end->value->cstring,dailyGoal.endHr,dailyGoal.endMin);
    if(dailyGoal.endHr==0)dailyGoal.endHr=24;
    msgRec = true;
  } 
  //weather
  if(weather_units){
    weather.flgCelcius = (weather_units->value->int8 == 'c');
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Use celcius:[%s],[%u],[%c]",weather.flgCelcius?"YES":"NO",weather_units->value->uint8,weather_units->value->uint8);
    msgRec = true;
  } 
  if(weather_temp){
    weather.flgTemperature = (weather_temp->value->int32 == 1);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Show temperature:[%s]",weather.flgTemperature?"YES":"NO");
    msgRec = true;
  } 
  if(weather_show){
    weather.flgShow = (weather_show->value->int32 == 1);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Show weather:[%s]",weather.flgShow?"YES":"NO");
    msgRec = true;
  } 
  if(msgRec){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Configuration Message received");
    struct stConfig config;
    dailyGoal.walkTime=(dailyGoal.endHr==0?24:dailyGoal.endHr-dailyGoal.startHr)*60+(dailyGoal.endMin-dailyGoal.startMin);

    config.weather=weather;
    config.dailyGoal=dailyGoal;
    persist_write_data(PERSISTENT_CONFIG, &config, sizeof(config));
  }else{
    //  APP_LOG(APP_LOG_LEVEL_DEBUG, "Configuration Message received");

  }
};