#include <pebble.h>
#include <main.h>

#define KEY_COLOR_BACKGROUND  0
#define KEY_COLOR_TIME        1
#define KEY_COLOR_DATE        2
#define KEY_COLOR_HOUR_HAND   3
#define KEY_COLOR_MINUTE_HAND 4
#define KEY_COLOR_SECOND_HAND 5
#define KEY_SHOW_HANDS        6
#define KEY_SHOW_SECOND_HAND  7
#define KEY_SHOW_TIME         8
#define KEY_SHOW_DATE         9

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_bluetooth_layer;

static Layer *s_hands_layer;

static GPath *s_hour_arrow;
static GPath *s_minute_arrow;

static bool b_show_hands       = true;
static bool b_show_second_hand = true;
static bool b_show_time        = true;
static bool b_show_date 	   = true;
static bool b_show_battery     = true;
static bool b_show_bluetooth   = true;

static GColor gcolor_background;
static GColor gcolor_time;
static GColor gcolor_date;
static GColor gcolor_hour_hand;
static GColor gcolor_minute_hand;
static GColor gcolor_second_hand;

static void load_persisted_values() {

	// Background Color
	if (persist_exists(KEY_COLOR_BACKGROUND)) {
		int color_hex = persist_read_int(KEY_COLOR_BACKGROUND);
		gcolor_background = GColorFromHEX(color_hex);
	}
	else {
		gcolor_background = GColorBlack;
	}

	// Time Text Color
	if (persist_exists(KEY_COLOR_TIME)) {
		int color_hex = persist_read_int(KEY_COLOR_TIME);
		gcolor_time = GColorFromHEX(color_hex);
	}
	else {
		gcolor_time = GColorWhite;
	}

	// Date Text Color
	if (persist_exists(KEY_COLOR_DATE)) {
		int color_hex = persist_read_int(KEY_COLOR_DATE);
		gcolor_date = GColorFromHEX(color_hex);
	}
	else {
		gcolor_date = GColorWhite;
	}

	// Hour Hand Color
	if (persist_exists(KEY_COLOR_HOUR_HAND)) {
		int color_hex = persist_read_int(KEY_COLOR_HOUR_HAND);
		gcolor_hour_hand = GColorFromHEX(color_hex);
	}
	else {
		gcolor_hour_hand = GColorRed;
	}

	// Minute Hand Color
	if (persist_exists(KEY_COLOR_MINUTE_HAND)) {
		int color_hex = persist_read_int(KEY_COLOR_MINUTE_HAND);
		gcolor_minute_hand = GColorFromHEX(color_hex);
	}
	else {
		gcolor_minute_hand = GColorRed;
	}

	// Second Hand Color
	if (persist_exists(KEY_COLOR_SECOND_HAND)) {
		int color_hex = persist_read_int(KEY_COLOR_SECOND_HAND);
		gcolor_second_hand = GColorFromHEX(color_hex);
	}
	else {
		gcolor_second_hand = GColorRed;
	}
}

static void update_time() {

	//Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into a buffer
	static char time_text[10];
	strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, time_text);
}
static void update_date() {

	// Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current date into a buffer
	static char date_text[16];
	strftime(date_text, sizeof(date_text), "%a %d %b", tick_time);

	// Display this date on the TextLayer
	text_layer_set_text(s_date_layer, date_text);
}

static void update_hands_proc(Layer *layer, GContext *ctx) {

	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	int16_t second_hand_length = bounds.size.w / 2 - 5;
	int16_t second_hand_tail_length = 15;

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
	int32_t second_angle_tail = TRIG_MAX_ANGLE * (t->tm_sec + 30) / 60;

	GPoint second_hand = {
		.x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
		.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
	};
	GPoint second_hand_tail = {
		.x = (int16_t)(sin_lookup(second_angle_tail) * (int32_t)second_hand_tail_length / TRIG_MAX_RATIO) + center.x,
		.y = (int16_t)(-cos_lookup(second_angle_tail) * (int32_t)second_hand_tail_length / TRIG_MAX_RATIO) + center.y,
	};

	if (b_show_hands) {
		// Minute hand
		graphics_context_set_fill_color(ctx, gcolor_minute_hand);
		gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
		gpath_draw_filled(ctx, s_minute_arrow);

		// Hour hand
		graphics_context_set_fill_color(ctx, gcolor_hour_hand);
		gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
		gpath_draw_filled(ctx, s_hour_arrow);

		// Second hand
		if (b_show_second_hand) {
			graphics_context_set_stroke_color(ctx, gcolor_second_hand);
			graphics_draw_line(ctx, second_hand, center);
			graphics_draw_line(ctx, second_hand_tail, center);
		}

		// Draw the dot in the middle
		graphics_context_set_fill_color(ctx, GColorRed);
		graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), 4);

		// Draw the dot stroke
		graphics_context_set_stroke_color(ctx, GColorBlack);
		graphics_context_set_stroke_width(ctx, 2);
		graphics_draw_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), 6);
	}
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	update_date();
	layer_mark_dirty(window_get_root_layer(s_main_window));
}

static void handle_battery(BatteryChargeState charge_state) {
	static char battery_text[] = "100% charged";

	// Change battery text layer if in charge, otherwise change battery percentage
	if (charge_state.is_charging) {
		snprintf(battery_text, sizeof(battery_text), "Charging");

		text_layer_set_text_color(s_battery_layer, GColorLightGray);
	}
	else {
		snprintf(battery_text, sizeof(battery_text), "%d%% Charged", charge_state.charge_percent);

		// If battery percent is below 20 set text color to red
		if (charge_state.charge_percent <= 20) {
			text_layer_set_text_color(s_battery_layer, GColorRed);
		}
		else {
			text_layer_set_text_color(s_battery_layer, GColorLightGray);
		}
	}

	// Update battery layer
	text_layer_set_text(s_battery_layer, battery_text);
}

static void handle_bluetooth(bool connected) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth %sconnected", connected ? "" : "dis");
}

static void setup_window_background() {

	// Set Window background color
	window_set_background_color(s_main_window, gcolor_background);
}

static void setup_time_layers() {

	// Get information about the Window
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_time_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(30, 24), bounds.size.w, 26));

	// Improve the layout to be more like a watchface
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, gcolor_time);
	text_layer_set_text(s_time_layer, "--:--");
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

	// Update initial time
	update_time();
}

static void setup_date_layers() {

	//Get window information
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds =layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_date_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(106, 100), bounds.size.w, 16));

	// Style the TextLayer
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, gcolor_date);
	text_layer_set_text(s_date_layer, "--");
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	// Add it as child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

	// Update initial date
	update_date();
}

static void setup_battery_layer() {

	// Get information about the window
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_battery_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(122, 116), bounds.size.w, 16));

	// Style the TextLayer
	text_layer_set_background_color(s_battery_layer, GColorClear);
	text_layer_set_text_color(s_battery_layer, GColorLightGray);
	text_layer_set_text(s_battery_layer, "100% Charged");
	text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);

	// Add it as child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

	// Update initial battery
	handle_battery(battery_state_service_peek());
}

static void setup_bluetooth_layer() {

	// Get information about the window
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_bluetooth_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(138, 132), bounds.size.w, 16));

	// Style the TextLayer
	text_layer_set_background_color(s_bluetooth_layer, GColorClear);
	text_layer_set_text_color(s_bluetooth_layer, GColorLightGray);
	text_layer_set_text(s_bluetooth_layer, "Bluetooth Connected");
	text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentCenter);

	// Add it as child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_bluetooth_layer));

	// Update initial battery
	handle_bluetooth(connection_service_peek_pebble_app_connection());
}

static void setup_hands_layer() {

	// Get information about the window
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);
	GPoint center = grect_center_point(&bounds);

	// Create hour arrow
	s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
	gpath_move_to(s_hour_arrow, center);

	// Create minute arrow
	s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
	gpath_move_to(s_minute_arrow, center);

	// Create the Layer with specific bounds
	s_hands_layer = layer_create(bounds);
	layer_set_update_proc(s_hands_layer, update_hands_proc);
	layer_add_child(window_layer, s_hands_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {

	Tuple *color_background_t = dict_find(iter, KEY_COLOR_BACKGROUND);
	if (color_background_t) {
		gcolor_background = GColorFromHEX(color_background_t->value->int32);
		setup_window_background();
		persist_write_int(KEY_COLOR_BACKGROUND, color_background_t->value->int32);
	}

	Tuple *color_time_t = dict_find(iter, KEY_COLOR_TIME);
	if (color_time_t) {
		gcolor_time = GColorFromHEX(color_time_t->value->int32);
		persist_write_int(KEY_COLOR_TIME, color_time_t->value->int32);
	}

	Tuple *color_date_t = dict_find(iter, KEY_COLOR_DATE);
	if (color_date_t) {
		gcolor_date = GColorFromHEX(color_date_t->value->int32);
		persist_write_int(KEY_COLOR_DATE, color_date_t->value->int32);
	}

	Tuple *color_hour_hand_t = dict_find(iter, KEY_COLOR_HOUR_HAND);
	if (color_hour_hand_t) {
		gcolor_hour_hand = GColorFromHEX(color_hour_hand_t->value->int32);
		persist_write_int(KEY_COLOR_HOUR_HAND, color_hour_hand_t->value->int32);
	}

	Tuple *color_minute_hand_t = dict_find(iter, KEY_COLOR_MINUTE_HAND);
	if (color_minute_hand_t) {
		gcolor_minute_hand = GColorFromHEX(color_minute_hand_t->value->int32);
		persist_write_int(KEY_COLOR_MINUTE_HAND, color_minute_hand_t->value->int32);
	}

	Tuple *color_second_t = dict_find(iter, KEY_COLOR_SECOND_HAND);
	if (color_second_t) {
		gcolor_second_hand = GColorFromHEX(color_second_t->value->int32);
		persist_write_int(KEY_COLOR_SECOND_HAND, color_second_t->value->int32);
	}


	Tuple *show_hands_t = dict_find(iter, KEY_SHOW_HANDS);
	if (show_hands_t) {
		b_show_hands = show_hands_t->value->uint8;
		persist_write_int(KEY_SHOW_HANDS, b_show_hands);
	}

	Tuple *show_second_hand_t = dict_find(iter, KEY_SHOW_SECOND_HAND);
	if (show_second_hand_t) {
		b_show_second_hand = show_second_hand_t->value->uint8;
		persist_write_int(KEY_SHOW_SECOND_HAND, b_show_second_hand);
	}

	Tuple *show_time_t = dict_find(iter, KEY_SHOW_TIME);
	if (show_time_t) {
		b_show_time = show_time_t->value->uint8;
		persist_write_int(KEY_SHOW_HANDS, b_show_time);
	}

	Tuple *show_date_t = dict_find(iter, KEY_SHOW_DATE);
	if (show_date_t) {
		b_show_date = show_date_t->value->uint8;
		persist_write_int(KEY_SHOW_DATE, b_show_date);
	}
}

static void main_window_load(Window *window) {

	// Load persisted values from localstorage
	load_persisted_values();

	// Setup background color
	setup_window_background();

	// Setup layers
	setup_time_layers();
	setup_date_layers();
	if (b_show_battery) setup_battery_layer();
	if (b_show_bluetooth) setup_bluetooth_layer();
	if (b_show_hands) setup_hands_layer();

	// Register TickTimerService
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

	// Register BatteryStateService
	if (b_show_battery) battery_state_service_subscribe(handle_battery);

	//Register ConnectionService
	if (b_show_bluetooth) {
		connection_service_subscribe((ConnectionHandlers) {
			.pebble_app_connection_handler = handle_bluetooth
		});
	}

	// Register Inbox Handler
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(128, 128);
}

static void main_window_unload(Window *window) {

	// Unregister TickTimerService
	tick_timer_service_unsubscribe();

	// Unregister BatteryStateService
	if (b_show_battery) battery_state_service_unsubscribe();

	// Unregister ConnectionService
	if (b_show_bluetooth) bluetooth_connection_service_unsubscribe();

	// Destroy TextLayers
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	if (b_show_battery) text_layer_destroy(s_battery_layer);
	if (b_show_bluetooth) text_layer_destroy(s_bluetooth_layer);

	// Destroy GPaths
	if (b_show_hands) gpath_destroy(s_hour_arrow);
	if (b_show_hands) gpath_destroy(s_minute_arrow);

	// Destroy Layers
	if (b_show_hands) layer_destroy(s_hands_layer);
}

static void init() {

	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);
}

static void deinit() {

	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}