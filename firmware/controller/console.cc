#include <limits.h>
#include <stdio.h>

#include <avr/pgmspace.h>

#include "console.hh"
#include "tests.hh"
#include "util.hh"

void Console::execute(char cmd)
{
    const int ERROR = INT_MIN;
    
    auto error = [](){ printf_P(PSTR("Syntax error.\n")); };
    
    auto ask_value_P = [&](const char* question) -> int {
        int value = 0;
        printf_P(question);
        putchar('?');
        putchar(' ');
        serial_.set_echo(true);
        int n = scanf("%i", &value);
        serial_.set_echo(true);
        if (n != 1) {
            error();
            return ERROR;
        } else {
            putchar('\n');
            return value;
        }
    };
    
    struct Values { int v1, v2; };
    auto ask_two_values_P = [&](const char* question) -> Values {
        Values values = { 0, 0 };
        printf_P(question);
        putchar('?');
        putchar(' ');
        serial_.set_echo(true);
        int n = scanf("%i %i", &values.v1, &values.v2);
        serial_.set_echo(true);
        if (n != 2) {
            error();
            return { ERROR, 0 };
        } else {
            putchar('\n');
            return values;
        }
    };
    
    putchar(cmd);
    putchar('\n');
#ifdef ENABLE_TESTS
    if (do_tests(cmd, fortuna1_, buffer_))
        return;
#endif
    switch (cmd) {
        case 'h':
        case '?':
            printf_P(PSTR("Ctrl:    [f] bytes free   [R] reset       [t] soft reset\n"));
            printf_P(PSTR("RAM:     [r] read byte    [w] write byte  [W] write multiple bytes  [d] dump memory\n"));
            printf_P(PSTR("SdCard:  [l] last status  [s] dump block\n"));
            printf_P(PSTR("Z80:     [i] CPU info     [p] step        [P] step + print cycles\n"));
            printf_P(PSTR("Low lvl: [b] buses state  [c] Z80 cycle\n"));
#ifdef ENABLE_TESTS
            printf_P(tests_help());
#endif
            break;
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        case 't':
            fortuna1_.soft_reset();
            printf_P(PSTR("System soft reset.\n"));
            break;
        case 'R':
            fortuna1_.hard_reset(buffer_);
            printf_P(PSTR("System reset.\n"));
            break;
        case 'r': {
            int addr = ask_value_P(PSTR("Addr"));
            if (addr != ERROR)
                printf_P(PSTR("0x%02X\n"), fortuna1_.read_byte(addr));
        }
            break;
        case 'w': {
            Values vv = ask_two_values_P(PSTR("Addr Data"));
            if (vv.v1 != ERROR) {
                printf_P(PSTR("0x%02X\n"), fortuna1_.write_byte(vv.v1, vv.v2));
            }
        }
            break;
        case 'd': {
            int page = ask_value_P(PSTR("Page (0x100)"));
            if (page != ERROR) {
                if (!fortuna1_.read_block(page * 0x100, 0x100, [](uint16_t addr, uint8_t byte, void* page) {
                    if (addr % 0x10 == 0)
                        printf_P(PSTR("%04X : "), addr + (*(int*)page * 0x100));
                    printf_P(PSTR("%02X "), byte);
                    if (addr % 0x10 == 0xf)
                        putchar('\n');
                }, &page))
                    printf_P(PSTR("Checksum error\n"));
            }
        }
            break;
        case 'l':
            printf_P(PSTR("Last stage: 0x%02X   last response: 0x%02X\n"), fortuna1_.sdcard().last_stage(), fortuna1_.sdcard().last_response());
            break;
        case 's': {
            int block = ask_value_P(PSTR("Block"));
            if (block == ERROR)
                return;
            if (!fortuna1_.sdcard().read_page(block, buffer_)) {
                printf_P(PSTR("Error reading SDCard.\n"));
                return;
            }
            for (size_t i = 0; i < 512; ++i) {
                if (i % 0x10 == 0)
                    printf_P(PSTR("%08X : "), block * 512 + i);
                printf_P(PSTR("%02X "), buffer_.data[i]);
                if (i % 0x10 == 0xf)
                    putchar('\n');
            }
        }
            break;
        case 'i':
            printf_P(PSTR("Powered: %s   Cycle: %d   PC: 0x%04X\n"),
                     fortuna1_.z80().powered() ? "yes" : "no",
                     fortuna1_.z80().cycle_count(), fortuna1_.z80().pc());
            break;
        case 'p':
            fortuna1_.z80().step();
            printf_P(PSTR("PC = %04X\n"), fortuna1_.z80().pc());
            break;
        case 'P': {
                struct Ptr {
                    Console const& console;
                    RAM const&     ram;
                    Z80 const&     z80;
                };
                auto f_each_cycle = [](bool first, void* data) -> void {
                    auto ptr = (Ptr*) data;
                    ptr->console.print_z80_state(ptr->ram, ptr->z80, first);
                };
                Ptr ptr = { *this, fortuna1_.ram(), fortuna1_.z80() };
                fortuna1_.z80().step(f_each_cycle, &ptr);
                printf_P(PSTR("PC = %04X\n"), fortuna1_.z80().pc());
            }
            break;
        case 'b':
            print_z80_state(fortuna1_.ram(), fortuna1_.z80(), true);
            break;
        case 'c':
            fortuna1_.z80().cycle();
            print_z80_state(fortuna1_.ram(), fortuna1_.z80(), true);
            break;
        default:
            error();
    }
}

void Console::print_z80_state(RAM const& ram, Z80 const& z80, bool add_header) const
{
    uint8_t data = ram.data_bus();
    uint16_t addr = ram.addr_bus();
    RAM::MemoryBus mbus = ram.memory_bus();
    Z80Pins pins = z80.state();
    char addr_s[5] = { 0 };
    char data_s[3] = { 0 };
    if (mbus.mreq == 0 && (mbus.we == 0 || mbus.rd == 0 || pins.iorq == 0)) {
        sprintf(addr_s, "%04X", addr);
        sprintf(data_s, "%02X", data);
    } else {
        sprintf(addr_s, "----");
        sprintf(data_s, "--");
    }
    auto bit = [](bool v) { if (v) return "\e[0;32m1\e[0m"; else return "\e[0;31m0\e[0m"; };
    if (add_header)
        printf_P(PSTR("ADDR DATA  MREQ WR RD  INT NMI RST BUSRQ  HALT IORQ M1 BUSAK  #CYCLE\n"));
    printf_P(PSTR("%s  %s     %s   %s  %s   %s   %s   %s    %s      %s    %s   %s    %s  %-08d\n"),
             addr_s, data_s, bit(mbus.mreq), bit(mbus.we), bit(mbus.rd),
             bit(pins.int_), bit(pins.nmi), bit(pins.rst), bit(pins.busrq),
             bit(pins.halt), bit(pins.iorq), bit(pins.m1), bit(pins.busak), z80.cycle_count());
}

