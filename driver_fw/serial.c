/* 8seg LED display driver firmware
 * Copyright (C) 2018 Sebastian Götte <code@jaseg.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "serial.h"
#include "mac.h"

unsigned int uart_overruns = 0;
unsigned int invalid_frames = 0;

static union tx_buf_union tx_buf;
volatile union rx_buf_union rx_buf;

void serial_init() {
    USART1->CR1 = /* 8-bit -> M1, M0 clear */
        /* RTOIE clear */
          (8 << USART_CR1_DEAT_Pos) /* 8 sample cycles/1 bit DE assertion time */
        | (8 << USART_CR1_DEDT_Pos) /* 8 sample cycles/1 bit DE assertion time */
        /* OVER8 clear. Use default 16x oversampling */
        /* CMIF clear */
        | USART_CR1_MME
        /* WAKE clear */
        /* PCE, PS clear */
        | USART_CR1_RXNEIE /* Enable receive interrupt */
        /* other interrupts clear */
        | USART_CR1_TE
        | USART_CR1_RE;
    /* Invert TX and DE to accomodate the level shifters */
    USART1->CR2 = USART_CR2_TXINV;
    USART1->CR3 = USART_CR3_DEM | USART_CR3_DEP; /* enable RS485 DE (output on RTS) */
    /* Set divider for 9600 baud rate @48MHz system clock. */
    int usartdiv = 5000;
    USART1->BRR = usartdiv;

    /* And... go! */
    USART1->CR1 |= USART_CR1_UE;

    /* Enable receive interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 1);
}

void tx_char(uint8_t c) {
    while (!(USART1->ISR & USART_ISR_TC));
    USART1->TDR = c;
}

void send_frame_formatted(uint8_t *buf, int len) {
    uint8_t *p=buf, *q=buf, *end=buf+len;
    do {
        while (*q && q!=end)
            q++;
        tx_char(q-p+1);
        while (*p && p!=end)
            tx_char(*p++);
        p++, q++;
    } while (p < end);
    tx_char('\0');
}

void send_status_reply(void) {
    tx_buf.desc_reply.firmware_version = FIRMWARE_VERSION;
    tx_buf.desc_reply.hardware_version = HARDWARE_VERSION;
    tx_buf.desc_reply.pad[0] = tx_buf.desc_reply.pad[1] = 0;
    tx_buf.desc_reply.uptime_s = sys_time_s;
    //tx_buf.desc_reply.vcc_mv = adc_vcc_mv;
    //tx_buf.desc_reply.temp_celsius = adc_temp_celsius;
    tx_buf.desc_reply.global_brightness = global_brightness;
    tx_buf.desc_reply.framerate_millifps = frame_duration_us > 0 ? 1000000000 / frame_duration_us : 0;
    tx_buf.desc_reply.uart_overruns = uart_overruns;
    tx_buf.desc_reply.invalid_frames = invalid_frames;
    send_frame_formatted(tx_buf.byte_data, sizeof(tx_buf.desc_reply));
}

/* This is the higher-level protocol handler for the serial protocol. It gets passed the number of data bytes in this
 * frame (which may be zero) and returns a pointer to the buffer where the next frame should be stored.
 */
volatile uint8_t *packet_received(int len) {
    static enum {
        PROT_ADDRESSED = 0,
        PROT_IGNORE = 2,
    } protocol_state = PROT_IGNORE; 
    /* Use mac frames as delimiters to synchronize this protocol layer */
    trigger_comm_led();
    if (len == 0) { /* Discovery packet */
        if (sys_time_tick < 100) { /* Only respond during the first 100ms after boot */
            send_frame_formatted((uint8_t*)&device_mac, sizeof(device_mac));
        }

    } else if (len == 1) { /* Command packet */
        if (protocol_state == PROT_ADDRESSED) {
            switch (rx_buf.byte_data[0]) {
            case 0x01:
                send_status_reply();
                break;
            }
        } else {
            invalid_frames++;
            trigger_error_led();
        }
        protocol_state = PROT_IGNORE;

    } else if (len == 4) { /* Address packet */
        if (rx_buf.mac_data == device_mac) { /* we are addressed */
            protocol_state = PROT_ADDRESSED; /* start listening for frame buffer data */
        } else { /* we are not addressed */
            protocol_state = PROT_IGNORE; /* ignore packet */
        }

    } else if (len == sizeof(rx_buf.set_fb_rq)/2) {
        if (protocol_state == PROT_ADDRESSED) { /* First of two half-framebuffer data frames */

            /* FIXME */

            /* Go to "hang mode" until next zero-length packet. */
            protocol_state = PROT_IGNORE;
        }

    } else {
        /* FIXME An invalid packet has been received. What should we do? */
        invalid_frames++;
        trigger_error_led();
        protocol_state = PROT_IGNORE; /* go into "hang mode" until next zero-length packet */
    }

    /* By default, return rx_buf.byte_data . This means if an invalid protocol state is reached ("hang mode"), the next
     * frame is still written to rx_buf. This is not a problem since whatever garbage is written at that point will be
     * overwritten before the next buffer transfer. */
    return rx_buf.byte_data;
}

void USART1_IRQHandler(void) {
    /* Since a large amount of data will be shoved down this UART interface we need a more reliable and more efficient
     * way of framing than just waiting between transmissions.
     *
     * This code uses "Consistent Overhead Byte Stuffing" (COBS). For details, see its Wikipedia page[0] or the proper
     * scientific paper[1] published on it. Roughly, it works like this:
     *
     * * A frame is at most 254 bytes in length.
     * * The null byte 0x00 acts as a frame delimiter. There is no null bytes inside frames.
     * * Every frame starts with an "overhead" byte indicating the number of non-null payload bytes until the next null
     *   byte in the payload, **plus one**. This means this byte can never be zero.
     * * Every null byte in the payload is replaced by *its* distance to *its* next null byte as above.
     *
     * This means, at any point the receiver can efficiently be synchronized on the next frame boundary by simply
     * waiting for a null byte. After that, only a simple state machine is necessary to strip the overhead byte and a
     * counter to then count skip intervals.
     *
     * Here is Wikipedia's table of example values:
     *
     *    Unencoded data          Encoded with COBS
     *    00                      01 01 00
     *    00 00                   01 01 01 00
     *    11 22 00 33             03 11 22 02 33 00
     *    11 22 33 44             05 11 22 33 44 00
     *    11 00 00 00             02 11 01 01 01 00
     *    01 02 ...FE             FF 01 02 ...FE 00
     *
     * [0] https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
     * [1] Cheshire, Stuart; Baker, Mary (1999). "Consistent Overhead Byte Stuffing"
     *     IEEE/ACM Transactions on Networking. doi:10.1109/90.769765
     *     http://www.stuartcheshire.org/papers/COBSforToN.pdf
     */

    /* This pointer stores where we write data. The higher-level protocol logic decides on a frame-by-frame-basis where
     * the next frame's data will be stored. */
    static volatile uint8_t *writep = rx_buf.byte_data;
    /* Index inside the current frame payload */
    static int rxpos = 0;
    /* COBS state machine. This implementation might be a little too complicated, but it works well enough and I find it
     * reasonably easy to understand. */
    static enum {
        COBS_WAIT_SYNC = 0,  /* Synchronize with frame */
        COBS_WAIT_START = 1, /* Await overhead byte */
        COBS_RUNNING = 2     /* Process payload */
    } cobs_state = 0;
    /* COBS skip counter. During payload processing this contains the remaining non-null payload bytes */
    static int cobs_count = 0;

    if (USART1->ISR & USART_ISR_ORE) { /* Overrun handling */
        uart_overruns++;
        trigger_error_led();
        /* Reset and re-synchronize. Retry next frame. */
        rxpos = 0;
        cobs_state = COBS_WAIT_SYNC;
        /* Clear interrupt flag */
        USART1->ICR = USART_ICR_ORECF;

    } else { /* Data received */
        uint8_t data = USART1->RDR; /* This automatically acknowledges the IRQ */

        if (data == 0x00) { /* End-of-packet */
            /* Process higher protocol layers on this packet. */
            writep = packet_received(rxpos);

            /* Reset for next packet. */
            cobs_state = COBS_WAIT_START;
            rxpos = 0;

        } else { /* non-null byte */
            if (cobs_state == COBS_WAIT_SYNC) { /* Wait for null byte */
                /* ignore data */

            } else if (cobs_state == COBS_WAIT_START) { /* Overhead byte */
                cobs_count = data;
                cobs_state = COBS_RUNNING;

            } else { /* Payload byte */
                if (--cobs_count == 0) { /* Skip byte */
                    cobs_count = data;
                    data = 0;
                }

                /* Write processed payload byte to current receive buffer */
                writep[rxpos++] = data;
            }
        }
    }
}

