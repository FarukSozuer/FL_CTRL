/**
 ******************************************************************************
 * @file		: ds_telemetry_ins.cpp
 * @brief	  : INS Telemetry Class
 * @author	: Gurtac Kadem
 * 					  This file contains ins telemetry class
 * @date		: 04.06.2020
 * @version: 0.1.0
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

#include <ds_telemetry_ins.hpp>
#include <cstring>

Telemetry::INS_TELEMETRY ins_telemetry(&uart3, Telemetry::telemetry_id_type::INS, Telemetry::telemetry_id_type::FLIGHT_CONTROLLER);

namespace Telemetry
{

/**
 * @brief Default constructor
 *
 * @param[in]  Peripherals::Uart::H747_UART* uart_module
 * @param[in]  telemetry_id_type source_id
 * @param[in]  telemetry_id_type destination_id
 *
 * @return 	void
 */
INS_TELEMETRY::INS_TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id, telemetry_id_type destination_id) :
		TELEMETRY(uart_module, source_id, destination_id)
{

}

/**
 * @brief Processing message
 *
 * @param[in]  const payload_packet_type &payload_packet
 *
 * @return 	void
 */
void INS_TELEMETRY::processReceivedPacket(const payload_packet_type &payload_packet)
{
	ins_receive_type ins_receive;
	ins_receive_headers_type ins_header = static_cast<ins_receive_headers_type>(payload_packet.header);

	if ((ins_header == ins_receive_headers_type::RX_INS) && (payload_packet.data_length == INS_RX_BUFFER_SIZE))
	{
		std::memcpy(ins_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief Sending message
 *
 * @param[in]  void
 *
 * @return 	void
 */
void INS_TELEMETRY::sendMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::INS);
	ins_transmit_type ins_transmit;
	uint8_t data[512] = { 0 };
	static uint8_t changing_byte = 0;

	uint8_t buffer[512] = { 0 };
	uint16_t buffer_index = 0;
	uint8_t crc_data[512] = { 0 };
	uint16_t crc_index = 0;
	uint16_t index = 0;
	uint16_t crc16 = 0;

	uint16_t data_length = INS_TX_BUFFER_SIZE;

	ins_transmit.data.reserved = 0;

	std::memcpy(data, ins_transmit.buffer, data_length);

	buffer[buffer_index++] = static_cast<uint8_t>(ins_transmit_headers_type::TX_INS);

	buffer[buffer_index++] = INS_VERSION;
	crc_data[crc_index++] = INS_VERSION;

	buffer[buffer_index++] = changing_byte;
	crc_data[crc_index++] = changing_byte;
	changing_byte++;

	for (index = 0; index < data_length; index++)
	{
		buffer[buffer_index++] = data[index];
		crc_data[crc_index++] = data[index];
	}

	crc16 = calculateCrc16(crc_data, crc_index);

	buffer[buffer_index++] = static_cast<uint8_t>(crc16 % 256);
	buffer[buffer_index++] = static_cast<uint8_t>(crc16 / 256);

	buffer[buffer_index++] = static_cast<uint8_t>(ins_transmit_headers_type::TX_INS) + HEADER_FOOTER_DIFF;

	sendPacket(src_id, dest_id, buffer, buffer_index);
}

/**
 * @brief Default destructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
INS_TELEMETRY::~INS_TELEMETRY()
{

}

}
