/**
 ******************************************************************************
 * @file		: ds_telemetry_radar.hpp
 * @brief	  : Radar Telemetry Class
 * @author	: Gurtac Kadem
 * 					  This file contains radar telemetry class
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

#ifndef DS_TELEMETRY_RADAR_HPP
#define DS_TELEMETRY_RADAR_HPP

#include "ds_telemetry_gcs.hpp"

/*
 * Begin of Macro Definitions
 */
const uint16_t RADAR_TX_BUFFER_SIZE = 4;
const uint16_t RADAR_RX_BUFFER_SIZE = 25;

//End of Macro Definitions

namespace Telemetry
{

/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class radar_receive_headers_type : uint8_t
{
	RX_RADAR = 1,
};
enum class radar_transmit_headers_type : uint8_t
{
	TX_RADAR = 2,
};

#pragma pack(1)
union radar_receive_type
{
		struct
		{
				uint8_t id = 0;
				float x_distance_m = 0.0;
				float y_distance_m = 0.0;
				float z_distance_m = 0.0;
				float x_vel_mps = 0.0;
				float y_vel_mps = 0.0;
				float z_vel_mps = 0.0;

		} data;

		uint8_t buffer[RADAR_RX_BUFFER_SIZE] = { 0 };

		radar_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union radar_transmit_type
{
		struct
		{
				uint32_t reserved = 0;

		} data;

		uint8_t buffer[RADAR_TX_BUFFER_SIZE] = { 0 };

		radar_transmit_type()
		{
		}
};
#pragma pack()

// End of Enum, Union and Struct Definitions

/*
 * Begin of RADAR_TELEMETRY Class Definition
 */

class RADAR_TELEMETRY: public TELEMETRY
{
	public:
		explicit RADAR_TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id,
				telemetry_id_type destination_id);

		virtual ~RADAR_TELEMETRY();
		RADAR_TELEMETRY(const RADAR_TELEMETRY &other);

	protected:
			void processReceivedPacket(payload_packet_type const &payload_packet) override;

		private:
			const uint8_t RADAR_VERSION = 0;

			void sendMessage(void);
};

} /* End of namespace Telemetry */

/*
 * External Linkages
 */
extern Telemetry::RADAR_TELEMETRY radar_telemetry;
// End of External Linkages

#endif /* DS_TELEMETRY_RADAR_HPP */
