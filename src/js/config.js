module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Stride++ configuration page."
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
  "defaultValue": true
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
];