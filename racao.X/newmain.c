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

int valor; //valor da conversão

void __interrupt() tes(void){
    if (ADIF)
    {
        PIR1bits.ADIF = 0;
        valor = ADRESH;
        
    }
    
}


void main(void) {
    TRISA = 0b11111111; 
    TRISB = 0b00000001; //B0 vai ser botão de interrupção do despejamento de ração
    OPTION_REGbits.nRBPU = 0;
    
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1; //interrupção após converter
    
    
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
    //da maneira acima, todos os
    
    
    
    
    
    
    
    return;
}
