#ifndef STR_SCANNER_H
#define STR_SCANNER_H

#include "../my_strings.h"

typedef struct str_scanner {
    const char *str;
    int length;
    int read;
} str_scanner;

void str_scanner_set(str_scanner *scanner, const char *str, int length);

const char *str_scanner_rest(str_scanner *scanner);

int str_scanner_rest_length(str_scanner *scanner);

my_byte strsc_byte(str_scanner *scanner);

short strsc_short(str_scanner *scanner);

int strsc_int(str_scanner *scanner);

long strsc_long(str_scanner *scanner);

#endif /* STR_SCANNER_H */

