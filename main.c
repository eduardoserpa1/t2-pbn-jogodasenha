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

/*

    AUTORES: José Eduardo Rodrigues Serpa e Manoella Jarces de Azevedo
    DISCIPLINA: Programação de Baixo Nível

    Aviso

    Professor, tentamos de diversas maneiras implementar o sorteio da senha
    de maneira que sempre que o programa executar gere um novo número, porém
    o microprocessador não possui nenhum dado para utilizar na seed, como
    por exemplo no sistema operacional temos o relógio.

    Para isso pensei em utilizar da memoria RAM/ROM (EEPROM) para armazenar 
    tal informação, de modo que sempre que o programa for iniciado, é somado
    um grande número nessa memória estática, e logo em seguida é capturado 
    para ser utilizado como seed.

    Porém, procurei de diversas formas entender como funcionava esse armazen
    amento, mas infelizmente sem sucesso. 
    
    A memória precisaria apenas de 1 endereço para funcionar, sendo ele 
    preferencialmente de 8 bytes.

    A forma de aleatoriedade que resolvi utilizar é o tempo em que o jogador 
    se distrai ou lê a tela de menu, visto que a cada iteração do laço while
    (que é bem rapido), é gerada uma nova senha. Sabendo disso, dá pra 
    imaginar o quão improvável é que o jogador fique o mesmo tempo na tela 
    de menu por vezes consecutivas.

    De qualquer forma, a senha é pseudo-aleatória na medida do possível, como pode 
    observar pelo funcionamento do código.

    Encontrei esse repositório no github que me pareceu interessante, mas 
    de qualquer forma deixo ele aqui apenas por curiosidade e contribuição,
    encontrei ele enquanto pesquisava acerca das memórias e acho que tem 
    exemplos bem legais.

    https://github.com/Devilbinder/ATMEGA328P_C

    OBS: Deixei 1 print na linha 487 para printar sempre que uma nova senha é gerada.

*/


int senha_diferente();
int joystick_control(float,  float,  uint8_t,  int *);
int confere_senha();
void def_senha();
void draw_line(uint8_t index);
void led_on(uint8_t pcorreta, uint8_t pincorreta);
void led_off();


char senha[5] = {'0','0','0','0','\0'};
char palpite[5] = {'0','0','0','0','\0'};
char historico[5] = {'0','0','0','0','\0'};

int main(void)
{
    DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | 
            (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7) ;

    DDRB &= ~(1 << PB0);

    srand(time(NULL));

    int run = 1;

    def_senha();
    
    nokia_lcd_init();
    USART_Init();
    adc_init();
    
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


    uint8_t menu = 1;
    uint8_t manual = 0;
    uint8_t game = 0;
    uint8_t perdeu = 0;
    uint8_t ganhou = 0;
    uint8_t efeito_led_win = 0;
    uint8_t tutorial = 1;

    int index_seletor = 0;

    int frame = 0;

    int tentativas_restantes = 10;
    int sec = 30;

    while(run){

        frame++;

        uint8_t pressed = 0;


        adc_set_channel(0);
        float x = adc_read() * 0.0009765625;
        adc_set_channel(1);
        float y = adc_read() * 0.0009765625;   
        

        if(PINB & (1<<PB0))
            pressed = 1;
        else
            pressed = 0;


        if(menu){

            for(int i = 0; i < 4; i++){
                senha[i] = 48;
                palpite[i] = 48;
                historico[i] = 48;
            } 

            def_senha();

            tentativas_restantes = 10;
            sec = 30;

            nokia_lcd_clear();
            nokia_lcd_write_string("BEM VINDO AO   JOGO DA SENHA",1);
            nokia_lcd_set_cursor(0,24);
            nokia_lcd_write_string(" press button",1);
            nokia_lcd_render();

            if(pressed){
                _delay_ms(300);
                menu = 0;
                manual = 1;
                pressed = 0;
            }
        }

        if(manual){
            if(tutorial){

                tutorial = 0;

                nokia_lcd_clear();
                nokia_lcd_write_string("explicando os leds acima.",1);
                nokia_lcd_render();
                _delay_ms(3000);

                nokia_lcd_clear();
                nokia_lcd_write_string("4 leds da esquerda indicam  quantos numeros tem o valor correto no lugar correto.",1);
                nokia_lcd_render();
                _delay_ms(6000);

                nokia_lcd_clear();
                nokia_lcd_write_string("4 leds da direita indicam quantos numeros tem valor correto porem no  lugar errado.",1);
                nokia_lcd_render();
                _delay_ms(6000);

                nokia_lcd_clear();
                nokia_lcd_write_string("lembrete:a senha e composta sempre por 4 digitos diferentes de 0 a 9.",1);
                nokia_lcd_render();
                _delay_ms(6000);

                nokia_lcd_clear();
                nokia_lcd_write_string("portanto, nao faca apostas com numeros repetidos para nao perder vidas em vao.",1);
                nokia_lcd_render();
                _delay_ms(6000);

                nokia_lcd_clear();
                nokia_lcd_write_string("bom jogo      divirta-se :D",1);
                nokia_lcd_render();
                _delay_ms(5000);
            }

            manual = 0;
            game = 1;  
        }
        
        if(game){

            print("\nsenha: ");
            print(senha);

            if(frame%40==0)
                sec--;

            nokia_lcd_clear();

            nokia_lcd_set_cursor(0,0);
            nokia_lcd_write_string("current",1);

            nokia_lcd_set_cursor(8,12);
            nokia_lcd_write_string(palpite,1);

            nokia_lcd_set_cursor(52,0);
            nokia_lcd_write_string("last",1);

            nokia_lcd_set_cursor(52,12);
            nokia_lcd_write_string(historico,1);

            int delay = joystick_control(x,y,pressed,&index_seletor);

            if(delay)
                _delay_ms(300);

            draw_line(index_seletor);


            char time[8] = "time:xx\0"; 

            char tentativas[8] = "life:xx\0";


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

            if(sec < 30){

                if(pressed){

                    _delay_ms(500);

                    sec = 30;

                    int r = senha_diferente(); 

                    int val_correto = 0;

                    for(int i = 0; i < 4; i++){
                        for(int j = 0; j < 4; j++){
                            if(palpite[i] == senha[j])
                                val_correto++;
                        }
                    }

                    int pos_correta = 4 - r;

                    val_correto = val_correto - pos_correta;

                    led_on(pos_correta,val_correto);

                    for(int i = 0; i < 4; i++)
                        historico[i] = palpite[i];

                    if(pos_correta == 4){

                        _delay_ms(1000);

                        game = 0;
                        efeito_led_win = 1;
                        tentativas_restantes++;
                    }

                    tentativas_restantes--;

                    _delay_ms(500);
                }
            }

            if(sec <= 0 || tentativas_restantes <= 0){
                game = 0;
                perdeu = 1;
                _delay_ms(300);
                pressed = 0;
            }
            
        }
        if(perdeu){

            led_off();

            nokia_lcd_clear();

            nokia_lcd_set_cursor(24,0);
            nokia_lcd_write_string("YOU",2);

            nokia_lcd_set_cursor(20,16);
            nokia_lcd_write_string("LOSE",2);

            nokia_lcd_set_cursor(8,34);
            nokia_lcd_write_string("press button",1);

            nokia_lcd_render();

            if(pressed){
                perdeu = 0;
                menu = 1;

                _delay_ms(500);

                pressed = 0;
            }
            
        }

        if(efeito_led_win){

            pressed = 0;

            efeito_led_win = 0;

            ganhou = 1;

            led_off();

            _delay_ms(1000);

            for(int i = 0; i < 3; i++){
                led_on(4,4);
                _delay_ms(500);
                led_off();
                _delay_ms(500);
            }

            led_off();
        }

        if(ganhou){
            
            nokia_lcd_clear();

            nokia_lcd_set_cursor(24,0);
            nokia_lcd_write_string("YOU",2);

            nokia_lcd_set_cursor(24,16);
            nokia_lcd_write_string("WIN",2);

            nokia_lcd_set_cursor(8,34);
            nokia_lcd_write_string("press button",1);

            nokia_lcd_render();

            led_on(4,4);

            if(pressed){
                ganhou = 0;
                menu = 1;

                led_off();

                _delay_ms(500);

                pressed = 0;
            }
        }
    } 

    return 0;
}


int senha_diferente(){
    int response = 0;

    for(int i = 0; i < 4; i++){
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
        caractere++;
        if(caractere > '9')
            caractere = '0';
        palpite[*index] = caractere;
        response = 1;
    break;
    case 3:
        caractere = palpite[*index];
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
    print("\n");
    print(senha);
}

void draw_line(uint8_t index){

    switch (index)
    {
    case 0:
        nokia_lcd_drawline(8,20,13,20);
        nokia_lcd_drawline(8,10,13,10);
    break;
    
    case 1:
        nokia_lcd_drawline(14,20,19,20);
        nokia_lcd_drawline(14,10,19,10);
    break;

    case 2:
        nokia_lcd_drawline(20,20,25,20);
        nokia_lcd_drawline(20,10,25,10);
    break;

    case 3:
        nokia_lcd_drawline(26,20,31,20);
        nokia_lcd_drawline(26,10,31,10);
    break;
    
    default:
        
    break;
    }

}

void led_on(uint8_t pcorreta, uint8_t pincorreta){
    uint8_t leds_pcorreta = 0;
    uint8_t leds_pincorreta = 0;

    switch (pcorreta)
    {
    case 1:
        leds_pcorreta = 0b001;
    break;
    case 2:
        leds_pcorreta = 0b011;
    break;
    case 3:
        leds_pcorreta = 0b0111;
    break;
    case 4:
        leds_pcorreta = 0b1111;
    break;
    
    default:
        break;
    }

    switch (pincorreta)
    {
    case 1:
        leds_pincorreta = 0b001;
    break;
    case 2:
        leds_pincorreta = 0b011;
    break;
    case 3:
        leds_pincorreta = 0b0111;
    break;
    case 4:
        leds_pincorreta = 0b1111;
    break;
    
    default:
        break;
    }

    uint8_t leds_final = 0;

    leds_final = leds_pincorreta;
    leds_final = leds_final << 4;
    leds_final = leds_final + leds_pcorreta;

    PORTD = leds_final;
    
}

void led_off(){
        PORTD = 0x0;
}

