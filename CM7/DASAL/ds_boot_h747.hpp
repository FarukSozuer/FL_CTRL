/**
 ******************************************************************************
  * @file	  	: ds_boot_h747.hpp
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

#ifndef DS_BOOT_H747_HPP
#define DS_BOOT_H747_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



namespace Bootloader
{



/*
 * Begin of Macro Definitions
 */
const uint32_t SYSTEM_BOOTLOADER_ADDR = 0x1FF00000;
 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */
union status_flags_t
{
	struct
	{
		uint8_t flash_unlock_err     :1;
		uint8_t flash_ob_unlock_err  :1;
		uint8_t ob_program_err 		   :1;
		uint8_t ob_launch_err        :1;
		uint8_t invalid_boot_addr    :1;
		uint8_t flash_lock_err       :1;
		uint8_t flash_ob_lock_err    :1;
		uint8_t reserved1            :1;

	}bits;
	uint8_t all;
};
// End of Enum, Union and Struct Definitions



/*
 * Begin of BOOT_H747 Class Definition
 */
class BOOT_H747
{
    public:
			BOOT_H747();

			status_flags_t status_flags;

			bool activateBootloader ( void );

			BOOT_H747(const BOOT_H747& orig);
			virtual ~BOOT_H747();

    private:

			bool unlock						  ( void );
			bool programOptionBytes ( void );
			bool checkOptionBytes 	( void );
			bool lock								( void );
			void systemReset      	( void );

};
// End of BOOT_H747 Class Definition



} //End of namespace Bootloader



/*
 * External Linkages
 */
extern Bootloader::BOOT_H747 boot;
// End of External Linkages



#endif /* DS_BOOT_H747_HPP */
