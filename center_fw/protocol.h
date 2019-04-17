#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>

#define PKT_TYPE_BULK_FLAG 0x80

struct proto_rx_st {
    int packet_type;
    int is_bulk;
    int rxpos;
    int address;
    uint8_t argbuf[8];
    int offset;
    const struct command_if_def *cmd_if;
};

struct command_if_def {
    int packet_type_max;
    int payload_len[0];
};

extern volatile uint32_t decoding_error_cnt, protocol_error_cnt;
extern volatile bool backchannel_frame;

/* Callback */
void handle_command(int command, uint8_t *args);

void receive_symbol(struct proto_rx_st *st, int symbol);
void reset_receiver(struct proto_rx_st *st, const struct command_if_def *cmd_if);

#endif
