/**
 ******************************************************************************
 * @file		: ds_pixycam.hpp
 * @brief		: Pixycam driver class
 * @author		: Emre TUNCAY
 * @date		: 22.12.2020
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

#ifndef DS_PIXY_CAM_HPP
#define	DS_PIXY_CAM_HPP

#pragma once
#include "ds_uart_h747.hpp"
#include <math.h>
const uint8_t MAX_NUMBER_OF_OBJECT_PER_FRAME = 10;
const uint8_t BREAK_LOOP = 0;
const uint8_t CONTINUE_LOOP = 1;

enum class pixy_cam_read_state_t : uint8_t
{
	SOF_L,
	SOF_H,
	SYNC_L,
	SYNC_H,
	CHECKSUM_L,
	CHECKSUM_H,
	SIGN_NUMBER_L,
	SIGN_NUMBER_H,
	X_L,
	X_H,
	Y_L,
	Y_H,
	WEIGHT_L,
	WEIGHT_H,
	HEIGHT_L,
	HEIGHT_H,
	NEW_OBJECT_L,
	NEW_OBJECT_H
};

enum class process_frame_state_t : uint8_t
{
	SOF_IN_OLDPACKET,
	SOF_IN_CURRRENTPACKET,
};

enum class frame_update_t : uint8_t
{
	FULL_FRAME_UPDATE = 0,
	HALF_FRAME_UPDATE = 1,
};

typedef struct
{
		uint16_t sync;
		uint16_t checksum;
		uint16_t sign_number;
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
} object_t;

typedef struct
{
		float x;
		float y;
		float w;
		float h;
} estimated_object_t;

typedef struct
{
		object_t object[MAX_NUMBER_OF_OBJECT_PER_FRAME];
} frame_t;

typedef struct
{
		uint8_t healty :1, wrong_sign_number :1, wrong_checksum :1;
} sensor_status_t;

typedef struct
{
		float x_cm;
		float y_cm;
} distance_to_object_t;

typedef struct
{
		float x;
		float y;
} object_speed_ref_t;

class PIXY_CAMERA
{
	public:

		PIXY_CAMERA();

		bool isItActive = false;

		uint32_t error_counter = 0;
		frame_t frame = { 0 };
		estimated_object_t estimated_object = { 0 };
		uint8_t number_of_object = 0;
		uint8_t new_frame = 0;
		distance_to_object_t distance_from_irlock = { 0 };
		sensor_status_t sensor_status = { 0 };
		object_speed_ref_t object_speed_ref = { 0 };

		virtual ~PIXY_CAMERA();

		void read();
		uint8_t estimateObject();
		void findDistanceFromOrigin(float target_distance, float lens_focus);
		void scheduler(float altitude);

	protected:

	private:

};

extern PIXY_CAMERA pixy_camera;

#endif	/* DS_PIXY_CAM_HPP */
