
#include "protocol.h"
#include "8b10b.h"

void receive_symbol(struct proto_rx_st *st, int symbol) {
    if (symbol == -K28_1) { /* Comma/frame delimiter */
        st->rxpos = 0;
        /* Fall through and return and just ignore incomplete packets */

    } else if (symbol == -DECODING_ERROR) {
        st->rxpos = -1;

    } else if (symbol < 0) { /* Unknown comma symbol or error */
        st->rxpos = -1;

    } else if (st->rxpos == -1) {
        return;

    } else if (st->rxpos == 0) { /* First data symbol, and not an error or comma symbol */
        st->packet_type = symbol & ~PKT_TYPE_BULK_FLAG;
        if (st->packet_type >= st->cmd_if->packet_type_max) {
            st->rxpos = -1;
            return;
        }

        st->is_bulk = symbol & PKT_TYPE_BULK_FLAG;
        st->offset = (st->is_bulk) ? st->address*st->cmd_if->payload_len[st->packet_type]+1 : 2;
        st->rxpos++;

    } else if (!st->is_bulk && st->rxpos == 1) {
        st->rxpos = (symbol == st->address) ? 2 : -1;

    } else {
        st->argbuf[st->rxpos - st->offset] = symbol;
        st->rxpos++;

        if (st->rxpos - st->offset == st->cmd_if->payload_len[st->packet_type]) {
            handle_command(st->packet_type, (uint8_t *)st->argbuf);
            st->rxpos = -1;
        }
    }
}

