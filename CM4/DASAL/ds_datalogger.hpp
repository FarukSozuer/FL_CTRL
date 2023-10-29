/**
 ******************************************************************************
  * @file		  : ds_datalogger.hpp
  * @brief		: This file contains datalogger class
  * @author		: Baris Aycin
  * @date		  : 26.06.2020
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

#ifndef DS_DATALOGGER_HPP
#define DS_DATALOGGER_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



namespace Datalogger
{



/*
 * Begin of Macro Definitions
 */
static const uint32_t CHECK_STATE_MIN_FREE_SPACE_KB = 3145728; /*  3GB  */
static const uint32_t WRITE_STATE_MIN_FREE_SPACE_KB = 102400;  /* 100MB */
static const uint8_t  MAX_ERROR_COUNT  = 2;   /* Max: 255 */
static const uint16_t FILE_NAME_LENGHT = 256; /* Max: 256 */
static const uint16_t MAX_PACKET_SIZE  = 896; /* Max: 896 */
static const uint16_t MAX_ROW_COUNT    = 64;  /* Max: 64  */
static const uint16_t MAX_SIZE_WRITE   = MAX_PACKET_SIZE * MAX_ROW_COUNT;
 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class result_t : uint8_t
{
	RES_OK 		 = 0x00,
	RES_ERROR  = 0x01,
};



enum class scheduler_state_t : uint8_t
{
	INIT 	   = 0x00,
	STAND_BY = 0x01,
	CHECK    = 0x02,
	CREATE 	 = 0x03,
	WRITE 	 = 0x04,
	ERROR 	 = 0x05,
};



enum class update_file_cmd_t : uint8_t
{
	OPEN  = 0x00,
	WRITE = 0x01,
	CLOSE = 0x02,
};



enum class command_t : uint8_t
{
	STAND_BY = 0x00,
	WRITE    = 0x01,
};



/*
* @brief
*/
#pragma pack(1)
	union gnss_date_and_time_t
	{
		struct
		{
			uint32_t iTOW  ;
			uint16_t year  ;
			uint8_t  month ;
			uint8_t  day   ;
			uint8_t  hour  ;
			uint8_t  minute;
			uint8_t  second;
		}data;

		uint8_t buffer[sizeof(data)];

		gnss_date_and_time_t () : buffer{} {}
	};
#pragma pack()



/*
 * @brief
 */
union error_flags_t
{
	struct
	{
		uint32_t datalogger_err				 	      :1;
		uint32_t init_state_err 		       	  :1;
		uint32_t check_state_err  	       		:1;
		uint32_t check_state_free_space_err  	:1;
		uint32_t create_state_err   	       	:1;
		uint32_t write_state_write_err    	  :1;
		uint32_t write_state_check_err    	  :1;
		uint32_t write_state_free_space_err   :1;
		uint32_t write_state_open_file_err    :1;
		uint32_t write_state_close_file_err   :1;
		uint32_t format_name_err              :1;
		uint32_t set_packet_size_err          :1;
		uint32_t write_packet_size_err        :1;
		uint32_t reserved19                   :19;
	}bits;
	uint32_t all;
};



/*
 * @brief
 */
union status_flags_t
{
	struct
	{
		uint8_t config_params_received	  :1;
		uint8_t write_storage_device 		  :1;
		uint8_t reserved6                 :6;
	}bits;
	uint8_t all;
};
// End of Enum, Union and Struct Definitions



/*
 * Begin of DATA_LOGGER Class Definition
 */
class DATA_LOGGER
{
	public:
		DATA_LOGGER();

		command_t command = command_t::STAND_BY;
		gnss_date_and_time_t gnss_date_and_time;

		error_flags_t error_flags;
		status_flags_t status_flags;
		uint32_t datalogger_err_cntr = 0;

		void scheduler( void );
		void setPacket( uint8_t buffer[], uint16_t buffer_size );
		bool setPacketSizeWrite( uint16_t size );
		scheduler_state_t getState( void );

		DATA_LOGGER(const DATA_LOGGER& orig);
				virtual ~DATA_LOGGER();

	protected:

	private:

		uint32_t storage_free_space_kb = 0;
		uint32_t storage_capacity_kb = 0;
		uint8_t receive_buffer[MAX_SIZE_WRITE] = {};
		uint16_t receive_buffer_size = 0;

		char w_file_name[FILE_NAME_LENGHT] = "DASAL_10102020_121212.DAT";

		scheduler_state_t scheduler_state = scheduler_state_t::INIT;

		void initState	 ( void );
		void checkState	 ( void );
		void createState ( void );
		void standByState( void );
		void writeState	 ( void );
		void errorState	 ( void );

		void formatDate( uint16_t input,
										 char *output );

		result_t setFileName( void );
		result_t initialize ( void );
		result_t disconnect ( void );
		result_t checkDevice( void );
		result_t createFile	( void );
		result_t updateFile	( update_file_cmd_t update_file_cmd );
};
// End of DATA_LOGGER Class Definition


} //End of namespace Datalogger



/*
 * External Linkages
 */
extern Datalogger::DATA_LOGGER datalogger;
// End of External Linkages



#endif /* DS_DATALOGGER_HPP */
