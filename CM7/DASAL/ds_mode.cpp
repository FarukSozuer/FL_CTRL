/**
 ******************************************************************************
 * @file		: ds_mode.cpp
 * @brief		: Mode Interface Class
 *				  This file contains mode interface class
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

#include <ds_mode.hpp>
#include "ds_vehicle.hpp"
#include "ds_parameters.hpp"
#include <math.h>

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE::MODE()
{
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE::~MODE()
{
}

/**
  * @brief Getting number of mode
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE::number_t MODE::getModeNumber() const
{
	return mode_number;
}

/**
  * @brief Getting reason of mode change
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE::reason_t MODE::getModeReason() const
{
	return mode_reason;
}

/**
  * @brief Setting reason of mode change
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MODE::setModeReason(reason_t modeReason)
{
	mode_reason = modeReason;
}

/**
  * @brief Check mode completed
  *
  * @param[in]  void
  *
  * @return 	void
  */
bool MODE::isModeCompleted() const
{
	return mode_completed;
}

/**
  * @brief Set coordinate as target position
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MODE::gotoCoordinate(int32_t requested_latitude,
		int32_t requested_longitude)
{
	if ((requested_latitude == parameters.gnss_latitude)
			&& (requested_longitude == parameters.gnss_longitude))
	{
		coordinate_target_reached = true;
	}
	else
	{
		parameters.latitude_reference = requested_latitude;
		parameters.longitude_reference = requested_longitude;
		coordinate_target_reached = false;
	}
}

/**
  * @brief Yaw command event handler
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MODE::yawCommandEvent(void)
{
	if (parameters.gcs_command.bits.yaw_cw)
	{
		parameters.heading_reference += parameters.YAW_CHANGE_RATE_DEG;
	}
	else if (parameters.gcs_command.bits.yaw_ccw)
	{
		parameters.heading_reference -= parameters.YAW_CHANGE_RATE_DEG;
	}

	if (parameters.heading_reference > 180)
	{
		parameters.heading_reference -= 360;
	}
	else if (parameters.heading_reference < -180)
	{
		parameters.heading_reference += 360;
	}
}

/**
  * @brief Altitude command event handler
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MODE::altitudeCommandEvent(void)
{
	if (parameters.gcs_command.bits.increase_altitude)
	{
		parameters.altitude_reference += parameters.ALTITUDE_CHANGE_RATE_M;
	}
	else if (parameters.gcs_command.bits.decrease_altitude)
	{
		parameters.altitude_reference -= parameters.ALTITUDE_CHANGE_RATE_M;

		if (parameters.altitude_reference < parameters.min_altitude_reference)
		{
			parameters.altitude_reference = parameters.min_altitude_reference;
		}
	}
}

/**
  * @brief Checking heading reference
  *
  * @param[in]  void
  *
  * @return 	void
  */
bool MODE::checkHeadingReference(void)
{
	float temp_yaw_difference = 0.0;
	bool result = false;

	temp_yaw_difference = fabs(parameters.heading_reference - parameters.heading);
	if (temp_yaw_difference > 180.0)
	{
		temp_yaw_difference -= 360.0;
	}

	if (fabs(temp_yaw_difference) < parameters.MAX_HEADING_DIFF_DEG)
	{
		result = true;
	}

	return result;
}

/**
  * @brief Landing event
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MODE::landEvent(void)
{
	static bool land_wait_started = false;
	static uint8_t land_counter = 0;

	if (slow_down_active == false)
	{
		parameters.vertical_speed_reference = -3.0;
		land_wait_started = false;
		land_counter = 0;
	}
	else
	{
		if (land_wait_started == false)
		{
			parameters.vertical_speed_reference = 0.0;
			land_counter++;
			if (land_counter >= parameters.SLOW_DOWN_WAIT_COUNTER)
			{
				land_wait_started = true;
			}
		}
		else
		{
			parameters.vertical_speed_reference = -0.6;
		}
	}
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
DISARM::DISARM()
{
	mode_number = number_t::DISARM;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
DISARM::~DISARM()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void DISARM::init()
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
void DISARM::run()
{
	if (!mode_started)
	{
		parameters.arm_status = false;
		mode_started = true;
	}
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void DISARM::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
MANUAL::MANUAL()
{
	mode_number = number_t::MANUAL;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
MANUAL::~MANUAL()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MANUAL::init()
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
void MANUAL::run()
{
	//empty
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void MANUAL::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
READY_TO_TAKE_OFF::READY_TO_TAKE_OFF()
{
	mode_number = number_t::READY_TO_TAKE_OFF;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
READY_TO_TAKE_OFF::~READY_TO_TAKE_OFF()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void READY_TO_TAKE_OFF::init()
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
void READY_TO_TAKE_OFF::run()
{
	parameters.arm_status = true;
	parameters.home_latitude = parameters.gnss_latitude;
	parameters.home_longitude = parameters.gnss_longitude;
	parameters.home_heading = parameters.heading;
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void READY_TO_TAKE_OFF::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
USER_INPUT::USER_INPUT()
{
	mode_number = number_t::USER_INPUT;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
USER_INPUT::~USER_INPUT()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void USER_INPUT::init()
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
void USER_INPUT::run()
{
	altitudeCommandEvent();

	if (fabs(parameters.altitude_reference - parameters.altitude)
			< parameters.ALTITUDE_RADIUS_M)
	{
		mode_completed = true;
	}
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void USER_INPUT::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
BRAKE::BRAKE()
{
	mode_number = number_t::BRAKE;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
BRAKE::~BRAKE()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void BRAKE::init()
{
	mode_completed = false;
	mode_started = false;
}

/**
  * @brief Run
  *
  * @param[in]  void
  *
  * @return 	void
  */
void BRAKE::run()
{
	parameters.speed_N_reference = 0.0;
	parameters.speed_E_reference = 0.0;
	parameters.vertical_speed_reference = 0.0;

	if ((fabs(parameters.speed_N) < parameters.SPEED_RADIUS_M)
			&& (fabs(parameters.speed_E) < parameters.SPEED_RADIUS_M)
			&& (fabs(parameters.vertical_speed) < parameters.SPEED_RADIUS_M))
	{
		mode_completed = true;
		parameters.latitude_reference = parameters.gnss_latitude;
		parameters.longitude_reference = parameters.gnss_longitude;
		parameters.altitude_reference = parameters.altitude;
	}
	else
	{
		mode_completed = false;
	}
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void BRAKE::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PRECISION::PRECISION()
{
	mode_number = number_t::PRECISION;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PRECISION::~PRECISION()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void PRECISION::init()
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
void PRECISION::run()
{
	//empty
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void PRECISION::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
RC_GPS::RC_GPS()
{
	mode_number = number_t::RC_GPS;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
RC_GPS::~RC_GPS()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RC_GPS::init()
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
void RC_GPS::run()
{
	//empty
}

/**
  * @brief Exit
  *
  * @param[in]  void
  *
  * @return 	void
  */
void RC_GPS::exitMode()
{
	//empty
}

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
ESCAPE::ESCAPE()
{
	mode_number = number_t::ESCAPE;
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
ESCAPE::~ESCAPE()
{
}

/**
  * @brief Init
  *
  * @param[in]  void
  *
  * @return 	void
  */
void ESCAPE::init()
{
	mode_completed = false;
	mode_started = false;
}

/**
  * @brief Run
  *
  * @param[in]  void
  *
  * @return 	void
  */
void ESCAPE::run()
{
	if (parameters.gcs_command.bits.escape)
	{
		parameters.altitude_reference = parameters.altitude + ESCAPE_ALTITUDE_M;
		mode_started = true;
	}
	else
	{
		if ((parameters.altitude_reference - parameters.altitude)
				< parameters.ALTITUDE_RADIUS_M)
		{
			mode_completed = true;
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
void ESCAPE::exitMode()
{
	//empty
}
