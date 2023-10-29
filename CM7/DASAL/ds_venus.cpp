/**
 ******************************************************************************
 * @file			: ds_venus.cpp
 * @brief		: Task scheduler header file
 *				  		This file contains task structure of the application
 * @author		: Gurtac Kadem
 * 						Alperen Emiroglu
 * @date			: 18.11.2020
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

/*
 * Begin of Includes
 */
#include <ds_venus.hpp>
#include <ds_uart.hpp>
#include <ds_uart_h747.hpp>
#include <ds_debug_tools.hpp>
#include <stdio.h>
#include <math.h>
#include "ds_parse.hpp"
#include "ds_parameters.hpp"
// End of Includes

DS_VENUS venus;

/**
 * @brief 			Default constructor
 *
 * @param[in]  void
 *
 * @return 		void
 */
DS_VENUS::DS_VENUS()
{
	status_flag.u16 = 0;
}

/**
 * @brief 			Read package
 *
 * @param[in]  void
 *
 * @return 		void
 */
void DS_VENUS::readPackage(void)
{
	uint8_t coming_byte_number = 0;
	uint8_t coming_byte[DS_VENUS_UART_BYTE_SIZE] = { 0 };

	 coming_byte_number = uart1.getDataFromBuffer(coming_byte, DS_VENUS_UART_BYTE_SIZE);
	 if(coming_byte_number > DS_VENUS_UART_BYTE_SIZE)
	 {
		 coming_byte_number = DS_VENUS_UART_BYTE_SIZE;
	 }

	 if(coming_byte_number > 0)
	 {
		 coming_byte_number = uart1.getDataFromBuffer(coming_byte, coming_byte_number);
		 grabPackage( coming_byte, coming_byte_number );
	 }
}

/**
 * @brief 			Grab package
 *
 * @param[in]  uint8_t *coming_bytes
 * @param[in]	uint16_t bytes_number
 *
 * @return 		void
 */
void DS_VENUS::grabPackage(uint8_t *coming_bytes, uint16_t bytes_number)
{
	uint16_t index = 0;
	uint16_t payload_index = 0;

	for (index = 0; index < bytes_number; index++)
	{
		switch (packet_state)
		{
			case venus_packet_state_t::START_BYTE:

				if (PACKET_START_BYTE == coming_bytes[index])
				{
					packet_state = venus_packet_state_t::PACKET_ID;
				}

				break;

			case venus_packet_state_t::PACKET_ID:

				pkt_id = coming_bytes[index];
				packet_state = venus_packet_state_t::DATA_SIZE;
				break;

			case venus_packet_state_t::DATA_SIZE:

				pkt_size = coming_bytes[index];
				packet_state = venus_packet_state_t::HEADER_CHECKSUM;
				break;

			case venus_packet_state_t::HEADER_CHECKSUM:

				pkt_header_chksm = coming_bytes[index];
				calc_header_chksm = (uint8_t) (pkt_id + pkt_size);

				if (pkt_header_chksm == calc_header_chksm)
				{
					packet_state = venus_packet_state_t::RESPONSE;
					pkt_data_idx = 0;
				}
				else
				{
					packet_state = venus_packet_state_t::START_BYTE;
				}
				break;

			case venus_packet_state_t::RESPONSE:

				pkt_data[pkt_data_idx] = coming_bytes[index];
				pkt_data_idx++;

				if (pkt_data_idx >= pkt_size)
				{
					packet_state = venus_packet_state_t::PAYLOAD_CHECKSUM;
					calc_payload_chksm = 0;
				}
				break;

			case venus_packet_state_t::PAYLOAD_CHECKSUM:

				pkt_payload_chksm = coming_bytes[index];

				for (payload_index = 0; payload_index < pkt_size; payload_index++)
				{
					calc_payload_chksm = (uint8_t) (calc_payload_chksm + pkt_data[payload_index]);
				}

				if (pkt_payload_chksm == calc_payload_chksm)
				{
					parsePackage();
				}
				packet_state = venus_packet_state_t::START_BYTE;
				break;

			default:
			{
				packet_state = venus_packet_state_t::START_BYTE;
				break;
			}
		}
	}
}

/**
 * @brief 			Parse package
 *
 * @param[in]  void
 *
 * @return 		void
 */
void DS_VENUS::parsePackage(void)
{
	uint16_t idx = 0;
	float gimbal_target_deg = 0.0;

	switch (pkt_id)
	{
		case BOARD_INFORMATION:
			packet.add_uint8(&(board_info_pkt.card_version), pkt_data, &idx);
			packet.add_uint16(&(board_info_pkt.software_version), pkt_data, &idx);
			packet.add_uint8(&(board_info_pkt.reserved[14]), pkt_data, &idx);

			break;

		case EXTRA_BOARD_INFORMATION:
			packet.add_uint8(&(extra_board_info_pkt.device_id[9]), pkt_data, &idx);
			packet.add_uint8(&(extra_board_info_pkt.mcu_id[12]), pkt_data, &idx);
			packet.add_uint32(&(extra_board_info_pkt.eeprom_size), pkt_data, &idx);
			packet.add_uint8(&(extra_board_info_pkt.reserved[44]), pkt_data, &idx);

			break;

		case ANGLE_INFORMATION:
			packet.add_int16(&(angle_info_pkt.roll_imu), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.roll_target_imu), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.roll_target_speed), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.pitch_imu), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.pitch_target_imu), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.pitch_target_speed), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.yaw_imu), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.yaw_target_imu), pkt_data, &idx);
			packet.add_int16(&(angle_info_pkt.yaw_target_speed), pkt_data, &idx);

			angle_info_pkt.roll_angle_deg = (float) angle_info_pkt.roll_imu * IMU_TO_DEG;
			angle_info_pkt.pitch_angle_deg = (float) angle_info_pkt.pitch_imu * IMU_TO_DEG;
			angle_info_pkt.yaw_angle_deg = (float) angle_info_pkt.yaw_imu * IMU_TO_DEG;

			if ((get_target_imu_unit_deg == true) && (angle_info_pkt.yaw_target_imu != 0) && (angle_info_pkt.yaw_target_imu == angle_info_pkt.yaw_imu))
			{
				get_target_imu_unit_deg = false;
				angle_info_pkt.yaw_imu_command = angle_info_pkt.yaw_target_imu;

				if (!first_pilot_mode_command)
				{
					first_pilot_mode_command = true;
				}
			}

			break;

		case STATUS:
			packet.add_int16(&(status_pkt.acc_data), pkt_data, &idx);
			packet.add_int16(&(status_pkt.gyro_data), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.serial_err_cnt), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.system_error), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.system_sub_error), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.reserved1[3]), pkt_data, &idx);
			packet.add_int8(&(status_pkt.reserved2[6]), pkt_data, &idx);
			packet.add_int16(&(status_pkt.reserved3), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.reserved4[4]), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.imu_angle[3]), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.frame_imu_angle[3]), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.target_angle[3]), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.cycle_time), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.i2c_error_count), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.error_code), pkt_data, &idx);
			packet.add_uint16(&(status_pkt.bat_level), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.rt_data_flags), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.cur_imu), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.reserved5), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.motor_power[3]), pkt_data, &idx);
			packet.add_uint8(&(status_pkt.reserved6[8]), pkt_data, &idx);

			break;

		case BROAD_STATUS:
			packet.add_uint8(&(broad_status_pkt.reserved1[13]), pkt_data, &idx);
			packet.add_uint16(&(broad_status_pkt.current), pkt_data, &idx);
			packet.add_uint8(&(broad_status_pkt.reserved2[6]), pkt_data, &idx);
			packet.add_int8(&(broad_status_pkt.imu_temperature), pkt_data, &idx);
			packet.add_int8(&(broad_status_pkt.frame_imu_temperature), pkt_data, &idx);
			packet.add_uint8(&(broad_status_pkt.reserved3[101]), pkt_data, &idx);

			break;

		case CONFIRM:
			switch (pkt_data[0])
			{
				case MOTOR_ENABLE:
					confirm_flag.bits.motor_enable_active = 1;

					break;

				case MOTOR_DISABLE:
					confirm_flag.bits.motor_disable_active = 1;

					break;

				case GIMBAL_RESET:
					confirm_flag.bits.gimbal_reset_active = 1;

					break;

				case EXECUTE_COMMAND:
					confirm_flag.bits.execute_command_active = 1;

					break;

				case CONTROL_COMMAND:
					confirm_flag.bits.control_command_active = 1;

					break;
				case RAW_ENCODER_DATA:
					confirm_flag.bits.raw_encoder_command_active = 1;

					break;
				default:

					break;
			}

			break;

		case RAW_ENCODER_BROAD_STATUS:
			packet.add_uint16(&(encoder_status_pkt.timestamp_ms), pkt_data, &idx);
			packet.add_int24(&(encoder_status_pkt.encoder_roll), pkt_data, &idx);
			packet.add_int24(&(encoder_status_pkt.encoder_pitch), pkt_data, &idx);
			packet.add_int24(&(encoder_status_pkt.encoder_yaw), pkt_data, &idx);

			if ((!encoder_zero_calculated) && (first_pilot_mode_command))
			{
				encoder_zero_calculated = true;
				encoder_status_pkt.first_data_deg = (((float) encoder_status_pkt.encoder_yaw / 16777216.0) * 360.0);
			}

			encoder_status_pkt.encoder_yaw_angle_deg = (((float) encoder_status_pkt.encoder_yaw / 16777216.0) * 360.0) - encoder_status_pkt.first_data_deg;
			checkLimitsDeg(encoder_status_pkt.encoder_yaw_angle_deg, MINIMUM_ANGLE_DEG, MAXIMUM_ANGLE_DEG);
			encoder_status_pkt.true_north_deg = encoder_status_pkt.encoder_yaw_angle_deg + parameters.heading;
			checkLimitsDeg(encoder_status_pkt.true_north_deg, MINIMUM_ANGLE_DEG, MAXIMUM_ANGLE_DEG);

			gimbal_target_deg = 90.0 + angle_info_pkt.pitch_angle_deg;

			if (gimbal_target_deg != 90.0 && parameters.GPS_scaleLonDown != 0.0)
			{
				target_latitude_uddeg = (float) (parameters.gnss_latitude
						+ (int32_t) ((parameters.altitude * tanf(gimbal_target_deg * C_PI / 180.0f)) * 89.83152098
								* cosf(encoder_status_pkt.true_north_deg * C_PI / 180.0f))) / 10000000.0;
				target_longitude_uddeg = (float) (parameters.gnss_longitude
						+ (int32_t) (((parameters.altitude * tanf(gimbal_target_deg * C_PI / 180.0f)) * 89.83152098
								* sinf(encoder_status_pkt.true_north_deg * C_PI / 180.0f)) / parameters.GPS_scaleLonDown)) / 10000000.0;
			}
			break;

		default:
			break;
	}

}

/**
 * @brief 			Send request
 *
 * @param[in]  message_id_t message_id
 *
 * @return 		void
 */
void DS_VENUS::sendRequest(message_id_t message_id)
{
	uint8_t gnd_data[16] = { 0 };
	uint8_t idx = 0;
	uint8_t chk_sum = 0;

	gnd_data[0] = 0x3E;
	gnd_data[1] = (uint8_t) message_id;
	gnd_data[2] = 0x00;

	for (idx = 1; idx < 3; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx];
	}

	gnd_data[3] = chk_sum;
	chk_sum = 0;

	for (idx = 0; idx < 1; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx + 4];
	}

	gnd_data[5] = chk_sum;

	//Uart.uartSendData(UART_NUMBER_4, gnd_data, 6);

}

/**
 * @brief 			Send command
 *
 * @param[in]  message_id_t message_id
 * @param[in]	command_t command
 *
 * @return 		void
 */
void DS_VENUS::sendCommand(message_id_t message_id, command_t command)
{
	uint8_t gnd_data[16] = { 0 };
	uint8_t idx = 0;
	uint8_t chk_sum = 0;

	gnd_data[0] = 0x3E;
	gnd_data[1] = (uint8_t) message_id;
	gnd_data[2] = 0x01;

	for (idx = 1; idx < 3; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx];
	}

	gnd_data[3] = chk_sum;
	chk_sum = 0;
	idx = 4;

	gnd_data[idx] = (uint8_t) command;

	for (idx = 0; idx < 1; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx + 4];
	}

	gnd_data[5] = chk_sum;

	//Uart.uartSendData(UART_NUMBER_4, gnd_data, 6);

}

/**
 * @brief 			Send control command
 *
 * @param[in]  control_command_pkt_t ref_control_command
 *
 * @return 		void
 */
void DS_VENUS::sendControlCommand(control_command_pkt_t ref_control_command)
{
	uint8_t gnd_data[50] = { 0 };
	uint16_t idx = 0;
	uint8_t chk_sum = 0;

	gnd_data[0] = 0x3E;
	gnd_data[1] = 0x43;
	gnd_data[2] = 0x0F;

	for (idx = 1; idx < 3; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx];
	}

	gnd_data[3] = chk_sum;

	chk_sum = 0;

	idx = 4;

	packet.parse_uint8(&(ref_control_command.control_mode_roll), gnd_data, &idx);
	packet.parse_uint8(&(ref_control_command.control_mode_pitch), gnd_data, &idx);
	packet.parse_uint8(&(ref_control_command.control_mode_yaw), gnd_data, &idx);
	packet.parse_int16(&(ref_control_command.speed_roll), gnd_data, &idx);
	packet.parse_int16(&(ref_control_command.imu_roll), gnd_data, &idx);
	packet.parse_int16(&(ref_control_command.speed_pitch), gnd_data, &idx);
	packet.parse_int16(&(ref_control_command.imu_pitch), gnd_data, &idx);
	packet.parse_int16(&(ref_control_command.speed_yaw), gnd_data, &idx);
	packet.parse_int16(&(ref_control_command.imu_yaw), gnd_data, &idx);

	for (idx = 0; idx < 15; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx + 4];
	}

	gnd_data[19] = chk_sum;

	// Uart.uartSendData(UART_NUMBER_4, gnd_data, 20);

}

/**
 * @brief 			Send encoder raw data command
 *
 * @param[in]  encoder_command_pkt_t ref_encoderRawData_command
 *
 * @return 		void
 */
void DS_VENUS::sendEncoderRawDataCommand(encoder_command_pkt_t ref_encoderRawData_command)
{
	uint8_t gnd_data[50] = { 0 };
	uint16_t idx = 0;
	uint8_t chk_sum = 0;

	gnd_data[0] = 0x3E;
	gnd_data[1] = 0x55;
	gnd_data[2] = 0x15;

	for (idx = 1; idx < 3; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx];
	}

	gnd_data[3] = chk_sum;
	chk_sum = 0;
	idx = 4;

	packet.parse_uint8(&(ref_encoderRawData_command.id), gnd_data, &idx);
	packet.parse_uint16(&(ref_encoderRawData_command.interval_ms), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[0]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[1]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[2]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[3]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[4]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[5]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[6]), gnd_data, &idx);
	packet.parse_int8(&(ref_encoderRawData_command.config[7]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[0]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[1]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[2]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[3]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[4]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[5]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[6]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[7]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[8]), gnd_data, &idx);
	packet.parse_uint8(&(ref_encoderRawData_command.reserved[9]), gnd_data, &idx);

	for (idx = 0; idx < 21; idx++)
	{
		chk_sum = chk_sum + gnd_data[idx + 4];
	}

	gnd_data[25] = chk_sum;

	//Uart.uartSendData(UART_NUMBER_4, gnd_data, 26);

}

/**
 * @brief 			Send reference for axes
 *
 * @param[in]  mode_command_t mode_command
 * @param[in]	axis_t axis
 * @param[in]	int16_t speed_ref_dps
 * @param[in]	float angle_ref_deg
 *
 * @return 		void
 */
void DS_VENUS::sendReference(mode_command_t mode_command, axis_t axis, int16_t speed_ref_dps, float angle_ref_deg)
{
	control_command_pkt_t tr_control_command_pkt;
	int16_t temp_angle_ref_deg = 0;

	switch (axis)
	{
		case axis_t::ROLL:
			tr_control_command_pkt.control_mode_roll = (uint8_t) mode_command;

			if (mode_command == mode_command_t::SPEED_MODE)
			{
				tr_control_command_pkt.speed_roll = speed_ref_dps;
			}
			else if (mode_command == mode_command_t::ANGLE_MODE)
			{
				tr_control_command_pkt.imu_roll = angle_info_pkt.roll_imu + (int16_t) (angle_ref_deg * DEG_TO_IMU);
			}
			else if (mode_command == mode_command_t::SPEED_ANGLE_MODE)
			{
				tr_control_command_pkt.speed_roll = speed_ref_dps;
				tr_control_command_pkt.imu_roll = angle_info_pkt.roll_imu + (int16_t) (angle_ref_deg * DEG_TO_IMU);
			}
			else if (mode_command == mode_command_t::MODE_ANGLE_REL_FRAME)
			{
				tr_control_command_pkt.imu_roll = (int16_t) (angle_ref_deg * DEG_TO_IMU);
				;
			}
			else
			{
				tr_control_command_pkt.speed_roll = 0;
				tr_control_command_pkt.imu_roll = 0;
			}

			break;

		case axis_t::PITCH:
			tr_control_command_pkt.control_mode_pitch = (uint8_t) mode_command;

			if (mode_command == mode_command_t::SPEED_MODE)
			{
				tr_control_command_pkt.speed_pitch = speed_ref_dps;
			}
			else if (mode_command == mode_command_t::ANGLE_MODE)
			{
				tr_control_command_pkt.imu_pitch = angle_info_pkt.pitch_imu + (int16_t) (angle_ref_deg * DEG_TO_IMU);
			}
			else if (mode_command == mode_command_t::SPEED_ANGLE_MODE)
			{
				tr_control_command_pkt.speed_pitch = speed_ref_dps;
				tr_control_command_pkt.imu_pitch = angle_info_pkt.pitch_imu + (int16_t) (angle_ref_deg * DEG_TO_IMU);
			}
			else if (mode_command == mode_command_t::MODE_ANGLE_REL_FRAME)
			{
				tr_control_command_pkt.imu_pitch = (int16_t) (angle_ref_deg * DEG_TO_IMU);
			}
			else
			{
				tr_control_command_pkt.speed_pitch = 0;
				tr_control_command_pkt.imu_pitch = 0;
			}

			break;

		case axis_t::YAW:
			tr_control_command_pkt.control_mode_yaw = (uint8_t) mode_command;

			if (encoder_status_pkt.encoder_yaw_angle_deg + angle_ref_deg > YAW_MAXIMUM_ANGLE_DEG)
			{
				temp_angle_ref_deg = YAW_MAXIMUM_ANGLE_DEG - (int16_t) encoder_status_pkt.encoder_yaw_angle_deg;
			}
			else if (encoder_status_pkt.encoder_yaw_angle_deg + angle_ref_deg < YAW_MINIMUM_ANGLE_DEG)
			{
				temp_angle_ref_deg = YAW_MINIMUM_ANGLE_DEG - (int16_t) encoder_status_pkt.encoder_yaw_angle_deg;
			}
			else
			{
				temp_angle_ref_deg = (int16_t) angle_ref_deg;
			}

			if (mode_command == mode_command_t::SPEED_MODE)
			{
				tr_control_command_pkt.speed_yaw = speed_ref_dps;
			}
			else if (mode_command == mode_command_t::ANGLE_MODE)
			{
				tr_control_command_pkt.imu_yaw = angle_info_pkt.yaw_imu + (temp_angle_ref_deg * DEG_TO_IMU);
			}
			else if (mode_command == mode_command_t::SPEED_ANGLE_MODE)
			{
				tr_control_command_pkt.speed_yaw = speed_ref_dps;
				tr_control_command_pkt.imu_yaw = angle_info_pkt.yaw_imu + (temp_angle_ref_deg * DEG_TO_IMU);
			}
			else if (mode_command == mode_command_t::MODE_ANGLE_REL_FRAME)
			{
				tr_control_command_pkt.imu_yaw = (temp_angle_ref_deg * DEG_TO_IMU);
			}
			else
			{
				tr_control_command_pkt.speed_yaw = 0;
				tr_control_command_pkt.imu_yaw = 0;
			}

			angle_info_pkt.yaw_imu_command = tr_control_command_pkt.imu_yaw;
			angle_info_pkt.encoder_yaw_angle_command_deg = encoder_status_pkt.encoder_yaw_angle_deg + temp_angle_ref_deg;

			break;

		default:
			break;
	}
	checkLimits(tr_control_command_pkt.imu_roll, ROLL_MINIMUM_ANGLE_DEG, ROLL_MAXIMUM_ANGLE_DEG);
	checkLimits(tr_control_command_pkt.imu_pitch, PITCH_MINIMUM_ANGLE_DEG, PITCH_MAXIMUM_ANGLE_DEG);

	sendControlCommand(tr_control_command_pkt);
}

/**
 * @brief 			Send pilot mode
 *
 * @param[in]  void
 *
 * @return 		void
 */
void DS_VENUS::sendPilotMode(void)
{
	sendExecuteCommand(command_t::HOME_POSITION);
	angle_info_pkt.encoder_yaw_angle_command_deg = 0;

	pilot_mode_command_send = true;
}

/**
 * @brief 			Send 2D command
 *
 * @param[in]  float command_pitch_deg
 * @param[in]	float command_yaw_deg
 *
 * @return 		void
 */
void DS_VENUS::send2DCommand(float command_pitch_deg, float command_yaw_deg)
{
	int16_t temp_angle_ref_deg = 0;
	control_command_pkt_t tr_control_command_pkt;

	if (encoder_status_pkt.encoder_yaw_angle_deg + command_yaw_deg > (float) YAW_MAXIMUM_ANGLE_DEG)
	{
		temp_angle_ref_deg = YAW_MAXIMUM_ANGLE_DEG - (int16_t) encoder_status_pkt.encoder_yaw_angle_deg;
	}
	else if (encoder_status_pkt.encoder_yaw_angle_deg + command_yaw_deg < (float) YAW_MINIMUM_ANGLE_DEG)
	{
		temp_angle_ref_deg = YAW_MINIMUM_ANGLE_DEG - (int16_t) encoder_status_pkt.encoder_yaw_angle_deg;
	}
	else
	{
		temp_angle_ref_deg = (int16_t) command_yaw_deg;
	}

	tr_control_command_pkt.control_mode_roll = (uint8_t) mode_command_t::NO_CONTROL;
	tr_control_command_pkt.control_mode_pitch = (uint8_t) mode_command_t::ANGLE_MODE;
	tr_control_command_pkt.control_mode_yaw = (uint8_t) mode_command_t::ANGLE_MODE;

	tr_control_command_pkt.imu_pitch = angle_info_pkt.pitch_imu + (int16_t) (command_pitch_deg * DEG_TO_IMU);
	tr_control_command_pkt.imu_yaw = angle_info_pkt.yaw_imu + (int16_t) (temp_angle_ref_deg * DEG_TO_IMU);

	checkLimits(tr_control_command_pkt.imu_pitch, PITCH_MINIMUM_ANGLE_DEG, PITCH_MAXIMUM_ANGLE_DEG);

	angle_info_pkt.yaw_imu_command = tr_control_command_pkt.imu_yaw;
	angle_info_pkt.encoder_yaw_angle_command_deg = encoder_status_pkt.encoder_yaw_angle_deg + temp_angle_ref_deg;

	sendControlCommand(tr_control_command_pkt);
}

/**
 * @brief 			Send 2D angle command
 *
 * @param[in]  float command_pitch_deg
 * @param[in]	float command_yaw_deg
 *
 * @return 		void
 */
void DS_VENUS::send2DAngleCommand(float command_pitch_deg, float command_yaw_deg)
{
	sendAngleReference(mode_command_t::NO_CONTROL, mode_command_t::ANGLE_MODE, mode_command_t::ANGLE_MODE, 0.0, command_pitch_deg, command_yaw_deg);
}

/**
 * @brief 			Send yaw angle command
 *
 * @param[in]	float command_yaw_deg
 *
 * @return 		void
 */
void DS_VENUS::sendYawAngleCommand(float command_yaw_deg)
{
	sendAngleReference(mode_command_t::NO_CONTROL, mode_command_t::NO_CONTROL, mode_command_t::ANGLE_MODE, 0.0, 0.0, command_yaw_deg);
}

/**
 * @brief 			Send motor enable
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendMotorEnable(void)
{
	sendCommand(message_id_t::MOTOR_ENABLE, command_t::NORMAL_MODE);
}

/**
 * @brief 			Send motor disable
 *
 * @param[in]	command_t command
 *
 * @return 		void
 */
void DS_VENUS::sendMotorDisable(command_t command)
{
	sendCommand(message_id_t::MOTOR_DISABLE, command);
}

/**
 * @brief 			Send board information
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendBoardInformation(void)
{
	sendCommand(message_id_t::BOARD_INFORMATION, command_t::NORMAL_MODE);
}

/**
 * @brief 			Send extra board information
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendExtraBoardInformation(void)
{
	sendCommand(message_id_t::EXTRA_BOARD_INFORMATION, command_t::NORMAL_MODE);
}

/**
 * @brief 			Send gimbal reset
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendReset(void)
{
	sendRequest(message_id_t::GIMBAL_RESET);
}

/**
 * @brief 			Send angle information
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendAngleInformation(void)
{
	if (pilot_mode_command_send)
	{
		pilot_mode_command_send = false;
		get_target_imu_unit_deg = true;
	}

	sendRequest(message_id_t::ANGLE_INFORMATION);
}

/**
 * @brief 			Send execute command
 *
 * @param[in]	command_t command
 *
 * @return 		void
 */
void DS_VENUS::sendExecuteCommand(command_t command)
{
	sendCommand(message_id_t::EXECUTE_INFORMATION, command);
}

/**
 * @brief 			Send status
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendStatus(void)
{
	sendCommand(message_id_t::STATUS, command_t::NORMAL_MODE);
}

/**
 * @brief 			Send broad status
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::sendBroadStatus(void)
{
	sendCommand(message_id_t::BROAD_STATUS, command_t::NORMAL_MODE);
}

/**
 * @brief 			Check limits
 *
 * @param[in]	int16_t& command_unit_deg
 * @param[in]  int16_t const minimum_deg
 * @param[in]  int16_t const maximum_deg
 *
 * @return 		void
 */
void DS_VENUS::checkLimits(int16_t &command_unit_deg, int16_t const minimum_deg, int16_t const maximum_deg)
{
	int16_t maximum_unit_deg = uint16_t(maximum_deg * DEG_TO_IMU);
	int16_t minimum_unit_deg = uint16_t(minimum_deg * DEG_TO_IMU);

	if (command_unit_deg >= maximum_unit_deg)
	{
		command_unit_deg = maximum_unit_deg;
	}
	else if (command_unit_deg <= minimum_unit_deg)
	{
		command_unit_deg = minimum_unit_deg;
	}
}

/**
 * @brief 			Degree check limits
 *
 * @param[in]	float& command_deg
 * @param[in]  float minimum_deg
 * @param[in]  float maximum_deg
 *
 * @return 		void
 */
void DS_VENUS::checkLimitsDeg(float &command_deg, float minimum_deg, float maximum_deg)
{
	if (command_deg > maximum_deg)
	{
		command_deg -= 360.0;
	}
	else if (command_deg < minimum_deg)
	{
		command_deg += 360.0;
	}
}

/**
 * @brief 			Gimbal scheduler
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::scheduler(void)
{
	static bool first_pilot_mode_send = false;

	if (confirm_flag.bits.motor_enable_active == 0)
	{
		sendMotorEnable();
	}
	else
	{
		if (!first_pilot_mode_send)
		{
			sendPilotMode();
			first_pilot_mode_send = true;
			sendEncoderRawDataCommand(ref_encoder_command);
		}
		else
		{
			sendAngleInformation();
			checkDrift();
		}
	}
}

/**
 * @brief 			Look up command
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::lookUp(void)
{
	float angle_ref_deg = LOOK_UP_ANGLE_DEG - venus.angle_info_pkt.pitch_angle_deg;
	sendReference(mode_command_t::ANGLE_MODE, axis_t::PITCH, venus.speed_reference_dps, angle_ref_deg);
}

/**
 * @brief 			Look down command
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::lookDown(void)
{
	float angle_ref_deg = LOOK_DOWN_ANGLE_DEG - venus.angle_info_pkt.pitch_angle_deg;
	sendReference(mode_command_t::ANGLE_MODE, axis_t::PITCH, venus.speed_reference_dps, angle_ref_deg);
}

/**
 * @brief 			To control gimbal drift
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::checkDrift(void)
{
	static float true_north_angle_deg = 0.0;
	float yaw_angle_command_deg = 0.0;
	float lat_dist_m = 0.0;
	float lon_dist_m = 0.0;
	float geolock_pitch_command_deg = 0.0;
	float geolock_yaw_command_deg = 0.0;
	float geolock_true_north_deg = 0.0;
	float target_distance_m = 0.0;

	if (angle_info_pkt.yaw_imu_command == 0 && angle_info_pkt.yaw_target_imu != 0)
	{
		angle_info_pkt.yaw_imu_command = angle_info_pkt.yaw_target_imu;
	}

	if (status_flag.bits.geolock_active)
	{
		true_north_angle_deg = encoder_status_pkt.true_north_deg;

		if (fabs(angle_info_pkt.yaw_imu_command - angle_info_pkt.yaw_imu) < 200)
		{
			lat_dist_m = (geolock_latitude_uddeg - (float) parameters.gnss_latitude) * DEG_TO_M;
			lon_dist_m = (geolock_longitude_uddeg - (float) parameters.gnss_longitude) * DEG_TO_M * parameters.GPS_scaleLonDown;

			geolock_true_north_deg = atan2f(lon_dist_m, lat_dist_m) * RAD_TO_DEG;

			if (geolock_true_north_deg > 180.0)
			{
				geolock_true_north_deg -= 360.0;
			}
			else if (geolock_true_north_deg < -180.0)
			{
				geolock_true_north_deg += 360.0;
			}

			geolock_yaw_command_deg = geolock_true_north_deg - encoder_status_pkt.true_north_deg;

			if (fabs(geolock_yaw_command_deg) < 4.0)
			{
				geolock_yaw_command_deg = 0.0;
			}

			if (geolock_yaw_command_deg > 180.0)
			{
				geolock_yaw_command_deg -= 360.0;
			}
			else if (geolock_yaw_command_deg < -180.0)
			{
				geolock_yaw_command_deg += 360.0;
			}

			if (geolock_yaw_command_deg + encoder_status_pkt.encoder_yaw_angle_deg > (float) YAW_MAXIMUM_ANGLE_DEG)
			{
				geolock_yaw_command_deg = YAW_MINIMUM_ANGLE_DEG - encoder_status_pkt.encoder_yaw_angle_deg;
			}
			else if (geolock_yaw_command_deg + encoder_status_pkt.encoder_yaw_angle_deg < (float) YAW_MINIMUM_ANGLE_DEG)
			{
				geolock_yaw_command_deg = -1.0 * (encoder_status_pkt.encoder_yaw_angle_deg - YAW_MAXIMUM_ANGLE_DEG);
			}

			target_distance_m = parameters.altitude * cosf(geolock_true_north_deg * DEG_TO_RAD);
			if (target_distance_m != 0.0)
			{
				geolock_pitch_command_deg = atanf(lat_dist_m / target_distance_m) * RAD_TO_DEG - 90.0;
			}

			if (geolock_pitch_command_deg < -60.0)
			{
				geolock_pitch_command_deg = -60.0;
			}
			else if (geolock_pitch_command_deg > 25.0)
			{
				geolock_pitch_command_deg = 25.0;
			}

			geolock_pitch_command_deg = geolock_pitch_command_deg - angle_info_pkt.pitch_angle_deg;

			send2DCommand(geolock_pitch_command_deg, geolock_yaw_command_deg);
		}
	}
	else
	{
		if (encoder_zero_calculated)
		{
			if ((get_target_imu_unit_deg == false) && (fabs(angle_info_pkt.yaw_imu_command - angle_info_pkt.yaw_imu) < YAW_DRIFT_IMU_CHECK_LIMIT)
					&& (true_north_angle_deg != 0.0))
			{
				if (fabs(true_north_angle_deg - encoder_status_pkt.true_north_deg) > YAW_DRIFT_TRUE_NORTH_LIMIT)
				{
					yaw_angle_command_deg = true_north_angle_deg - encoder_status_pkt.true_north_deg;
					checkLimitsDeg(yaw_angle_command_deg, MINIMUM_ANGLE_DEG, MAXIMUM_ANGLE_DEG);
					sendErrorCorrection(yaw_angle_command_deg);
					true_north_angle_deg = encoder_status_pkt.true_north_deg;
				}
			}
			else
			{
				true_north_angle_deg = encoder_status_pkt.true_north_deg;
			}
		}
		else
		{
			true_north_angle_deg = 0.0;
		}
	}
}

/**
 * @brief 			send angle reference to axes
 *
 * @param[in]	mode_command_t roll_mode
 * @param[in] 	mode_command_t pitch_mode
 * @param[in]  mode_command_t yaw_mode
 * @param[in]  float command_roll_deg
 * @param[in]  float command_pitch_deg
 * @param[in]  float command_yaw_deg
 *
 * @return 		void
 */
void DS_VENUS::sendAngleReference(mode_command_t roll_mode, mode_command_t pitch_mode, mode_command_t yaw_mode, float command_roll_deg,
		float command_pitch_deg, float command_yaw_deg)
{
	control_command_pkt_t tr_control_command_pkt;
	int16_t temp_angle_ref_deg = 0;

	switch (roll_mode)
	{
		case mode_command_t::NO_CONTROL:
			break;

		case mode_command_t::SPEED_MODE:
			tr_control_command_pkt.speed_roll = (int16_t) command_roll_deg;
			break;

		case mode_command_t::ANGLE_MODE:
			tr_control_command_pkt.imu_roll = angle_info_pkt.roll_imu + (int16_t) (command_roll_deg * DEG_TO_IMU);
			break;

		case mode_command_t::SPEED_ANGLE_MODE:
			tr_control_command_pkt.speed_roll = (int16_t) speed_reference_dps;
			tr_control_command_pkt.imu_roll = angle_info_pkt.roll_imu + (int16_t) (command_roll_deg * DEG_TO_IMU);
			break;

		case mode_command_t::MODE_ANGLE_REL_FRAME:
			tr_control_command_pkt.imu_roll = (int16_t) (command_roll_deg * DEG_TO_IMU);
			break;

		default:
			break;
	}

	switch (pitch_mode)
	{
		case mode_command_t::NO_CONTROL:
			break;

		case mode_command_t::SPEED_MODE:
			tr_control_command_pkt.speed_pitch = (int16_t) command_pitch_deg;
			break;

		case mode_command_t::ANGLE_MODE:
			tr_control_command_pkt.imu_pitch = angle_info_pkt.pitch_imu + (int16_t) (command_pitch_deg * DEG_TO_IMU);
			break;

		case mode_command_t::SPEED_ANGLE_MODE:
			tr_control_command_pkt.speed_pitch = (int16_t) speed_reference_dps;
			tr_control_command_pkt.imu_pitch = angle_info_pkt.pitch_imu + (int16_t) (command_pitch_deg * DEG_TO_IMU);
			break;

		case mode_command_t::MODE_ANGLE_REL_FRAME:
			tr_control_command_pkt.imu_pitch = (int16_t) (command_pitch_deg * DEG_TO_IMU);
			break;

		default:
			break;
	}

	tr_control_command_pkt.control_mode_yaw = (uint8_t) yaw_mode;

	if (encoder_status_pkt.encoder_yaw_angle_deg + command_yaw_deg > YAW_MAXIMUM_ANGLE_DEG)
	{
		temp_angle_ref_deg = YAW_MAXIMUM_ANGLE_DEG - encoder_status_pkt.encoder_yaw_angle_deg;
	}
	else if (encoder_status_pkt.encoder_yaw_angle_deg + command_yaw_deg < YAW_MINIMUM_ANGLE_DEG)
	{
		temp_angle_ref_deg = YAW_MINIMUM_ANGLE_DEG - encoder_status_pkt.encoder_yaw_angle_deg;
	}
	else
	{
		temp_angle_ref_deg = command_yaw_deg;
	}

	switch (yaw_mode)
	{
		case mode_command_t::NO_CONTROL:
			break;

		case mode_command_t::SPEED_MODE:
			tr_control_command_pkt.speed_yaw = (int16_t) command_yaw_deg;
			break;

		case mode_command_t::ANGLE_MODE:
			tr_control_command_pkt.imu_yaw = angle_info_pkt.yaw_imu + (int16_t) (temp_angle_ref_deg * DEG_TO_IMU);
			break;

		case mode_command_t::SPEED_ANGLE_MODE:
			tr_control_command_pkt.speed_yaw = (int16_t) speed_reference_dps;
			tr_control_command_pkt.imu_yaw = angle_info_pkt.yaw_imu + (int16_t) (temp_angle_ref_deg * DEG_TO_IMU);
			break;

		case mode_command_t::MODE_ANGLE_REL_FRAME:
			tr_control_command_pkt.imu_yaw = (int16_t) (temp_angle_ref_deg * DEG_TO_IMU);
			break;

		default:
			break;
	}

	angle_info_pkt.yaw_imu_command = tr_control_command_pkt.imu_yaw;
	angle_info_pkt.encoder_yaw_angle_command_deg = encoder_status_pkt.encoder_yaw_angle_deg + temp_angle_ref_deg;

	checkLimits(tr_control_command_pkt.imu_roll, ROLL_MINIMUM_ANGLE_DEG, ROLL_MAXIMUM_ANGLE_DEG);
	checkLimits(tr_control_command_pkt.imu_pitch, PITCH_MINIMUM_ANGLE_DEG, PITCH_MAXIMUM_ANGLE_DEG);

	sendControlCommand(tr_control_command_pkt);
}

/**
 * @brief 			Geolock activate
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::activateGeolock(void)
{
	status_flag.bits.geolock_active = 1;

	geolock_latitude_uddeg = target_latitude_uddeg * 10000000.0;
	geolock_longitude_uddeg = target_longitude_uddeg * 10000000.0;
}

/**
 * @brief 			Geolock deactivate
 *
 * @param[in]	void
 *
 * @return 		void
 */
void DS_VENUS::deactivateGeolock(void)
{
	if (status_flag.bits.geolock_active)
	{
		status_flag.bits.geolock_active = 0;
	}
}

/**
 * @brief 			Look here command limits
 *
 * @param[in]	float pitch_command_deg_s32
 *	@param[in] 	float yaw_command_deg_s32
 *
 * @return 		void
 */
void DS_VENUS::lookHereLimits(float pitch_command_deg, float yaw_command_deg)
{
	if (yaw_command_deg > 180.0)
	{
		yaw_command_deg -= 360.0;
	}
	else if (yaw_command_deg < -180.0)
	{
		yaw_command_deg += 360.0;
	}

	yaw_command_deg = yaw_command_deg - venus.encoder_status_pkt.true_north_deg;

	if (yaw_command_deg > 180.0)
	{
		yaw_command_deg -= 360;
	}
	else if (yaw_command_deg < -180.0)
	{
		yaw_command_deg += 360;
	}

	if (yaw_command_deg + venus.encoder_status_pkt.encoder_yaw_angle_deg > 180.0)
	{
		yaw_command_deg -= 360.0;
	}
	else if (yaw_command_deg + venus.encoder_status_pkt.encoder_yaw_angle_deg < -180.0)
	{
		yaw_command_deg += 360.0;
	}

	if (pitch_command_deg < -60.0)
	{
		pitch_command_deg = -60.0;
	}
	else if (pitch_command_deg > 25.0)
	{
		pitch_command_deg = 25.0;
	}
}

/**
 * @brief 		Zoom Scale
 *
 * @param[in]	void
 *
 * @return 		void
 */
float DS_VENUS::zoomScale()
{
    float value = 5.0;

    if(zoom_info == 0 || zoom_info == 1)
    {
        value = 5.0;
    }
    else if(zoom_info >= 2 && zoom_info < 5)
    {
        value = 4.5;
    }
    else if(zoom_info >= 5 && zoom_info < 10)
    {
        value = 4.0;
    }
    else if(zoom_info >= 10 && zoom_info < 15)
    {
        value = 3.0;
    }
    else if(zoom_info >= 15 && zoom_info < 20)
    {
        value = 2.0;
    }
    else if(zoom_info >= 20 )
    {
        value = 1.0;
    }
    else
    {
        value = 5.0;
    }

    return value;
}

/**
 * @brief 		Trim Command
 *
 * @param[in]	float angle_ref_deg
 *
 * @return 		void
 */
void DS_VENUS::sendErrorCorrection(float angle_ref_deg)
{
    if(angle_ref_deg > 0.0)
    {
        trim_command -= trim_ref;
    }
    else if(angle_ref_deg < 0.0)
    {
        trim_command += trim_ref;
    }

    if(trim_command > 20000)
    {
        trim_command = 20000;
    }
    else if(trim_command < -20000)
    {
        trim_command = -20000;
    }

    uint8_t gnd_data[50] = {0};
    uint16_t idx = 0;
    uint8_t chk_sum_u8 = 0;

    gnd_data[0] = 0x3E;
    gnd_data[1] = 0x1F;
    gnd_data[2] = 0x06;

    for( idx = 1; idx < 3; idx++ )
    {
        chk_sum_u8 = chk_sum_u8 + gnd_data[ idx ];
    }

    gnd_data[3] = chk_sum_u8;

    chk_sum_u8 = 0;

    idx = 4;

    gnd_data[idx++] = 0x01;
    gnd_data[idx++] = 0x26;

    packet.parse_int32(&trim_command, gnd_data, &idx);

    for( idx = 0; idx < 6 ;idx++ )
    {
        chk_sum_u8 = chk_sum_u8 + gnd_data[ idx + 4 ];
    }

    gnd_data[10] = chk_sum_u8;

    //Uart.uartSendData(UART_NUMBER_4, gnd_data, 11);
}
