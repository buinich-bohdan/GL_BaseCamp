
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#define NOID 0xCC //Skip identification
char sernum[8]; //device serial number

#define PORTTEMP PORTB
#define DDRTEMP DDRB
#define PINTEMP PINB
#define BITTEMP 0

void USART_init(void)
{

    UBRR0H = (uint8_t)(BAUD_PRESCALLER >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

uint8_t USART_receive(void)
{

    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

void USART_send(uint8_t data)
{

    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

void USART_putstring(char* StringPtr)
{

    while (*StringPtr != 0x00) {
        USART_send(*StringPtr);
        StringPtr++;
    }
}

//EEPROM
void EEPROM_write(uint16_t uiAddress, uint8_t ucData)
{
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE))
        ;
    /* Set up address and Data Registers */
    EEAR = uiAddress;
    EEDR = ucData;
    /* Write logical one to EEMPE +запись*/
    EECR |= (1 << EEMPE);
    /* Start eeprom write by setting EEPE записуем*/
    EECR |= (1 << EEPE);
}

uint8_t EEPROM_read(uint16_t uiAddress)
{
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE))
        ;
    /* Set up address register */
    EEAR = uiAddress;
    /* Start eeprom read by writing EERE */
    EECR |= (1 << EERE);
    /* Return data from Data Register */
    return EEDR;
}

const char* EEPROM_read_string(uint16_t uiAddress, uint16_t sz)

{
    unsigned int i;
    char* str1;
    str1 = (char*)realloc(NULL, sz);
    for (i = 0; i < sz; i++)
        str1[i] = EEPROM_read(uiAddress + i);
    return str1;
}

//DS18B20

//sensor detection function on the bus
uint8_t dt_testdevice(void)
{
    char stektemp = SREG; // save stack
    cli(); //Disable interrupts
    char dt;
    DDRTEMP |= 1 << BITTEMP; //pull the bus
    _delay_us(485); //delay of at least 480 microseconds
    DDRTEMP &= ~(1 << BITTEMP); //release the bus
    _delay_us(65); //delay a minimum 60 microseconds
    if ((PINTEMP & (1 << BITTEMP)) == 0) //check if the device responds
    {
        dt = 1; //device online
    }
    else
        dt = 0; //device disable
    SREG = stektemp; // return the value of the stack
    _delay_us(420); 
    return dt;
}

//function of writing a bit to the device
void dt_sendbit(char bt)
{
    char stektemp = SREG; // save stack
    cli(); //Disable interrupts
    DDRTEMP |= 1 << BITTEMP; //pull the bus
    _delay_us(2);
    if (bt)
        DDRTEMP &= ~(1 << BITTEMP); //release the bus
    _delay_us(65);
    DDRTEMP &= ~(1 << BITTEMP); //release the bus
    SREG = stektemp; // return the value of the stack
}
// function of writing a byte to the device
void dt_sendbyte(unsigned char bt)
{
    for (uint8_t i = 0; i < 8; i++) //send each bit to the device separately
    {
        if ((bt & (1 << i)) == 1 << i) //send 1
            dt_sendbit(1);
        else //send 0
            dt_sendbit(0);
    }
}

// function of reading a bit from the device
char dt_readbit(void)
{
    char stektemp = SREG; // save stack
    cli(); //Disable interrupts
    char bt;
    DDRTEMP |= 1 << BITTEMP; //pull the bus
    _delay_us(2); 
    DDRTEMP &= ~(1 << BITTEMP); //pull the bus
    _delay_us(13);
    bt = (PINTEMP & (1 << BITTEMP)) >> BITTEMP; //read bit
    _delay_us(45);
    SREG = stektemp; // return the value of the stack
    return bt; //return result
}

// function of reading bytes from the device
unsigned char dt_readbyte(void)
{
    char c = 0;
    char i;
    for (i = 0; i < 8; i++)
        c |= dt_readbit() << i; //read bit
    return c;
}

uint8_t dt_check()
{
    if (dt_testdevice() == 1)// if the device was found
    {
        dt_sendbyte(0x33); //skip identification, we only have one device on the bus
        _delay_ms(750); //in the 12-bit conversion mode - 750 milliseconds

        for (int i = 0; i <= 7; i++)
            sernum[i] = dt_readbyte();
        return 1;
    }
    else
        return 0;
}

int main(void)
{
    //EEPROM_write(0, 0);
    USART_init(); //Call the USART initialization code

    //nubmer device in EEPROM
    uint16_t number = EEPROM_read(0);

    //save new serial in EEPROM
    uint8_t button = 0;
    if (button == 0) {
        if (dt_check() != 0) {
            EEPROM_write(0, ++number);
            uint32_t add = number * 8 + 1;
            for (int i = 0; i <= 7; i++)
                EEPROM_write(add + i, sernum[i]);
        }
        else
            USART_putstring("Read error");
    }
    number = EEPROM_read(0);
    //check device
   // dt_check();
    
    uint16_t next = 0;

    for (int i = 0; i < number; i++) {
        char* buff = EEPROM_read_string(1 + next, 8 + next);
        //for (int i = 0; i <= 7; i++)
        //USART_send(buff[i]);
        next += 8;
        int result = strncmp(buff, sernum, 8);	

        if (result == 0) {
            USART_putstring("Door open\n");
        }
        else
            USART_putstring("Door closed\n");
    }
}
