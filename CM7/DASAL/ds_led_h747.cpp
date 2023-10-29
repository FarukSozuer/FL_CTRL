/**
 ******************************************************************************
  * @file			: ds_led_h747.cpp
  * @brief		: H747I-DISCO Board LED Library
  *				  		This file contains led class
  * @author		: Onur Yildirim
  * @date			: 18.02.2020
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
#include "ds_led_h747.hpp"
#include "gpio.h"
// End of Includes



/*
 * Begin of Object Definitions
 */
Peripherals::Led::H747_LED led;
// End of Object Definitions



namespace Peripherals
{

namespace Led
{


/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_LED::H747_LED()
{ }



/**
  * @brief 			Set initial state of LEDs
  *
  * @param[in]  void	Nothing
  *
  * @return 		void	Nothing
  */
void H747_LED::initialize(void)
{
	changeState(0, led_control_type::OFF);
	changeState(1, led_control_type::OFF);
	changeState(2, led_control_type::OFF);
	changeState(3, led_control_type::OFF);
}



/**
  * @brief 			Led task scheduler
  *
  * @param[in]  void	Nothing
  *
  * @return 		void	Nothing
  */
void H747_LED::scheduler(void)
{
	static int32_t tick = 0;

	tick++;
	if(tick == 0)
	{
		changeState(0, led_control_type::ON);
		changeState(1, led_control_type::OFF);
		changeState(2, led_control_type::OFF);
		changeState(3, led_control_type::OFF);
	}
	else if(tick == 50)
	{
		changeState(0, led_control_type::OFF);
		changeState(1, led_control_type::ON);
		changeState(2, led_control_type::OFF);
		changeState(3, led_control_type::OFF);
	}
	else if(tick == 57)
	{
		changeState(0, led_control_type::OFF);
		changeState(1, led_control_type::OFF);
		changeState(2, led_control_type::ON);
		changeState(3, led_control_type::OFF);
	}
	else if(tick == 64)
	{
		changeState(0, led_control_type::OFF);
		changeState(1, led_control_type::OFF);
		changeState(2, led_control_type::OFF);
		changeState(3, led_control_type::ON);
	}
	else if(tick == 86)
	{
		changeState(0, led_control_type::OFF);
		changeState(1, led_control_type::OFF);
		changeState(2, led_control_type::ON);
		changeState(3, led_control_type::OFF);
	}
	else if(tick == 93)
	{
		changeState(0, led_control_type::OFF);
		changeState(1, led_control_type::ON);
		changeState(2, led_control_type::OFF);
		changeState(3, led_control_type::OFF);
	}
	else if(tick == 99)
	{
		tick = -1;
	}
}



/**
  * @brief 			Change the LED state
  *
  * @param[in]  void	Nothing
  *
  * @return 		void	Nothing
  */
void H747_LED::changeState(uint8_t led_number, led_control_type state)
{
	switch(led_number)
	{
		case 0:
			if(state == led_control_type::OFF)
			{
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_SET);
			}
			else if(state == led_control_type::ON)
			{
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_RESET);
			}
			else if(state == led_control_type::TOGGLE)
			{
				HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_12);
			}
			break;


		case 1:
			if(state == led_control_type::OFF)
			{
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_13, GPIO_PIN_SET);
			}
			else if(state == led_control_type::ON)
			{
				HAL_GPIO_WritePin(GPIOI, GPIO_PIN_13, GPIO_PIN_RESET);
			}
			else if(state == led_control_type::TOGGLE)
			{
				HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_13);
			}
			break;


		case 2:
			if(state == led_control_type::OFF)
			{
				//HAL_GPIO_WritePin(GPIOI, GPIO_PIN_14, GPIO_PIN_SET);
			}
			else if(state == led_control_type::ON)
			{
				//HAL_GPIO_WritePin(GPIOI, GPIO_PIN_14, GPIO_PIN_RESET);
			}
			else if(state == led_control_type::TOGGLE)
			{
				//HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_14);
			}
			break;


		case 3:
			if(state == led_control_type::OFF)
			{
				//HAL_GPIO_WritePin(GPIOI, GPIO_PIN_15, GPIO_PIN_SET);
			}
			else if(state == led_control_type::ON)
			{
				//HAL_GPIO_WritePin(GPIOI, GPIO_PIN_15, GPIO_PIN_RESET);
			}
			else if(state == led_control_type::TOGGLE)
			{
				//HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_15);
			}
			break;


		default:
			//
			break;
	}
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_LED::H747_LED(const H747_LED& orig)
{

}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_LED::~H747_LED()
{
    
}



} //End of namespace Led

} //End of namespace Peripherals

