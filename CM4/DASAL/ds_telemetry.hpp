/**
 ******************************************************************************
 * @file		: ds_telemetry.hpp
 * @brief	: Telemetry Base Class
 *				  	This file contains telemetry interface class
 * @author	: Onur Yildirim
 * @date		: 25.03.2020
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

#ifndef DS_TELEMETRY_HPP
#define	DS_TELEMETRY_HPP

/*
 * Begin of Includes
 */
#include <stdint.h>
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

// End of Enum, Union and Struct Definitions

/*
 * Begin of BASE Abstract Class Definition
 */
class BASE
{
	public:
		BASE();

		virtual void scheduler(void) = 0;
		virtual void parseReceivedData(void) = 0;
		virtual void sendPacket(uint16_t command, uint8_t data[], uint16_t length) = 0;
		virtual void sendPeriodicPacket(void) = 0;

		BASE(const BASE &orig);
		virtual ~BASE();

	protected:
		virtual void processReceivedPacket(uint16_t command, uint8_t data[], uint16_t length) = 0;
		bool calculateCrc16(uint8_t *aBuffer, uint16_t aLength, uint16_t command, bool aCheck);
		uint16_t calculateCrc16(const uint8_t *aBuffer, uint16_t aLength);
		bool checkCrc16(const uint8_t *aBuffer, uint16_t aLength);

	private:

};
// End of of BASE Class Definition

} /* End of namespace Telemetry */

/*
 * External Linkages
 */

// End of External Linkages

#endif	/* DS_TELEMETRY_HPP */

