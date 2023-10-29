/**
 ******************************************************************************
  * @file		: ds_debug_tools.hpp
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

#ifndef DS_DEBUG_TOOLS_HPP
#define	DS_DEBUG_TOOLS_HPP



/*************************************************/

#define DEBUG_PRINTF   // Uncomment this line if you want to use debug.printf() function over UART4
                         // When uncommented it blocks sending gimbal commands over UART4 so it can be used as debugging port
/*************************************************/



/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
 * ---- PERF_MONITOR Usage: ---------
 * ****************************************************************************
 *      float    freq            = 0;
 *      uint32_t elapsed_time_us = 0;
 *      uint8_t  free_channel    = 30;
 * 
 *      freq = monitor.getFrequency_Hz(free_channel);
 *      monitor.startMeasure(free_channel);
 *      some_function();
 *      elapsed_time_us = monitor.getElapsedTime_us(free_channel);
 * 
 *      // freq variable returns 9999 once after initialization,
 *      // after second call it gives the right frequency value in Hz
 * 
 * ****************************************************************************
 * ****************************************************************************
 * ---- DtMeasure Usage: ---------
 * ****************************************************************************
 *      float    dt_seconds      = 0;
 *      uint8_t  free_channel    = 30;
 *      float    expected_dt_seconds = 0.01;
 * 
 *      monitor.assignDtMeasureChannel(free_channel, expected_dt_seconds);
 *      
 *      monitor.updateDtMeasure(free_channel);
 *      result = monitor.getDtMeasure(free_channel);
 * 
 *      // freq variable returns expected_dt_seconds once after initialization,
 *      // after second call it gives the right dt value in seconds
 * 
 * ****************************************************************************/
/////////////////////////////////////////////////////////////////////////////////



namespace Tools
{



/*
 * Begin of Macro Definitions
 */
 const uint16_t MEASURE_CHANNEL_NUMBER = 32;
 const uint32_t COUNTER_PRELOAD_VALUE  = 4294967295;
 const uint32_t COUNTER_MS_DIVISOR		 = 4;
 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */

//Time measure Channel Map type
enum class time_measure_channel_map_type: uint8_t
{
	MAIN_TASK_IDLE_TIME	=  0,
	LOOP_ALL						=  1,
	LOOP_800_HZ					=  2,
	LOOP_400_HZ					=  3,
	LOOP_200_HZ					=  4,
	LOOP_100_HZ					=  5,
	LOOP_50_HZ					=  6,
	LOOP_20_HZ					=  7,
	LOOP_10_HZ					=  8,
	LOOP_5_HZ						=  9,
	LOOP_2_HZ						= 10,
	LOOP_1_HZ						= 11,
};


enum class dt_measure_channel_map_type: uint8_t
{
		CHANNEL1 = 0,
		CHANNEL2 = 1,
};
// End of Enum, Union and Struct Definitions



/*
 * Begin of PERF_MONITOR Class Definition
 */
class PERF_MONITOR 
{
    public:
        PERF_MONITOR();

        uint32_t    idle_time_us		= 0;
        float     	cpu_load_active	= 0;
        
        void        startElapsedTimeMeasure		( time_measure_channel_map_type channel );
        uint32_t    getElapsedTime_us     		( time_measure_channel_map_type channel );
        uint32_t    getElapsedTime_ms       	( time_measure_channel_map_type channel );
        float       getFrequency_Hz         	( time_measure_channel_map_type channel );
        uint8_t     measureCpuLoad         		( void );

        void        assignDtMeasureChannel 		( dt_measure_channel_map_type channel,
                                           			float   expected_dt_seconds);
        void        updateDtMeasure       		( dt_measure_channel_map_type channel);
        float       getDtMeasure_s						( dt_measure_channel_map_type channel);
        
        uint32_t	getMicros	( void );
        uint32_t	getMillis	( void );
        void			delay_us	( uint32_t time_us );
        void			delay_ms	( uint32_t time_ms );

        PERF_MONITOR(const PERF_MONITOR& orig);
        virtual ~PERF_MONITOR();
        
    private:
        bool        measure_started[MEASURE_CHANNEL_NUMBER]             = {false};
        uint32_t    start_time_us[MEASURE_CHANNEL_NUMBER]               = {0};
        
        bool        freq_calculation_started[MEASURE_CHANNEL_NUMBER]    = {false};
        uint32_t    freq_start_time_us[MEASURE_CHANNEL_NUMBER]          = {0};

        bool        dt_measure_started[MEASURE_CHANNEL_NUMBER]          = {false};
        bool        dt_channel_initialized[MEASURE_CHANNEL_NUMBER]      = {false};
        uint32_t    dt_start_time_us[MEASURE_CHANNEL_NUMBER]            = {0};
        float       dt_seconds[MEASURE_CHANNEL_NUMBER]                  = {0};
        
        uint32_t    time_us_max[MEASURE_CHANNEL_NUMBER]                 = {0};
        float       freq_hz_min[MEASURE_CHANNEL_NUMBER]                 = {9999};
        
        uint8_t     cpu_load 	 	= 0;

        uint32_t    time_us_current[MEASURE_CHANNEL_NUMBER] = {0};
        float       freq_hz_current[MEASURE_CHANNEL_NUMBER] = {0};
};
// End of PERF_MONITOR Class Definition



} //End of namespace Tools



/*
 * External Linkages
 */
extern Tools::PERF_MONITOR monitor;
// End of External Linkages



#ifdef DEBUG_PRINTF


namespace Tools
{



/*
 * Begin of DEBUG_INTERFACE Class Definition
 */
class DEBUG_INTERFACE
{
    public:
        DEBUG_INTERFACE();

        int printf(const char *fmt, ...);  // Custom printf code over UART
        
        DEBUG_INTERFACE(const DEBUG_INTERFACE& orig);
        virtual ~DEBUG_INTERFACE();
    private:

};
// End of DEBUG_INTERFACE Class Definition



} //End of namespace Tools



/*
 * External Linkages
 */
extern Tools::DEBUG_INTERFACE debug;       // To use DEBUG_INTERFACE class
// End of External Linkages

#endif



#endif	/* DS_DEBUG_TOOLS_HPP */

