/**
 ******************************************************************************
  * @file		: ds_telemetry_core.cpp
  * @brief	: Core 2 core Telemetry Class
  *				  	This file contains Core 2 core telemetry class
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



/*
 * Begin of Includes
 */
#include "ds_telemetry_core.hpp"
#include <cstring>
#include "ds_shared_ram_h747.hpp"
#include "ds_uart_h747.hpp"
#include "ds_debug_tools.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
 Telemetry::CORE_TELEMETRY telemetry_core;
// End of Object Definitions



/*
 * Begin of Local Constant Definitions
 */

//Telemetry Parser constants
const uint8_t HDR_1	= 0xED;
const uint8_t HDR_2	= 0xAB;
const uint8_t FTR 	= 0xDE;


//End of Local Constant Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */

//Telemetry Parser States
enum class parser_state_type: uint8_t 
{
    HEADER_1	= 0,
    HEADER_2	= 1,
		COMMAND_1	= 2,
		COMMAND_2	= 3,
		LENGTH_1	= 4,
		LENGTH_2	= 5,
    PAYLOAD		= 6,
		CRC_1			= 7,
		CRC_2			= 8,
		FOOTER		= 9,
};
// End of Enum, Union and Struct Definitions



namespace Telemetry
{



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
CORE_TELEMETRY::CORE_TELEMETRY() :
BASE()
{
	parse_error_counter = 0;
	shared_buff_parse_err_cntr = 0;
}



/**
  * @brief 			TBD
  *
  * @param[in]	void
  *
  * @return 		void
  */
void CORE_TELEMETRY::scheduler(void)
{

}



/**
  * @brief 			Parse and process incoming data
  *
  * @param[in]	void
  *
  * @return 		void
  */
void CORE_TELEMETRY::parseReceivedData(void)
{
	uint8_t buffer[512]	= {0};
	uint16_t size	= 0;
	uint16_t k		= 0;
	
	static parser_state_type			state = parser_state_type::HEADER_1;
	static uint8_t 	payload[1024]	= {0};
	static uint16_t counter 			=  0;
	static uint16_t length				=  0;
	static uint16_t command				=  0;

	
	/*
	 * | HEADER	 		 | COMMAND		| LENGTH			| PAYLOAD	   | CRC16			| FOOTER	|
	 * | 0xED 	0xAB | 0x00  0x00 | 0x00  0x00 	| 0xAA  0xBB | 0xAA  0xBB |	0xFA		|
	 */
	
	size = inter_core.getDataFromBuffer(buffer, sizeof(buffer));
	
	if(size > 0)
	{
		for(k=0;k<size;k++)
		{
			switch(state)
			{
				case parser_state_type::HEADER_1 :
					if(buffer[k] == HDR_1)
					{
						length  					= 0;
						command 					= 0;
						counter   				= 0;
						state = parser_state_type::HEADER_2;
					}
					break;


				case parser_state_type::HEADER_2 :
					if(buffer[k] == HDR_2)
					{
						state = parser_state_type::COMMAND_1;
					}
					else
					{
						state = parser_state_type::HEADER_1;
					}
					break;


				case parser_state_type::COMMAND_1 :
					command = (static_cast<uint16_t>(buffer[k]) );
					state = parser_state_type::COMMAND_2;
					break;


				case parser_state_type::COMMAND_2 :
					command += (static_cast<uint16_t>(buffer[k]) << 8 );
					state = parser_state_type::LENGTH_1;
					break;


				case parser_state_type::LENGTH_1 :
					length = ( static_cast<uint16_t>(buffer[k]) );
					state = parser_state_type::LENGTH_2;
					break;


				case parser_state_type::LENGTH_2 :
					length += (static_cast<uint16_t>(buffer[k]) << 8 );
					state = parser_state_type::PAYLOAD;
					break;


				case parser_state_type::PAYLOAD :
					payload[counter++] = buffer[k];
					if(counter >= length)
					{
						state = parser_state_type::CRC_1;
					}
					break;


				case parser_state_type::CRC_1 :
					payload[counter] = buffer[k];
					state = parser_state_type::CRC_2;
					break;


				case parser_state_type::CRC_2 :
					payload[counter+1] = buffer[k];
					if(calculateCrc16(payload, counter, command, true) == true  )
					{
						state = parser_state_type::FOOTER;
					}
					else
					{
						parse_error_counter++;
						state = parser_state_type::HEADER_1;
					}
					break;


				case parser_state_type::FOOTER :
					if(buffer[k] == FTR)
					{
						processReceivedPacket(command, payload, length);
					}
					else
					{
						parse_error_counter++;
					}
					state = parser_state_type::HEADER_1;
					break;


				default :
					state = parser_state_type::HEADER_1;
					break;
			}
		}
	}
}



/**
  * @brief 			Sends a packet to the other core
  *
  * @param[in]	uint16_t command
  * @param[in]	uint8_t  data[]
  * @param[in]	uint16_t length
  *
  * @return 		void
  */
void CORE_TELEMETRY::sendPacket(uint16_t command, uint8_t data[], uint16_t length)
{
		uint16_t k = 0;
		uint16_t t = 0;
    uint8_t  buffer[1024] = {0};

    buffer[k++] = HDR_1;
    buffer[k++] = HDR_2;

		buffer[k++] = (uint8_t)(command%256);
    buffer[k++] = (uint8_t)(command/256);

		buffer[k++] = (uint8_t)(length%256);
    buffer[k++] = (uint8_t)(length/256);

    for(t = 0; t < length; t++)
    {
        buffer[k++] = data[t];
    }

		calculateCrc16(&buffer[2], k-2, 0, false);
		k += 2;
    buffer[k++] = FTR;

		inter_core.sendData(buffer, k);
		
}



/**
  * @brief 			Sends periodic packet
  *
  * @param[in]	void
  *
  * @return 		void
  */
void CORE_TELEMETRY::sendPeriodicPacket(void)
{
	test_send.data.cpu_load 	 = monitor.cpu_load_active;
	test_send.data.yaw_deg 		+= 1;

	sendPacket(static_cast<uint16_t>(command_type::TEST_PARAMETERS),test_send.buffer,sizeof(test_send.buffer));
}



/**
  * @brief 			Parse and process incoming data
  *
  * @param[in]	uint32_t address
  * @param[in]	uint8_t  payload[]
  * @param[in]	uint16_t payload_size
  *
  * @return 		bool packet_received
  */
bool CORE_TELEMETRY::receivePacketFromSharedBuff(uint32_t address, uint8_t payload[], uint16_t payload_size)
{
	const uint16_t PACKET_SIZE_WITHOUT_PAYLOAD = 9;
	uint8_t buffer[1024] = {0};
	uint8_t crc_data[1024] = {0};
	uint16_t size	= 0;
	uint16_t k = 0;
	uint16_t t = 0;
	bool packet_received = false;

	static parser_state_type state = parser_state_type::HEADER_1;
	static uint16_t counter = 0;
	static uint16_t length	= 0;
	static uint16_t command	= 0;

	/*
	 * | HEADER	 		 | COMMAND		| LENGTH			| PAYLOAD	   | CRC16			| FOOTER	|
	 * | 0xED 	0xAB | 0x00  0x00 | 0x00  0x00 	| 0xAA  0xBB | 0xAA  0xBB |	0xFA		|
	 */

	size = payload_size + PACKET_SIZE_WITHOUT_PAYLOAD;
	inter_core.getDataFromSharedRam(address, buffer, size);

	for(k=0;k<size;k++)
	{
		switch(state)
		{
			case parser_state_type::HEADER_1 :
				if(buffer[k] == HDR_1)
				{
					length  					= 0;
					command 					= 0;
					counter   				= 0;
					state = parser_state_type::HEADER_2;
				}
				break;


			case parser_state_type::HEADER_2 :
				if(buffer[k] == HDR_2)
				{
					state = parser_state_type::COMMAND_1;
				}
				else
				{
					state = parser_state_type::HEADER_1;
				}
				break;


			case parser_state_type::COMMAND_1 :
				command = (static_cast<uint16_t>(buffer[k]) );
				state = parser_state_type::COMMAND_2;
				break;


			case parser_state_type::COMMAND_2 :
				command += (static_cast<uint16_t>(buffer[k]) << 8 );
				state = parser_state_type::LENGTH_1;
				break;


			case parser_state_type::LENGTH_1 :
				length = ( static_cast<uint16_t>(buffer[k]) );
				state = parser_state_type::LENGTH_2;
				break;


			case parser_state_type::LENGTH_2 :
				length += (static_cast<uint16_t>(buffer[k]) << 8 );
				state = parser_state_type::PAYLOAD;
				break;


			case parser_state_type::PAYLOAD :
				crc_data[counter++] = buffer[k];
				if(counter >= length)
				{
					state = parser_state_type::CRC_1;
				}
				break;


			case parser_state_type::CRC_1 :
				crc_data[counter] = buffer[k];
				state = parser_state_type::CRC_2;
				break;


			case parser_state_type::CRC_2 :
				crc_data[counter+1] = buffer[k];
				if(calculateCrc16(crc_data, counter, command, true) == true  )
				{
					state = parser_state_type::FOOTER;
				}
				else
				{
					shared_buff_parse_err_cntr++;
					state = parser_state_type::HEADER_1;
				}
				break;


			case parser_state_type::FOOTER :
				if(buffer[k] == FTR)
				{
					packet_received = true;
					for(t=0;t<length;t++)
					{
						payload[t] = crc_data[t];
					}
				}
				else
				{
					shared_buff_parse_err_cntr++;
				}
				state = parser_state_type::HEADER_1;
				break;


			default :
				state = parser_state_type::HEADER_1;
				break;
		}
	}

	return packet_received;
}



/**
  * @brief 			Sends a packet to the shared buffer
  *
  * @param[in]	uint16_t command
  * @param[in]	uint8_t  data[]
  * @param[in]	uint16_t length
  *
  * @return 		void
  */
void CORE_TELEMETRY::sendPacketToSharedBuff(uint32_t address, uint16_t command, uint8_t data[], uint16_t length)
{
	uint16_t k = 0;
	uint16_t t = 0;
  uint8_t  buffer[1024] = {0};

  buffer[k++] = HDR_1;
  buffer[k++] = HDR_2;

	buffer[k++] = (uint8_t)(command%256);
  buffer[k++] = (uint8_t)(command/256);

	buffer[k++] = (uint8_t)(length%256);
  buffer[k++] = (uint8_t)(length/256);

  for(t = 0; t < length; t++)
  {
      buffer[k++] = data[t];
  }

	calculateCrc16(&buffer[2], k-2, 0, false);
	k += 2;
  buffer[k++] = FTR;

	inter_core.sendDataToSharedRam(address,buffer, k);
}



/**
  * @brief 			Processes the received packet
  *
  * @param[in]	uint16_t command
  * @param[in]	uint8_t  data[]
  * @param[in]	uint16_t length
  *
  * @return 		void
  */
void CORE_TELEMETRY::processReceivedPacket(uint16_t command, uint8_t data[], uint16_t length)
{
	
	switch(command)
	{
		case static_cast<uint16_t>(command_type::UART1_SEND_DATA):
			uart1.sendData(data, length);
			break;


		case static_cast<uint16_t>(command_type::TEST_PARAMETERS):
			std::memcpy(test_receive.buffer, data, length);
			break;


		default :
			//printf("Undefined Command! \n");
			break;
	}
	
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
CORE_TELEMETRY::CORE_TELEMETRY(const CORE_TELEMETRY& orig)
{
    
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
CORE_TELEMETRY::~CORE_TELEMETRY()
{
    
}



} /* End of namespace Telemetry */

