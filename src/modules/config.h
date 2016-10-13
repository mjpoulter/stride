#pragma once
#define PERSISTENT_CONFIG 1
#include <pebble.h>

struct stDailyGoal{
  int startHr;
  int startMin;
  int endHr;
  int endMin;
  int walkTime;
  int steps;
  bool flgShow;
} ;

struct stWeather{
  bool flgShow;
  bool flgTemperature;
  bool flgCelcius;
} ;

bool config_init();
// bool config_drawDailyGoal();
struct stDailyGoal config_getDailyGoal();
struct stWeather config_getWeather();
int config_get_daily_goal();