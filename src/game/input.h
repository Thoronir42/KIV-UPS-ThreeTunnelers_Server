#ifndef INPUT_H
#define	INPUT_H

const unsigned int
INPUT_MASK_UP,
		INPUT_MASK_DOWN,
		INPUT_MASK_LEFT,
		INPUT_MASK_RIGHT,
		INPUT_MASK_SHOOT;

typedef struct input {
	int heldKeys;
} input;


void input_set(input *p_input, unsigned int input);

int input_is_held(input *p_input, unsigned int input_mask);

#endif	/* INPUT_H */

