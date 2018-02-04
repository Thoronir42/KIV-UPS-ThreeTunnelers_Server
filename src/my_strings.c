#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "my_strings.h"
#include "logger.h"

#define MY_STR_RADIX 16

void strrev(char *dest, const char *src, int length) {
    int i;
    for (i = 0; i < length; i++) {
        dest[i] = src[length - 1 - i];
    }
}

int strpos(char *haystack, char *needle) {
    char *p = strstr(haystack, needle);
    if (!p){
        return STR_NOT_FOUND;
    }
    return p - haystack;
}

void strshift(char *str, int length, int positions) {
    int i;
    for (i = 0; i + positions < length; i++) {
        str[i] = str[i + positions];
    }
    memset(str + length - positions, '\0', positions);
}

void strrand(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static const int alphanum_len = sizeof(alphanum) - 1;
    int i;
    for (i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % alphanum_len];
    }

    s[len] = 0;
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
    char buf[17];
    snprintf(buf, 17, "%016LX", val);
    memcpy(dest, buf, 16);
}

void write_hex_int(char *dest, int val) {
    char buf[9];
    snprintf(buf, 9, "%08X", val);
    memcpy(dest, buf, 8);
}

void write_hex_short(char *dest, short val) {
    char buf[5];
    snprintf(buf, 5, "%04X", val);
    memcpy(dest, buf, 4);
}

void write_hex_byte(char *dest, my_byte val) {
    char buf[3];
    snprintf(buf, 3, "%02X", val);
    memcpy(dest, buf, 2);
}

char char_from_num(my_byte val){
    char buf[2];
    if(val < 0 || val > 16){
        glog(LOG_WARNING, "Invalid char value %d", val);
        return 'X';
    }
    snprintf(buf, 2, "%01X", val);
    return buf[0];
}
