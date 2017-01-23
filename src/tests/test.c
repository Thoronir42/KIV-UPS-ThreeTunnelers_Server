#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../logger.h"
#include "../my_strings.h"
#include "../networks/network_command.h"
#include "../networks/net_client.h"
#include "../networks/netadapter.h"
#include "../core/str_scanner.h"
#include "../map/tunneler_map.h"
#include "../map_generator/map_generator.h"
#include "../game/entity_shape.h"

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
        printf(" %d: %s\n", positions, string);
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

    memcpy(&cmd.data, "Nevypust supy ven.\n", 20);

    network_command_to_string(bfr, &cmd);

    network_command_print("Test", &cmd);
    printf("CharDst : %s", bfr);
}

void test_network_client_idle() {
    statistics s_stats;
    netadapter adapter;

    net_client clients[4];
    tcp_connection connections[4];
    memset(clients, 0, sizeof (net_client) * 4);
    memset(connections, 0, sizeof (net_client) * 4);
    int i;

    for (i = 0; i < 4; i++) {
        clients[i].connection = connections + i;
    }

    netadapter_init(&adapter, 0, &s_stats,
            clients, 4,
            NULL, NULL, 0);
    printf("Please ignore port related errors. Starting clients idle tests.\n"
            "Max allowed idletime set to %d", 2);

    *(short *) &adapter.ALLOWED_IDLE_TIME = 2;

    memcpy(clients[0].name, "Adam", 5);
    memcpy(clients[2].name, "Barbara", 8);

    clients[0].connection = connections + 0;
    clients[2].connection = connections + 1;

    clients[0].connection->last_active = time(NULL);
    sleep(1);
    clients[2].connection->last_active = time(NULL);
    _cli_list_clients(&adapter);

    for (i = 1; i <= 3; i++) {
        printf("Sleeping for 1 sec %d/%d", i, 3);
        sleep(1);
        //        netadapter_check_idle_clients(&adapter);
        _cli_list_clients(&adapter);
    }
}

void test_logger_formatting() {
    glog(LOG_INFO, "Tester tests");
    glog(LOG_WARNING, "Is six six? Eg %d == 6", 6);
    glog(LOG_ERROR, "I'm affraid, %s, I can not let you do that ( division by %d ).", "Dave", 0);
}

void test_command_appending() {
    network_command cmd;
    network_command *p_cmd = &cmd;
    char *str = "Ahoj";
    my_byte b = 12;
    short s = 176;
    int i = 640;
    long l = 1280L;

    network_command_prepare(p_cmd, NCT_LEAD_INTRODUCE);

    network_command_append_str(p_cmd, str);
    network_command_append_string(p_cmd, str, 4);
    network_command_print("cmd_str", p_cmd);

    network_command_prepare(p_cmd, NCT_LEAD_INTRODUCE);
    network_command_append_byte(p_cmd, b);
    network_command_append_byte(p_cmd, b);
    network_command_print("cmd_byte", p_cmd);

    network_command_prepare(p_cmd, NCT_LEAD_INTRODUCE);
    network_command_append_short(p_cmd, s);
    network_command_append_short(p_cmd, s);
    network_command_print("cmd_shrt", p_cmd);

    network_command_prepare(p_cmd, NCT_LEAD_INTRODUCE);
    network_command_append_int(p_cmd, i);
    network_command_append_int(p_cmd, i);
    network_command_print("cmd_int", p_cmd);

    network_command_prepare(p_cmd, NCT_LEAD_INTRODUCE);
    network_command_append_char(p_cmd, 'A');
    network_command_append_char(p_cmd, 'h');
    network_command_append_char(p_cmd, 'o');
    network_command_append_char(p_cmd, 'j');
    network_command_print("cmd_char", p_cmd);



    //    network_command_prepare(p_cmd, NCT_LEAD_INTRODUCE);
    //    printf("Command to be extended by %ld\n", l);
    //    network_command_append_long(p_cmd, l);
    //    printf("Command extended 1, long = %ld?\n", l);
    //    l = 2048L;
    //    network_command_append_long(p_cmd, l);
    //    printf("Command extended 2?\n");
}

void test_str_scanner() {
    char *src = "AHOJ1111222233334444";
    char dst[10];
    str_scanner scanner;

    memset(dst, 0, 10);
    str_scanner_set(&scanner, src, strlen(src));

    my_byte b;
    short s;
    int i;
    long l;

    str_scanner_print(&scanner);

    strsc_str(&scanner, dst, 4);
    printf("%s, ", dst);
    str_scanner_print(&scanner);

    b = strsc_byte(&scanner);
    printf("%u, ", b);
    str_scanner_print(&scanner);

    s = strsc_short(&scanner);
    printf("%d, ", s);
    str_scanner_print(&scanner);

    printf("Setting scanner for long.. \n");
    str_scanner_print(&scanner);
    scanner.read = 4;
    l = strsc_long(&scanner);
    printf("%ld, ", l);
    str_scanner_print(&scanner);


}

void _test_dump_map(tunneler_map *map) {
    int cx, cy, bx, by;
    tunneler_map_chunk *p_chunk;
    printf("Printing chunks\n");
    for (cy = 0; cy < map->chunk_dimensions.height; cy++) {
        for (cx = 0; cx < map->chunk_dimensions.width; cx++) {
            p_chunk = tunneler_map_get_chunk(map, cx, cy);
            printf("Chunk y=%d.x=%d, size = %d (%s)\n", cy, cx, p_chunk->size,
                    p_chunk->type == TUNNELER_MAP_CHUNK_TYPE_PLAYER_BASE ? "base" : "regular");
            for (by = 0; by < p_chunk->size; by++) {
                printf("%d: ", by);
                for (bx = 0; bx < p_chunk->size; bx++) {
                    printf("%3d", tunneler_map_chunk_get_block(p_chunk, bx, by));
                }
                printf("\n");
            }
            printf("\n");
        }
    }
}

void test_map_generation() {
    int width = 4, height = 5, chunkSize = 10, playerCount = 2;
    tunneler_map map;

    printf("Initializing map to be %d chunks wide and %d chunks high\n", width, height);
    tunneler_map_init(&map, width, height, chunkSize);
    printf("Generating map content for %d players\n", playerCount);
    map_generator_generate(&map, playerCount);

    _test_dump_map(&map);




}

void _shape_print(shape s, char *label) {
    int x, y;
    printf("Shape size = %d*%d '%s'\n", s.size.width, s.size.height, label);
    for (y = 0; y < s.size.height; y++) {
        for (x = 0; x < s.size.width; x++) {
            printf("%c", shape_is_solid(s, x, y) ? '#' : ' ');
        }
        printf("\n");
    }

}

void test_print_shapes() {
    _shape_print(shape_get(DIRECTION_N, SHAPE_TANK_BODY), "Body upward");
    _shape_print(shape_get(DIRECTION_NE, SHAPE_TANK_BODY), "Body diagonal");
    _shape_print(shape_get(DIRECTION_N, SHAPE_TANK_BELT), "Belt north");
    _shape_print(shape_get(DIRECTION_NE, SHAPE_TANK_BELT), "Belt north east");
    _shape_print(shape_get(DIRECTION_E, SHAPE_TANK_BELT), "Belt east");
    _shape_print(shape_get(DIRECTION_SE, SHAPE_TANK_BELT), "Belt south east");
    _shape_print(shape_get(DIRECTION_N, SHAPE_PROJECTILE), "Projectile north");
    _shape_print(shape_get(DIRECTION_NE, SHAPE_PROJECTILE), "Projectile north east");
    _shape_print(shape_get(DIRECTION_E, SHAPE_PROJECTILE), "Projectile east");
    _shape_print(shape_get(DIRECTION_SE, SHAPE_PROJECTILE), "Projectile south east");
}

void run_tests() {
    printf("Three Tunnelers Server - auto-testing\n");
    //    test_hex_formatting();
    //    test_command_parsing();

    //    test_strpos();
    //    test_strshift();
    //    test_logger_formatting();

    //    test_command_appending();
    //    test_str_scanner();
    //    test_network_client_idle();

    //    test_map_generation();
    
    test_print_shapes();
}
