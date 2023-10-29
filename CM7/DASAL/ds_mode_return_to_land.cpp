/**
 ******************************************************************************
 * @file		: ds_mode_return_to_land.cpp
 * @brief		: Return To Land Mode Class
 *				  This file contains return to land class
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
#include <math.h>

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
RETURN_TO_LAND::RETURN_TO_LAND()
{
	mode_number = number_t::RETURN_TO_LAND;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
RETURN_TO_LAND::~RETURN_TO_LAND()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::init()
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
void RETURN_TO_LAND::run()
{
	float distance = 0.0;

	if (!mode_started)
	{
		lat_dist = (parameters.home_latitude - parameters.gnss_latitude) * 1.113195
				* 0.01;
		lon_dist = (parameters.home_longitude - parameters.gnss_longitude)
				* parameters.GPS_scaleLonDown * 1.113195 * 0.01;
		distance = sqrtf((lat_dist * lat_dist) + (lon_dist * lon_dist));

		if (distance < RTL_RADIUS_M)
		{
			rtl_state = return_to_land_state_t::APPLY_COORDINATE;
		}
		else
		{
			rtl_state = return_to_land_state_t::CLIMB;
			parameters.min_altitude_reference = parameters.rtl_altitude;
		}
		mode_started = true;
	}

	switch (rtl_state)
	{
		case return_to_land_state_t::CLIMB:
			climbEvent();
			break;

		case return_to_land_state_t::WAIT_FOR_HEADING_CHANGE:
			waitForHeadingEvent();
			break;

		case return_to_land_state_t::APPLY_COORDINATE:
			applyCoordinateEvent();
			break;

		case return_to_land_state_t::WAIT_FOR_HOME_HEADING_CHANGE:
			waitForHomeHeadingChangeEvent();
			break;

		case return_to_land_state_t::LAND:
			landEvent();
			break;

		default:
			rtl_state = return_to_land_state_t::CLIMB;
			break;

	}
}

/**
  * @brief Climb event handler
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::climbEvent(void)
{
	if (parameters.altitude
			< (parameters.rtl_altitude - parameters.ALTITUDE_RADIUS_M))
	{
		parameters.altitude_reference = parameters.rtl_altitude;
	}
	else
	{
		parameters.altitude_reference = parameters.altitude;

		rtl_state = return_to_land_state_t::WAIT_FOR_HEADING_CHANGE;
	}
}

/**
  * @brief Waiting for heading change
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::waitForHeadingEvent(void)
{
	parameters.heading_reference = atan2f(lon_dist, lat_dist) * (180.0 / C_PI);

	if(checkHeadingReference())
	{
		rtl_state = return_to_land_state_t::APPLY_COORDINATE;
	}
}

/**
  * @brief Apply coordinate as target position
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::applyCoordinateEvent(void)
{
	gotoCoordinate(parameters.home_latitude, parameters.home_longitude);

	if (coordinate_target_reached)
	{
		rtl_state = return_to_land_state_t::WAIT_FOR_HOME_HEADING_CHANGE;
	}

	altitudeCommandEvent();
}

/**
  * @brief Wait for home heading change
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::waitForHomeHeadingChangeEvent(void)
{
	parameters.heading_reference = parameters.home_heading;

	if(checkHeadingReference())
	{
		rtl_state = return_to_land_state_t::LAND;
	}
}

/**
  * @brief Landing event handler
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::landEvent(void)
{
	if (parameters.altitude > parameters.TAKE_OFF_ALTITUDE_M)
	{
		if ((parameters.altitude - parameters.TAKE_OFF_ALTITUDE_M)
				< SPEED_REDUCTION_ALT_DIFF)
		{
			parameters.vertical_speed_reference = (parameters.altitude
					- parameters.TAKE_OFF_ALTITUDE_M) / (SPEED_REDUCTION_DIVIDE_CONSTANT)
					- SPEED_REDUCTION_CONSTANT;
		}
		else
		{
			parameters.vertical_speed_reference = -3.0;
		}
	}
	else
	{
		parameters.vertical_speed_reference = -0.6;
	}
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RETURN_TO_LAND::exitMode()
{
	parameters.min_altitude_reference = parameters.MIN_ALTITUDE_REFERENCE_M;
}
