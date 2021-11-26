#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <time.h>

#include "nokia5110.h"

#include "adc.h"
#include "print.h"
#include "usart.h"


int joystick_control(float,  float,  uint8_t,  int *);
int confere_senha();
void def_senha();
void draw_line(uint8_t index);

char senha[5] = {'0','0','0','0','\0'};
char palpite[5] = {'0','0','0','0','\0'};


  unsigned short lfsr = 0xACE1u;
  unsigned bit;

  unsigned Rand()
  {
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
  }

int main(void)
{
    srand(time(NULL));

    int run = 1;

    def_senha();

    print("\nsenha: ");
    print(senha);
    print("\n");
    
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
    int perdeu = 0;
    int tentativas_restantes = 10;
    int index_seletor = 0;

    int frame = 0;

    int sec = 30;

    while(run){
        frame++;
        if(frame%30==0){
            //sec++;
        }

        adc_set_channel(0);
        float x = adc_read() * 0.0009765625;
        adc_set_channel(1);
        float y = adc_read() * 0.0009765625;   
        
        uint8_t pressed = 0;

        //print("\nx: ");
        //printfloat(x);
        //print("  y: ");
        //printfloat(y);

        if(PINB & (1<<PB0))
            pressed = 1;
        else
            pressed = 0;

        if(pressed && !perdeu){
            menu = 0;
            game = 1;

        }

        if(menu){
            nokia_lcd_clear();
            nokia_lcd_write_string("BEM VINDO AO   JOGO DA SENHA",1);
            nokia_lcd_set_cursor(0,24);
            nokia_lcd_write_string(" press button",1);
            nokia_lcd_render();
        }
        
        if(game){
            if(frame%40==0)
                sec--;
            
            if(sec == 0 || tentativas_restantes == 0){
                print("perdeu");
                game = 0;
                perdeu = 1;
            }
                

            nokia_lcd_clear();

            nokia_lcd_set_cursor(0,2);

            nokia_lcd_write_string(palpite,2);

            int delay = joystick_control(x,y,pressed,&index_seletor);

            if(delay)
                _delay_ms(100);

            draw_line(index_seletor);

            char time[8] = "time:xx\0"; 

            if(sec > 9){
                for(int i = 0; i < 2;  i++){
                    int r_dezena = sec/10;
                    int r_unidade = sec%10;
                    char dezena = r_dezena + 48;
                    char unidade = r_unidade + 48;
                    time[5] = dezena;
                    time[6] = unidade;
                }
            }else{
                time[5] = '0';
                time[6] = sec + 48;
            }
         

            char tentativas[8] = "life:xx\0";

            if(tentativas_restantes==10){
                tentativas[5] = '1';
                tentativas[6] = '0';
            }else{
                tentativas[5] = '0';
                tentativas[6] = tentativas_restantes + 48;
            }
            nokia_lcd_set_cursor(0,32);
            nokia_lcd_write_string(tentativas,1);
            nokia_lcd_set_cursor(0,40);
            nokia_lcd_write_string(time,1);

            nokia_lcd_render();
            if(sec != 30){
                if(pressed){
                sec = 30;
                tentativas_restantes--;
                    if(!senha_diferente()){
                        print("igual");
                    }else{
                        print("diferente");
                    } 
                _delay_ms(100);
                }
            }
            
        }
        if(perdeu){

        }


    } 
    return 0;
}


int senha_diferente(){
    int response = 0;

    for(int i = 0; i < 5; i++){
        if(palpite[i] != senha[i])
            response++;
    }

    return response;
}

int joystick_control(float x, float y, uint8_t pressed, int *index){

    int response = 0;
    int direction = 0;
    int caractere;

    if(y > 0.9f)
        direction = 1;
    else
    if(y < 0.1f)
        direction = -1;
    else
    if(x < 0.1f)
        direction = 2;
    else
    if(x > 0.9f)
        direction = 3;

    switch (direction)
    {
    case 1:
        *index = *index + 1;
        response = 1;
    break;
    case -1:
        *index = *index - 1;
        response = 1;
    break;
    case 2:
        caractere = palpite[*index];
        print("\nup\n");
        printint(caractere);
        caractere++;
        if(caractere > '9')
            caractere = '0';
        palpite[*index] = caractere;
        response = 1;
    break;
    case 3:
        caractere = palpite[*index];
        print("\ndown\n");
        printint(caractere);
        caractere--;
        if(caractere < '0')
            caractere = '9';
        palpite[*index] = caractere;
        response = 1;
    break;
        
    default: 
    break;
    }

    if(*index >= 4)
        *index = 0;

    if(*index < 0)
        *index = 3;

    return response;
}

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
    

    print("\ns: ");
    print(senha);

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


    //_delay_ms(1000);
}

