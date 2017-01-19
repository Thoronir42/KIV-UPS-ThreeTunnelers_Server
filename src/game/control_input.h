#ifndef CONTROL_INPUT_H
#define CONTROL_INPUT_H

#define INPUT_UP 0x01
#define INPUT_DOWN 0x02
#define INPUT_LEFT 0x04
#define INPUT_RIGHT 0x08
#define INPUT_SHOOT 0x16

typedef struct control_input {
    int _state;
} control_input;


int input_set_state(control_input *p_input, int state);

int input_is_held(control_input *p_input, unsigned int input_mask);

int input_direction_x(control_input *p);
int input_direction_y(control_input *p);

#endif /* CONTROL_INPUT_H */

