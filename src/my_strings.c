#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "my_strings.h"

#define MY_STR_RADIX 16

void strrev(char *dest, const char *src, int length) {
    int i;
    for (i = 0; i < length; i++) {
        dest[i] = src[length - 1 - i];
    }
}

int strpos(char *haystack, char *needle) {
    char *p = strstr(haystack, needle);
    if (p)
        return p - haystack;
    return STR_NOT_FOUND;
}

void strshift(char *str, int length, int positions) {
    int i;
    for (i = 0; i + positions < length; i++) {
        str[i] = str[i + positions];
    }
    memset(str + length - positions, '\0', positions);
}

long read_hex_long(const char *src) {
    char tmp[17];
    memset(tmp, 0, 17);
    memcpy(tmp, src, 16);

    return strtol(tmp, NULL, MY_STR_RADIX);
}

int read_hex_int(const char *src) {
    char tmp[9];
    memset(tmp, 0, 9);
    memcpy(tmp, src, 8);

    return (int) strtol(tmp, NULL, MY_STR_RADIX);
}

short read_hex_short(const char *src) {
    char tmp[5];
    memset(tmp, 0, 5);
    memcpy(tmp, src, 4);

    return (short) strtol(tmp, NULL, MY_STR_RADIX);
}

my_byte read_hex_byte(const char *src) {
    char tmp[3];
    memset(tmp, 0, 3);
    memcpy(tmp, src, 2);

    return (my_byte) (strtol(tmp, NULL, MY_STR_RADIX) % 256);
}

void write_hex_long(char *dest, long val) {
    sprintf(dest, "%016LX", val);
}

void write_hex_int(char *dest, int val) {
    sprintf(dest, "%08X", val);
}

void write_hex_short(char *dest, short val) {
    sprintf(dest, "%04X", val);
}

void write_hex_byte(char *dest, my_byte val) {
    sprintf(dest, "%02X", val);
}