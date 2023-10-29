/**
 ******************************************************************************
  * @file		  : ds_gnss.cpp
  * @brief		: GNSS Interface Class
  *				  	  This file contains vn100 interface class
  * @author		: Faruk Sozuer
  * @date		  : 05.03.2020
  * @version	: 0.0.1
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


#include "ds_gnss.hpp"
#include <cstring>
#include <cstdio>
#include "ds_debug_tools.hpp"



Sensors::Gnss::ZEDF9P gnss(&uart4);

namespace Sensors
{
namespace Gnss
{

/**
  * @brief Default constructor
  *
  * @param[in]  uart_module
  *
  * @return 	void Nothing
  */
ZEDF9P::ZEDF9P(Peripherals::Uart::H747_UART *uart_module):
uart(uart_module)
{

}


/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
ZEDF9P::ZEDF9P(const ZEDF9P& orig)
{

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
uint16_t ZEDF9P::calculateCRC(uint8_t data[],uint16_t length)
{
	uint16_t i;
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	uint16_t crc=0;

	for (i=2;i<=(length-1);i++)
	{
		ck_a = (uint8_t) (ck_a+data[i]);
		ck_b = (uint8_t) (ck_b+ck_a);
	}
	crc = static_cast<uint16_t> ((ck_b<<8) | (ck_a));
	return crc;
}



/*
 * @brief
 *
 * @return void	Nothing
 */
void ZEDF9P::getByte(void)
{
	uint16_t size = 0;
	uint8_t buffer[2048] = {0};

	size = uart->getDataFromBuffer(buffer);

	if (size > 0)
	{
		getData(buffer,size);
	}
}



/*
 * @brief Function that extracts incoming packets if the sensor's setup is correct. Branches here from the getByte function.
 *
 * @param[in] buffer: Incoming packets.
 * @param[in]	size  :	Incoming packet byte size.
 * @return void	Nothing
 */
void ZEDF9P::getData(uint8_t buffer[],uint16_t size)
{
	static zedf9p_parse_status_type parse_status = zedf9p_parse_status_type::PREAMBLE1;
	static uint16_t read_incoming_index = 0;

	calculated_crc = 0;
	incoming_crc = 0;
	parse_message_type = zedf9_parse_message_type::NULL_VALUE;
	payload_length = 0;

	for (read_incoming_index = 0; read_incoming_index < size; read_incoming_index++)
	{
			switch (parse_status)
			{
				case zedf9p_parse_status_type::PREAMBLE1:

					if (buffer[read_incoming_index] == PREAMBLE1)
					{
						parse_status = zedf9p_parse_status_type::PREAMBLE2;
					}
					else
					{
						parse_status = zedf9p_parse_status_type::PREAMBLE1;
					}

				break;

				case zedf9p_parse_status_type::PREAMBLE2:

					if (buffer[read_incoming_index] == PREAMBLE2)
					{
						parse_status = zedf9p_parse_status_type::CLASS;
					}
					else
					{
						parse_status = zedf9p_parse_status_type::PREAMBLE1;
					}

				break;

				case zedf9p_parse_status_type::CLASS:

					if (buffer[read_incoming_index] == UBX_NAV_CLASS)
					{
						parse_status = zedf9p_parse_status_type::MSG_ID;
					}
					else
					{
						parse_status = zedf9p_parse_status_type::PREAMBLE1;
					}

				break;

				case zedf9p_parse_status_type::MSG_ID:

					if (buffer[read_incoming_index] == UBX_NAV_HPPOSLLH)
					{
						parse_message_type = zedf9_parse_message_type::UBX_NAV_HPPOSLLH;
						parse_status = zedf9p_parse_status_type::PAYLOAD_LENGTH;
					}

					else if (buffer[read_incoming_index] == UBX_NAV_PVT)
					{
						parse_message_type = zedf9_parse_message_type::UBX_NAV_PVT;
						parse_status = zedf9p_parse_status_type::PAYLOAD_LENGTH;
					}

					else if (buffer[read_incoming_index] == UBX_NAV_RELPOSNED)
					{
						parse_message_type = zedf9_parse_message_type::UBX_NAV_RELPOSNED;
						parse_status = zedf9p_parse_status_type::PAYLOAD_LENGTH;
					}

					else
					{

						parse_status = zedf9p_parse_status_type::PREAMBLE1;
					}


				break;

				case zedf9p_parse_status_type::PAYLOAD_LENGTH:

					if ((buffer[read_incoming_index] >=1) && (buffer[read_incoming_index] <= 255))
					{
						payload_length = buffer[read_incoming_index];
						parse_status = zedf9p_parse_status_type::CHECKSUM;
					}
					else
					{
						parse_status = zedf9p_parse_status_type::PREAMBLE1;
						payload_length = 0;
					}

				break;

				case zedf9p_parse_status_type::CHECKSUM:

				  //incoming_crc = (uint16_t)((buffer[(size-1)]<<8) | (buffer[size-2]));
				  incoming_crc = (uint16_t)((buffer[(payload_length+7)]<<8) | (buffer[payload_length+6]));

				  calculated_crc = calculateCRC(buffer, (payload_length+6));
				  //calculated_crc = calculateCRC(buffer, (size-2));

					if (calculated_crc == incoming_crc)
					{
						parse_status = zedf9p_parse_status_type::PARSE;
					}
					else
					{
						parse_status = zedf9p_parse_status_type::PREAMBLE1;
						size = 0;
						read_incoming_index = 0;
						calculated_crc = 0;
						incoming_crc = 0;
					}
				break;

				case zedf9p_parse_status_type::PARSE:

					if (parse_message_type == zedf9_parse_message_type::UBX_NAV_HPPOSLLH)
					{
						std::memcpy(&high_preision_geodic_pos.buffer[0],&buffer[read_incoming_index],UBX_NAV_HPPOSLLH_LENGTH);
						//read_incoming_index = read_incoming_index + UBX_NAV_HPPOSLLH_LENGTH;
						read_incoming_index = read_incoming_index + payload_length;

						parse_status = zedf9p_parse_status_type::PREAMBLE1;
						size = 0;
						read_incoming_index = 0;
						payload_length = 0;

					}
					else if (parse_message_type == zedf9_parse_message_type::UBX_NAV_PVT)
					{
						std::memcpy(&navigation_position_velocity_time.buffer[0],&buffer[read_incoming_index],UBX_NAV_PVT_LENGTH);
						read_incoming_index = read_incoming_index + payload_length;//UBX_NAV_PVT_LENGTH;

						parse_status = zedf9p_parse_status_type::PREAMBLE1;
						size = 0;
						read_incoming_index = 0;
						payload_length = 0;


					}
					else if (parse_message_type == zedf9_parse_message_type::UBX_NAV_RELPOSNED)
					{
						std::memcpy(&relative_pos_information.buffer[0],&buffer[read_incoming_index],UBX_NAV_RELPOSNED_LENGTH);
						read_incoming_index = read_incoming_index + payload_length;
						parse_status = zedf9p_parse_status_type::PREAMBLE1;
						size = 0;
						read_incoming_index = 0;
						payload_length = 0;
					}
				break;
			}
	}
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
ZEDF9P::~ZEDF9P()
{

}


}
}



