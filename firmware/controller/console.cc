#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/pgmspace.h>

#include "console.hh"
#include "tests.hh"
#include "util.hh"

void Console::execute(char cmd)
{
    auto error = [](){ printf_P(PSTR("Syntax error.\n")); };
    
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
            uint32_t addr;
            if (ask_question_P(PSTR("Address"), 2, &addr))
                printf_P(PSTR("0x%02X\n"), fortuna1_.read_byte(addr));
        }
            break;
        case 'w': {
            Question q[] = { { PSTR("Address"), 4 }, { PSTR("Data"), 2 } };
            if (ask_question_P(q, 2))
                printf_P(PSTR("0x%02X\n"), fortuna1_.write_byte(q[0].response, q[1].response));
        }
            break;
        case 'd': {
            uint32_t page;
            if (ask_question_P(PSTR("Page (0x100)"), 2, &page)) {
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
            uint32_t block;
            if (!ask_question_P(PSTR("Block"), 8, &block))
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
            printf_P(PSTR("\e[1A"));
            print_z80_state(fortuna1_.ram(), fortuna1_.z80(), !last_was_cycle_);
            last_was_cycle_ = true;
        case 'c':
            printf_P(PSTR("\e[1A"));
            fortuna1_.z80().cycle();
            print_z80_state(fortuna1_.ram(), fortuna1_.z80(), !last_was_cycle_);
            last_was_cycle_ = true;
            return;
        default:
            error();
    }
    last_was_cycle_ = false;
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

bool Console::ask_question_P(char const* question, uint8_t size, uint32_t* response)
{
    Question q[] = { { question, size } };
    bool v = ask_question_P(q, 1);
    *response = q[0].response;
    return v;
}

bool Console::ask_question_P(Question* question, size_t n_questions)
{
    for (size_t i = 0; i < n_questions; ++i) {
        printf_P(question[i].question);
        printf_P(PSTR("? 0x"));
        
        size_t p = 0;
        char buffer[9] = { 0 };
        while (true) {
            char c = getchar();
            if (c >= 'a' && c <= 'f')
                c += 'A' - 'a';  // convert to uppercase
            if (c == '\n') {
                putchar('\n');
                break;
            } else if (c == '\e') {
                putchar('\n');
                goto error;
            } else if (((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) && p <= question->size) {
                putchar(c);
                buffer[p++] = c;
            } else if (c == '\b' && p > 0) {
                putchar('\b');
                buffer[p--] = '\0';
            }
        }
        
        // convert value
        uint32_t value = strtoul(buffer, NULL, 16);
        if (value == ULONG_MAX)
            goto error;
        else
            question[i].response = value;
    }
    return true;
    
error:
    printf_P(PSTR("Error.\n"));
    return false;
}

