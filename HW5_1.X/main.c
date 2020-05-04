#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h>
#include "i2c_master_noint.h" // I2C interface
#include "mcp23017.h" // MCP23017 IO expander
#include "ssd1306.h"  // SSD1306 OLED display 
#include "ws2812b.h" // Gamer LEDS


// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

#define CLOCK_RATE 48000000

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations


void sleep_ms(double ms) {
    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
    // remember the core timer runs at half the sysclk
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < CLOCK_RATE / 2000 * ms) {;}
}

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // A4 output: User LED
    TRISBbits.TRISB4 = 1; // B4 input: User button
    LATAbits.LATA4 = 0;   // Set A4 to low

    // Setup I2C
    i2c_master_setup();
    mcp_init();
    ssd1306_setup();
    
    __builtin_enable_interrupts();
    
    int k = 1000;
    char message[50];
    char fps[25];
    
       int count=0;
       int last_tick=0;
       int current_tick=0;
   
    ws2812b_setup();
    //Brightness
    int i = 10;
    //counter
    int j = 0;
    
    //Color vars 
    wsColor colors[4];
    //intensity level
    float level = 0.05;
    //hue phase shifts
    float hue[4] = {0,90,180,360};
    
    while (1){
    for (j=0;j<4;j++){
        //cycle hues
        hue[j] = hue[j]++;
        //reset over 360 color wheel
        if (hue[j]>360){hue[j]=0.0;}
        colors[j] = HSBtoRGB(hue[j],i,level);

    }
        ws2812b_setColor(colors,4);

        
        unsigned char b = mcp_read(MCP_GPIOB);
        //light up the LED if button GPA7 is pressed
        if (b & 0x01) {
            mcp_clrP(7);
        } else {
            mcp_setP(7);
            count++;
        }
//////http://engineeringnotes.blogspot.com/2013/07/using-ssd1306-textgraphics-display.html helpful LED screen resource
////        char ii;
////        for (ii = 1; ii < 1024; ii++){
////        ssd1306_drawPixel(1<<ii, 20+ii, 1);
////        ssd1306_update();
////        ssd1306_drawPixel(40+ii, ii, 1);
////        ssd1306_update();
////        }
       sprintf(message, "LED TEST %d",count);
       print_str(10, 10, message);
       ssd1306_update();
       
    //FPS TIMER
     current_tick=_CP0_GET_COUNT();
     if(current_tick % 10 ==9){
        sprintf(fps, "fps:%g", 24000000/((float)current_tick - last_tick));
        print_str(10,20, fps);
        ssd1306_update();
        last_tick=_CP0_GET_COUNT();
     }
     last_tick = current_tick;
    }        
               
               
        LATAbits.LATA4 = !LATAbits.LATA4; // Flip led
        sleep_ms(200);
    }

void print_letter(short x,short y,char letter){
    letter=letter - 0x20;
    int i=0;
    for(i;i<5;i++){
        char col=ASCII[letter][i];
        int j=0;
        for(j;j<8;j++){
            if ((col >> j & 1) == 1){
                ssd1306_drawPixel(x+i,y+j,1);
            }
            else{ssd1306_drawPixel(x+i,y+j,0);
            }
        }
    }
}

void print_str(short x,short y,char* str){

    int j=0;
    while(str[j] && j <26){
        print_letter(x+j*5,y,str[j]);
        j++;
    }

}
