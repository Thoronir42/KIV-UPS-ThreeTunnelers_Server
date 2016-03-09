#ifndef INPUT_H
#define	INPUT_H

const unsigned int
INPUT_MASK_UP = 0x01,
		INPUT_MASK_DOWN = 0x02,
		INPUT_MASK_LEFT = 0x04,
		INPUT_MASK_RIGHT = 0x08,
		INPUT_MASK_SHOOT = 0x10;

typedef struct input {
	int heldKeys;
} input;


void input_set(input *p_input, int input);

int input_is_held(input *p_input, int input_mask);

#endif	/* INPUT_H */

