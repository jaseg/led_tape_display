
#include <unistd.h>
#include "protocol.h"
#include "8b10b.h"

void reset_receiver(struct proto_rx_st *st, struct command_if_def *cmd_if) {
    st->rxpos = -1;
    st->address = 5; /* FIXME debug code */
    st->cmd_if = cmd_if;
}

void receive_symbol(struct proto_rx_st *st, int symbol) {
    if (symbol == -K28_1) { /* Comma/frame delimiter */
        st->rxpos = 0;
        /* Fall through and return and just ignore incomplete packets */

    } else if (symbol == -DECODING_ERROR) {
        goto reset;

    } else if (symbol < 0) { /* Unknown comma symbol or error */
        goto reset;

    } else if (st->rxpos == -1) {
        return;

    } else if (st->rxpos == 0) { /* First data symbol, and not an error or comma symbol */
        st->packet_type = symbol & ~PKT_TYPE_BULK_FLAG;
        if (st->packet_type >= st->cmd_if->packet_type_max)
            goto reset;

        int payload_len = st->cmd_if->payload_len[st->packet_type];
        st->is_bulk = symbol & PKT_TYPE_BULK_FLAG;
        st->offset = (st->is_bulk) ? (st->address*payload_len + 1) : 2;
        st->rxpos++;

        if (payload_len == 0 && st->is_bulk) {
            handle_command(st->packet_type, NULL);
            goto reset;
        }

    } else if (!st->is_bulk && st->rxpos == 1) {
        if (symbol != st->address)
            goto reset;

        if (st->cmd_if->payload_len[st->packet_type] == 0) {
            handle_command(st->packet_type, NULL);
            goto reset;
        }
        st->rxpos = 2;

    } else {
        if (st->rxpos - st->offset >= 0)
            st->argbuf[st->rxpos - st->offset] = symbol;
        st->rxpos++;

        if (st->rxpos - st->offset == st->cmd_if->payload_len[st->packet_type]) {
            handle_command(st->packet_type, (uint8_t *)st->argbuf);
            goto reset;
        }
    }

    return;
reset:
    st->rxpos = -1;
}

