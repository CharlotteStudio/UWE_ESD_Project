#include <msp430.h>
#include <stdbool.h>
#include <string.h>

/////+++++/////+++++///// Function /////+++++/////+++++/////

extern void ClearDisplay();
extern void DisplayIcon();
extern void DisplayChar();
extern void DisplayTime();
extern void DisplayWeek();
extern void StartDisplay();
extern int  DaysInMonth();
extern void ParseString();
extern void CreateAlarmCommand();

void Init();
void ResetChrono();
void ResetTimeSetting();
void SetUpButton();
void SwtichMode();

void CheckOnClickButtonStartStop();
void CheckOnClickButtonLapReset();
void CheckOnClickButtonMode();
void OnClickSetTime();
void OnClickSetAlarm();

void UpdateNormalTimer();
void UpdateStopwatchTimer();

void ShowTimer();
void ShowCalender();
void ShowAlarm();
void ShowSettingTimer();
void ShowSettingCalender();
void ShowSettingAlarm();
void ShowStopwatch();

void DisplayDay();
void DisplayMonth();

void InitURAT();
void URAT_Send_String();
void URAT_Send_Value();
void SendAlarmSetting();
void SendAlarmOnOff();
void HandleURATCommand();

void SetUpTimeByTimestamp();
void SetUpAlarmByTimestamp();

/////+++++/////+++++///// Button /////+++++/////+++++/////

#define Button_StartStop  BIT1  // P1.1 Button
#define Button_LapReset   BIT5  // P4.5 Button
#define Button_Mode       BIT3  // P1.3 Button

volatile bool onClickButtonStartStop = false;
volatile bool onClickButtonLapReset  = false;
volatile bool onClickButtonMode      = false;

volatile bool startStopWatch = false;

/////+++++/////+++++///// Mode /////+++++/////+++++/////

// Mode :
#define NormalMode       0
#define TimeSettingMode  1
#define AlarmSettingMode 2
#define ChronoMode       3

// TimeSetting :
#define TimeSetting_Hour   0
#define TimeSetting_Min    1
#define TimeSetting_Week   2
#define TimeSetting_Day    3
#define TimeSetting_Month  4

volatile unsigned int currentMode        = 0;
volatile unsigned int currentTimeSetting = 0;

/////+++++/////+++++///// Time /////+++++/////+++++/////

#define MonthMax  12
#define WeekMax    7
#define HourMax   24
#define MinMax    60

volatile unsigned int chrono_milliseconds = 0;
volatile unsigned int chrono_seconds      = 0;
volatile unsigned int chrono_minutes      = 0;

volatile unsigned int alarm_hours   = 7;
volatile unsigned int alarm_minutes = 30;

volatile unsigned int milliseconds  = 0;
volatile unsigned int seconds       = 0;
volatile unsigned int minutes       = 0;
volatile unsigned int hours         = 0;
volatile unsigned int week          = 1; // 1 ~ 7
volatile unsigned int day           = 2; // 1 ~ 31
volatile unsigned int month         = 4; // 1 ~ 12
volatile         bool isLeapYear    = false;

/////+++++/////+++++///// Internet update /////+++++/////+++++/////

volatile bool activeInternetUpdate = false;

/////+++++/////+++++///// Alarm /////+++++/////+++++/////

#define AlarmCoolDownSecond 300
#define AlarmOnSecond 60

volatile bool activeAlarm = false;
volatile bool onAlarm     = false;
volatile unsigned int alarmCoolDown = 0;
volatile unsigned int alarmOnTime = 0;

/////+++++/////+++++///// Display /////+++++/////+++++/////

// Font
#define Empty        2
#define IconMargin   4
#define TimerMargin  24
#define MarkIcon     48
#define lineIcon     49

// Icon
#define WiFiMargin          0
#define NormalTimerMargin   4
#define ChronoMargin        6
#define AlarmMargin         8
#define CalenderMargin      11

#define AlarmIcon    0
#define ChronoIcon   2
#define WiFiIcon     4
#define TimerIcon    6
#define CalenderIcon 8

/////+++++/////+++++///// RxTx /////+++++/////+++++/////

int adclist[100];
volatile int adcpointer = 0;
volatile int rPeakDetected = 0;  // Flag to indicate R-peak detection

#define RX_BUFFER_SIZE 64
volatile char         receivedBuffer[RX_BUFFER_SIZE];
volatile unsigned int receivedIndex = 0;
volatile bool         receivedEnd = false;
volatile int          sendoutCoolDown = 0;

int main(void)
{
    Init();
    InitURAT();
    SetUpButton();

////////////////////////////////////////////////////////////////////////////////////

    TA0CCR0 =  999;                 // Count up to 999, t = 1 / (999 / 1MHz)
    TA0CCTL0 = 0x10;                // Enable counter interrupts, bit 4=1
    TA0CTL =  TASSEL_2 + MC_1;      // Timer A using subsystem master clock, SMCLK(1 MHz)
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

        HandleURATCommand();
        UpdateNormalTimer();

        switch (currentMode) {
            case NormalMode:
                if (onClickButtonLapReset)
                    ShowAlarm();
                else if (onClickButtonStartStop)
                    ShowCalender();
                else
                    ShowTimer();
                break;

            case TimeSettingMode:
                if (currentTimeSetting < TimeSetting_Day)
                    ShowSettingTimer();
                else
                    ShowSettingCalender();
                break;

            case AlarmSettingMode:
                ShowSettingAlarm();
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
    if (startStopWatch)
        chrono_milliseconds++;
}

// for read the URAT
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    char temp[1];
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE:
            break;

        case USCI_UART_UCRXIFG:
            temp[0] = UCA0RXBUF;

            if (temp[0] == '\r')
            {
                receivedBuffer[receivedIndex] = '\0';
                receivedEnd = true;
                receivedIndex = 0;
            } 
            else
            {
                if (receivedIndex < RX_BUFFER_SIZE - 1)
                    receivedBuffer[receivedIndex++] = temp[0];
            }
            break;

        case USCI_UART_UCTXIFG:
            break;

        default: 
            break;
    }
}

void Init(){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    currentMode        = NormalMode;
    onClickButtonMode  = false;

    ResetChrono();
    ResetTimeSetting();
    
    minutes       = 38;
    hours         = 14;
    week          = 1;
    day           = 2;
    month         = 4;
    isLeapYear    = false;
    alarm_hours   = 7;
    alarm_minutes = 30;
    activeInternetUpdate = false;
    activeAlarm   = false;
    onAlarm       = false;
    alarmOnTime   = 0;
    alarmCoolDown = 0;
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

            if (onAlarm){
                SendAlarmOnOff(0);
                onAlarm = false;
                return;
            }

            if (currentMode == NormalMode && onClickButtonLapReset)     // set Alarm on / off
                activeAlarm = !activeAlarm;

            if (currentMode == TimeSettingMode)
                OnClickSetTime();

            if (currentMode == AlarmSettingMode)
                OnClickSetAlarm();

            if (currentMode == ChronoMode){
                startStopWatch = !startStopWatch;
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
            if (week > WeekMax)
                week = 1;
            break;

        case TimeSetting_Day:
            day++;
            if (day > DaysInMonth(month, 2025))
                day = 1;
            break;

        case TimeSetting_Month:
            month++;
            if (month > MonthMax)
                month = 1;
            break;
    }
}

void OnClickSetAlarm(){
    switch (currentTimeSetting) {
        case TimeSetting_Hour:
            alarm_hours++;
            if (alarm_hours >= HourMax)
                alarm_hours = 0;
            break;
        case TimeSetting_Min:
            alarm_minutes++;
            if (alarm_minutes >= MinMax)
                alarm_minutes = 0;
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

            if (onAlarm){
                SendAlarmOnOff(0);
                onAlarm = false;
                return;
            }

            if (currentMode == TimeSettingMode){
                currentTimeSetting++;
                if (currentTimeSetting > TimeSetting_Month)
                    currentTimeSetting = TimeSetting_Hour;
            }

            if (currentMode == AlarmSettingMode){
                currentTimeSetting++;
                if (currentTimeSetting > TimeSetting_Min)
                    currentTimeSetting = TimeSetting_Hour;
            }

            if (currentMode == ChronoMode){
                if (!startStopWatch)
                    ResetChrono();
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

            // set auto update on / off
            if (currentMode == NormalMode && onClickButtonStartStop){
                activeInternetUpdate = !activeInternetUpdate;
                return;
            }

            SwtichMode();
        }
    }
}

void SwtichMode(){
    currentMode++;

    if (currentMode > ChronoMode){
        currentMode = NormalMode;
        SendAlarmSetting();
    }
    
    switch (currentMode) {
        case TimeSettingMode:
            ResetTimeSetting();
            break;
        case AlarmSettingMode:
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
        if (alarmCoolDown > 0)
            alarmCoolDown--;

        if (alarmOnTime > 0)
            alarmOnTime--;

        if (sendoutCoolDown > 0)
            sendoutCoolDown--;

        if(seconds >= MinMax)
        {
            seconds = 0;
            minutes++;

            // Alarm on 1 min
            if (sendoutCoolDown == 0 && onAlarm && alarmOnTime == 0){
                onAlarm = false;
                SendAlarmOnOff(0);
            }

            // Send Alarm on & Set Cool Down 300 second
            if (sendoutCoolDown == 0 && activeAlarm && minutes == alarm_minutes&& hours == alarm_hours && !onAlarm && alarmCoolDown == 0){
                onAlarm = true;
                alarmOnTime = AlarmOnSecond;
                alarmCoolDown = AlarmCoolDownSecond;
                SendAlarmOnOff(1);
            }
            
            if(minutes >= MinMax)
            {
                minutes = 0;
                hours++;

                if (hours >= HourMax)
                {
                    hours = 0;
                    day++;
                    week++;
                    
                    if (day > DaysInMonth(month, 2025)){
                        day = 1;
                        month++;
                        if (month > MonthMax)
                        {
                            month = 1;
                        }
                    }

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

void ShowTimer(){
    ClearDisplay();

    if (activeInternetUpdate)
        DisplayIcon(WiFiIcon,  IconMargin, WiFiMargin);

    //DisplayIcon(ChronoIcon, IconMargin, ChronoMargin);
    DisplayIcon(TimerIcon,  IconMargin, NormalTimerMargin);
    //DisplayIcon(CalenderIcon, IconMargin, CalenderMargin);

    if (activeAlarm)
        DisplayIcon(AlarmIcon, IconMargin, AlarmMargin);

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

void ShowCalender(){
    ClearDisplay();

    if (activeInternetUpdate)
        DisplayIcon(WiFiIcon,  IconMargin, WiFiMargin);

    DisplayIcon(CalenderIcon,  IconMargin, CalenderMargin);

    if (activeAlarm)
        DisplayIcon(AlarmIcon, IconMargin, AlarmMargin);

    DisplayDay();

    DisplayChar(lineIcon, TimerMargin, 5);

    DisplayMonth();

    DisplayWeek(week, TimerMargin, 11);

    StartDisplay();
}

void ShowAlarm(){
    ClearDisplay();

    if (activeInternetUpdate)
        DisplayIcon(WiFiIcon,  IconMargin, WiFiMargin);

    DisplayIcon(AlarmIcon, IconMargin, AlarmMargin);

    DisplayTime((alarm_hours / 10), TimerMargin, 1);
    DisplayTime((alarm_hours % 10), TimerMargin, 3);

    DisplayChar(MarkIcon, TimerMargin, 5);

    DisplayTime((alarm_minutes / 10), TimerMargin, 6);
    DisplayTime((alarm_minutes % 10), TimerMargin, 8);

    DisplayChar(Empty, TimerMargin, 11);
    DisplayChar(Empty, TimerMargin, 12);
    DisplayChar(Empty, TimerMargin, 13);
    DisplayChar(Empty, TimerMargin, 14);

    StartDisplay();
}

void ShowSettingTimer(){
    ClearDisplay();

    DisplayIcon(TimerIcon,  IconMargin, NormalTimerMargin);

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

void ShowSettingCalender(){
    ClearDisplay();

    DisplayIcon(CalenderIcon,  IconMargin, CalenderMargin);

    if (currentTimeSetting == TimeSetting_Day){
        if ((seconds % 2) == 0)
        {
            DisplayDay();
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 1);
            DisplayChar(Empty, TimerMargin, 2);
            DisplayChar(Empty, TimerMargin, 3);
            DisplayChar(Empty, TimerMargin, 4);
        }
    } 
    else 
    {
        DisplayDay();
    }

    DisplayChar(lineIcon, TimerMargin, 5);

    if (currentTimeSetting == TimeSetting_Month){
        if ((seconds % 2) == 0)
        {
            DisplayMonth();
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 6);
            DisplayChar(Empty, TimerMargin, 7);
            DisplayChar(Empty, TimerMargin, 8);
            DisplayChar(Empty, TimerMargin, 9);
        }
    } 
    else
    {
        DisplayMonth();
    }

    DisplayWeek(week, TimerMargin, 11);

    StartDisplay();
}

void ShowSettingAlarm(){
    ClearDisplay();

    DisplayIcon(AlarmIcon, IconMargin, AlarmMargin);

    if (currentTimeSetting == TimeSetting_Hour){
        if ((seconds % 2) == 0)
        {
            DisplayTime((alarm_hours / 10), TimerMargin, 1);
            DisplayTime((alarm_hours % 10), TimerMargin, 3);
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 1);
            DisplayChar(Empty, TimerMargin, 2);
            DisplayChar(Empty, TimerMargin, 3);
            DisplayChar(Empty, TimerMargin, 4);
        }
    } else {
        DisplayTime((alarm_hours / 10), TimerMargin, 1);
        DisplayTime((alarm_hours % 10), TimerMargin, 3);
    }

    DisplayChar(MarkIcon, TimerMargin, 5);

    if (currentMode == AlarmSettingMode && currentTimeSetting == TimeSetting_Min){
        if ((seconds % 2) == 0)
        {
            DisplayTime((alarm_minutes / 10), TimerMargin, 6);
            DisplayTime((alarm_minutes % 10), TimerMargin, 8);
        }
        else
        {
            DisplayChar(Empty, TimerMargin, 6);
            DisplayChar(Empty, TimerMargin, 7);
            DisplayChar(Empty, TimerMargin, 8);
            DisplayChar(Empty, TimerMargin, 9);
        }
    } else {
        DisplayTime((alarm_minutes / 10), TimerMargin, 6);
        DisplayTime((alarm_minutes % 10), TimerMargin, 8);
    }

    DisplayChar(Empty, TimerMargin, 11);
    DisplayChar(Empty, TimerMargin, 12);
    DisplayChar(Empty, TimerMargin, 13);
    DisplayChar(Empty, TimerMargin, 14);

    StartDisplay();
}

void ShowStopwatch(){
    ClearDisplay();

    DisplayIcon(ChronoIcon, IconMargin, ChronoMargin);

    DisplayTime((chrono_minutes / 10), TimerMargin, 1);
    DisplayTime((chrono_minutes % 10), TimerMargin, 3);
    DisplayTime((chrono_seconds / 10), TimerMargin, 6);
    DisplayTime((chrono_seconds % 10), TimerMargin, 8);
    DisplayTime((chrono_milliseconds / 100), TimerMargin, 11);
    DisplayTime(((chrono_milliseconds / 10) % 10), TimerMargin, 13);
    StartDisplay();
}

void DisplayDay(){
    if ((day / 10) < 1){
        DisplayChar(Empty, TimerMargin, 1);
        DisplayChar(Empty, TimerMargin, 2);
    }
    else
        DisplayTime((day / 10), TimerMargin, 1);
    DisplayTime((day % 10), TimerMargin, 3);
}

void DisplayMonth(){
    if ((month / 10) < 1){
        DisplayChar(Empty, TimerMargin, 6);
        DisplayChar(Empty, TimerMargin, 7);
    }
    else
        DisplayTime((month / 10), TimerMargin, 6);
    DisplayTime((month % 10), TimerMargin, 8);
}

void InitURAT(void) {
    // Configure GPIO
    P1OUT &= ~BIT0;                           // Clear P1.0 output latch
    P1DIR |= BIT0;                            // For LED on P1.0
    P2SEL1 |= BIT0 | BIT1;                    // Configure UART pins
    P2SEL0 &= ~(BIT0 | BIT1);
    PJSEL0 |= BIT4 | BIT5;                    // Configure XT1 pins

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // XT1 Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL4 &= ~LFXTOFF;                       // Enable LFXT1

    do {
      CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
      SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
    CSCTL0_H = 0;                             // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 = UCSSEL__ACLK;                  // Set ACLK = 32768 as UCBRCLK
    UCA0BR0 = 3;                              // 9600 baud
    UCA0MCTLW |= 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
                                              // UCBRSx value = 0x53 (See UG)
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;                     // release from reset
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void URAT_Send_String(const char *str) {
    while (*str != '\0') {
        while (!(UCA0IFG & UCTXIFG));  // 等待發送緩衝區準備
        UCA0TXBUF = *str++;            // 發送當前字符並移動指針
    }
}

void URAT_Send_Value(int value) {
    char buffer[10];
    int i = 0;
    int num = value;

    // Convert number to string (reverse order)
    do {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);
    
    // Send characters in reverse (correct) order
    while (i--) {
        while (!(UCA0IFG & UCTXIFG));  // Wait for TX buffer
        UCA0TXBUF = buffer[i];
    }

    // Send a newline or other delimiter if needed
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = '\n';
}

void SendAlarmSetting(){
    if (sendoutCoolDown > 0) return;
    sendoutCoolDown = 2;
    char alarm_command[12];
    CreateAlarmCommand(alarm_hours, alarm_minutes, alarm_command);
    URAT_Send_String(alarm_command);
}

void SendAlarmOnOff(int value){
    if (sendoutCoolDown > 0) return;
    sendoutCoolDown = 2;
    if (value > 0)
        URAT_Send_String("4,on,0");
    else
        URAT_Send_String("4,off,0");
}

// Command
// 1 -> Set time
// 2 -> On Off Auto update
// 3 -> Set Alarm
// 4 -> On/Off Alarm
// 5 -> Start / Stop Buzzer
void HandleURATCommand(){
    if (receivedEnd)
    {
        int messageParts[8];
        ParseString(receivedBuffer, messageParts);

        switch (messageParts[0]) {
            case 1:
                if (activeInternetUpdate)
                    SetUpTimeByTimestamp(messageParts);
                break;
            case 3:
                SetUpAlarmByTimestamp(messageParts);
                break;
        }

        receivedEnd = false;
    }
}

void SetUpTimeByTimestamp(int message[8]){
    int year = message[1];
    month    = message[2];
    day      = message[3];
    week     = message[4];
    hours    = message[5];
    minutes  = message[6];
    seconds  = message[7];
}

void SetUpAlarmByTimestamp(int message[8]){
    alarm_hours   = message[1];
    alarm_minutes = message[2];
}
