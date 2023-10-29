/**
******************************************************************************
  * @file		: ds_parse.hpp
  * @brief		: PARSE MESSAGE Interface Class
  *				  This file contains telemetry interface class
  * @author		: Faruk Sozuer
  * @date		: 10.02.2020
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

#ifndef DS_PARSE_HPP
#define DS_PARSE_HPP

#include <stdint.h>

namespace AppLayer
{
namespace Parse
{

class PACKET
{
public:

	PACKET();

	void parse_uint8 (const uint8_t  *value_u8,  uint8_t *buffer, uint16_t *index);
	void parse_int8  (const int8_t   *value_s8,  uint8_t *buffer, uint16_t *index);
	void parse_uint16(const uint16_t *value_u16, uint8_t *buffer, uint16_t  *index);
	void parse_int16 (const int16_t  *value_s16, uint8_t *buffer, uint16_t  *index);
	void parse_uint32(const uint32_t *value_u32, uint8_t *buffer, uint16_t *index);
	void parse_int32 (const int32_t  *value_s32, uint8_t *buffer, uint16_t *index);
	void parse_float (const float    *value_f32, uint8_t *buffer, uint16_t  *index);

	void add_uint8 	(uint8_t  *value_u8,  const uint8_t *buffer, uint16_t *index);
	void add_int8  	(int8_t   *value_s8,  const uint8_t *buffer, uint16_t *index);
	void add_uint16	(uint16_t *value_u16, const uint8_t *buffer, uint16_t *index);
	void add_int16 	(int16_t  *value_s16, const uint8_t *buffer, uint16_t *index);
	void add_int24	(int32_t *value_s32, 	const uint8_t *buffer, uint16_t *index );
	void add_uint32	(uint32_t *value_u32, const uint8_t *buffer, uint16_t *index);
	void add_int32 	(int32_t  *value_s32, const uint8_t *buffer, uint16_t *index);
	void add_uint64 (uint64_t *value_u64,	const uint8_t *buffer, uint16_t *index);
	void add_float 	(float    *value_f32, const uint8_t *buffer, uint16_t *index);


	PACKET(const PACKET& orig);
	virtual ~PACKET();

protected:

private:

};

}// Enf of  namespace Telemetry

} // End of namespace Interface

extern AppLayer::Parse::PACKET  packet;


namespace AppLayer
{
namespace Telemetry
{

class CIRCULAR
{
public:
	CIRCULAR();

	void buffer(uint16_t incomingIndex,uint8_t incomingBuffer[]);
    uint8_t checkPreamble(uint8_t preamble1, uint8_t preamble2, uint8_t preamble3,uint8_t buffer[]);
    void IncreaseMessageIndex(uint8_t increment);
    uint8_t controllerPayloadLength(uint8_t message_length);

    uint8_t msgID = 0;
    uint8_t payloadLength = 0;
    uint8_t deviceID = 0;
    uint8_t payload[1024] = {0};

	CIRCULAR(const CIRCULAR& orig);
	virtual ~CIRCULAR();

private:
	const uint16_t CIRBUFSIZE = 2048;
	const uint16_t SUCCESS = 0;
	const uint16_t FAILURE = 1;
	const uint16_t TEST_MESSAGE_LENGTH_MIN = 0;
	const uint16_t TEST_MESSAGE_LENGTH_MAX = 65535;


	uint16_t readIncomingIndex = 0;
	uint8_t  packetStatus = 0;

};
}// Enf of  namespace Telemetry
}// End of namespace Interface

extern AppLayer::Telemetry::CIRCULAR   circular;

#endif /* DS_TELEMETRY_HPP */
