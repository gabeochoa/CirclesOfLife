#include <pebble.h>
#include <math.h>
#include "CircleOfLife.h"

Window *window;
Layer *minute_display_layer;
Layer *second_display_layer;
Layer *hour_display_layer;
Layer *ball_layer;

struct Ball
{
	int x;
	int y;
}inst;

const GPathInfo SECOND_SEGMENT_PATH_POINTS = {
	3,
	(GPoint []) {
	{0, 0},
	{-4, -35}, // 80 = radius + fudge; 8 = 80*tan(6 degrees); 6 degrees per second;
	{4,  -35},
	}
};

static GPath *second_segment_path;

const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
	3,
	(GPoint []) {
	{0, 0},
	{-5, -50}, // 80 = radius + fudge; 8 = 80*tan(6 degrees); 6 degrees per minute;
	{5,  -50},
	}
};

static GPath *minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
	3,
	(GPoint []) {
	{0, 0},
	{-18, -70}, // 50 = radius + fudge; 13 = 50*tan(15 degrees); 30 degrees per hour;
	{18,  -70},
	}
};

static GPath *hour_segment_path;

static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) 
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	unsigned int angle = (t->tm_hour % 12) * 30;
	
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_circle(ctx, center, 64);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_circle(ctx, center, 64);

	for (unsigned int i = 0; i < 360; i+=15) 
	{
		if ((i != angle) && (i != (angle + 15)) && (i != ((angle-15+360) % 360)) ) 
		{
			
		}
		else
		{
			gpath_rotate_to(hour_segment_path, (TRIG_MAX_ANGLE / 360) * i);
			graphics_context_set_fill_color(ctx, GColorBlack);
			gpath_draw_filled(ctx, hour_segment_path);
		}
	}

}

static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) 
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	unsigned int angle = (t->tm_min % 60) * 6;
	
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_circle(ctx, center, 48);
	
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_circle(ctx, center, 48);
	
	for (unsigned int i = 0; i < 360; i+=6) 
	{
		if ((i != angle) && (i != (angle + 6)) && (i != ((angle-6+360) % 360)) ) 
		{
			
		}
		else
		{
			gpath_rotate_to(minute_segment_path, (TRIG_MAX_ANGLE / 360) * i);
			graphics_context_set_fill_color(ctx, GColorBlack);
			gpath_draw_filled(ctx, minute_segment_path);
		}
	}
}

static void second_display_layer_update_callback(Layer *layer, GContext* ctx) 
{
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	unsigned int angle = (t->tm_sec*6)%360;
	
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_circle(ctx, center, 32);
	
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_circle(ctx, center, 32);

	graphics_context_set_stroke_color(ctx, GColorWhite);
/*
	char ar[30];
	snprintf(ar, 30, "%d", angle);

	graphics_draw_text(ctx, ar,
		fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
		(GRect){ .origin = GPoint(10, 5), .size = layer_get_frame(layer).size },
		GTextOverflowModeWordWrap,
		GTextAlignmentLeft,
			NULL);
		*/
//	GPoint second = GPoint(3*cos_lookup(angle),3*sin_lookup(angle));


//	graphics_draw_line(ctx, center, second);

	for (unsigned int i = 0; i < 360; i+=6) 
	{
		if ((i != angle) && (i != (angle + 6)) && (i != ((angle-6+360) % 360)) ) 
		{
			//gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * i);
			//graphics_context_set_fill_color(ctx, GColorWhite);
			//gpath_draw_filled(ctx, second_segment_path);
		}
		else
		{
			gpath_rotate_to(second_segment_path, (TRIG_MAX_ANGLE / 360) * i);
			graphics_context_set_fill_color(ctx, GColorBlack);
			gpath_draw_filled(ctx, second_segment_path);
		}
	}
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {

	layer_mark_dirty(second_display_layer);
	layer_mark_dirty(minute_display_layer);
	layer_mark_dirty(hour_display_layer);
}

// Long lived buffer
static char s_buffer[128];

static void data_handler(AccelData *data, uint32_t num_samples) {
		 
	// Compose string of all data for 3 samples
	snprintf(s_buffer, sizeof(s_buffer), 
		"N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %d,%d,%d", 
    	data[0].x, data[0].y, data[0].z, 
   		data[1].x, data[1].y, data[1].z, 
    	data[2].x, data[2].y, data[2].z
		);
}

static void ball_mvt_update(Layer *layer, GContext* ctx) 
{
	/*
	graphics_draw_text(ctx, s_buffer,
		fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
		(GRect){ .origin = GPoint(10, 5), .size = layer_get_frame(layer).size },
		GTextOverflowModeWordWrap,
		GTextAlignmentLeft,
			NULL);
			*/
}




static void init(void) {	
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_stack_push(window, true);

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	// Init the layer for the hour display
	hour_display_layer = layer_create(bounds);
	layer_set_update_proc(hour_display_layer, hour_display_layer_update_callback);
	layer_add_child(window_layer, hour_display_layer);
	// Init the hour segment path
	hour_segment_path = gpath_create(&HOUR_SEGMENT_PATH_POINTS);
	gpath_move_to(hour_segment_path, grect_center_point(&bounds));
	
	// Init the layer for the minute display
	minute_display_layer = layer_create(bounds);
	layer_set_update_proc(minute_display_layer, minute_display_layer_update_callback);
	layer_add_child(window_layer, minute_display_layer);
	// Init the minute segment path
	minute_segment_path = gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
	gpath_move_to(minute_segment_path, grect_center_point(&bounds));

	// Init the layer for the second display
	second_display_layer = layer_create(bounds);
	layer_set_update_proc(second_display_layer, second_display_layer_update_callback);
	layer_add_child(window_layer, second_display_layer);
	// Init the second segment path
	second_segment_path = gpath_create(&SECOND_SEGMENT_PATH_POINTS);
	gpath_move_to(second_segment_path, grect_center_point(&bounds));
	
	ball_layer = layer_create(bounds);
	layer_set_update_proc(ball_layer, ball_mvt_update);
	layer_add_child(window_layer, ball_layer);
	
	
	uint32_t num_samples = 3;
	accel_data_service_subscribe(num_samples, data_handler);
	
	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void deinit(void) {
	gpath_destroy(second_segment_path);
	gpath_destroy(minute_segment_path);
	gpath_destroy(hour_segment_path);

	tick_timer_service_unsubscribe();
	accel_tap_service_unsubscribe();
	
	window_destroy(window);
	layer_destroy(second_display_layer);
	layer_destroy(minute_display_layer);
	layer_destroy(hour_display_layer);
	layer_destroy(ball_layer);
	
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}