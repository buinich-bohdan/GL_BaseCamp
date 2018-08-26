# Controller installed on the board, for example atmega328 
DEVICE     = atmega328

  # The clock frequency
CLOCK      = 16000000

DEPS:=segm


# The command to run avrdude. It needs to be copied from the Arduino IDE. 
AVRDUDE = /opt/arduino-1.8.5/hardware/tools/avr/bin/avrdude -C/opt/arduino-1.8.5/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/ttyACM0 -b115200 -D
OBJECTS    = main.o

COMPILE = avr-gcc -Wall -std=c11 -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	clean | main.hex 

.c.o: 
	$(COMPILE) -c $< -o $@
	
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf $(OBJECTS) $(DEPS) $(addsuffix .o, $(DEPS))

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS) $(addsuffix .o, $(DEPS))

segm.elf:
	$(COMPILE) -c segm.c -o segm.o

main.hex: segm.elf main.elf 
	rm -f main.hex $(DEPS) $(addsuffix .o, $(DEPS))
	$(COMPILE) -c segm.c -o segm
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
