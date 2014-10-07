#include <pebble.h>

//Initiating ui components
static Window *s_main_window;
static TextLayer *s_x_layer;
static TextLayer *s_y_layer;
static TextLayer *s_z_layer;

static DictionaryIterator *iter;

static int16_t accel_last[3];
static uint64_t time_last[3];
static int max = 0;
static int state = 0;
static time_t rel_time;

//Message functions
void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
}
void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
}
void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
}
 
//Accel functions
static int update_rate(int16_t accel_cur, uint64_t time_cur, int16_t key){
  int da_dt = (accel_cur - accel_last[key])/((time_cur - time_last[key]) / 100);
  time_last[key] = time_cur;
  accel_last[key] = accel_cur;
  return da_dt;
}

//Window functions
static void main_window_load(Window *window) {
  s_x_layer = text_layer_create(GRect(0, 5, 144, 50));
  s_y_layer = text_layer_create(GRect(0, 55, 144, 50));
  s_z_layer = text_layer_create(GRect(0, 105, 144, 50));
  
  text_layer_set_background_color(s_x_layer, GColorClear);
  text_layer_set_text_color(s_x_layer, GColorBlack);
  text_layer_set_background_color(s_y_layer, GColorClear);
  text_layer_set_text_color(s_y_layer, GColorBlack);
  text_layer_set_background_color(s_z_layer, GColorClear);
  text_layer_set_text_color(s_z_layer, GColorBlack);
  
  text_layer_set_font(s_x_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_x_layer, GTextAlignmentCenter);
  text_layer_set_font(s_y_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_y_layer, GTextAlignmentCenter);
  text_layer_set_font(s_z_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_z_layer, GTextAlignmentCenter);
  
  text_layer_set_text(s_x_layer, "");
  text_layer_set_text(s_y_layer, "TurnIt");
  text_layer_set_text(s_z_layer, "");
  
  for(int i = 0; i < 3; i++){
    accel_last[i] = 0;
    time_last[i] = 0;
  }
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_x_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_y_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_z_layer));
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_x_layer);
  text_layer_destroy(s_y_layer);
  text_layer_destroy(s_z_layer);
}

//Accelerometer event handler
void accel_data_handler(AccelData *data, uint32_t num_samples){
  /*static char accel_x_data_string[6];
  static char accel_y_data_string[6];
  static char accel_z_data_string[6];
  snprintf(accel_x_data_string, 6, "%d", update_rate(data->x, data->timestamp, 0));
  snprintf(accel_y_data_string, 6, "%d", update_rate(data->y, data->timestamp, 1));
  snprintf(accel_z_data_string, 6, "%d", update_rate(data->z, data->timestamp, 2));
  text_layer_set_text(s_x_layer, accel_x_data_string);
  text_layer_set_text(s_y_layer, accel_y_data_string);
  text_layer_set_text(s_z_layer, accel_z_data_string);
  if(abs(update_rate(data->x, data->timestamp, 0)) > 1000){
    text_layer_set_text(s_y_layer, "RIGHT");
  } else if(abs(update_rate(data->y, data->timestamp, 1)) > 1000){
    text_layer_set_text(s_y_layer, "LEFT");
  } else if(abs(update_rate(data->z, data->timestamp, 2)) > 1000){
    text_layer_set_text(s_y_layer, "STOP");
  }
  int sum = abs(update_rate(data->x, data->timestamp, 0)) + abs(update_rate(data->y, data->timestamp, 1)) + abs(update_rate(data->z, data->timestamp, 2));
  static char accel_y_data_string[6];
  snprintf(accel_y_data_string, 6, "%d", sum);
  text_layer_set_text(s_y_layer, accel_y_data_string);
  if(sum > 2000){
    text_layer_set_text(s_x_layer, "Ping!");
  }else{
    text_layer_set_text(s_x_layer, "");
  }*/
  int x_cur = update_rate(data->x, data->timestamp, 0);
  int y_cur = update_rate(data->y, data->timestamp, 1);
  int z_cur = update_rate(data->z, data->timestamp, 2);
  int sum = abs(x_cur) + abs(y_cur) + abs(z_cur);
  static char accel_x_data_string[6];
  static char accel_y_data_string[6];
  static char accel_z_data_string[6];
  //snprintf(accel_x_data_string, 6, "%d", x_cur);
  //snprintf(accel_z_data_string, 6, "%d", y_cur);
  //text_layer_set_text(s_x_layer, accel_x_data_string);
  //text_layer_set_text(s_z_layer, accel_z_data_string);
  if(state == 0 && sum >= 600){
    rel_time = time(NULL);
    state = 1;
  } else if(state == 1 && time(NULL) - rel_time >= 1){
    if(data->y < -800){
      //text_layer_set_text(s_y_layer, "RIGHT");
      state = 2;
      rel_time = time(NULL);
      dict_write_cstring(iter, 1, "1");
      app_message_outbox_send();
      vibes_short_pulse();
    } else if(data->x > 800){
      //text_layer_set_text(s_y_layer, "LEFT");
      state = 2;
      rel_time = time(NULL);
      dict_write_cstring(iter, 1, "2");
      app_message_outbox_send();
      vibes_short_pulse();
    } else if(data->x < -800){
      //text_layer_set_text(s_y_layer, "STOP");
      state = 2;
      rel_time = time(NULL);
      dict_write_cstring(iter, 1, "3");
      app_message_outbox_send();
      vibes_short_pulse();
    } else {
      state = 0;
    }
  }else if(state == 2 && time(NULL) - rel_time >= 5){
    //text_layer_set_text(s_y_layer, "");
    state = 0;
    dict_write_cstring(iter, 1, "0");
    app_message_outbox_send();
  }
}

static void init(){
  rel_time = time(NULL);
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  accel_data_service_subscribe(1, accel_data_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, 1, "0");
  app_message_outbox_send();
}
  
  
static void deinit(){
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}