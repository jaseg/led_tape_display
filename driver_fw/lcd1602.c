/*
    This is free and unencumbered software released into the public domain.
    ( https://github.com/KonstantinDM )

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <http://unlicense.org>
 */

#include "global.h"
#include "i2c.h"
#include "lcd1602.h"

static void write_byte(uint8_t in_u8Byte); // Отпвить байт на шину
static void send_half_byte(uint8_t in_u8Byte); // Отправить пол байта
static void send_command(uint8_t in_u8Byte); // Отправить байт команду
static void delay_micro(uint32_t in_u8micros);

static uint8_t lcd_gpio = 0;

/*
   Инициализация дисплея, обязательнаяпроцедура
   на входе: *
   на выходе: *
*/
void lcd1602_init()
{
    // Инициализация экрана обязательна
    delay_micro(15000);
    send_half_byte(LCD_INITIALIZATION);
    delay_micro(4000);
    send_half_byte(LCD_INITIALIZATION);
    delay_micro(100000);
    send_half_byte(LCD_INITIALIZATION);
    delay_micro(1000);
    send_half_byte(LCD_SET_CURSOR_TO_START);

    // Найстрока дисплея
    send_command(LCD_SET_INTERFACE_LINES_FONT | LCD_4BIT_INTERFACE | LCD_TWO_LINE | LCD_5x8_FONT);
    send_command(LCD_SET_CURSOR_AND_POWE_MODE | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_CURSOR_BLINK_OFF);
    send_command(LCD_SET_SHIFT_AND_CHAR_DIRECTION | LCD_CHAR_DIRECTION_LEFT_RIGHT | LCD_SHIFT_DISABLE);

    // Очистить экран
    lcd_clear();

    // Включить подсветку и режим записи
    lcd_gpio |= LCD_MODE_LED;
    lcd_gpio &= ~LCD_MODE_WRITE;
};

/*
   Отправить строку на экран с указанием позиции
   на входе: in_u8X   - позиция символа в строке
             in_u8Y   - номер строки
             in_cChar - символ для установки
   на выходе: *
*/
void lcd_write_str(uint8_t in_u8X, uint8_t in_u8Y, char* in_cChar)
{
    lcd_set_pos(in_u8X, in_u8Y);
    lcd_send_str(in_cChar);
};

/*
   Установка курсора
   на входе: in_u8X   - позиция символа в строке
             in_u8Y   - номер строки
   на выходе: *
*/
void lcd_set_pos(uint8_t in_u8X, uint8_t in_u8Y)
{
    switch (in_u8Y) {
    case 0:
        send_command(in_u8X | LCD_SET_DDRAM_TO_ADDRESS);
        break;
    case 1:
        send_command((LCD_2_LINE_OFFSET + in_u8X) | LCD_SET_DDRAM_TO_ADDRESS);
        break;
    }
};

/*
   Отправка строки на экран
   на входе: in_cChar - указатель на строку
   на выходе: *
*/
void lcd_send_str(char* in_pszChar)
{
    char* l_pszChar = in_pszChar;
    while ((l_pszChar)[0])
        lcd_send_char((l_pszChar++)[0]);
};

/*
   Отправка символа на экран
   на входе: in_cChar - символ
   на выходе: *
*/
void lcd_send_char(char in_cChar)
{
    lcd_gpio |= LCD_MODE_DATA;
    send_half_byte(in_cChar >> 4);
    send_half_byte(in_cChar);
};

/*
   Очистить экран
   на входе: *
   на выходе: *
*/
void lcd_clear()
{
    send_command(LCD_SET_CLEAR);
    delay_micro(1530);
};

/*
   Отправка байта на шину
   на входе: in_u8Byte - байт с командой
   на выходе: *
*/
void write_byte(uint8_t in_u8Byte)
{
    uint8_t buf[1] = { lcd_gpio | in_u8Byte };

    i2c_transmit(LCD_I2C_PERIPH, buf, 1, LCD_I2C_ADDR, I2C_GENSTOP_YES);
    delay_micro(39);
};

/*
   Отправка половины байта экрану
   на входе: in_u8Byte - байт с командой
   на выходе: *
*/
void send_half_byte(uint8_t in_u8Byte)
{
    write_byte(LCD_MODE_E_SET | (in_u8Byte << 4));
    write_byte(LCD_MODE_E_RESET);
};

/*
   Отправка команды дисплею
   на входе: in_u8Byte - байт с командой
   на выходе: *
*/
void send_command(uint8_t in_u8Byte)
{
    lcd_gpio &= ~LCD_MODE_DATA;
    send_half_byte(in_u8Byte >> 4);
    send_half_byte(in_u8Byte);
};

/*
   Отправка половины байта экрану
   на входе: in_u8Byte - байт с командой
   на выходе: *
*/
void delay_micro(uint32_t in_u8micros)
{
    /* FIXME */
    in_u8micros *= (SystemCoreClock / 1000000) / 9;
    while (in_u8micros--)
        ;
};
