/**
 ******************************************************************************
  * @file		  : ds_serializer.hpp
  * @brief		: This file contains serializer class
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

#ifndef DS_SERIALIZER_HPP
#define DS_SERIALIZER_HPP



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
const uint8_t  EMPTY  = 0;
const uint8_t  FULL   = 1;
const uint16_t MAX_BUFFER_SIZE = 58240;
const uint32_t MAX_SHARED_BUFF_ERR_CNT = 100;

const uint8_t  MASTER_CMD_STAND_BY = 0x00;
const uint8_t  MASTER_CMD_WRITE = 0x01;

const uint32_t SHARED_BUFFER_SIZE      = 0xF000;
const uint32_t SHARED_BUFFER_BASE_ADDR = 0x24000000;

const uint8_t  SHARED_BUFFER_COUNT     = 8;
const uint32_t SHARED_BUFF_STATUS_ADDR = 0x38007F70;
const uint32_t SLAVE_BUFF_INDEX_ADDR   = 0x3800FF70;
const uint32_t MASTER_BUFF_INDEX_ADDR  = 0x3800FF80;
const uint32_t MASTER_CMD_ADDR         = 0x3800FF90;
const uint32_t SLAVE_STATUS_ADDR       = 0x3800FFA0;
const uint32_t DATE_AND_TIME_ADDR      = 0x3800FFC0;
const uint32_t CONFIG_PARAMS_ADDR      = 0x38007FF0;
 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */

enum class serializer_state_t : uint8_t
{
	STAND_BY    = 0x00,
	WRITE       = 0x01,
};



enum class cmd_type : uint16_t
{
	SHARED_BUFFER_STATUS = 0x1001,
	SLAVE_BUFFER_INDEX 	 = 0x1002,
	MASTER_BUFFER_INDEX  = 0x1003,
	MASTER_CMD           = 0x1004,
	SLAVE_STATUS         = 0x1005,
	SHARED_BUFFER        = 0x1006,
	CONFIG_PARAMS        = 0x1007,
	DATE_AND_TIME        = 0x1008,
};



/*
 * @brief
 */
union slave_status_flags_t
{
	struct
	{
		uint8_t write             :1;
		uint8_t all_buffer_empty  :1;
		uint8_t datalogger_ready 	:1;
		uint8_t reserved5         :5;

	}bits;
	uint8_t all;
};


/*
 * @brief
 */
union slave_err_flags_t
{
	struct
	{
		uint8_t get_config_err           :1;
		uint8_t get_master_buff_idx_err  :1;
		uint8_t get_shared_buff_stat_err :1;
		uint8_t receive_packet_err       :1;
		uint8_t get_master_cmd_err       :1;
		uint8_t get_date_time_err        :1;
		uint8_t shared_buff_err          :1;
		uint8_t reserved1                :1;

	}bits;
	uint8_t all;
};

// End of Enum, Union and Struct Definitions



/*
 * Begin of SERIALIZER Class Definition
 */
class SERIALIZER
{
    public:

	SERIALIZER();

		void initialize	(void);
		void scheduler	(void);

		SERIALIZER(const SERIALIZER& orig);
        virtual ~SERIALIZER();

    protected:

    private:
			uint8_t datalogger_buffer[MAX_BUFFER_SIZE] = {0};
			uint8_t shared_buffer_index_slave = 0;
			uint8_t shared_buffer_index_master = 0;
			uint8_t shared_buffer_status = 0;

			uint16_t packet_count_write = 8;   /* Value must be 4, 8, 16, 32 or 64 */
			uint16_t packet_size        = 512; /* Value must be 128, 256, 384, 512, 640, 768 or 896 */
			uint16_t config_reserved    = 1;

			uint32_t shared_buff_err_cntr = 0;

			uint8_t master_cmd = MASTER_CMD_STAND_BY;
			slave_status_flags_t slave_status_flags;
			slave_err_flags_t slave_err_flags;

			void increaseSlaveSharedBufferIndex ( void );
			void setSlaveStatus                 ( void );
			bool getMasterCommand               ( void );
			void setSlaveSharedBufferIndex      ( void );
			bool getMasterSharedBufferIndex     ( void );
			void clearSharedBufferStatus        ( void );
			bool getSharedBufferStatus          ( void );
			void setSharedBufferStatus          ( void );
			bool getDateAndTime                 ( void );
			bool readFromSharedBuffer           ( void );
			bool getConfigParams                ( void );

};
// End of SERIALIZER Class Definition



} //End of namespace Datalogger



/*
 * External Linkages
 */
extern Datalogger::SERIALIZER serializer;
// End of External Linkages



#endif /* DS_SERIALIZER_HPP */
