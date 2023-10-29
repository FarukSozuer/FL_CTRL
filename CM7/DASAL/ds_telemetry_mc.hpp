/**
 ******************************************************************************
 * @file		: ds_telemetry_mc.hpp
 * @brief	: Telemetry MC Class
 * @author	: Gurtac Kadem
 * 					This file contains mission computer telemetry class
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

#ifndef DS_TELEMETRY_MC_HPP
#define DS_TELEMETRY_MC_HPP

#include "ds_telemetry_gcs.hpp"

/*
 * Begin of Macro Definitions
 */
const uint16_t MC_TX_BUFFER_SIZE = 32;
const uint16_t MC_RX_BUFFER_SIZE = 27;

//End of Macro Definitions

namespace Telemetry
{

/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class mc_receive_headers_type : uint8_t
{
	RX_MC = 1,
};
enum class mc_transmit_headers_type : uint8_t
{
	TX_MC = 2,
};

#pragma pack(1)
union mc_receive_type
{
		struct
		{
				uint8_t id = 0;
				uint8_t product_id = 0;
				uint8_t software_ver = 0;
				uint32_t timestamp = 0;
				float distance_down = 0.0;
				float distance_front = 0.0;
				float distance_left = 0.0;
				float distance_right = 0.0;
				uint32_t status_flag = 0;

		} data;

		uint8_t buffer[MC_RX_BUFFER_SIZE] = { 0 };

		mc_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union mc_transmit_type
{
		struct
		{
				uint32_t timestamp = 0;
				float roll_deg = 0;
				float pitch_deg = 0;
				float yaw_deg = 0;
				int32_t latiude_deg = 0;
				int32_t longitude_deg = 0;
				float altitude_m = 0.0;
				uint32_t reserved = 0;

		} data;

		uint8_t buffer[MC_TX_BUFFER_SIZE] = { 0 };

		mc_transmit_type()
		{
		}
};
#pragma pack()

// End of Enum, Union and Struct Definitions

/*
 * Begin of MC_TELEMETRY Class Definition
 */

class MC_TELEMETRY: public TELEMETRY
{
	public:
		explicit MC_TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id,
				telemetry_id_type destination_id);

		MC_TELEMETRY(const MC_TELEMETRY &orig);
		virtual ~MC_TELEMETRY();

	protected:
		void processReceivedPacket(payload_packet_type const &payload_packet) override;

	private:
		const uint8_t MC_VERSION = 0;

		void sendMessage(void);
};
// End of of MC_TELEMETRY Definition

} /* End of namespace Telemetry */

/*
 * External Linkages
 */
extern Telemetry::MC_TELEMETRY mc_telemetry;
// End of External Linkages
#endif /* DS_TELEMETRY_MC_HPP */
