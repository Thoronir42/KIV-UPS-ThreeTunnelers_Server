#include "my_strings.h"

void strrev(char *src, char *dest, int length){
    int i;
    for(i = 0; i < length; i++){
        dest[i] = src[length - 1 - i];
    }
}