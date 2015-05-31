#include <pebble.h>

static Window *s_main_window;
TextLayer *s_time_layer;
TextLayer *s_status_layer;
static GFont s_time_font;
static GFont s_status_font;
static int hour;
static int day;

static struct tm *tick_time;
static struct tm *t;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  tick_time = localtime(&temp);
  
  // Create a long-lived buffer
  static char time_text [] = "00:00";
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //use 24 hour format
    strftime(time_text, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(time_text, sizeof("00:00"), "%I:%M", tick_time);
    }
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, time_text);
}

static void update_status() {
  time_t temp = time(NULL);
  tick_time = localtime(&temp);
  
  day = ((int)(t->tm_wday));  
  hour = ((int)(t->tm_hour));
  
  static char open_status[8];
  if(day > 0 && day < 5) {
    if(9 <= hour && hour <= 17) {
      strftime(open_status, sizeof(open_status), "OPEN", tick_time);
      } else {
      strftime(open_status, sizeof(open_status), "CLOSED", tick_time);
      } 
  } else if(day == 5) {
    if(9 <= hour && hour <= 18) {
      strftime(open_status, sizeof(open_status), "OPEN", tick_time);
      } else {
      strftime(open_status, sizeof(open_status), "CLOSED", tick_time);
      }
    } else if(day == 6) {
    if(9 <= hour && hour <= 13) {
      strftime(open_status, sizeof(open_status), "OPEN", tick_time);
      } else {
      strftime(open_status, sizeof(open_status), "CLOSED", tick_time);
      }
    } else {
    strftime(open_status, sizeof(open_status), "CLOSED", tick_time);
    }
  text_layer_set_text(s_status_layer, open_status);
}


static void main_window_load(Window *window) {
  //Create GBitmap, set to BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_BOLD_32));
  s_status_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_24));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 102, 134, 38));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Create status TextLayer
  s_status_layer = text_layer_create(GRect(5, 140, 134, 24));
  text_layer_set_background_color(s_status_layer, GColorClear);
  text_layer_set_text_color(s_status_layer, GColorWhite);
  text_layer_set_font(s_status_layer, s_status_font);
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_text(s_status_layer, "OPEN");
  

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_status_layer));
  
  update_time();
  update_status();
}

static void main_window_unload(Window *window) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  //unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_status_font);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_status_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
   // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
     
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
}

static void deinit() {
   window_destroy(s_main_window);
}

int main (void) {
  init();
  app_event_loop();
  deinit();
}