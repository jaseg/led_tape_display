
#include "global.h"
#include "transmit.h"
#include "8b10b.h"

struct {
    uint8_t *buf;
    size_t pos, len;
    uint16_t current_symbol;
    struct state_8b10b_enc enc;
} tx_state = {0};

volatile uint32_t tx_overflow_cnt = 0;

void tx_init(uint8_t *tx_buf) {
    tx_state.buf = tx_buf;
    tx_state.pos = 0;
    tx_state.current_symbol = 0;
    xfr_8b10b_encode_reset(&tx_state.enc);
}

int tx_transmit(size_t len) {
    if (!tx_state.buf)
        return TX_ERR_UNINITIALIZED;

    if (tx_state.len) {
        tx_overflow_cnt++;
        return TX_ERR_BUSY;
    }

    tx_state.len = len;
    tx_state.current_symbol = 1;
    return 0;
}

int tx_next_bit() { 
    if (!tx_state.len)
        return TX_IDLE;

    int sym = tx_state.current_symbol;
    if (sym == 1) /* We're transmitting the first bit of a new frame now. */
        sym = xfr_8b10b_encode(&tx_state.enc, tx_state.buf[tx_state.pos++]) | (1<<10);

    int bit = sym&1;
    sym >>= 1;

    if (sym == 1 && tx_state.pos == tx_state.len)
        /* We're transmitting the last bit of a transmission now. Reset state. */
        tx_state.pos = tx_state.len = sym = 0;

    tx_state.current_symbol = sym;
    return bit;
}
