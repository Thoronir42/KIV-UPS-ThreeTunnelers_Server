#ifndef MYSTRINGS_H
#define MYSTRINGS_H

typedef unsigned char my_byte;

void strrev(char *dest, const char *src, int length);

long read_hex_long(const char *src);
int read_hex_int(const char *src);
short read_hex_short(const char *src);
my_byte read_hex_byte(const char *src);

void write_hex_long(char *dest, long val);
void write_hex_int(char *dest, int val);
void write_hex_short(char *dest, short val);
void write_hex_byte(char *dest, my_byte val);


#endif /* MYSTRINGS_H */

