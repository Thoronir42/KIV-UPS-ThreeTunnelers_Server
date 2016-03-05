#ifndef INPUT_H
#define	INPUT_H

#define INPUT_MASK_UP 0x01
#define INPUT_MASK_DOWN 0x02
#define INPUT_MASK_LEFT 0x04
#define INPUT_MASK_RIGHT 0x08
#define INPUT_MASK_SHOOT 0x10

typedef struct input{
	int heldKeys;
} input;


void input_key_press(input *p_input, int input_type);

void input_key_press(input *p_input, int input_type);

#endif	/* INPUT_H */

