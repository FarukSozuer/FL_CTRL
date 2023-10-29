/**
 ******************************************************************************
  * @file			: ds_lw20.hpp
  * @brief		: Lightware driver class
  * @author		: Emre TUNCAY
  * @date			: 19.01.2021
  * @version	: 0.0.1
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 DASAL, All Rights Reserved </center></h2>
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

#ifndef DS_LW20_HPP_
#define DS_LW20_HPP_

#include <stdio.h>
#include <iostream>
#include "stdint.h"
#include "string.h"
#include "ds_uart_h747.hpp"
#include "ds_filter_database.hpp"
#include "ds_debug_tools.hpp"

using namespace std;

namespace Sensors
{

namespace Lightware
{

const char		product_name[4]		   						= {'S','F','2','0'};
const float	  natural_vehicle_ground_distance	= 0.1;
const uint8_t lower_case_offset 	   					= 32;
const uint8_t max_character_size 	   					= 32;
const uint8_t max_float_ascii_size   					= 10;
const	uint8_t	max_parameter_character_size		= 6;
const	float 	max_communication_timeout_delay	= 1;

typedef struct
{
	char param_case[max_parameter_character_size];
	uint8_t param_size;
}parameter_t;

const struct
{
	parameter_t product_name														= {"PN",2 };
	parameter_t hardware_version												= {"PF",2};
	parameter_t firmware_version												= {"PS",2};
	parameter_t default_raw_distance										= {"L",1};
	parameter_t default_median_distance									= {"LD",2};
	parameter_t median_distance_first_return						= {"LDF,0",5};
	parameter_t raw_distance_first_return								= {"LDF,1",5};
	parameter_t closest_distance_first_return						= {"LDF,2",5};
	parameter_t furthest_distance_first_return					= {"LDF,3",5};
	parameter_t median_distance_last_return							= {"LDL,0",5};
	parameter_t raw_distance_last_return								= {"LDL,1",5};
	parameter_t closest_distance_last_return						= {"LDL,2",5};
	parameter_t furthest_distance_last_return						= {"LDL,3",5};
	parameter_t signal_strength_of_first_default_signal	= {"LH",2};
	parameter_t signal_strength_of_first_return					= {"LHF",3};
	parameter_t signal_strength_of_last_return					= {"LHL",3};
	parameter_t distance_measurement_datum_offset				= {"LO",2};
	parameter_t first_alarm_distance										= {"LAA",3};
	parameter_t second_alarm_distance										= {"LAB",3};
	parameter_t hysteresis_value												= {"LAH",3};
	parameter_t update_rate															= {"LM",2};
	parameter_t laser_state															= {"LF",2};
	parameter_t internal_temperature										= {"LT",2};
	parameter_t background_noise_level									= {"LN",2};
	parameter_t lost_signal_confirmation_number					= {"LC",2};
	parameter_t serial_baud_rate												= {"CB",2};
	parameter_t i2c_address															= {"CI",2};
}parameter_table;

enum class lw20_status_t :uint8_t
{
	LW20_OK 	 		= 0,
	LW20_TIMEOUT	= 1,
	LW20_ERROR		= 2,
};

enum class lw20_initialize_state_t :uint8_t
{
	INIT_UPDATE_RATE = 0,
	INIT_BAUD_RATE 	= 1,
};

enum class lw20_scheduler_state_t :uint8_t
{
	INITIALIZE		= 0,
	CHECK_DEVICE 	= 1,
	SET_STREAM		= 2,
	MEASURE 			= 3,
};

enum class lw20_response_state_t :uint8_t
{
	PARAMETER_CASE	= 0,
	COLON						= 1,
	NUMBER_VALUE		= 2,
	CARRIAGE_RETURN = 3,
	LINE_FEED				= 4,
};

enum class distance_stream_state_t :uint8_t
{
	STREAM_SIGN			= 0,
	PARAMETER_CASE	= 1,
	COLON						= 2,
	VALUE						= 3,
	CARRIAGE_RETURN	= 4,
	LINE_FEED				= 5,
};

enum class look_product_name_state_t :uint8_t
{
	PARAMETER				= 0,
	COLON						= 1,
	PRODUCT_NAME		= 2,
	CARRIAGE_RETURN	= 3,
	LINE_FEED				= 4,
};

enum class command_t :char
{
	READ		= '?',
	WRITE		= '#',
	STREAM	= '$',
	SAVE		= '%',
};

enum class channel_t :uint8_t
{
	PRODUCT       = (uint8_t)'P',
	LASER         = (uint8_t)'L',
	SERVO		  		= (uint8_t)'S',
	ALARM		  		= (uint8_t)'A',
	COMMUNICATION = (uint8_t)'C',
};

enum class update_rate_t :uint8_t
{
	update_rate_388ps = 1,
	update_rate_194ps = 2,
	update_rate_129ps	= 3,
	update_rate_97ps	= 4,
	update_rate_78ps	= 5,
	update_rate_65ps 	= 6,
	update_rate_55ps	= 7,
	update_rate_48ps	= 8,
};

enum class baudrate_t :uint8_t
{
	baudrate_9600 	= 0,
	baudrate_19200 	= 1,
	baudrate_38400	= 2,
	baudrate_57600	= 3,
	baudrate_115200	= 4,
	baudrate_230400 = 5,
	baudrate_460800	= 6,
	baudrate_921600	= 7,
};

typedef struct
{
	update_rate_t 		 					 update_rate;
	Peripherals::Uart::H747_UART *uart_source;
	baudrate_t									 baudrate;
}init_def_t;


typedef union
{
	struct
	{
		uint8_t sensor_health 				: 1,
						sensor_initiated			: 1,
						read_distance_err			: 1,
						lens_occlusion_err  	: 1,
						product_name_err			: 1,
						minimum_distance_err	: 1,
						communication_timeout	: 1,
						reserved							: 1;
	}bits;
	uint8_t all;
}lw20_health_t;

class LIGHTWARE_LW20
{
public:

	explicit LIGHTWARE_LW20(Peripherals::Uart::H747_UART *uart_source, update_rate_t update_rate, baudrate_t baudrate);

	virtual ~LIGHTWARE_LW20();

	init_def_t init_def;

	lw20_health_t health 						= {.all = 0};

	float distance 									= 0;

	float estimated_vertical_speed 	= 0;

	uint32_t error_count 						= 0;

	bool	new_stream_detected = false;

	lw20_status_t initialize();

	lw20_status_t readParameter(parameter_t *parameter, float &data);

	lw20_status_t writeParameter(parameter_t *parameter, float value);

	lw20_status_t saveChannel(channel_t channel);

	lw20_status_t	changeBaudRate(baudrate_t baudrate);

	lw20_status_t setStream(parameter_t *parameter, uint8_t stream_num);

	lw20_status_t checkProductName();

	void 					checkSensorHealth(lw20_status_t read_distance_status);

	bool					verticalSpeedEstimation(float dt);

	bool					verticalSpeedEstimation();

	void					scheduler();

protected:

private:

	FILTER_DATABASE vertical_speed_estimation_low_pass_filter;

	float old_distance = 0;

	lw20_status_t readDistanceStream();

	lw20_status_t response(parameter_t &parameter, float &response_data);

	void setDistanceStream();

};

};

};

extern Sensors::Lightware::LIGHTWARE_LW20 lw20;

#endif /* DS_LW20_HPP */
