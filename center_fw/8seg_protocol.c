
#include "global.h"
#include "adc.h"
#include "transmit.h"
#include "8seg_protocol.h"

struct cmd_if_struct {
    struct command_if_def cmd_if;
    int payload_len[PKT_TYPE_MAX];
};

const struct cmd_if_struct cmd_if = {{.packet_type_max=PKT_TYPE_MAX}, {
    [PKT_TYPE_RESERVED] = 0,
    [PKT_TYPE_SET_OUTPUTS_BINARY] = 1,
    [PKT_TYPE_SET_GLOBAL_BRIGHTNESS] = 1,
    [PKT_TYPE_SET_OUTPUTS] = 8,
    [PKT_TYPE_GET_STATUS] = 0 }
};

volatile union {
    struct status_tx status_tx;
    uint8_t p[0];
} tx_buf;

void protocol_init() {
	adc_configure_monitor_mode(&cmd_if.cmd_if);
    tx_init((uint8_t *)&tx_buf);
}


void handle_command(int command, uint8_t *args) {
    static int global_brightness = 0xff;
    switch (command) {
        case PKT_TYPE_SET_OUTPUTS_BINARY:
            set_outputs_binary(args[0], global_brightness);
            break;

        case PKT_TYPE_SET_GLOBAL_BRIGHTNESS:
            global_brightness = args[0];
            break;

        case PKT_TYPE_SET_OUTPUTS:
            set_outputs(args);
            break;

        case PKT_TYPE_GET_STATUS:
            tx_buf.status_tx.temp_tenths_C = adc_data.temp_celsius_tenths;
            tx_buf.status_tx.uptime_s = sys_time_seconds;
            tx_buf.status_tx.decoding_error_cnt = decoding_error_cnt;
            tx_buf.status_tx.protocol_error_cnt = protocol_error_cnt;
            tx_buf.status_tx.vcc_mv = adc_data.vcc_mv;
            tx_buf.status_tx.vin_mv = adc_data.mean_a_mv;
            tx_buf.status_tx.vskew_mv = adc_data.mean_diff_mv;
            tx_buf.status_tx.jitter_meas_avg_ns = jitter_meas_avg_ns;

            /* Initialize transmission here, *after* all data has been copied to the buffer */
            tx_transmit(sizeof(tx_buf.status_tx));
            break;
    }
}

