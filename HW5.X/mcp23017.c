/* 
 * File:   mcp23017.c
 * Author: Noah DeMar
 *
 * Created on April 26, 2020, 7:14 PM
 */

#include "mcp23017.h"

void mcp_write(unsigned char address, unsigned char data) {
    i2c_master_start();         // Start bit
    i2c_master_send(MCP_WRITE_ADDR);//I2C write address 
    i2c_master_send(address);  // Register to be written
    i2c_master_send(data);      // Sending data
    i2c_master_stop();          // Sending stop bit
}

unsigned char mcp_read(unsigned char address) {
    i2c_master_start();         // Start bit
    i2c_master_send(MCP_WRITE_ADDR);// I2C write address
    i2c_master_send(address);  // Read address
    i2c_master_restart();       // Sending restart bit
    i2c_master_send(MCP_READ_ADDR); //I2C address read address
    
    unsigned char rdata = i2c_master_recv();      
    i2c_master_ack(1);
    i2c_master_stop();
    return rdata;
}

void mcp_init() {
    mcp_write(MCP_IODIRA, 0x00); // A pins
    mcp_write(MCP_IODIRB, 0xFF); // B pins
}

//clear pin
void mcp_clrP(unsigned char pin) {
    unsigned char curr_A = mcp_read(MCP_OLATA);
    mcp_write(MCP_OLATA, curr_A & ~(0x01 << pin));
}

//set pin
void mcp_setP(unsigned char pin) {
    mcp_write(MCP_OLATA, (0x01 << pin));
}


