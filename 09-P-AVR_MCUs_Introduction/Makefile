#Контроллер, установленный на плате. Может быть другим, например atmega328 
DEVICE     = atmega328

#Тактовая частота 16 МГц 
CLOCK      = 16000000

#Команда запуска avrdude. Ее нужно скопировать из Arduino IDE.
AVRDUDE = /opt/arduino-1.8.5/hardware/tools/avr/bin/avrdude -C/opt/arduino-1.8.5/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/ttyACM0 -b115200 -D
OBJECTS    = main.o

COMPILE = avr-gcc -Wall -std=c11 -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf $(OBJECTS)

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf