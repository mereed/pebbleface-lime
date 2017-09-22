module.exports=[
    {
        "type": "heading",
        "defaultValue": "Configuration"
    },
    {
        "type": "text",
        "defaultValue": "<h6>A white button = OFF <br> An orange button = ON</h6>",
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "<h5>Weather</h5>"
            },
            {
                "type": "input",
                "messageKey": "location",
                "label": "Location",
                "description": "Enter a location for \"Use GPS\" NO. ie. city name or postal/zip code.",
                "defaultValue": ""
            },
            {
                "type": "toggle",
                "messageKey": "use_gps",
                "label": "Use GPS",
                "description": "If you want to conserve battery, set GPS=NO AND enter your location eg. London.",
                "defaultValue": true
            },
            {
                "type": "radiogroup",
                "messageKey": "units",
                "label": "Temp. Units",
                "defaultValue": "fahrenheit",
                "options": [
                    {
                        "label": "Celsius",
                        "value": "celsius"
                    },
                    {
                        "label": "Fahrenheit",
                        "value": "fahrenheit"
                    }
                ]
            },
			            {
                "type": "select",
                "messageKey": "iconnum",
                "label": " Weather icon style",
                "defaultValue": "0.",
                "options": [
 
                    {
                        "label": "Hex",
                        "value": "0."
                    },
			        {
                        "label": "Pebble",
                        "value": "1."
                    },
			                   {
                        "label": "Simple B&W",
                        "value": "2."
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
                "defaultValue": "<h5>Display</h5>"
            },
			
			{
                "type": "toggle",
                "messageKey": "basic_center",
                "label": " <strong> CENTERED</strong> mode",
                "description": "This option centers the time digits.  Use the toggles below to turn off other elements.",
                "defaultValue": false
            },			
			
			
			{
                "type": "toggle",
                "messageKey": "row1right",
                "label": "Remove Status Icons (BT/Batt)",
                "defaultValue": false
            },
			{
                "type": "toggle",
                "messageKey": "row2left",
                "label": "Remove Date",
                "defaultValue": false
            },
            {
                "type": "toggle",
                "messageKey": "weatherstatus",
                "label": "Remove Weather (also disables)",
                "defaultValue": false
            },
			{
                "type": "toggle",
                "messageKey": "language",
                "label": "Remove Heart Rate",
                 "defaultValue": false
           },
			{
                "type": "toggle",
                "messageKey": "color_status",
                "label": "Remove Steps",
                "defaultValue": false
            },
			 {
                "type": "toggle",
                "messageKey": "health",
                "label": "Remove ALL health",
                "defaultValue": false
            },
			 {
                "type": "toggle",
                "messageKey": "fill",
                "label": "Remove leading Zero",
                "defaultValue": false
            },
			 {
                "type": "toggle",
                "messageKey": "lines",
                "label": "Remove divider lines",
                "defaultValue": false
            },
			 {
                "type": "toggle",
                "messageKey": "invert",
                "label": "Invert colours",
                "defaultValue": false
            },
		]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "<h5>Vibration</h5>"
            },
            {
                "type": "toggle",
                "messageKey": "bluetoothvibe_status",
                "label": "Bluetooth vibration",
                "defaultValue": false
            },
            {
                "type": "toggle",
                "messageKey": "hourlyvibe_status",
                "label": "Hourly vibration",
                "defaultValue": false
            }
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save Settings"
    },
    {
        "type": "text",
        "defaultValue": "<h6><center>If you find this watchface useful, <br> please consider making a <a href='https://www.paypal.me/markchopsreed'>small donation</a>. <br> Thank you. </center></h6>"
    }
];