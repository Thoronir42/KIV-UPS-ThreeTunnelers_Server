#include "direction.h"

void directions_initialise() {
	// -1 on X (West)
	G_DIRECTIONS[DIRECTION_NW].x = G_DIRECTIONS[DIRECTION_W].x = G_DIRECTIONS[DIRECTION_SW].x = -1;
	//  0 on X (neutral)
	G_DIRECTIONS[DIRECTION_N].x  = G_DIRECTIONS[DIRECTION_0].x = G_DIRECTIONS[DIRECTION_S].x  =  0;
	// +1 on X (East)
	G_DIRECTIONS[DIRECTION_NE].x = G_DIRECTIONS[DIRECTION_E].x = G_DIRECTIONS[DIRECTION_SE].x = +1;

	// -1 on Y (North)
	G_DIRECTIONS[DIRECTION_NW].y = G_DIRECTIONS[DIRECTION_N].y = G_DIRECTIONS[DIRECTION_NE].y = -1;
	//  0 on Y (neutral)
	G_DIRECTIONS[DIRECTION_W].y  = G_DIRECTIONS[DIRECTION_0].y = G_DIRECTIONS[DIRECTION_E].y  =  0;
	// +1 on Y (South)
	G_DIRECTIONS[DIRECTION_SW].y = G_DIRECTIONS[DIRECTION_S].y = G_DIRECTIONS[DIRECTION_SE].y = +1;
}
