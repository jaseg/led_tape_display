#ifndef __8SEG_PROTOCOL_H__
#define __8SEG_PROTOCOL_H__

enum packet_type {
    PKT_TYPE_RESERVED = 0,
    PKT_TYPE_SET_OUTPUTS_BINARY = 1,
    PKT_TYPE_SET_GLOBAL_BRIGHTNESS = 2,
    PKT_TYPE_SET_OUTPUTS = 3,
    PKT_TYPE_GET_STATUS = 4,
    PKT_TYPE_MAX
};

struct status_tx {
    int16_t temp_tenths_C;
    uint32_t uptime_s;
    uint32_t decoding_error_cnt, protocol_error_cnt;
    int16_t vcc_mv, vin_mv, vskew_mv;
    uint16_t jitter_meas_avg_ns;
};

extern void set_outputs(uint8_t val[8]);
extern void set_outputs_binary(int mask, int global_brightness);

void protocol_init(void);

#endif /* __8SEG_PROTOCOL_H__ */
