/**
 ******************************************************************************
 * @file		: ds_mode_emergency_land.cpp
 * @brief		: Emergency Land Mode Class
 *				  This file contains emergency land class
 * @author		: Gurtac Kadem
 * @date		: 01.04.2020
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

#include "ds_mode.hpp"
#include "ds_parameters.hpp"

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
EMERGENCY_LAND::EMERGENCY_LAND()
{
	mode_number = number_t::EMERGENCY_LAND;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
EMERGENCY_LAND::~EMERGENCY_LAND()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void EMERGENCY_LAND::init()
{
	mode_started = false;
}

/**
  * @brief Run
  *
  * @param[in]  void
  *
  * @return 	void
  */
void EMERGENCY_LAND::run()
{
	landEvent();
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void EMERGENCY_LAND::exitMode()
{
	//empty
}

