/**
 ******************************************************************************
  * @file		  : ds_fatfs_h747.hpp
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

#ifndef DS_FATFS_H747_HPP
#define DS_FATFS_H747_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



namespace Middlewares
{



/*
 * Begin of Macro Definitions
 */

 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class fatfs_result_t : uint8_t
{
	FR_OK = 0,				      /* (0) Succeeded */
	FR_DISK_ERR,			      /* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				      /* (2) Assertion failed */
	FR_NOT_READY,			      /* (3) The physical drive cannot work */
	FR_NO_FILE,				      /* (4) Could not find the file */
	FR_NO_PATH,				      /* (5) Could not find the path */
	FR_INVALID_NAME,		    /* (6) The path name format is invalid */
	FR_DENIED,				      /* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				        /* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,	  	/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,	  	/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		    /* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			    /* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		    /* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		    /* (14) The f_mkfs() aborted due to any problem */
	FR_TIMEOUT,				      /* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				      /* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		  /* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_LOCK */
	FR_INVALID_PARAMETER,	  /* (19) Given parameter is invalid */
	FR_FNO_ERROR,    		    /* (20) TBD */
	FR_END_OF_FILE,    		  /* (21) TBD */
	FR_DIRECTORY,    		    /* (22) TBD */
	FR_SD_ERROR,    		    /* (23) TBD */
	FR_INVALID_CNV,         /* (24) TBD */
};



enum class fatfs_cmd_t : uint8_t
{
	WRITE = 0x00,
	OPEN  = 0x01,
	CLOSE = 0x02,
};
// End of Enum, Union and Struct Definitions



/*
 * Begin of FATFS_H747 Class Definition
 */
class FATFS_H747
{
	public:
		FATFS_H747();

		fatfs_result_t fatfs_result;

		fatfs_result_t deviceIsDetected ( void );
		fatfs_result_t connect			    ( void );
		fatfs_result_t disconnect       ( void );
		fatfs_result_t checkSd 			    ( uint32_t 	&sd_capacity   ,
											                uint32_t 	&sd_free_space );
		fatfs_result_t createFile	      ( char *name );
		fatfs_result_t updateFile		    ( char  		  *name       ,
											                uint8_t 	  *buffer     ,
											                uint32_t 	  buffer_size ,
											                fatfs_cmd_t fatfs_cm    );

		FATFS_H747(const FATFS_H747& orig);
		virtual ~FATFS_H747();

	protected:

	private:

};
// End of FATFS_H747 Class Definition



} //End of namespace Middlewares



/*
 * External Linkages
 */
extern Middlewares::FATFS_H747 fatfs;
// End of External Linkages



#endif /* DS_FATFS_H747_HPP */
