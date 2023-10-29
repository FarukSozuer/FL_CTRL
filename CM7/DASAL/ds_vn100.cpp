/**
 ******************************************************************************
  * @file		: ds_vn100.cpp
  * @brief		: VN100 Interface Class
  *				  This file contains vn100 interface class
  * @author		: Faruk Sozuer
  * @date		: 05.03.2020
  * @version	: 0.0.3
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


#include "ds_vn100.hpp"
#include <cstring>
#include <cstdio>
#include "ds_debug_tools.hpp"
#include "ds_parse.hpp"

#define ALBATROS
//#define ARIKUSU

//Sensors::Imu::VN100 imu(&uart1,
//						Sensors::Imu::GROUP_COMMON |
//						Sensors::Imu::GROUP_IMU    |
//						Sensors::Imu::GROUP_ATTITUDE,
//
//						Sensors::Imu::BINARY_GROUP_1_ACCEL |
//						Sensors::Imu::BINARY_GROUP_1_ANGULAR_RATE,
//
//						Sensors::Imu::BINARY_GROUP_2_SYNC_OUT_CNT |
//						Sensors::Imu::BINARY_GROUP_2_TIME_STARTUP,
//
//						Sensors::Imu::BINARY_GROUP_3_ANGULAR_RATE|
//						Sensors::Imu::BINARY_GROUP_3_MAG,
//
//						Sensors::Imu::BINARY_GROUP_4_VPE_STATUS |
//						Sensors::Imu::BINARY_GROUP_4_YPRU);

Sensors::Imu::VN100 imu(&uart6,Sensors::Imu::HZ100,Sensors::Imu::CHANNEL1,921600);

namespace Sensors
{
namespace Imu
{

/**
  * @brief Default constructor
  *
  * @param[in]  uart_module
  * @param[in]  baud_rate
  * @param[in]  bit_rate
  * @param[in]  serial_channel
  *
  * @return 	void Nothing
  */
VN100::VN100(Peripherals::Uart::H747_UART *uart_module,uint8_t bit_rate,uint8_t serial_channel,uint32_t baud_rate):
uart(uart_module),
vn100_bitRate(bit_rate),
vn100_serial_channel(serial_channel),
vn100_baudrate(baud_rate)
{
	// default init here
	group_header   = GROUP_COMMON;

	binary_group_1 = BINARY_GROUP_1_YPR          |
					 BINARY_GROUP_1_ANGULAR_RATE |
					 BINARY_GROUP_1_ACCEL		 |
					 BINARY_GROUP_1_MAG_PRESS    |
					 BINARY_GROUP_1_VPE_STATUS;

	binary_group_2 = BINARY_GROUP_2_NOT_USED;
	binary_group_3 = BINARY_GROUP_3_NOT_USED;
	binary_group_5 = BINARY_GROUP_5_NOT_USED;

}

/**
  * @brief Setting constructor
  *
  * @param[in]  uart_module
  * @param[in]  groupHeader
  * @param[in]  binary_group1
  * @param[in]  binary_group2
  * @param[in]  binary_group3
  * @param[in]  binary_group4
  *
  * @return 	void Nothing
  */
VN100::VN100(Peripherals::Uart::H747_UART *uart_module, uint8_t groupHeader,uint16_t binary_group1,uint16_t binary_group2,uint16_t binary_group3,uint16_t binary_group5):
uart(uart_module),
group_header(groupHeader),
binary_group_1(binary_group1),
binary_group_2(binary_group2),
binary_group_3(binary_group3),
binary_group_5(binary_group5)
{
}



/*
 * @brief
 *
 * @return
 */
void VN100::scheduler(void)
{
	static vn100_scheduler_state_type vn100_scheduler_state = vn100_scheduler_state_type::FIRST_START;

	switch (vn100_scheduler_state)
	{
		case vn100_scheduler_state_type::FIRST_START:
			if (initialize() == 1)
			{
				vn100_scheduler_state = vn100_scheduler_state_type::MEASURE;
			}
			else
			{
				vn100_scheduler_state = vn100_scheduler_state_type::FIRST_START;
			}
		break;

		case vn100_scheduler_state_type::MEASURE:
			getByte();
			vn100_scheduler_state = vn100_scheduler_state_type::MEASURE;
		break;

		default:
		break;
	}
}



/*
 * @brief Init function makes the necessary opening settings for the sensor. These settings
 * are to make the desired baudrate setting, turn off sync messages, open the related serial
 * port,set bitrate and determine what data will be received. It tries to make settings for
 * about 10 seconds.If it does not make the settings, it returns with the related error flags.
 * This function then waits another 10 seconds and tries to adjust again.
 *
 * @return imu_initialize: 1: Imu is okey.
 * 						   0: Imu is not okey.
 *
 * @warning The "system_error_code" flags are enabled in this function.
 */
uint8_t VN100::initialize(void)
{
	static uint8_t  next_state = 1;
	static uint8_t  state = 1;
	static uint8_t  baudrate_array_counter = 0;
	static uint8_t  time_out_counter = 0;
	static uint32_t last_ms = 0;
	static uint32_t delay_time_ms = 300;
	static uint32_t refresh_time = 0;

	switch (state)
	{
		case 0:			///< Wait state
			if ( monitor.getMillis() - last_ms > delay_time_ms)
			{
				state = next_state;
				time_out_counter++;
			}

			if (time_out_counter >= 200) ///< 200*50ms = 10sn trial period
			{
				time_out_counter = 0;
				state = 12;
			}
		break;

		case 1: 		///< close sync
				closeAsyncOutputConfig();
				responseController();
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				state = 0;
				next_state = 2;	// Kendi response kontrolune dallan.
		break;

		case 2: 		///< responseController
			if (responseController() == VN100_RESPONSE_TRUE)
			{
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				response = 0;
				state = 0;
				next_state = 3;	// bir sonraki sorgu state dallan
				uart->changeBaudRate(vn100_baudrate);
				time_out_counter = 0;		///< Timeout counter is reset on every correct answer.
			}
			else
			{
				uart->changeBaudRate(baudrate_array[baudrate_array_counter++]);
				if (baudrate_array_counter > 8)
				{
					baudrate_array_counter = 0;
				}
				system_error_code.bits.init_sync_error = 1;
				system_error_code.all = (response<<7) | 16;
				delay_time_ms = 500;
				last_ms = monitor.getMillis();
				state = 0;
				next_state = 1; //Kendi sorgusuna dallan.
				imu_initialize = 0;
			}
		break;

		case 3:			///< The sensor finds the baudrate and the processor configures the baud rate accordingly.
			sendBaudRate();
			delay_time_ms = 50;
			last_ms = monitor.getMillis();
			state = 0;
			next_state = 4; // kendi response kontrolune dallan.
		break;

		case 4:			///< responseController
			if (responseController() == VN100_RESPONSE_TRUE)
			{
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				response = 0;
				state = 0;
				next_state = 5;	//bir sonraki sorgu state dallan
				uart->changeBaudRate(vn100_baudrate);
				time_out_counter = 0;		///< Timeout counter is reset on every correct answer.
			}
			else
			{
				uart->changeBaudRate(baudrate_array[baudrate_array_counter++]);
				if (baudrate_array_counter > 8)
				{
					baudrate_array_counter = 0;
				}
				system_error_code.bits.baudrate_error = 1;
				system_error_code.all = (response<<7) | 2;
				delay_time_ms = 500;
				last_ms = monitor.getMillis();
				state = 0;
				next_state = 3; // Kendi sorgu statine dallan.
				imu_initialize = 0;
			}
		break;

		case 5:			///< openSerialPort
			openSerialPort();
			delay_time_ms = 50;
			last_ms = monitor.getMillis();
			state = 0;
			next_state = 6;
		break;

		case 6:			///< responseController
			if (responseController() == VN100_RESPONSE_TRUE)
			{
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				response = 0;
				state = 0;
				next_state = 7;
				time_out_counter = 0;		///< Timeout counter is reset on every correct answer.
			}
			else
			{
				system_error_code.bits.init_open_serial_error = 1;
				system_error_code.all = (response<<7) | 8;
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				state = 0;
				next_state = 5;
				imu_initialize = 0;
			}
		break;

		case 7:			///< settingBitrate
			settingBitrate();
			delay_time_ms = 20;
			last_ms = monitor.getMillis();
			state = 0;
			next_state = 8;
		break;

		case 8:			///< responseController
			if (responseController() == VN100_RESPONSE_TRUE)
			{
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				response = 0;
				state = 0;
				next_state = 9;
				time_out_counter = 0;		///< Timeout counter is reset on every correct answer.
			}
			else
			{
				system_error_code.bits.init_bitrate_error = 1;
				system_error_code.all = (response<<7) | 32;
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				state = 0;
				next_state = 7;
				imu_initialize  = 0;
			}
		break;

		case 9:			///<settingOutputGroup
			settingOutputGroup();
			delay_time_ms = 20;
			last_ms = monitor.getMillis();
			state = 0;
			next_state = 10;
		break;

		case 10:		///< responseController
			if (responseController() == VN100_RESPONSE_TRUE)
			{
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				response = 0;
				state = 0;
				next_state = 11;
				time_out_counter = 0;		///< Timeout counter is reset on every correct answer.
			}
			else
			{
				system_error_code.bits.init_output_group_error = 1;
				system_error_code.all = (response<<7) | 64;
				delay_time_ms = 50;
				last_ms = monitor.getMillis();
				state = 0;
				next_state = 9;
				imu_initialize = 0;
			}
		break;

		case 11:		// This state IMU initialize is okey.
				system_error_code.all = 0;
				imu_initialize = 1;
				next_state = 1;
				state = 0;
		break;

		case 12:
			system_error_code.bits.init_error = 1;
			system_error_code.bits.init_time_out_error = 1;
			imu_initialize = 0;
			refresh_time = monitor.getMillis();
			state = 13;
		break;

		case 13:   		///< Refresh initialize routine delay.
			if ( monitor.getMillis() - refresh_time > 10000)
			{
				state = 1;
				next_state = 1;
			}
		break;

		default:

		break;
	}
	return imu_initialize;
}



/**
  * @brief 		VN100 calculate checksum
  *
  * @param[in]  char buffer[]    : The buffer entry to be sent.
  * @param[in]  uint16_t length  : The buffer size you want to send.
  *
  * @return 	void	cksum
  *
  * Example:
  * @code
  * uint8_t check=0;
  * check = calculateChecksum(buffer, size);
  * @endcode
  */
uint8_t VN100::calculateChecksum(char buffer[], uint16_t length)
{
	uint16_t i;
	uint8_t cksum = 0;

	for(i=1; i<length; i++)
	{
	   cksum ^= buffer[i];
	}
	return cksum;
}



/**
  * @brief 	Sends the message packet according to the baudrate setting selected for the VN100.
  *
  * @return void	Nothing
  */
void VN100::sendBaudRate(void)
{
	char transmit_buffer[100] = {0};
	uint16_t length=0;
	uint8_t checksum=0;

	length = sprintf (transmit_buffer,"$VNWRG,05,%ld,%d",vn100_baudrate,vn100_serial_channel);
	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);
}



/**
  * @brief 	Transfers a byte from the corresponding serial line.
  *
  * @return void	Nothing
  */
void VN100::closeAsyncOutputConfig(void)
{
	char transmit_buffer[100] = {0};
	uint16_t length=0;
	uint8_t checksum=0;

	length = sprintf (transmit_buffer,"$VNWRG,06,0,%d",vn100_serial_channel);
	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);
}



/**
  * @brief 	Transfers a byte from the corresponding serial line.
  *
  * @return void	Nothing
  */
void VN100::openSerialPort(void)
{
	char transmit_buffer[100] = {0};
	uint16_t length = 0;
	uint8_t checksum=0;

	if (vn100_serial_channel == CHANNEL1)
	{
		length = sprintf (transmit_buffer,"$VNWRG,75,%d,0,00",vn100_serial_channel);
	}
	else if (vn100_serial_channel == CHANNEL2)
	{
		length = sprintf (transmit_buffer,"$VNWRG,76,%d,0,00",vn100_serial_channel);
	}

	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);
}



/**
  * @brief 	Transfers a byte from the corresponding serial line.
  *
  * @return void	Nothing
  */
void VN100::settingBitrate(void)
{
	char transmit_buffer[100] = {0};

	uint16_t length = 0;
	uint8_t checksum=0;

	if (vn100_serial_channel == CHANNEL1)
	{
		length = sprintf (transmit_buffer,"$VNWRG,75,%d,%d,00",vn100_serial_channel,vn100_bitRate);
	}
	else if (vn100_serial_channel == CHANNEL2)
	{
		length = sprintf (transmit_buffer,"$VNWRG,76,%d,%d,00",vn100_serial_channel,vn100_bitRate);
	}

	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);
}



/*
 * @brief Function that determines which group messages to open. See all detail VN-100 User Manual Page 34 Tittle 4.1 Available Output Types
 *
 * @return 	void	Nothing
 */
void VN100::settingOutputGroup(void)
{
	char transmit_buffer[100] = {0};
	uint8_t checksum = 0;
	uint16_t length = 0;
	uint8_t group_header_input = 0;

	group_header_input = group_header;

	if (vn100_serial_channel == CHANNEL1)
	{
		length = sprintf (transmit_buffer,"$VNWRG,75,%d,%d,%x,",vn100_serial_channel,vn100_bitRate,group_header);
	}
	else if (vn100_serial_channel == CHANNEL2)
	{
		length = sprintf (transmit_buffer,"$VNWRG,76,%d,%d,%x,",vn100_serial_channel,vn100_bitRate,group_header);
	}

	if( (group_header_input & GROUP_COMMON ) > 0 )
	{
		length += sprintf (&transmit_buffer[length],"%x,",binary_group_1);
	}
	if( (group_header_input & GROUP_TIME) > 0 )
	{
		length += sprintf (&transmit_buffer[length],"%x,",binary_group_2);
	}
	if( (group_header_input & GROUP_IMU) > 0 )
	{
		length += sprintf (&transmit_buffer[length],"%x,",binary_group_3);
	}
	if( (group_header_input & GROUP_ATTITUDE) > 0 )
	{
		length += sprintf (&transmit_buffer[length],"%x",binary_group_5);
	}
	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);
}



/**
  * @brief 	Transfers a byte from the corresponding serial line.
  *
  * @return void	Nothing
  */
void VN100::sendData(char buffer[],uint16_t size)
{
	uart->sendData((uint8_t *)buffer,size);
}



/*
 * @brief Function that tests the accuracy of the message sent while the sensor's init settings are being made.
*
* | PREAMBLE  | FIRST_INDEX		| SECOND_INDEX	| PAYLOAD	| PARSE
* | $         | V    			| N 			| -- 		| --
*
 * @return response
 **/
uint8_t VN100::responseController(void)
{
	static uint8_t payload_buffer[1024] = {0};
	static vn100_response_parse_type response_parse_status = vn100_response_parse_type::PREAMBLE;
	static uint16_t read_incoming_index = 0;
	static uint8_t payload_counter = 0;
	static uint8_t payload_length = 0;
	static uint8_t buffer[1024] = {0};
	static uint16_t size = 0;

	size = uart->getDataFromBuffer(buffer);

	if (size >0)
	{
		for (read_incoming_index = 0; read_incoming_index < size;read_incoming_index++)
		{
			switch (response_parse_status)
			{
				case vn100_response_parse_type::PREAMBLE:
					if (buffer[read_incoming_index] == '$')
					{
						response_parse_status = vn100_response_parse_type::FIRST_INDEX;
					}
					else
					{
						response_parse_status = vn100_response_parse_type::PREAMBLE;
						response = VN100_RESPONSE_FALSE;
					}
				break;

				case  vn100_response_parse_type::FIRST_INDEX:

					if (buffer[read_incoming_index] == 'V')
					{
						response_parse_status = vn100_response_parse_type::SECOND_INDEX;

					}
					else
					{
						response_parse_status = vn100_response_parse_type::PREAMBLE;
						response = VN100_RESPONSE_FALSE;
					}
				break;

				case  vn100_response_parse_type::SECOND_INDEX:

					if (buffer[read_incoming_index] == 'N')
					{
						response_parse_status = vn100_response_parse_type::PAYLOAD;
					}
					else
					{
						response_parse_status = vn100_response_parse_type::PREAMBLE;
						response = VN100_RESPONSE_FALSE;
					}
				break;

				case vn100_response_parse_type::PAYLOAD:
					payload_length = size-read_incoming_index;

					for (payload_counter=0; payload_counter<payload_length;payload_counter++)
					{
						payload_buffer[payload_counter] = buffer[read_incoming_index+payload_counter];
					}
					response_parse_status = vn100_response_parse_type::PARSE;
				break;

				case vn100_response_parse_type::PARSE:

					response_parse_status = vn100_response_parse_type::PREAMBLE;

					if ((payload_buffer[0] == 'E') && (payload_buffer[1] == 'R') && (payload_buffer[2] == 'R') && (payload_buffer[3] == ','))
					{
						response = ((payload_buffer[4]-48) + (payload_buffer[5]-48)); // -48 = ASCII to decimal
						read_incoming_index = 0;
						size = 0;
					}
					else if ((payload_buffer[0] == 'W') && (payload_buffer[1] == 'R') && (payload_buffer[2] == 'G') && (payload_buffer[3] == ','))
					{
						response = VN100_RESPONSE_TRUE;
						read_incoming_index = 0;
						size = 0;
					}
					else if ((payload_buffer[0] == 'R') && (payload_buffer[1] == 'R') && (payload_buffer[2] == 'G') && (payload_buffer[3] == ','))
					{
						response = VN100_RESPONSE_TRUE;
						read_incoming_index = 0;
						size = 0;

						if (payload_buffer[5] == '2')
						{
							sensor_info.hardware_rev = payload_buffer[6] - 48;
						}
					}
				break;

				default:
					response_parse_status = vn100_response_parse_type::PREAMBLE;
				break;
			}
		}
	}
	return response;
}

/*
 * @brief
 *
 * @return void	Nothing
 */
void VN100::getByte(void)
{
	uint16_t	size = 0;
	uint16_t	read_incoming_index		 = 0;
	buffer_container_type buffer;

	static vn100_parser_status_type  parse_state = vn100_parser_status_type::PREAMBLE;
	static buffer_container_type 	 payload;

	size = uart->getDataFromBuffer(buffer.data);

	if (size > 0)
	{
		for(read_incoming_index=0;read_incoming_index<size;read_incoming_index++)
		{
			switch (parse_state)
			{
				case vn100_parser_status_type::PREAMBLE:
					if (buffer.data[read_incoming_index] == SYNC_MESSAGE)
					{
						parse_state = vn100_parser_status_type::GROUP;
						payload.length  		  = 0;
						payload.crc_counter   	  = 0;
						payload.received_checksum = 0;
						payload.checksum 		  = 0;
						payload.counter 	      = 0;
						for (uint16_t t=0;t<1024;t++)
						{
							payload.data[t] = 0x00;
						}
					}
				break;

				case vn100_parser_status_type::GROUP:
					if((buffer.data[read_incoming_index] >= 0x01) && (buffer.data[read_incoming_index] <= 0x17))	// Total values that can be taken are between 1 and 23.
					{
						msg_group_header.all = buffer.data[read_incoming_index];
						parse_state = vn100_parser_status_type::COMMON_GROUP_LOW;
						payload.crc_counter++;
					}
					else
					{
						parse_state = vn100_parser_status_type::PREAMBLE;
					}
				break;

				case vn100_parser_status_type::COMMON_GROUP_LOW:
					if (msg_group_header.bits.common == 1)
					{
						common_group_msg.all = (static_cast<uint16_t>(buffer.data[read_incoming_index]));
						parse_state = vn100_parser_status_type::COMMON_GROUP_HIGH;
						payload.crc_counter++;
					}
					else
					{
						parse_state = vn100_parser_status_type::TIME_GROUP_LOW;
					}
				break;

				case vn100_parser_status_type::COMMON_GROUP_HIGH:
					common_group_msg.all += (static_cast<uint16_t>(buffer.data[read_incoming_index]) << 8 );

					payload.length += calculatedPayloadLength(common_group_msg.all,0);
					payload.crc_counter++;
					if ( (msg_group_header.bits.time == 0) && (msg_group_header.bits.imu == 0) && (msg_group_header.bits.attitude == 0) )
					{
						parse_state = vn100_parser_status_type::PAYLOAD_CONTAINER;
					}
					else
					{
						parse_state = vn100_parser_status_type::TIME_GROUP_LOW;
					}

				break;

				case vn100_parser_status_type::TIME_GROUP_LOW:

					if (msg_group_header.bits.time == 1)
					{
						time_group_msg.all = (static_cast<uint16_t> ( buffer.data[read_incoming_index]));
						parse_state = vn100_parser_status_type::TIME_GROUP_HIGH;
						payload.crc_counter++;
					}
					else
					{
						parse_state = vn100_parser_status_type::IMU_GROUP_LOW;
					}
				break;

				case vn100_parser_status_type::TIME_GROUP_HIGH:
					time_group_msg.all += (static_cast<uint16_t>(buffer.data[read_incoming_index]) << 8 );
					payload.crc_counter++;
					payload.length += calculatedPayloadLength(time_group_msg.all,1);

					if ( (msg_group_header.bits.imu == 0) && (msg_group_header.bits.attitude == 0) )
					{
						parse_state = vn100_parser_status_type::PAYLOAD_CONTAINER;
					}
					else
					{
						parse_state = vn100_parser_status_type::IMU_GROUP_LOW;
					}
				break;

				case vn100_parser_status_type::IMU_GROUP_LOW:

					if (msg_group_header.bits.imu == 1)
					{
						imu_group_msg.all = (static_cast<uint16_t> ( buffer.data[read_incoming_index]));
						parse_state = vn100_parser_status_type::IMU_GROUP_HIGH;
						payload.crc_counter++;
					}
					else
					{
						parse_state = vn100_parser_status_type::ATTITUDE_GROUP_LOW;
					}

				break;

				case vn100_parser_status_type::IMU_GROUP_HIGH:
					imu_group_msg.all += (static_cast<uint16_t> ( buffer.data[read_incoming_index])<<8);
					payload.crc_counter++;
					payload.length += calculatedPayloadLength(imu_group_msg.all,2);

					if (msg_group_header.bits.attitude == 0)
					{
						parse_state = vn100_parser_status_type::PAYLOAD_CONTAINER;
					}
					else
					{
						parse_state = vn100_parser_status_type::ATTITUDE_GROUP_LOW;
					}
				break;

				case vn100_parser_status_type::ATTITUDE_GROUP_LOW:
					if (msg_group_header.bits.attitude == 2)
					{
						attiude_group_msg.all = (static_cast<uint16_t> (buffer.data[read_incoming_index]));
						parse_state = vn100_parser_status_type::ATTITUDE_GROUP_HIGH;
						payload.crc_counter++;
					}
					else
					{
						parse_state = vn100_parser_status_type::PAYLOAD_CONTAINER;
					}
				break;

				case vn100_parser_status_type::ATTITUDE_GROUP_HIGH:
					attiude_group_msg.all += (static_cast<uint16_t> (buffer.data[read_incoming_index])<<8);
					payload.crc_counter++;
					payload.length += calculatedPayloadLength(attiude_group_msg.all,3);

					parse_state = vn100_parser_status_type::PAYLOAD_CONTAINER;
				break;


				case vn100_parser_status_type::PAYLOAD_CONTAINER:

					payload.data[payload.counter] = buffer.data[read_incoming_index];
					payload.counter++;
					payload.crc_counter++;
					if ( payload.counter >= payload.length )
					{
						parse_state = vn100_parser_status_type::CRC_LOW;
					}
				break;

				case vn100_parser_status_type::CRC_LOW:
					payload.received_checksum =  (static_cast<uint16_t>( buffer.data[read_incoming_index])<<8);
					parse_state = vn100_parser_status_type::CRC_CHECK;
				break;

				case vn100_parser_status_type::CRC_CHECK:
					payload.received_checksum +=  (static_cast<uint16_t>( buffer.data[read_incoming_index]));

					if (payload.received_checksum == (calculateCRC(buffer.data, (payload.crc_counter))) )
					{
						processReceivedPacket(payload);
					}
					else
					{
						parse_state = vn100_parser_status_type::PREAMBLE;
					}
					parse_state = vn100_parser_status_type::PREAMBLE;
			    break;

				default:
					parse_state = vn100_parser_status_type::PREAMBLE;
				break;
			}
		}
	}
}

/*
 * Bu kisimda harika bir algoritma yaz. bg1-bg5 amelasyonundan kurtulmaya calis.
 */
void VN100::processReceivedPacket(buffer_container_type &payload)
{
	uint16_t read_index = 0;

	if (msg_group_header.bits.common == 1)
	{
		if (common_group_msg.bits.time_startup == 1)
		{
			packet.add_uint64(&output_raw_data.time_startup, payload.data, &read_index);
		}
		if (common_group_msg.bits.time_sync_in == 1)
		{
			packet.add_uint64(&output_raw_data.time_sync_in, payload.data, &read_index);
		}
		if (common_group_msg.bits.ypr == 1)
		{

			packet.add_float(&output_raw_data.ypr.yaw, payload.data, &read_index);
			packet.add_float(&output_raw_data.ypr.pitch, payload.data, &read_index);
			packet.add_float(&output_raw_data.ypr.roll, payload.data, &read_index);
		}
		if (common_group_msg.bits.queternion == 1)
		{
			packet.add_float(&output_raw_data.quaternion.qtn_1, payload.data, &read_index);
			packet.add_float(&output_raw_data.quaternion.qtn_2, payload.data, &read_index);
			packet.add_float(&output_raw_data.quaternion.qtn_3, payload.data, &read_index);
			packet.add_float(&output_raw_data.quaternion.qtn_4, payload.data, &read_index);
		}

		if (common_group_msg.bits.angular_rate == 1)
		{
			packet.add_float(&output_raw_data.angular_rate_rs.body_x_axis_rs, payload.data, &read_index);
			packet.add_float(&output_raw_data.angular_rate_rs.body_y_axis_rs, payload.data, &read_index);
			packet.add_float(&output_raw_data.angular_rate_rs.body_z_axis_rs, payload.data, &read_index);
		}

		if (common_group_msg.bits.accel == 1)
		{
			packet.add_float(&output_raw_data.accel_ned.north_axis_ms, payload.data, &read_index);
			packet.add_float(&output_raw_data.accel_ned.east_axis_ms, payload.data, &read_index);
			packet.add_float(&output_raw_data.accel_ned.down_axis_ms, payload.data, &read_index);
		}

		if (common_group_msg.bits.imu == 1)
		{
			packet.add_float(&output_raw_data.imu.body_x_axis_accel, payload.data, &read_index);
			packet.add_float(&output_raw_data.imu.body_y_axis_accel, payload.data, &read_index);
			packet.add_float(&output_raw_data.imu.body_z_axis_accel, payload.data, &read_index);
			packet.add_float(&output_raw_data.imu.body_x_axis_rate_rs, payload.data, &read_index);
			packet.add_float(&output_raw_data.imu.body_y_axis_rate_rs, payload.data, &read_index);
			packet.add_float(&output_raw_data.imu.body_z_axis_rate_rs, payload.data, &read_index);
		}

		if (common_group_msg.bits.magpres == 1)
		{
			packet.add_float(&output_raw_data.mag_pres.mag_x, payload.data, &read_index);
			packet.add_float(&output_raw_data.mag_pres.mag_y, payload.data, &read_index);
			packet.add_float(&output_raw_data.mag_pres.mag_z, payload.data, &read_index);
			packet.add_float(&output_raw_data.mag_pres.temp, payload.data, &read_index);
			packet.add_float(&output_raw_data.mag_pres.pres, payload.data, &read_index);
		}

		if (common_group_msg.bits.delta_theta == 1)
		{
			packet.add_float(&output_raw_data.delta_theta_vel.dtime, payload.data, &read_index);
			packet.add_float(&output_raw_data.delta_theta_vel.dtheta_0, payload.data, &read_index);
			packet.add_float(&output_raw_data.delta_theta_vel.dtheta_1, payload.data, &read_index);
			packet.add_float(&output_raw_data.delta_theta_vel.dtheta_2, payload.data, &read_index);
			packet.add_float(&output_raw_data.delta_theta_vel.dvel_0, payload.data, &read_index);
			packet.add_float(&output_raw_data.delta_theta_vel.dvel_1, payload.data, &read_index);
			packet.add_float(&output_raw_data.delta_theta_vel.dvel_2, payload.data, &read_index);
		}

		if (common_group_msg.bits.vpe_status == 1)
		{
			packet.add_uint16(&output_raw_data.imu_status, payload.data, &read_index);

		}
	}

//	if (msg_group_header.bits.time == 1)
//	{
//			if (time_group_msg.bits.time_startup == 1)
//			{
//				 packet.add_uint64(&output_raw_data.time_startup,payload.data,&read_index);
//			}
//
//			if (time_group_msg.bits.time_sync_in == 1)
//			{
//				packet.add_uint64(&output_raw_data.time_sync_in,payload.data,&read_index);
//			}
//
//			if (time_group_msg.bits.sync_in_cnt == 1)
//			{
//				packet.add_uint32(&output_raw_data.sync_in_cnt,payload.data,&read_index);
//			}
//
//			if (time_group_msg.bits.sync_out_cnt == 1)
//			{
//				packet.add_uint32(&output_raw_data.sync_out_cnt,payload.data,&read_index);
//			}
//
//			if (time_group_msg.bits.time_status == 1)
//			{
//				packet.add_uint16(&output_raw_data.imu_status,payload.data,&read_index); // Todo: Bu mesaj paketi guncellenmis olabilir.
//			}
//	}
//
//	if (msg_group_header.bits.imu == 1)
//	{
//
//	}
//	if (msg_group_header.bits.attitude == 1)
//	{
//
//	}
}


uint16_t VN100::calculatedPayloadLength(uint16_t value,uint8_t group_index)
{
	uint16_t old_value;
	uint16_t new_value = value;

	uint16_t kalan =  0;
	uint16_t length = 0;
	if (value == 1)
	{
		length += BinaryPacketGroupLengths[group_index][0];
	}
	else
	{
		for (uint8_t index=1; index<16;index++)
		{
			old_value = new_value/2;
			new_value = old_value;
			if (old_value == 1)
			{
				length += BinaryPacketGroupLengths[group_index][index];
			}
			else
			{
				new_value = old_value;
				kalan = old_value%2;
				if (kalan == 1)
				{
					length += BinaryPacketGroupLengths[group_index][index];
				}
			}
		}
	}
	return length;
}



/*
 * @brief
 *
 * @return void	Nothing
 */
void VN100::baseInitialize(void)
{
	uint16_t trial = 0;
	uint16_t size = 0;

	uint8_t buffer[1024]= {0};

	for (trial=0; trial<=10; trial++)
	{
		monitor.delay_ms(100);
		closeAsyncOutputConfig();
		size = uart->getDataFromBuffer(buffer);
	}
}



/*
 * @brief
 *
 * @return response
 */
uint8_t VN100::getModel(void)
{
	char transmit_buffer[100] = {0};
	uint16_t length = 0;
	uint8_t response = 0;
	uint8_t checksum=0;

	length = sprintf (transmit_buffer,"$VNRRG,1");

	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);

	response = responseController();
	return response;
}



/*
 * @brief
 *
 * @return response
 */
uint8_t VN100::getHardwareRevision(void)
{
	char transmit_buffer[100] = {0};
	uint16_t length = 0;
	uint8_t response = 0;
	uint8_t checksum=0;

	length = sprintf (transmit_buffer,"$VNRRG,2");

	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);

	response = responseController();
	return response;
}



/*
 * @brief
 *
 * @return response
 */
uint8_t VN100::getSerialNumber(void)
{
	char transmit_buffer[100] = {0};
	uint8_t response = 0;
	uint16_t length = 0;
	uint8_t checksum=0;

	length = sprintf (transmit_buffer,"$VNRRG,3");

	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);

	response = responseController();
	return  response;
}



/*
 * @brief
 *
 * @return response
 */
uint8_t VN100::getFirmwareVersion(void)
{
	char transmit_buffer[100] = {0};
	uint8_t response = 0;
	uint16_t length = 0;
	uint8_t checksum=0;

	length = sprintf (transmit_buffer,"$VNRRG,4");

	checksum = calculateChecksum(transmit_buffer,length);
	length += sprintf (&transmit_buffer[length],"*%x\r\n",checksum);
	sendData(transmit_buffer, length);

	response = responseController();
	return response;
}



/*
 * @brief CRC calculation required for VN100. Source: VN-100 User Manual Page 32  Title 3.9.3
 *
 * @param[in] Package to be calculated.
 *
 * @length[in] Package to be calculated byte size.
 *
 * @return  crc
 *
 * Example:
 * @code
 * uint16_t crc = 0;
 * crc = calculateCRC(buffer,size);
 * @endcode
 */
uint16_t VN100::calculateCRC(uint8_t data[],uint16_t length)
{
	uint16_t i;
	uint16_t crc  =0;

	for (i=1;i<=length;i++)
	{
		crc = (uint8_t)(crc>>8) | (crc<<8);
		crc ^= data[i];
		crc ^= (uint8_t)(crc & 0xff) >> 4;
		crc ^= crc << 12;
		crc ^= (crc & 0x00ff) << 5;
	}
	return crc;
}


/*
 * @brief Some messages sent in VM100 are of ASCII character type. CRC calculation for array necessary for their calculation.
 *
 * @param[in] Package to be calculated. (ASCII Char Array)
 *
 * @length[in] Package to be calculated byte size.
 *
 * @return  crc
 */
unsigned short VN100::calculateChar_CRC(char  data[], uint8_t length)
{
	uint8_t i;
	unsigned short crc = 0;

	for(i=1; i<length; i++)
	{
		crc = (unsigned char)(crc >> 8) | (crc << 8);
		crc ^= data[i];
		crc ^= (unsigned char)(crc & 0xff) >> 4;
		crc ^= crc << 12;
		crc ^= (crc & 0x00ff) << 5;
	}
	return crc;
}



/*
 * @brief The function from which gyro bias are taken.
 *
 * @return void Nothing
 */
void VN100::setGyroBias(void)
{
	char transmitBuffer[23] = {0};
	char checksum_Buffer[18] = {0};

	uint8_t checksum=0;

	sprintf (checksum_Buffer,"VNSGB");
	checksum = calculateChecksum(checksum_Buffer,strlen(checksum_Buffer));

	sprintf (transmitBuffer,"$VNSGB*%x\r\n",checksum);
	sendData(transmitBuffer, strlen(transmitBuffer));
}



/*
 * @brief The function where the angular values taken by the sensor on the 3x3 axis are entered
 * 			  according to the position where the sensor stops.
 *
 * @return void Nothing
 */
void VN100::setReferanceFrame(float x1, float x2, float x3, float y1, float y2, float y3, float z1, float z2, float z3)
{
	char transmit_buffer[255] = {0};
	uint16_t length = 0;
	unsigned short checksum=0;

	length = sprintf (transmit_buffer,"$VNWRG,26,%+.6f,%+.6f,%+.6f,%+.6f,%+.6f,%+.6f,%+.6f,%+.6f,%+.6f",x1,x2,x3,y1,y2,y3,z1,z2,z3);
	checksum = calculateChar_CRC(transmit_buffer,length);

	length += sprintf (&transmit_buffer[length],"*%.4x\r\n",checksum);
	sendData(transmit_buffer, length);
}



/*
 * @brief
 *
 * @return void Nothing
 */
void VN100::getHeadingCompass(void)
{
	float mag_x_uT = 0;
	float mag_y_uT = 0;

#ifdef ARIKUSU
	float mag_z_uT = 0;
	float compound_vector = 0;
#endif

#ifdef ALBATROS
	mag_x_uT = (output_raw_data.mag_pres.mag_x*1000.0);
	mag_y_uT = (output_raw_data.mag_pres.mag_y*1000.0);

	heading.magnetic_north_2d = (atan2(mag_y_uT, mag_x_uT) * 180.0) / M_PI_C;
	 // Normalize to 0-360
	 if (heading.magnetic_north_2d < 0)
	 {
		 heading.magnetic_north_2d = 360 + heading.magnetic_north_2d;
	 }
#endif

#ifdef ARIKUSU
		mag_x_uT = (output_raw_data.mag_pres.mag_x*1000.0);
		mag_y_uT = (output_raw_data.mag_pres.mag_y*1000.0);
		mag_z_uT = (output_raw_data.mag_pres.mag_z*1000.0);

		compound_vector = sqrt( (mag_x_uT*mag_x_uT) + (mag_y_uT * mag_y_uT) * (mag_z_uT * mag_z_uT) );


#endif
}



/*
 * @brief conversion of some parameters to rad/s at m/s.
 *
 * @return void Nothing
 */
void VN100::conversionRadToMs(void)
{
	output_raw_data.angular_rate_ms.body_x_axis_ms = static_cast<double> (output_raw_data.angular_rate_rs.body_x_axis_rs/(M_PI_C/180.0));
	output_raw_data.angular_rate_ms.body_y_axis_ms = static_cast<double> (output_raw_data.angular_rate_rs.body_y_axis_rs/(M_PI_C/180.0));
	output_raw_data.angular_rate_ms.body_z_axis_ms = static_cast<double> (output_raw_data.angular_rate_rs.body_z_axis_rs/(M_PI_C/180.0));

	output_raw_data.imu_ms.body_x_axis_rate_ms = static_cast<double> (output_raw_data.imu.body_x_axis_rate_rs/(M_PI_C/180.0));
	output_raw_data.imu_ms.body_y_axis_rate_ms = static_cast<double> (output_raw_data.imu.body_y_axis_rate_rs/(M_PI_C/180.0));
	output_raw_data.imu_ms.body_z_axis_rate_ms = static_cast<double> (output_raw_data.imu.body_z_axis_rate_rs/(M_PI_C/180.0));
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
VN100::VN100(const VN100& orig)
{

}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
VN100::~VN100()
{

}

} // End of namespace Vn100
} // End of namespace Imu
