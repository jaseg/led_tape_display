#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__

#include "global.h"
#include "8b10b.h"

#define TX_IDLE (-1)

#define TX_ERR_BUSY -1
#define TX_ERR_UNINITIALIZED -2

extern volatile uint32_t tx_overflow_cnt;

void tx_init(uint8_t *tx_buf);
int tx_transmit(size_t len);
int tx_next_bit(void);

#endif /* __TRANSMIT_H__ */
