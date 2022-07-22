/*
 * File:   newmain.c
 * Author: Lucca Machado da silva (19208083)
 *
 * Created on 19 de Julho de 2022, 10:44
 */

#include <xc.h>
#include <pic16f877a.h>  
#include <stdio.h>


#pragma config WDTE = OFF
#pragma config FOSC = HS
#pragma config PWRTE = ON
#pragma config BOREN = ON

#define _XTAL_FREQ 4000000

#define motor PORTDbits.RD7
#define buzzer PORTDbits.RD6
#define btn_emergencia PORTBbits.RB0
#define btn_despejo PORTBbits.RB1
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
int aux_despejo;
int interrupcao;
int conta = 0;

void __interrupt() tes(void){
    if(TMR1IF){
        PIR1bits.TMR1IF = 0; //reseta o flag da interrupcao
        TMR1L = 0xDC;
        TMR1H = 0X0B;
        conta++;
        if(conta == 8){
            T1CONbits.TMR1ON = 0;
            conta = 0;
            aux_despejo = 1;
        }
        
    }
    else if (INTCONbits.INTF)
    {
        INTCONbits.INTF = 0;
        motor = 0;
        Lcd_Clear();  //limpa LCD
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("INTERROMPIDO...");
        interrupcao = 1;
        aux_despejo = 1;
    }
    return;
}

int PesoCambuca(){
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
int PesoEstoque(){
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
    TRISB = 0b00000011;
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
    
    // ---------TIMER--------- //
    //configs do timer
    PIE1bits.TMR1IE = 1; //ativa o timer
    T1CONbits.TMR1CS = 0;   //Define timer 1 como temporizador (Fosc/4)
    T1CONbits.T1CKPS0 = 1;  //bit pra configurar pre-escaler, neste caso 1:8
    T1CONbits.T1CKPS1 = 1;  //bit pra configurar pre-escaler, neste caso 1:8
    
    //4 mhz externo / 4 logo temos 1 mhz
    //1 mhz divido por 8 da config to T1CKPS0 + T1CKPS
    //logo, teremos 125 khz e precisaremos de 62500 ciclos para chegarmos em 500ms, 
    //então o valor inicial será 3036
    TMR1L = 0xDC;
    TMR1H = 0X0B;
     
    //Configs dos AD, desta maneira AN0 e AN1 serão Analogicos
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
    
    Lcd_Init();
    Lcd_Padrao();
    resistencia = 1;
    interrupcao = 0;
    aux_despejo = 0;
    while(1){
       peso_cambuca = PesoCambuca();
       peso_estoque = PesoEstoque();
        if(interrupcao == 1){
            interrupcao = 0;
            __delay_ms(2000);
            Lcd_Padrao();
        }
       if(peso_cambuca <= 51)
       {
           buzzer = 1 ;
           __delay_ms(1000);
           buzzer = 0;
       }
       
        motor = 0;
        if(btn_despejo == 0){
            aux_despejo = 0;
            Lcd_Clear();  //limpa LCD
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("DESPEJANDO...");
            T1CONbits.TMR1ON = 1;
            while (aux_despejo == 0){
                motor = 1;
            };
            motor = 0;
            if(interrupcao == 0){
                Lcd_Clear();  //limpa LCD
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("RACAO COLOCADA.");
            }
            __delay_ms(2000);
            Lcd_Padrao();
        }
    }
    return;
}
