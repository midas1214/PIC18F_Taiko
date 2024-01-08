#include <xc.h>
#include "uart.h"
    //setting TX/RX

char mystring[20];
int lenStr = 0;

void UART_Initialize(void) {
           
    /*       TODObasic   
           Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX 
     */  
    
    
    TRISCbits.TRISC6 = 0;  // TX pin as output            
    TRISCbits.TRISC7 = 1;  // RX pin as input       
    
    // Setting baud rate
    TXSTAbits.SYNC = 0;    // Asynchronous mode
    BAUDCONbits.BRG16 = 0;  // 8-bit Baud Rate Generator
    TXSTAbits.BRGH = 0;     // Low speed
    SPBRG = 12;             // For 1200 baud rate at 1MHz oscillator frequency
    
    // Serial enable
    RCSTAbits.SPEN = 1;     // Serial port enabled
    PIR1bits.TXIF = 0;      // Clear the TX interrupt flag
    PIR1bits.RCIF = 0;      // Clear the RX interrupt flag
    TXSTAbits.TXEN = 1;     // Enable transmission
    RCSTAbits.CREN = 1;     // Enable continuous reception
    PIE1bits.TXIE = 0;      // TX interrupt disabled
    IPR1bits.TXIP = 0;      // TX interrupt low priority
    PIE1bits.RCIE = 0;      // RX interrupt disabled
    IPR1bits.RCIP = 0;      // RX interrupt low priority
            
    }

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:20 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void ClearBuffer(){
    for(int i = 0; i < 20 ; i++)
        mystring[i] = '\0';
    lenStr = 0;
}

void MyusartRead()
{
    /* TODObasic: try to use UART_Write to finish this function */
        // Wait for a new character
    if (RCIF)
    {
        // Read the received character
        char receivedChar = RCREG;

        // Check for newline character
        if (receivedChar == '\n' || receivedChar == '\r')
        {
            // End the string with null character
            mystring[lenStr] = '\0';

            // Process the received string or perform any necessary actions
            // For example, you can print the received string back to the terminal
            UART_Write_Text("PIC18F: ");
            UART_Write_Text(mystring);
            UART_Write('\r');
            UART_Write('\n');

            // Clear the buffer for the next input
            ClearBuffer();

        }
        else
        {
            // Add the character to the string if there is space
            if (lenStr < sizeof(mystring) - 1)
            {
                mystring[lenStr] = receivedChar;
                lenStr++;
            }
        }   
        return ;        
    }

}

char * GetString(){
    return mystring;
}


// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void)
{
    if(RCIF)
    {
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        MyusartRead();
    }
    
   // process other interrupt sources here, if required
    return;
}