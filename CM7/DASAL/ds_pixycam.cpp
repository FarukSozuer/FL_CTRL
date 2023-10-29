/**
 ******************************************************************************
 * @file			: ds_pixycam.hpp
 * @brief		: Pixycam driver class
 * @author		: Emre TUNCAY
 * @date			: 22.12.2020
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

#include "ds_pixycam.hpp"
#include "ds_uart_h747.hpp"
#include "stdint.h"

PIXY_CAMERA pixy_camera;

/**
 * @brief Default constructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
PIXY_CAMERA::PIXY_CAMERA()
{

}

/**
 * @brief Default deconstructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
PIXY_CAMERA::~PIXY_CAMERA()
{

}

/**
 * @brief Reads uart circuler buffer for finding frame start byte (55aa55aa).
 *          If there is frame start byte in the circuler buffer this method read
 *          all frame packet until frame end and write to frame object.
 *
 * @param[in]  void
 *
 * @return 	void
 */
void PIXY_CAMERA::read()
{
	pixy_cam_read_state_t pixy_cam_read_state = pixy_cam_read_state_t::SOF_L;
	uint8_t cbuffer[Peripherals::Uart::UART_BUFFER_SIZE] = { 0 };
	uint16_t coming_data_size = uart2.getDataFromBuffer(cbuffer);
	uint16_t temp_cheksum = 0;

	static frame_t temp_frame;
	static uint8_t temp_number_of_object = 0;

	new_frame = 0;

	for (uint16_t cbuffer_index = 0; cbuffer_index < coming_data_size; cbuffer_index++)
	{
		switch (pixy_cam_read_state)
		{
			case pixy_cam_read_state_t::SOF_L:
				if (cbuffer[cbuffer_index] == 0x55)
				{
					pixy_cam_read_state = pixy_cam_read_state_t::SOF_H;
				}
				break;

			case pixy_cam_read_state_t::SOF_H:
				if (cbuffer[cbuffer_index] == 0xAA)
				{
					pixy_cam_read_state = pixy_cam_read_state_t::SYNC_L;
				}
				else
				{
					pixy_cam_read_state = pixy_cam_read_state_t::SOF_L;
				}
				break;

			case pixy_cam_read_state_t::SYNC_L:
				if (cbuffer[cbuffer_index] == 0x55)
				{
					temp_frame.object[temp_number_of_object].sync = cbuffer[cbuffer_index];
					pixy_cam_read_state = pixy_cam_read_state_t::SYNC_H;
				}
				else
				{
					pixy_cam_read_state = pixy_cam_read_state_t::SOF_L;
				}
				break;

			case pixy_cam_read_state_t::SYNC_H:
				if (cbuffer[cbuffer_index] == 0xAA)
				{
					temp_frame.object[temp_number_of_object].sync = (temp_frame.object[temp_number_of_object].sync << 8) | cbuffer[cbuffer_index];
					pixy_cam_read_state = pixy_cam_read_state_t::CHECKSUM_L;
				}
				else
				{
					pixy_cam_read_state = pixy_cam_read_state_t::SOF_L;
				}
				break;

			case pixy_cam_read_state_t::CHECKSUM_L:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].checksum))[0] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::CHECKSUM_H;
				break;

			case pixy_cam_read_state_t::CHECKSUM_H:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].checksum))[1] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::SIGN_NUMBER_L;
				break;

			case pixy_cam_read_state_t::SIGN_NUMBER_L:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].sign_number))[0] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::SIGN_NUMBER_H;
				break;

			case pixy_cam_read_state_t::SIGN_NUMBER_H:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].sign_number))[1] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::X_L;
				break;

			case pixy_cam_read_state_t::X_L:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].x))[0] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::X_H;
				break;

			case pixy_cam_read_state_t::X_H:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].x))[1] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::Y_L;
				break;

			case pixy_cam_read_state_t::Y_L:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].y))[0] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::Y_H;
				break;

			case pixy_cam_read_state_t::Y_H:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].y))[1] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::WEIGHT_L;
				break;

			case pixy_cam_read_state_t::WEIGHT_L:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].width))[0] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::WEIGHT_H;
				break;

			case pixy_cam_read_state_t::WEIGHT_H:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].width))[1] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::HEIGHT_L;
				break;

			case pixy_cam_read_state_t::HEIGHT_L:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].height))[0] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::HEIGHT_H;
				break;

			case pixy_cam_read_state_t::HEIGHT_H:
				((uint8_t*) &(temp_frame.object[temp_number_of_object].height))[1] = cbuffer[cbuffer_index];
				pixy_cam_read_state = pixy_cam_read_state_t::NEW_OBJECT_L;
				break;

			case pixy_cam_read_state_t::NEW_OBJECT_L:
				if (cbuffer[cbuffer_index] == 0x55)
				{
					pixy_cam_read_state = pixy_cam_read_state_t::NEW_OBJECT_H;
				}
				else
				{
					temp_number_of_object++;
					if (temp_number_of_object >= MAX_NUMBER_OF_OBJECT_PER_FRAME)
					{
						temp_number_of_object = MAX_NUMBER_OF_OBJECT_PER_FRAME;
						error_counter++;
					}
					for (uint8_t i = 0; i < temp_number_of_object; i++)
					{
						temp_cheksum = ((uint8_t*) &(temp_frame.object[i]))[4] + ((uint8_t*) &(temp_frame.object[i]))[5] + ((uint8_t*) &(temp_frame.object[i]))[6]
								+ ((uint8_t*) &(temp_frame.object[i]))[7] + ((uint8_t*) &(temp_frame.object[i]))[8] + ((uint8_t*) &(temp_frame.object[i]))[9]
								+ ((uint8_t*) &(temp_frame.object[i]))[10] + ((uint8_t*) &(temp_frame.object[i]))[11] + ((uint8_t*) &(temp_frame.object[i]))[12]
								+ ((uint8_t*) &(temp_frame.object[i]))[13];

						if (temp_cheksum == temp_frame.object[i].checksum)
						{
							sensor_status.wrong_checksum = 0;
						}
						else
						{
							sensor_status.wrong_checksum = 1;
						}

						if (temp_frame.object[i].sign_number == 1)
						{
							sensor_status.wrong_sign_number = 0;
						}
						else
						{
							sensor_status.wrong_sign_number = 1;
						}

						if (sensor_status.wrong_checksum && sensor_status.wrong_sign_number)
						{
							sensor_status.healty = 0;
							error_counter++;
						}
						else
						{
							sensor_status.healty = 1;
						}

						frame.object[i].sync = temp_frame.object[i].sync;
						frame.object[i].checksum = temp_frame.object[i].checksum;
						frame.object[i].sign_number = temp_frame.object[i].sign_number;
						frame.object[i].x = temp_frame.object[i].x;
						frame.object[i].y = temp_frame.object[i].y;
						frame.object[i].width = temp_frame.object[i].width;
						frame.object[i].height = temp_frame.object[i].height;
					}
					number_of_object = temp_number_of_object;
					temp_number_of_object = 0;
					new_frame = 1;

					pixy_cam_read_state = pixy_cam_read_state_t::SOF_L;
				}
				break;

			case pixy_cam_read_state_t::NEW_OBJECT_H:
				if (cbuffer[cbuffer_index] == 0xAA)
				{
					temp_number_of_object++;
					if (temp_number_of_object >= MAX_NUMBER_OF_OBJECT_PER_FRAME)
					{
						temp_number_of_object = MAX_NUMBER_OF_OBJECT_PER_FRAME;
						error_counter++;
					}
					pixy_cam_read_state = pixy_cam_read_state_t::CHECKSUM_L;
				}
				else
				{
					temp_number_of_object++;
					if (temp_number_of_object > MAX_NUMBER_OF_OBJECT_PER_FRAME)
					{
						temp_number_of_object = MAX_NUMBER_OF_OBJECT_PER_FRAME;
						error_counter++;
					}
					for (uint8_t i = 0; i < temp_number_of_object; i++)
					{
						temp_cheksum = ((uint8_t*) &(temp_frame.object[i]))[4] + ((uint8_t*) &(temp_frame.object[i]))[5] + ((uint8_t*) &(temp_frame.object[i]))[6]
								+ ((uint8_t*) &(temp_frame.object[i]))[7] + ((uint8_t*) &(temp_frame.object[i]))[8] + ((uint8_t*) &(temp_frame.object[i]))[9]
								+ ((uint8_t*) &(temp_frame.object[i]))[10] + ((uint8_t*) &(temp_frame.object[i]))[11] + ((uint8_t*) &(temp_frame.object[i]))[12]
								+ ((uint8_t*) &(temp_frame.object[i]))[13];

						if (temp_cheksum == temp_frame.object[i].checksum)
						{
							sensor_status.wrong_checksum = 0;
						}
						else
						{
							sensor_status.wrong_checksum = 1;
						}

						if (temp_frame.object[i].sign_number == 1)
						{
							sensor_status.wrong_sign_number = 0;
						}
						else
						{
							sensor_status.wrong_sign_number = 1;
						}

						if (sensor_status.wrong_checksum && sensor_status.wrong_sign_number)
						{
							sensor_status.healty = 0;
							error_counter++;
						}
						else
						{
							sensor_status.healty = 1;
						}

						frame.object[i].sync = temp_frame.object[i].sync;
						frame.object[i].checksum = temp_frame.object[i].checksum;
						frame.object[i].sign_number = temp_frame.object[i].sign_number;
						frame.object[i].x = temp_frame.object[i].x;
						frame.object[i].y = temp_frame.object[i].y;
						frame.object[i].width = temp_frame.object[i].width;
						frame.object[i].height = temp_frame.object[i].height;
					}
					number_of_object = temp_number_of_object;
					temp_number_of_object = 0;
					new_frame = 1;

					pixy_cam_read_state = pixy_cam_read_state_t::SOF_L;
				}
				break;

			default:
				break;
		}
	}
}

/**
 * @brief Sometimes the objects read by sensor can be parts of a single
 *          irlock due to light refraction or other environmental condition.
 *          In this case, a single irlock position is estimated by combining
 *          the pieces.
 *          **This method should be used when single irlock is used.**
 *
 * @param[in]  void
 *
 * @return 	 0: Object position estimation made using 1 object position.
 *           1: Object position estimation made using more than 1 object position.
 */
uint8_t PIXY_CAMERA::estimateObject()
{
	uint16_t Xmax = 0, Xmin = 0;
	uint16_t Ymax = 0, Ymin = 0;
	uint8_t return_value = 0;
	if (number_of_object > 50)
	{
		Xmin = frame.object[0].x;
		Ymin = frame.object[0].y;
		for (uint8_t i = 0; i < number_of_object; i++)
		{
			if (frame.object[i].x > Xmax)
			{
				Xmax = frame.object[i].x;
			}

			if (frame.object[i].y > Ymax)
			{
				Ymax = frame.object[i].y;
			}

			if (frame.object[i].x < Xmin)
			{
				Xmin = frame.object[i].x;
			}

			if (frame.object[i].x < Xmin)
			{
				Ymin = frame.object[i].y;
			}
		}
		estimated_object.w = Xmax - Xmin;
		estimated_object.h = Ymax - Ymin;
		estimated_object.x = (float) Xmin + (float) (estimated_object.w) / 2;
		estimated_object.y = (float) Ymin + (float) (estimated_object.h) / 2;

		return_value = 1;
	}
	else if (number_of_object == 1)
	{
		estimated_object.w = frame.object[0].width;
		estimated_object.h = frame.object[0].height;
		estimated_object.x = frame.object[0].x;
		estimated_object.y = frame.object[0].y;

		return_value = 0;
	}
	return return_value;
}


/**
 * @brief Estimate distance between the object and pixel domain origin.
 *
 * @param[in]  float target_distance:  Vertical distance beetween vehicle and irlock
 *             float lens_focus     :  Pixy camera lens focus length about mm
 *
 * @return 	void Nothing
 */
void PIXY_CAMERA::findDistanceFromOrigin(float target_distance, float lens_focus)
{
	distance_from_irlock.x_cm = (((float) estimated_object.x - (float) 160) * target_distance) / lens_focus;
	distance_from_irlock.y_cm = (((float) 100 - (float) estimated_object.y) * target_distance) / lens_focus;
}

/**
 * @brief Scheduler
 *
 * @param[in]  void
 *
 * @return 	   void
 */
void PIXY_CAMERA::scheduler(float altitude)
{
	float lens_focus = 308.0;

	read();
	if (new_frame && sensor_status.healty)
	{
		estimateObject();
		findDistanceFromOrigin(altitude, lens_focus);
		isItActive = true;
	}
	else
	{
		isItActive = false;
		object_speed_ref.x = 0.0;
		object_speed_ref.y = 0.0;
	}
}
