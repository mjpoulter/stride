module.exports = [
  {
    "type": "heading",
    "defaultValue": "Stride++ configuration page"
  },
   {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather"
      },
      {
        "type": "toggle",
        "messageKey": "showTemp",
        "label": "Show temperature",
        "defaultValue": 1
      },
      // Disable this option on this release
      // {
      //   "type": "toggle",
      //   "label": "Show weather",
      //   "defaultValue": 1,
      //   "messageKey": "showWeather"
      // },
      {
        "type": "select",
        "messageKey": "tempUnits",
        "defaultValue": "C",
        "label": "Temperature:",
        "options": [
          {
            "label": "C",
            "value": "c",
          },
          {
            "label": "F",
            "value": "f",
          }
        ]
      }

    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Steps"
      },
      {
        "type": "input",
        "messageKey": "dailySteps",
        "defaultValue": "14000",
        "label": "Daily Steps Goal",
      },
      {
        "type": "toggle",
        "messageKey": "goalTick",
        "label": "Show daily goal tick",
        "defaultValue": 1
      },
      {
        "type": "input",
        "messageKey": "dailyStart",
        "defaultValue": "08:00",
        "label": "Daily step count start",
        "attributes": {
          "placeholder": "08:00",
          "limit": 6,
          "type": "time"
        },
      },
      {
        "type": "input",
        "messageKey": "dailyEnd",
        "defaultValue": "00:00",
        "label": "Daily step count stop",
        "attributes": {
          "placeholder": "00:00",
          "limit": 6,
          "type": "time"
        }
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
]