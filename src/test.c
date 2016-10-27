#include <stdio.h>

#include "my_strings.h"

void test_hex_formatting() {
	char strings[4][16] = {"FF", "FF00", "FF000000", "FF00000000000000"};
	char output[4][16];
	my_byte b;
	short s;
	int i;
	long l;
	printf("Hex parse tests: \n");

	b = read_hex_byte(strings[0]);
	s = read_hex_short(strings[1]);
	i = read_hex_int(strings[2]);
	l = read_hex_long(strings[3]);

	write_hex_byte(output[0], b);
	write_hex_short(output[1], s);
	write_hex_int(output[2], i);
	write_hex_long(output[3], l);


	printf("Byte> %s = %u\n", strings[0], b);
	printf("    < %s\n", output[0]);
	printf("Short> %s = %ud\n", strings[1], s);
	printf("     < %s\n", output[1]);
	printf("Int> %s = %ud\n", strings[2], i);
	printf("   < %s\n", strings[2]);
	printf("Long> %s = %ul\n", strings[3], l);
	printf("    < %s\n", output[3]);



}

void run_tests() {
	test_hex_formatting();
}
