#include <pebble.h>

#ifdef PBL_RECT

static const GPathInfo MINUTE_HAND_POINTS = {
	7,
	(GPoint []) {
		{ -3, -1},
		{ 3, -1 },
		{ 3, -62 },
		{ 2, -63 },
		{ 0, -64 },
		{ -2, -63 },
		{ -3, -62 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	7,
	(GPoint []){
		{ -4, -1 },
		{ 4, -1 },
		{ 4, -45 },
		{ 3, -46 },
		{ 0, -47 },
		{ -3, -46 },
		{ -4, -45 }
	}
};
#else

static const GPathInfo MINUTE_HAND_POINTS = {
	7,
	(GPoint []) {
		{ -3, -1},
		{ 3, -1 },
		{ 3, -72 },
		{ 2, -73 },
		{ 0, -74 },
		{ -2, -73 },
		{ -3, -72 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	7,
	(GPoint []){
		{ -4, -1 },
		{ 4, -1 },
		{ 4, -55 },
		{ 3, -56 },
		{ 0, -58 },
		{ -3, -56 },
		{ -4, -55 }
	}
};
#endif