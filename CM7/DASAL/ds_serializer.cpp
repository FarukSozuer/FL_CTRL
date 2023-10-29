/**
 ******************************************************************************
  * @file		  : ds_serializer.cpp
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

/*
 * Begin of Includes
 */
#include <string.h>
#include "ds_serializer.hpp"
#include "ds_telemetry_core.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Datalogger::SERIALIZER serializer(Datalogger::scheduler_freq_t::LOOP_100_HZ,
															 	  Datalogger::packet_size_t::SIZE_896);
// End of Object Definitions



namespace Datalogger
{


/**
  * @brief      Default Constructor
  *
  * @param[in]  scheduler_freq_t    freq
  *             packet_size_t       size
  *
  * @return 	  void
  */
SERIALIZER::SERIALIZER(scheduler_freq_t freq, packet_size_t size):
packet_count_write(static_cast<uint16_t>(freq)),
packet_size(static_cast<uint16_t>(size))
{
	uint16_t datalogger_packet_size = sizeof(dl_packet.buffer);

	master_err_flags.all = 0;
	slave_status_flags.all = 0;
	datalogger_err_flags.all = 0;

	if(packet_size != datalogger_packet_size)
	{
		master_err_flags.bits.config_mismatch_err = 1;
	}
}



/**
  * @brief 		  Initializes the variables
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::initialize(void)
{
	master_cmd = command_t::STAND_BY;
	setConfigParams();
	setMasterSharedBufferIndex();
	clearSharedBuffStatusBytes();
	updateMasterStatus();
}



/**
  * @brief 		  Scheduler (Maximum calling frequency:200Hz)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::scheduler(void)
{
	static serializer_state_t serializer_state = serializer_state_t::STAND_BY;
	static bool update_buff_idx_master = true;
	static bool first_write = true;

	updateDateAndTime();

	if((getSlaveStatus()) && (!master_err_flags.bits.shared_buff_err))
	{
		switch(serializer_state)
		{
			case serializer_state_t::STAND_BY:
				if((master_cmd == command_t::STAND_BY)&&
					 (slave_status_flags.bits.datalogger_ready))
				{
					if(command == command_t::WRITE)
					{
						shared_buffer_index_master = 0;
						setMasterSharedBufferIndex();

						first_write = true;
						writing_completed = false;
						update_buff_idx_master = true;
						packet_counter = 0;
						clearSharedBuffStatusBytes();
						serializer_state = serializer_state_t::WRITE;
					}
				}
				break;


			case serializer_state_t::WRITE:
				if(!datalogger_err_flags.bits.datalogger_err)
				{
					updatePacket();
					if(update_buff_idx_master == true)
					{
						update_buff_idx_master = false;

						if(first_write)
						{
							first_write = false;
						}
						else
						{
							last_buffer_index = shared_buffer_index_master;
							if(getSlaveSharedBufferIndex())
							{
								increaseMasterSharedBufferIndex();
								if(shared_buffer_index_master == shared_buffer_index_slave)
								{
									master_err_flags.bits.synchronization_err = 1;
									increaseMasterSharedBufferIndex();
									last_buffer_index = shared_buffer_index_master;
									increaseMasterSharedBufferIndex();
								}
								else
								{
									master_err_flags.bits.synchronization_err = 0;
								}
								setMasterSharedBufferIndex();
							}
						}

						if(command != command_t::WRITE)
						{
							writing_completed = true;
							serializer_state = serializer_state_t::STAND_BY;
						}
					}

					if(serializer_state == serializer_state_t::WRITE)
					{
						writeToSharedBuffer();
						packet_counter++;

						if(packet_counter >= static_cast<uint32_t>(packet_count_write))
						{
							packet_counter = 0;
							update_buff_idx_master = true;

							if(getSharedBufferStatus())
							{
								if(shared_buffer_status == EMPTY)
								{
									master_err_flags.bits.overwrite_err = 0;
									shared_buffer_status = FULL;
									setSharedBufferStatus();
								}
								else
								{
									master_err_flags.bits.overwrite_err = 1;
									overwrite_err_cntr++;
								}
							}
						}
					}
				}
				else
				{
					if(master_cmd == command_t::STAND_BY)
					{
						serializer_state = serializer_state_t::STAND_BY;
					}
				}
				break;


			default:
				break;
		}
	}

	if((shared_buff_err_cntr > MAX_SHARED_BUFF_ERR_CNT)&&
		 (!master_err_flags.bits.shared_buff_err))
	{
		master_err_flags.bits.shared_buff_err = 1;
		serializer_state = serializer_state_t::STAND_BY;
	}

	updateMasterStatus();
}



/**
  * @brief 		  Updates packet to be logged
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::updatePacket(void)
{
	//TODO: Can be deleted after tests are done
	dl_packet.data.dummy_data_000_d64 = -12345.678901231;
	dl_packet.data.dummy_data_001_d64 = 12345.678901232;
	dl_packet.data.dummy_data_002_d64 = -12345.678901233;
	dl_packet.data.dummy_data_003_u64 = 0;
	dl_packet.data.dummy_data_004_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_005_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_006_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_007_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_008_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_009_f32 = -12.3450;
	dl_packet.data.dummy_data_010_f32 = 12.3451;
	dl_packet.data.dummy_data_011_f32 = -12.3452;
	dl_packet.data.dummy_data_012_u32 = 3234567890;
	dl_packet.data.dummy_data_013_u32 = 3234567891;
	dl_packet.data.dummy_data_014_u32 = 3234567892;
	dl_packet.data.dummy_data_015_s32 = -1234567890;
	dl_packet.data.dummy_data_016_s32 = 1234567891;
	dl_packet.data.dummy_data_017_s32 = -1234567892;
	dl_packet.data.dummy_data_018_u16 = 30123;
	dl_packet.data.dummy_data_019_u16 = 30124;
	dl_packet.data.dummy_data_020_u16 = 30125;
	dl_packet.data.dummy_data_021_s16 = -15123;
	dl_packet.data.dummy_data_022_s16 = 15124;
	dl_packet.data.dummy_data_023_s16 = -15125;
	dl_packet.data.dummy_data_024_u8  = 22;
	dl_packet.data.dummy_data_025_u8  = 23;
	dl_packet.data.dummy_data_026_u8  = 24;
	dl_packet.data.dummy_data_027_u8  = 25;
	dl_packet.data.dummy_data_028_s8  = 4;
	dl_packet.data.dummy_data_029_s8  = 26;
	dl_packet.data.dummy_data_030_s8  = -27;
	dl_packet.data.dummy_data_031_s8  = 28; /* 128 Bytes */
	dl_packet.data.dummy_data_032_d64 = -12345.678901231;
	dl_packet.data.dummy_data_033_d64 = 12345.678901232;
	dl_packet.data.dummy_data_034_d64 = -12345.678901233;
	dl_packet.data.dummy_data_035_u64 = 0;
	dl_packet.data.dummy_data_036_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_037_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_038_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_039_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_040_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_041_f32 = -12.3450;
	dl_packet.data.dummy_data_042_f32 = 12.3451;
	dl_packet.data.dummy_data_043_f32 = -12.3452;
	dl_packet.data.dummy_data_044_u32 = 3234567890;
	dl_packet.data.dummy_data_045_u32 = 3234567891;
	dl_packet.data.dummy_data_046_u32 = 3234567892;
	dl_packet.data.dummy_data_047_s32 = -1234567890;
	dl_packet.data.dummy_data_048_s32 = 1234567891;
	dl_packet.data.dummy_data_049_s32 = -1234567892;
	dl_packet.data.dummy_data_050_u16 = 30123;
	dl_packet.data.dummy_data_051_u16 = 30124;
	dl_packet.data.dummy_data_052_u16 = 30125;
	dl_packet.data.dummy_data_053_s16 = -15123;
	dl_packet.data.dummy_data_054_s16 = 15124;
	dl_packet.data.dummy_data_055_s16 = -15125;
	dl_packet.data.dummy_data_056_u8  = 22;
	dl_packet.data.dummy_data_057_u8  = 23;
	dl_packet.data.dummy_data_058_u8  = 24;
	dl_packet.data.dummy_data_059_u8  = 25;
	dl_packet.data.dummy_data_060_s8  = 4;
	dl_packet.data.dummy_data_061_s8  = 26;
	dl_packet.data.dummy_data_062_s8  = -27;
	dl_packet.data.dummy_data_063_s8  = 28; /* 256 Bytes */
	dl_packet.data.dummy_data_064_d64 = -12345.678901231;
	dl_packet.data.dummy_data_065_d64 = 12345.678901232;
	dl_packet.data.dummy_data_066_d64 = -12345.678901233;
	dl_packet.data.dummy_data_067_u64 = 0;
	dl_packet.data.dummy_data_068_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_069_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_070_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_071_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_072_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_073_f32 = -12.3450;
	dl_packet.data.dummy_data_074_f32 = 12.3451;
	dl_packet.data.dummy_data_075_f32 = -12.3452;
	dl_packet.data.dummy_data_076_u32 = 3234567890;
	dl_packet.data.dummy_data_077_u32 = 3234567891;
	dl_packet.data.dummy_data_078_u32 = 3234567892;
	dl_packet.data.dummy_data_079_s32 = -1234567890;
	dl_packet.data.dummy_data_080_s32 = 1234567891;
	dl_packet.data.dummy_data_081_s32 = -1234567892;
	dl_packet.data.dummy_data_082_u16 = 30123;
	dl_packet.data.dummy_data_083_u16 = 30124;
	dl_packet.data.dummy_data_084_u16 = 30125;
	dl_packet.data.dummy_data_085_s16 = -15123;
	dl_packet.data.dummy_data_086_s16 = 15124;
	dl_packet.data.dummy_data_087_s16 = -15125;
	dl_packet.data.dummy_data_088_u8  = 22;
	dl_packet.data.dummy_data_089_u8  = 23;
	dl_packet.data.dummy_data_090_u8  = 24;
	dl_packet.data.dummy_data_091_u8  = 25;
	dl_packet.data.dummy_data_092_s8  = 4;
	dl_packet.data.dummy_data_093_s8  = 26;
	dl_packet.data.dummy_data_094_s8  = -27;
	dl_packet.data.dummy_data_095_s8  = 28; /* 384 Bytes */
	dl_packet.data.dummy_data_096_d64 = -12345.678901231;
	dl_packet.data.dummy_data_097_d64 = 12345.678901232;
	dl_packet.data.dummy_data_098_d64 = -12345.678901233;
	dl_packet.data.dummy_data_099_u64 = 0;
	dl_packet.data.dummy_data_100_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_101_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_102_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_103_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_104_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_105_f32 = -12.3450;
	dl_packet.data.dummy_data_106_f32 = 12.3451;
	dl_packet.data.dummy_data_107_f32 = -12.3452;
	dl_packet.data.dummy_data_108_u32 = 3234567890;
	dl_packet.data.dummy_data_109_u32 = 3234567891;
	dl_packet.data.dummy_data_110_u32 = 3234567892;
	dl_packet.data.dummy_data_111_s32 = -1234567890;
	dl_packet.data.dummy_data_112_s32 = 1234567891;
	dl_packet.data.dummy_data_113_s32 = -1234567892;
	dl_packet.data.dummy_data_114_u16 = 30123;
	dl_packet.data.dummy_data_115_u16 = 30124;
	dl_packet.data.dummy_data_116_u16 = 30125;
	dl_packet.data.dummy_data_117_s16 = -15123;
	dl_packet.data.dummy_data_118_s16 = 15124;
	dl_packet.data.dummy_data_119_s16 = -15125;
	dl_packet.data.dummy_data_120_u8  = 25;
	dl_packet.data.dummy_data_121_u8  = 26;
	dl_packet.data.dummy_data_122_u8  = 27;
	dl_packet.data.dummy_data_123_u8  = 28;
	dl_packet.data.dummy_data_124_s8  = -25;
	dl_packet.data.dummy_data_125_s8  = 26;
	dl_packet.data.dummy_data_126_s8  = -27;
	dl_packet.data.dummy_data_127_s8  = 28; /* 512 Bytes */
	dl_packet.data.dummy_data_128_d64 = -12345.678901231;
	dl_packet.data.dummy_data_129_d64 = 12345.678901232;
	dl_packet.data.dummy_data_130_d64 = -12345.678901233;
	dl_packet.data.dummy_data_131_u64 = 0;
	dl_packet.data.dummy_data_132_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_133_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_134_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_135_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_136_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_137_f32 = -12.3450;
	dl_packet.data.dummy_data_138_f32 = 12.3451;
	dl_packet.data.dummy_data_139_f32 = -12.3452;
	dl_packet.data.dummy_data_140_u32 = 3234567890;
	dl_packet.data.dummy_data_141_u32 = 3234567891;
	dl_packet.data.dummy_data_142_u32 = 3234567892;
	dl_packet.data.dummy_data_143_s32 = -1234567890;
	dl_packet.data.dummy_data_144_s32 = 1234567891;
	dl_packet.data.dummy_data_145_s32 = -1234567892;
	dl_packet.data.dummy_data_146_u16 = 30123;
	dl_packet.data.dummy_data_147_u16 = 30124;
	dl_packet.data.dummy_data_148_u16 = 30125;
	dl_packet.data.dummy_data_149_s16 = -15123;
	dl_packet.data.dummy_data_150_s16 = 15124;
	dl_packet.data.dummy_data_151_s16 = -15125;
	dl_packet.data.dummy_data_152_u8  = 25;
	dl_packet.data.dummy_data_153_u8  = 26;
	dl_packet.data.dummy_data_154_u8  = 27;
	dl_packet.data.dummy_data_155_u8  = 28;
	dl_packet.data.dummy_data_156_s8  = -25;
	dl_packet.data.dummy_data_157_s8  = 26;
	dl_packet.data.dummy_data_158_s8  = -27;
	dl_packet.data.dummy_data_159_s8  = 28; /* 640 Bytes */
	dl_packet.data.dummy_data_160_d64 = -12345.678901231;
	dl_packet.data.dummy_data_161_d64 = 12345.678901232;
	dl_packet.data.dummy_data_162_d64 = -12345.678901233;
	dl_packet.data.dummy_data_163_u64 = 0;
	dl_packet.data.dummy_data_164_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_165_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_166_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_167_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_168_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_169_f32 = -12.3450;
	dl_packet.data.dummy_data_170_f32 = 12.3451;
	dl_packet.data.dummy_data_171_f32 = -12.3452;
	dl_packet.data.dummy_data_172_u32 = 3234567890;
	dl_packet.data.dummy_data_173_u32 = 3234567891;
	dl_packet.data.dummy_data_174_u32 = 3234567892;
	dl_packet.data.dummy_data_175_s32 = -1234567890;
	dl_packet.data.dummy_data_176_s32 = 1234567891;
	dl_packet.data.dummy_data_177_s32 = -1234567892;
	dl_packet.data.dummy_data_178_u16 = 30123;
	dl_packet.data.dummy_data_179_u16 = 30124;
	dl_packet.data.dummy_data_180_u16 = 30125;
	dl_packet.data.dummy_data_181_s16 = -15123;
	dl_packet.data.dummy_data_182_s16 = 15124;
	dl_packet.data.dummy_data_183_s16 = -15125;
	dl_packet.data.dummy_data_184_u8  = 25;
	dl_packet.data.dummy_data_185_u8  = 26;
	dl_packet.data.dummy_data_186_u8  = 27;
	dl_packet.data.dummy_data_187_u8  = 28;
	dl_packet.data.dummy_data_188_s8  = -25;
	dl_packet.data.dummy_data_189_s8  = 26;
	dl_packet.data.dummy_data_190_s8  = -27;
	dl_packet.data.dummy_data_191_s8  = 28; /* 768 Bytes */
	dl_packet.data.dummy_data_192_d64 = -12345.678901231;
	dl_packet.data.dummy_data_193_d64 = 12345.678901232;
	dl_packet.data.dummy_data_194_d64 = -12345.678901233;
	dl_packet.data.dummy_data_195_u64 = 0;
	dl_packet.data.dummy_data_196_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_197_u64 = 2345678901234567890;
	dl_packet.data.dummy_data_198_s64 = -1234567890123456780;
	dl_packet.data.dummy_data_199_s64 = 1234567890123456781;
	dl_packet.data.dummy_data_200_s64 = -1234567890123456782;
	dl_packet.data.dummy_data_201_f32 = -12.3450;
	dl_packet.data.dummy_data_202_f32 = 12.3451;
	dl_packet.data.dummy_data_203_f32 = -12.3452;
	dl_packet.data.dummy_data_204_u32 = 3234567890;
	dl_packet.data.dummy_data_205_u32 = 3234567891;
	dl_packet.data.dummy_data_206_u32 = 3234567892;
	dl_packet.data.dummy_data_207_s32 = -1234567890;
	dl_packet.data.dummy_data_208_s32 = 1234567891;
	dl_packet.data.dummy_data_209_s32 = -1234567892;
	dl_packet.data.dummy_data_210_u16 = 30123;
	dl_packet.data.dummy_data_211_u16 = 30124;
	dl_packet.data.dummy_data_212_u16 = 30125;
	dl_packet.data.dummy_data_213_s16 = -15123;
	dl_packet.data.dummy_data_214_s16 = 15124;
	dl_packet.data.dummy_data_215_s16 = -15125;
	dl_packet.data.dummy_data_216_u8  = 25;
	dl_packet.data.dummy_data_217_u8  = 26;
	dl_packet.data.dummy_data_218_u8  = 27;
	dl_packet.data.dummy_data_219_u8  = 28;
	dl_packet.data.dummy_data_220_s8  = -25;
	dl_packet.data.dummy_data_221_s8  = 26;
	dl_packet.data.dummy_data_222_s8  = -27;
	dl_packet.data.dummy_data_223_s8  = 28; /* 896 Bytes */
}



/**
  * @brief 		  Sends the date and time information to the slave
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::updateDateAndTime(void)
{
	const uint16_t BUFFER_SIZE = sizeof(gnss_date_and_time.buffer);
		  uint8_t  set_buffer[BUFFER_SIZE] = {0};

	// TODO: Will be deleted after GNSS packet added - Test
	/***********************************************************************************************************************************/
	gnss_date_and_time.data.iTOW   = 123456789;
	gnss_date_and_time.data.year   = 2020;
	gnss_date_and_time.data.month  = 1;
	gnss_date_and_time.data.day    = 1;
	gnss_date_and_time.data.hour   = 1;
	gnss_date_and_time.data.minute = 1;
	gnss_date_and_time.data.second = 1;
	/***********************************************************************************************************************************/

	memcpy(set_buffer,gnss_date_and_time.buffer,BUFFER_SIZE);

	telemetry_core.sendPacketToSharedBuff(DATE_AND_TIME_ADDR,static_cast<uint16_t>(cmd_type::DATE_AND_TIME),set_buffer,BUFFER_SIZE);
}



/**
  * @brief 		  Updates the master status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::updateMasterStatus(void)
{
	if((command == command_t::WRITE)&&(!datalogger_err_flags.bits.datalogger_err)&&(!master_err_flags.bits.shared_buff_err))
	{
		if(master_cmd == command_t::STAND_BY)
		{
			master_cmd = command_t::WRITE;
		}
	}
	else
	{
		if(master_cmd == command_t::WRITE)
		{
			getSlaveSharedBufferIndex();

			if(((shared_buffer_index_slave == last_buffer_index) && (writing_completed == true))||
				 (datalogger_err_flags.bits.datalogger_err)||
				 (master_err_flags.bits.shared_buff_err))
			{
				master_cmd = command_t::STAND_BY;
			}
		}
	}

	setMasterCommand();
}



/**
  * @brief 		  Sends master commands to slave
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::setMasterCommand(void)
{
	      uint8_t  master_command = static_cast<uint8_t>(master_cmd);
	const uint16_t BUFFER_SIZE = sizeof(master_command);
				uint8_t  set_buffer[BUFFER_SIZE] = {0};

	memcpy(set_buffer,&master_command,BUFFER_SIZE);

	telemetry_core.sendPacketToSharedBuff(MASTER_CMD_ADDR,static_cast<uint16_t>(cmd_type::MASTER_CMD),set_buffer,BUFFER_SIZE);
}



/**
  * @brief 		  Gets slave status byte(s) and data logger status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getSlaveStatus(void)
{
	const uint16_t BUFFER_SIZE = sizeof(slave_status_flags.all) +
									             sizeof(datalogger_err_flags.all) +
									             sizeof(datalogger_err_cntr);

				uint8_t  get_buffer[BUFFER_SIZE] = {0};
				bool     success = false;

	success = telemetry_core.receivePacketFromSharedBuff(SLAVE_STATUS_ADDR, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&slave_status_flags.all, get_buffer, sizeof(slave_status_flags.all));
		memcpy(&datalogger_err_flags.all, &get_buffer[sizeof(slave_status_flags.all)], sizeof(datalogger_err_flags.all));
		memcpy(&datalogger_err_cntr, &get_buffer[sizeof(slave_status_flags.all) + sizeof(datalogger_err_flags.all)], sizeof(datalogger_err_cntr));

		master_err_flags.bits.get_slave_status_err = 0;
	}
	else
	{
		master_err_flags.bits.get_slave_status_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief 		  Sends packet to the shared buffer
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::writeToSharedBuffer(void)
{
	uint32_t address = SHARED_BUFFER_ADDR +
			           (shared_buffer_index_master * SHARED_BUFFER_SIZE) +
			  		   (packet_counter * (sizeof(dl_packet.buffer) + static_cast<uint32_t>(9)));

	telemetry_core.sendPacketToSharedBuff(address,
										  static_cast<uint16_t>(cmd_type::SHARED_BUFFER),
										  dl_packet.buffer,
										  sizeof(dl_packet.buffer));
}



/**
  * @brief 		  Increases the master buffer index byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return     void	Nothing
  */
void SERIALIZER::increaseMasterSharedBufferIndex(void)
{
	if(shared_buffer_index_master >= (SHARED_BUFFER_COUNT - 1))
	{
		shared_buffer_index_master = 0;
	}
	else
	{
		shared_buffer_index_master++;
	}
}



/**
  * @brief 		  Gets the slave buffer index byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getSlaveSharedBufferIndex(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_index_slave);
				bool     success = false;
				uint8_t  get_buffer[BUFFER_SIZE] = {0};

	success = telemetry_core.receivePacketFromSharedBuff(SLAVE_BUFF_INDEX_ADDR, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&shared_buffer_index_slave,get_buffer,BUFFER_SIZE);

		if(shared_buffer_index_slave >= SHARED_BUFFER_COUNT)
		{
			success = false;
		}
	}

	if(success == true)
	{
		master_err_flags.bits.get_slave_buff_idx_err = 0;
	}
	else
	{
		master_err_flags.bits.get_slave_buff_idx_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief 		  Sets the master buffer index byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::setMasterSharedBufferIndex(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_index_master);
				uint8_t  set_buffer[BUFFER_SIZE] = {0};

	memcpy(set_buffer,&shared_buffer_index_master,BUFFER_SIZE);

	telemetry_core.sendPacketToSharedBuff(MASTER_BUFF_INDEX_ADDR,static_cast<uint16_t>(cmd_type::MASTER_BUFFER_INDEX),set_buffer,BUFFER_SIZE);
}



/**
  * @brief 		  Clears all shared buffer status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::clearSharedBuffStatusBytes(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_status);
				uint8_t  set_buffer[BUFFER_SIZE] = {0};
				uint8_t  i = 0;

  shared_buffer_status = 0;

  memcpy(set_buffer,&shared_buffer_status,BUFFER_SIZE);

	for(i = 0;i < SHARED_BUFFER_COUNT;i++)
	{
		telemetry_core.sendPacketToSharedBuff((SHARED_BUFF_STATUS_ADDR + (i * static_cast<uint32_t>(0x10))),static_cast<uint16_t>(cmd_type::SHARED_BUFFER_STATUS),set_buffer,BUFFER_SIZE);
	}
}



/**
  * @brief 		  Gets the shared buffer status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getSharedBufferStatus(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_status);
				bool     success = false;
				uint8_t  get_buffer[BUFFER_SIZE] = {0};
				uint32_t address = SHARED_BUFF_STATUS_ADDR + (shared_buffer_index_master * static_cast<uint32_t>(0x10));

	success = telemetry_core.receivePacketFromSharedBuff(address, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&shared_buffer_status,get_buffer,BUFFER_SIZE);

		master_err_flags.bits.get_shared_buff_stat_err = 0;
	}
	else
	{
		master_err_flags.bits.get_shared_buff_stat_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief 		  Sends the shared buffer status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::setSharedBufferStatus(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_status);
				uint8_t  set_buffer[BUFFER_SIZE] = {0};
				uint32_t address = SHARED_BUFF_STATUS_ADDR + (shared_buffer_index_master * static_cast<uint32_t>(0x10));

	memcpy(set_buffer,&shared_buffer_status,BUFFER_SIZE);
	telemetry_core.sendPacketToSharedBuff(address,static_cast<uint16_t>(cmd_type::SHARED_BUFFER_STATUS),set_buffer,BUFFER_SIZE);
}



/**
  * @brief 		  Sends the configuration parameters to the slave
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::setConfigParams(void)
{
	const uint16_t BUFFER_SIZE = sizeof(packet_count_write) + sizeof(packet_size) + sizeof(config_reserved);
				uint8_t  set_buffer[BUFFER_SIZE] = {0};

	memcpy(set_buffer, &packet_count_write, sizeof(packet_count_write));
	memcpy(&set_buffer[sizeof(packet_count_write)], &packet_size, sizeof(packet_size));
	memcpy(&set_buffer[sizeof(packet_count_write) + sizeof(packet_size)], &config_reserved, sizeof(config_reserved));

	telemetry_core.sendPacketToSharedBuff(CONFIG_PARAMS_ADDR,static_cast<uint16_t>(cmd_type::CONFIG_PARAMS),set_buffer,BUFFER_SIZE);
}



/**
  * @brief 		  Sets datalogger commands
  *
  * @param[in]  command_t serializer_cmd
  *
  * @return 	  bool	success
  */
bool SERIALIZER::setCommand(command_t serializer_cmd)
{
	bool success = false;

	if(!master_err_flags.bits.shared_buff_err)
	{
		if(((serializer_cmd == command_t::STAND_BY)||
				((master_cmd == command_t::STAND_BY)&&
				 (slave_status_flags.bits.datalogger_ready)))&&
				(!master_err_flags.bits.config_mismatch_err))
		{
			success = true;
			command = serializer_cmd;
		}
	}

	return success;
}



/**
  * @brief 		  Gets master error flags
  *
  * @param[in]  void Nothing
  *
  * @return 	  uint32_t master_err_flags.all
  */
uint8_t SERIALIZER::getMasterErrFlags(void)
{
	return master_err_flags.all;
}



/**
  * @brief 		  Gets slave status flags
  *
  * @param[in]  void Nothing
  *
  * @return 	  uint32_t slave_status_flags.all
  */
uint8_t SERIALIZER::getSlaveStatusFlags(void)
{
	return slave_status_flags.all;
}



/**
  * @brief 		  Gets datalogger error flags
  *
  * @param[in]  void Nothing
  *
  * @return 	  uint32_t datalogger_err_flags.all
  */
uint32_t SERIALIZER::getDataloggerErrFlags(void)
{
	return datalogger_err_flags.all;
}



/**
  * @brief 		  Gets overwrite error counter
  *
  * @param[in]  void Nothing
  *
  * @return 	  uint32_t datalogger_err_cntr
  */
uint32_t SERIALIZER::getOverwriteErrCntr(void)
{
	return overwrite_err_cntr;
}



/**
  * @brief 		  Gets datalogger error counter
  *
  * @param[in]  void Nothing
  *
  * @return 	  uint32_t datalogger_err_cntr
  */
uint32_t SERIALIZER::getDataloggerErrCntr(void)
{
	return datalogger_err_cntr;
}



/**
  * @brief 		  Gets shared buffer error counter
  *
  * @param[in]  void Nothing
  *
  * @return 	  uint32_t datalogger_err_cntr
  */
uint32_t SERIALIZER::getSharedBuffErrCntr(void)
{
	return shared_buff_err_cntr;
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
SERIALIZER::SERIALIZER(const SERIALIZER& orig)
{

}



/**
  * @brief      Default destructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
SERIALIZER::~SERIALIZER()
{

}



} //End of namespace Datalogger
