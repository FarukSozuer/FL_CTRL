/**
 ******************************************************************************
  * @file		: ds_main.cpp
  * @brief		: Task scheduler source file
  *				  This file contains task structure of the application
  * @author		: Onur Yildirim
  * @date		: 05.02.2020
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
#include "ds_main.hpp"
#include "tim.h"
#include "ds_debug_tools.hpp"
#include "ds_uart_h747.hpp"
#include "ds_can_h747.hpp"
#include "ds_led_h747.hpp"
#include "ds_shared_ram_h747.hpp"
#include "ds_telemetry_core.hpp"
#include "ds_sbus2.hpp"
#include "ds_serializer.hpp"
#include "ds_lw20.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
SCHEDULER task;
//End of Object Definitions



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
SCHEDULER::SCHEDULER() :
task_tick(0),
task_counter(0)
{ }



/**
  * @brief One time init task for peripherals
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::init( void )
{
	uart8.initialize();
	sbus.initialize();
	led.initialize();
	uart1.initialize();
	inter_core.initialize();
	serializer.initialize();
	uart2.initialize();
	lw20.initialize();
}



/**
  * @brief 		800Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task800Hz( void )
{
	inter_core.scheduler();
}



/**
  * @brief 		400Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task400Hz( void )
{

}



/**
  * @brief 		200Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task200Hz( void )
{

}



/**
  * @brief 		100Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task100Hz( void )
{
	led.scheduler();
	sbus.scheduler();
	serializer.scheduler();
}



/**
  * @brief 		50Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task50Hz( void )
{
	lw20.scheduler();
}



/**
  * @brief 		20Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task20Hz( void )
{
	telemetry_core.parseReceivedData();
}



/**
  * @brief 		10Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task10Hz( void )
{
	telemetry_core.sendPeriodicPacket();
}



/**
  * @brief 		5Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task5Hz( void )
{

}



/**
  * @brief 		2Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task2Hz( void )
{

}



/**
  * @brief 		1Hz loop function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::task1Hz( void )
{

}



/**
  * @brief 		Cpu main function
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::main( void )
{
	const uint64_t LOOP_PRESCALER_400HZ =   2;
	const uint64_t LOOP_PRESCALER_200HZ =   4;
	const uint64_t LOOP_PRESCALER_100HZ =   8;
	const uint64_t LOOP_PRESCALER_50HZ  =  16;
	const uint64_t LOOP_PRESCALER_20HZ  =  40;
	const uint64_t LOOP_PRESCALER_10HZ  =  80;
	const uint64_t LOOP_PRESCALER_5HZ   = 160;
	const uint64_t LOOP_PRESCALER_2HZ   = 400;
	const uint64_t LOOP_PRESCALER_1HZ   = 800;

	init();
	while(true)
	{
		if( task_tick == 1 ) // 800Hz
		{
			task_tick = 0;
			task_counter++;
			monitor.idle_time_us += monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::MAIN_TASK_IDLE_TIME);
			monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_ALL);

			monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_800_HZ);
			monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_800_HZ);
			task800Hz();
			monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_800_HZ);

			if( (task_counter % LOOP_PRESCALER_400HZ ) == 0 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_400_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_400_HZ);
				task400Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_400_HZ);
			}

			if( (task_counter % LOOP_PRESCALER_200HZ ) == 1 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_200_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_200_HZ);
				task200Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_200_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_100HZ ) == 3 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_100_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_100_HZ);
				task100Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_100_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_50HZ ) == 7 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_50_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_50_HZ);
				task50Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_50_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_20HZ ) == 11 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_20_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_20_HZ);
				task20Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_20_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_10HZ ) == 15 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_10_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_10_HZ);
				task10Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_10_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_5HZ ) == 19 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_5_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_5_HZ);
				task5Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_5_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_2HZ ) == 27 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_2_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_2_HZ);
				task2Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_2_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_1HZ ) == 31 )
			{
				monitor.getFrequency_Hz(Tools::time_measure_channel_map_type::LOOP_1_HZ);
				monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::LOOP_1_HZ);
				task1Hz();
				monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_1_HZ);
			}
			
			if( (task_counter % LOOP_PRESCALER_1HZ ) == 799 )
			{ 
				monitor.measureCpuLoad();
			}
			
			monitor.getElapsedTime_us(Tools::time_measure_channel_map_type::LOOP_ALL);
			monitor.startElapsedTimeMeasure(Tools::time_measure_channel_map_type::MAIN_TASK_IDLE_TIME);
		}
	}
}



/**
  * @brief 		Time tick for tasks
  *
  * @param[in]  void
  *
  * @return 	void
  */
void SCHEDULER::setTick( void )
{
	task_tick = 1;
}



/**
  * @brief 		C++ to C wrapper function for STM32
  *
  * @param[in]  void
  *
  * @return 	void
  */
void mainLoopWrapper(void)
{
	task.main();
}



#ifdef __cplusplus
extern "C"
{
#endif



/**
  * @brief 		Timer Interrupt Function
  *
  * @param[in]  TIM_HandleTypeDef *htim : Timer Instance
  *
  * @return 	void
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM13)
	{
		sbus.timerInterruptHandler();
	}
	if(htim->Instance == TIM6)	//800Hz Timer Interrupt
	{
		task.setTick(); //800Hz tick
	}
}



#ifdef __cplusplus
}
#endif



/**
  * @brief 		Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
SCHEDULER::SCHEDULER(const SCHEDULER& orig) 
{
    
}



/**
  * @brief 		Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
SCHEDULER::~SCHEDULER() 
{
    
}


