/*
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

#define F_CPU                  4000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BUTTON_INTERRUPT       (PORTF.INTFLAGS & PIN6_bm)
#define BUTTON_FALLING_EDGE    (!(PORTF.IN & (PIN6_bm)))
#define BUTTON_CLEAR_INT_FLAG  (PORTF.INTFLAGS &= PIN6_bm)

#define SHORT_DELAY             100
#define LONG_DELAY              500

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

typedef enum
{
    BUTTON_RELEASED,
    BUTTON_PRESSED
} button_status_t;

static volatile button_status_t button_status = BUTTON_RELEASED;

ISR(PORTF_PORT_vect)
{
    if (BUTTON_INTERRUPT)
    { 
        /* There is no debounce done. For the purpose of this demo
           this is not relevant.
         */

        if(BUTTON_FALLING_EDGE) 
        {
            /* Button is pressed -> long */
            button_status = BUTTON_PRESSED;
        } 
        else 
        {
            /* Button is released -> short */
            button_status = BUTTON_RELEASED;
        }

        BUTTON_CLEAR_INT_FLAG; 
    }
}

void myDelay(void)
{
    if(BUTTON_RELEASED == button_status)
    {
        _delay_ms(SHORT_DELAY);
    }
    else
    {
        _delay_ms(LONG_DELAY);
    }
}

int main (void)
{
    /* Configure PORTF 2,3,4,5 pins as outputs */
    PORTF.DIRSET = PORT_PINCTRLSET_2_bm | PORT_PINCTRLSET_3_bm | PORT_PINCTRLSET_4_bm | PORT_PINCTRLSET_5_bm;
        
    /* Enable both edges interrupt detect and internal pull-up on PF6 */ 
    PORTF.PINCONFIG      = PORT_ISC_BOTHEDGES_gc | PORT_PULLUPEN_bm;
    PORTF.PINCTRLUPD     = PORT_PINCTRLUPD_6_bm;
    /* Configure PF6 as input */
    PORTF.DIRCLR = PIN6_bm;
    
    sei();
    
    while (1)
    {
        /* PORTF pins 2,3,4,5 are inverted */
        PORTF.PINCONFIG   = PORT_INVEN_bm;
        PORTF.PINCTRLUPD  = PORT_PINCTRLSET_2_bm | PORT_PINCTRLSET_3_bm | PORT_PINCTRLSET_4_bm | PORT_PINCTRLSET_5_bm;

        myDelay();

        /* PORTF pins 2,3,4,5 are not inverted */
        PORTF.PINCONFIG   = (uint8_t)~PORT_INVEN_bm;
        PORTF.PINCTRLUPD  = PORT_PINCTRLSET_2_bm | PORT_PINCTRLSET_3_bm | PORT_PINCTRLSET_4_bm | PORT_PINCTRLSET_5_bm;

        myDelay();
    }
}
