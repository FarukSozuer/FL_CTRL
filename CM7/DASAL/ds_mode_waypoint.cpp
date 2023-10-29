/**
 ******************************************************************************
 * @file		: ds_mode_waypoint.cpp
 * @brief		: Waypoint Mode Class
 *				  This file contains waypoint mode class
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
WAYPOINT::WAYPOINT()
{
	mode_number = number_t::WAYPOINT;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
WAYPOINT::~WAYPOINT()
{

}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::init(void)
{
	//empty
}

/**
  * @brief Run
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::run(void)
{
	if (waypoint_number > 0)
	{
		switch (state_waypoint)
		{
			case waypoint_state_t::INCREASE_ALTITUDE:
				increaseAltitudeEvent();
				break;

			case waypoint_state_t::WAIT_FOR_HEADING_CHANGE:
				waitForHeadingEvent();
				break;

			case waypoint_state_t::APPLY_COORDINATE:
				applyCoordinateEvent();
				break;

			case waypoint_state_t::DECREASE_ALTITUDE:
				decreaseAltitudeEvent();
				break;

			case waypoint_state_t::CHANGE_WAYPOINT:
				changeWaypointEvent();
				break;

			default:
				state_waypoint = waypoint_state_t::INCREASE_ALTITUDE;
				break;

		}
	}
}

/**
  * @brief Increase altitude
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::increaseAltitudeEvent(void)
{
	float temp_lat_dist = 0.0;
	float temp_lon_dist = 0.0;
	float temp_distance = 0.0;
	float temp_heading = 0.0;

	if (parameters.altitude < (waypoint_data[waypoint_index].altitude_m - parameters.ALTITUDE_RADIUS_M))
	{
		parameters.altitude_reference = waypoint_data[waypoint_index].altitude_m;
	}
	else
	{
		temp_lat_dist = (waypoint_data[waypoint_index].latitude - parameters.gnss_latitude) * 1.113195 * 0.01;
		temp_lon_dist = (waypoint_data[waypoint_index].longitude - parameters.gnss_longitude) * parameters.GPS_scaleLonDown * 1.113195 * 0.01;
		temp_distance = sqrtf((temp_lat_dist * temp_lat_dist) + (temp_lon_dist * temp_lon_dist));
		temp_heading = atan2f(temp_lon_dist, temp_lat_dist) * (180.0 / C_PI);

		if (fabs(parameters.altitude_reference - parameters.altitude) > MAX_ALTITUDE_DISTANCE)
		{
			parameters.altitude_reference = parameters.altitude;
		}

		if (temp_distance > MAX_HORIZONTAL_DISTANCE)
		{
			parameters.heading_reference = temp_heading;
			state_waypoint = waypoint_state_t::WAIT_FOR_HEADING_CHANGE;
		}
		else
		{
			state_waypoint = waypoint_state_t::APPLY_COORDINATE;
		}
	}
}

/**
  * @brief Waiting for heading change
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::waitForHeadingEvent(void)
{
	if(checkHeadingReference())
	{
		state_waypoint = waypoint_state_t::APPLY_COORDINATE;
	}
}

/**
  * @brief Apply coordinate as target position
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::applyCoordinateEvent(void)
{
	gotoCoordinate(waypoint_data[waypoint_index].latitude, waypoint_data[waypoint_index].longitude);

	if (coordinate_target_reached)
	{
		state_waypoint = waypoint_state_t::DECREASE_ALTITUDE;
	}

	altitudeCommandEvent();
}

/**
  * @brief Decrease altitude
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::decreaseAltitudeEvent(void)
{
	if (parameters.altitude > (waypoint_data[waypoint_index].altitude_m + parameters.ALTITUDE_RADIUS_M))
	{
		parameters.altitude_reference = waypoint_data[waypoint_index].altitude_m;
	}
	else
	{
		state_waypoint = waypoint_state_t::CHANGE_WAYPOINT;
	}
}

/**
  * @brief Changing waypoint
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::changeWaypointEvent(void)
{
	if ((parameters.gcs_command.bits.waypoint_continue_auto) || (parameters.gcs_command.bits.continue_t))
	{
		waypoint_index++;
		if (waypoint_index >= waypoint_number)
		{
			waypoint_index = 0;
		}
		state_waypoint = waypoint_state_t::INCREASE_ALTITUDE;
	}

	altitudeCommandEvent();

	if (fabs(parameters.vertical_speed) < 1.0)
	{
		yawCommandEvent();
	}
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void WAYPOINT::exitMode()
{
	//empty
}
