#include <xc.h>
    //setting TX/RX

void UART_Initialize() {
    /* 
           Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX  
    */    
    TRISCbits.TRISC6 = 1; //pic18 to computer
    TRISCbits.TRISC7 = 1; //computer to pic18
    
    //  Setting baud rate (1200 for 1MHz)
    TXSTAbits.SYNC = 0; //0 = use async mode
    TXSTAbits.BRGH = 0; //high baud rate select bit
    BAUDCONbits.BRG16 = 0; //choose 8or16 bits for baud rate generator 8:SPBRG, 16:SPBRGH|SPBRG
    SPBRG = 12; //controls period of free-running timer
    
   //   Serial enable
    RCSTAbits.SPEN = 1; //enable async serial port
    TXSTAbits.TXEN = 1; //enable transmission
    RCSTAbits.CREN = 1; //enable continuous receive
}

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}
