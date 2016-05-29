#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_battery_layer;

static void update_time() {

	//Get a tm structure
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into a buffer
	static char time_text[10];
	strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H:%M:%S" : "%I:%M:%S", tick_time);

	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, time_text);

}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void handle_battery(BatteryChargeState charge_state) {
	static char battery_text[] = "100% charged";

	// Change battery text layer if in charge, otherwise change battery percentage
	if (charge_state.is_charging) {
		snprintf(battery_text, sizeof(battery_text), "charging");
	}
	else {
		snprintf(battery_text, sizeof(battery_text), "%d%% charged", charge_state.charge_percent);
	}

	// Update battery layer
	text_layer_set_text(s_battery_layer, battery_text);
}

static void setup_time_layers() {

	// Get information about the Window
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_time_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

	// Improve the layout to be more like a watchface
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_text(s_time_layer, "--:--:--");
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

	// Update initial time
	update_time();
}

static void setup_date_layers() {}

static void setup_battery_layer() {

	// Get information about the window
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_battery_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(88, 82), bounds.size.w, 80));

	// Style the TextLayer
	text_layer_set_background_color(s_battery_layer, GColorClear);
	text_layer_set_text_color(s_battery_layer, GColorWhite);
	text_layer_set_text(s_battery_layer, "100% charged");
	text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);

	// Add it as child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

	// Update initial battery
	handle_battery(battery_state_service_peek());
}

static void main_window_load(Window *window) {

	/**
	 * Set Window background color
	 * https://developer.pebble.com/guides/tools-and-resources/color-picker/
	 */
	window_set_background_color(window, GColorSunsetOrange);

	// Setup time/date/battery layers
	setup_time_layers();
	setup_date_layers();
	setup_battery_layer();

	// Register TickTimerService
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

	// Register BatteryStateService
	battery_state_service_subscribe(handle_battery);
}

static void main_window_unload(Window *window) {

	// Unregister TickTimerService
	tick_timer_service_unsubscribe();

	// Unregister BatteryStateService
	battery_state_service_unsubscribe();

	// Destroy TextLayers
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_battery_layer);
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