/*--------------------------------------------------------------------------  */
/*  RTC controls for STM32                                                    */
/*  Copyright (c) 2009, Martin Thomas 4/2009, BSD-license                     */
/*  partly based on code from STMircoelectronics, Peter Dannegger, "LaLaDumm" */
/*--------------------------------------------------------------------------  */

#include "rtc.h"

#define FIRSTYEAR 2000    // start year
#define FIRSTDAY 6        // 0 = Sunday

static const uint8_t DaysInMonth[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*******************************************************************************
 * Function Name  : isDST
 * Description    : checks if given time is in Daylight Saving time-span.
 * Input          : time-struct, must be fully populated including weekday
 * Output         : none
 * Return         : false: no DST ("winter"), true: in DST ("summer")
 *  DST according to German standard
 *  Based on code from Peter Dannegger found in the microcontroller.net forum.
 *******************************************************************************/
static uint8_t isDST(const RTC_t* t)
{
    uint8_t wday, month;    // locals for faster access

    month = t->month;

    if(month < 3 || month > 10) {    // month 1, 2, 11, 12
        return 0;                    // -> Winter
    }

    wday = t->wday;

    if(t->mday - wday >= 25 && (wday || t->hour >= 2)) {    // after last Sunday 2:00
        if(month == 10) {                                   // October -> Winter
            return 0;
        }
    }
    else {                  // before last Sunday 2:00
        if(month == 3) {    // March -> Winter
            return 0;
        }
    }

    return 1;
}

/*******************************************************************************
 * Function Name  : adjustDST
 * Description    : adjusts time to DST if needed
 * Input          : non DST time-struct, must be fully populated including weekday
 * Output         : time-stuct gets modified
 * Return         : false: no DST ("winter"), true: in DST ("summer")
 *  DST according to German standard
 *  Based on code from Peter Dannegger found in the microcontroller.net forum.
 *******************************************************************************/
static uint8_t adjustDST(RTC_t* t)
{
    uint8_t hour, day, wday, month;    // locals for faster access

    hour = t->hour;
    day = t->mday;
    wday = t->wday;
    month = t->month;

    if(isDST(t)) {
        t->dst = 1;
        hour++;             // add one hour
        if(hour == 24) {    // next day
            hour = 0;
            wday++;    // next weekday
            if(wday == 7) {
                wday = 0;
            }
            if(day == DaysInMonth[month - 1]) {    // next month
                day = 0;
                month++;
            }
            day++;
        }
        t->month = month;
        t->hour = hour;
        t->mday = day;
        t->wday = wday;
        return 1;
    }
    else {
        t->dst = 0;
        return 0;
    }
}

/*******************************************************************************
 * Function Name  : counter_to_struct
 * Description    : populates time-struct based on counter-value
 * Input          : - counter-value (unit seconds, 0 -> 1.1.2000 00:00:00),
 *                  - Pointer to time-struct
 * Output         : time-struct gets populated, DST not taken into account here
 * Return         : none
 *  Based on code from Peter Dannegger found in the microcontroller.net forum.
 *******************************************************************************/
void counter_to_struct(uint32_t sec, RTC_t* t)
{
    uint16_t day;
    uint8_t year;
    uint16_t dayofyear;
    uint8_t leap400;
    uint8_t month;

    sec -= 946684800;

    t->sec = sec % 60;
    sec /= 60;
    t->min = sec % 60;
    sec /= 60;
    t->hour = sec % 24;
    day = (uint16_t)(sec / 24);

    t->wday = (day + FIRSTDAY) % 7;    // weekday

    year = FIRSTYEAR % 100;                       // 0..99
    leap400 = 4 - ((FIRSTYEAR - 1) / 100 & 3);    // 4, 3, 2, 1

    for(;;) {
        dayofyear = 365;
        if((year & 3) == 0) {
            dayofyear = 366;                                 // leap year
            if(year == 0 || year == 100 || year == 200) {    // 100 year exception
                if(--leap400) {                              // 400 year exception
                    dayofyear = 365;
                }
            }
        }
        if(day < dayofyear) {
            break;
        }
        day -= dayofyear;
        year++;    // 00..136 / 99..235
    }
    t->year = year + FIRSTYEAR / 100 * 100;    // + century

    if(dayofyear & 1 && day > 58) {    // no leap year and after 28.2.
        day++;                         // skip 29.2.
    }

    for(month = 1; day >= DaysInMonth[month - 1]; month++) {
        day -= DaysInMonth[month - 1];
    }

    t->month = month;     // 1..12
    t->mday = day + 1;    // 1..31
}

/*******************************************************************************
 * Function Name  : struct_to_counter
 * Description    : calculates second-counter from populated time-struct
 * Input          : Pointer to time-struct
 * Output         : none
 * Return         : counter-value (unit seconds, 0 -> 1.1.2000 00:00:00),
 *  Based on code from "LalaDumm" found in the microcontroller.net forum.
 *******************************************************************************/
uint32_t struct_to_counter(const RTC_t* t)
{
    uint8_t i;
    uint32_t result = 0;
    uint16_t idx, year;

    year = t->year;

    /* Calculate days of years before */
    result = (uint32_t)year * 365;
    if(t->year >= 1) {
        result += (year + 3) / 4;
        result -= (year - 1) / 100;
        result += (year - 1) / 400;
    }

    /* Start with 2000 a.d. */
    result -= 730485UL;

    /* Make month an array index */
    idx = t->month - 1;

    /* Loop thru each month, adding the days */
    for(i = 0; i < idx; i++) {
        result += DaysInMonth[i];
    }

    /* Leap year? adjust February */
    if(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) {
        ;
    }
    else {
        if(t->month > 1) {
            result--;
        }
    }

    /* Add remaining days */
    result += t->mday;

    /* Convert to seconds, add all the other stuff */
    result = (result - 1) * 86400L + (uint32_t)t->hour * 3600 + (uint32_t)t->min * 60 + t->sec;

    result += 946684800;
    return result;
}

/*******************************************************************************
 * Function Name  : rtc_gettime
 * Description    : populates structure from HW-RTC, takes DST into account
 * Input          : None
 * Output         : time-struct gets modified
 * Return         : always true/not used
 *******************************************************************************/

void rtc_gettime(RTC_t* rtc)
{
    uint32_t t;

    while((t = RTC_GetCounter()) != RTC_GetCounter()) {
        ;
    }
    t -= 946684800;
    counter_to_struct(t, rtc);    // get non DST time
    adjustDST(rtc);
}

/*******************************************************************************
 * Function Name  : my_RTC_SetCounter
 * Description    : sets the hardware-counter
 * Input          : new counter-value
 * Output         : None
 * Return         : None
 *******************************************************************************/
void my_RTC_SetCounter(uint32_t cnt)
{
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current time */
    RTC_SetCounter(cnt);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

/*******************************************************************************
 * Function Name  : rtc_settime
 * Description    : sets HW-RTC with values from time-struct, takes DST into
 *                  account, HW-RTC always running in non-DST time
 * Input          : None
 * Output         : None
 * Return         : not used
 *******************************************************************************/
void rtc_settime(RTC_t* rtc)
{
    uint32_t cnt;
    volatile uint16_t i;
    RTC_t ts;

    cnt = struct_to_counter(rtc);    // non-DST counter-value
    cnt += 946684800;
    counter_to_struct(cnt, &ts);    // normalize struct (for weekday)
    if(isDST(&ts)) {
        cnt -= 60 * 60;    // Subtract one hour
    }
    PWR_BackupAccessCmd(ENABLE);
    my_RTC_SetCounter(cnt);
    PWR_BackupAccessCmd(DISABLE);
}

/*******************************************************************************
 * Function Name  : rtc_init
 * Description    : initializes HW RTC,
 *                  sets default time-stamp if RTC has not been initialized before
 * Input          : None
 * Output         : None
 * Return         : not used
 *  Based on code from a STM RTC example in the StdPeriph-Library package
 *******************************************************************************/
int rtc_init(void)
{
    volatile uint16_t i;

    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* LSI clock stabilization time */
    for(i = 0; i < 5000; i++) {
        ;
    }

    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        /* Backup data register value is not correct or not yet programmed (when
the first time the program is executed) */

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /* Reset Backup Domain */
        BKP_DeInit();

        /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);

        /* Wait till LSE is ready */
        uint32_t Timeout = 800000;
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
            Timeout--;
            if(!(Timeout))
                break;
        }

        if(Timeout)    // LSE - OK.
        {
            /* Select LSE as RTC Clock Source */
            RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    // LSE = 32.768 KHz

            /* Enable RTC Clock */
            RCC_RTCCLKCmd(ENABLE);

            /* Wait for RTC registers synchronization */
            RTC_WaitForSynchro();

            /* Wait until last write operation on RTC registers has finished */
            RTC_WaitForLastTask();

            /* Set RTC prescaler: set RTC period to 1sec */
            RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
        }
        else    // LSE - NO.
        {
            RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);    // HSE = 8.00 MHz

            /* Enable RTC Clock */
            RCC_RTCCLKCmd(ENABLE);

            /* Wait for RTC registers synchronization */
            RTC_WaitForSynchro();

            /* Wait until last write operation on RTC registers has finished */
            RTC_WaitForLastTask();

            /* Set RTC prescaler: set RTC period to 1sec */
            RTC_SetPrescaler(62499);

            // Error LSE.
        }

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Set initial value */
        RTC_SetCounter((uint32_t)(1262304000));    // here: 1st January 2010 11:55:00

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);

        /* Lock access to BKP Domain */
        PWR_BackupAccessCmd(DISABLE);
    }
    else {
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
    }

    return 0;
}

int RTC_LSE(void)
{
    uint32_t count = 0xFFFFFFFF;

    // LSE Enable
    RCC_LSEConfig(RCC_LSE_ON);

    // Wait till LSE is ready
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && (--count) > 0) {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
    if(count == 0)
        return -1;

    // LCD Clock Source Selection
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    return 0;
}

int RTC_LSI(void)
{
    uint32_t count = 0xFFFFFFFF;

    // Enable the LSI OSC
    RCC_LSICmd(ENABLE);

    // Wait till LSI is ready
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET && (--count) > 0) {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
    if(count == 0)
        return -1;

    // Select the RTC Clock Source
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    return 0;
}

int rtc_isWork(void)
{
    int lse, lsi;
    volatile int32_t count = SystemCoreClock / 8;
    volatile uint32_t RTC_Counter_Clock;

    lse = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
    lsi = RCC_GetFlagStatus(RCC_FLAG_LSIRDY);

    if(!(lse || lsi))
        return -1;    // RTC Disable

    RTC_Counter_Clock = RTC_GetCounter();
    while(count > 0)
        count--;

    if(RTC_Counter_Clock == RTC_GetCounter())
        return -1;

    return 0;
}
