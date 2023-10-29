/**
 ******************************************************************************
 * @file		: ds_parameters.hpp
 * @brief		: Parameters Class
 *				  This file contains parameters class
 * @author		: Gurtac Kadem
 * @date		: 03.04.2020
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

#ifndef DS_PARAMETERS_HPP
#define DS_PARAMETERS_HPP

#include "stdint.h"

#define C_PI    3.141592653589793238463L

typedef union
{
		struct
		{
				uint16_t arm :1; //1
				uint16_t disarm :1; //2
				uint16_t increase_altitude :1; //3
				uint16_t decrease_altitude :1; //4
				uint16_t yaw_cw :1; //5
				uint16_t yaw_ccw :1; //6
				uint16_t escape :1; //7
				uint16_t cancel :1; //8
				uint16_t waypoint_continue_auto :1; //9
				uint16_t continue_t :1; //10
				uint16_t take_off :1; //11
				uint16_t return_to_land :1; //12
				uint16_t land :1; //13
				uint16_t waypoint :1; //14
				uint16_t brake :1; //15
				uint16_t unused_16 :1; //16
		} bits;
		uint16_t u16;
} gcs_command_t;

class PARAMETERS
{
	public:
		PARAMETERS();
		virtual ~PARAMETERS();

		bool arm_status = false;
		gcs_command_t gcs_command;

		float altitude = 0.0;
		float altitude_reference = 0.0;
		float rtl_altitude = 0.0;

		float heading = 0.0;
		float heading_reference = 0.0;
		float home_heading = 0.0;

		int32_t gnss_latitude = 0;
		int32_t gnss_longitude = 0;
		int32_t latitude_reference = 0;
		int32_t longitude_reference = 0;
		int32_t home_latitude = 0;
		int32_t home_longitude = 0;
		float GPS_scaleLonDown = 0.0;

		float speed_N = 0.0;
		float speed_E = 0.0;
		float speed_N_reference = 0.0;
		float speed_E_reference = 0.0;
		float vertical_speed = 0.0;
		float vertical_speed_reference = 0;

		const float ALTITUDE_CHANGE_RATE_M = 5.0;
		const float YAW_CHANGE_RATE_DEG = 10.0;
		const float TAKE_OFF_ALTITUDE_M = 20.0;
		const float ALTITUDE_RADIUS_M = 1.0;
		const float SPEED_RADIUS_M = 1.0;
		const float COORDINATE_RADIUS_M = 3.0;
		const float SLOW_DOWN_LIMIT = 20.0;
		const float MIN_ALTITUDE_REFERENCE_M = -500.0;
		const float MAX_ALTITDE_REFERENCE_M = 1000.0;
		const float MAX_HEADING_DIFF_DEG = 4.0;
		const uint8_t SLOW_DOWN_WAIT_COUNTER = 50;

		float min_altitude_reference = MIN_ALTITUDE_REFERENCE_M;
};

extern PARAMETERS parameters;

#endif /* DS_PARAMETERS_HPP */
