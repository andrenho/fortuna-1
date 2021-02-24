PROJECT=controller
OBJECTS=main.o repl.o serial.o pb_common.o pb_encode.o pb_decode.o messages.pb.o
AVRDUDE_FLAGS=-p m328p -C ./avrdude_gpio.conf -c pi_1 -B 0.5 -V
FUSES=-U lfuse:w:0xDE:m	-U hfuse:w:0xD9:m -U efuse:w:0xFF:m -U lock:w:0xFF:m
MCU=atmega328p
F_CPU=16000000UL   # running at 16 Mhz

CC=avr-gcc
CXX=avr-g++
CFLAGS=-std=c11
CXXFLAGS=-std=c++17
CPPFLAGS=-Wall -Wextra -Os -DF_CPU=${F_CPU} -mmcu=${MCU} -ffunction-sections -fdata-sections -I.

all: ${PROJECT}.hex

${PROJECT}.hex: ${PROJECT}.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	arm-linux-gnueabihf-size ${PROJECT}.hex

${PROJECT}.elf: ${OBJECTS}
	$(CC) -mmcu=${MCU} -o $@ $^ -Wl,--gc-sections
	avr-size -C --mcu=${MCU} ${PROJECT}.elf	

repl.o: messages.pb.h

messsages.pb.o: messages.pb.c

messages.pb.h messages.pb.c: messages.proto
	python3 ../../contrib/nanopb/generator/nanopb_generator.py $^

test-connection:
	sudo avrdude ${AVRDUDE_FLAGS}

clean:
	rm -f *.elf *.o *.hex test-disasm test-simulator

upload: ${PROJECT}.hex
	sudo avrdude ${AVRDUDE_FLAGS} -U flash:w:$<:i

fuse:
	sudo avrdude ${AVRDUDE_FLAGS} ${FUSES}

# vim: set ts=8 sts=8 sw=8 noexpandtab:
