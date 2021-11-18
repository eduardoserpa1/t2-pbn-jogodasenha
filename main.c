#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "nokia5110.h"


uint8_t glyph[] = { 0b00010000,
                    0b00100100,
                    0b00100000, 
                    0b00100100, 
                    0b00010000};

int main(void)
{
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1,glyph);
    nokia_lcd_write_string("8 BIT POWER!",1);
    nokia_lcd_set_cursor(0, 12);
    nokia_lcd_write_string("!!@@##$$**~~", 1);
    nokia_lcd_set_cursor(0, 22);
    nokia_lcd_write_string("\001\001\001\001\001",2);
    nokia_lcd_render();
    

    for (;;) {
        _delay_ms(1000);
    } 
}
