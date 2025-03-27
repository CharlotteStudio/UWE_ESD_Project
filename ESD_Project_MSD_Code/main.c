#include <msp430.h>
#include <stdbool.h>

extern void ClearDisplay();
extern void DisplayChar();
extern void DisplayNumber();
extern void StartDisplay();

#define Button_StartStop  BIT1  // P1.1 Button
#define Button_LapReset   BIT5  // P4.5 Button
#define Button_Mode       BIT3  // P1.3 Button

volatile bool onClickButtonMode = false;

#define NormalMode       0
#define TimeSettingMode  1
#define AlarmSettingMode 2
#define ChronoMode       3
//#define AlarmMode        

volatile unsigned int currentMode = 0;


// 使用 volatile，防止编译器优化
volatile bool start = false;

volatile unsigned int chrono_milliseconds = 0;
volatile unsigned int chrono_seconds      = 0;
volatile unsigned int chrono_minutes      = 0;

volatile unsigned int milliseconds = 0;
volatile unsigned int seconds      = 0;
volatile unsigned int minutes      = 0;
volatile unsigned int hours        = 0;

int main(void)
{
    Init();
    SetUpButton();

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

        if ((P1IN & Button_StartStop) == 0)
        {
            start = true;
            //printf("Button_StartStop");
        }

        if ((P4IN & Button_LapReset) == 0)
        {
            start = false;
            //printf("Button_StartStop");
        }

        CheckOnClickButtonMode();

        volatile unsigned int i;
        for (i = 0; i < 100; i++) {}

        switch (currentMode) {
            case NormalMode:
                UpdateNormalTimer();
                ShowNormalTimer();
                break;
            case ChronoMode:
                UpdateStopwatchTimer();
                ShowStopwatch();
                break;
        }
    }
}

// Timer_A0 is a Stopwatch Timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) 
{
    milliseconds++;
    if (start)
        chrono_milliseconds++;
}


void Init(){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    currentMode = NormalMode;
    onClickButtonMode = false;

    ResetChrono();
    
    minutes = 14;
    hours = 2;
}

void ResetChrono(){
    chrono_milliseconds = 0;
    chrono_seconds      = 0;
    chrono_minutes      = 0;
}

void SetUpButton(){
    // set up Button P1.1
    P1DIR &= ~Button_StartStop;     // set P1.1 to input
    P1REN |= Button_StartStop;      // use P1.1 up
    P1OUT |= Button_StartStop;      // set P1.1 up

    // set up Button P4.5
    P4DIR &= ~Button_LapReset;       // set P4.5 to input
    P4REN |= Button_LapReset;        // usw P4.5 up
    P4OUT |= Button_LapReset;        // set P4.5 up

    // set up Button P1.3
    P1DIR &= ~Button_Mode;           // set P1.3 to input
    P1REN |= Button_Mode;            // use P1.3 up
    P1OUT |= Button_Mode;            // set P1.3 up
}

void CheckOnClickButtonMode(){
    if ((P1IN & Button_Mode) == 0)
    {
        if (!onClickButtonMode)
            onClickButtonMode = true;
    } 
    else 
    {
        if (onClickButtonMode)
        {
            onClickButtonMode = false;
            currentMode++;
            
            if (currentMode == ChronoMode)
                ResetChrono();

            if (currentMode > ChronoMode)
                currentMode = NormalMode;
        }
    }
}

void UpdateNormalTimer(){
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
                hours++;

                if (hours >= 24)
                {
                    hours = 0;
                }
            }
        }
    }
}

void UpdateStopwatchTimer(){
    if(chrono_milliseconds >= 1000)
    {
        chrono_milliseconds = 0;
        chrono_seconds++;
        
        if(chrono_seconds >= 60)
        {
            chrono_seconds = 0;
            chrono_minutes++;
            
            if(chrono_minutes >= 60)
                chrono_minutes = 0;
        }
    }
}

void ShowNormalTimer(){
    ClearDisplay();

    DisplayNumber((hours / 10), 10, 1);
    DisplayNumber((hours % 10), 10, 3);

    if ((seconds % 2) == 0)
        DisplayChar(20, 10, 5);
    else
        DisplayChar(2, 10, 5);

    DisplayNumber((minutes / 10), 10, 6);
    DisplayNumber((minutes % 10), 10, 8);

    //DisplayNumber((sec / 10), 10, 11);
    //DisplayNumber((sec % 10), 10, 13);
    //DisplayNumber((milliseconds / 100), 10, 11);
    //DisplayNumber(((milliseconds / 10) % 10), 10, 13);
    StartDisplay();
}

void ShowStopwatch(){
    ClearDisplay();
    DisplayNumber((chrono_minutes / 10), 10, 1);
    DisplayNumber((chrono_minutes % 10), 10, 3);
    DisplayNumber((chrono_seconds / 10), 10, 6);
    DisplayNumber((chrono_seconds % 10), 10, 8);
    DisplayNumber((chrono_milliseconds / 100), 10, 11);
    DisplayNumber(((chrono_milliseconds / 10) % 10), 10, 13);
    StartDisplay();
}