#include "controls.h"

int controls_set_state(controls *p_input, int state) {
    int current_state = p_input->_state;
    p_input->_state = state;

    return current_state != p_input->_state;
}

int controls_is_input_held(controls *p_input, control_input input) {
    return p_input->_state & input;
}

int controls_direction_x(controls *p) {
    int result = 0;
    if (controls_is_input_held(p, INPUT_LEFT)) {
        result -= 1;
    }
    if (controls_is_input_held(p, INPUT_RIGHT)) {
        result += 1;
    }

    return result;
}

int controls_direction_y(controls *p) {
    int result = 0;
    if (controls_is_input_held(p, INPUT_UP)) {
        result -= 1;
    }
    if (controls_is_input_held(p, INPUT_DOWN)) {
        result += 1;
    }

    return result;
}
