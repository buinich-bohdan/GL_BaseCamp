#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "segm.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

//------------------------------
const uint8_t digit[11] = {
    //маска для цифры
    0b11000000, //0
    0b11111001, //1
    0b10100100, //2
    0b10110000, //3
    0b10011001, //4
    0b10010010, //5
    0b10000010, //6
    0b11111000, //7
    0b10000000, //8
    0b10010000, //9
    0b11111111 //[ ]

};
long t_time;
long t_hour;
long t_min;
uint8_t digitBuffer[4];
uint8_t digitBuffer1[4];
uint8_t dotBuffer[4];
//------------------------------
void setDot(char posDot, char state) //Устанавливаем состояние определенной точки
{
    dotBuffer[posDot % 4] = state;
}
char getDot(char posDot) //Узнаем состояние определенной точки
{
    return dotBuffer[posDot % 4];
}

/** Timer2 Interrupt (on overflow), see datasheet
 * For vectors, refer to <avr/iom328p.h>
 * For more on interrupts handling with AVR-GCC see
 * https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 */
ISR(TIMER2_OVF_vect, ISR_BLOCK)
{
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20)); /* stop timer */
    /* It's often required to manually reset interrupt flag */
    /* to avoid infinite processing of it.                  */
    /* not on AVRs (unless OCB bit set)                     */
    /* 	TIFR2 &= ~TOV2;                                 */
}
void sleep_ms(uint16_t ms_val)
{
    /* Set Power-Save sleep mode */
    /* https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html */
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    cli(); /* Disable interrupts -- as memory barrier */
    sleep_enable(); /* Set SE (sleep enable bit) */
    sei(); /* Enable interrupts. We want to wake up, don't we? */
    TIMSK2 |= (1 << TOIE2); /* Enable Timer2 Overflow interrupt by mask */
    while (ms_val--) {
        /* Count 1 ms from TCNT2 to 0xFF (up direction) */
        TCNT2 = (uint8_t)(0xFF - (F_CPU / 128) / 1000);

        /* Enable Timer2 */
        TCCR2B = (1 << CS22) | (1 << CS20); /* f = Fclk_io / 128, start timer */

        sleep_cpu(); /* Put MCU to sleep */

        /* This is executed after wakeup */
    }
    sleep_disable(); /* Disable sleeps for safety */
}
static struct segm_Port PB = {
    .DDR = &DDRB,
    .PIN = &PINB,
    .PORT = &PORTB,
};
static struct segm_Display display = {
    .SHCP = {.port = &PB, .pin = 0 },
    .STCP = {.port = &PB, .pin = 1 },
    .DS = {.port = &PB, .pin = 2 },
    .delay_func = &_delay_loop_1, /* 3 cycles / loop, busy wait */
    .sleep_ms_func = &sleep_ms, /* 3 cycles / loop, busy wait */
    .is_comm_anode = true /* We have common cathode display */
};

bool poll_btn_low(struct segm_Pin* btn)
{
    if (!(*(btn->port->PIN) & (1 << btn->pin))) {
        sleep_ms(10);
        if (!(*(btn->port->PIN) & (1 << btn->pin)))
            return true;
    }
    return false;
}

struct segm_Pin btn1 = {.port = &PB, .pin = 3 },
                btn2 = {.port = &PB, .pin = 4 },
                btn3 = {.port = &PB, .pin = 5 };

ISR(TIMER1_COMPA_vect) // Timer 1 interrupt
{
    setDot(1, getDot(1) == 0); // Flashing point
    if (!poll_btn_low(&btn1)) {
        t_time += getDot(1);
        t_time %= 86400;
    } //refresh seconds
}

void printNum(long digit)
{ // Display digit
    digitBuffer[0] = digit % 10000 / 1000;
    digitBuffer[1] = digit % 1000 / 100;
    digitBuffer[2] = digit % 100 / 10;
    digitBuffer[3] = digit % 10;
}

void printClock(long digit)
{ //  Display digit
    digitBuffer1[0] = digit % 10000 / 1000;
    digitBuffer1[1] = digit % 1000 / 100;
    digitBuffer1[2] = digit % 100 / 10;
    digitBuffer1[3] = digit % 10;
}
bool VAR_alarm_FLAG = false;
bool VAR_alarm_FLAG1 = false;

void Func_alarm_action()
{
    VAR_alarm_FLAG = false;
    VAR_alarm_FLAG1 = false;
    PORTD |= 1 << 7;
    uint8_t i = 1;
    uint8_t symbols[] = { 0xFE, 0xFE, 0xFE, 0xFE };
    segm_indicate4(&display, symbols);
    while (i)
        if (poll_btn_low(&btn3)) {
            PORTD &= ~(1 << 7);
            i = 0;
        }
}

void timer_init()
{
    cli(); // disable interrupts for setting timers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 31250; // compare match register 16MHz/256/2Hz Correct to achieve an accurate frequency match.
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS12); // prescaler 256
    TIMSK1 |= (1 << OCIE1A); //  interrupt by coincidence
    sei(); // Turning on timer
}

int main(void)
{
    long t_alarm_min = 02;
    long t_alarm_hour = 10;
    long t_alarm_clock = t_alarm_hour * 3600 + t_alarm_min * 60;
    bool t_clock = true;
    DDRD |= 1 << 7;
    timer_init();
    t_time = 10 * 3600L + 1 * 60;
    segm_init(&display);

    /* Configure buttons for input with pullup R */
    *(btn1.port->DDR) &= ~(1 << btn1.pin);
    *(btn1.port->PORT) |= (1 << btn1.pin);
    *(btn2.port->DDR) &= ~(1 << btn2.pin);
    *(btn2.port->PORT) |= (1 << btn2.pin);
    *(btn3.port->DDR) &= ~(1 << btn3.pin);
    *(btn3.port->PORT) |= (1 << btn3.pin);
    while (1) {
        t_clock = true;
        if (poll_btn_low(&btn2)) { //seting alarm
            t_clock = false;
            VAR_alarm_FLAG = true;
            if (poll_btn_low(&btn1) && poll_btn_low(&btn2)) { //add hour for alarm
                t_hour = t_alarm_clock / 3600;
                t_alarm_hour++;
                t_alarm_hour %= 24;
                t_alarm_min = t_alarm_clock % 3600 / 60;
                t_alarm_clock = t_alarm_hour * 3600 + t_alarm_min * 60;
            }

            if (poll_btn_low(&btn3) && poll_btn_low(&btn2)) { //add minute for alarm
                t_alarm_hour = t_alarm_clock / 3600;
                t_alarm_min = t_alarm_clock % 3600 / 60;
                t_alarm_min++;
                t_alarm_min %= 60;
                t_alarm_clock = (t_alarm_hour)*3600 + t_alarm_min * 60;
            }
        }

        if (t_clock == false) { //show alarm
            printClock((t_alarm_clock / 3600 * 100) + (t_alarm_clock % 3600 / 60));
            uint8_t symbols[] = { digit[digitBuffer1[0]], (digit[digitBuffer1[1]] - 128), digit[digitBuffer1[2]], digit[digitBuffer1[3]] };
            segm_indicate4(&display, symbols);
        }
        else { //shor clock
            printNum((t_time / 3600 * 100) + (t_time % 3600 / 60));
            uint8_t symbols[] = { digit[digitBuffer[0]], (digit[digitBuffer[1]] - ((dotBuffer[1] > 0) ? (128) : (0))), digit[digitBuffer[2]], (digit[digitBuffer[3]] - ((dotBuffer[3] > 0) ? (128) : (0))) };
            segm_indicate4(&display, symbols);
        }
        if (VAR_alarm_FLAG) { // if alarm is on
            if (((t_time % 3600 / 60)) == (t_alarm_clock % 3600 / 60)) {
                VAR_alarm_FLAG1 = true;
            }
        } //turn on alarm
        if (VAR_alarm_FLAG1)
            Func_alarm_action();
    }
}