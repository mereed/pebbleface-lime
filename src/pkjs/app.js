// config page

var Clay = require('pebble-clay');
var clayConfig = require('./config.js');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

// constants

var messageKeys = require('message_keys');

var CLEAR_DAY = 0;
var CLEAR_NIGHT = 1;
var WINDY = 2;
var COLD = 3;
var PARTLY_CLOUDY_DAY = 4;
var PARTLY_CLOUDY_NIGHT = 5;
var HAZE = 6;
var CLOUD = 7;
var RAIN = 8;
var SNOW = 9;
var HAIL = 10;
var CLOUDY = 11;
var STORM = 12;
var FOG = 13;
var NA = 14;

var imageId = {
  0 : STORM, //tornado
  1 : STORM, //tropical storm
  2 : STORM, //hurricane
  3 : STORM, //severe thunderstorms
  4 : STORM, //thunderstorms
  5 : HAIL, //mixed rain and snow
  6 : HAIL, //mixed rain and sleet
  7 : HAIL, //mixed snow and sleet
  8 : HAIL, //freezing drizzle
  9 : RAIN, //drizzle
  10 : HAIL, //freezing rain
  11 : RAIN, //showers
  12 : RAIN, //showers
  13 : SNOW, //snow flurries
  14 : SNOW, //light snow showers
  15 : SNOW, //blowing snow
  16 : SNOW, //snow
  17 : HAIL, //hail
  18 : HAIL, //sleet
  19 : HAZE, //dust
  20 : FOG, //foggy
  21 : HAZE, //haze
  22 : HAZE, //smoky
  23 : WINDY, //blustery
  24 : WINDY, //windy
  25 : COLD, //cold
  26 : CLOUDY, //cloudy
  27 : CLOUDY, //mostly cloudy (night)
  28 : CLOUDY, //mostly cloudy (day)
  29 : PARTLY_CLOUDY_NIGHT, //partly cloudy (night)
  30 : PARTLY_CLOUDY_DAY, //partly cloudy (day)
  31 : CLEAR_NIGHT, //clear (night)
  32 : CLEAR_DAY, //sunny
  33 : CLEAR_NIGHT, //fair (night)
  34 : CLEAR_DAY, //fair (day)
  35 : HAIL, //mixed rain and hail
  36 : CLEAR_DAY, //hot
  37 : STORM, //isolated thunderstorms
  38 : STORM, //scattered thunderstorms
  39 : STORM, //scattered thunderstorms
  40 : STORM, //scattered showers
  41 : SNOW, //heavy snow
  42 : SNOW, //scattered snow showers
  43 : SNOW, //heavy snow
  44 : CLOUD, //partly cloudy
  45 : STORM, //thundershowers
  46 : SNOW, //snow showers
  47 : STORM, //isolated thundershowers
  3200 : NA, //not available
};

var options = localStorage.getItem('clay-settings');
console.log('options: ' + options);
var options = JSON.parse(options);

// if no options are available, then the JS needs this minimal config to work
// the watch will have it's defaults set in the C code
if (options === null)
    options = {
        "use_gps" : true,
        "units" : "fahrenheit",
        "hideweather" : false
    };

function getWeatherFromLatLong(latitude, longitude) {
  getWeatherFromLocation('(' + latitude + ',' + longitude + ')');
}

function getWeatherFromLocation(location) {
  console.log("location: " + location);

  var celsius = (options.units == 'celsius');
  var query = encodeURI('SELECT item.condition, item.title ' +
                        'FROM weather.forecast ' +
                        'WHERE woeid IN ( ' +
                        ' SELECT woeid ' +
                        ' FROM geo.places ' +
                        ' WHERE text="' + location + '" ' +
                        ' LIMIT 1) ' +
                        'AND u="' + (celsius ? 'c' : 'f') + '"');

  var url = "http://query.yahooapis.com/v1/public/yql?q=" + query +
            "&format=json&diagnostics=false";
  console.log("url: " + url);

  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  req.timeout = 20000; // 20 sec
  req.onload = function(e) {
    if (req.readyState != 4) return;

    console.log("response: " + req.responseText);

    if (req.status != 200) return;

    var response;
    try {
      response = JSON.parse(req.responseText);
      if (response === null) return;
    } catch(err) {
      return;
    }

    if (response.query.results.channel === null) return;

    var item = response.query.results.channel.item;
    var temperature = item.condition.temp + '\u00B0';
    var icon = imageId[item.condition.code];

    console.log("title: " + JSON.stringify(item.title));
    // console.log("condition " + condition.text);
    console.log("temp: " + temperature);
    console.log("icon: " + icon);

    Pebble.sendAppMessage({
      "icon" : icon,
      "temperature" : temperature
    });
  };
  req.send(null);
}

//var locationOptions = { "timeout": 30000, "maximumAge": 60000 };
var locationOptions = { "timeout": 15000, "maximumAge": 60000 };

function updateWeather() {
  if (options.hideweather === true) return;

  if (options.use_gps === true) {
    navigator.geolocation.getCurrentPosition(locationSuccess,
                                             locationError,
                                             locationOptions);
  } else {
    getWeatherFromLocation(options.location);
  }
}

function locationSuccess(pos) {
  getWeatherFromLatLong(pos.coords.latitude,
                        pos.coords.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);

  Pebble.sendAppMessage({
    "icon": 14,
    "temperature":"   "
  });
}

Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
    if (e && !e.response) {
        console.log('no options received');
        return;
    }

    var dict = clay.getSettings(e.response);

    console.log("new options dump: " + JSON.stringify(dict));

    // transform values to be backward compatible
    for (var key in dict) {
      if (dict.hasOwnProperty(key)) {
        // if the value is a string, simply parse it to int
        if (typeof dict[key] == "string") {
          dict[key] = parseInt(dict[key]);
        }
      }
    }
    console.log("transformed options dump: " + JSON.stringify(dict));

    // we have to delete NaN key, because some options do not have messageKey
    // those options are used only in this JS!
    delete dict.NaN; 

    // send new values to pebble
    Pebble.sendAppMessage(dict);

    // load the flattened edition of settings
    options = JSON.parse(localStorage.getItem('clay-settings'));

    updateWeather();
});

Pebble.addEventListener("ready", function(e) {
  //console.log("connect!" + e.ready);

  updateWeather();

  setInterval(function() {
    //console.log("timer fired");
    updateWeather();
//  }, 60000); // 1 minutes
//  }, 300000); // 5 minutes
//  }, 600000); // 10 minutes
//  }, 1200000); // 20 minutes
  }, 1800000); // 30 minutes

  console.log(e.type);
});

/*
var myAPIKey = 'f7a37589bf26fb434e35fe5b24d1a20b';

function iconFromWeatherId(weatherIcon) {
  var iconFile = "";
    switch (weatherIcon) {
        case "01d" : iconFile = 0; break; //clearday
        case "01n" : iconFile = 1; break; //clearnight
        case "02d" : iconFile = 4; break; //cloudday
        case "02n" : iconFile = 5; break; //cloudnight
        case "03d" : iconFile = 2; break; //clouds
        case "03n" : iconFile = 2; break; //clouds
        case "04d" : iconFile = 2; break; //clouds
        case "04n" : iconFile = 2; break; //clouds
        case "09d" : iconFile = 6; break; //rain
        case "09n" : iconFile = 6; break; //rain
        case "10d" : iconFile = 6; break; //rain
        case "10n" : iconFile = 6; break; //rain
        case "11d" : iconFile = 9; break; //thunder
        case "11n" : iconFile = 9; break; //thunder
        case "13d" : iconFile = 7; break; //snow
        case "13n" : iconFile = 7; break; //snow
        case "50d" : iconFile = 10; break; //mist
        case "50n" : iconFile = 10; break; //mist
        default    : iconFile = 3; break; //blank
    }
    return iconFile;
}

function fetchWeather(latitude, longitude) {
  var req = new XMLHttpRequest();
  req.open('GET', 'http://api.openweathermap.org/data/2.5/weather?' +
    'lat=' + latitude + '&lon=' + longitude + '&cnt=1&appid=' + myAPIKey, true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
        var response = JSON.parse(req.responseText);
        var temperatureC = Math.round(response.main.temp - 273.15);
        var temperatureF = Math.round(((response.main.temp -273.5) * 9/5) + 32);
    var icon = iconFromWeatherId(response.weather[0].icon);
//  var icon = iconFromWeatherId(response.weather[0].id);
   //     var city = response.name;
        console.log(temperatureC);
        console.log(temperatureF);
        console.log(icon);
   //     console.log(city);
        Pebble.sendAppMessage({
          'WEATHER_ICON_KEY': icon,
          'WEATHER_TEMPERATUREC_KEY': temperatureC + '\xB0',
          'WEATHER_TEMPERATUREF_KEY': temperatureF + '\xB0'
    //      'WEATHER_CITY_KEY': city
        });
      } else {
        console.log('Error');
      }
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
//    'WEATHER_CITY_KEY': 'Loc?',
    'WEATHER_TEMPERATUREC_KEY': 'N/A',
    'WEATHER_TEMPERATUREF_KEY': 'N/A'
  });
}

var locationOptions = {
  'timeout': 15000,
  'maximumAge': 60000
};

Pebble.addEventListener('ready', function (e) {
  console.log('connect!' + e.ready);
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  console.log(e.type);
});

Pebble.addEventListener('appmessage', function (e) {
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  console.log(e.type);
  console.log(e.payload.temperature);
  console.log('message!');
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});
*/