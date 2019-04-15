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

#ifndef _C_LCD1602_H_INCLUDED_
#define _C_LCD1602_H_INCLUDED_

#define LCD_I2C_PERIPH I2C1
#define LCD_I2C_ADDR 0x4e

/* 16 spaces you can concatenate to printf formats to make sure the entire LCD line is always cleared */
#define LCD_FILL "    ""    ""    ""    "

void lcd1602_init(); // Инициализация дисплея
void lcd_write_str(uint8_t in_u8X, uint8_t in_u8Y, char* in_cChar); // Отправить строку на экран с указанием позиции
void lcd_send_char(char in_cChar); // Отправить символ на экран
void lcd_send_str(char* in_cChar); // Отправить строку на экран
void lcd_set_pos(uint8_t in_u8X, uint8_t in_u8Y); // Установить позицию курсора
void lcd_clear(); // Очистить экран

/*
	|P7|P6|P5|P4|P3|P2|P1|P0|
	|B7|B6|B5|B4|LED|E|RW|RS|
*/

#define LCD_INITIALIZATION 0x03 // Инициализационный байт дисплея

#define LCD_MODE_COMMAND 0x00 // Флаг команды
#define LCD_MODE_DATA 0x01 // Флаг данных
#define LCD_MODE_LED 0x08 // флаг работы подсветки
#define LCD_MODE_WRITE 0x02 // Флаг записи
#define LCD_MODE_E_SET 0x04 // Флаг установки регистра Е
#define LCD_MODE_E_RESET 0xFB // Флаг сброса регистра Е

#define LCD_2_LINE_OFFSET 0x40 // Адрес второй строки дисплея

#define LCD_SET_CLEAR 0x01 // (1.53ms) Очистка дисплея с установкой курсора в начало первой строки
#define LCD_SET_CURSOR_TO_START 0x02 // (1.53ms) Установка курсора в начало первой строки

#define LCD_SET_SHIFT_AND_CHAR_DIRECTION 0x04 // (39mks)  Установка направления вывода символов, разрешение сдвига экрана
#define LCD_CHAR_DIRECTION_LEFT_RIGHT 0x02 // Вывод символов справа-налево, декремент адресного указателя DDRAM/CGRAM памяти
#define LCD_CHAR_DIRECTION_RIGHT_LEFT 0x00 // Вывод символов слева-направо, инкремент адресного указателя DDRAM/CGRAM памяти
#define LCD_SHIFT_DISABLE 0x00 // Запрет сдвига экрана при выводе символов
#define LCD_SHIFT_ENABLE 0x01 // Разрешение сдвига экрана при выводе символов

#define LCD_SET_CURSOR_AND_POWE_MODE 0x08 // (39mks) Управление режимом питания дисплея и отображением курсора
#define LCD_DISPLAY_OFF 0x00 // Выключить экран дисплея, сегменты погашены, содержимое внутренней памяти сохраняется
#define LCD_DISPLAY_ON 0x04 // Включить экран дисплея, нормальный режим работы
#define LCD_CURSOR_OFF 0x00 // Отключить отображение курсора
#define LCD_CURSOR_ON 0x02 // Включить отображение курсора
#define LCD_CURSOR_BLINK_OFF 0x00 // Отключить функцию мигания курсора
#define LCD_CURSOR_BLINK_ON 0x01 // Включить функцию мигания курсора

#define LCD_SET_CURSOR_AND_DISPLAY_SHIFT 0x10 // (39mks) Команда сдвига курсора и экрана
#define LCD_CURSOR_SHIFT 0x00 // Выбрать курсор для сдвига
#define LCD_DISPLAY_AND_CURSOR_SHIFT 0x08 // Выбрать экран (вместе с курсором) для сдвига
#define LCD_LEFT_SHIFT 0x00 // Сдвиг влево (только курсор или весь экран, зависит от бита S/C)
#define LCD_RIGHT_SHIFT 0x04 // Сдвиг вправо (только курсор или весь экран, зависит от бита S/C)

#define LCD_SET_INTERFACE_LINES_FONT 0x20 // (39mks) Настройка интерфейса ввода/вывода данных, количества строк для вывода символов, размера шрифта
#define LCD_4BIT_INTERFACE 0x00 // Сдвиг вправо (только курсор или весь экран, зависит от бита S/C)
#define LCD_8BIT_INTERFACE 0x10 // 8-битный интерфейс ввода/вывода данных
#define LCD_ONE_LINE 0x00 // Использовать одну строку для вывода символов
#define LCD_TWO_LINE 0x08 // Задействовать 2 строки для вывода символов
#define LCD_5x8_FONT 0x00 // Размер шрифта 5×8 пикселей
#define LCD_5x11_FONT 0x04 // Размер шрифта 5×11 пикселей

#define LCD_SET_CGRAM_TO_ADDRESS 0x40 // (39mks) Запись адреса CGRAM памяти в адресный указатель
#define LCD_CGRAM_TO_ADDRESS_MASK 0x3F // Маска байта данных

#define LCD_SET_DDRAM_TO_ADDRESS 0x80 // (39mks) Запись адреса DDRAM памяти в адресный указатель
#define LCD_DDRAM_TO_ADDRESS_MASK 0x7F // Маска байта данных

#endif // _C_LCD1602_H_INCLUDED_
