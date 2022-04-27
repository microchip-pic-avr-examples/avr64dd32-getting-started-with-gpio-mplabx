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

#include <util/delay.h>
#include <util/atomic.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdbool.h>

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

volatile bool button_event;

ISR(PORTF_PORT_vect)
{
    if(PORTF.INTFLAGS & PIN6_bm)
    {
        PORTF.INTFLAGS = PIN6_bm; /* write '1' to clear */
        button_event = true;
    }
}

void LED_Init(void)
{
    PORTF.OUTSET = PIN5_bm;
    PORTF.DIRSET = PIN5_bm;
}

void LED_Toggle(void)
{
    PORTF.OUTTGL = PIN5_bm;
}

void BUTTON_Init(void)
{
    PORTF.DIRCLR = PIN6_bm;
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

bool BUTTON_IsPressed(void)
{
    return !(PORTF.IN & PIN6_bm);
}

int main(void)
{  
    LED_Init();
    BUTTON_Init();

    button_event = false;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    sei();

    while (1)
    {
        sleep_mode();
        
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            _delay_ms(10);
        }
        
        if(button_event)
        {
            if(BUTTON_IsPressed())
            {
                LED_Toggle();
            }
            button_event = false;
        }
    }
}
