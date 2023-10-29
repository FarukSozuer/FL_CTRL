/**
 ******************************************************************************
  * @file		: ds_main.hpp
  * @brief		: Task scheduler header file
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

#ifndef DS_MAIN_HPP
#define	DS_MAIN_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
// End Of Includes



/*
 * Begin of Macro Definitions
 */

//End of Macro Definitions


#ifdef __cplusplus

/*
 * SCHEDULER Class Definition
 */
class SCHEDULER
{
    public:
			SCHEDULER();
			
			void init		( void );
			void task1Hz	( void );
			void task2Hz	( void );
			void task5Hz	( void );
			void task10Hz	( void );
			void task20Hz	( void );
			void task50Hz	( void );
			void task100Hz	( void );
			void task200Hz	( void );
			void task400Hz	( void );
			void task800Hz	( void );
			void main		( void );
			void setTick	( void );
			
			SCHEDULER(const SCHEDULER& orig);
			virtual ~SCHEDULER();
			
    private:
			uint8_t 	task_tick;
			uint64_t 	task_counter;
		
};
// End of SCHEDULER Class Definition



extern "C"
{
#endif

void mainLoopWrapper(void);

#ifdef __cplusplus
}
#endif

#endif	/* DS_MAIN_HPP */
