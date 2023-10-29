/**
 ******************************************************************************
  * @file		: ds_debug_tools.cpp
  * @brief		: Performance Monitor & printf functions
  *				  This file contains performance monitor & printf functions
  * @author		: Onur Yildirim
  * @date		: 06.02.2020
  * @version	: 0.1.0
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 DASAL, All Rights Reserved </center></h2>
  *
  * All information contained herein is, and remains the property of DASAL. The
  * intellectual and technical concepts contained herein are proprietary to DASAL
  * and are protected by trade secret or copyright law. Dissemination of this
  * information or reproduction of this material is strictly forbidden unless
  * prior written permission is obtained from DASAL.  Access to the source code
  * contained herein is hereby forbidden to anyone except current DASAL employees,
  * managers or contractors who have executed Confidentiality and Non-disclosure
  * agreements explicitly covering such access.
  *
  *
 ******************************************************************************
  */



/*
 * Begin of Includes
 */
#include "ds_debug_tools.hpp"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "tim.h"
#include "ds_telemetry_core.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Tools::PERF_MONITOR monitor;
// End of Object Definitions



namespace Tools
{



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PERF_MONITOR::PERF_MONITOR() 
{
    
}



/**
  * @brief 		Start time measuring
  *
  * @param[in]  time_measure_channel_map_type channel: Channel Map
  *
  * @return 	void
  */
void PERF_MONITOR::startElapsedTimeMeasure(time_measure_channel_map_type channel)
{
    start_time_us[static_cast<uint8_t>(channel)]   = getMicros();
    measure_started[static_cast<uint8_t>(channel)] = true;
}



/**
  * @brief 		Returns the elapsed time in us continuously
  *
  * @param[in]  time_measure_channel_map_type channel: Channel Map
  *
  * @return 	uint32_t : elapsed time in us
  */
uint32_t PERF_MONITOR::getElapsedTime_us(time_measure_channel_map_type channel)
{
    uint32_t temp_us = 0;
    uint32_t result  = 1;
    
    temp_us = getMicros();
    
    if ( !measure_started[static_cast<uint8_t>(channel)] )
    { 
        result = 1;
    }
    else 
    {   
        if ( temp_us >= start_time_us[static_cast<uint8_t>(channel)] )
        { 
            result =  temp_us - start_time_us[static_cast<uint8_t>(channel)];
        }
        else                            
        { 
            result = COUNTER_PRELOAD_VALUE - start_time_us[static_cast<uint8_t>(channel)] + temp_us;
        }

        if( time_us_max[static_cast<uint8_t>(channel)] < result )
        {
            time_us_max[static_cast<uint8_t>(channel)] = result;
        }
        else
        {
        	//it is already maximum
        }
    }
    
    return result;
}



/**
  * @brief 		Returns the elapsed time in ms continuously
  *
  * @param[in]  time_measure_channel_map_type channel: Channel Map
  *
  * @return 	uint32_t : elapsed time in ms
  */
uint32_t PERF_MONITOR::getElapsedTime_ms(time_measure_channel_map_type channel)
{
    uint32_t temp_us = 0;
    uint32_t result  = 1;
    
    temp_us = getMicros();
    
    if ( !measure_started[static_cast<uint8_t>(channel)] )
    { 
        result = 1;
    }
    else
    {
        if ( temp_us >= start_time_us[static_cast<uint8_t>(channel)] )
        { 
            result = ( temp_us - start_time_us[static_cast<uint8_t>(channel)] ) / 1000;
        }
        else                            
        { 
        	result = ( COUNTER_PRELOAD_VALUE - start_time_us[static_cast<uint8_t>(channel)] + temp_us ) / 1000;
        }
    }
    
    return result;
}



/**
  * @brief 		Returns the frequency in Hz
  *
  * @param[in]  time_measure_channel_map_type channel: Channel Map
  *
  * @return 	float : frequency in Hz
  */
float PERF_MONITOR::getFrequency_Hz(time_measure_channel_map_type channel)
{
    uint32_t temp_us = 0;
    float    result  = 9999.0;
    
    temp_us = getMicros();
    
    if( !freq_calculation_started[static_cast<uint8_t>(channel)] )
    {
        freq_calculation_started[static_cast<uint8_t>(channel)] = true;
    }
    else
    {
        if ( temp_us > freq_start_time_us[static_cast<uint8_t>(channel)] )
        { 
            result = 1000000.0 / static_cast<float>( temp_us - freq_start_time_us[static_cast<uint8_t>(channel)] );
        }
        else if ( temp_us == start_time_us[static_cast<uint8_t>(channel)] )
        {
            result = 9950.0;
        }
        else
        {
        	result = 1000000.0 / static_cast<float>( COUNTER_PRELOAD_VALUE - freq_start_time_us[static_cast<uint8_t>(channel)] + temp_us );
        }
    }
    
    if( freq_hz_min[static_cast<uint8_t>(channel)] > result )
    {
        freq_hz_min[static_cast<uint8_t>(channel)] = result;
    }
    freq_start_time_us[static_cast<uint8_t>(channel)] = getMicros();
    
    return result;
}



/**
  * @brief 		Returns the cpu load in percent
  *
  * @param[in]  void	Nothing
  *
  * @return 	uint8_t : percentage
  */
uint8_t PERF_MONITOR::measureCpuLoad( void )
{
	const uint16_t MAXIMUM_TASK_TIME_US = 3000;
    uint8_t 	   result	= 0;
    uint8_t 	   k		= 0;
    
    for(k=0;k<MEASURE_CHANNEL_NUMBER;k++)
    {
        time_us_current[k] = time_us_max[k];
        time_us_max[k] 	   = 0;

        freq_hz_current[k] = freq_hz_min[k];
        freq_hz_min[k]     = 9999.0;
    }

    if( ( time_us_current[static_cast<uint8_t>(time_measure_channel_map_type::LOOP_ALL)] ) > MAXIMUM_TASK_TIME_US )
    {
        cpu_load_active = 253;
    }
    else
    {
        cpu_load_active = (100.0 )*( static_cast<float>(time_us_current[static_cast<uint8_t>(time_measure_channel_map_type::LOOP_ALL)]) / 1250.0 );
    }
    
    result = cpu_load_active;
    
    cpu_load     = 100.0 - (100.0 * ( static_cast<float>(idle_time_us) / 1000000.0 )) ;
    idle_time_us = 0;

    return result;
}



/**
  * @brief 		Assign the channel to measure dt seconds
  *
  * @param[in]  dt_measure_channel_map_type channel: Channel Map
  *
  * @param[in]  float expected_dt_seconds: Initial return value
  *
  * @return 	void	Nothing
  */
void PERF_MONITOR::assignDtMeasureChannel (dt_measure_channel_map_type channel, float expected_dt_seconds)
{
    if( !dt_measure_started[static_cast<uint8_t>(channel)] )
    {
        dt_measure_started[static_cast<uint8_t>(channel)] = true;
        dt_seconds[static_cast<uint8_t>(channel)]         = expected_dt_seconds;
    }
}



/**
  * @brief 		Update the measurement
  *
  * @param[in]  dt_measure_channel_map_type channel: Channel Map
  *
  * @return 	void	Nothing
  */
void PERF_MONITOR::updateDtMeasure (dt_measure_channel_map_type channel)
{
    uint32_t current_time_us = 0;
    
    current_time_us = getMicros();
    
    if( dt_measure_started[static_cast<uint8_t>(channel)] )
    {
        if(!dt_channel_initialized[static_cast<uint8_t>(channel)])
        {
            dt_channel_initialized[static_cast<uint8_t>(channel)] = true;
        }
        else
        {
            if( current_time_us >= dt_start_time_us[static_cast<uint8_t>(channel)] )
            {
                dt_seconds[static_cast<uint8_t>(channel)] = ( static_cast<float>( current_time_us - dt_start_time_us[static_cast<uint8_t>(channel)]) )/1000000.0;
            }
            else
            {
            	dt_seconds[static_cast<uint8_t>(channel)] = ( static_cast<float>( COUNTER_PRELOAD_VALUE - dt_start_time_us[static_cast<uint8_t>(channel)]) + current_time_us )/1000000.0;
            }
        }
        dt_start_time_us[static_cast<uint8_t>(channel)] = current_time_us;
    }
    else
    {
    	//Error
    }

}



/**
  * @brief 		Returns the measured dt in seconds
  *
  * @param[in]  uint8_t channel: Channel Number
  *
  * @return 	float : dt time in seconds
  */
float PERF_MONITOR::getDtMeasure_s(dt_measure_channel_map_type channel)
{
    float result = 1.0;
    
    if( dt_measure_started[static_cast<uint8_t>(channel)] )
    {
        result = dt_seconds[static_cast<uint8_t>(channel)];
    }
    
    return result;
}



/**
  * @brief 		Returns the 64 bit timer us tick.
  *
  * @param[in]  void	Nothing
  *
  * @return 	uint32_t : elapsed time in ms
  */
uint32_t PERF_MONITOR::getMicros( void )
{
	return static_cast<uint32_t>(TIM2->CNT);  //us tick
}



/**
  * @brief 		Returns the runtime_ms
  *
  * @param[in]  uint8_t channel: Channel Number
  *
  * @return 	uint32_t : elapsed time in ms
  */
uint32_t PERF_MONITOR::getMillis ( void )
{
	return static_cast<uint32_t>(TIM5->CNT)/COUNTER_MS_DIVISOR;	//ms tick
}



/**
  * @brief 		Returns the runtime_ms
  *
  * @param[in]  uint8_t channel: Channel Number
  *
  * @return 	uint32_t : elapsed time in ms
  */
void PERF_MONITOR::delay_us (uint32_t time_us)
{
	uint32_t current_time_us 	= getMicros();
	uint32_t remaining_time_us 	= 0;

	if( ( remaining_time_us + time_us ) >= COUNTER_PRELOAD_VALUE  )
	{
		while( getMicros() > current_time_us )
		{
			;
		}
		remaining_time_us = time_us - 1*(COUNTER_PRELOAD_VALUE - current_time_us);

		while( getMicros() < remaining_time_us )
		{
			;
		}
	}
	else
	{
		while( time_us > ( getMicros() - current_time_us ) )
		{
			;
		}
	}
}



/**
  * @brief 		Returns the runtime_ms
  *
  * @param[in]  uint8_t channel: Channel Number
  *
  * @return 	uint32_t : elapsed time in ms
  */
void PERF_MONITOR::delay_ms (uint32_t time_ms)
{
	uint32_t current_time_ms 		= getMillis();
	uint32_t remaining_time_ms 	= 0;

	if( ( current_time_ms + time_ms ) >= COUNTER_PRELOAD_VALUE/COUNTER_MS_DIVISOR  )
	{
		while( getMillis() > current_time_ms )
		{
			;
		}
		remaining_time_ms = time_ms - 1*(COUNTER_PRELOAD_VALUE/COUNTER_MS_DIVISOR - current_time_ms);

		while( getMillis() < remaining_time_ms )
		{
			;
		}
	}
	else
	{
		while( time_ms > ( getMillis() - current_time_ms ) )
		{
			;
		}
	}
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PERF_MONITOR::PERF_MONITOR(const PERF_MONITOR& orig) 
{
    
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PERF_MONITOR::~PERF_MONITOR() 
{
    
}



} //End of namespace Tools



#ifdef DEBUG_PRINTF

/*
 * Begin of Object Definitions
 */
Tools::DEBUG_INTERFACE debug;       // To use DEBUG_INTERFACE class
// End of Object Definitions



namespace Tools
{



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
DEBUG_INTERFACE::DEBUG_INTERFACE() 
{
    
}



/**
  * @brief 		Sends ascii text over UART
  *
  * @param[in]  const char *fmt : Text
  *
  * @return 	int: Number of bytes sent over UART
  */
int DEBUG_INTERFACE::printf(const char *fmt, ...)               // Custom printf code over UART
{
    va_list argptr;
    int len = 1;
    char    _dstr[120];
   

    memset(&_dstr, 0x00, sizeof(_dstr) );
    va_start(argptr, fmt);
    len = vsprintf(_dstr, fmt, argptr);
    va_end(argptr);

   
    if ( (len > 0) && (len < 120 ) )
    {
    	telemetry_core.sendPacket(static_cast<uint16_t>(Telemetry::command_type::UART1_SEND_DATA), (uint8_t *)_dstr, len);
    }
    return len;
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
DEBUG_INTERFACE::DEBUG_INTERFACE(const DEBUG_INTERFACE& orig) 
{
    
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
DEBUG_INTERFACE::~DEBUG_INTERFACE() 
{
    
}



} //End of namespace Tools



#endif


