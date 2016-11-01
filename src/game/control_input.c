#include "control_input.h"


void input_set(control_input *p_input, unsigned int input_type){
	p_input->heldKeys = input_type;
}

int input_is_held(control_input *p_input, unsigned int input_mask){
	return p_input->heldKeys & input_mask;
}

int input_direction_x(control_input *p) {
	int result = 0;
	if (input_is_held(p, INPUT_LEFT)) {
		result -= 1;
	}
	if (input_is_held(p, INPUT_RIGHT)) {
		result += 1;
	}
	
	return result;
}

int input_direction_y(control_input *p) {
	int result = 0;
	if (input_is_held(p, INPUT_UP)) {
		result -= 1;
	}
	if (input_is_held(p, INPUT_DOWN)) {
		result += 1;
	}
	
	return result;
}
