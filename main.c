#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <time.h>

#include "nokia5110.h"

#include "adc.h"
#include "print.h"
#include "usart.h"

char senha[4];

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

}

int main(void)
{
    srand(time(NULL));

    int run = 1;

    def_senha();

    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_write_string(senha,2);
    nokia_lcd_render();


    //_delay_ms(1000);

    USART_Init();
    adc_init();

    DDRB &= ~(1 << PB0); // seta PB0 como entrada

    while(run){
        adc_set_channel(0);
        float x = adc_read() * 0.0009765625;
        adc_set_channel(1);
        float y = adc_read() * 0.0009765625;

        print("\nx: ");
        printfloat(x);
        print("  y: ");
        printfloat(y);
        // Código do simulador tem um BUG, sempre vai retornar
        // ON no estado do switch
        

        if(PINB & (1<<PB0))
            print(" ON");
        else
            print(" OFF");
    } 
    return 0;
}
