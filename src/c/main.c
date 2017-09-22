/*

Copyright (C) 2017 Mark Reed / Balazas B

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-------------------------------------------------------------------

*/

#include <pebble.h>
#include "pebble-effect-layer.h"

#define TOTAL_TIME_DIGITS 4

EffectLayer* effect_layer;

static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_CLEAR_DAY,
  RESOURCE_ID_CLEAR_NIGHT,
  RESOURCE_ID_WINDY,
  RESOURCE_ID_COLD,
  RESOURCE_ID_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_HAZE,
  RESOURCE_ID_CLOUD,
  RESOURCE_ID_RAIN,
  RESOURCE_ID_SNOW,
  RESOURCE_ID_HAIL,
  RESOURCE_ID_CLOUDY,
  RESOURCE_ID_STORM,
  RESOURCE_ID_FOG,
  RESOURCE_ID_NA,
};

static const uint32_t WEATHER_ICONS2[] = {
  RESOURCE_ID_CLEAR_DAY2,
  RESOURCE_ID_CLEAR_NIGHT2,
  RESOURCE_ID_WINDY2,
  RESOURCE_ID_COLD2,
  RESOURCE_ID_PARTLY_CLOUDY_DAY2,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT2,
  RESOURCE_ID_HAZE2,
  RESOURCE_ID_CLOUD2,
  RESOURCE_ID_RAIN2,
  RESOURCE_ID_SNOW2,
  RESOURCE_ID_HAIL2,
  RESOURCE_ID_CLOUDY2,
  RESOURCE_ID_STORM2,
  RESOURCE_ID_FOG2,
  RESOURCE_ID_NA,
};

static const uint32_t WEATHER_ICONS3[] = {
  RESOURCE_ID_CLEAR_DAY3,
  RESOURCE_ID_CLEAR_NIGHT3,
  RESOURCE_ID_WINDY3,
  RESOURCE_ID_COLD3,
  RESOURCE_ID_PARTLY_CLOUDY_DAY3,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT3,
  RESOURCE_ID_HAZE3,
  RESOURCE_ID_CLOUD3,
  RESOURCE_ID_RAIN3,
  RESOURCE_ID_SNOW3,
  RESOURCE_ID_HAIL3,
  RESOURCE_ID_CLOUDY3,
  RESOURCE_ID_STORM3,
  RESOURCE_ID_FOG3,
  RESOURCE_ID_NA,
};


// Setting values
static bool basic_center;
static bool fill;
static bool weather_status;
static bool bluetoothvibe_status;
static bool hourlyvibe_status;
static bool row1right;
static bool row2left;
static bool current_language;
static bool color_status;
static bool lines;
static bool health;
static bool invert;
static enum weatherKeys { FORMAT_ICON1 = 0, FORMAT_ICON2, FORMAT_ICON3, FORMAT_END = FORMAT_ICON3 } iconnum;



// Setting keys
enum settingKeys {
  SETTING_STATUS_KEY,
  SETTING_LANGUAGE_KEY,
  SETTING_FORMAT_KEY,
  SETTING_TEMPERATURE_KEY,
  SETTING_ICON_KEY,
  SETTING_WEATHERSTATUS_KEY,
  HOURLYVIBE_KEY,
  BLUETOOTHVIBE_KEY,
  SECS_KEY,
  SETTING_COLORSTATUS_KEY,
  SETTING_LINES_KEY,
  SETTING_HEALTH_KEY,
  SETTING_ICONNUM_KEY,
  BASIC_CENTER_KEY,
  INVERT_COLOR_KEY
};

static Window *s_main_window;
static Layer *window_layer;

static GBitmap *s_time_digits[TOTAL_TIME_DIGITS];
static BitmapLayer *s_time_digits_layers[TOTAL_TIME_DIGITS];

static const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0,
  RESOURCE_ID_IMAGE_NUM_1,
  RESOURCE_ID_IMAGE_NUM_2,
  RESOURCE_ID_IMAGE_NUM_3,
  RESOURCE_ID_IMAGE_NUM_4,
  RESOURCE_ID_IMAGE_NUM_5,
  RESOURCE_ID_IMAGE_NUM_6,
  RESOURCE_ID_IMAGE_NUM_7,
  RESOURCE_ID_IMAGE_NUM_8,
  RESOURCE_ID_IMAGE_NUM_9
};

const int BIG_DIGIT2_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM2_0,
  RESOURCE_ID_IMAGE_NUM2_1,
  RESOURCE_ID_IMAGE_NUM2_2,
  RESOURCE_ID_IMAGE_NUM2_3,
  RESOURCE_ID_IMAGE_NUM2_4,
  RESOURCE_ID_IMAGE_NUM2_5,
  RESOURCE_ID_IMAGE_NUM2_6,
  RESOURCE_ID_IMAGE_NUM2_7,
  RESOURCE_ID_IMAGE_NUM2_8,
  RESOURCE_ID_IMAGE_NUM2_9
};

//BACKGROUND
static GBitmap* background;
static BitmapLayer *background_layer;   

static GBitmap *dateblank_image;
static GBitmap *heart_image;
static GBitmap *block_image;
static GBitmap *block2_image;
static BitmapLayer *dateblank_layer;
static BitmapLayer *heart_layer;
static BitmapLayer *block_layer;
static BitmapLayer *block2_layer;

int charge_percent = 0;

TextLayer *text_daynum_layer;
static GFont futura;
static GFont futura2;

BitmapLayer *layer_conn_img;
GBitmap *img_bt_connect;
GBitmap *img_bt_disconnect;

static TextLayer *steps_label, *hr_label;

static GBitmap *icon_bitmap = NULL;
static GBitmap *bitmap_quiettime;    
BitmapLayer *layer_quiettime;                          
static BitmapLayer *icon_layer;
static TextLayer *temp_layer;

static AppSync        app;
static uint8_t        sync_buffer[256]; 

TextLayer *s_hrm_layer;
static int s_hr;
static char s_current_hr_buffer[8];

static uint8_t batteryPercent;
static BitmapLayer *battery_image_layer;
static BitmapLayer *battery_layer;
static GBitmap *battery_image;


void change_battery_icon(bool charging) {

if (battery_image) {
    gbitmap_destroy(battery_image);
    battery_image = NULL;
  }
	if(charging) {
    battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGE);
  }  else {
    battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  }  
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
  layer_mark_dirty(bitmap_layer_get_layer(battery_image_layer));
}

void update_battery_state(BatteryChargeState charge_state) {

  batteryPercent = charge_state.charge_percent;

  if(batteryPercent==100) {
        change_battery_icon(false);
  
    return;
  }
  layer_set_hidden(bitmap_layer_get_layer(battery_layer), charge_state.is_charging);
  change_battery_icon(charge_state.is_charging);
} 

void battery_layer_update_callback(Layer *me, GContext* ctx) {        
  //draw the remaining battery percentage
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(2, 2, ((batteryPercent/100.0)*19.0), 5), 0, GCornerNone);
}

// Current heart rate
static void get_hr() {
		  s_hr = (int)health_service_peek_current_value(HealthMetricHeartRateBPM);
}

static void display_heart_rate() {
		
  if(s_hr>0) {
    snprintf(s_current_hr_buffer,sizeof(s_current_hr_buffer),"%d",s_hr);
    text_layer_set_text(hr_label,s_current_hr_buffer);

  }
}

static void health_handler(HealthEventType event, void *context) {

  if(event != HealthEventSleepUpdate) {
    get_hr();
    display_heart_rate(); 
  }	
 
  static char s_value_buffer[8];
		
  if (event == HealthEventMovementUpdate) {
    // display the step count
    snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", (int)health_service_sum_today(HealthMetricStepCount));	  
    text_layer_set_text(steps_label, s_value_buffer);	  
  }
}

void set_invert_color(bool invert) {
  if (invert && effect_layer == NULL) {
    // Add inverter layer
    Layer *window_layer = window_get_root_layer(s_main_window);

#if PBL_PLATFORM_CHALK
    effect_layer = effect_layer_create(GRect(0, 0, 180, 180));
#else
	effect_layer = effect_layer_create(GRect(0, 0, 144, 168));
#endif
    layer_add_child(window_layer, effect_layer_get_layer(effect_layer));
	effect_layer_add_effect(effect_layer, effect_invert, NULL);

  } else if (!invert && effect_layer != NULL) {
    // Remove Inverter layer
    layer_remove_from_parent(effect_layer_get_layer(effect_layer));
    effect_layer_destroy(effect_layer);
    effect_layer = NULL;
  }
  // No action required
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;

  *bmp_image = gbitmap_create_with_resource(resource_id);
	
  GRect bounds = gbitmap_get_bounds(*bmp_image);

  GRect main_frame = GRect(origin.x, origin.y, bounds.size.w, bounds.size.h);
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), main_frame);

  if (old_image != NULL) {
  	gbitmap_destroy(old_image);
  }
}

void toggle_bluetooth_icon(bool connected) {
	
    if (connected) {
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
    } else {
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_disconnect);
	}
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth_icon(connected);
}

static unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;
  return display_hour ? display_hour : 12;
}

static void update_display(struct tm *tick_time) {
	
  static char date_text1b[] = "xxx 00xx xxx";
	
// Display daynum 

  strftime(date_text1b, sizeof(date_text1b), "%a %b %e", tick_time);
  text_layer_set_text(text_daynum_layer, date_text1b);

// Display hour
  unsigned short display_hour = get_display_hour(tick_time->tm_hour);	

  set_container_image(&s_time_digits[0], s_time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour / 10], GPoint(40, 5));
  set_container_image(&s_time_digits[1], s_time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour % 10], GPoint(95, 5));
	
  set_container_image(&s_time_digits[2], s_time_digits_layers[2], BIG_DIGIT2_IMAGE_RESOURCE_IDS[tick_time->tm_min / 10], GPoint(40, 78));
  set_container_image(&s_time_digits[3], s_time_digits_layers[3], BIG_DIGIT2_IMAGE_RESOURCE_IDS[tick_time->tm_min % 10], GPoint(95, 78));
	
  if (basic_center) {
		
  set_container_image(&s_time_digits[0], s_time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour / 10], GPoint(22, 5));
  set_container_image(&s_time_digits[1], s_time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour % 10], GPoint(77, 5));
	
  set_container_image(&s_time_digits[2], s_time_digits_layers[2], BIG_DIGIT2_IMAGE_RESOURCE_IDS[tick_time->tm_min / 10], GPoint(22, 78));
  set_container_image(&s_time_digits[3], s_time_digits_layers[3], BIG_DIGIT2_IMAGE_RESOURCE_IDS[tick_time->tm_min % 10], GPoint(77, 78));		
		
	}
	
  if (fill) {
   if (display_hour / 10 == 0) {
    	layer_set_hidden(bitmap_layer_get_layer(s_time_digits_layers[0]), true);
    } else {
    	layer_set_hidden(bitmap_layer_get_layer(s_time_digits_layers[0]), false);
    }
  }
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  layer_set_hidden(bitmap_layer_get_layer(layer_quiettime), !quiet_time_is_active());

  update_display(tick_time);

	if (!quiet_time_is_active() && hourlyvibe_status && (units_changed & HOUR_UNIT)) {
	
	 	vibes_short_pulse();
    return;
    }   
}

/*
  Handle update in settings
*/

// validate upper limit (can not be higher, than 1)
// mind: value is unsigned, so it can not be less than 0
#define VALIDATE_BOOL(value) if (value > 1) return;

#define VALIDATE_MAXIMUM(name, value, max) \
if (value > (uint8_t)max) \
{ \
    APP_LOG( APP_LOG_LEVEL_ERROR, "%s boundary error: %u is not less or equal than %u", name, value, max ); \
    return; \
}

static void tuple_changed_callback( const uint32_t key, const Tuple* tuple_new, const Tuple* tuple_old, void* context )
{
  uint8_t value = tuple_new->value->uint8;

  // APP_LOG( APP_LOG_LEVEL_DEBUG, "tuple_changed_callback: %lu", key );

  switch ( key )
  {
    case SETTING_STATUS_KEY:
      VALIDATE_BOOL(value)

      persist_write_int( SETTING_STATUS_KEY, value );
      fill = value;
      break;

    case SETTING_LANGUAGE_KEY:
      VALIDATE_BOOL(value)

      persist_write_int( SETTING_LANGUAGE_KEY, value );
      current_language = value;
	  
 	  if (current_language) {
 	   health_service_events_unsubscribe();
	   layer_set_hidden(text_layer_get_layer(hr_label), true);
	   layer_set_hidden(bitmap_layer_get_layer(heart_layer), true);
      } else {
// subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }	        
		layer_set_hidden(text_layer_get_layer(hr_label), false);
	 	layer_set_hidden(bitmap_layer_get_layer(heart_layer), false);
      }  
	  break;

    case SETTING_ICON_KEY:
      VALIDATE_MAXIMUM("SETTING_ICON_KEY", value, ARRAY_LENGTH(WEATHER_ICONS))

      if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }
	  
	  switch (iconnum) {
		  
		case 0:
		      icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS3[tuple_new->value->uint8]);
		break;
		  
		case 1:
      		 icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS2[tuple_new->value->uint8]);
		break;	  
		  
		case 2:
      		 icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[tuple_new->value->uint8]);
		break;
		  
	  }
	  
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
      break;

    case SETTING_TEMPERATURE_KEY:
      text_layer_set_text(temp_layer, tuple_new->value->cstring);
      break;

    case BLUETOOTHVIBE_KEY:
      VALIDATE_BOOL(value)

      persist_write_int( BLUETOOTHVIBE_KEY, value );
      bluetoothvibe_status = value;
      break;

    case HOURLYVIBE_KEY:
      VALIDATE_BOOL(value)

      persist_write_int( HOURLYVIBE_KEY, value );
      hourlyvibe_status = value;
      break;

    case SECS_KEY:
      VALIDATE_BOOL(value)
		  
      persist_write_int( SECS_KEY, value );
      row1right = value;

  	  if (row1right) {
	   layer_set_hidden(bitmap_layer_get_layer(block_layer), false);
      } else {
	   layer_set_hidden(bitmap_layer_get_layer(block_layer), true);
      }	  
      break;
	
	case SETTING_FORMAT_KEY:
      VALIDATE_BOOL(value)

      persist_write_int( SETTING_FORMAT_KEY, value );
      row2left = value;
	
	  if (row2left) {
       layer_set_hidden(text_layer_get_layer(text_daynum_layer), true);
      } else {
       layer_set_hidden(text_layer_get_layer(text_daynum_layer), false);
      }
      break;
	  
	case SETTING_COLORSTATUS_KEY:
	  VALIDATE_BOOL(value)

      persist_write_int( SETTING_COLORSTATUS_KEY, value );
      color_status = value;

	  if (color_status) {
 	   health_service_events_unsubscribe();
       layer_set_hidden(text_layer_get_layer(steps_label), true);
       layer_set_hidden(bitmap_layer_get_layer(dateblank_layer), true);
      } else {
// subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }	
	   layer_set_hidden(text_layer_get_layer(steps_label), false);
       layer_set_hidden(bitmap_layer_get_layer(dateblank_layer), false);
      }
	  break;	  
	  
	case SETTING_WEATHERSTATUS_KEY:
      persist_write_int( SETTING_WEATHERSTATUS_KEY, value );
      weather_status = value;

	  if (weather_status) {
       layer_set_hidden(bitmap_layer_get_layer(icon_layer), true);
        layer_set_hidden(text_layer_get_layer(temp_layer), true);
      } else {
		layer_set_hidden(bitmap_layer_get_layer(icon_layer), false);
        layer_set_hidden(text_layer_get_layer(temp_layer), false);
      }
      break;

	case SETTING_LINES_KEY:
	  	  VALIDATE_BOOL(value)

      persist_write_int( SETTING_LINES_KEY, value );
      lines = value;

	  if (lines) {
       layer_set_hidden(bitmap_layer_get_layer(background_layer), true);
      } else {
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
      }
      break;
	  
	 case SETTING_HEALTH_KEY:
	  	  VALIDATE_BOOL(value)

      persist_write_int( SETTING_HEALTH_KEY, value );
      health = value;

	  if (health) {
       layer_set_hidden(bitmap_layer_get_layer(block2_layer), false);
      } else {
		layer_set_hidden(bitmap_layer_get_layer(block2_layer), true);
      }
      break;
	  
	case SETTING_ICONNUM_KEY:
      VALIDATE_MAXIMUM("SETTING_ICONNUM_KEY", value, FORMAT_END)

      persist_write_int( SETTING_ICONNUM_KEY, value );
      iconnum = value;
	  
      break;

	case BASIC_CENTER_KEY:
      VALIDATE_BOOL(value)
		  
      persist_write_int( BASIC_CENTER_KEY, value );
      basic_center = value;

	case INVERT_COLOR_KEY:
	  VALIDATE_BOOL(value)

      persist_write_int( INVERT_COLOR_KEY, value );
      invert = value;			  

	  
      set_invert_color(invert);
      break;
	  
	  break;
	  
	  default:
      APP_LOG(APP_LOG_LEVEL_INFO, "unknown tuple key: %lu", key);
  }

  // Refresh display
  // Get current time
  time_t now = time( NULL );
  struct tm *tick_time = localtime( &now );

  // Force update to avoid a blank screen at startup of the watchface
  handle_tick(tick_time, 0);
}

/*
  Handle errors
*/
static void app_error_callback( DictionaryResult dict_error, AppMessageResult app_message_error, void* context ) {
  APP_LOG( APP_LOG_LEVEL_ERROR, "app error: %d", app_message_error );
//  vibes_double_pulse();
}

static void main_window_load(Window *window) {
	
  Layer *window_layer = window_get_root_layer(s_main_window);
	
  window_set_background_color(window, GColorBlack);

  futura = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURAITALIC_16));
  futura2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURAITALIC_15));	

  GRect bounds = layer_get_bounds(window_layer);

//background lines
	
  background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(background_layer, background);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
	

//------------- heart rate and steps ------------//
	
// heart
  heart_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EMPTY);
#ifdef PBL_PLATFORM_BASALT
  GRect frame2 = GRect(98, 151, 17, 15);
#else
  GRect frame2 = GRect(40, 151, 17, 15);
#endif
  heart_layer = bitmap_layer_create(frame2);
  bitmap_layer_set_bitmap(heart_layer, heart_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(heart_layer));		

// shoe
  dateblank_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EMPTY2);
  GRect bitmap_bounds = gbitmap_get_bounds(dateblank_image);
#ifdef PBL_PLATFORM_BASALT
  GRect frame = GRect(1, 151, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame = GRect(65, 151, bitmap_bounds.size.w, bitmap_bounds.size.h);	
#endif
  dateblank_layer = bitmap_layer_create(frame);
  bitmap_layer_set_bitmap(dateblank_layer, dateblank_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(dateblank_layer));		

// Create Heart Rate Layer

#ifdef PBL_PLATFORM_BASALT
  hr_label = text_layer_create(GRect(120, 148, 50, 50));
#else
  hr_label = text_layer_create(GRect(0, 148, 37, 50));
#endif
  text_layer_set_background_color(hr_label, GColorClear);
  text_layer_set_text_color(hr_label, GColorWhite);
  text_layer_set_text_alignment(hr_label, GTextAlignmentRight);
  text_layer_set_font(hr_label, futura);
  text_layer_set_text(hr_label, "---");
  layer_add_child(window_layer, text_layer_get_layer(hr_label));
	
#ifdef PBL_PLATFORM_BASALT
  steps_label = text_layer_create(GRect(27, 148, 136, 22));	
  text_layer_set_background_color(steps_label, GColorBlack);
#else
  steps_label = text_layer_create(GRect(91, 148, 60, 22));
  text_layer_set_background_color(steps_label, GColorClear);
#endif
  text_layer_set_text_alignment(steps_label, GTextAlignmentLeft);
  text_layer_set_text_color(steps_label, GColorWhite);
  text_layer_set_font(steps_label, futura);	
  text_layer_set_text(steps_label, "-----");
  layer_add_child(window_layer, text_layer_get_layer(steps_label));	

//----- weather 1 -----//

	icon_layer = bitmap_layer_create(GRect( 1, 54,  25,  25 ));
    layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));
    
	temp_layer = text_layer_create(GRect( 1, 78,  70,  20 ));	
    text_layer_set_text_alignment(temp_layer, GTextAlignmentLeft);
    text_layer_set_font(temp_layer, futura2);
    text_layer_set_text_color(temp_layer, GColorWhite);
    text_layer_set_background_color(temp_layer, GColorClear);
//    text_layer_set_font(s_temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(temp_layer));

//------------- battery ------------//
	
  battery_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
  GRect bitmap_bounds_batt = gbitmap_get_bounds(battery_image);
  GRect frame_batt = GRect(1, 40, bitmap_bounds_batt.size.w, bitmap_bounds_batt.size.h);
  battery_layer = bitmap_layer_create(frame_batt);
  battery_image_layer = bitmap_layer_create(frame_batt);
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
  layer_set_update_proc(bitmap_layer_get_layer(battery_layer), battery_layer_update_callback);
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_image_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));
	
//------------- bluetooth ------------//

  img_bt_connect     = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTHON);
  img_bt_disconnect  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTHOFF);
	
  layer_conn_img  = bitmap_layer_create(GRect(4, 4, 13, 13));
  bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_conn_img)); 	

//-------- status block -------//
	
  block_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK);
  GRect frame3 = GRect(0, 0, 37, 50);
  block_layer = bitmap_layer_create(frame3);
  bitmap_layer_set_bitmap(block_layer, block_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(block_layer));		
	
//-------- health block -------//
	
  block2_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK2);
  GRect frame4 = GRect(0, 144, 144, 24);
  block2_layer = bitmap_layer_create(frame4);
  bitmap_layer_set_bitmap(block2_layer, block2_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(block2_layer));	
	
//------- time and qt -------//
	
  GRect dummy_frame = GRect(0, 0, 0, 0);
	
 for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
    s_time_digits_layers[i] = bitmap_layer_create(dummy_frame);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_time_digits_layers[i]));
  }
	
  bitmap_quiettime = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ICON_QT);
  layer_quiettime = bitmap_layer_create(GRect(5,20, 11,14));
  bitmap_layer_set_bitmap(layer_quiettime,bitmap_quiettime);
  layer_add_child(window_layer,bitmap_layer_get_layer(layer_quiettime));
	
//------- day/date --------//
	
  text_daynum_layer = text_layer_create(GRect(  1, 97,  40,  70 ));
  text_layer_set_text_alignment(text_daynum_layer, GTextAlignmentLeft);	
  text_layer_set_font(text_daynum_layer, futura2);
  text_layer_set_background_color(text_daynum_layer, GColorClear);
  text_layer_set_text_color(text_daynum_layer, GColorWhite );
  layer_add_child( window_layer, text_layer_get_layer( text_daynum_layer ) );
	
	
	
	// update the battery on launch
   bluetooth_connection_callback(bluetooth_connection_service_peek());
   update_battery_state(battery_state_service_peek());

}

static void main_window_unload(Window *window) {
	
for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(s_time_digits_layers[i]));
    gbitmap_destroy(s_time_digits[i]);
    bitmap_layer_destroy(s_time_digits_layers[i]);
	s_time_digits[i] = NULL;
	s_time_digits_layers[i] = NULL;
  }
	
  text_layer_destroy( text_daynum_layer );
  text_layer_destroy( steps_label );
  text_layer_destroy( hr_label );
  text_layer_destroy( temp_layer );
	
  fonts_unload_custom_font(futura);
  fonts_unload_custom_font(futura2);

  layer_remove_from_parent(bitmap_layer_get_layer(layer_quiettime));
  bitmap_layer_destroy(layer_quiettime);
  gbitmap_destroy(bitmap_quiettime);
	
  layer_remove_from_parent(bitmap_layer_get_layer(dateblank_layer));
  bitmap_layer_destroy(dateblank_layer);
  gbitmap_destroy(dateblank_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(heart_layer));
  bitmap_layer_destroy(heart_layer);
  gbitmap_destroy(heart_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(block_layer));
  bitmap_layer_destroy(block_layer);
  gbitmap_destroy(block_image);

  layer_remove_from_parent(bitmap_layer_get_layer(block2_layer));
  bitmap_layer_destroy(block2_layer);
  gbitmap_destroy(block2_image);

  layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  bitmap_layer_destroy(background_layer);
  gbitmap_destroy(background);
	
  layer_remove_from_parent(bitmap_layer_get_layer(icon_layer));
  bitmap_layer_destroy(icon_layer);
  gbitmap_destroy(icon_bitmap);
	
  layer_remove_from_parent(bitmap_layer_get_layer(layer_conn_img));
  bitmap_layer_destroy(layer_conn_img);
  gbitmap_destroy(img_bt_connect);
  gbitmap_destroy(img_bt_disconnect);

  layer_remove_from_parent(bitmap_layer_get_layer(battery_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(battery_image_layer));
  bitmap_layer_destroy(battery_layer);  
  bitmap_layer_destroy(battery_image_layer);  
  gbitmap_destroy(battery_image);
	
  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
}

static void init() {
		
// Read persistent data

#define GET_PERSIST_VALUE_OR_DEFAULT(key, def) \
  persist_exists( key ) ? persist_read_int( key ) : def

  // Read watchface settings from persistent data or use default values
  basic_center		   = GET_PERSIST_VALUE_OR_DEFAULT( BASIC_CENTER_KEY,          false);
  fill			       = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_STATUS_KEY,        false);
  weather_status       = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_WEATHERSTATUS_KEY, false);
  current_language     = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_LANGUAGE_KEY,      false);
  row1right		       = GET_PERSIST_VALUE_OR_DEFAULT( SECS_KEY,			      false);
  row2left		       = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_FORMAT_KEY,        false);
  bluetoothvibe_status = GET_PERSIST_VALUE_OR_DEFAULT( BLUETOOTHVIBE_KEY,         false);
  hourlyvibe_status    = GET_PERSIST_VALUE_OR_DEFAULT( HOURLYVIBE_KEY ,           false);
  color_status 		   = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_COLORSTATUS_KEY ,  false);
  lines		 		   = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_LINES_KEY ,  	  false);
  health		 	   = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_HEALTH_KEY ,  	  false);
  invert		 	   = GET_PERSIST_VALUE_OR_DEFAULT( INVERT_COLOR_KEY ,  	  false);
  iconnum		 	   = GET_PERSIST_VALUE_OR_DEFAULT( SETTING_ICONNUM_KEY ,  	  FORMAT_ICON1);
  
		
// Initial settings
  Tuplet initial_values[] = { TupletInteger( SETTING_STATUS_KEY, fill )
                            , TupletInteger( BASIC_CENTER_KEY, basic_center )
                            , TupletInteger( SETTING_WEATHERSTATUS_KEY, weather_status )
                            , TupletInteger( SETTING_LANGUAGE_KEY, current_language )
                            , TupletInteger( SETTING_FORMAT_KEY, row2left )
                            , TupletInteger( BLUETOOTHVIBE_KEY, bluetoothvibe_status )
                            , TupletInteger( HOURLYVIBE_KEY, hourlyvibe_status )
                            , TupletInteger( SECS_KEY, row1right )
                            , TupletInteger( SETTING_ICON_KEY, (uint8_t) 14)
                            , TupletCString( SETTING_TEMPERATURE_KEY, "---")
							, TupletInteger( SETTING_COLORSTATUS_KEY, color_status )
							, TupletInteger( SETTING_LINES_KEY, lines )
							, TupletInteger( SETTING_HEALTH_KEY, health )
							, TupletInteger( SETTING_ICONNUM_KEY, iconnum )
							, TupletInteger( INVERT_COLOR_KEY, invert )
                            };
	
	
// Open AppMessage to transfers
  app_message_open( 256, 256 );

// Initialize AppSync
  app_sync_init( &app, sync_buffer
               , sizeof( sync_buffer )
               , initial_values
               , ARRAY_LENGTH( initial_values )
               , tuple_changed_callback
               , app_error_callback
               , NULL
               );	
	
// international support
  setlocale(LC_ALL, "");
	
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
		
	
// Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  update_display(tick_time);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  bluetooth_connection_service_subscribe(toggle_bluetooth_icon);
  battery_state_service_subscribe(&update_battery_state);
	
// subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }	
}

static void deinit() {
  
  app_sync_deinit(&app);

  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  health_service_events_unsubscribe();
	
  if (effect_layer != NULL) {
	  effect_layer_destroy(effect_layer);
  }
  // Destroy window
  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
	
	window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
