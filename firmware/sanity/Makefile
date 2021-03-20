PROJECT=sanity
OBJECTS=main.o
AVRDUDE_FLAGS=-p m328p -C ./avrdude_gpio.conf -c pi_1 -V
FUSES=-U lfuse:w:0xDE:m	-U hfuse:w:0xD9:m -U efuse:w:0xFF:m -U lock:w:0xFF:m
MCU=atmega328p
F_CPU=16000000UL   # running at 16 Mhz

CC=avr-gcc
CFLAGS=-std=c11
CPPFLAGS=-Wall -Wextra -Os -DF_CPU=${F_CPU} -mmcu=${MCU} -ffunction-sections -fdata-sections -I. -DEMBEDDED
LDFLAGS=-Wl,--gc-sections 

all: ${PROJECT}.hex

${PROJECT}.hex: ${PROJECT}.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	arm-linux-gnueabihf-size ${PROJECT}.hex

${PROJECT}.elf: ${OBJECTS}
	$(CC) -mmcu=${MCU} -o $@ $^ ${LDFLAGS}
	avr-size -C --mcu=${MCU} ${PROJECT}.elf	

test-connection:
	sudo avrdude ${AVRDUDE_FLAGS}

clean:
	rm -f *.elf *.o **/*.o *.hex test-disasm test-simulator *.pb.* libf1comm/*.o libf1comm/**/*.o

upload: ${PROJECT}.hex
	sudo avrdude ${AVRDUDE_FLAGS} -U flash:w:$<:i

fuse:
	sudo avrdude ${AVRDUDE_FLAGS} ${FUSES}

# vim: set ts=8 sts=8 sw=8 noexpandtab:
