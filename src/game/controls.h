#ifndef CONTROL_INPUT_H
#define CONTROL_INPUT_H

typedef enum control_input
{
    INPUT_UP = 0x01,
    INPUT_DOWN = 0x02,
    INPUT_LEFT = 0x04,
    INPUT_RIGHT = 0x08,
    INPUT_SHOOT = 0x16,
} control_input;

typedef struct controls
{
    int _state;
} controls;


int controls_set_state(controls *p_input, int state);

int controls_is_input_held(controls *p_input, control_input input);

int controls_direction_x(controls *p);
int controls_direction_y(controls *p);

#endif /* CONTROL_INPUT_H */

