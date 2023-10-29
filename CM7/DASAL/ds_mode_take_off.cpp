/**
 ******************************************************************************
  * @file		: ds_mode_take_off.cpp
  * @brief		: Take Off Mode Class
  *				  This file contains take off mode class
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
TAKE_OFF::TAKE_OFF()
{
	mode_number = number_t::TAKE_OFF;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
TAKE_OFF::~TAKE_OFF()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void TAKE_OFF::init()
{
	mode_completed = false;
}

/**
  * @brief Run
  *
  * @param[in]  void
  *
  * @return 	void
  */
void TAKE_OFF::run()
{
	if( (parameters.TAKE_OFF_ALTITUDE_M - parameters.altitude) < parameters.ALTITUDE_RADIUS_M )
	{
		mode_completed = true;
	}
	else
	{
		if(!mode_started)
		{
			parameters.altitude_reference = parameters.TAKE_OFF_ALTITUDE_M;
			mode_completed = false;
			mode_started = true;
		}
	}
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void TAKE_OFF::exitMode()
{
	//empty
}

