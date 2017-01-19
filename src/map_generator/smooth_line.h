#ifndef SMOOTH_LINE_H
#define SMOOTH_LINE_H

#define SMOOTH_LINE_SMOOTHNESS 3

typedef struct smooth_line
{
    int members[SMOOTH_LINE_SMOOTHNESS];
} smooth_line;

void smooth_line_init(smooth_line *p, int initialValue)
{
    int i;
    for (i = 0; i < SMOOTH_LINE_SMOOTHNESS; i++)
    {
        p->members[i] = initialValue;
    }
}

int smooth_line_put(smooth_line *p, int value)
{
    int i;
    for (i = 0; i < SMOOTH_LINE_SMOOTHNESS - 1; i++)
    {
        p->members[i] = p->members[i + 1];
    }
    p->members[SMOOTH_LINE_SMOOTHNESS - 1] = value;

    return smooth_line_get_value(p);
}

int smooth_line_get_value(smooth_line *p)
{
    int i, sum = 0;

    for (i = 0; i < SMOOTH_LINE_SMOOTHNESS; i++)
    {
        sum += p->members[i];
    }

    return sum / SMOOTH_LINE_SMOOTHNESS;
}


#endif /* SMOOTH_LINE_H */

