#ifndef __8B10B_H__
#define __8B10B_H__

#include <stdint.h>
#include <stdbool.h>

enum k_code {
    K28_0=1, K28_1, K28_2, K28_3,
    K28_4, K28_5, K28_6, K28_7,
    K23_7, K27_7, K29_7, K30_7,
    K_CODES_LAST
};

enum decoder_return_codes {
    _K_CODES_LAST = K_CODES_LAST,
    DECODING_ERROR,
    DECODING_IN_PROGRESS
};

struct entry_3b4b {
    uint8_t rd_neg, rd_pos;
};

struct entry_5b6b {
    uint8_t rd_neg, rd_pos;
};

struct state_8b10b {
    uint32_t rx; 
    int bit_ctr;
};


void xfr_8b10b_reset(struct state_8b10b *st);
int xfr_8b10b_feed_bit(struct state_8b10b *st, int bit);
bool xfr_8b10b_has_sync(struct state_8b10b *st);

#endif
