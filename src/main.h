#include <pebble.h>

#ifdef PBL_RECT

static const GPathInfo MINUTE_HAND_POINTS = {
	9,
	(GPoint []) {
		{ -2, 0},
		{ 2, 0 },
		{ 2, -20 },
		{ 4, -20 },
		{ 4, -60 },
		{ 0, -64 },
		{ -4, -60 },
		{ -4, -20 },
		{ -2, -20 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	9, (GPoint []){
		{ -2, 0 },
		{ 2, 0 },
		{ 2, -15 },
		{ 4, -15 },
		{ 4, -44 },
		{ 0, -48 },
		{ -4, -44 },
		{ -4, -15 },
		{ -2, -15 }
	}
};
#else

static const GPathInfo MINUTE_HAND_POINTS = {
	9,
	(GPoint []) {
		{ -2, 0},
		{ 2, 0 },
		{ 2, -25 },
		{ 4, -25 },
		{ 4, -75 },
		{ 0, -79 },
		{ -4, -75 },
		{ -4, -25 },
		{ -2, -25 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	9, (GPoint []){
		{ -2, 0 },
		{ 2, 0 },
		{ 2, -20 },
		{ 4, -20 },
		{ 4, -59 },
		{ 0, -63 },
		{ -4, -59 },
		{ -4, -20 },
		{ -2, -20 }
	}
};
#endif