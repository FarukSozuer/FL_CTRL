/**
 ******************************************************************************
 * @file		: ds_telemetry_gcs.cpp
 * @brief	: Telemetry and Telemetry GCS Class
 * 					This file contains telemetry and telemetry gcs class
 * @author	: Gurtac Kadem
 * @date		: 02.06.2020
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

#include <ds_telemetry_gcs.hpp>
#include <cstring>
#include "ds_telemetry_core.hpp"

Telemetry::GCS_TELEMETRY gcs_telemetry(&uart3, Telemetry::telemetry_id_type::GCS, Telemetry::telemetry_id_type::FLIGHT_CONTROLLER);

/*
 * Begin of Local Constant Definitions
 */
const uint8_t TELEMETRY_HEADER = 0xFA;
// End of Local Constant Definitions

/*
 * Begin of Enum, Union and Struct Definitions
 */

//Telemetry Parser States
enum class parser_state_type : uint8_t
{
	HEADER = 0,
	SOURCE_ID = 1,
	DESTINATION_ID = 2,
	PAYLOAD_SIZE_1 = 3,
	PAYLOAD_SIZE_2 = 4,
	PAYLOAD = 5,
	CRC_1 = 6,
	CRC_2 = 7,
};

//Payload Parser States
enum class payload_state_type : uint8_t
{
	PAYLOAD_HEADER = 0,
	VERSION = 1,
	CHANGING_BYTE = 2,
	DATA = 3,
	CRC_1 = 4,
	CRC_2 = 5,
	FOOTER = 6,
};

// End of Enum, Union and Struct Definitions

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
TELEMETRY::TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id, telemetry_id_type destination_id) :
		uart_module(uart_module), source_id(source_id), destination_id(destination_id)
{

}

/**
 * @brief 			Process incoming payload data
 *
 * @param[in]	const uint8_t payload[]
 * @param[in]	uint16_t payload_length
 *
 * @return 		void
 */
void TELEMETRY::processPayloadPacket(const uint8_t payload[], uint16_t payload_length)
{
	uint16_t index = 0;

	payload_state_type state = payload_state_type::PAYLOAD_HEADER;
	uint8_t crc_data[512] = { 0 };
	uint16_t crc_counter = 0;
	uint16_t data_counter = 0;
	payload_packet_type payload_packet;

	/*
	 * | PAYLOAD HEADER	| VERSION			| CHANGING_BYTE	| DATA		| CRC			| FOOTER		|
	 */

	for (index = 0; index < payload_length; index++)
	{
		switch (state)
		{
			case payload_state_type::PAYLOAD_HEADER:
				payload_packet.header = payload[index];
				payload_packet.data_length = payload_length - 6;
				data_counter = 0;
				crc_counter = 0;
				state = payload_state_type::VERSION;
				break;

			case payload_state_type::VERSION:
				payload_packet.version = payload[index];
				crc_data[crc_counter++] = payload[index];
				state = payload_state_type::CHANGING_BYTE;
				break;

			case payload_state_type::CHANGING_BYTE:
				payload_packet.changing_byte = payload[index];
				crc_data[crc_counter++] = payload[index];
				state = payload_state_type::DATA;
				break;

			case payload_state_type::DATA:
				payload_packet.data[data_counter++] = payload[index];
				crc_data[crc_counter++] = payload[index];
				if (data_counter >= payload_packet.data_length)
				{
					state = payload_state_type::CRC_1;
				}
				break;

			case payload_state_type::CRC_1:
				crc_data[crc_counter] = payload[index];
				state = payload_state_type::CRC_2;
				break;

			case payload_state_type::CRC_2:
				crc_data[crc_counter + 1] = payload[index];
				if (checkCrc16(crc_data, crc_counter) == true)
				{
					state = payload_state_type::FOOTER;
				}
				else
				{
					payload_error_counter++;
					state = payload_state_type::PAYLOAD_HEADER;
				}
				break;

			case payload_state_type::FOOTER:
				payload_packet.footer = payload[index];
				if ((payload_packet.footer - payload_packet.header) == HEADER_FOOTER_DIFF)
				{
					processReceivedPacket(payload_packet);
				}
				state = payload_state_type::PAYLOAD_HEADER;
				break;

			default:
				state = payload_state_type::PAYLOAD_HEADER;
				break;
		}
	}
}

void TELEMETRY::scheduler(void)
{
	parseReceivedData();
}

/**
 * @brief 			Parse and process incoming data
 *
 * @param[in]	void
 *
 * @return 		void
 */
void TELEMETRY::parseReceivedData(void)
{
	uint8_t buffer[512] = { 0 };
	uint16_t size = 0;
	uint16_t index = 0;

	static parser_state_type state = parser_state_type::HEADER;
	static uint8_t payload[512] = { 0 };
	static uint16_t payload_counter = 0;
	static uint8_t crc_data[512] = { 0 };
	static uint16_t crc_counter = 0;
	static uint16_t length = 0;

	/*
	 * | HEADER	 		 | SOURCE_ID		| DESTINATION_ID	| PAYLOAD_SIZE	| PAYLOAD			| CRC		|
	 * | 0xFA        | 0x00-0x05    | 0x00-0x05 			| -- 						| -- 					|	--		|
	 */

	size = uart_module->getDataFromBuffer(buffer, sizeof(buffer));

	for (index = 0; index < size; index++)
	{
		switch (state)
		{
			case parser_state_type::HEADER:
				if (buffer[index] == TELEMETRY_HEADER)
				{
					length = 0;
					payload_counter = 0;
					crc_counter = 0;
					state = parser_state_type::SOURCE_ID;
				}
				break;

			case parser_state_type::SOURCE_ID:
				if (buffer[index] == static_cast<uint8_t>(source_id))
				{
					crc_data[crc_counter++] = buffer[index];
					state = parser_state_type::DESTINATION_ID;
				}
				else
				{
					state = parser_state_type::HEADER;
				}
				break;

			case parser_state_type::DESTINATION_ID:
				if (buffer[index] == static_cast<uint8_t>(destination_id))
				{
					crc_data[crc_counter++] = buffer[index];
					state = parser_state_type::PAYLOAD_SIZE_1;
				}
				else
				{
					state = parser_state_type::HEADER;
				}
				break;

			case parser_state_type::PAYLOAD_SIZE_1:
				length = (static_cast<uint16_t>(buffer[index]));
				crc_data[crc_counter++] = buffer[index];
				state = parser_state_type::PAYLOAD_SIZE_2;
				break;

			case parser_state_type::PAYLOAD_SIZE_2:
				length += (static_cast<uint16_t>(buffer[index]) << 8);
				crc_data[crc_counter++] = buffer[index];
				state = parser_state_type::PAYLOAD;
				break;

			case parser_state_type::PAYLOAD:
				payload[payload_counter++] = buffer[index];
				crc_data[crc_counter++] = buffer[index];
				if (payload_counter >= length)
				{
					state = parser_state_type::CRC_1;
				}
				break;

			case parser_state_type::CRC_1:
				crc_data[crc_counter] = buffer[index];
				state = parser_state_type::CRC_2;
				break;

			case parser_state_type::CRC_2:
				crc_data[crc_counter + 1] = buffer[index];
				if (checkCrc16(crc_data, crc_counter) == true)
				{
					processPayloadPacket(payload, length);
				}
				else
				{
					parse_error_counter++;
				}
				state = parser_state_type::HEADER;
				break;

			default:
				state = parser_state_type::HEADER;
				break;
		}
	}
}

/**
 * @brief 			Prepare payload message packet
 *
 * @param[in]	uint8_t  src_id
 * @param[in]	uint8_t  dest_id
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void TELEMETRY::preparePayload(uint8_t src_id, uint8_t dest_id, payload_packet_type const &payload_packet)
{
	uint8_t buffer[512] = { 0 };
	uint16_t buffer_index = 0;
	uint8_t crc_data[512] = { 0 };
	uint16_t crc_index = 0;
	uint16_t index = 0;
	uint16_t crc16 = 0;

	buffer[buffer_index++] = payload_packet.header;

	buffer[buffer_index++] = payload_packet.version;
	crc_data[crc_index++] = payload_packet.version;

	buffer[buffer_index++] = payload_packet.changing_byte;
	crc_data[crc_index++] = payload_packet.changing_byte;

	for (index = 0; index < payload_packet.data_length; index++)
	{
		buffer[buffer_index++] = payload_packet.data[index];
		crc_data[crc_index++] = payload_packet.data[index];
	}

	crc16 = calculateCrc16(crc_data, crc_index);

	buffer[buffer_index++] = static_cast<uint8_t>(crc16 % 256);
	buffer[buffer_index++] = static_cast<uint8_t>(crc16 / 256);

	buffer[buffer_index++] = payload_packet.footer;

	sendPacket(src_id, dest_id, buffer, buffer_index);
}

/**
 * @brief 			Sends a packet to the other core
 *
 * @param[in]	uint8_t  source_id
 * @param[in]	uint8_t  destination_id
 * @param[in]	const uint8_t  data[]
 * @param[in]	uint16_t length
 *
 * @return 		void
 */
void TELEMETRY::sendPacket(uint8_t src_id, uint8_t dest_id, const uint8_t data[], uint16_t length)
{
	uint16_t buffer_index = 0;
	uint16_t index = 0;
	uint8_t buffer[1024] = { 0 };
	uint8_t crc_data[1024] = { 0 };
	uint16_t crc_counter = 0;
	uint16_t crc16 = 0;

	buffer[buffer_index++] = TELEMETRY_HEADER;
	buffer[buffer_index++] = src_id;
	buffer[buffer_index++] = dest_id;

	crc_data[crc_counter++] = src_id;
	crc_data[crc_counter++] = dest_id;

	buffer[buffer_index++] = static_cast<uint8_t>(length % 256);
	buffer[buffer_index++] = static_cast<uint8_t>(length / 256);

	crc_data[crc_counter++] = static_cast<uint8_t>(length % 256);
	crc_data[crc_counter++] = static_cast<uint8_t>(length / 256);

	if (length < 1024)
	{
		for (index = 0; index < length; index++)
		{
			buffer[buffer_index++] = data[index];
			crc_data[crc_counter++] = data[index];
		}
	}

	crc16 = calculateCrc16(crc_data, crc_counter);

	buffer[buffer_index++] = static_cast<uint8_t>(crc16 % 256);
	buffer[buffer_index++] = static_cast<uint8_t>(crc16 / 256);

	uart_module->sendData(buffer, buffer_index);

}

void TELEMETRY::sendPacket(uint16_t command, uint8_t data[], uint16_t length)
{
}

void TELEMETRY::sendPeriodicPacket(void)
{
}

void TELEMETRY::processReceivedPacket(uint16_t command, uint8_t data[], uint16_t length)
{
}

/**
 * @brief Default copy constructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
TELEMETRY::TELEMETRY(const TELEMETRY &orig)
{

}

/**
 * @brief Default destructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
TELEMETRY::~TELEMETRY()
{

}

/**
 * @brief Default constructor
 *
 * @param[in]  Peripherals::Uart::H747_UART* uart_module
 * @param[in]  telemetry_id_type source_id
 * @param[in]  telemetry_id_type destination_id
 *
 * @return 	void
 */
GCS_TELEMETRY::GCS_TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id, telemetry_id_type destination_id) :
		TELEMETRY(uart_module, source_id, destination_id)
{

}

/**
 * @brief 			Processes the received packet
 *
 * @param[in]	const payload_packet_type payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processReceivedPacket(payload_packet_type const &payload_packet)
{
	gcs_receive_headers_type gcs_header = static_cast<gcs_receive_headers_type>(payload_packet.header);
	switch (gcs_header)
	{
		case gcs_receive_headers_type::RX_HEARTBEAT:
			processHeartBeatMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_VEHICLE:
			processVehicleMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_PAYLOAD:
			processPayloadMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_WAYPOINT:
			processWaypointMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_CALIBRATION:
			processCalibrationMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_CONTROL_1:
			processControlMessage(payload_packet, control_message_type::TYPE_1);
			break;

		case gcs_receive_headers_type::RX_CONTROL_2:
			processControlMessage(payload_packet, control_message_type::TYPE_2);
			break;

		case gcs_receive_headers_type::RX_CONTROL_3:
			processControlMessage(payload_packet, control_message_type::TYPE_3);
			break;

		case gcs_receive_headers_type::RX_CONTROL_4:
			processControlMessage(payload_packet, control_message_type::TYPE_4);
			break;

		case gcs_receive_headers_type::RX_CONFIG:
			processConfigMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_CHANGE_HOME:
			processCHPMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_NO_FLY_ZONE:
			processNFZMessage(payload_packet);
			break;

		case gcs_receive_headers_type::RX_BOOT:
			processBootMessage(payload_packet);
			break;

		default:
			break;
	}
}

/**
 * @brief 			Processes heartbeat message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processHeartBeatMessage(payload_packet_type const &payload_packet)
{
	heartbeat_receive_type heartbeat_receive;
	if (payload_packet.data_length == HEARTBEAT_RX_BUFFER_SIZE)
	{
		std::memcpy(heartbeat_receive.buffer, payload_packet.data, payload_packet.data_length);
	}

}

/**
 * @brief 			Processes vehicle message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processVehicleMessage(payload_packet_type const &payload_packet)
{
	vehicle_receive_type vehicle_receive;
	if (payload_packet.data_length == VEHICLE_RX_BUFFER_SIZE)
	{
		std::memcpy(vehicle_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes payload message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processPayloadMessage(payload_packet_type const &payload_packet)
{
	payload_receive_type payload_receive;
	if (payload_packet.data_length == PAYLOAD_RX_BUFFER_SIZE)
	{
		std::memcpy(payload_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes waypoint message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processWaypointMessage(payload_packet_type const &payload_packet)
{
	waypoint_receive_type waypoint_receive;
	if (payload_packet.data_length == WAYPOINT_RX_BUFFER_SIZE)
	{
		std::memcpy(waypoint_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes calibration message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processCalibrationMessage(payload_packet_type const &payload_packet)
{
	calibration_receive_type calibration_receive;
	if (payload_packet.data_length == CALIBRATION_RX_BUFFER_SIZE)
	{
		std::memcpy(calibration_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes control message
 *
 * @param[in]	const payload_packet_type  payload_packet
 * @param[in]	uint8_t  type
 *
 * @return 		void
 */
void GCS_TELEMETRY::processControlMessage(payload_packet_type const &payload_packet, control_message_type type)
{
	control_1_receive_type control_1_receive;
	control_2_receive_type control_2_receive;
	control_3_receive_type control_3_receive;
	control_4_receive_type control_4_receive;

	switch (type)
	{
		case control_message_type::TYPE_1:
			if (payload_packet.data_length == CONTROL_1_RX_BUFFER_SIZE)
			{
				std::memcpy(control_1_receive.buffer, payload_packet.data, payload_packet.data_length);
			}
			break;

		case control_message_type::TYPE_2:
			if (payload_packet.data_length == CONTROL_2_RX_BUFFER_SIZE)
			{
				std::memcpy(control_2_receive.buffer, payload_packet.data, payload_packet.data_length);
			}
			break;

		case control_message_type::TYPE_3:
			if (payload_packet.data_length == CONTROL_3_RX_BUFFER_SIZE)
			{
				std::memcpy(control_3_receive.buffer, payload_packet.data, payload_packet.data_length);
			}
			break;

		case control_message_type::TYPE_4:
			if (payload_packet.data_length == CONTROL_4_RX_BUFFER_SIZE)
			{
				std::memcpy(control_4_receive.buffer, payload_packet.data, payload_packet.data_length);
			}
			break;

		default:
			break;
	}
}

/**
 * @brief 			Processes configuration message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processConfigMessage(payload_packet_type const &payload_packet)
{
	config_receive_type config_receive;
	if (payload_packet.data_length == CONFIG_RX_BUFFER_SIZE)
	{
		std::memcpy(config_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes change home point message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processCHPMessage(payload_packet_type const &payload_packet)
{
	chp_receive_type chp_receive;
	if (payload_packet.data_length == CHP_RX_BUFFER_SIZE)
	{
		std::memcpy(chp_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes no fly zone message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processNFZMessage(payload_packet_type const &payload_packet)
{
	nfz_receive_type nfz_receive;
	if (payload_packet.data_length == NFZ_RX_BUFFER_SIZE)
	{
		std::memcpy(nfz_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Processes boot message
 *
 * @param[in]	const payload_packet_type  payload_packet
 *
 * @return 		void
 */
void GCS_TELEMETRY::processBootMessage(payload_packet_type const &payload_packet)
{
	boot_receive_type boot_receive;
	if (payload_packet.data_length == BOOT_RX_BUFFER_SIZE)
	{
		std::memcpy(boot_receive.buffer, payload_packet.data, payload_packet.data_length);
	}
}

/**
 * @brief 			Send heartbeat message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendHeartBeatMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	heartbeat_transmit_type heartbeat_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_HEARTBEAT);
	payload_packet.version = HEARTBEAT_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = HEARTBEAT_TX_BUFFER_SIZE;

	heartbeat_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, heartbeat_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send vehicle message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendVehicleMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	vehicle_transmit_type vehicle_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_VEHICLE);
	payload_packet.version = VEHICLE_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = VEHICLE_TX_BUFFER_SIZE;

	vehicle_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, vehicle_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send payload message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendPayloadMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	payload_transmit_type payload_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_PAYLOAD);
	payload_packet.version = PAYLOAD_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = PAYLOAD_TX_BUFFER_SIZE;

	payload_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, payload_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send waypoint message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendWaypointMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	waypoint_transmit_type waypoint_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_WAYPOINT);
	payload_packet.version = WAYPOINT_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = WAYPOINT_TX_BUFFER_SIZE;

	waypoint_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, waypoint_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send telemetry 5hz message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendTelemetry5HzMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	telemetry_5hz_type telemetry_5hz_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_TELEMETRY_5HZ);
	payload_packet.version = TELEMETRY_5HZ_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = TELEMETRY_5HZ_TX_BUFFER_SIZE;

	telemetry_5hz_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, telemetry_5hz_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send telemetry 1hz message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendTelemetry1HzMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	telemetry_1hz_type telemetry_1hz_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_TELEMETRY_1HZ);
	payload_packet.version = TELEMETRY_1HZ_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = TELEMETRY_1HZ_TX_BUFFER_SIZE;

	telemetry_1hz_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, telemetry_1hz_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send calibration message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendCalibrationMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	calibration_transmit_type calibration_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CALIBRATION);
	payload_packet.version = CALIBRATION_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = CALIBRATION_TX_BUFFER_SIZE;

	calibration_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, calibration_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send control message
 *
 * @param[in]	control_message_type type
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendControlMessage(control_message_type type)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	payload_packet_type payload_packet;

	static uint8_t changing_byte_1 = 0;
	static uint8_t changing_byte_2 = 0;
	static uint8_t changing_byte_3 = 0;
	static uint8_t changing_byte_4 = 0;

	control_1_transmit_type control_1_transmit;
	control_2_transmit_type control_2_transmit;
	control_3_transmit_type control_3_transmit;
	control_4_transmit_type control_4_transmit;

	payload_packet.version = CONTROL_VERSION;

	switch (type)
	{
		case control_message_type::TYPE_1:
			payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CONTROL_1);
			payload_packet.changing_byte = changing_byte_1++;

			payload_packet.data_length = CONTROL_1_TX_BUFFER_SIZE;

			control_1_transmit.data.reserved = 0;

			std::memcpy(payload_packet.data, control_1_transmit.buffer, payload_packet.data_length);
			break;

		case control_message_type::TYPE_2:
			payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CONTROL_2);
			payload_packet.changing_byte = changing_byte_2++;

			payload_packet.data_length = CONTROL_2_TX_BUFFER_SIZE;

			control_2_transmit.data.reserved = 0;

			std::memcpy(payload_packet.data, control_2_transmit.buffer, payload_packet.data_length);
			break;

		case control_message_type::TYPE_3:
			payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CONTROL_3);
			payload_packet.changing_byte = changing_byte_3++;

			payload_packet.data_length = CONTROL_3_TX_BUFFER_SIZE;

			control_3_transmit.data.reserved = 0;

			std::memcpy(payload_packet.data, control_3_transmit.buffer, payload_packet.data_length);
			break;

		case control_message_type::TYPE_4:
			payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CONTROL_4);
			payload_packet.changing_byte = changing_byte_4++;

			payload_packet.data_length = CONTROL_4_TX_BUFFER_SIZE;

			control_4_transmit.data.reserved = 0;

			std::memcpy(payload_packet.data, control_4_transmit.buffer, payload_packet.data_length);
			break;

		default:
			break;
	}

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send configuration message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendConfigMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	config_transmit_type config_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CONFIG);
	payload_packet.version = CONFIG_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = CONFIG_TX_BUFFER_SIZE;

	config_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, config_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send change home point message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendCHPMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	chp_transmit_type chp_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_CHANGE_HOME);
	payload_packet.version = CHP_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = CHP_TX_BUFFER_SIZE;

	chp_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, chp_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send no fly zone message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendNFZMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	nfz_transmit_type nfz_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_NO_FLY_ZONE);
	payload_packet.version = NFZ_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = NFZ_TX_BUFFER_SIZE;

	nfz_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, nfz_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief 			Send boot message
 *
 * @param[in]	void
 *
 * @return 		void
 */
void GCS_TELEMETRY::sendBootMessage(void)
{
	uint8_t src_id = static_cast<uint8_t>(telemetry_id_type::FLIGHT_CONTROLLER);
	uint8_t dest_id = static_cast<uint8_t>(telemetry_id_type::GCS);
	boot_transmit_type boot_transmit;
	payload_packet_type payload_packet;
	static uint8_t changing_byte = 0;

	payload_packet.header = static_cast<uint8_t>(gcs_transmit_headers_type::TX_BOOT);
	payload_packet.version = BOOT_VERSION;
	payload_packet.changing_byte = changing_byte++;

	payload_packet.data_length = BOOT_TX_BUFFER_SIZE;

	boot_transmit.data.reserved = 0;

	std::memcpy(payload_packet.data, boot_transmit.buffer, payload_packet.data_length);

	payload_packet.footer = payload_packet.header + HEADER_FOOTER_DIFF;

	preparePayload(src_id, dest_id, payload_packet);
}

/**
 * @brief Default destructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
GCS_TELEMETRY::~GCS_TELEMETRY()
{

}

} /* End of namespace Telemetry */

