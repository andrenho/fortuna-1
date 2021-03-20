#include <avr/io.h>
#include <avr/pgmspace.h>

int main()
{
    // set config
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);     // Enable Receiver and Transmitter
    UCSR0C = (1<<UMSEL01) | (1<<UCSZ01) | (1<<UCSZ00);   // Async-mode
    
    // set baud rate - http://ruemohr.org/~ircjunk/avr/baudcalc/avrbaudcalc-1.0.8.php?postbitrate=38400&postclock=8
    int ubrr = 25;       // 38400 at 16 Mhz
    UBRR0H = (ubrr>>8);
    UBRR0L = (ubrr);

    // send dot
    while (!( UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
    UDR0 = '.';

    while (1) {
        // recv
        while (!( UCSR0A & (1<<RXC0)));  // wait for empty receive buffer
        char c = UDR0;

        // send
        while (!( UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
        UDR0 = c;
    }
}
