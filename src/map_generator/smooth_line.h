#ifndef SMOOTH_LINE_H
#define SMOOTH_LINE_H

#define SMOOTH_LINE_SMOOTHNESS 3

typedef struct smooth_line
{
    int members[SMOOTH_LINE_SMOOTHNESS];
} smooth_line;

void smooth_line_init(smooth_line *p, int initialValue);

int smooth_line_put(smooth_line *p, int value);

int smooth_line_get_value(smooth_line *p);


#endif /* SMOOTH_LINE_H */

