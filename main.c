#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <time.h>

#include "nokia5110.h"

#include "adc.h"
#include "print.h"
#include "usart.h"



char senha[5];
char palpite[5] = {'0','0','0','0','\0'};

uint8_t seletor[] =    {0b00010000, 0b00100100, 0b11100000, 0b00100100, 0b00010000};


void def_senha(){

    int num;

    for(int i = 0; i < 4; i++){
        int tem = 0;
        num = 48 + (rand() % 10);
        for(int j = 0; j < 4; j++){
            if(senha[j]==num){
                tem++;
            }
        }
        if(tem){
            i--;
        }else{
            senha[i] = num;
        }
        
    }
    senha[4] = 0;

}

void draw_line(uint8_t index){

    switch (index)
    {
    case 0:
        nokia_lcd_drawline(0,17,10,17);
        nokia_lcd_drawline(0,0,10,0);
    break;
    
    case 1:
        nokia_lcd_drawline(11,17,21,17);
        nokia_lcd_drawline(11,0,21,0);
    break;

    case 2:
        nokia_lcd_drawline(22,17,32,17);
        nokia_lcd_drawline(22,0,32,0);
    break;

    case 3:
        nokia_lcd_drawline(33,17,43,17);
        nokia_lcd_drawline(33,0,43,0);
    break;
    
    default:
        
    break;
    }
  
}

int main(void)
{
    srand(time(NULL));

    int run = 1;

    def_senha();

    
    nokia_lcd_init();

    nokia_lcd_clear();

    /*
    for(int i = 0; i < 3; i++){
        nokia_lcd_clear();
        nokia_lcd_set_cursor(0,12);
        nokia_lcd_write_string("(honest :p)",1);
        nokia_lcd_render();
        nokia_lcd_set_cursor(0,0);
        nokia_lcd_write_string("CARREGANDO.",1);
        nokia_lcd_render();
        _delay_ms(500);
        nokia_lcd_write_string(".",1);
        nokia_lcd_render();
        _delay_ms(500);
        nokia_lcd_write_string(".",1);
        nokia_lcd_render();
        _delay_ms(500);
    }
    */
    

    USART_Init();
    adc_init();

    DDRB &= ~(1 << PB0); // seta PB0 como entrada

    int menu = 1;
    int game = 0;

    while(run){
        adc_set_channel(0);
        float x = adc_read() * 0.0009765625;
        adc_set_channel(1);
        float y = adc_read() * 0.0009765625;   
        
        int pressed = 0;

        if(PINB & (1<<PB0))
            pressed = 1;
        else
            pressed = 0;

        if(menu){
            nokia_lcd_clear();
            nokia_lcd_write_string("BEM VINDO AO   JOGO DA SENHA",1);
            nokia_lcd_set_cursor(0,24);
            nokia_lcd_write_string(" press button",1);
            nokia_lcd_render();
        }
        if(pressed){
            menu = 0;
            game = 1;
            print("x");
        }
        if(game){
            nokia_lcd_clear();
            nokia_lcd_set_cursor(0,2);
            nokia_lcd_write_string(palpite,2);
            //draw_line(0);
            //draw_line(1);
            draw_line(2);
            //draw_line(3);
            nokia_lcd_render();
            
        }


    } 
    return 0;
}


