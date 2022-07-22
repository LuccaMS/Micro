/*
 * File:   newmain.c
 * Author: Lucca Machado da silva (19208083)
 *
 * Created on 19 de Julho de 2022, 10:44
 */

#include <xc.h>
#include <pic16f877a.h>  
#include <stdio.h>


#pragma config WDTE = ON
#pragma config FOSC = HS
#pragma config PWRTE = ON
#pragma config BOREN = ON

#define _XTAL_FREQ 4000000

#define motor PORTDbits.RD7
#define buzzer PORTDbits.RD6
#define btn_emergencia PORTBbits.RB0
#define btn_reinicio PORTBbits.RB1
#define btn_despejo PORTBbits.RB2
#define resistencia PORTDbits.RD5

/**
 * configs do LCD
 */

#define RS RC0
#define RW RC1
#define EN RC2
#define D4 RC4
#define D5 RC5
#define D6 RC6
#define D7 RC7

#include "LCD.h"

int peso_cambuca = 0;
int peso_estoque = 0;
int valor;
int aux_timer = 0;
int interrupcao;
int conta = 0;

void __interrupt() tes(void){
    if(TMR1IF){
        PIR1bits.TMR1IF = 0; //reseta o flag da interrupcao
        TMR1L = 0xDC;
        TMR1H = 0X0B;
        CLRWDT(); //limpando o watchdog
        conta++;
        if(conta == 8){
            T1CONbits.TMR1ON = 0;
            conta = 0;
            aux_timer = 1;
        }
        
    }
    else if (INTCONbits.INTF)
    {
        INTCONbits.INTF = 0;
        motor = 0;
        CLRWDT();
        Lcd_Clear();  //limpa LCD
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("FUNCIONAMENTO");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String("INTERROMPIDO");
        interrupcao = 1;
    }
    return;
}

int PesoEstoque(){
     //Seleciona canal de entrada 0 como entrada anal?gica
    int peso;
    __delay_us(10);
    ADCON0bits.CHS0 = 0; //configura canal 0 como entrada anal?gica
    __delay_us(10);
    ADCON0bits.CHS1 = 0; //configura canal 0 como entrada anal?gica
    __delay_us(10);
    ADCON0bits.CHS2 = 0; //configura canal 0 como entrada anal?gica
    __delay_us(10);
    ADCON0bits.GO = 1;  //converte
    __delay_us(10);     //tempo de convers?o
    peso = ADRESH;     // passa valores convertido do reg para a vari?vel
    return peso;
}
int PesoCambuca(){
     //Seleciona canal de entrada 0 como entrada anal?gica
    int peso;
    __delay_us(10);
    ADCON0bits.CHS0 = 1; //configura canal 0 como entrada anal?gica
    __delay_us(10);
    ADCON0bits.CHS1 = 0; //configura canal 0 como entrada anal?gica
    __delay_us(10);
    ADCON0bits.CHS2 = 0; //configura canal 0 como entrada anal?gica
    __delay_us(10);
    ADCON0bits.GO = 1;  //converte
    __delay_us(10);     //tempo de convers?o
    peso = ADRESH;     // passa valores convertido do reg para a vari?vel
    return peso;
}

void Lcd_Padrao(){
    Lcd_Clear();  //limpa LCD
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("SIGMA DOG");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("GRINDSET");
}

void main(void) {
    TRISA = 0b11111111; 
    TRISB = 0b00000111;
    TRISC = 0;
    TRISD = 0;
    OPTION_REG = 0b00111111;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    INTCONbits.GIE = 1; //ativa as interrupt
    INTCONbits.PEIE = 1;
    INTCONbits.INTE=1; //RB0 como interrupt
    
    PIE1bits.TMR1IE = 1; //ativa o timer
    T1CONbits.TMR1CS = 0;   //Define timer 1 como temporizador (Fosc/4)
   
    T1CONbits.T1CKPS0 = 1;  //Pre-escaler para 1:8
    T1CONbits.T1CKPS1 = 1;  //Pre-escaler para 1:8
    
    //4 mhz externo / 4 logo temos 1 mhz
    //1 mhz divido por 8 da config to T1CKPS0 + T1CKPS
    //logo, teremos 125 khz e precisaremos de 62500 ciclos para chegarmos em 500ms, 
    //ent�o o valor inicial ser� 3036
    TMR1L = 0xDC;
    TMR1H = 0X0B;
     
    //Configs dos AD, desta maneira AN0 e AN1 ser�o Analogicos
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
	
	ADCON0bits.ADCS0 = 0;   
    ADCON0bits.ADCS1 = 0;   
    
    ADCON1bits.ADFM = 0; //setando para 8 bits, caso queira 10 bits bote como 1
    
    //inicializa registradores do AD
    ADRESL = 0x00;          //inicializar valor anal?gico com 0
    ADRESH = 0x00;          
    
    ADCON0bits.ADON = 1; //liga nosso AD
    
    
    //abaixo estamos setando as configs do Watchdog time, apesar de acima j� termos configurado, s� para ter certeza
    OPTION_REGbits.PSA = 1; 
    
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;
    // 72 ms para o watchdog time
    
    CLRWDT();
    Lcd_Init();
    Lcd_Padrao();
    resistencia = 1;
    interrupcao = 0;
    aux_timer = 0;
    while(1){
        CLRWDT();
        while(interrupcao == 1){
            // n�o iremos usar o watchdog aqui, porque se ele apertar na interrup��o e demorar muito at� apertar o reinicio novamente
            //iremos resetar o microcontrolador
            if(btn_reinicio == 0){
                interrupcao = 0;
                Lcd_Padrao();
            }
        }
        if(btn_despejo == 0){
            CLRWDT();
            aux_timer = 0;
            Lcd_Clear();  
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("DESPEJANDO...");
            T1CONbits.TMR1ON = 1;
            while (aux_timer == 0 && interrupcao == 0){
                motor = 1;
            };
            motor = 0;
            if(interrupcao == 0){
                Lcd_Clear(); 
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("RACAO COLOCADA.");
                __delay_ms(2000);
                Lcd_Padrao();
            }     
        }
        if(PesoCambuca()<= 51 && PesoEstoque() > 51 && interrupcao == 0){
            CLRWDT();
            Lcd_Clear();  
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("DESPEJANDO...");
            while(PesoCambuca() <= 204 && PesoEstoque() > 51){ //80% e 20%
                //se ele estiver preso aqui por muito tempo o watchdog vai resetarm devido ao algum eventual bug de sensores.
                CLRWDT();
                if(interrupcao == 1){
                    break;
                }
                motor = 1;
            }
            if(interrupcao == 0){
                motor = 0;
                Lcd_Clear(); 
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("RACAO COLOCADA.");
                __delay_ms(2000);
                Lcd_Padrao();
            } 
        }
       if(PesoEstoque() <= 51)
       {
           CLRWDT();
           aux_timer = 0;
           buzzer = 1;
           Lcd_Clear();  
           Lcd_Set_Cursor(1,1);
           Lcd_Write_String("ESTOQUE EM NIVEL");
           Lcd_Set_Cursor(2,1);
           Lcd_Write_String("BAIXO...");
           T1CONbits.TMR1ON = 1;
           while(aux_timer == 0 && btn_despejo == 1){
               if(interrupcao == 1){
                    break;
               }
               else if(PesoEstoque() > 51){
                   break;
               }
            };
           buzzer = 0;
           if(interrupcao == 0){
                Lcd_Padrao();
                __delay_ms(2000);
           }
       }
    }
    return;
}
