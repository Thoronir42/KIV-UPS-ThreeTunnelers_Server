#ifndef INPUT_H
#define	INPUT_H

#define INPUT_UP 0x01
#define INPUT_DOWN 0x02
#define INPUT_LEFT 0x04
#define INPUT_RIGHT 0x08
#define INPUT_SHOOT 0x10

typedef struct input {
	int heldKeys;
} input;


void input_set(input *p_input, unsigned int input);

int input_is_held(input *p_input, unsigned int input_mask);

int input_direction_x(input *p);
int input_direction_y(input *p);

#endif	/* INPUT_H */

