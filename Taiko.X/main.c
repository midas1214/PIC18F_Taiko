#include "setting.h"
#include "uart.h"
#include <stdlib.h>
#include <pic18f4520.h>
#include "stdio.h"
#include "string.h"
#define _XTAL_FREQ 100000

void main(void) 
{   
    OSCCONbits.IRCF = 0b100;  // Internal Oscillator Frequency, Fosc = 1 MHz, Tosc = 1 µs
    
    //////////////////////////////////////////////////
    //////////// uart setting (in uart.c) ////////////
    //////////////////////////////////////////////////
    UART_Initialize();
    
    
    //////////////////////////////////////////////////
    //////////////////// adc setting /////////////////
    //////////////////////////////////////////////////
    TRISA = 0xff;		// Set as input port
    ADCON1 = 0x0f;  	// Ref vtg is VDD & Configure pin as analog pin 
    ADFM = 1 ;          // Right Justifie
    ADCON2bits.ADCS = 7; // 
    ADRESH=0;  			// Flush ADC output Register
    ADRESL=0;  

    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1100; //AN0 AN1 AN2 as analog input
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b000;  //????000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b001;  //Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1; //enable A/D
    ADCON2bits.ADFM = 0;    //left justified 
    
    //step2
    PIE1bits.ADIE = 1; //enable A/D interrupt
    PIR1bits.ADIF = 0; //clear A/D interrupt bit

    //step3
    ADCON0bits.GO = 1; //set go/done bit
    
    
    //////////////////////////////////////////////////
    ////////////// interrupt setting /////////////////
    //////////////////////////////////////////////////
    INTCONbits.PEIE = 1; //enable peripheral interrupt
    INTCONbits.GIE = 1; //enable interrupt
    INTCONbits.GIEH = 1;    //enable high priority interrupt
    INTCONbits.GIEL = 1; 
    RCONbits.IPEN = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    
    //////////////////////////////////////////////////
    //////////////////// PWM setting /////////////////
    //////////////////////////////////////////////////
    T2CONbits.TMR2ON = 0b1; // timer2 on
    T2CONbits.T2CKPS = 0b11; // prescaler 1 : 16
    T2CONbits.T2OUTPS = 0b0001; // postscaler 1 : 2
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISCbits.RC2 = 0;
    LATC = 15;
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler) * (TMR2 postscaler)
     * = (0x9b + 1) * 4 * 1µs * 16 * 2
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;
    
    CCPR1L = 0x00;
    CCP1CONbits.DC1B = 0b00;
    
    
    // start running
    while(1);
    return;
}

void __interrupt(high_priority) Hi_ISR(void)
{
    // value of pressure
    int value = (ADRESH<<2) | (ADRESL>>6);

    // change Analog channel
    ADCON0bits.CHS = ADCON0bits.CHS==0 ? 1 : 0;
    
    // threshold
    if ((value > 500 && ADCON0bits.CHS == 0) || (value >= 450 && ADCON0bits.CHS==1)){   
        // calculate pwm duty cycle, map to 16 - 75 (math by Dog Su)
        int x = 0;
        if (ADCON0bits.CHS==1)
        {
            x = ((value - 16) / 59) * (1000-450) + 450;
        }
        else
        {
            x = (3 * value - 1468) / 2;
        }
         /*
         * Duty cycle
         * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler) * (TMR2 postscaler)
         */
        CCPR1L = (x >> 2);
        CCP1CONbits.DC1B = x % 4;
        
        // send to computer
        char c = ADCON0bits.CHS==0 ? 'j' : 'k'; // j for inner drum, k for outer drum
        UART_Write(c);
        __delay_ms(100);
    }
    
    // decay the led light
    CCP1CONbits.DC1B --;
    if (CCP1CONbits.DC1B <= 0)
    {
        CCPR1L --;
        CCP1CONbits.DC1B = 3;
    }
    if (CCPR1L <= 1)
    {
        CCPR1L = 1;
        CCP1CONbits.DC1B = 3;
    }  
    
    //clear adc flag bit
    PIR1bits.ADIF = 0;
    __delay_ms(1);
    ADCON0bits.GO = 1;
    
    return ;
}