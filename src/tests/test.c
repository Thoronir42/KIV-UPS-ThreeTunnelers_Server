#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../my_strings.h"
#include "../networks/network_command.h"
#include "../networks/net_client.h"
#include "../networks/netadapter.h"

void test_strpos() {
    printf("Strpos test\n");

    char *haystack = "0004ASDF\n";
    char *needle = "\n";

    int expected = 8;
    int pos = strpos(haystack, needle);

    printf(" %d expected: %d, got %d\n", pos == expected, expected, pos);
}

void test_strshift() {
    printf("strshift test \n");
    char string[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int len = strlen(string);
    int i, positions = 1;

    printf(" 0: %s\n", string);
    for (i = 0; i < 3; i++) {
        strshift(string, len, positions);
        printf(" %d: %s\n" ,positions, string);
        positions *= 2;
    }
}

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

void test_command_parsing() {
    char bfr[64] = "0004Jelenovi pivo nelej.";
    network_command cmd;

    network_command_from_string(&cmd, bfr, strlen(bfr));

    printf("CharSrc : %s\n", bfr);
    network_command_print("Parsed", &cmd);

    cmd.id = 127;
    memcpy(&cmd.data, "Nevypust supy ven.\n", 20);

    network_command_to_string(bfr, &cmd);

    network_command_print("Test", &cmd);
    printf("CharDst : %s", bfr);
}

void test_network_client_idle() {
    net_client clients[4];
    statistics s_stats;
    netadapter adapter;
    memset(clients, 0, sizeof (net_client) * 4);
    int i;

    netadapter_init(&adapter, 0, &s_stats,
            clients, 4,
            NULL, 0,
            NULL, 0);
    printf("Please ignore port related errors. Starting clients idle tests.\n"
            "Max allowed idletime set to %d", 2);

    *(short *) &adapter.ALLOWED_IDLE_TIME = 2;

    memcpy(clients[0].name, "Adam", 5);
    memcpy(clients[2].name, "Barbara", 8);

    clients[0].connection.status = TCP_CONNECTION_STATUS_DISCONNECTED;
    clients[2].connection.status = TCP_CONNECTION_STATUS_DISCONNECTED;

    clients[0].connection.last_active = time(NULL);
    sleep(1);
    clients[2].connection.last_active = time(NULL);
    _cli_list_clients(&adapter);

    for (i = 1; i <= 3; i++) {
        printf("Sleeping for 1 sec %d/%d", i, 3);
        sleep(1);
        netadapter_check_idle_clients(&adapter);
        _cli_list_clients(&adapter);
    }
}

void run_tests() {
    test_hex_formatting();
    test_command_parsing();

    test_strpos();
    test_strshift();

    //test_network_client_idle();
}
