#include "input.h"


void input_key_press(input *p_input, int input_type){
	p_input->heldKeys |= input_type;
}

int input_is_held(input *p_input, int input_mask){
	return p_input->heldKeys & input_mask;
}
