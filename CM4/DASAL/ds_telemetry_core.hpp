/**
 ******************************************************************************
 * @file		: ds_telemetry_core.hpp
 * @brief		: Core 2 core Telemetry Class
 *				  This file contains Core 2 core telemetry class
 * @author		: Onur Yildirim
 * @date		: 25.03.2020
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

#ifndef DS_TELEMETRY_CORE_HPP
#define	DS_TELEMETRY_CORE_HPP

/*
 * Begin of Includes
 */
#include <stdint.h>
#include "ds_telemetry.hpp"
// End of Includes

namespace Telemetry
{

/*
 * Begin of Macro Definitions
 */

//End of Macro Definitions

/*
 * Begin of Enum, Union and Struct Definitions
 */

//Telemetry Commands
enum class command_type : uint16_t
{
	UART1_SEND_DATA = 0x1000,
	TEST_PARAMETERS = 0x2000,
};

//Dummy Test Data
#pragma pack(1)
union test_data_type
{
		struct
		{
				float cpu_load;
				float vel_x_mps;
				float vel_y_mps;
				float vel_z_mps;
				float roll_deg;
				float pitch_deg;
				float yaw_deg;
		} data;

		uint8_t buffer[28];

		test_data_type() :
				buffer { }
		{
		}
};
#pragma pack()

// End of Enum, Union and Struct Definitions

/*
 * Begin of GCS_TELEMETRY Class Definition
 */
class CORE_TELEMETRY: public BASE
{
	public:
		CORE_TELEMETRY();

		test_data_type test_send;		//TODO: Can be deleted after tests are done
		test_data_type test_receive;	//TODO: Can be deleted after tests are done

		uint16_t parse_error_counter;
		uint16_t shared_buff_parse_err_cntr;

		void scheduler(void) override;
		void parseReceivedData(void) override;
		void sendPacket(uint16_t command, uint8_t data[], uint16_t length) override;
		void sendPeriodicPacket(void) override;

		bool receivePacketFromSharedBuff(uint32_t address, uint8_t payload[], uint16_t payload_size);
		void sendPacketToSharedBuff(uint32_t address, uint16_t command, uint8_t data[], uint16_t length);

		CORE_TELEMETRY(const CORE_TELEMETRY &orig);
		virtual ~CORE_TELEMETRY();

	protected:
		void processReceivedPacket(uint16_t command, uint8_t data[], uint16_t length) override;

	private:

};
// End of of CORE_TELEMETRY Definition

} /* End of namespace Telemetry */

/*
 * External Linkages
 */
extern Telemetry::CORE_TELEMETRY telemetry_core;
// End of External Linkages

#endif	/* DS_TELEMETRY_CORE_HPP */

