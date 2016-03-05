#include "input.h"


void input_key_press(input *p_input, int input_type){
	p_input->heldKeys |= input_type;
}

void input_key_press(input *p_input, int input_type){
	p_input->heldKeys &= ~input_type;
}
