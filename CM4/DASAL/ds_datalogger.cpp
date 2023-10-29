/**
 ******************************************************************************
  * @file		  : ds_datalogger.cpp
  * @brief		: This file contains data logger class
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
#include <stdio.h>
#include <string.h>
#include "ds_datalogger.hpp"
#include "ds_fatfs_h747.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Datalogger::DATA_LOGGER datalogger;
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
DATA_LOGGER::DATA_LOGGER()
{
	error_flags.all = 0;
	status_flags.all = 0;
}



/**
  * @brief 		  Scheduler
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::scheduler(void)
{
	switch(scheduler_state)
	{
		case scheduler_state_t::INIT:
			initState();
			break;

		case scheduler_state_t::CHECK:
			checkState();
			break;

		case scheduler_state_t::CREATE:
			createState();
			break;

		case scheduler_state_t::STAND_BY:
			standByState();
			break;

		case scheduler_state_t::WRITE:
			writeState();
			break;

		case scheduler_state_t::ERROR:
			errorState();
			break;

		default:
			break;
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::initState(void)
{
	static uint8_t init_state_err_cntr = 0;

	status_flags.all = 0;

	if(initialize() == result_t::RES_OK)
	{
		error_flags.bits.init_state_err = 0;
		scheduler_state = scheduler_state_t::CHECK;
	}
	else
	{
		datalogger_err_cntr++;
		error_flags.bits.init_state_err = 1;
		if(init_state_err_cntr < MAX_ERROR_COUNT)
		{
			init_state_err_cntr++;
		}
		else
		{
			scheduler_state = scheduler_state_t::ERROR;
		}
	}

	if(scheduler_state != scheduler_state_t::INIT)
	{
		init_state_err_cntr = 0;
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::checkState(void)
{
	static uint8_t check_state_err_cntr = 0;

	if(status_flags.bits.config_params_received)
	{
		if(checkDevice() == result_t::RES_OK)
		{
			error_flags.bits.check_state_err = 0;
			if(storage_free_space_kb > CHECK_STATE_MIN_FREE_SPACE_KB)
			{
				error_flags.bits.check_state_free_space_err = 0;
				scheduler_state = scheduler_state_t::CREATE;
			}
			else
			{
				error_flags.bits.check_state_free_space_err = 1;
				scheduler_state = scheduler_state_t::ERROR;
			}
		}
		else
		{
			datalogger_err_cntr++;
			error_flags.bits.check_state_err = 1;
			if(check_state_err_cntr < MAX_ERROR_COUNT)
			{
				check_state_err_cntr++;
			}
			else
			{
				scheduler_state = scheduler_state_t::ERROR;
			}
		}
	}

	if(scheduler_state != scheduler_state_t::CHECK)
	{
		check_state_err_cntr = 0;
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::createState(void)
{
	static uint8_t create_file_err_cntr = 0;

	if(createFile() == result_t::RES_OK)
	{
		error_flags.bits.create_state_err = 0;
		scheduler_state = scheduler_state_t::STAND_BY;
	}
	else
	{
		datalogger_err_cntr++;
		error_flags.bits.create_state_err = 1;
		if(create_file_err_cntr < MAX_ERROR_COUNT)
		{
			create_file_err_cntr++;
		}
		else
		{
			scheduler_state = scheduler_state_t::ERROR;
		}
	}

	if(scheduler_state != scheduler_state_t::CREATE)
	{
		create_file_err_cntr = 0;
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::standByState(void)
{
	if(command == command_t::WRITE)
	{
		scheduler_state = scheduler_state_t::WRITE;
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::writeState(void)
{
	static uint8_t write_state_write_err_cntr = 0;
	static uint8_t write_state_open_file_err_cntr = 0;
	static uint8_t write_state_check_err_cntr = 0;
	static uint8_t write_state_close_file_err_cntr = 0;
	static bool first_write = true;
	static bool write_state_update = false;

	result_t result = result_t::RES_ERROR;

	if(command == command_t::WRITE)
	{
		if(status_flags.bits.write_storage_device)
		{
			if((checkDevice() == result_t::RES_OK))
			{
				write_state_check_err_cntr = 0;
				error_flags.bits.write_state_check_err = 0;
				if(storage_free_space_kb > WRITE_STATE_MIN_FREE_SPACE_KB)
				{
					error_flags.bits.write_state_free_space_err = 0;

					if(first_write)
					{
						result = updateFile(update_file_cmd_t::OPEN);

						if(result == result_t::RES_OK)
						{
							error_flags.bits.write_state_open_file_err = 0;
							first_write = false;
							write_state_update = true;
						}
						else
						{
							datalogger_err_cntr++;
							error_flags.bits.write_state_open_file_err = 1;
							if(write_state_open_file_err_cntr < MAX_ERROR_COUNT)
							{
								write_state_open_file_err_cntr++;
							}
							else
							{
								scheduler_state = scheduler_state_t::ERROR;
							}
						}
					}

					if(write_state_update)
					{
						result = updateFile(update_file_cmd_t::WRITE);

						if(result == result_t::RES_OK)
						{
							error_flags.bits.write_state_write_err = 0;
							status_flags.bits.write_storage_device = 0;
						}
						else
						{
							datalogger_err_cntr++;
							error_flags.bits.write_state_write_err = 1;
							if(write_state_write_err_cntr < MAX_ERROR_COUNT)
							{
								write_state_write_err_cntr++;
							}
							else
							{
								scheduler_state = scheduler_state_t::ERROR;
							}
						}
					}
				}
				else
				{
					error_flags.bits.write_state_free_space_err = 1;

					result = updateFile(update_file_cmd_t::CLOSE);

					if(result == result_t::RES_OK)
					{
						error_flags.bits.write_state_close_file_err = 0;
					}
					else
					{
						datalogger_err_cntr++;
						error_flags.bits.write_state_close_file_err = 1;
					}

					scheduler_state = scheduler_state_t::ERROR;
				}
			}
			else
			{
				datalogger_err_cntr++;
				error_flags.bits.write_state_check_err = 1;
				if(write_state_check_err_cntr < MAX_ERROR_COUNT)
				{
					write_state_check_err_cntr++;
				}
				else
				{
					scheduler_state = scheduler_state_t::ERROR;
				}
			}
		}
	}
	else
	{
		result = updateFile(update_file_cmd_t::CLOSE);

		if(result == result_t::RES_OK)
		{
			error_flags.bits.write_state_close_file_err = 0;
			scheduler_state = scheduler_state_t::CHECK;
		}
		else
		{
			datalogger_err_cntr++;
			error_flags.bits.write_state_close_file_err = 1;
			if(write_state_close_file_err_cntr < MAX_ERROR_COUNT)
			{
				write_state_close_file_err_cntr++;
			}
			else
			{
				scheduler_state = scheduler_state_t::ERROR;
			}
		}
	}

	if(scheduler_state != scheduler_state_t::WRITE)
	{
		write_state_check_err_cntr = 0;
		write_state_write_err_cntr = 0;
		write_state_open_file_err_cntr = 0;
		write_state_close_file_err_cntr = 0;
		first_write = true;
		write_state_update = false;
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::errorState(void)
{
	static uint8_t state_err_cntr = 0;

	if((state_err_cntr < MAX_ERROR_COUNT)&&
		 (!error_flags.bits.check_state_free_space_err)&&
		 (!error_flags.bits.write_state_free_space_err))
	{
		state_err_cntr++;
		scheduler_state = scheduler_state_t::INIT;
		error_flags.bits.datalogger_err = 0;
	}
	else
	{
		//Error!
		error_flags.bits.datalogger_err = 1;
	}
}



/**
  * @brief 		  Initialize
  *
  * @param[in]  void	Nothing
  *
  * @return 	  result_t	result
  */
result_t DATA_LOGGER::initialize(void)
{
	result_t result;
	Middlewares::fatfs_result_t fatfs_res = fatfs.deviceIsDetected();

	if(fatfs_res == Middlewares::fatfs_result_t::FR_OK)
	{
		fatfs_res = fatfs.connect();
		if(fatfs_res == Middlewares::fatfs_result_t::FR_OK)
		{
			result = result_t::RES_OK;
		}
		else
		{
			result = result_t::RES_ERROR;
		}
	}
	else
	{
		result = result_t::RES_ERROR;
	}

	return result;
}



/**
  * @brief 		  Unmount the default drive
  *
  * @param[in]  void	Nothing
  *
  * @return 	  result_t result
  */
result_t DATA_LOGGER::disconnect(void)
{
	result_t result;
	Middlewares::fatfs_result_t fatfs_res = fatfs.disconnect();

	if(fatfs_res == Middlewares::fatfs_result_t::FR_OK)
	{
		result = result_t::RES_OK;
	}
	else
	{
		result = result_t::RES_ERROR;
	}

	return result;
}



/**
  * @brief 		  Gets storage capacity and free space
  *
  * @param[in]  void	Nothing
  *
  * @return 	  result_t	result
  */
result_t DATA_LOGGER::checkDevice(void)
{
	result_t result;
	Middlewares::fatfs_result_t fatfs_res = fatfs.checkSd(storage_capacity_kb,storage_free_space_kb);

	if(fatfs_res == Middlewares::fatfs_result_t::FR_OK)
	{
		result = result_t::RES_OK;
	}
	else
	{
		result = result_t::RES_ERROR;
	}

	return result;
}



/**
  * @brief 		  Opens a file and gives name to it
  *
  * @param[in]  void	Nothing
  *
  * @return 	  result_t	result
  */
result_t DATA_LOGGER::createFile(void)
{
	result_t result = result_t::RES_ERROR;
	Middlewares::fatfs_result_t fatfs_res;

	result = setFileName();

	if(result == result_t::RES_OK)
	{
		error_flags.bits.format_name_err = 0;
	}
	else
	{
		error_flags.bits.format_name_err = 1;
	}

	fatfs_res = fatfs.createFile(w_file_name);

	if(fatfs_res == Middlewares::fatfs_result_t::FR_OK)
	{
		result = result_t::RES_OK;
	}
	else
	{
		result = result_t::RES_ERROR;
	}

	return result;
}



/**
  * @brief 		  Opens a file and writes data to it
  *
  * @param[in]  update_file_cmd_t  update_file_cmd
  *
  * @return 	  result_t	result
  */
result_t DATA_LOGGER::updateFile(update_file_cmd_t  update_file_cmd)
{
	result_t result = result_t::RES_ERROR;
	Middlewares::fatfs_cmd_t fasfs_update_cmd = Middlewares::fatfs_cmd_t::OPEN;

	if(update_file_cmd == update_file_cmd_t::OPEN)
	{
		fasfs_update_cmd = Middlewares::fatfs_cmd_t::OPEN;
	}
	else if(update_file_cmd == update_file_cmd_t::WRITE)
	{
		fasfs_update_cmd = Middlewares::fatfs_cmd_t::WRITE;
	}
	else
	{
		fasfs_update_cmd = Middlewares::fatfs_cmd_t::CLOSE;
	}

	Middlewares::fatfs_result_t fatfs_res = fatfs.updateFile(w_file_name, receive_buffer, receive_buffer_size, fasfs_update_cmd);

	if(fatfs_res == Middlewares::fatfs_result_t::FR_OK)
	{
		result = result_t::RES_OK;
	}
	else
	{
		result = result_t::RES_ERROR;
	}

	return result;
}



/**
  * @brief 		  TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	  result_t	result
  */
result_t DATA_LOGGER::setFileName(void)
{
	result_t result = result_t::RES_ERROR;

	const uint8_t  MAX_DAY   = 31;
	const uint8_t  MAX_MONTH = 12;
	const uint16_t MAX_YEAR  = 9999;
	const uint8_t  MAX_HOUR  = 23;
	const uint8_t  MAX_MIN   = 59;
	const uint8_t  MAX_SEC   = 59;

	char day[3]   = "";
	char month[3] = "";
	char year[3]  = "";
	char hour[3]  = "";
	char min[3]   = "";
	char sec[3]   = "";

	if((gnss_date_and_time.data.day    > MAX_DAY  )||
     (gnss_date_and_time.data.month  > MAX_MONTH)||
	   (gnss_date_and_time.data.year   > MAX_YEAR )||
	   (gnss_date_and_time.data.hour   > MAX_HOUR )||
	   (gnss_date_and_time.data.minute > MAX_MIN  )||
	   (gnss_date_and_time.data.second > MAX_SEC  ))
	{
		result = result_t::RES_ERROR;
		sprintf(w_file_name,"DASAL_%s%s%s_%s%s%s.DAT","01","01","20","00","00","00");
	}
	else
	{
		formatDate((static_cast<uint16_t>(gnss_date_and_time.data.day   )),day  );
		formatDate((static_cast<uint16_t>(gnss_date_and_time.data.month )),month);
		formatDate((static_cast<uint16_t>(gnss_date_and_time.data.year  )),year );
		formatDate((static_cast<uint16_t>(gnss_date_and_time.data.hour  )),hour );
		formatDate((static_cast<uint16_t>(gnss_date_and_time.data.minute)),min  );
		formatDate((static_cast<uint16_t>(gnss_date_and_time.data.second)),sec  );

		sprintf(w_file_name,"DASAL_%s%s%s_%s%s%s.DAT",day,month,year,hour,min,sec);

		result = result_t::RES_OK;
	}

	return result;
}



/**
  * @brief 		  TBD
  *
  * @param[in]  uint16_t input
  *             char *output
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::formatDate(uint16_t input, char *output)
{
	const uint8_t max_input_value = 10;
	uint8_t format_input =  input % static_cast<uint16_t>(100);

	if(format_input < max_input_value)
	{
		sprintf(output,"0%u",format_input);
	}
	else
	{
		sprintf(output,"%u",format_input);
	}
}



/**
  * @brief 		  TBD
  *
  * @param[in]  uint8_t buffer[]
  *             uint16_t buffer_size
  *
  * @return 	  void	Nothing
  */
void DATA_LOGGER::setPacket(uint8_t buffer[], uint16_t buffer_size)
{
	uint16_t k = 0;
	uint16_t max_buffer_size = sizeof(receive_buffer);

	if(buffer_size <= max_buffer_size)
	{
		error_flags.bits.set_packet_size_err = 0;

		for(k = 0; k < buffer_size; k++)
		{
			receive_buffer[k] = buffer[k];
		}
	}
	else
	{
		error_flags.bits.set_packet_size_err = 1;
	}
}



/**
  * @brief 		  Sets the size of the packet to be written in the storage device
  *
  * @param[in]  uint16_t size
  *
  * @return 	  void	Nothing
  */
bool DATA_LOGGER::setPacketSizeWrite(uint16_t size)
{
	bool success = false;

	if(size <= MAX_SIZE_WRITE)
	{
		error_flags.bits.write_packet_size_err = 0;
		success = true;
		receive_buffer_size = size;
	}
	else
	{
		error_flags.bits.write_packet_size_err = 1;
	}

	return success;
}



/**
  * @brief 		  Gets scheduler state
  *
  * @param[in]  void Nothing
  *
  * @return 	  scheduler_state_t scheduler_state
  */
scheduler_state_t DATA_LOGGER::getState(void)
{
	return scheduler_state;
}



/**
  * @brief      Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
DATA_LOGGER::DATA_LOGGER(const DATA_LOGGER& orig)
{

}



/**
  * @brief      Default destructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
DATA_LOGGER::~DATA_LOGGER()
{

}



} //End of namespace Datalogger
