#include <stdio.h>
#include <string.h>

#include "my_strings.h"
#include "networks/network_command.h"

void test_hex_formatting() {
	char strings[4][17] = {"FF", "1234", "12345678", "123456789ABCDEF0"};
	char output[4][17];
	memset(output, 0, 4 * 17);

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
	printf("Long> %s = %ld\n", strings[3], l);
	printf("    < %s\n", output[3]);



}

void test_command_parsing(){
	char bfr[64] = "AB000413Jelenovi pivo nelej.";
	network_command cmd;
	
	network_command_from_string(&cmd, bfr);
	
	printf("CharSrc : %s\n", bfr);
	network_command_print("Parsed", &cmd);
	
	cmd.id = 127;
	memcpy(&cmd.data, "Nevypust supy ven.\n", 20);
	
	network_command_to_string(bfr, &cmd);
	
	network_command_print("Test", &cmd);
	printf("CharDst : %s", bfr);
	
	
}

void run_tests() {
	test_hex_formatting();
	test_command_parsing();
}
