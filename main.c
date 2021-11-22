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
char palpite[5] = {'0','1','2','3','\0'};

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
            print("i");
        }
        if(game){
            nokia_lcd_clear();
            nokia_lcd_write_string(palpite,2);
            //nokia_lcd_custom(1,seletor);
            nokia_lcd_render();
        }


    } 
    return 0;
}

void draw_line(uint8_t index){

    switch (index)
    {
    case 0:
        /* code
        codigo para cada marcador
        
         */
        
    break;
    
    case 1:
        /* code */
    break;

    case 2:
        /* code */
    break;

    case 3:
        /* code */
    break;
    
    default:
        
    break;
    }

    
}
