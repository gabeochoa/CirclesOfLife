#pragma once

#include <pebble.h>
#include <math.h>
	
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
