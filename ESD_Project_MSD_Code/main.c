#include <msp430.h>
#include <stdbool.h>

/////+++++/////+++++///// Function /////+++++/////+++++/////

extern void ClearDisplay();
extern void DisplayChar();
extern void DisplayTime();
extern void DisplayWeek();
extern void StartDisplay();

void Init();
void ResetChrono();
void ResetTimeSetting();
void SetUpButton();
void SwtichMode();
void CheckOnClickButtonStartStop();
void OnClickSetTime();
void CheckOnClickButtonLapReset();
void CheckOnClickButtonMode();
void UpdateNormalTimer();
void UpdateStopwatchTimer();
void ShowNormalTimer();
void ShowSettingTimer();
void ShowStopwatch();

/////+++++/////+++++///// Button /////+++++/////+++++/////

#define Button_StartStop  BIT1  // P1.1 Button
#define Button_LapReset   BIT5  // P4.5 Button
#define Button_Mode       BIT3  // P1.3 Button

volatile bool onClickButtonStartStop = false;
volatile bool onClickButtonLapReset  = false;
volatile bool onClickButtonMode      = false;

// 使用 volatile，防止编译器优化
volatile bool start = false;


/////+++++/////+++++///// Mode /////+++++/////+++++/////

// Mode :
#define NormalMode       0
#define TimeSettingMode  1
#define AlarmSettingMode 2
#define ChronoMode       3
//#define AlarmMode        

// TimeSetting :
#define TimeSetting_Hour 0
#define TimeSetting_Min  1
#define TimeSetting_Week 2

volatile unsigned int currentMode        = 0;
volatile unsigned int currentTimeSetting = 0;

/////+++++/////+++++///// Time /////+++++/////+++++/////

#define WeekMax  7
#define HourMax 24
#define MinMax  60

volatile unsigned int chrono_milliseconds = 0;
volatile unsigned int chrono_seconds      = 0;
volatile unsigned int chrono_minutes      = 0;

volatile unsigned int milliseconds  = 0;
volatile unsigned int seconds       = 0;
volatile unsigned int minutes       = 0;
volatile unsigned int hours         = 0;
volatile unsigned int week          = 1;

/////+++++/////+++++///// Display /////+++++/////+++++/////


#define Empty       2
#define TimerMargin 10
#define MarkIcon    28


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
        CheckOnClickButtonStartStop();
        CheckOnClickButtonLapReset();
        CheckOnClickButtonMode();

        volatile unsigned int i;

        for (i = 0; i < 100; i++) {}

        switch (currentMode) {
            case NormalMode:
                UpdateNormalTimer();
                ShowNormalTimer();
                break;

            case TimeSettingMode:
                UpdateNormalTimer();
                ShowSettingTimer();
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

    currentMode        = NormalMode;
    onClickButtonMode  = false;

    ResetChrono();
    ResetTimeSetting();
    
    minutes = 38;
    hours = 14;
    week = 1;
}

void ResetChrono(){
    chrono_milliseconds = 0;
    chrono_seconds      = 0;
    chrono_minutes      = 0;
}

void ResetTimeSetting(){
    currentTimeSetting = TimeSetting_Hour;
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

void CheckOnClickButtonStartStop(){
    if ((P1IN & Button_StartStop) == 0)
    {
        if (!onClickButtonMode)
            onClickButtonStartStop = true;
    } 
    else 
    {
        if (onClickButtonStartStop)
        {
            onClickButtonStartStop = false;
        
            if (currentMode == TimeSettingMode)
                OnClickSetTime();

            if (currentMode == ChronoMode){
                start = !start;
            }

            if (currentMode > ChronoMode)
                currentMode = NormalMode;
        }
    }
}

void OnClickSetTime(){
    switch (currentTimeSetting) {
        case TimeSetting_Hour:
            hours++;
            if (hours >= HourMax)
                hours = 0;
            break;
        case TimeSetting_Min:
            minutes++;
            if (minutes >= MinMax)
                minutes = 0;
            break;
        case TimeSetting_Week:
            week++;
            if (week >= WeekMax)
                week = 1;
            break;
    }
}

void CheckOnClickButtonLapReset(){
    if ((P4IN & Button_LapReset) == 0)
    {
        if (!onClickButtonLapReset)
            onClickButtonLapReset = true;
    } 
    else 
    {
        if (onClickButtonLapReset)
        {
            onClickButtonLapReset = false;
            
            if (currentMode == TimeSettingMode){
                currentTimeSetting++;
                if (currentTimeSetting > TimeSetting_Week)
                    currentTimeSetting = TimeSetting_Hour;
            }

            if (currentMode == ChronoMode){

            }

            if (currentMode > ChronoMode)
                currentMode = NormalMode;
        }
    }
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
            SwtichMode();
        }
    }
}

void SwtichMode(){
    currentMode++;

    if (currentMode > ChronoMode)
        currentMode = NormalMode;
    
    switch (currentMode) {
        case TimeSettingMode:
            ResetTimeSetting();
            break;
        case ChronoMode:
            ResetChrono();
            break;
    } 
}

void UpdateNormalTimer(){
    if(milliseconds >= 1000)
    {
        milliseconds = 0;
        seconds++;
        
        if(seconds >= MinMax)
        {
            seconds = 0;
            minutes++;
            
            if(minutes >= MinMax)
            {
                minutes = 0;
                hours++;

                if (hours >= HourMax)
                {
                    hours = 0;
                    week++;
                    if (week > WeekMax)
                    {
                        week = 1;
                    }
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

    DisplayTime((hours / 10), TimerMargin, 1);
    DisplayTime((hours % 10), TimerMargin, 3);

    if ((seconds % 2) == 0)
        DisplayChar(MarkIcon, TimerMargin, 5);
    else
        DisplayChar(Empty, TimerMargin, 5);

    DisplayTime((minutes / 10), TimerMargin, 6);
    DisplayTime((minutes % 10), TimerMargin, 8);
    DisplayWeek(week, TimerMargin, 11);

    StartDisplay();
}

void ShowSettingTimer(){
    ClearDisplay();

    if (currentTimeSetting == TimeSetting_Hour){
        if ((seconds % 2) == 0)
        {
            DisplayTime((hours / 10), TimerMargin, 1);
            DisplayTime((hours % 10), TimerMargin, 3);
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 1);
            DisplayChar(Empty, TimerMargin, 2);
            DisplayChar(Empty, TimerMargin, 3);
            DisplayChar(Empty, TimerMargin, 4);
        }
    } else {
        DisplayTime((hours / 10), TimerMargin, 1);
        DisplayTime((hours % 10), TimerMargin, 3);
    }

    DisplayChar(MarkIcon, TimerMargin, 5);

    if (currentTimeSetting == TimeSetting_Min){
        if ((seconds % 2) == 0)
        {
            DisplayTime((minutes / 10), TimerMargin, 6);
            DisplayTime((minutes % 10), TimerMargin, 8);
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 6);
            DisplayChar(Empty, TimerMargin, 7);
            DisplayChar(Empty, TimerMargin, 8);
            DisplayChar(Empty, TimerMargin, 9);
        }
    } else {
        DisplayTime((minutes / 10), TimerMargin, 6);
        DisplayTime((minutes % 10), TimerMargin, 8);
    }

    if (currentTimeSetting == TimeSetting_Week){
        if ((seconds % 2) == 0)
        {
            DisplayWeek(week, TimerMargin, 11);
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 11);
            DisplayChar(Empty, TimerMargin, 12);
            DisplayChar(Empty, TimerMargin, 13);
            DisplayChar(Empty, TimerMargin, 14);
        }
    } else {
        DisplayWeek(week, TimerMargin, 11);
    }


    StartDisplay();
}

void ShowStopwatch(){
    ClearDisplay();
    DisplayTime((chrono_minutes / 10), TimerMargin, 1);
    DisplayTime((chrono_minutes % 10), TimerMargin, 3);
    DisplayTime((chrono_seconds / 10), TimerMargin, 6);
    DisplayTime((chrono_seconds % 10), TimerMargin, 8);
    DisplayTime((chrono_milliseconds / 100), TimerMargin, 11);
    DisplayTime(((chrono_milliseconds / 10) % 10), TimerMargin, 13);
    StartDisplay();
}