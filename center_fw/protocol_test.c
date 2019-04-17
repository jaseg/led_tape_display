/* Unit test file testing protocol.c */

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "protocol.h"
#include "8b10b.h"

static int urandom_fd = -1;
static long long n_tests = 0;

struct test_cmd_if {
    struct command_if_def cmd_if;
    int payload_len[256];
};

struct {
    int ncalls;
    int last_cmd;
    uint8_t last_args[sizeof(((struct proto_rx_st *)0)->argbuf)];
} handler_state;


void handle_command(int command, uint8_t *args) {
    handler_state.ncalls++;
    handler_state.last_cmd = command;
    if (args)
        memcpy(handler_state.last_args, args, 8);
}

void send_test_command_single(struct test_cmd_if *cmd_if, struct proto_rx_st *st, int cmd, int address, unsigned char pattern[256]) {
    n_tests++;
    receive_symbol(st, -K28_1);
    receive_symbol(st, cmd);
    receive_symbol(st, address);
    for (int i=0; i<cmd_if->payload_len[cmd]; i++)
        receive_symbol(st, pattern[i]);
}

void send_test_command_bulk(struct test_cmd_if *cmd_if, struct proto_rx_st *st, int cmd, int index, int len, unsigned char pattern[256]) {
    n_tests++;
    receive_symbol(st, -K28_1);
    receive_symbol(st, cmd | PKT_TYPE_BULK_FLAG);
    for (int j=0; j<len; j++) {
        for (int i=0; i<cmd_if->payload_len[cmd]; i++) {
            if (j == index)
                receive_symbol(st, pattern[i]);
            else
                receive_symbol(st, 0xaa);
        }
    }
}

void test_commands_with_pattern(struct test_cmd_if *cmd_if, unsigned char pattern[256]) {
    struct proto_rx_st st;

    for (int cmd=0; cmd<cmd_if->cmd_if.packet_type_max; cmd++) {
        /* Addresssed tests */
        reset_receiver(&st, &cmd_if->cmd_if);
        st.address = 23;
        handler_state.ncalls = 0;
        send_test_command_single(cmd_if, &st, cmd, 23, pattern);
        assert(handler_state.ncalls == 1);
        assert(handler_state.last_cmd == cmd);
        assert(!memcmp(handler_state.last_args, pattern, cmd_if->payload_len[cmd]));

        reset_receiver(&st, &cmd_if->cmd_if);
        st.address = 23;
        handler_state.ncalls = 0;
        send_test_command_single(cmd_if, &st, cmd, 5, pattern);
        assert(handler_state.ncalls == 0);

        reset_receiver(&st, &cmd_if->cmd_if);
        st.address = 5;
        handler_state.ncalls = 0;
        send_test_command_single(cmd_if, &st, cmd, 5, pattern);
        assert(handler_state.ncalls == 1);
        assert(handler_state.last_cmd == cmd);
        assert(!memcmp(handler_state.last_args, pattern, cmd_if->payload_len[cmd]));

        /* Bulk test */
        reset_receiver(&st, &cmd_if->cmd_if);
        st.address = 5;
        handler_state.ncalls = 0;
        send_test_command_bulk(cmd_if, &st, cmd, 5, 8, pattern);
        assert(handler_state.ncalls == 1);
        assert(handler_state.last_cmd == cmd);
        assert(!memcmp(handler_state.last_args, pattern, cmd_if->payload_len[cmd]));
    }
}

void test_commands(struct test_cmd_if *cmd_if) {
    unsigned char data[256];

    memset(data, 0, sizeof(data));
    test_commands_with_pattern(cmd_if, data);

    memset(data, 1, sizeof(data));
    test_commands_with_pattern(cmd_if, data);

    memset(data, 255, sizeof(data));
    test_commands_with_pattern(cmd_if, data);

    for (int i=0; i<5; i++) {
        assert(read(urandom_fd, (char *)data, sizeof(data)) == sizeof(data));
        test_commands_with_pattern(cmd_if, data);
    }
}

int main(void) {
    struct test_cmd_if cmd_if;

    urandom_fd = open("/dev/urandom", O_RDONLY);
    assert(urandom_fd > 0);

    for (int ncmds=1; ncmds<128; ncmds++) {
        cmd_if.cmd_if.packet_type_max = ncmds;

        /* Case 1 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = 0;
        test_commands(&cmd_if);

        /* Case 2 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = 1;
        test_commands(&cmd_if);

        /* Case 3 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = i&1 ? 1 : 0;
        test_commands(&cmd_if);

        /* Case 4 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = i&1 ? 0 : 1;
        test_commands(&cmd_if);

        /* Case 5 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = i&1 ? 1 : 2;
        test_commands(&cmd_if);

        /* Case 6 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = i%8;
        test_commands(&cmd_if);

        /* Case 7 */
        for (int i=0; i<ncmds; i++)
            cmd_if.payload_len[i] = 4;
        test_commands(&cmd_if);
    }

    assert(!close(urandom_fd));

    printf("Successfully ran %lld tests\n", n_tests);
}
