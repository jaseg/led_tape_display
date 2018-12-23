#ifndef __8B10B_H__
#define __8B10B_H__

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

enum k_code {
    K28_0=1, K28_1, K28_2, K28_3,
    K28_4, K28_5, K28_6, K28_7,
    K23_7, K27_7, K29_7, K30_7,
    K_CODES_LAST
};

enum decoder_return_codes {
    _K_CODES_LAST = K_CODES_LAST,
    DECODING_ERROR,
    DECODING_IN_PROGRESS,
    DECODER_RETURN_CODE_LAST
};

struct entry {
    uint8_t rd_neg, rd_pos, disp;
};

struct state_8b10b_dec {
    uint32_t rx; 
    int bit_ctr;
};

struct state_8b10b_enc {
    int rd;
};

void xfr_8b10b_reset(struct state_8b10b_dec *st);
int xfr_8b10b_feed_bit(struct state_8b10b_dec *st, int bit);
bool xfr_8b10b_has_sync(struct state_8b10b_dec *st);

void xfr_8b10b_encode_reset(struct state_8b10b_enc *st);
int xfr_8b10b_encode(struct state_8b10b_enc *st, int data);

#endif
