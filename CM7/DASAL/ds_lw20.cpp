/**
 ******************************************************************************
 * @file		: ds_lw20.cpp
 * @brief		: Lightware lw20 Lidar driver class
 * @author	: Emre TUNCAY
 * @date		: 19.01.2021
 * @version	: 0.1.0
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

#include "ds_lw20.hpp"

Sensors::Lightware::LIGHTWARE_LW20 lw20(&uart2, Sensors::Lightware::update_rate_t::update_rate_55ps, Sensors::Lightware::baudrate_t::baudrate_115200);

namespace Sensors
{

namespace Lightware
{

	/**
	* @brief	Default constructor
	*
	* @param	void
	*
	* @return	void
	*/
LIGHTWARE_LW20::LIGHTWARE_LW20(Peripherals::Uart::H747_UART *uart_source, update_rate_t update_rate, baudrate_t baudrate)
{
	init_def.uart_source 				= uart_source;
	init_def.update_rate				= update_rate;
	init_def.baudrate						= baudrate;
}

/**
* @brief	Default deconstructor
*
* @param   	void
*
* @return 	void
*/
LIGHTWARE_LW20::~LIGHTWARE_LW20()
{

}

/**
* @brief	Initilation lightware lw20 lidar sensor
*
* @param	void
*
* @return	LW20_OK  			-> OK
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::initialize()
{

	static lw20_initialize_state_t lw20_initialize_state = lw20_initialize_state_t::INIT_UPDATE_RATE;

	health.bits.sensor_initiated = 0;

	switch (lw20_initialize_state)
	{
		case lw20_initialize_state_t::INIT_UPDATE_RATE:
			if(writeParameter((parameter_t*)&parameter_table.update_rate, (float)init_def.update_rate) == lw20_status_t::LW20_OK)
			{
				lw20_initialize_state = lw20_initialize_state_t::INIT_BAUD_RATE;
			}
			else
			{
				return lw20_status_t::LW20_ERROR;
			}
		break;

		case lw20_initialize_state_t::INIT_BAUD_RATE:
			if(changeBaudRate(init_def.baudrate) == lw20_status_t::LW20_OK)
			{
				health.bits.sensor_initiated = 1;
				return lw20_status_t::LW20_OK;
			}
			else
			{
				return lw20_status_t::LW20_ERROR;
			}
		break;

		default:
		break;
	}

	return lw20_status_t::LW20_ERROR;
}

/**
* @brief	Read parameter from lightware lw20 lidar
*
* @param	*parameter 		-> Parameter address that will be read from parameter table.
* 				data					-> Read data from lidar sensor.
*
* @return	LW20_OK  			-> OK
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::readParameter(parameter_t *parameter, float &data)
{
	uint8_t write_size 									= 0;
	char write_buf[max_character_size] 	= {0};
	lw20_status_t response_status				= lw20_status_t::LW20_OK;
	lw20_status_t return_value					= lw20_status_t::LW20_OK;

	write_buf[write_size++] = (char)command_t::READ;
	memcpy((write_buf+write_size),parameter->param_case,parameter->param_size);
	write_size += parameter->param_size;
	write_buf[write_size++] = '\r';
	write_buf[write_size++] = '\n';

	init_def.uart_source->sendData((uint8_t*)write_buf, ++write_size);
	response_status = response(*parameter, data);

	if(response_status == lw20_status_t::LW20_OK)
	{
		return_value = lw20_status_t::LW20_OK;
	}
	else
	{
		return_value = response_status;
	}

	return return_value;
}

/**
* @brief	Read value to lightware lw20 parameter
*
* @param	*parameter 	-> address of the parameter that will be written
* 				value	   		-> value to be written
*
* @return	LW20_OK  			-> OK
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::writeParameter(parameter_t *parameter, float value)
{
	uint8_t write_size 									= 0;
	uint8_t value_size 									= 0;
	float response_data 								= 0;
	char write_buf[max_character_size] 	= {0};
	char value_buf[max_character_size] 	= {0};
	lw20_status_t response_status 			= lw20_status_t::LW20_OK;
	lw20_status_t return_value 					= lw20_status_t::LW20_OK;

	write_buf[write_size++] = (char)command_t::WRITE;
	memcpy((write_buf+write_size),parameter->param_case,parameter->param_size);
	write_size += parameter->param_size;
	write_buf[write_size++] = ',';

	sprintf(value_buf, "%.2f", value);
	value_size = snprintf(NULL, 0, "%.2f", value);
	if(value_buf[value_size-1] == '0' && value_buf[value_size - 2] == '0')
	{
		memcpy((write_buf+write_size),value_buf,value_size - 3);
		write_size += value_size - 3;
	}
	else
	{
		sprintf((write_buf+write_size), "%.2f", value);
		write_size += snprintf(NULL, 0, "%.2f", value);
	}

	write_buf[write_size++] = '\r';
	write_buf[write_size++] = '\n';

	init_def.uart_source->sendData((uint8_t*)write_buf, write_size);
	response_status = response(*parameter, response_data);

	if(response_status == lw20_status_t::LW20_OK && response_data == value)
	{
		return_value = lw20_status_t::LW20_OK;
	}
	else
	{
		return_value = response_status;
	}

	return return_value;
}

/**
* @brief	Save channel setting
*
* @param	channel 	-> Specific channel
*
* @return	LW20_OK 	-> OK
*			LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::saveChannel(channel_t channel)
{
	uint8_t write_size 								 = 0;
	char write_buf[max_character_size] = {0};

	write_buf[write_size++] = (char)command_t::SAVE;
	write_buf[write_size++] = (char)channel;
	write_buf[write_size++] = '\r';
	write_buf[write_size++] = '\n';

	init_def.uart_source->sendData((uint8_t*)write_buf, ++write_size);

	return lw20_status_t::LW20_OK;
}

/**
* @brief	Change lidar sensor baud rate value.
*
* @param	baudrate 			-> Baudrate value.
*
* @return	LW20_OK 			-> OK
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t	LIGHTWARE_LW20::changeBaudRate(baudrate_t baudrate)
{
	return writeParameter((parameter_t*)&(parameter_table.serial_baud_rate), (float)baudrate);
}

/**
* @brief	Set a streaming value.
*
* @param	parameter 		-> Parameter
* 				stream_num		-> Unique stream value. (1 to 5)
*
* @return	LW20_OK 			-> OK
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::setStream(parameter_t *parameter, uint8_t stream_num)
{
	uint8_t write_size 								 = 0;
	char write_buf[max_character_size] = {0};

	write_buf[write_size++] = (char)command_t::STREAM;
	sprintf((write_buf + write_size++), "%d", stream_num);
	write_buf[write_size++] = ',';
	memcpy((write_buf+write_size),parameter->param_case,parameter->param_size);
	write_size += parameter->param_size;
	write_buf[write_size++] = '\r';

	init_def.uart_source->sendData((uint8_t*)write_buf, ++write_size);

	return lw20_status_t::LW20_OK;
}

/**
* @brief	Checks that the Product Name is 'SF20'
*
* @param 	void
*
* @return	LW20_OK 	-> Product name is correct
*			LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::checkProductName()
{
	static look_product_name_state_t look_state	= look_product_name_state_t::PARAMETER;
	static uint8_t product_name_character_size	= sizeof(product_name);
	static uint8_t product_name_character_count	= 0;
	static uint8_t parameter_character_count 		= 0;

	static uint16_t cbuffer_index 		= 0;
	static uint16_t coming_data_size 	= 0;
	static uint8_t cbuffer[2048] 			= {0};

	coming_data_size = init_def.uart_source->getDataFromBuffer(cbuffer);

	for (cbuffer_index = 0; cbuffer_index < coming_data_size; cbuffer_index++)
	{
		switch (look_state)
		{

			case look_product_name_state_t::PARAMETER:
				if(cbuffer[cbuffer_index] == (uint8_t)(parameter_table.product_name.param_case[parameter_character_count])+lower_case_offset)
				{
					parameter_character_count++;
					if(product_name_character_count > product_name_character_size)
					{
						product_name_character_count = 0;
						parameter_character_count = 0;
						return lw20_status_t::LW20_ERROR;
					}
				}
				if(parameter_character_count == parameter_table.product_name.param_size)
				{
					look_state = look_product_name_state_t::COLON;
				}
				break;

			case look_product_name_state_t::COLON:
				if(cbuffer[cbuffer_index] == (char)':')
				{
					look_state = look_product_name_state_t::PRODUCT_NAME;
				}
				else
				{
					product_name_character_count = 0;
					parameter_character_count = 0;
					look_state = look_product_name_state_t::PRODUCT_NAME;
					return lw20_status_t::LW20_ERROR;
				}
				break;

			case look_product_name_state_t::PRODUCT_NAME:
				if(cbuffer[cbuffer_index] == (uint8_t)product_name[product_name_character_count])
				{
					product_name_character_count++;
					if(product_name_character_count > product_name_character_size)
					{
						product_name_character_count = 0;
						parameter_character_count = 0;
						look_state = look_product_name_state_t::PRODUCT_NAME;
						return lw20_status_t::LW20_ERROR;
					}
				}
				else if(cbuffer[cbuffer_index] == ' ')
				{
					look_state = look_product_name_state_t::CARRIAGE_RETURN;
				}
				else
				{
					product_name_character_count = 0;
					parameter_character_count = 0;
					look_state = look_product_name_state_t::PARAMETER;
					return lw20_status_t::LW20_ERROR;
				}
				break;

			case look_product_name_state_t::CARRIAGE_RETURN:
				if(cbuffer[cbuffer_index] == (char)'\r')
				{
					look_state = look_product_name_state_t::LINE_FEED;
				}
				else
				{
					product_name_character_count = 0;
					parameter_character_count = 0;
					look_state = look_product_name_state_t::PRODUCT_NAME;
					return lw20_status_t::LW20_ERROR;
				}
				break;

			case look_product_name_state_t::LINE_FEED:
				if(cbuffer[cbuffer_index] == (char)'\n')
				{
					return lw20_status_t::LW20_OK;
				}
				else
				{
					product_name_character_count = 0;
					parameter_character_count = 0;
					look_state = look_product_name_state_t::PRODUCT_NAME;
					return lw20_status_t::LW20_ERROR;
				}
				break;
		}
	}

	return lw20_status_t::LW20_ERROR;
}

/**
* @brief	Check Sensor Health
* 				!!!This function must be call after readDistanceStream function!!!
*
* @param 	void
*
* @return	void
*/
void LIGHTWARE_LW20::checkSensorHealth(lw20_status_t read_distance_status)
{
	if(read_distance_status == lw20_status_t::LW20_OK)
	{
		health.bits.read_distance_err = 0;
		health.bits.communication_timeout = 0;
	}
	else if(read_distance_status == lw20_status_t::LW20_TIMEOUT)
	{
		health.bits.read_distance_err = 1;
		health.bits.communication_timeout = 1;
	}
	else
	{
		health.bits.read_distance_err = 1;
		health.bits.communication_timeout = 0;
	}

	if(distance <= natural_vehicle_ground_distance)
	{
		health.bits.minimum_distance_err = 1;
	}
	else
	{
		health.bits.minimum_distance_err = 0;
	}

	if(distance == -1)
	{
		health.bits.lens_occlusion_err = 1;
	}
	else
	{
		health.bits.lens_occlusion_err = 0;
	}

	if(health.bits.minimum_distance_err == 0 && health.bits.product_name_err == 0 && health.bits.lens_occlusion_err == 0 && health.bits.read_distance_err == 0)
	{
		health.bits.sensor_health = 1;
	}
	else
	{
		health.bits.sensor_health = 0;
	}
}

/**
* @brief	Estimation of vertical speed, using distance value
*
* @param 	dt-> Sampling time(ms)
*
* @return	void
*/
bool LIGHTWARE_LW20::verticalSpeedEstimation(float dt)
{
	if(dt <= 0)
	{
		return false;
	}
	else
	{
		estimated_vertical_speed 	= (distance-old_distance)/(((float)dt)/1000);
		old_distance 							= distance;
		return true;
	}

}

/**
* @brief	Estimation of vertical speed, using distance value.
* 				Sampling time calculated with HAL_GetTick()
*
* @param 	void
*
* @return	void
*/
bool LIGHTWARE_LW20::verticalSpeedEstimation()
{
	float temp_estimated_vertical_speed = 0;
	float dt_s = 0;

  monitor.assignDtMeasureChannel(Tools::dt_measure_channel_map_type::CHANNEL2,0.02);
  monitor.updateDtMeasure(Tools::dt_measure_channel_map_type::CHANNEL2);
  dt_s = monitor.getDtMeasure_s(Tools::dt_measure_channel_map_type::CHANNEL2);

	if(dt_s <= 0)
	{
		return false;
	}
	else
	{
		temp_estimated_vertical_speed = (distance-old_distance)/dt_s;
		vertical_speed_estimation_low_pass_filter.applyLowPass(estimated_vertical_speed, temp_estimated_vertical_speed, (float)25.0, dt_s);
		old_distance = distance;
		return true;
	}
}

/**
* @brief	scheduler
*
* @param	void
*
* @return	void
*/
void LIGHTWARE_LW20::scheduler()
{
	static lw20_scheduler_state_t lw20_scheduler_state = lw20_scheduler_state_t::INITIALIZE;
	lw20_status_t read_distance_stream_return = lw20_status_t::LW20_OK;

	switch (lw20_scheduler_state)
	{
		case lw20_scheduler_state_t::INITIALIZE:
			if(initialize() == lw20_status_t::LW20_OK)
			{
				lw20_scheduler_state = lw20_scheduler_state_t::CHECK_DEVICE;
			}
		break;

		case lw20_scheduler_state_t::CHECK_DEVICE:
			init_def.uart_source->sendData((uint8_t*)"?PN\r\n", 5); /*Ask device name */
			if(checkProductName() == lw20_status_t::LW20_OK)
			{
				health.bits.product_name_err = 0;
				lw20_scheduler_state = lw20_scheduler_state_t::SET_STREAM;
			}
			else
			{
				health.bits.product_name_err = 1;
				lw20_scheduler_state = lw20_scheduler_state_t::CHECK_DEVICE;
			}
		break;

		case lw20_scheduler_state_t::SET_STREAM:
			setDistanceStream();
			lw20_scheduler_state = lw20_scheduler_state_t::MEASURE;
		break;

		case lw20_scheduler_state_t::MEASURE:
			read_distance_stream_return = readDistanceStream();
			if(read_distance_stream_return == lw20_status_t::LW20_OK)
			{
				checkSensorHealth(lw20_status_t::LW20_OK);
				if(new_stream_detected)
				{
					verticalSpeedEstimation();
				}
			}
			else
			{
				checkSensorHealth(read_distance_stream_return);
			}
			lw20_scheduler_state = lw20_scheduler_state_t::MEASURE;
		break;

		default:
		break;
	}

}

/**
* @brief	Read coming data from lightware lw20 lidar and decode.
*
* @param	parameter 		-> Responsed parameter address
* 				response_data -> Parameter value address that coming from lightware lw20 lidar.
*
* @return	LW20_OK 			-> OK
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::response(parameter_t &parameter, float &response_data)
{
	static uint16_t coming_data_size = 0;
	static uint8_t parameter_character_count = 0;
	static uint8_t number_value_count = 0;
	static char number_value_buf[10] = {0};

	static uint16_t cbuffer_index = 0;
	static uint8_t cbuffer[2048] = {0};
	static lw20_response_state_t response_state = lw20_response_state_t::PARAMETER_CASE;

	coming_data_size = init_def.uart_source->getDataFromBuffer(cbuffer,2048);
	for (cbuffer_index = 0; cbuffer_index < coming_data_size; cbuffer_index++)
	{
		switch (response_state)
		{

			case lw20_response_state_t::PARAMETER_CASE:
				if(cbuffer[cbuffer_index] == (uint8_t)(parameter.param_case[parameter_character_count]) + lower_case_offset ||
					 cbuffer[cbuffer_index] == (uint8_t)(parameter.param_case[parameter_character_count]))
				{
					parameter_character_count++;
					if(parameter_character_count > max_parameter_character_size)
					{
						number_value_count = 0;
						parameter_character_count = 0;
						response_state = lw20_response_state_t::PARAMETER_CASE;
						return lw20_status_t::LW20_ERROR;
					}
				}
				if(parameter_character_count == parameter.param_size)
				{
					response_state = lw20_response_state_t::COLON;
				}
				break;

			case lw20_response_state_t::COLON:
				if(cbuffer[cbuffer_index] == (char)':')
				{
					response_state = lw20_response_state_t::NUMBER_VALUE;
				}
				else
				{
					number_value_count = 0;
					parameter_character_count = 0;
					response_state = lw20_response_state_t::PARAMETER_CASE;
					return lw20_status_t::LW20_ERROR;
				}
				break;

			case lw20_response_state_t::NUMBER_VALUE:
				if(cbuffer[cbuffer_index] == (uint8_t)' ')
				{
					response_state = lw20_response_state_t::CARRIAGE_RETURN;
				}
				else
				{
					number_value_buf[number_value_count++] = (char)cbuffer[cbuffer_index];
					if(number_value_count > max_float_ascii_size)
					{
						number_value_count = 0;
						parameter_character_count = 0;
						response_state = lw20_response_state_t::PARAMETER_CASE;
						return lw20_status_t::LW20_ERROR;
					}
				}
				break;

			case lw20_response_state_t::CARRIAGE_RETURN:
				if(cbuffer[cbuffer_index] == (uint8_t)'\r')
				{
					response_state = lw20_response_state_t::LINE_FEED;
				}
				else
				{
					number_value_count = 0;
					parameter_character_count = 0;
					response_state = lw20_response_state_t::PARAMETER_CASE;
					return lw20_status_t::LW20_ERROR;
				}
				break;

			case lw20_response_state_t::LINE_FEED:
				if(cbuffer[cbuffer_index] == (uint8_t)'\n')
				{
					response_data = (float)atof(number_value_buf);
					number_value_count = 0;
					parameter_character_count = 0;
					response_state = lw20_response_state_t::PARAMETER_CASE;
					return lw20_status_t::LW20_OK;
				}
				else
				{
					number_value_count = 0;
					parameter_character_count = 0;
					response_state = lw20_response_state_t::PARAMETER_CASE;
					return lw20_status_t::LW20_ERROR;
				}
				break;
		}
	}
	return lw20_status_t::LW20_ERROR;
}

/**
* @brief 	Reading measured distance value in the form of data stream.
*
* @param	void
*
* @return	LW20_OK 			-> OK
* 				LW20_OK 			-> TIMEOUT : When sensor dont response until 1 second, function returns timeout
*					LW20_ERROR 	 	-> ERROR
*/
lw20_status_t LIGHTWARE_LW20::readDistanceStream()
{
	static distance_stream_state_t read_distance_stream_state = distance_stream_state_t::STREAM_SIGN;
	static uint8_t parameter_character_count 		= 0;
	static uint8_t number_value_count 					= 0;
	static uint8_t cbuffer[2048]								= {0};
	static char number_value_buf[10] 						= {0};
	static uint16_t coming_data_size 						= 0;
	static uint16_t cbuffer_index 							= 0;
	static float communication_timeout_delay 		= 0;

	float dt_s = 0;
  monitor.assignDtMeasureChannel(Tools::dt_measure_channel_map_type::CHANNEL1,0.02);
  monitor.updateDtMeasure(Tools::dt_measure_channel_map_type::CHANNEL1);
  dt_s = monitor.getDtMeasure_s(Tools::dt_measure_channel_map_type::CHANNEL1);

	new_stream_detected = false;

	coming_data_size = init_def.uart_source->getDataFromBuffer(cbuffer,2048);

	if(coming_data_size > 0)
	{
		communication_timeout_delay = 0;

		for (cbuffer_index = 0; cbuffer_index < coming_data_size; cbuffer_index++)
		{
			switch (read_distance_stream_state)
			{

				case distance_stream_state_t::STREAM_SIGN:
					if(cbuffer[cbuffer_index] == (uint8_t)'$')
					{
						number_value_count 					= 0;
						parameter_character_count 	= 0;
						read_distance_stream_state	= distance_stream_state_t::PARAMETER_CASE;
					}
					break;

				case distance_stream_state_t::PARAMETER_CASE:
					if(cbuffer[cbuffer_index] == (uint8_t)(parameter_table.median_distance_first_return.param_case[parameter_character_count])+lower_case_offset || (uint8_t)(parameter_table.median_distance_first_return.param_case[parameter_character_count]))
					{
						parameter_character_count++;
						if(parameter_character_count > max_parameter_character_size)
						{
							read_distance_stream_state 	= distance_stream_state_t::STREAM_SIGN;
							parameter_character_count 	= 0;
							number_value_count 					= 0;
							return lw20_status_t::LW20_ERROR;
						}
					}
					else
					{
						read_distance_stream_state 	= distance_stream_state_t::STREAM_SIGN;
						parameter_character_count 	= 0;
						return lw20_status_t::LW20_ERROR;
					}
					if(parameter_character_count == parameter_table.median_distance_first_return.param_size)
					{
						read_distance_stream_state = distance_stream_state_t::COLON;
					}
					break;

				case distance_stream_state_t::COLON:
					if(cbuffer[cbuffer_index] == (uint8_t)':')
					{
						read_distance_stream_state = distance_stream_state_t::VALUE;
					}
					else
					{
						read_distance_stream_state = distance_stream_state_t::STREAM_SIGN;
						parameter_character_count = 0;
						number_value_count = 0;
						return lw20_status_t::LW20_ERROR;
					}
					break;

				case distance_stream_state_t::VALUE:
					if(cbuffer[cbuffer_index] == (uint8_t)' ')
					{
						read_distance_stream_state = distance_stream_state_t::CARRIAGE_RETURN;
					}
					else
					{
						number_value_buf[number_value_count++] = (char)cbuffer[cbuffer_index];
						if(number_value_count > max_float_ascii_size)
						{
							read_distance_stream_state = distance_stream_state_t::STREAM_SIGN;
							parameter_character_count = 0;
							number_value_count = 0;
							return lw20_status_t::LW20_ERROR;
						}
					}
					break;

				case distance_stream_state_t::CARRIAGE_RETURN:
					if(cbuffer[cbuffer_index] == (uint8_t)'\r')
					{
						read_distance_stream_state = distance_stream_state_t::LINE_FEED;
					}
					else
					{
						read_distance_stream_state = distance_stream_state_t::STREAM_SIGN;
						parameter_character_count = 0;
						number_value_count = 0;
						return lw20_status_t::LW20_ERROR;
					}
					break;

				case distance_stream_state_t::LINE_FEED:
					if(cbuffer[cbuffer_index] == (uint8_t)'\n')
					{
						sscanf(number_value_buf, "%f", &distance);
						read_distance_stream_state = distance_stream_state_t::STREAM_SIGN;
						parameter_character_count = 0;
						number_value_count = 0;
						new_stream_detected = true;
						return lw20_status_t::LW20_OK;
					}
					else
					{
						read_distance_stream_state = distance_stream_state_t::STREAM_SIGN;
						parameter_character_count = 0;
						number_value_count = 0;
						return lw20_status_t::LW20_ERROR;
					}
					break;
			}
		}
	}
	else
	{
		communication_timeout_delay += dt_s;
		if(communication_timeout_delay >= max_communication_timeout_delay)
		{
			communication_timeout_delay = max_communication_timeout_delay;
			return lw20_status_t::LW20_TIMEOUT;
		}
	}
	return lw20_status_t::LW20_ERROR;
}

/**
* @brief 	Distance stream configuration
*
* @param	void
*
* @return	LW20_OK			-> OK
*					LW20_ERROR	-> ERROR
*/
void LIGHTWARE_LW20::setDistanceStream()
{
	init_def.uart_source->sendData((uint8_t*)"$1,ldf,0\r", 9);
}

};

};
