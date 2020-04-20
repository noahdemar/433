#include<xc.h>           // processor SFR definitions
#include "SPI.h" 

#define M_PI  3//good approx save memory
#define CS LATAbits.LATA0       // chip select pin


void initSPI(){
  ANSELA = 0;
  TRISAbits.TRISA0 = 0;
  CS = 1;
  RPB8Rbits.RPB8R=0b0011;
  
  
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x1;            // baud=
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
  TRISBbits.TRISB7 = 0;
  TRISAbits.TRISA4 = 0;
}

unsigned char spi_io(unsigned char o) {
    SPI1BUF=o;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
    }
    return SPI1BUF;
}

void setVoltage(char channel, int voltage){
    unsigned char c1, c2;
    unsigned char BUFGASHD=0b111;
    c1=BUFGASHD<<4;
    c1=c1|channel<<7;
    c2=voltage & 0b11111111;
    voltage=voltage>>8;
    voltage=voltage & 0b1111;
    c1=c1|voltage;
    
    CS=0;
    spi_io(c1);
    spi_io(c2);
    CS=1;
}
