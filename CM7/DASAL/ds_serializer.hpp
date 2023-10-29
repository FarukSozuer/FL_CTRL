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
const uint32_t MAX_SHARED_BUFF_ERR_CNT = 100;
const uint8_t  SHARED_BUFFER_COUNT     = 8;
const uint32_t SHARED_BUFFER_SIZE      = 0xF000;
const uint32_t SHARED_BUFFER_ADDR      = 0x24000000;
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



/*
* @brief
*/
#pragma pack(1)
	union dl_packet_t
	{
		struct
		{
			//TODO: Can be deleted after tests are done
			double   dummy_data_000_d64;
			double   dummy_data_001_d64;
			double   dummy_data_002_d64;
			uint64_t dummy_data_003_u64;
			uint64_t dummy_data_004_u64;
			uint64_t dummy_data_005_u64;
			int64_t  dummy_data_006_s64;
			int64_t  dummy_data_007_s64;
			int64_t  dummy_data_008_s64;
			float    dummy_data_009_f32;
			float    dummy_data_010_f32;
			float    dummy_data_011_f32;
			uint32_t dummy_data_012_u32;
			uint32_t dummy_data_013_u32;
			uint32_t dummy_data_014_u32;
			int32_t  dummy_data_015_s32;
			int32_t  dummy_data_016_s32;
			int32_t  dummy_data_017_s32;
			uint16_t dummy_data_018_u16;
			uint16_t dummy_data_019_u16;
			uint16_t dummy_data_020_u16;
			int16_t  dummy_data_021_s16;
			int16_t  dummy_data_022_s16;
			int16_t  dummy_data_023_s16;
			uint8_t  dummy_data_024_u8;
			uint8_t  dummy_data_025_u8;
			uint8_t  dummy_data_026_u8;
			uint8_t  dummy_data_027_u8;
			int8_t   dummy_data_028_s8;
			int8_t   dummy_data_029_s8;
		  int8_t   dummy_data_030_s8;
			int8_t   dummy_data_031_s8; /* 128 Bytes */
			double   dummy_data_032_d64;
			double   dummy_data_033_d64;
			double   dummy_data_034_d64;
			uint64_t dummy_data_035_u64;
			uint64_t dummy_data_036_u64;
			uint64_t dummy_data_037_u64;
			int64_t  dummy_data_038_s64;
			int64_t  dummy_data_039_s64;
			int64_t  dummy_data_040_s64;
			float    dummy_data_041_f32;
			float    dummy_data_042_f32;
			float    dummy_data_043_f32;
			uint32_t dummy_data_044_u32;
			uint32_t dummy_data_045_u32;
			uint32_t dummy_data_046_u32;
			int32_t  dummy_data_047_s32;
			int32_t  dummy_data_048_s32;
			int32_t  dummy_data_049_s32;
			uint16_t dummy_data_050_u16;
			uint16_t dummy_data_051_u16;
			uint16_t dummy_data_052_u16;
			int16_t  dummy_data_053_s16;
			int16_t  dummy_data_054_s16;
			int16_t  dummy_data_055_s16;
			uint8_t  dummy_data_056_u8;
			uint8_t  dummy_data_057_u8;
			uint8_t  dummy_data_058_u8;
			uint8_t  dummy_data_059_u8;
			int8_t   dummy_data_060_s8;
			int8_t   dummy_data_061_s8;
			int8_t   dummy_data_062_s8;
			int8_t   dummy_data_063_s8; /* 256 Bytes */
			double   dummy_data_064_d64;
			double   dummy_data_065_d64;
			double   dummy_data_066_d64;
			uint64_t dummy_data_067_u64;
			uint64_t dummy_data_068_u64;
			uint64_t dummy_data_069_u64;
			int64_t  dummy_data_070_s64;
			int64_t  dummy_data_071_s64;
			int64_t  dummy_data_072_s64;
			float    dummy_data_073_f32;
			float    dummy_data_074_f32;
			float    dummy_data_075_f32;
			uint32_t dummy_data_076_u32;
			uint32_t dummy_data_077_u32;
			uint32_t dummy_data_078_u32;
			int32_t  dummy_data_079_s32;
			int32_t  dummy_data_080_s32;
			int32_t  dummy_data_081_s32;
			uint16_t dummy_data_082_u16;
			uint16_t dummy_data_083_u16;
			uint16_t dummy_data_084_u16;
			int16_t  dummy_data_085_s16;
			int16_t  dummy_data_086_s16;
			int16_t  dummy_data_087_s16;
			uint8_t  dummy_data_088_u8;
			uint8_t  dummy_data_089_u8;
			uint8_t  dummy_data_090_u8;
			uint8_t  dummy_data_091_u8;
			int8_t   dummy_data_092_s8;
			int8_t   dummy_data_093_s8;
			int8_t   dummy_data_094_s8;
			int8_t   dummy_data_095_s8; /* 384 Bytes */
			double   dummy_data_096_d64;
			double   dummy_data_097_d64;
			double   dummy_data_098_d64;
			uint64_t dummy_data_099_u64;
			uint64_t dummy_data_100_u64;
			uint64_t dummy_data_101_u64;
			int64_t  dummy_data_102_s64;
			int64_t  dummy_data_103_s64;
			int64_t  dummy_data_104_s64;
			float    dummy_data_105_f32;
			float    dummy_data_106_f32;
			float    dummy_data_107_f32;
			uint32_t dummy_data_108_u32;
			uint32_t dummy_data_109_u32;
			uint32_t dummy_data_110_u32;
			int32_t  dummy_data_111_s32;
			int32_t  dummy_data_112_s32;
			int32_t  dummy_data_113_s32;
			uint16_t dummy_data_114_u16;
			uint16_t dummy_data_115_u16;
			uint16_t dummy_data_116_u16;
			int16_t  dummy_data_117_s16;
			int16_t  dummy_data_118_s16;
			int16_t  dummy_data_119_s16;
			uint8_t  dummy_data_120_u8;
			uint8_t  dummy_data_121_u8;
			uint8_t  dummy_data_122_u8;
			uint8_t  dummy_data_123_u8;
			int8_t   dummy_data_124_s8;
			int8_t   dummy_data_125_s8;
			int8_t   dummy_data_126_s8;
			int8_t   dummy_data_127_s8; /* 512 Bytes */
			double   dummy_data_128_d64;
			double   dummy_data_129_d64;
			double   dummy_data_130_d64;
			uint64_t dummy_data_131_u64;
			uint64_t dummy_data_132_u64;
			uint64_t dummy_data_133_u64;
			int64_t  dummy_data_134_s64;
			int64_t  dummy_data_135_s64;
			int64_t  dummy_data_136_s64;
			float    dummy_data_137_f32;
			float    dummy_data_138_f32;
			float    dummy_data_139_f32;
			uint32_t dummy_data_140_u32;
			uint32_t dummy_data_141_u32;
			uint32_t dummy_data_142_u32;
			int32_t  dummy_data_143_s32;
			int32_t  dummy_data_144_s32;
			int32_t  dummy_data_145_s32;
			uint16_t dummy_data_146_u16;
			uint16_t dummy_data_147_u16;
			uint16_t dummy_data_148_u16;
			int16_t  dummy_data_149_s16;
			int16_t  dummy_data_150_s16;
			int16_t  dummy_data_151_s16;
			uint8_t  dummy_data_152_u8;
			uint8_t  dummy_data_153_u8;
			uint8_t  dummy_data_154_u8;
			uint8_t  dummy_data_155_u8;
			int8_t   dummy_data_156_s8;
			int8_t   dummy_data_157_s8;
			int8_t   dummy_data_158_s8;
			int8_t   dummy_data_159_s8; /* 640 Bytes */
			double   dummy_data_160_d64;
			double   dummy_data_161_d64;
			double   dummy_data_162_d64;
			uint64_t dummy_data_163_u64;
			uint64_t dummy_data_164_u64;
			uint64_t dummy_data_165_u64;
			int64_t  dummy_data_166_s64;
			int64_t  dummy_data_167_s64;
			int64_t  dummy_data_168_s64;
			float    dummy_data_169_f32;
			float    dummy_data_170_f32;
			float    dummy_data_171_f32;
			uint32_t dummy_data_172_u32;
			uint32_t dummy_data_173_u32;
			uint32_t dummy_data_174_u32;
			int32_t  dummy_data_175_s32;
			int32_t  dummy_data_176_s32;
			int32_t  dummy_data_177_s32;
			uint16_t dummy_data_178_u16;
			uint16_t dummy_data_179_u16;
			uint16_t dummy_data_180_u16;
			int16_t  dummy_data_181_s16;
			int16_t  dummy_data_182_s16;
			int16_t  dummy_data_183_s16;
			uint8_t  dummy_data_184_u8;
			uint8_t  dummy_data_185_u8;
			uint8_t  dummy_data_186_u8;
			uint8_t  dummy_data_187_u8;
			int8_t   dummy_data_188_s8;
			int8_t   dummy_data_189_s8;
			int8_t   dummy_data_190_s8;
			int8_t   dummy_data_191_s8; /* 768 Bytes */
			double   dummy_data_192_d64;
			double   dummy_data_193_d64;
			double   dummy_data_194_d64;
			uint64_t dummy_data_195_u64;
			uint64_t dummy_data_196_u64;
			uint64_t dummy_data_197_u64;
			int64_t  dummy_data_198_s64;
			int64_t  dummy_data_199_s64;
			int64_t  dummy_data_200_s64;
			float    dummy_data_201_f32;
			float    dummy_data_202_f32;
			float    dummy_data_203_f32;
			uint32_t dummy_data_204_u32;
			uint32_t dummy_data_205_u32;
			uint32_t dummy_data_206_u32;
			int32_t  dummy_data_207_s32;
			int32_t  dummy_data_208_s32;
			int32_t  dummy_data_209_s32;
			uint16_t dummy_data_210_u16;
			uint16_t dummy_data_211_u16;
			uint16_t dummy_data_212_u16;
			int16_t  dummy_data_213_s16;
			int16_t  dummy_data_214_s16;
			int16_t  dummy_data_215_s16;
			uint8_t  dummy_data_216_u8;
			uint8_t  dummy_data_217_u8;
			uint8_t  dummy_data_218_u8;
			uint8_t  dummy_data_219_u8;
			int8_t   dummy_data_220_s8;
			int8_t   dummy_data_221_s8;
			int8_t   dummy_data_222_s8;
			int8_t   dummy_data_223_s8; /* 896 Bytes */

		}data;

		uint8_t buffer[sizeof(data)];

		dl_packet_t () : buffer{} {}
	};
#pragma pack()




enum class scheduler_freq_t : uint16_t
{
	LOOP_200_HZ = 64,
	LOOP_100_HZ = 32,
	LOOP_50_HZ  = 16,
	LOOP_20_HZ  = 8,
	LOOP_10_HZ  = 4,
	LOOP_5_HZ   = 4,
	LOOP_2_HZ   = 4,
	LOOP_1_HZ   = 4,
};



enum class packet_size_t : uint16_t
{
	SIZE_128 = 128,
	SIZE_256 = 256,
	SIZE_384 = 384,
	SIZE_512 = 512,
	SIZE_640 = 640,
	SIZE_768 = 768,
	SIZE_896 = 896,
};



enum class serializer_state_t : uint8_t
{
	STAND_BY = 0x00,
	WRITE    = 0x01,
	ERROR    = 0x02,
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



enum class command_t : uint8_t
{
	STAND_BY = 0x00,
	WRITE    = 0x01,
};



enum class packet_state_t : uint8_t
{
	READY     = 0x00,
	NOT_READY = 0x01,
};



/*
 * @brief
 */
union master_err_flags_t
{
	struct
	{
		uint8_t overwrite_err 		       :1;
		uint8_t synchronization_err      :1;
		uint8_t config_mismatch_err      :1;
		uint8_t get_slave_buff_idx_err   :1;
		uint8_t get_shared_buff_stat_err :1;
		uint8_t get_slave_status_err     :1;
		uint8_t shared_buff_err          :1;
		uint8_t reserved1     		       :1;

	}bits;
	uint8_t all;
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
union datalogger_err_flags_t
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



// End of Enum, Union and Struct Definitions



/*
 * Begin of SERIALIZER Class Definition
 */
class SERIALIZER
{
	public:
		SERIALIZER(scheduler_freq_t freq, packet_size_t size);

		dl_packet_t dl_packet;
		gnss_date_and_time_t gnss_date_and_time;

		void initialize( void );
		void scheduler ( void );

		uint8_t  getMasterErrFlags    ( void );
		uint8_t  getSlaveStatusFlags  ( void );
		uint32_t getDataloggerErrFlags( void );
		uint32_t getOverwriteErrCntr  ( void );
		uint32_t getDataloggerErrCntr ( void );
		uint32_t getSharedBuffErrCntr ( void );

		bool setCommand( command_t serializer_cmd );

	SERIALIZER(const SERIALIZER& orig);
		virtual ~SERIALIZER();

	protected:

	private:
		uint16_t packet_count_write;
		uint16_t packet_size;
		uint16_t config_reserved = 1;

		uint8_t  shared_buffer_index_slave = 0;
		uint8_t  shared_buffer_index_master = 0;
		uint8_t  last_buffer_index = 0;
		uint8_t  shared_buffer_status = 0;
		uint32_t packet_counter = 0;
		uint32_t overwrite_err_cntr = 0;
		uint32_t datalogger_err_cntr = 0;
		uint32_t shared_buff_err_cntr = 0;

		bool writing_completed = false;

		master_err_flags_t master_err_flags;
		slave_status_flags_t slave_status_flags;
		datalogger_err_flags_t datalogger_err_flags;

		command_t command = command_t::STAND_BY;
		command_t master_cmd = command_t::STAND_BY;

		void updatePacket     	    				 ( void );
		void updateDateAndTime               ( void );
		void updateMasterStatus 						 ( void );
		void setMasterCommand   						 ( void );
		bool getSlaveStatus    							 ( void );
		void writeToSharedBuffer 	    			 ( void );
		void increaseMasterSharedBufferIndex ( void );
		bool getSlaveSharedBufferIndex       ( void );
		void setMasterSharedBufferIndex      ( void );
		void clearSharedBuffStatusBytes      ( void );
		bool getSharedBufferStatus           ( void );
		void setSharedBufferStatus           ( void );
		void setConfigParams                 ( void );
};
// End of SERIALIZER Class Definition



} //End of namespace Datalogger



/*
 * External Linkages
 */
extern Datalogger::SERIALIZER serializer;
// End of External Linkages



#endif /* DS_SERIALIZER_HPP */
