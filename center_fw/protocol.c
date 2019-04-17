/* Control protocol receiver sitting between 8b10b.c and logical protocol handlers */

#include <unistd.h>
#include "protocol.h"
#include "8b10b.h"

volatile uint32_t decoding_error_cnt = 0, protocol_error_cnt = 0;
volatile bool backchannel_frame = 0;

/* Reset the given protocol state and register the command definition given with it. */
void reset_receiver(struct proto_rx_st *st, const struct command_if_def *cmd_if) {
    st->rxpos = -1;
    st->address = 5; /* FIXME debug code */
    st->cmd_if = cmd_if;
}

/* Receive an 8b10b symbol using the given protocol state. Handle any packets matching the enclosed command definition.
 *
 * This method is called from adc.c during the last bit period of the symbol, just before the actual end of the symbol
 * and start of the next symbol.
 */
void receive_symbol(struct proto_rx_st *st, int symbol) {

    if (symbol == -K28_2) { /* Backchannel marker */
        /* This symbol is inserted into the symbol stream at regular intervals.  It is not passed to the higher protocol
         * layers but synchronizes the backchannel logic through all nodes. The backchannel works by a node putting a
         * specified additional load of about 100mA (FIXME) on the line (1) or not (0) with all other nodes being
         * silent.  The master can detect this additional current.  The backchannel is synchronized to the 8b10b frame
         * being sent from the master, and the data is also 8b10b encoded. This means the backchannel is independent
         * from the forward-channel.
         *
         * This means while the forward-channel (the line voltage) might go like the upper trace, the back-channel (the
         * line current drawn by the node) might simultaneously look like the lower trace:
         *
         * Zoomed in on two master frames:
         *
         *                          |<---       D31.1      --->|  |<---      D03.6       --->|
         *     Master -> Node       1  0  1  0  1  1  1  0  0  1  1  1  0  0  0  1  0  1  1  0
         *        Voltage (V)  .../^^\__/^^\__/^^^^^^^^\_____/^^^^^^^^\________/^^\__/^^^^^\___...
         *
         *        Current (I)  ...\_____________________________/^^^^^V^^^^^^^^V^^V^^V^^^^^V^^\...
         *     Node -> Master                     0                            1
         *
         *
         * Zoomed out on two node frames, or twenty master frames:
         *
         *     Master -> Node     |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |<- symbols, one after another
         *        Voltage (V)  ...XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX...
         *
         *        Current (I)  ...___/^^^^^\__/^^\_____/^^\__/^^^^^\_____/^^\__/^^^^^\__/^^\___...
         *     Node -> Master       0  1  1  0  1  0  0  1  0  1  1  0  0  1  0  1  1  0  1  0
         *                          |<---       D22.2      --->|  |<---      D09.5       --->|
         *
         * Note that during backchannel transmissions only one node transmits at a time, and all nodes including the
         * transmitter keep their LEDs blanked to allow the master to more easily demodulate the transmission.
         *
         * This means that:
         *    * backchannel transmissions should be sparse (one per several regular symbols) to not affect brightness
         *      too much
         *    * backchannel transmissions should be spaced-out evenly and frequent enough to not cause visible flicker
         *
         * A consequence of this is that the backchannel has a bandwidth of only a fraction of the forward-channel. The
         * master can dynamically adjust the frequency of the forward-channel and spacing of the backchannel markers.
         * For 5kHz and 10% backchannel data (every tenth symbol being a backchannel symbol) the bandwidth works out to:
         *
         *      BW(forward-channel) = 5 [kHz] / 10 [8b10b] = 500 byte/s
         *      BW(backchannel)     = 5 [kHz] / 10 [8b10b] / 10 [every 10th symbol] / 10 [8b10b again] = 5 byte/s
         *
         * Luckily, we only use the backchannel for monitoring anyway and at ~20byte per monitoring frame we can easily
         * monitor a bus-load (heh!) of nodes once a minute, which is enough for our purposes.
         */

        /* Blank the LEDs for the next frame to keep the bus quiet during backchannel transmission. This happens on all
         * nodes. */
        backchannel_frame = true;
        return; /* We're done handling this symbol */
    } else {
        /* On anything else than a backchannel marker, turn off backchannel blanking for the next frame */
        backchannel_frame = false;
    }

    if (symbol == -K28_1) { /* Comma/frame delimiter */
        st->rxpos = 0;
        /* Fall through and return and just ignore incomplete packets */

    } else if (symbol == -DECODING_ERROR) {
        if (decoding_error_cnt < UINT32_MAX)
            decoding_error_cnt++;
        goto reset;

    } else if (symbol < 0) { /* Unknown comma symbol */
        if (protocol_error_cnt < UINT32_MAX)
            protocol_error_cnt++;
        goto reset;

    } else if (st->rxpos == -1) { /* Receiver freshly reset and no comma seen yet */
        return;

    } else if (st->rxpos == 0) { /* First data symbol, and not an error or comma symbol */
        st->packet_type = symbol & ~PKT_TYPE_BULK_FLAG;
        if (st->packet_type >= st->cmd_if->packet_type_max)
            goto reset; /* Not a protocol error */

        /* If this a bulk packet, calculate and store the offset of our portion of it. Otherwise just prime the state
         * for receiving the indidual packet by setting the offset to the first packet byte after the address. */
        int payload_len = st->cmd_if->payload_len[st->packet_type];
        st->is_bulk = symbol & PKT_TYPE_BULK_FLAG;
        st->offset = (st->is_bulk) ? (st->address*payload_len + 1) : 2;
        st->rxpos++;

        if (payload_len == 0 && st->is_bulk) {
            /* Length-0 packet type, handle now for bulk packets as we don't know when the master will send the next
             * comma or other symbol. For individually addressed packets, wait for the address byte. */
            handle_command(st->packet_type, NULL);
            goto reset;
        }

    } else if (!st->is_bulk && st->rxpos == 1) { /* First byte (address byte) of individually adressed packet */
        if (symbol != st->address) /* A different node is adressed */
            goto reset;

        if (st->cmd_if->payload_len[st->packet_type] == 0) {
            /* Length-0 packet type, handle now as we don't know when the master will send the next comma or other
             * symbol. */
            handle_command(st->packet_type, NULL);
            goto reset;
        }
        st->rxpos++;

    } else { /* Receiving packet body */
        if (st->rxpos - st->offset >= 0) {
            /* Either we're receiving an individually adressed packet adressed to us, or we're in the middle of a bulk
             * packet at our offset */
            st->argbuf[st->rxpos - st->offset] = symbol;
        }
        st->rxpos++;

        if (st->rxpos - st->offset == st->cmd_if->payload_len[st->packet_type]) {
            /* We're at the end of either an individual packet or our portion of a bulk packet. Handle packet here. */
            handle_command(st->packet_type, (uint8_t *)st->argbuf);
            goto reset;
        }
    }

    return;
reset:
    st->rxpos = -1;
}

