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

#define Motor PORTDbits.RD7
#define Buzzer PORTDbits.RD6
#define Button PORTBbits.RB0

int valor;

void __interrupt() tes(void){
    if(TMR1IF){
        PIR1bits.TMR1IF = 0; //reseta o flag da interrup??o
        TMR1L = 0xDC;        //reinicia contagem com 3036
        TMR1H = 0x0B; 
    }
    else if ()
    {
        PIR1bits.ADIF = 0;
        valor = ADRESH;
        
    }
    
}


void main(void) {
    TRISA = 0b11111111; 
    TRISB = 0b00000001; 
    TRISD = 0;
    OPTION_REGbits.nRBPU = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR1IE = 1; //ativa o timer
    //Interrupção global
    
    T1CONbits.TMR1CS = 0;   //Define timer 1 como temporizador (Fosc/4)
    T1CONbits.T1CKPS0 = 1;  //bit pra configurar pre-escaler, nesta caso 1:8
    T1CONbits.T1CKPS1 = 1;  //bit pra configurar pre-escaler, nesta caso 1:8
    
    
    //4 mhz externo / 4 logo temos 1 mhz
    //1 mhz divido por 1 da config to T1CKPS0 + T1CKPS
    //logo, teremos 500 khz, então, usaremos 50 mil ciclos para contarmos até 100ms, o padrão deverá ser 13336
    
    //configs do timer
    //nosso valor inicial deverá ser 13336
    
    TMR1L = 01100000;
    TMR1H = 11010000;
    
    T1CONbits.TMR1ON = 1;
    
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
    //Configs dos AD
    
    ADCON0bits.ADCS0 = 0  ;   
    ADCON0bits.ADCS1 = 0  ;   
    
    ADCON1bits.ADFM = 0; //setando para 8 bits
    
    ADRESH = 0x00;
    ADCON0bits.ADON = 1; //liga nosso AD
    
    while(1){
        
    }
    
    return;
}
