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

int valor;

int interrupcao;
int test = 0;

void __interrupt() tes(void){
    if(TMR1IF){
         PIR1bits.TMR1IF = 0; //reseta o flag da interrupcao
         TMR1L = 0xDC;
         TMR1H = 0X0B;  
         test++;
    }
    else if (INTCONbits.INTF)
    {
        INTCONbits.INTF = 0;
        motor = 0;
        Lcd_Clear();  //limpa LCD
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("INTERROMPIDO...");
        interrupcao = 1;
        
    }
    return;
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
    PIE1bits.TMR1IE = 1; //ativa o timer
    T1CONbits.TMR1CS = 0    ;   //Define timer 1 como temporizador (Fosc/4)
    T1CONbits.T1CKPS0 = 1;  //bit pra configurar pre-escaler, neste caso 1:8
    T1CONbits.T1CKPS1 = 1;  //bit pra configurar pre-escaler, neste caso 1:8
    
    TMR1L = 0xDC;
    TMR1H = 0X0B;

    //4 mhz externo / 4 logo temos 1 mhz
    //1 mhz divido por 8 da config to T1CKPS0 + T1CKPS
    //logo, teremos 125 khz e precisaremos de 62500 ciclos para chegarmos em 500ms, 
    //então o valor inicial será 3036
     
    
    //configs do timer
    //nosso valor inicial deverá ser 3036
    
    
    //T1CONbits.TMR1ON = 1; //liga o timer
    
    /*
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
	// ---------TIMER--------- //
	
	
    //Configs dos AD, desta maneira todos os AN0 até AN7 serão Analogicos
    
    ADCON0bits.ADCS0 = 0  ;   
    ADCON0bits.ADCS1 = 0  ;   
    
    ADCON1bits.ADFM = 0; //setando para 8 bits, caso queira 10 bits bote como 1
    
    ADRESH = 0x00;
    ADCON0bits.ADON = 1; //liga nosso AD
    */
    
    Lcd_Init();
    Lcd_Clear();  //limpa LCD
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("SIGMA DOG");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("GRINDSET");
    resistencia = 1;
    while(1){
        if(interrupcao == 1){
            interrupcao = 0;
        }
        motor = 0;
        if(btn_despejo == 0){
            Lcd_Clear();  //limpa LCD
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("DESPEJANDO...");
            motor = 1;
            T1CONbits.TMR1ON = 1;
            motor = 0;
        }
        
        /*if(btn_despejo == 0){
            Lcd_Clear();  //limpa LCD
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("DESPEJANDO...");
            motor = 1;
            while(btn_despejo == 0){
                if(interrupcao == 1){
                    interrupcao = 0;
         return;
                }
                motor = 1;
            }
            Lcd_Clear();  //limpa LCD
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("SIGMA DOG");
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String("GRINDSET");
        } */
    }
    
    return ;
}
