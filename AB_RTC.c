#include "AB_RTC.h" 
#include "i2c_driver_init.h"
#include <time.h>


static uint32_t m_time_stamp;
static uint32_t minimum_epoc = 989443411;// Wednesday, 9 May 2001 ., 21:23:31 ????

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
static uint32_t abrtc_write(uint8_t reg, uint8_t* val, uint16_t len);
static uint32_t abrtc_read(uint8_t reg,  uint8_t* val, uint16_t len);
static uint32_t abrtc_decimal_of_seconds_set(abrtc_decimal_of_seconds_t val);
static uint32_t abrtc_decimal_of_seconds_get(abrtc_decimal_of_seconds_t* val);



uint32_t AB_RTC_set_timestamp(uint32_t unix_time_stamp)
{
    // Dummy abstraction until real RTC driver will be implemented
    // m_time_stamp = unix_time_stamp;
    // return 0;





  uint32_t ret = 0;
  abrtc_seconds_t sec_reg;
  abrtc_minutes_t min_reg;
  abrtc_flags_t   flag_reg;
  abrtc_calendar_t clock_regs;
  struct tm *hmn_rdbl_tm;

  Twi1_Enable();   
    
  memset(&sec_reg,0,sizeof(sec_reg));
  memset(&flag_reg,0,sizeof(flag_reg));
  
  ret  = abrtc_read(ABRTC_SECONDS_REG,(uint8_t*)&sec_reg,sizeof(sec_reg));
  ret |= abrtc_read(ABRTC_FLAGS_REG,(uint8_t*)&flag_reg,sizeof(flag_reg));
  
  if(ret == 0)
  {
    
    if( (sec_reg.os != osc_is_enabled) & (flag_reg.of != oscilator_failure_not_detected) )
    { 
      flag_reg.of = oscilator_failure_not_detected;
      ret |= abrtc_write(ABRTC_FLAGS_REG,(uint8_t*)&flag_reg,sizeof(flag_reg));   
      
      sec_reg.os = osc_is_enabled;
      ret |= abrtc_write(ABRTC_SECONDS_REG,(uint8_t*)&sec_reg,sizeof(sec_reg));
    }
    
    hmn_rdbl_tm = localtime( (time_t*)&unix_time_stamp );
    
    ret  = abrtc_read(ABRTC_DECIMAL_OF_SECONDS_REG,(uint8_t*)&clock_regs,sizeof(clock_regs));

    clock_regs.decimal_seconds.sec_div_10  = 0;
    clock_regs.decimal_seconds.sec_div_100 = 0;
    clock_regs.seconds.seconds = bin2bcd(hmn_rdbl_tm->tm_sec);
    clock_regs.minutes.minutes = bin2bcd(hmn_rdbl_tm->tm_min);
    clock_regs.hours.hours     = bin2bcd(hmn_rdbl_tm->tm_hour);
    clock_regs.dayes.weekday   = bin2bcd((hmn_rdbl_tm->tm_wday  +  1)); // weekday accepts 1-7 values tm_wday are  0-6
    clock_regs.date.date       = bin2bcd((hmn_rdbl_tm->tm_mday )); 
    clock_regs.month_century.month     = bin2bcd((hmn_rdbl_tm->tm_mon + 1)); // month accepts 1-12 values tm_wday are  0-11
    clock_regs.month_century.century   = century_20xx;
    clock_regs.year.year               = bin2bcd((hmn_rdbl_tm->tm_year + 1900) - 2000);
    ret  |= abrtc_write(ABRTC_DECIMAL_OF_SECONDS_REG,(uint8_t*)&clock_regs,sizeof(clock_regs));

    
  }

  Twi1_Disable();

  return ret;
}

uint32_t AB_RTC_get_timestamp()
{
    // Dummy abstraction until real RTC driver will be implemented
    // m_time_stamp++;
    // return m_time_stamp;






  uint32_t ret;
  abrtc_seconds_t sec_reg;
  abrtc_minutes_t min_reg;
  abrtc_flags_t   flag_reg;
  abrtc_calendar_t clock_regs;
  struct tm hmn_rdbl_tm;

  Twi1_Enable();   

  memset(&sec_reg,0,sizeof(sec_reg));
  memset(&flag_reg,0,sizeof(flag_reg));
  memset(&hmn_rdbl_tm,0,sizeof(hmn_rdbl_tm));

   
  ret  = abrtc_read(ABRTC_SECONDS_REG,(uint8_t*)&sec_reg,sizeof(sec_reg));
  ret |= abrtc_read(ABRTC_FLAGS_REG,(uint8_t*)&flag_reg,sizeof(flag_reg));

  if(ret  == 0)
  {
  
    if( (sec_reg.os != osc_is_enabled) & (flag_reg.of != oscilator_failure_not_detected) )
    { 
      flag_reg.of = oscilator_failure_not_detected;
      ret |= abrtc_write(ABRTC_FLAGS_REG,(uint8_t*)&flag_reg,sizeof(flag_reg));   
      
      sec_reg.os = osc_is_enabled;
      ret |= abrtc_write(ABRTC_SECONDS_REG,(uint8_t*)&sec_reg,sizeof(sec_reg));

      AB_RTC_set_timestamp(minimum_epoc);
      m_time_stamp = minimum_epoc;
    }
    else
    {
      
      ret  = abrtc_read(ABRTC_DECIMAL_OF_SECONDS_REG,(uint8_t*)&clock_regs,sizeof(clock_regs));
      hmn_rdbl_tm.tm_sec = bcd2bin(clock_regs.seconds.seconds);
      hmn_rdbl_tm.tm_min = bcd2bin(clock_regs.minutes.minutes);
      hmn_rdbl_tm.tm_hour = bcd2bin(clock_regs.hours.hours);
      hmn_rdbl_tm.tm_wday = bcd2bin(clock_regs.dayes.weekday) - 1;
      hmn_rdbl_tm.tm_mday = bcd2bin(clock_regs.date.date);
      hmn_rdbl_tm.tm_mon  = bcd2bin(clock_regs.month_century.month) - 1;
      hmn_rdbl_tm.tm_year = (bcd2bin(clock_regs.year.year) + 2000 ) - 1900;
      hmn_rdbl_tm.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
      m_time_stamp = mktime(&hmn_rdbl_tm);
    }
  
  }


  return m_time_stamp;
}

uint32_t abrtc_init(uint8_t* osc_does_not_stopped)
{
  uint32_t ret = 0;
  abrtc_seconds_t sec_reg;
  abrtc_minutes_t min_reg;
  abrtc_flags_t   flag_reg;
  abrtc_month_alarm_t alarm_reg;
  Twi1_Enable();
  
  memset(&sec_reg,0,sizeof(sec_reg));
  memset(&flag_reg,0,sizeof(flag_reg));
  
  ret  = abrtc_read(ABRTC_SECONDS_REG,(uint8_t*)&sec_reg,sizeof(sec_reg));
  ret |= abrtc_read(ABRTC_FLAGS_REG,(uint8_t*)&flag_reg,sizeof(flag_reg));

  
  ret |= abrtc_read(ABRTC_MONTH_ALARM_REG,(uint8_t*)&alarm_reg,sizeof(alarm_reg));
  if(alarm_reg.clk_out_en != clk_out_disable)
  {
    alarm_reg.clk_out_en = clk_out_disable; 
    ret |= abrtc_write(ABRTC_MONTH_ALARM_REG,(uint8_t*)&alarm_reg,sizeof(alarm_reg)); 
  }
  
  if(ret  == 0)
  {
    
    if( (sec_reg.os == osc_is_enabled) & (flag_reg.of == oscilator_failure_not_detected) )
    {
      *osc_does_not_stopped = 1;
    }
    else
    {
      *osc_does_not_stopped = 0;
      flag_reg.of = oscilator_failure_not_detected;
      ret |= abrtc_write(ABRTC_FLAGS_REG,(uint8_t*)&flag_reg,sizeof(flag_reg));      
    }
    
  }

  Twi1_Disable();
  return ret;
}


uint32_t abrtc_write(uint8_t reg, uint8_t* val, uint16_t len)
{
  uint32_t ret = 0;
        
    ret = TWI1_Write_Gen(ABRTC_ADDR,reg,val,len);

  return ret;
}

uint32_t abrtc_read(uint8_t reg,  uint8_t* val, uint16_t len)
{
  uint32_t ret = 0;

    ret = TWI1_Read_Gen(ABRTC_ADDR,reg,val,len);

  return ret;
}
