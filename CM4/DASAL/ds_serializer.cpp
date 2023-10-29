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
#include "ds_datalogger.hpp"
#include "ds_telemetry_core.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Datalogger::SERIALIZER serializer;
// End of Object Definitions



namespace Datalogger
{



/**
  * @brief      Default constructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
SERIALIZER::SERIALIZER()
{
	slave_status_flags.all = 0;
	slave_err_flags.all = 0;
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
	setSlaveSharedBufferIndex();
}



/**
  * @brief 		  Scheduler (Maximum calling frequency:20Hz)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::scheduler(void)
{
	static serializer_state_t serializer_state = serializer_state_t::STAND_BY;

	if(!slave_err_flags.bits.shared_buff_err)
	{
		if(!datalogger.status_flags.bits.config_params_received)
		{
			if((getConfigParams()) && (getDateAndTime()))
			{
				datalogger.status_flags.bits.config_params_received = 1;
			}
		}
		else
		{
			getDateAndTime();
		}

		if(getMasterCommand())
		{
			switch(serializer_state)
			{
				case serializer_state_t::STAND_BY:
					if(datalogger.command == Datalogger::command_t::WRITE)
					{
						datalogger.command = Datalogger::command_t::STAND_BY;
					}

					if(datalogger.getState() == scheduler_state_t::STAND_BY)
					{
						if(master_cmd == MASTER_CMD_WRITE)
						{
							datalogger.command = Datalogger::command_t::WRITE;
							serializer_state = serializer_state_t::WRITE;
							slave_status_flags.bits.datalogger_ready = 0;
						}
						else
						{
							slave_status_flags.bits.datalogger_ready = 1;
						}
					}
					break;


				case serializer_state_t::WRITE:
					if((datalogger.getState() == scheduler_state_t::WRITE) && (master_cmd == MASTER_CMD_WRITE))
					{
						if(!datalogger.status_flags.bits.write_storage_device)
						{
							if(getMasterSharedBufferIndex())
							{
								if(shared_buffer_index_slave == shared_buffer_index_master)
								{
									slave_status_flags.bits.all_buffer_empty = 1;
								}
								else
								{
									slave_status_flags.bits.all_buffer_empty = 0;

									setSlaveSharedBufferIndex();

									if(getSharedBufferStatus())
									{
										if(shared_buffer_status == FULL)
										{
											if(readFromSharedBuffer())
											{
												slave_err_flags.bits.receive_packet_err = 0;
												shared_buffer_status = 0;
												setSharedBufferStatus();
												datalogger.status_flags.bits.write_storage_device = 1;
												datalogger.setPacket(datalogger_buffer, (packet_size * packet_count_write));
											}
											else
											{
												slave_err_flags.bits.receive_packet_err = 1;
											}
										}

										increaseSlaveSharedBufferIndex();
									}
								}
							}
						}
					}
					else
					{
						serializer_state = serializer_state_t::STAND_BY;
					}

					if(serializer_state != serializer_state_t::WRITE)
					{
						shared_buffer_index_slave = 0;
						shared_buffer_status = 0;
					}
					break;


				default:
					break;
			}
		}
	}

	if((shared_buff_err_cntr > MAX_SHARED_BUFF_ERR_CNT)&&
		 (!slave_err_flags.bits.shared_buff_err))
	{
		slave_err_flags.bits.shared_buff_err = 1;
		serializer_state = serializer_state_t::STAND_BY;
		datalogger.command = Datalogger::command_t::STAND_BY;
	}

	setSlaveStatus();
	datalogger.scheduler();
}



/**
  * @brief 		  Receives the date and time information from the master
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getDateAndTime(void)
{
	const uint16_t BUFFER_SIZE = sizeof(datalogger.gnss_date_and_time.buffer);
		    uint8_t  get_buffer[BUFFER_SIZE] = {0};
		    bool     success = false;

	success = telemetry_core.receivePacketFromSharedBuff(DATE_AND_TIME_ADDR, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(datalogger.gnss_date_and_time.buffer,get_buffer,BUFFER_SIZE);
		slave_err_flags.bits.get_date_time_err = 0;
	}
	else
	{
		slave_err_flags.bits.get_date_time_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief 		  Receives packet from the shared buffer
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::readFromSharedBuffer(void)
{
	uint32_t destination_addr = SHARED_BUFFER_BASE_ADDR + (shared_buffer_index_slave * SHARED_BUFFER_SIZE);
	bool success = false;
	uint16_t z = 0;

	for(z = 0; z<packet_count_write; z++)
	{
		success = telemetry_core.receivePacketFromSharedBuff((destination_addr + (z * (packet_size + static_cast<uint32_t>(9)))),
				                                                 &datalogger_buffer[z * packet_size],
															                           packet_size);

		if(success == false)
		{
			shared_buff_err_cntr++;
			return success;
		}
	}

	return success;
}



/**
  * @brief 		  Increases the slave shared buffer index
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::increaseSlaveSharedBufferIndex(void)
{
	if(shared_buffer_index_slave >= (SHARED_BUFFER_COUNT - 1))
	{
		shared_buffer_index_slave = 0;
	}
	else
	{
		shared_buffer_index_slave++;
	}
}



/**
  * @brief 		  Sets the slave status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::setSlaveStatus(void)
{
	const uint16_t BUFFER_SIZE = sizeof(slave_status_flags.all) +
								 sizeof(datalogger.error_flags.all) +
								 sizeof(datalogger.datalogger_err_cntr);

		    uint8_t  set_buffer[BUFFER_SIZE] = {0};

	memcpy(set_buffer,&slave_status_flags.all,sizeof(slave_status_flags.all) );
	memcpy(&set_buffer[sizeof(slave_status_flags.all)],&datalogger.error_flags.all,sizeof(datalogger.error_flags.all));
	memcpy(&set_buffer[sizeof(slave_status_flags.all) + sizeof(datalogger.error_flags.all)],&datalogger.datalogger_err_cntr,sizeof(datalogger.datalogger_err_cntr));

	telemetry_core.sendPacketToSharedBuff(SLAVE_STATUS_ADDR,
								                        static_cast<uint16_t>(cmd_type::SLAVE_STATUS),
								                        set_buffer,
										                    BUFFER_SIZE);
}



/**
  * @brief 		  Receives the master command byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getMasterCommand(void)
{
	const uint16_t BUFFER_SIZE = sizeof(master_cmd);
		    uint8_t  get_buffer[BUFFER_SIZE] = {0};
		    bool     success = false;

	success = telemetry_core.receivePacketFromSharedBuff(MASTER_CMD_ADDR, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&master_cmd,get_buffer,BUFFER_SIZE);
		slave_err_flags.bits.get_master_cmd_err = 0;
	}
	else
	{
		slave_err_flags.bits.get_master_cmd_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief 		  Sets the slave shared buffer index byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::setSlaveSharedBufferIndex(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_index_slave);
		    uint8_t  set_buffer[BUFFER_SIZE] = {0};

  memcpy(set_buffer, &shared_buffer_index_slave, BUFFER_SIZE);

	telemetry_core.sendPacketToSharedBuff(SLAVE_BUFF_INDEX_ADDR,
								                        static_cast<uint16_t>(cmd_type::SLAVE_BUFFER_INDEX),
								                        set_buffer,
										                    BUFFER_SIZE);
}



/**
  * @brief 		  Gets the master shared buffer index byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getMasterSharedBufferIndex(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_index_master);
		    uint8_t  get_buffer[BUFFER_SIZE] = {0};
		    bool     success = false;

  success = telemetry_core.receivePacketFromSharedBuff(MASTER_BUFF_INDEX_ADDR, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&shared_buffer_index_master, get_buffer, BUFFER_SIZE);

		if(shared_buffer_index_master >= SHARED_BUFFER_COUNT)
		{
			success = false;
		}
	}

	if(success == true)
	{
		slave_err_flags.bits.get_master_buff_idx_err = 0;
	}
	else
	{
		slave_err_flags.bits.get_master_buff_idx_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief 		  Clears all shared buffer status byte(s)
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void SERIALIZER::clearSharedBufferStatus(void)
{
	const uint16_t BUFFER_SIZE = sizeof(shared_buffer_status);
		    uint8_t  set_buffer[BUFFER_SIZE] = {0};
		    uint8_t  i = 0;

	shared_buffer_status = 0;

	memcpy(set_buffer,&shared_buffer_status,BUFFER_SIZE);

	for(i = 0;i < SHARED_BUFFER_COUNT;i++)
	{
		telemetry_core.sendPacketToSharedBuff((SHARED_BUFF_STATUS_ADDR + (i * static_cast<uint32_t>(0x10))),
				                                  static_cast<uint16_t>(cmd_type::SHARED_BUFFER_STATUS),
											                    set_buffer,BUFFER_SIZE);
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
		    uint32_t address = SHARED_BUFF_STATUS_ADDR + (shared_buffer_index_slave * static_cast<uint32_t>(0x10));

  success = telemetry_core.receivePacketFromSharedBuff(address, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&shared_buffer_status,get_buffer,BUFFER_SIZE);
		slave_err_flags.bits.get_shared_buff_stat_err = 0;
	}
	else
	{
		slave_err_flags.bits.get_shared_buff_stat_err = 1;
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
		    uint32_t address = SHARED_BUFF_STATUS_ADDR + (shared_buffer_index_slave * static_cast<uint32_t>(0x10));

	memcpy(set_buffer, &shared_buffer_status, BUFFER_SIZE);
	telemetry_core.sendPacketToSharedBuff(address,static_cast<uint16_t>(cmd_type::SHARED_BUFFER_STATUS),set_buffer,BUFFER_SIZE);
}



/**
  * @brief 		  Receives the configuration parameters to the master
  *
  * @param[in]  void	Nothing
  *
  * @return 	  bool	success
  */
bool SERIALIZER::getConfigParams(void)
{
	const uint16_t BUFFER_SIZE = sizeof(packet_count_write) + sizeof(packet_size) + sizeof(config_reserved);
		    uint8_t  get_buffer[BUFFER_SIZE] = {0};
		    bool     success = false;

    success = telemetry_core.receivePacketFromSharedBuff(CONFIG_PARAMS_ADDR, get_buffer, BUFFER_SIZE);

	if(success == true)
	{
		memcpy(&packet_count_write, get_buffer, sizeof(packet_count_write));
		memcpy(&packet_size, &get_buffer[sizeof(packet_count_write)], sizeof(packet_size));
		memcpy(&config_reserved, &get_buffer[sizeof(packet_count_write) + sizeof(packet_size)], sizeof(config_reserved));
	}

	if((success == true)&&
	   (datalogger.setPacketSizeWrite(packet_size * packet_count_write)))
	{
		slave_err_flags.bits.get_config_err = 0;
	}
	else
	{
		slave_err_flags.bits.get_config_err = 1;
		shared_buff_err_cntr++;
	}

	return success;
}



/**
  * @brief      Default copy constructor
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
