#include <string.h>
#include <stdio.h>
#include "str_scanner.h"
#include "../my_strings.h"

void str_scanner_print(str_scanner *sc){
    printf("Scanner: [%02d/%02d] %s\n", sc->read, sc->length, sc->str);
}

void str_scanner_set(str_scanner *scanner, const char *str, int length) {
    scanner->str = str;
    scanner->length = length;
    scanner->read = 0;
}

const char *str_scanner_rest(str_scanner *scanner){
    return scanner->str + scanner->read;
}

int str_scanner_rest_length(str_scanner *scanner){
    return scanner->length - scanner->read;
}

void strsc_str(str_scanner *scanner, char *dst, int len){
    memcpy(dst, scanner->str + scanner->read, len);
    scanner->read += len;
}

my_byte strsc_byte(str_scanner *scanner){
    my_byte val = read_hex_byte(scanner->str + scanner->read);
    scanner->read += 2;
    
    return val;
}

short strsc_short(str_scanner *scanner){
    short val = read_hex_short(scanner->str + scanner->read);
    scanner->read += 4;
    
    return val;
}

int strsc_int(str_scanner *scanner){
    int val = read_hex_int(scanner->str + scanner->read);
    scanner->read += 8;
    
    return val;
}

long strsc_long(str_scanner *scanner){
    long val = read_hex_long(scanner->str + scanner->read);
    scanner->read += 16;
    
    return val;
}