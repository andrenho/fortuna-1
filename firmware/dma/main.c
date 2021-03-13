#include <stddef.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "ram.h"
#include "spi.h"

#if defined(RUN_TESTS) || defined(DEBUG_UART)
#include <stdio.h>
#include <avr/pgmspace.h>
#include "serial.h"
#endif

#define CMD_TEST        0x1
#define CMD_READ_BYTE   0x2
#define CMD_WRITE_BYTE  0x3
#define CMD_READ_BLOCK  0x4
#define CMD_WRITE_BLOCK 0x5
#define CMD_READ_DATA   0x6
#define CMD_WRITE_DATA  0x7
#define CMD_READ_ADDR   0x8
#define CMD_READ_MBUS   0x9

int main()
{
    spi_init();
    ram_init();

#if defined(RUN_TESTS) || defined(DEBUG_UART)
    serial_init();
    printf_P(PSTR(/* "\e[1;1H\e[2J" */ "\nDMA initialized.\n"));
#endif
#ifdef RUN_TESTS
    extern void run_tests();
    run_tests();
#endif

    while (1) {
        uint8_t r = spi_swap(0xff);
        wdt_enable(WDTO_2S);
        spi_activate();
        switch (r) {
            case CMD_TEST:
                spi_ready();
                spi_swap('H');
                spi_swap('e');
                spi_swap('l');
                spi_swap('l');
                spi_swap('o');
                spi_swap('\n');
                spi_done();
#ifdef DEBUG_UART
                printf_P(PSTR("Returned 'Hello' message.\n"));
                spi_done();
#endif
                break;
            case CMD_READ_BYTE: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    spi_ready();
                    uint8_t data = ram_read_byte(addr);
                    spi_swap(data);
                    spi_swap(data);
                    spi_done();
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Read byte 0x%02X from address 0x%X.\n"), data, addr);
                    spi_done();
#endif
                }
                break;
            case CMD_WRITE_BYTE: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint8_t data = spi_swap(0xff);
                    uint8_t written = ram_write_byte(addr, data);
                    spi_ready();
                    spi_swap(written);
                    spi_done();
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Written byte 0x%02X to address 0x%X (confirmation: 0x%02X).\n"), data, addr, written);
                    spi_done();
#endif
            }
                break;
            case CMD_READ_BLOCK: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint16_t sz = spi_swap(0xff);
                    sz |= ((uint16_t) spi_swap(0xff)) << 8;
                    ram_read_stream_start();
                    uint16_t sum1 = 0, sum2 = 0;
                    for (size_t i = 0; i < sz; ++i) {
                        uint8_t byte = ram_read_byte_stream(addr + i);
                        spi_ready();
                        spi_swap(byte);
                        sum1 = (sum1 + byte) % 255;
                        sum2 = (sum2 + sum1) % 255;
                        spi_done();
                    }
                    ram_read_stream_end();
                    spi_swap(sum1);
                    spi_swap(sum2);
                    spi_done();
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Read block from address 0x%X to 0x%X - data checksum = 0x%04X.\n"), addr, addr + sz, sum1 | (sum2 << 8));
                    spi_done();
#endif
            }
                break;
            case CMD_WRITE_BLOCK: {
                    uint16_t addr = spi_swap(0xff);
                    addr |= ((uint16_t) spi_swap(0xff)) << 8;
                    uint16_t sz = spi_swap(0xff);
                    sz |= ((uint16_t) spi_swap(0xff)) << 8;
                    ram_write_stream_start();
                    uint16_t sum1 = 0, sum2 = 0;
                    for (size_t i = 0; i < sz; ++i) {
                        uint8_t byte = spi_swap(0xff);
                        spi_done();
                        ram_write_byte_stream(addr + i, byte);
                        spi_ready();
                        sum1 = (sum1 + byte) % 255;
                        sum2 = (sum2 + sum1) % 255;
                        _delay_us(30);
                    }
                    spi_done();
                    ram_write_stream_end();
                    spi_swap(sum1);
                    spi_swap(sum2);
                    spi_done();
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Written block from address 0x%X to 0x%X - data checksum = 0x%04X.\n"), addr, addr + sz, sum1 | (sum2 << 8));
                    spi_done();
#endif
            }
                break;
            case CMD_READ_DATA: {
                    spi_ready();
                    uint8_t byte = ram_get_data();
                    _delay_us(30);
                    spi_done();
                    spi_swap(byte);
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Read data bus, value 0x%02X.\n"), byte);
                    spi_done();
#endif
                }
                break;
            case CMD_WRITE_DATA: {
                    uint8_t data = spi_swap(0xff);
                    ram_set_data(data);
                    spi_ready();
                    _delay_us(30);
                    spi_done();
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Written value 0x%02X to data bus.\n"), data);
                    spi_done();
#endif
                }
                break;
            case CMD_READ_ADDR: {
                    spi_ready();
                    uint16_t addr = ram_get_addr();
                    _delay_us(30);
                    spi_done();
                    spi_swap(addr & 0xff);
                    spi_swap((addr >> 8) & 0xff);
#ifdef DEBUG_UART
                    _delay_ms(1);
                    printf_P(PSTR("Read data bus, value 0x%04X.\n"), addr);
                    spi_done();
#endif
                }
                break;
            case CMD_READ_MBUS: {
                spi_ready();
                struct MemoryBus mbus = ram_read_memory_bus();
                _delay_us(30);
                spi_done();
                spi_swap(*(uint8_t*) &mbus);
#ifdef DEBUG_UART
                _delay_ms(1);
                printf_P(PSTR("Read memory bus:  MREQ: %d   WE: %d   RD:%d\n"), mbus.mreq, mbus.we, mbus.rd);
                spi_done();
#endif
            }
                break;
        }
        spi_deactivate();
        wdt_disable();
    }
}
