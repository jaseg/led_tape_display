
#include <stdio.h>

#include "8b10b.h"

static const char * const rc_names[] = {
    [K28_0] = "K.28.0",
    [K28_1] = "K.28.1",
    [K28_2] = "K.28.2",
    [K28_3] = "K.28.3",
    [K28_4] = "K.28.4",
    [K28_5] = "K.28.5",
    [K28_6] = "K.28.6",
    [K28_7] = "K.28.7",
    [K23_7] = "K.23.7",
    [K27_7] = "K.27.7",
    [K29_7] = "K.29.7",
    [K30_7] = "K.30.7",
    [DECODING_ERROR] = "ERROR",
    [DECODING_IN_PROGRESS] = "."
};

int main(void) {
    struct state_8b10b st;

    xfr_8b10b_reset(&st);

    int c;
    int comment = 0;
    while ((c=fgetc(stdin)) != EOF) {
        int bit;

        if (comment) {
            if (c == '\n')
                comment = 0;
            continue;
        }

        if (c == '\r' || c == ' ' || c == '\t' || c == '\n')
            continue;

        if (c == '#') {
            comment = 1;
            continue;
        }

        if (c == '0') {
            bit = 0;
        } else if (c == '1') {
            bit = 1;
        } else {
            fprintf(stderr, "Parse error: Bit must be 0 or 1, not '%c'. Exiting.\n", c);
            return 1;
        }

        int read_result = xfr_8b10b_feed_bit(&st, bit);
        char sync_status = xfr_8b10b_has_sync(&st) ? 'S' : 'U';

        if (read_result >= 0) {
            fprintf(stdout, "%c%02x ", sync_status, read_result);

        } else {
            if (-read_result > sizeof(rc_names)/sizeof(rc_names[0])) {
                fprintf(stderr, "Illegal read result %d. Exiting.\n", read_result);
                return 2;
            }

            const char * const msg = rc_names[-read_result];
            if (!msg) {
                fprintf(stderr, "Illegal read result %d. Exiting.\n", read_result);
                return 2;
            }

            fprintf(stdout, "%c%s ", sync_status, msg);
        }
    }
}

