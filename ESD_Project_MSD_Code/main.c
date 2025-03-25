#include <msp430.h>
#include <stdbool.h>

extern void ClearDisplay();
extern void DisplayChar(int, int);
extern void StartDisplay();

// 使用 volatile，防止编译器优化
volatile bool start = false;

volatile unsigned int milliseconds = 0;
volatile unsigned int seconds = 0;
volatile unsigned int minutes = 0;

#define BUTTON1 BIT1  // P1.1 Button
#define BUTTON2 BIT5  // P4.5 Button


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    // set up Button P1.1
    P1DIR &= ~BUTTON1;              // set   P1.1 to input
    P1REN |= BUTTON1;               // using P1.1 up
    P1OUT |= BUTTON1;               // set   P1.1 up

    // set up Button P4.5
    P4DIR &= ~BUTTON2;              // set   P4.5 to input
    P4REN |= BUTTON2;               // using P4.5 up
    P4OUT |= BUTTON2;               // set   P4.5 up

////////////////////////////////////////////////////////////////////////////////////

    TA0CCR0 =  1100;                // Count up to 1100, t = 1 / (1100/1.1MHz)
    TA0CCTL0 = 0x10;                // Enable counter interrupts, bit 4=1
    TA0CTL =  TASSEL_2 + MC_1;      // Timer A using subsystem master clock, SMCLK(1.1 MHz)
                                    // and count UP to create a 1ms interrupt

    // LCD
    P4DIR |= 0x04; // Set P4.2 to output direction (LCD Power On)
    P4DIR |= 0x08; // Set P4.3 to output direction (LCD Enable)
    // SPI Ports
    P1SEL0 &= ~0x40; // Set P1.6 to output direction (SPI MOSI)
    P1SEL1 |= 0x40; // Set P1.6 to output direction (SPI MOSI)
    P1DIR |= 0x40; // Set P1.6 to output direction (SPI MOSI)
    P2SEL0 &= ~0x04; // Set P2.2 to SPI mode (SPI CLK)
    P2SEL1 |= 0x04; // Set P2.2 to SPI mode (SPI CLK)
    P2DIR |= 0x04; // Set P2.2 to output direction (SPI CLK)
    P2DIR |= 0x10; // Set P2.4 to output direction (SPI CS)

    // SPI Interface
    UCB0CTLW0 |= UCSWRST;
    UCB0CTLW0 &= ~(UCCKPH + UCCKPL + UC7BIT + UCMSB);
    UCB0CTLW0 &= ~(UCSSEL_3);
    UCB0CTLW0 |= UCSSEL__SMCLK;
    UCB0BRW = 8;
    UCB0CTLW0 |= (UCMSB + UCCKPH + 0x00 + UCMST + UCSYNC + UCMODE_0);
    UCB0CTLW0 &= ~(UCSWRST);
    P4OUT |= 0x04; // Turn LCD Power On
    P4OUT |= 0x08; // Enable LCD

    _BIS_SR(GIE);

    while (1) {

        if ((P1IN & BUTTON1) == 0)
        {
            start = true;
        }

        if ((P4IN & BUTTON2) == 0)
        {
            start = false;
        }

        volatile unsigned int i;
        for (i = 0; i < 10; i++) {}

        UpdateStopwatchTime();
        ShowStopwatch();
    }
}

// Timer_A0 is a Stopwatch Timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    if (start)
        milliseconds++;
}


void UpdateStopwatchTime(){
    if(milliseconds >= 1000)
    {
        milliseconds = 0;
        seconds++;
        
        if(seconds >= 60)
        {
            seconds = 0;
            minutes++;
            
            if(minutes >= 60)
            {
                minutes = 0;
            }
        }
    }
}

void ShowStopwatch(){
    ClearDisplay();
    DisplayNumber((minutes / 10), 10, 1);
    DisplayNumber((minutes % 10), 10, 3);
    DisplayNumber((seconds / 10), 10, 6);
    DisplayNumber((seconds % 10), 10, 8);
    DisplayNumber((milliseconds / 100), 10, 11);
    DisplayNumber(((milliseconds / 10) % 10), 10, 13);
    StartDisplay();
}