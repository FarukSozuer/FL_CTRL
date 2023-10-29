/**
 ******************************************************************************
  * @file		  : ds_boot_h747.cpp
  * @brief		: This file contains boot class
  * @author		: Baris Aycin
  * @date		  : 05.06.2020
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
#include "stm32h7xx_hal.h"
#include "ds_boot_h747.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Bootloader::BOOT_H747 boot;
// End of Object Definitions



namespace Bootloader
{



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
BOOT_H747::BOOT_H747()
{

}



/**
  * @brief  STM32 System Bootloader is activated by this function
  *
  * @param[in]  void	Nothing
  *
  * @return 	bool	success
  */
bool BOOT_H747::activateBootloader( void )
{
	bool success = false;

	if(unlock())
	{
		if(programOptionBytes())
		{
			if(lock())
			{
				success = true;
				systemReset();
			}
		}
	}

	return success;
}



/**
  * @brief  Allow access to flash control registers and user flash
  *
  * @param[in]  void	Nothing
  *
  * @return 	bool	success
  */
bool BOOT_H747::unlock( void )
{
	bool success = false;
	HAL_StatusTypeDef result = HAL_ERROR;

	result = HAL_FLASH_Unlock();

	if(result == HAL_OK)
	{
		status_flags.bits.flash_unlock_err = 0;

		/* Allow Access to option bytes sector */
		result = HAL_FLASH_OB_Unlock();

		if(result == HAL_OK)
		{
			status_flags.bits.flash_ob_unlock_err = 0;
		}
		else
		{
			status_flags.bits.flash_ob_unlock_err = 1;
		}
	}
	else
	{
		status_flags.bits.flash_unlock_err = 1;
	}

	if(result == HAL_OK)
	{
		success = true;
	}
	else
	{
		success = false;
	}

	return success;
}



/**
  * @brief  Program option bytes
  *
  * @param[in]  void	Nothing
  *
  * @return 	bool	success
  */
bool BOOT_H747::programOptionBytes( void )
{
	bool success = false;
	HAL_StatusTypeDef result = HAL_ERROR;
	FLASH_OBProgramInitTypeDef  option_bytes = {0};

	/* Get the Dual boot configuration status */
	HAL_FLASHEx_OBGetConfig(&option_bytes);

	option_bytes.BootConfig = OB_BOOT_ADD0;
	option_bytes.BootAddr0  = SYSTEM_BOOTLOADER_ADDR;

	result = HAL_FLASHEx_OBProgram(&option_bytes);

	if(result == HAL_OK)
	{
		status_flags.bits.ob_program_err = 0;

		/* Start the Option Bytes programming process */
		result = HAL_FLASH_OB_Launch();

		if(result == HAL_OK)
		{
			status_flags.bits.ob_launch_err = 0;

			if(checkOptionBytes())
			{
				status_flags.bits.invalid_boot_addr = 0;
			}
			else
			{
				result = HAL_ERROR;

				status_flags.bits.invalid_boot_addr = 1;
			}
		}
		else
		{
			status_flags.bits.ob_launch_err = 1;
		}
	}
	else
	{
		status_flags.bits.ob_program_err = 1;
	}

	if(result == HAL_OK)
	{
		success = true;
	}
	else
	{
		success = false;
	}

	return success;
}



/**
  * @brief  Check option bytes
  *
  * @param[in]  void	Nothing
  *
  * @return 	bool	success
  */
bool BOOT_H747::checkOptionBytes( void )
{
	bool success = false;
	FLASH_OBProgramInitTypeDef  option_bytes = {0};

	HAL_FLASHEx_OBGetConfig(&option_bytes);

	if(option_bytes.BootAddr0 == SYSTEM_BOOTLOADER_ADDR)
	{
		success = true;
	}
	else
	{
		success = false;
	}

	return success;
}



/**
  * @brief  Prevent access to flash control registers
  *
  * @param[in]  void	Nothing
  *
  * @return 	void	Nothing
  */
bool BOOT_H747::lock( void )
{
	bool success = false;
	HAL_StatusTypeDef result = HAL_ERROR;

  /* Prevent Access to option bytes sector */
	result = HAL_FLASH_OB_Lock();

	if(result == HAL_OK)
	{
		status_flags.bits.flash_ob_lock_err = 0;

		/* Disable the Flash option control register access (recommended to protect
		the option Bytes against possible unwanted operations) */
		result = HAL_FLASH_Lock();

		if(result == HAL_OK)
		{
			status_flags.bits.flash_lock_err = 0;
		}
		else
		{
			status_flags.bits.flash_lock_err = 1;
		}
	}
	else
	{
		status_flags.bits.flash_ob_lock_err = 1;
	}

	if(result == HAL_OK)
	{
		success = true;
	}
	else
	{
		success = false;
	}

	return success;
}



/**
  * @brief  Initiate a system reset request to reset the MCU
  *
  * @param[in]  void	Nothing
  *
  * @return 	void	Nothing
  */
void BOOT_H747::systemReset( void )
{
	HAL_NVIC_SystemReset();
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
BOOT_H747::BOOT_H747(const BOOT_H747& orig)
{

}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
BOOT_H747::~BOOT_H747()
{

}



} //End of namespace Bootloader
