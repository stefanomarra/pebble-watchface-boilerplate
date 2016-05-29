#include <pebble.h>

#ifdef PBL_RECT

static const GPathInfo MINUTE_HAND_POINTS = {
	8,
	(GPoint []) {
		{ -3, -1},
		{ 3, -1 },
		{ 3, -62 },
		{ 2, -63 },
		{ 1, -64 },
		{ -1, -64 },
		{ -2, -63 },
		{ -3, -62 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	8, (GPoint []){
		{ -4, -1 },
		{ 4, -1 },
		{ 4, -45 },
		{ 3, -46 },
		{ 2, -48 },
		{ -2, -48 },
		{ -3, -46 },
		{ -4, -45 }
	}
};
#else

static const GPathInfo MINUTE_HAND_POINTS = {
	8,
	(GPoint []) {
		{ -3, -1},
		{ 3, -1 },
		{ 3, -72 },
		{ 2, -73 },
		{ 1, -74 },
		{ -1, -74 },
		{ -2, -73 },
		{ -3, -72 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	8, (GPoint []){
		{ -4, -1 },
		{ 4, -1 },
		{ 4, -55 },
		{ 3, -56 },
		{ 2, -58 },
		{ -2, -58 },
		{ -3, -56 },
		{ -4, -55 }
	}
};
#endif