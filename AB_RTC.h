// Real Time Clock (RTC) driver
// Part number: AB-RTCMC-32.768-AIGZ-S7-T

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef AB_RTC_H
#define AB_RTC_H

/* ========================================================================== */
/* ============================= include files ============================== */
/* ========================================================================== */

/* Inclusion of system and local header files goes here */
#include <stdint.h> 
#include <stdbool.h>
#include <stdlib.h>



/* ========================================================================== */
/* =============================== constants ================================ */
/* ========================================================================== */

/* #define and enum statements go here */

#define ABRTC_DEBUG

#ifdef ABRTC_DEBUG
  #define ABRTC_LOG_INFO(...)                      do{NRF_LOG_INFO( __VA_ARGS__);}while(0)
#else
  #define ABRTC_LOG_INFO(...)   
#endif


#define   ABRTC_REG_SEC       (0x01)
#define   ABRTC_ADDR          (0xD0 >> 1 )
#define   ABRTC_SPEED         (400000)



#define ABRTC_DECIMAL_OF_SECONDS_REG  0x00
typedef struct{
  uint8_t sec_div_100	: 4;
  uint8_t sec_div_10	: 4;
}abrtc_decimal_of_seconds_t;



#define ABRTC_SECONDS_REG 0x01
typedef struct{
  uint8_t seconds : 7;
  uint8_t os      : 1;
}abrtc_seconds_t;

typedef enum{
  osc_is_enabled  = 0, // 32.768kHz oscillator is enabled and starts within Tstart = 1 sec. 
  osc_is_disabled = 1, // 32.768kHz oscillator is disabled (stopped) 
}abrtc_os_t;


#define ABRTC_MINUTES_REG 0x02
typedef struct{
  uint8_t minutes : 7;
  uint8_t ofie    : 1;
}abrtc_minutes_t;

typedef enum{
  osc_fail_intrpt_enabled  = 1, //  Oscillator fail interrupt is enabled; an interrupt will be issued when an oscillator failure is detected
  osc_fail_intrpt_disabled = 0,
}abrtc_ofie_t;


#define ABRTC_HOURS_REG 0x03

typedef struct{
  uint8_t hours     : 6;
  uint8_t reserved  : 2; // must be set to “0” 
}abrtc_hours_t;

#define ABRTC_DAY_REG 0x04
typedef struct{
  uint8_t weekday	 : 3;
  uint8_t reserved : 1; // must be set to “0” 
  uint8_t clockout_freq :4;
}abrtc_dayes_t;

typedef enum{
  Sunday	= 0b001,
  Monday	= 0b010,
  Tuesday	= 0b011,
  Wednesday	= 0b100,
  Thursday	= 0b101,
  Friday	= 0b110,
  Saturday	= 0b111,
}abrtc_wekday_t;

#define ABRTC_DATE_REG              0x05
typedef struct{
	uint8_t date		:6;
	uint8_t reserved	:2; // must be set to “0” 
}abrtc_date_t;


#define ABRTC_MONTH_CENTURY_REG     0x06
typedef struct{
  uint8_t month		:5;
  uint8_t reserved	:1; // must be set to “0” 
  uint8_t century	:2;
}abrtc_month_century_t;

typedef enum{
  January 	= 0b00001,
  February 	= 0b00010,
  March		= 0b00011,
  April		= 0b00100,
  May 		= 0b00101,
  June 		= 0b00110,
  July 		= 0b00111,
  August	= 0b01000,
  September 	= 0b01001,
  October 	= 0b10000,
  November 	= 0b10001,
  December 	= 0b10010, 
}abrtc_month_t;

typedef enum{
  century_20xx = 0b00,
  century_21xx = 0b01,
  century_22XX = 0b10,
  century_23xx = 0b11,
}abrtc_century_t;

#define ABRTC_YEAR_REG                0x07

typedef  struct{
  uint8_t year    :8; // 0-99
}abrtc_year_t;


typedef struct{
  abrtc_decimal_of_seconds_t decimal_seconds;
  abrtc_seconds_t            seconds;
  abrtc_minutes_t            minutes;
  abrtc_hours_t              hours;
  abrtc_dayes_t              dayes;
  abrtc_date_t               date;
  abrtc_month_century_t      month_century;
  abrtc_year_t               year;
}abrtc_calendar_t;


#define ABRTC_FREQ_COMPENSATION_REG   0x08
typedef struct{
  uint8_t calibration_value  :5;
  uint8_t mode               :1;
  uint8_t reserved           :1;
  uint8_t out                :1;
}abrtc_freq_compensation_t; 

#define ABRTC_WATCHDOG_REG            0x09
typedef struct{
  uint8_t wd0   :1;
  uint8_t wd1   :1;
  uint8_t wdm0  :1;
  uint8_t wdm1  :1;
  uint8_t wdm2  :1;
  uint8_t wdm3  :1;
  uint8_t wdm4  :1;
  uint8_t wd2   :1;
}abrtc_watchog_t; 

#define ABRTC_MONTH_ALARM_REG         0x0A
typedef struct{
  uint8_t month_alarm :6;
  uint8_t reserved    :1;   // must be set to “0” 
  uint8_t clk_out_en  :1;
  uint8_t afe         :1;
}abrtc_month_alarm_t;

typedef enum{
  clk_out_enable  = 0,
  clk_out_disable = 1,
}abrtc_clk_out_t;

#define ABRTC_DATE_ALARM_REG 0x0B
typedef struct{
  uint8_t date_alarm  :6;
  uint8_t arm_4_5     :2;
}abrtc_date_alarm_t;


#define ABRTC_HOUR_ALARM_REG 0x0C
typedef struct{
  uint8_t hour_alarm  :6;
  uint8_t reserved    :1;    // must be set to “0”
  uint8_t arm_3       :1;
}abrtc_hour_alarm_t;

#define ABRTC_MINUTE_ALARM_REG        0x0D
typedef struct{
  uint8_t minute_alarm  :7;
  uint8_t arm_2         :1;
}abrtc_minute_alarm_t;


#define ABRTC_SECOND_ALARM_REG        0x0E
typedef struct{
  uint8_t second_alarm  :7;
  uint8_t arm_1         :1;
}abrtc_second_alarm_t;



#define ABRTC_FLAGS_REG               0x0F
typedef struct{
  uint8_t res       :2;
  uint8_t of        :1;
  uint8_t reserved  :3;
  uint8_t af        :1;
  uint8_t wdf       :1;
}abrtc_flags_t; 


typedef enum{
  oscilator_failure_not_detected = 0,
  oscilator_failure_detected     = 1,
}abrtc_of_t;


/* ========================================================================== */
/* ============================= public data ================================ */
/* ========================================================================== */

/* Definition of public (external) data types go here */

typedef struct
{
  unsigned char YearLow;
  unsigned char YearHigh;
  unsigned char Month;
  unsigned char Day;
  unsigned char Hours;
  unsigned char Minutes;
  unsigned char Seconds;
  unsigned char Ready;
} ttime;

extern ttime Time;

typedef struct
{
  unsigned char Sec;
  unsigned char Min;
  unsigned char Hour;
  unsigned char WeekDay;
  unsigned char Day;
  unsigned char Month;
  unsigned char Year;
} tabrtc;

extern tabrtc   abrtc;
extern unsigned char abst;



/* ========================================================================== */
/* ========================== public functions ============================== */
/* ========================================================================== */

/* Function prototypes for public (external) functions go here */

uint32_t abrtc_init(uint8_t* osc_running);

uint32_t AB_RTC_set_timestamp(uint32_t unix_time_stamp);
uint32_t AB_RTC_get_timestamp();



#endif
#ifdef __cplusplus
}
#endif //AB_RTC_H
