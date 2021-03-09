#include "ram.h"

#include <stddef.h>

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#define DDR_RAM  DDRB
#define PORT_RAM PORTB
#define MREQ     PINB0
#define WE       PINB1
#define RD       PINB2

#define WAIT     _delay_us(1)

void ram_init()
{
    DDR_RAM |= (1 << MREQ) | (1 << WE) | (1 << RD);   // set MREQ, WE and RD as outputs
    PORT_RAM |= (1 << MREQ) | (1 << WE) | (1 << RD);  // deactivate them (set to 1)
}

void ram_reset()
{
    // set pins as high impedance
    DDRB &= ~(1 << PINB3);  // A9
    DDRD &= ~0b11111101;    // A8, A10..15
    DDRA = 0x0;             // A0..A7
    DDRC = 0x0;             // D
}

void ram_set_addr(uint16_t addr)
{
    // set address pins as output
    DDRB |= (1 << PINB3);  // A9
    DDRD |= 0b11111101;    // A8, A10..15
    DDRA = 0xff;           // A0..A7

    // set address pins
    PORTA = addr & 0xff;         // A0..A7
    PORTD &= ~0b11111101;        // A8, A10..15
    PORTD |= (addr >> 8) & 0b11111101;
    if ((addr >> 9) & 1)
        PORTB |= (1 << PINB3);
    else
        PORTB &= ~(1 << PINB3);
}

uint16_t ram_get_addr()
{
    // set address pins as input
    DDRB &= ~(1 << PINB3);  // A9
    DDRD &= ~0b11111101;    // A8, A10..15
    DDRA = 0x0;             // A0..A7
    
    // get address
    uint16_t addr = (uint16_t) PINA | ((uint16_t) PIND << 8);   // all pins except A9
    if (PINB & (1 << PINB3))
        addr |= (1 << 9);
    else
        addr &= (1 << 9);
    
    return addr;
}

void ram_set_data(uint8_t data)
{
    DDRC = 0xff;
    PORTC = data;
}

uint8_t ram_get_data()
{
    return PINC;
}

uint8_t
ram_write_byte(uint16_t addr, uint8_t data)
{
    uint8_t written, check_again;
    do {
        ram_set_addr(addr);
        ram_set_data(data);
        PORT_RAM &= ~(1 << MREQ);
        PORT_RAM &= ~(1 << WE);
        WAIT;
        PORT_RAM |= (1 << WE) | (1 << MREQ);
        PORTC = 0;
        ram_reset();
        _delay_us(100);
        written = ram_read_byte(addr);
        _delay_us(100);
        check_again = ram_read_byte(addr);
    } while (written != data || data != check_again);
    return data;
}

uint8_t ram_read_byte(uint16_t addr)
{
    ram_set_addr(addr);
    DDRC = 0;
    PORT_RAM &= ~(1 << MREQ);
    PORT_RAM &= ~(1 << RD);
    WAIT;
    volatile uint8_t data = ram_get_data();
    PORT_RAM |= (1 << RD) | (1 << MREQ);
    ram_reset();
    return data;
}

void ram_write_stream_start()
{
}

void ram_write_byte_stream(uint16_t addr, uint8_t byte)
{
    ram_set_addr(addr);
    ram_set_data(byte);
    PORT_RAM &= ~(1 << MREQ);
    PORT_RAM &= ~(1 << WE);
    WAIT;
    PORT_RAM |= (1 << WE) | (1 << MREQ);
    WAIT;
}

void ram_write_stream_end()
{
    PORTC = 0;
    ram_reset();
}

void ram_read_stream_start()
{
    DDRC = 0;
}

uint8_t ram_read_byte_stream(uint16_t addr)
{
    ram_set_addr(addr);
    PORT_RAM &= ~(1 << MREQ);
    PORT_RAM &= ~(1 << RD);
    WAIT;
    uint8_t byte = ram_get_data();
    PORT_RAM |= (1 << RD) | (1 << MREQ);
    WAIT;
    return byte;
}

void ram_read_stream_end()
{
    ram_reset();
}
