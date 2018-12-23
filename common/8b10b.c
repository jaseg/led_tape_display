
#include <assert.h>
#include <stdio.h>

#include "8b10b.h"

/* Ignore warnings about some of the following arrays being unused. They will be removed by the linker. I want to keep
 * them here for future reference. */
#pragma GCC diagnostic ignored "-Wunused-const-variable=0"
static const struct entry map_5b6b[32] = {
    {0b100111, 0b011000, 2}, /* D.00 */
    {0b011101, 0b100010, 2}, /* D.01 */
    {0b101101, 0b010010, 2}, /* D.02 */
    {0b110001, 0b110001, 0}, /* D.03 */
    {0b110101, 0b001010, 2}, /* D.04 */
    {0b101001, 0b101001, 0}, /* D.05 */
    {0b011001, 0b011001, 0}, /* D.06 */
    {0b111000, 0b000111, 0}, /* D.07 */
    {0b111001, 0b000110, 2}, /* D.08 */
    {0b100101, 0b100101, 0}, /* D.09 */
    {0b010101, 0b010101, 0}, /* D.10 */
    {0b110100, 0b110100, 0}, /* D.11 */
    {0b001101, 0b001101, 0}, /* D.12 */
    {0b101100, 0b101100, 0}, /* D.13 */
    {0b011100, 0b011100, 0}, /* D.14 */
    {0b010111, 0b101000, 2}, /* D.15 */
    {0b011011, 0b100100, 2}, /* D.16 */
    {0b100011, 0b100011, 0}, /* D.17 */
    {0b010011, 0b010011, 0}, /* D.18 */
    {0b110010, 0b110010, 0}, /* D.19 */
    {0b001011, 0b001011, 0}, /* D.20 */
    {0b101010, 0b101010, 0}, /* D.21 */
    {0b011010, 0b011010, 0}, /* D.22 */
    {0b111010, 0b000101, 2}, /* D.23 */
    {0b110011, 0b001100, 2}, /* D.24 */
    {0b100110, 0b100110, 0}, /* D.25 */
    {0b010110, 0b010110, 0}, /* D.26 */
    {0b110110, 0b001001, 2}, /* D.27 */
    {0b001110, 0b001110, 0}, /* D.28 */
    {0b101110, 0b010001, 2}, /* D.29 */
    {0b011110, 0b100001, 2}, /* D.30 */
    {0b101011, 0b010100, 2}  /* D.31 */
};

static const int8_t map_6b5b[64] = {
    [0b000101] = 23, /* rd = +1 */
    [0b000110] =  8, /* rd = +1 */
    [0b000111] =  7, /* rd = +1 */
    [0b001001] = 27, /* rd = +1 */
    [0b001010] =  4, /* rd = +1 */
    [0b001011] = 20, /* rd = +1 */
    [0b001100] = 24, /* rd = +1 */
    [0b001101] = 12, /* rd = +1 */
    [0b001101] = 12, /* rd = -1 */
    [0b001110] = 28, /* rd = +1 */
    [0b010001] = 29, /* rd = +1 */
    [0b010010] =  2, /* rd = +1 */
    [0b010011] = 18, /* rd = +1 */
    [0b010100] = 31, /* rd = +1 */
    [0b010101] = 10, /* rd = +1 */
    [0b010101] = 10, /* rd = -1 */
    [0b010110] = 26, /* rd = +1 */
    [0b010111] = 15, /* rd = -1 */
    [0b011000] = -1, /* rd = +1 */
    [0b011001] =  6, /* rd = +1 */
    [0b011010] = 22, /* rd = +1 */
    [0b011011] = 16, /* rd = -1 */
    [0b011100] = 14, /* rd = -1 */
    [0b011101] =  1, /* rd = -1 */
    [0b011110] = 30, /* rd = -1 */
    [0b100001] = 30, /* rd = +1 */
    [0b100010] =  1, /* rd = +1 */
    [0b100011] = 17, /* rd = -1 */
    [0b100100] = 16, /* rd = +1 */
    [0b100101] =  9, /* rd = -1 */
    [0b100110] = 25, /* rd = -1 */
    [0b100111] = -1, /* rd = -1 */
    [0b101000] = 15, /* rd = +1 */
    [0b101001] =  5, /* rd = -1 */
    [0b101010] = 21, /* rd = -1 */
    [0b101011] = 31, /* rd = -1 */
    [0b101100] = 13, /* rd = -1 */
    [0b101101] =  2, /* rd = -1 */
    [0b101110] = 29, /* rd = -1 */
    [0b110001] =  3, /* rd = -1 */
    [0b110010] = 19, /* rd = -1 */
    [0b110011] = 24, /* rd = -1 */
    [0b110100] = 11, /* rd = -1 */
    [0b110101] =  4, /* rd = -1 */
    [0b110110] = 27, /* rd = -1 */
    [0b111000] =  7, /* rd = -1 */
    [0b111001] =  8, /* rd = -1 */
    [0b111010] = 23  /* rd = -1 */
};

static const struct entry K28 = {0b001111, 0b110000};

static const struct entry map_3b4b_d[8] = {
    {0b1011, 0b0100, 2}, /* D.x.0  */
    {0b1001, 0b1001, 0}, /* D.x.1  */
    {0b0101, 0b0101, 0}, /* D.x.2  */
    {0b1100, 0b0011, 0}, /* D.x.3  */
    {0b1101, 0b0010, 2}, /* D.x.4  */
    {0b1010, 0b1010, 0}, /* D.x.5  */
    {0b0110, 0b0110, 0}, /* D.x.6  */
    {0b1110, 0b0001, 2}  /* D.x.P7 */
};

static const struct entry Dx_A7 = {0b0111, 0b1000, 2};

static const struct entry map_3b4b_k[8] = {
    {0b1011, 0b0100}, /* K.x.0 */
    {0b0110, 0b1001}, /* K.x.1 */
    {0b1010, 0b0101}, /* K.x.2 */
    {0b1100, 0b0011}, /* K.x.3 */
    {0b1101, 0b0010}, /* K.x.4 */
    {0b0101, 0b1010}, /* K.x.5 */
    {0b1001, 0b0110}, /* K.x.6 */
    {0b0111, 0b1000}  /* K.x.7 */
};

static const int8_t map_4b3b[16] = {
    [0b0001] =  7, /* DxP7 rd = +1 */
    [0b0010] =  4, /* rd = +1 */
    [0b0011] =  3, /* rd = +1 */
    [0b0100] = -1, /* rd = +1 */
    [0b0101] =  2, /* rd = -1 */
    [0b0110] =  6, /* rd = -1 */
    [0b0111] =  7, /* DxA7 rd = -1 */
    [0b1000] =  7, /* DxA7 rd = +1 */
    [0b1001] =  1, /* rd = -1 */
    [0b1010] =  5, /* rd = -1 */
    [0b1011] = -1, /* rd = -1 */
    [0b1100] =  3, /* rd = -1 */
    [0b1101] =  4, /* rd = -1 */
    [0b1110] =  7, /* DxP7 rd = -1 */
    [0b1111] =  0, /* invalid */
    [0b0000] =  0  /* invalid */
};

static const uint16_t k_sym_map[K_CODES_LAST] = {
    [K28_0] = 0b0011110100,
    [K28_1] = 0b0011111001,
    [K28_2] = 0b0011110101,
    [K28_3] = 0b0011110011,
    [K28_4] = 0b0011110010,
    [K28_5] = 0b0011111010,
    [K28_6] = 0b0011110110,
    [K28_7] = 0b0011111000,
    [K23_7] = 0b1110101000,
    [K27_7] = 0b1101101000,
    [K29_7] = 0b1011101000,
    [K30_7] = 0b0111101000
};

void xfr_8b10b_reset(struct state_8b10b_dec *st) {
    st->rx = 0;
    st->bit_ctr = 0; /* unsynchronized */
}

int xfr_8b10b_feed_bit(struct state_8b10b_dec *st, int bit) {
    uint32_t pattern = st->rx = (st->rx<<1 | !!bit) & 0x3ff;
    uint16_t comma = k_sym_map[K28_1];
    if (pattern == comma || pattern == ((~comma)&0x3ff)) {
        st->bit_ctr = 1;
        return -K28_1;
    }

    if (st->bit_ctr == 10) {
        st->bit_ctr = 1;

        for (int i=1; i<sizeof(k_sym_map)/sizeof(k_sym_map[0]); i++) {
            if (pattern == k_sym_map[i])
                return -i;
        }

        int p3b = map_4b3b[pattern & 0xf];
        int p5b = map_6b5b[pattern >> 4];

        if (p3b == 0 || p5b == 0)
            return -DECODING_ERROR;

        p3b = (p3b == -1) ? 0 : p3b;
        p5b = (p5b == -1) ? 0 : p5b;
        return p3b<<5 | p5b;

    } else if (st->bit_ctr > 0) {
        st->bit_ctr++;
    } /* else we do not have sync yet */
    return -DECODING_IN_PROGRESS;
}

bool xfr_8b10b_has_sync(struct state_8b10b_dec *st) {
    return st->bit_ctr != 0;
}

void xfr_8b10b_encode_reset(struct state_8b10b_enc *st) {
    st->rd = -1;
}

int xfr_8b10b_encode(struct state_8b10b_enc *st, int data) {
    if (data < 0) {
        if (-data >= sizeof(k_sym_map)/sizeof(k_sym_map[0]))
            return -EINVAL;

        return k_sym_map[-data];
    }

    if (data > 255)
        return -EINVAL;

    int p5b = data&0x1f, p3b = data>>5;
    //fprintf(stderr, "\nrd %d data %x p5b %d p3b %d\n", st->rd, data, p5b, p3b);

    int x5b = (st->rd == -1) ?  map_5b6b[p5b].rd_neg : map_5b6b[p5b].rd_pos;
    st->rd -= map_5b6b[p5b].disp * st->rd;
    //fprintf(stderr, "\nnow: rd %d data %x p5b %d p3b %d\n", st->rd, data, p5b, p3b);
    //assert(st->rd == -1 || st->rd == 1);

    int x3b = (st->rd == -1) ?  map_3b4b_d[p3b].rd_neg : map_3b4b_d[p3b].rd_pos;
    if (p3b == 7) {
        if (   (st->rd == -1 && (p5b == 17 || p5b == 18 || p5b == 20))
            || (st->rd ==  1 && (p5b == 11 || p5b == 13 || p5b == 14))) {
            //fprintf(stderr, "A7 override\n");
            x3b = (st->rd == -1) ?  Dx_A7.rd_neg : Dx_A7.rd_pos;
        }
    }
    st->rd -= map_3b4b_d[p3b].disp * st->rd; /* D.x.A7 and D.x.P7 both have parity 2 */
    //assert(st->rd == -1 || st->rd == 1);

    return x5b<<4 | x3b;
}

