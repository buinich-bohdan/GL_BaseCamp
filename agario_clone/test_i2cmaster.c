/*!
 * \author Bohdan Buinich
 * \version 1.0
 * \date 2018-08-25
 *
 * \warning use only good wire for I2C interface
 *
 * \copyright GNU Public License
 * \mainpage Bublle Game
 * \section intro_sec Introduction
 * This code was developed to GLBaseCamp
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "i2cmaster.h"
#include "lcd.h"

volatile unsigned long timer1_millis; 
long milliseconds_since;

/*! \def max(a,b)
    \brief A macro that returns the maximum of \a a and \a b.
*/
#define max(a, b) ({typeof (a) _a = (a); typeof (b) _b = (b); _a > _b ? _a : _b; })

/*! \def min(a,b)
    \brief A macro that returns the minimum of \a a and \a b.
*/
#define min(a, b) ({typeof (a) _a = (a); typeof (b) _b = (b); _a < _b ? _a : _b; })

/*! \def constrain(v,lo,hi)
    \brief A macro сonstrains a number to be within a range.
    \param v The number to constrain, all data types.
    \param lo The lower end of the range, all data types.
    \param hi The upper end of the range, all data types.
*/
#define constrain(v, lo, hi) (max(min(v, hi), lo))


#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8) //!< Calculate the value needed for the CTC match value in OCR1A.

#define playAREAX 512 //!< X-axis game area
#define playAREAY 512 //!< Y-axis game area 

#define OLEDX 128  //!<  Oled size on the X-axis
#define OLEDY 64   //!<  Oled size on the Y-axis 


uint16_t XN = 0; //!<  Joystick position on the X-axis
uint16_t YN = 0; //!<  Joystick position on the Y-axis

//Initial Character Settings

uint8_t charR = 5;   //!< Size of the Character
uint8_t charX = 128; //!<  Character location X
uint8_t charY = 128; //!<  Character location Y
float charACCX = 0;  //!<  Character Momentum X
float charACCY = 0;  //!<  Character Momentum Y

//Game Settings
#define enemyCOUNT 30              //!< Enemy count in the game
uint16_t enemySTAT[enemyCOUNT][5]; //!<enemy location and stats

uint16_t JOYXPOS = 0;
uint16_t JOYYPOS = 0;


#define particleCOUNT 10 //!< Particle count in the game
uint16_t particle[particleCOUNT][3];

float friction = 0.05;

uint8_t eatPART = 0; //!< Count of the eaten elements

#define DevSSD1306 0x78 //!<  device address of SSD1306 OLED, uses 8-bit address (0x3c)!!!

uint8_t buffer[(128 * 64) / 8] = { //!< oled buffer 128 x 64
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55
};

uint8_t gameStart = 0; 

//! Start function
/*!
  \sa Start()
*/
void Start()
{
    gameStart = 1;
}

//! Interrupt on the button for start game
/*!
  \sa Start()
  \param INT0_vect
*/
ISR(INT0_vect)
{
    Start();
}

void init_TIMER()//!< Init timer for millis
{
    // CTC mode, Clock/8
    TCCR1B |= (1 << WGM12) | (1 << CS11);

    // Load the high byte, then the low byte
    // into the output compare
    OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
    OCR1AL = CTC_MATCH_OVERFLOW;

    // Enable the compare match interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Now enable global interrupts
    sei();
}

//! Interrupt on timer, add 1 for millils
/*!
  \param TIMER1_COMPA_vect
*/
ISR(TIMER1_COMPA_vect)
{
    timer1_millis++;
}

//! Return millis
/*!
  \return millis
*/
unsigned long millis()
{
    unsigned long millis_return;

    // Ensure this cannot be disrupted
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        millis_return = timer1_millis;
    }

    return millis_return;
}

void init_ADC() //!< Init ADC
{
    /* ADMUX section 23.9.1 page 262
    *   BIT 7 and BIT 6 >> AVCC with external capacitor at AREF pin REFS0 =0 and REFS1= 1
    *   BIT 5 >> ADC Right Adjust Result ADLAR = 0
    *   BIT 3:0 >> MUX3:0 0b0000 for channel 0
    */
    ADMUX = 0b01000000;
    /* DIDR0 >> Digital Input Disable Register 0 section Section 23.9.4 page 265 - 266
    *   Disable digital input buffer of ADC0 to save power consumption
    */
    DIDR0 = 0b00000001;

    /* ADSCRA ADC Control and Status Register A Section 23.9.2 page 263 -264
    *   Bit 7 >> ADEN: ADC Enable =1 enable ADC
    *   Bit 6 >> ADSC: ADC Start Conversion =0 do not start conversion
    *   Bit 5 >> ADATE: ADC Auto Trigger Enable = 0 Disable trigger
    *   Bit 4 >> ADIF: ADC Interrupt Flag = 0
    *   Bit 3 >> ADIE: ADC Interrupt Enable = 0 Disable ADC interrupt
    *   Bits 2:0 >> ADPS2:0: ADC Prescaler Select Bits 010 division factor = 4
    */
    ADCSRA = 0b10000010;
}
//! Return ADC value
/*!
  \param XN, value from A0
  \param YN, value from A1
*/
void ADC_data()
{
    ADMUX = ADMUX & ~_BV(MUX0);
    ADCSRA |= (1 << ADSC); // Enable Conversion For ADC0 PIN 0 or A0
    while (bit_is_set(ADCSRA, ADSC)) //Wait for conversion complete.
    {
        ;
    }
    XN = ADC; //Store the value in X Analog

    ADCSRA &= ~(1 << ADSC); //Disable Conversion
    ADMUX |= _BV(MUX0); // Change pin to ADC1 or A1
    ADCSRA |= (1 << ADSC); //Enable Conversion again
    while (bit_is_set(ADCSRA, ADSC)) {
        ;
    }
    YN = ADC; //Store the value in Y Analog
}

void init_interrupt() //!< Init interrup for PD2 pin
{

    DDRD &= ~(1 << DDD2); // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input
    PORTD |= (1 << PORTD2); // turn On the Pull-up
    // PD2 is now an input with pull-up enabled

    EICRA |= (1 << ISC00); // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT0); // Turns on INT0

    sei(); //Enable Interrup
}
//! Map function
/*!
    \brief Re-maps a number from one range to another. That is, a value of fromLow would get mapped to toLow, a value of fromHigh to toHigh, values in-between to values in-between, etc.
    \param x The number to map.
    \param in_min The lower bound of the value’s current range.
    \param in_max The upper bound of the value’s current range.
    \param out_min The lower bound of the value’s target range.
    \param out_max The upper bound of the value’s target range.
    \return Re-maps value
*/
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//! makeRandom function
/*!  \brief Functiom get random integers in a certain range
    \param upper value in which range must be new random value
    \return Random integers in a certain range
*/
uint16_t makeRandom(uint16_t upper)
{
    return (uint16_t)((double)rand() / ((double)RAND_MAX + 1) * upper);
}

/*!
    \brief Functiom check contact with enemy sprite
    \param spriteNUM number enemy sprite
    \param spriteX position on the X-axis
    \param spriteY position on the Y-axis
    \param spriteR radius 
    \retval  0 if no contact
    \retval  1 if contact
*/
uint16_t checkCONTACT(uint8_t spriteNUM, uint8_t spriteX, uint8_t spriteY, uint8_t spriteR)
{

    uint8_t contact = 0;
    uint8_t xDIST = abs(spriteX - charX);
    uint8_t yDIST = abs(spriteY - charY);
    double centerDIST = sqrt((xDIST * xDIST) + (yDIST * yDIST));
    //if distance between to points less then two radius combained  
    uint8_t combineR = charR + spriteR;
    if (combineR > centerDIST) {
        contact = 1;
    }
    return contact;
}
/*! 
    \brief Functiom check contact with particle sprite
    \param spriteNUM number enemy sprite
    \param spriteX position on the X-axis
    \param spriteY position on the Y-axis
    \retval  0 if no contact
    \retval  1 if contact
*/
int checkPCONTACT(uint8_t spriteNUM, uint8_t spriteX, uint8_t spriteY)
{
    uint8_t contact = 0;
    uint8_t xDIST = abs(spriteX - charX);
    uint8_t yDIST = abs(spriteY - charY);
    double centerDIST = sqrt((xDIST * xDIST) + (yDIST * yDIST));
        //if distance between to points less then our radius  
    if (charR > centerDIST) {
        contact = 1;
    }
    return contact;
}
/*! 
    \brief Functiom get random integers in a range \a min to \a max
    \param max value in which range must be new random value
    \param min value in which range must be new random value
    \return Random integers in a range
*/
int randint(uint16_t min, uint16_t max)
{
    if (min > max) {
        return (max + (int)((min - max + 1) * rand() / (RAND_MAX + 1.0)));
    }
    else {
        return (min + (int)((max - min + 1) * rand() / (RAND_MAX + 1.0)));
    }
}
/*! 
    \brief Functiom dead Animation when player lose game
*/
void deadANIMATION()
{
    uint8_t sizeR = charR;
    while (sizeR < 33) {
        i2c_start(DevSSD1306 + I2C_WRITE);
        drawCircle(64, 32, sizeR);
        sizeR++;
        drawBuffer(0, 0, buffer);
        clearBuffer(buffer);
    }
    lcd_draw_string(50, 32, "Game", buffer);
    lcd_draw_string(50, 45, "Over.", buffer);
    drawBuffer(0, 0, buffer);
    while (1)
        ;
}
/*! 
    \brief Functiom ememy dead
    \param enemyNUM numer dead enemy
*/
void enemyDEAD(uint8_t enemyNUM)
{
    enemySTAT[enemyNUM][3] = 0;
    enemySTAT[enemyNUM][4] = millis();
}
/*! 
    \brief Functiom win game
*/
void youWIN()
{
    clearBuffer(buffer);
    lcd_draw_string(55, 15, "You've", buffer);
    lcd_draw_string(61, 30, "Eaten", buffer);
    lcd_draw_string(46, 45, "Everything!", buffer);
    drawBuffer(0, 0, buffer);
    while (1);
}

/*! 
    \brief Game algoritm
*/
void gameMode()
{
    ADMUX = ADMUX & ~_BV(MUX0);
    ADCSRA |= (1 << ADSC); // Enable Conversion For ADC0 PIN 0 or A0
    while (bit_is_set(ADCSRA, ADSC)) //Wait for conversion complete.
    {
        ;
    }
    uint16_t moveX = ADC; //Store the value in X Analog

    ADCSRA &= ~(1 << ADSC); //Disable Conversion
    ADMUX |= _BV(MUX0); //Change pin to ADC1 or A1
    ADCSRA |= (1 << ADSC); //Enable Conversion again
    while (bit_is_set(ADCSRA, ADSC)) 
    {
        ;
    }
    uint16_t moveY = ADC; //Store the value in Y Analog

    moveX = map(moveX, 0, 1023, 500, -500);
    moveY = map(moveY, 0, 1023, -500, 500);
    float mX = (float)(moveX) / (float)(1000);
    float mY = (float)(moveY) / (float)(1000);

    charACCX = charACCX + mX;
    charACCY = charACCY + mY;

    charACCY = constrain(charACCY, -1, 1);
    charACCX = constrain(charACCX, -1, 1);

    charX = charX + charACCX;
    charY = charY + charACCY;
    charY = constrain(charY, 0, playAREAY);
    charX = constrain(charX, 0, playAREAX);

    int displayBOXX = charX + 64;
    int displayBOXY = charY + 32;

    if (charR > 40)
        youWIN();
    
    drawCircle(64, 32, charR);

    for (int j = 0; j < enemyCOUNT; j++) {
        if (enemySTAT[j][3] == 1) {
            if (enemySTAT[j][0] < displayBOXX + 20 && enemySTAT[j][0] > (displayBOXX - OLEDX - 20)) {
                if (enemySTAT[j][1] < displayBOXY + 20 && enemySTAT[j][1] > (displayBOXY - OLEDY - 20)) {
                    uint8_t eX = ((charX + (OLEDX / 2)) - enemySTAT[j][0]);
                    uint8_t eY = ((charY + (OLEDY / 2)) - enemySTAT[j][1]);
                    drawCircle(eX, eY, enemySTAT[j][2]);
                    int touch = checkCONTACT(j, enemySTAT[j][0], enemySTAT[j][1], enemySTAT[j][2]);
                    if (touch == 1) {
                        if (enemySTAT[j][2] > charR) {
                            deadANIMATION();
                        }
                        if (enemySTAT[j][2] < charR) {
                            charR++;
                            enemyDEAD(j);
                        }
                    }
                }
            }
        }
        if (enemySTAT[j][3] == 0) {
            int checkTIME = millis();
            if ((checkTIME - enemySTAT[j][4]) > 5000) {
                enemySTAT[j][0] = makeRandom(playAREAX);
                enemySTAT[j][1] = makeRandom(playAREAY);
                enemySTAT[j][2] = makeRandom(10);
                enemySTAT[j][3] = 1;
                enemySTAT[j][4] = 0;
            }
        }
    }

    for (int i = 0; i < particleCOUNT; i++) {
        if (particle[i][2] == 1) { //ALIVE
            if (particle[i][0] < displayBOXX && particle[i][0] > (displayBOXX - OLEDX)) { //В ДИАПАЗОНЕ
                if (particle[i][1] < displayBOXY && particle[i][1] > (displayBOXY - OLEDY)) {
                    uint8_t pX = ((charX + (OLEDX / 2)) - particle[i][0]);
                    uint8_t pY = ((charY + (OLEDY / 2)) - particle[i][1]);

                    drawPixel(pX, pY);

                    int eat = checkPCONTACT(i, particle[i][0], particle[i][1]);
                    if (eat == 1) {
                        eatPART++;
                        particle[i][2] = 0;
                    }

                    int selection = makeRandom(100);
                    if ((selection % 5) == 0) {
                        particle[i][0] += randint(-1, 2);
                        particle[i][1] += randint(-1, 2);
                    }
                }
            }
        }
    }
    if (eatPART >= 10) {
        charR++;
        eatPART = 0;
    }
}

int main(void)
{

    i2c_init(); // init I2C interfac
    uint8_t ret = i2c_start(DevSSD1306 + I2C_WRITE); // set device address and write mode
    if (ret) {
        /* failed to issue start condition, possibly no device found */
        i2c_stop();
    }
    else {

        init_ADC();
        init_TIMER(); //for millis
        init_interrupt(); //init interrupt for button to start
        ADC_data(); //calibrate joystick
        //RANDOMLY PLACE PARTICLES X AND Y
        for (uint16_t i = 0; i < particleCOUNT; i++) {
            particle[i][0] = makeRandom(playAREAX);
            particle[i][1] = makeRandom(playAREAY);
            particle[i][2] = 1; //ALIVE
        }
        for (uint16_t k = 0; k < enemyCOUNT; k++) {
            enemySTAT[k][2] = makeRandom(10); //RADIUS
            enemySTAT[k][0] = makeRandom(playAREAX - enemySTAT[k][2]); //X POSITION
            enemySTAT[k][1] = makeRandom(playAREAY - enemySTAT[k][2]); //Y POSITION
            enemySTAT[k][3] = 1; //ALIVE STATUS 1:ALIVE 0:DEAD
            enemySTAT[k][4] = 0; // TIME SINCE DEAD
        }
        setup_i2c();
        clearBuffer(buffer);
        while (gameStart == 0) {
            lcd_draw_string(0, 0, "Agario Clone!", buffer);
            lcd_draw_string(10, 4, "Press A to start", buffer);
            drawBuffer(0, 0, buffer);
        }
        //i2c_stop();
    }
    while (1) {
        gameMode();
        drawBuffer(0, 0, buffer);
        clearBuffer(buffer);
    }
}