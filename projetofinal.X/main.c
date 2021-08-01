/*
 * File:   main.c
 * Author: Orlando Beco
 *
 * Created on 31 de Julho de 2021, 11:25
 */

#include <pic18f4520.h>
#include "config.h"
#include "bits.h"
#include "lcd.h" 
#include "keypad.h"
#include "delay.h"
#include "pwm.h"
#include "adc.h"

int estadoPorta, estadoVent, estadoLuz, estadoAquec;
unsigned int tecla;
unsigned char vel;

void escolheOpcoes(void);   //Escolha de opções através do teclado
void luz(void);             //Liga/Desliga Luz
void vent(void);            //Liga/Desliga o Ventilador
void porta(void);           //Abre/Fecha a fechadura da porta
void aquec(void);           //Liga/Desliga o Aquecedor
void lcdPrint(void);        //Mostra as informações fixas no LCD
void lcdPrintInicio(void);  //Mostra os estados iniciais dos componentes
void contrVent(void);       //Realiza o controle da ventoinha

void main(void) {
    ADCON1 = 0x06;          //configurações iniciais
    TRISA = 0xC3;
    TRISB = 0x07;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    tecla = 16;             //inicialização de variáveis
    estadoPorta = 0;
    estadoVent = 0;
    estadoLuz = 0;
    estadoAquec = 0;

    adcInit();              //inicialização de funções
    pwmInit();
    lcdInit();
    kpInit();
    lcdPrintInicio();

    for (;;) {              //loop infinito
        escolheOpcoes();    //aguarda leitura de opção
        contrVent();        //controla velocidade da ventoinha
    }
}

void escolheOpcoes(void) {
    kpDebounce();
    if (kpRead() != tecla) {
        tecla = kpRead();
        for (int aux = 0; aux < 12; aux++) {
            if (bitTst(tecla, aux)) {
                if (aux == 11) {        //tecla 3 
                    luz();
                }
                if (aux == 10) {        //tecla 6
                    vent();
                }
                if (aux == 9) {         //tecla 9
                    porta();
                }
                if (aux == 8) {         //tecla #
                    aquec();
                }
            }
        }
    }
}

void luz(void) {                
    if (estadoLuz == 1) {       // se ligado no acionamento
        estadoLuz = 0;
        PORTB = 0x00;           // desliga
        lcdCommand(L_L1 + 10);
        lcd_str("OFF   ");
    } else {                    // se não
        estadoLuz = 1;
        PORTB = 0xF8;           // liga
        lcdCommand(L_L1 + 10);
        lcd_str("ON    ");
        ;
    }
}

void vent(void) {
    if (estadoVent == 1) {      // se ligado no acionamento
        estadoVent = 0;         // desliga
        lcdCommand(L_L2 + 10);
        lcd_str("OFF   ");
    } else {                    // se não
        estadoVent = 1;         // liga
        lcdCommand(L_L2 + 10);
        lcd_str("ON  ");
    }
}

void contrVent(void) {
    if (estadoVent) {                   // se ligado
        vel = (adcRead(0)*20) / 204;    // le o potenciômetro 1
        pwmSet1(vel);                   // controla a velocidade
    } else {                            // se não
        pwmSet1(0);                     // desliga
    }
}

void porta(void) {
    if (estadoPorta == 1) {     // se fechada no acionamento
        estadoPorta = 0;
        PORTCbits.RC0 = 0;      // manda abrir
        PORTEbits.RE0 = 0;
        lcdCommand(L_L3 + 10);
        lcd_str("OPEN   ");
    } else {                    // se aberta no acionamento
        estadoPorta = 1;
        PORTCbits.RC0 = 1;      // manda fechar
        PORTEbits.RE0 = 1;
        lcdCommand(L_L3 + 10);
        lcd_str("CLOSED ");
    }
}

void aquec(void) {
    if (estadoAquec == 1) {         // se ligado no acionamento
        estadoAquec = 0;            
        PORTCbits.RC5 = 0;          // desliga
        lcdCommand(L_L4 + 10);
        lcd_str("OFF ");
    } else {                        // se não
        estadoAquec = 1;            
        PORTCbits.RC5 = 1;          // liga
        lcdCommand(L_L4 + 10);
        lcd_str("ON  ");
    }
}

void lcdPrint(void) {       // Informações que não mudam
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcd_str("3-Luz:");
    lcdCommand(L_L2);
    lcd_str("6-Vent:");
    lcdCommand(L_L3);
    lcd_str("9-Porta:");
    lcdCommand(L_L4);
    lcd_str("#-Aquec:");
}

void lcdPrintInicio(void) {         // Informações de inicialização
    lcdCommand(L_CLR);
    lcdCommand(0x0C);
    lcdCommand(0x40);
    lcdCommand(L_L1);
    lcd_str("Inicializando...");
    atraso_s(2);
    lcdCommand(L_L2);
    lcd_str("   Smart Room   ");
    atraso_s(2);
    lcdCommand(L_L4);
    lcd_str("by: Orlando Beco");
    atraso_s(3);
    lcdPrint();
    lcdCommand(L_L1 + 10);          // Estados iniciais dos componentes
    lcd_str("OFF");
    lcdCommand(L_L2 + 10);
    lcd_str("OFF");
    lcdCommand(L_L3 + 10);
    lcd_str("OPEN");
    lcdCommand(L_L4 + 10);
    lcd_str("OFF");
}
