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
            printf_P(PSTR("Ctrl:     [f] bytes free   [R] reset       [t] soft reset            [!] system reset\n"));
            printf_P(PSTR("RAM:      [r] read byte    [w] write byte  [W] write multiple bytes  [d] dump memory\n"));
            printf_P(PSTR("SdCard:   [l] last status  [s] dump block\n"));
            printf_P(PSTR("Z80:      [i] CPU info     [p] step        [@] change debug mode\n"));
            printf_P(PSTR("Terminal: [k] keypress\n"));
            printf_P(PSTR("Low lvl:  [b] buses state  [c] Z80 cycle\n"));
#ifdef ENABLE_TESTS
            printf_P(tests_help());
#endif
            break;
        case 'f':
            printf_P(PSTR("%d bytes free.\n"), free_ram());
            break;
        case 't':
            print_reset_status(fortuna1_.soft_reset());
            break;
        case 'R':
            print_reset_status(fortuna1_.hard_reset(buffer_));
            break;
        case 'r': {
                uint32_t addr;
                if (ask_question_P(PSTR("Address"), 2, &addr))
                    printf_P(PSTR("[0x%04X] = 0x%02X\n"), addr, fortuna1_.read_byte(addr));
            }
            break;
        case 'w': {
                Question q[] = { { PSTR("Address"), 4 }, { PSTR("Data"), 2 } };
                if (ask_question_P(q, 2))
                    printf_P(PSTR("0x%02X\n"), fortuna1_.write_byte(q[0].response, q[1].response));
            }
            break;
        case 'W': {
                uint32_t addr = 0;
                if (!ask_question_P(PSTR("Address"), 2, &addr))
                    break;
                while (true) {
                    uint32_t data;
                    printf_P(PSTR("[0x%04X]"), addr);
                    if (!ask_question_P(PSTR(""), 2, &data))
                        break;
                        fortuna1_.write_byte(addr++, data);
                }
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
        case 'p': {
                fortuna1_.z80().step();
                printf_P(PSTR("PC = %04X\n"), fortuna1_.z80().pc());
                uint8_t lpc = fortuna1_.terminal().last_printed_char();
                if (lpc != 0)
                    printf_P(PSTR("Printed char: %02X %c\n"), lpc, lpc > 32 && lpc < 127 ? lpc : ' ');
            }
            break;
        case 'b':
            printf_P(PSTR("\e[1A"));
            fortuna1_.z80().print_pin_state();
        case 'c':
            printf_P(PSTR("\e[1A"));
            fortuna1_.z80().cycle();
            return;
        case '!':
            fortuna1_.system_reset();
            break;
        case 'k': {
                uint32_t key;
                if (ask_question_P(PSTR("Key (in hex)"), 2, &key))
                    fortuna1_.terminal().keypress(key);
            }
            break;
        case '@':
            fortuna1_.z80().set_debug_mode(!fortuna1_.z80().debug_mode());
            printf_P(PSTR("Debug mode is %s.\n"), fortuna1_.z80().debug_mode() ? "on" : "off");
            break;
        default:
            error();
    }
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
            if (c == '\r') {
                putchar('\n');
                break;
            } else if (c == '\e') {
                putchar('\n');
                goto error;
            } else if (((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) && p < question->size) {
                putchar(c);
                buffer[p++] = c;
            } else if ((c == '\b' || c == 127) && p > 0) {
                printf_P(PSTR("\b \b"));
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

void Console::print_reset_status(ResetStatus r) const
{
    switch (r) {
        case ResetStatus::Ok:
            printf_P(PSTR("Ok.\n"));
            break;
        case ResetStatus::SDCardInitError:
            printf_P(PSTR("SDCard initialization error.\n"));
            break;
        case ResetStatus::SDCardReadError:
            printf_P(PSTR("SDCard read error.\n"));
            break;
        case ResetStatus::SDCardNotBootable:
            printf_P(PSTR("SDCard not bootable.\n"));
            break;
    }
}
