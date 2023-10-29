/**
 ******************************************************************************
  * @file		  : ds_fatfs_h747.cpp
  * @brief		: This file contains fatfs class
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
#include <iostream>
#include <string.h>
#include "ds_fatfs_h747.hpp"
#include "fatfs.h"
// End of Includes



/*
 * Begin of Object Definitions
 */
Middlewares::FATFS_H747 fatfs;
// End of Object Definitions



FATFS 	fs ; /* File System      */
FIL 	  fil; /* File             */
FILINFO fno; /* File Information */
DIR 	  dir; /* Directory        */



namespace Middlewares
{



fatfs_result_t setFlag(FRESULT value);



/**
  * @brief      Default constructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
FATFS_H747::FATFS_H747()
{
	fatfs_result = fatfs_result_t::FR_OK;
}



/**
  * @brief 		  Checks the connection status of the storage device
  *
  * @param[in]  void  Nothing
  *
  * @return 	  fatfs_result_t	fatfs_result
  */
fatfs_result_t FATFS_H747::deviceIsDetected(void)
{
	fatfs_result_t fatfs_result = fatfs_result_t::FR_SD_ERROR;

	if(BSP_SD_IsDetected() == true)
	{
		fatfs_result = fatfs_result_t::FR_OK;
	}
	else
	{
		fatfs_result = fatfs_result_t::FR_SD_ERROR;
	}

	return fatfs_result;
}



/**
  * @brief 		  Gives work area to the FatFs module
  *
  * @param[in]  void	Nothing
  *
  * @return 	  fatfs_result_t	fatfs_result
  */
fatfs_result_t FATFS_H747::connect(void)
{
	fatfs_result_t fatfs_result = setFlag(f_mount(&fs, "", 0));

	return fatfs_result;
}



/**
  * @brief 		  Unmount the default drive
  *
  * @param[in]  void	Nothing
  *
  * @return 	  fatfs_result_t	fatfs_result
  */
fatfs_result_t FATFS_H747::disconnect(void)
{
	fatfs_result_t fatfs_result = setFlag(f_mount(0, "", 0));

	return fatfs_result;
}



/**
  * @brief 		  Gets total sectors and free sectors
  *
  * @param[in]  uint32_t &sd_capacity
  *             uint32_t &sd_free_space
  *
  * @return 	  fatfs_result_t	fatfs_result
  */
fatfs_result_t FATFS_H747::checkSd(uint32_t &sd_capacity, uint32_t &sd_free_space)
{
	DWORD fre_clust = 0;
	FATFS *pfs = nullptr;
	fatfs_result_t fatfs_result = setFlag(f_getfree("", &fre_clust, &pfs));

	if(fatfs_result == fatfs_result_t::FR_OK)
	{
		sd_capacity = static_cast<uint32_t>((pfs->n_fatent - 2) * pfs->csize * 0.5);
		sd_free_space = static_cast<uint32_t>(fre_clust * pfs->csize * 0.5);
	}
	else
	{
		sd_capacity = 0;
		sd_free_space = 0;
	}

	return fatfs_result;
}



/**
  * @brief 		  Opens a file
  *
  * @param[in]  char *name
  *
  * @return 	  fatfs_result_t	fatfs_result
  */
fatfs_result_t FATFS_H747::createFile(char *name)
{
	fatfs_result_t fatfs_result = setFlag(f_stat (name, &fno));

	if (fatfs_result == fatfs_result_t::FR_NO_FILE)
	{
		fatfs_result = setFlag(f_open(&fil, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE));

		if (fatfs_result == fatfs_result_t::FR_OK)
		{
			fatfs_result = setFlag(f_close(&fil));
		}
	}

	return fatfs_result;
}



/**
  * @brief 		  Opens a file and writes data to it
  *
  * @param[in]  char *name
  *             uint8_t *buffer
  *             uint32_t buffer_size
  *             fatfs_cmd_t fatfs_cmd
  *
  * @return 	  fatfs_result_t fatfs_result
  */
fatfs_result_t FATFS_H747::updateFile(char *name, uint8_t *buffer, uint32_t buffer_size, fatfs_cmd_t fatfs_cmd)
{
	fatfs_result_t fatfs_result = fatfs_result_t::FR_DISK_ERR;
	UINT bw = 0; // file write count

	if(fatfs_cmd == fatfs_cmd_t::OPEN)
	{
		fatfs_result = setFlag(f_open(&fil, name, FA_OPEN_EXISTING | FA_WRITE));
	}
	else if(fatfs_cmd == fatfs_cmd_t::WRITE)
	{
		fatfs_result = setFlag(f_write(&fil, buffer,buffer_size, &bw));
		if(fatfs_result == fatfs_result_t::FR_OK)
		{
			fatfs_result = setFlag(f_sync(&fil));
		}
	}
	else
	{
		fatfs_result = setFlag(f_close(&fil));
	}

	return fatfs_result;
}



/**
  * @brief 		  Converts FRESULT to fatfs_result_t
  *
  * @param[in]  FRESULT value
  *
  * @return 	  fatfs_result_t result
  */
fatfs_result_t setFlag(FRESULT value)
{
	fatfs_result_t result = fatfs_result_t::FR_INVALID_CNV;

	if(value == FR_OK)
	{
		result = fatfs_result_t::FR_OK;
	}
	else if(value == FR_DISK_ERR)
	{
		result = fatfs_result_t::FR_DISK_ERR;
	}
	else if(value == FR_INT_ERR)
	{
		result = fatfs_result_t::FR_INT_ERR;
	}
	else if(value == FR_NOT_READY)
	{
		result = fatfs_result_t::FR_NOT_READY;
	}
	else if(value == FR_NO_FILE)
	{
		result = fatfs_result_t::FR_NO_FILE;
	}
	else if(value == FR_NO_PATH)
	{
		result = fatfs_result_t::FR_NO_PATH;
	}
	else if(value == FR_INVALID_NAME)
	{
		result = fatfs_result_t::FR_INVALID_NAME;
	}
	else if(value == FR_DENIED)
	{
		result = fatfs_result_t::FR_DENIED;
	}
	else if(value == FR_EXIST)
	{
		result = fatfs_result_t::FR_EXIST;
	}
	else if(value == FR_INVALID_OBJECT)
	{
		result = fatfs_result_t::FR_INVALID_OBJECT;
	}
	else if(value == FR_WRITE_PROTECTED)
	{
		result = fatfs_result_t::FR_WRITE_PROTECTED;
	}
	else if(value == FR_INVALID_DRIVE)
	{
		result = fatfs_result_t::FR_INVALID_DRIVE;
	}
	else if(value == FR_NOT_ENABLED)
	{
		result = fatfs_result_t::FR_NOT_ENABLED;
	}
	else if(value == FR_NO_FILESYSTEM)
	{
		result = fatfs_result_t::FR_NO_FILESYSTEM;
	}
	else if(value == FR_MKFS_ABORTED)
	{
		result = fatfs_result_t::FR_MKFS_ABORTED;
	}
	else if(value == FR_TIMEOUT)
	{
		result = fatfs_result_t::FR_TIMEOUT;
	}
	else if(value == FR_LOCKED)
	{
		result = fatfs_result_t::FR_LOCKED;
	}
	else if(value == FR_NOT_ENOUGH_CORE)
	{
		result = fatfs_result_t::FR_NOT_ENOUGH_CORE;
	}
	else if(value == FR_TOO_MANY_OPEN_FILES)
	{
		result = fatfs_result_t::FR_TOO_MANY_OPEN_FILES;
	}
	else if(value == FR_INVALID_PARAMETER)
	{
		result = fatfs_result_t::FR_INVALID_PARAMETER;
	}
	else
	{
		//invalid result
	}

	return result;
}



/**
  * @brief      Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
FATFS_H747::FATFS_H747(const FATFS_H747& orig)
{

}



/**
  * @brief      Default destructor
  *
  * @param[in]  void
  *
  * @return 	  void
  */
FATFS_H747::~FATFS_H747()
{

}



} //End of namespace Middlewares
