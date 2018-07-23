/* Header is in /usr/avr/include. For more, see http://bit.ly/avrgcc-headers */
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define LED_PIN 4 // pin that will act like a pwm pin

uint16_t frequency = 1000; //the frequency of the cycle
uint32_t next = 1	; //next number
uint16_t RandMaxNew = 65535; // max random number
uint8_t time_on; // how long the output should be high
uint8_t time_off; //how long the output should be low
float cycle_length; //how long each pwm cycle should be on for
float duty_cycle; //the percentage of power we want
float v_out; //the amount of voltage the led should draw

// linear_congruential
uint16_t rand_new()
{
    next = (1103515245 * next + 12345) % (RandMaxNew);
    return next;
}

uint8_t getpwm()
{
    // generate random number from 0 to 65535
    uint16_t r = rand_new();
    // imitate probability density
    if (r > 50461) {
        return 15; // maximum pwm level
    }
    else if (r > 39976) {
        return 14;
    }
    else if (r > 34078) {
        return 13;
    }
    else if (r > 30801) {
        return 12;
    }
    else if (r > 27524) {
        return 11;
    }
    else if (r > 24247) {
        return 10;
    }
    else if (r > 20971) {
        return 9;
    }
    else if (r > 17694) {
        return 8;
    }
    else if (r > 14417) {
        return 7;
    }
    else if (r > 11140) {
        return 6;
    }
    else if (r > 7863) {
        return 5;
    }
    else if (r > 4587) {
        return 4;
    }
    else if (r > 1965) {
        return 3;
    }
    else if (r > 1310) {
        return 2;
    }
    else if (r > 654) {
        return 1;
    }
    else {
        return 0; // logical 0
    }
}

int main()
{

    DDRB |= 1 << LED_PIN;
    cycle_length = 1000000 / frequency; //length of one pwm cycle in microseconds

    while (1) {

        uint16_t val = getpwm(); //get the pwm value

        v_out = (uint16_t)val * 255L / 15L; //gets the pot value from 0-1024 and turns it into a value from 0-255

        duty_cycle = v_out / 255; // percentage of power being drawn as decimal.

        time_on = duty_cycle * cycle_length; // work out the time it should be on

        time_off = cycle_length - time_on; // work out the time it should be off

        if (time_on > 0)

        {
            PORTB |= 1 << LED_PIN;
            for (int i = 0; i < time_on; i++)
                _delay_us(1);
        }
        PORTB &= ~(1 << LED_PIN);
        for (int i = 0; i < time_off; i++)
            _delay_us(1);
    }
}
