#include <pebble.h>

static Window *s_window;
static TextLayer *s_text_layer;
static TextLayer *cond_text_layer;
static AppTimer *timer;

static void load_weather_and_set_text() {
  //app_message_outbox_send();
  //text_layer_set_text(s_text_layer, "-20.0");

  if (timer) {
    app_timer_cancel(timer);
    timer = NULL;
  }

  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
    int value = 0;

  // Add an item to ask for weather data
  dict_write_int(out_iter, MESSAGE_KEY_weather, &value, sizeof(int), true);
  app_message_outbox_send();
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
 
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

 text_layer_set_text(s_text_layer, "Refreshing...");
  load_weather_and_set_text();
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(s_text_layer, "Down");
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 58, bounds.size.w, 40));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  text_layer_set_text(s_text_layer, "Loading...");

  cond_text_layer = text_layer_create(GRect(0, 105, bounds.size.w, 70));
  text_layer_set_overflow_mode(cond_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(cond_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(cond_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(cond_text_layer));

  //load_weather_and_set_text();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void timer_callback(void *data) {
  window_stack_pop_all(true);
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *weather_tuple = dict_find(iter, MESSAGE_KEY_weather);
  char *weather = weather_tuple->value->cstring;
  
  Tuple *cond_tuple = dict_find(iter, MESSAGE_KEY_conditions);
  char *cond = cond_tuple->value->cstring;

  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));

  text_layer_set_text(s_text_layer, weather);

  text_layer_set_text(cond_text_layer, cond);

  timer = app_timer_register(4000, timer_callback, NULL);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  
  app_message_register_inbox_received(inbox_received);
  app_message_open(128, 128);

  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
