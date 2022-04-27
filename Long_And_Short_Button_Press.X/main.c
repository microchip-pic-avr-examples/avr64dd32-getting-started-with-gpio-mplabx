/*
    \file   main.c
    \brief  Main source file.
    (c) 2020 Microchip Technology Inc. and its subsidiaries.
    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
 */

#define F_CPU    4000000UL

#include <avr/io.h>
#include <util/delay.h>

/* Default fuses configuration:
- BOD disabled
- Oscillator in High-Frequency Mode
- UPDI pin active (WARNING: DO NOT CHANGE!)
- RESET pin used as GPIO
- CRC disabled
- MVIO enabled for dual supply
- Watchdog Timer disabled
*/
FUSES =
{
.BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
.BOOTSIZE = 0x0,
.CODESIZE = 0x0,
.OSCCFG = CLKSEL_OSCHF_gc,
.SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc | UPDIPINCFG_UPDI_gc,
.SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_0MS_gc,
.WDTCFG = PERIOD_OFF_gc | WINDOW_OFF_gc,
};


#define STEP_DELAY          10
#define THRESHOLD           100					/* 100 steps x 10 ms/step -> 1000 ms */

#define SLOW_BLINK          500
#define FAST_BLINK          100
#define NUMBER_OF_BLINKS    6

static inline void LED_blink(uint16_t time_ms, uint8_t blinks)
{
    while(blinks--)
    {
        PORTF.OUTTGL = PIN5_bm;
        _delay_ms(time_ms);
    }
}

int main(void)
{
    uint8_t counter;
    /* button init */
    PORTF.DIRCLR = PIN6_bm; /* set PF6 as input */
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm; /* enable internal pull-up */
    /* LED init */
    PORTF.OUTSET = PIN5_bm; /* LED is turned off */
    PORTF.DIRSET = PIN5_bm; /* set PF5 as output */

    while (1)
    {
        if (!(PORTF.IN & PIN6_bm)) /* check PF6 button is pressed */
        {
            counter = 0;
            while (!(PORTF.IN & PIN6_bm)) /* wait until PF6 button is released */
            {
                _delay_ms(STEP_DELAY);
                counter++;
                if (counter >= THRESHOLD)
                {
                    LED_blink(SLOW_BLINK, NUMBER_OF_BLINKS);
                    while (!(PORTF.IN & PIN6_bm));  /* wait until PF6 button is released */
                    break;
                }
            }
            if (counter < THRESHOLD)
            {
                LED_blink(FAST_BLINK, NUMBER_OF_BLINKS);
            }
        }
    }
}
