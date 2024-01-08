/*
 * File:   main.c
 * Author: Midas
 *
 * Created on 2024?1?8?, ?? 3:14
 */


#include <xc.h>
#include "setting.h"
#include <stdlib.h>

#include <pic18f4520.h>
#include "stdio.h"
#include "string.h"
// using namespace std;
#define _XTAL_FREQ 100000
char str[20];
int now = 1;
int counter = 0;

void main(void)
{
   
   // INTERRUPT_Initialize();
    UART_Initialize();
    TRISA = 0xff; // Set as input port
    ADCON1 = 0x0f;   // Ref vtg is VDD & Configure pin as analog pin
    // ADCON2 = 0x92;  
    ADFM = 1 ;          // Right Justifie
    ADCON2bits.ADCS = 7; //
    ADRESH=0;   // Flush ADC output Register
    ADRESL=0;  
   
    TRISAbits.RA0 = 1;       //analog input port
   
   
   
    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1100; //AN0 ?analog input,???? digital
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b000;  //????000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b001;  //Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1; //enable A/D
    ADCON2bits.ADFM = 0;    //left justified
   
    OSCCONbits.IRCF = 0b100;
   
    //step2
    PIE1bits.ADIE = 1; //enable A/D interrupt
    PIR1bits.ADIF = 0; //clear A/D interrupt bit
    INTCONbits.PEIE = 1; //enable peripheral interrupt
    INTCONbits.GIE = 1; //enable interrupt
   
    INTCONbits.GIEH = 1;    //enable high priority interrupt
    INTCONbits.GIEL = 1;

    //step3
    ADCON0bits.GO = 1; //set go/done bit
   
   
    //TRISBbits.RB0 = 1;
   
    //T1CON = 0x30; // pre = 8
    //PIE1bits.TMR1IE = 1;
    //PIR1bits.TMR1IF = 0;
   
   
    RCONbits.IPEN = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    INTCONbits.GIE = 1;
   
   
    while(1);
    return;
}

void __interrupt(high_priority) Hi_ISR(void)
{
    int value = ADRESH;
   
    if (ADCON0bits.CHS == 0b0000){
        ADCON0bits.CHS = 0b0001;
        now = 2;
    }
    else{
        ADCON0bits.CHS = 0b0000;
        now = 1;
    }//????
   
    if (value >100){
        if (now == 1){
            char c2[10] ;
            sprintf(c2, "1 %d \r\n", value);
            UART_Write_Text(c2);
        }
        else{
            char c2[10] ;
            sprintf(c2, "2 %d \r\n", value);

            UART_Write_Text(c2);
        }
    }
   
    //clear flag bit
    PIR1bits.ADIF = 0;
   
    //step5 & go back step3
    __delay_ms(1);
   
    ADCON0bits.GO = 1;
    
    return ;
}
