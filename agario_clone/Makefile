# Controller installed on the board, for example atmega328 
DEVICE     = atmega328

# The clock frequency
CLOCK      = 16000000

# The command to run avrdude. It needs to be copied from the Arduino IDE. 
AVRDUDE = /opt/arduino-1.8.5/hardware/tools/avr/bin/avrdude -C/opt/arduino-1.8.5/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/ttyACM0 -b115200 -D
OBJECTS    = test_i2cmaster.o twimaster.o lcd.o

COMPILE = avr-gcc -Wall -std=gnu99 -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

REMOVE = rm -f

# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o) 

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)

all:	test_i2cmaster.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:test_i2cmaster.hex:i

clean:
	rm -f test_i2cmaster.hex test_i2cmaster.elf $(OBJECTS)
	$(REMOVE) *.o


test_i2cmaster.elf: $(OBJECTS)
	$(COMPILE) -o test_i2cmaster.elf $(OBJECTS)

test_i2cmaster.hex: test_i2cmaster.elf
	rm -f test_i2cmaster.hex
	avr-objcopy -j .text -j .data -O ihex test_i2cmaster.elf test_i2cmaster.hex
	avr-size --format=avr --mcu=$(DEVICE) test_i2cmaster.elf
