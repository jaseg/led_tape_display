
#include <stdio.h>
#include <string.h>

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
};

int hex_to_uint(const char *s, size_t len) {
    if (len > 7)
        return -2;

    int acc = 0;
    for (int i=0; i<len; i++) {
        int digit = s[i];
        if ('0' <= digit && digit <= '9')
            digit -= '0';
        else if ('a' <= digit && digit <= 'f')
            digit -= 'a' - 0xa;
        else if ('A' <= digit && digit <= 'F')
            digit -= 'A' - 0xA;
        else return -1;
        acc = acc << 4 | digit;
    }

    return acc;
}

int main(void) {
    struct state_8b10b_enc st;
    xfr_8b10b_encode_reset(&st);

    int c;
    int comment = 0;
    char parse_buf[32];
    int parse_buf_pos = 0;
    while ((c=fgetc(stdin)) != EOF) {
        if (comment) {
            if (c == '\n')
                comment = 0;
            printf("%c", c);
            continue;
        }

        if (c == '#') {
            comment = 1;
            printf("%c", c);
            continue;
        }

        if (c != '\r' && c != ' ' && c != '\t' && c != '\n') {
            parse_buf[parse_buf_pos++] = (char)c;
            if (parse_buf_pos == sizeof(parse_buf)) {
                parse_buf[sizeof(parse_buf)-1] = '\0';
                fprintf(stderr, "Parse error: Token \"%s[...]\" too long. Exiting.\n", parse_buf);
                return 1;
            }
            continue;
        }

        if (parse_buf_pos == 0)
            continue;

        int data = 0;
        for (int i=1; i<sizeof(rc_names)/sizeof(rc_names[0]); i++) {
            if (!strncmp(rc_names[i], parse_buf, parse_buf_pos)) {
                data = -i;
                break;
            }
        }

        if (!data) {
            data = hex_to_uint(parse_buf, parse_buf_pos);
            if (data < 0) {
                parse_buf[parse_buf_pos] = '\0';
                fprintf(stderr, "Invalid hex number: \"%s\"\n", parse_buf);
                return 1;
            }
        }

        parse_buf_pos = 0; /* reset for next token */

        int encoded = xfr_8b10b_encode(&st, data);
        if (encoded < 0) {
            fprintf(stderr, "Invalid argument error encoding\n");
            return 2;
        }

        for (int i=0; i<10; i++)
            printf("%c", (encoded & (1<<(9-i))) ? '1' : '0');

        printf("%c", c); /* print whitespace character */
    }
}

