/**
 ******************************************************************************
  * @file			: ds_shared_ram_h747.cpp
  * @brief		: Shared Memory Transport Layer
  *				  		This file contains shared memory structure
  * @author		: Faruk Sozuer
  * 			  		Onur Yildirim
  * @date			: 16.03.2020
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
#include "ds_shared_ram_h747.hpp"
#include "ds_debug_tools.hpp"
// End of Includes



#ifdef __cplusplus
extern "C"
{
#endif

const uint16_t SHARED_RAM_BUFFER_SIZE = 0x7F00; //32512
//const uint16_t VARIABLE_MEMORY_SIZE	  = 0x0100; /* Used by Datalogger */


#ifdef CORE_CM7
	volatile uint8_t  * const transmit_buffer_ptr 		= reinterpret_cast<volatile uint8_t  *>(0x38000000);	//SRAM4 64K
	volatile uint16_t * const transmit_head_ptr				= reinterpret_cast<volatile uint16_t *>(0x38007F10);
	volatile uint16_t * const receive_tail_ptr				= reinterpret_cast<volatile uint16_t *>(0x38007F20);
	volatile uint32_t * const core_clock_ms_write_ptr	= reinterpret_cast<volatile uint32_t *>(0x38007F30);

	volatile const uint8_t  * const receive_buffer_ptr 			= reinterpret_cast<volatile uint8_t  *>(0x38008000);
	volatile const uint16_t * const receive_head_ptr				= reinterpret_cast<volatile uint16_t *>(0x3800FF10);
	volatile const uint32_t * const core_clock_ms_check_ptr	= reinterpret_cast<volatile uint32_t *>(0x3800FF30);

#endif

#ifdef CORE_CM4
	volatile uint8_t  * const transmit_buffer_ptr 		= reinterpret_cast<volatile uint8_t  *>(0x38008000);
	volatile uint16_t * const transmit_head_ptr				= reinterpret_cast<volatile uint16_t *>(0x3800FF10);
	volatile uint16_t * const receive_tail_ptr				= reinterpret_cast<volatile uint16_t *>(0x3800FF20);
	volatile uint32_t * const core_clock_ms_write_ptr	= reinterpret_cast<volatile uint32_t *>(0x3800FF30);

	volatile const uint8_t  * const receive_buffer_ptr 			= reinterpret_cast<volatile uint8_t  *>(0x38000000);
	volatile const uint16_t * const receive_head_ptr				= reinterpret_cast<volatile uint16_t *>(0x38007F10);
	volatile const uint32_t * const core_clock_ms_check_ptr	= reinterpret_cast<volatile uint32_t *>(0x38007F30);

#endif


#ifdef __cplusplus
}
#endif




/*
 * Begin of Object Definitions
 */
Peripherals::Ram::H747_SHARED_RAM inter_core;
// End of Object Definitions



namespace Peripherals
{

namespace Ram
{



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_SHARED_RAM::H747_SHARED_RAM()
{
	synchronization_complete = false;
}



/**
  * @brief 			Initializes the ram sector and the variables
  *
  * @param[in]  void	Nothing
  *
  * @return 		void	Nothing
  */
void H747_SHARED_RAM::initialize( void )
{
	uint16_t k = 0;

	for(k=0;k<(SHARED_RAM_BUFFER_SIZE);k++)
	{
		*(transmit_buffer_ptr + k)= 0x00;
	}

	*transmit_head_ptr = 0;
	*receive_tail_ptr  = 0;
}



/**
  * @brief 			Core Activity Check Task
  *
  * @param[in]  void	Nothing
  *
  * @return 		void	Nothing
  */
void H747_SHARED_RAM::scheduler( void )
{
	*core_clock_ms_write_ptr = monitor.getMillis();
}



/**
  * @brief 			Checks the other core activity
  *
  * @param[in]  void	Nothing
  *
  * @return 		bool Result
  */
bool H747_SHARED_RAM::isOtherCoreActive( void )
{
	const uint32_t DT_WRAP_LIMIT_MS = 1000000;
	const uint32_t DT_LIMIT_MS 		= 200;

	uint32_t dt_ms 	= 0;

	if(*core_clock_ms_write_ptr >= *core_clock_ms_check_ptr)
	{
		dt_ms = *core_clock_ms_write_ptr - *core_clock_ms_check_ptr;
	}
	else
	{
		dt_ms = *core_clock_ms_check_ptr - *core_clock_ms_write_ptr;
	}

	if(dt_ms < DT_WRAP_LIMIT_MS)
	{
		if(dt_ms <= DT_LIMIT_MS)
		{
			if(synchronization_complete == false)
			{
				*receive_tail_ptr = *receive_head_ptr;
				synchronization_complete = true;
			}
			else
			{
				//synchronization completed
			}
		}
		else
		{
			if(synchronization_complete == true)
			{
				synchronization_complete = false;
			}
			else
			{
				//other core is not active
			}
		}
	}
	else
	{
		//wrap around, return last state
	}

	return synchronization_complete;
}



/**
  * @brief 			Send data to other core
  *
  * @param[in]  uint8_t  buffer[]
  * @param[in]  uint16_t size
  *
  * @return 		uint16_t size
  */
uint16_t H747_SHARED_RAM::sendData(uint8_t buffer[], uint16_t size)
{
	uint16_t k = 0;
	uint16_t transmit_head = *transmit_head_ptr;

	if(size > SHARED_RAM_BUFFER_SIZE/2)
	{
		size = SHARED_RAM_BUFFER_SIZE/2;
	}

	if(isOtherCoreActive() == true)
	{
		for(k=0;k<size;k++)
		{
			*(transmit_buffer_ptr + transmit_head++) = buffer[k];
			if(transmit_head >= SHARED_RAM_BUFFER_SIZE)
			{
				transmit_head = 0;
			}
		}
	}

	*transmit_head_ptr = transmit_head;
	return size;
}



/**
  * @brief 			Get data from the buffer
  *
  * @param[in]  uint8_t  buffer[]
  * @param[in]  uint16_t size_limit
  *
  * @return 		uint16_t size
  */
uint16_t H747_SHARED_RAM::getDataFromBuffer(uint8_t buffer[], uint16_t size_limit)
{
	uint16_t size = 0;
	uint16_t receive_tail = *receive_tail_ptr;

	if(isOtherCoreActive() == true)
	{
		while( receive_tail != *receive_head_ptr )
		{
			buffer[size++] = *(receive_buffer_ptr + receive_tail++);
			if(receive_tail >= SHARED_RAM_BUFFER_SIZE)
			{
				receive_tail = 0;
			}
			if(size >= size_limit)
			{
				break;
			}
		}
	}
	*receive_tail_ptr = receive_tail;
	return size;
}



/**
  * @brief 			Send data to shared ram
  *
  * @param[in]  uint32_t ram_address
  * @param[in]  uint8_t  buffer[]
  * @param[in]  uint16_t size
  *
  * @return 	  void	Nothing
  */
void H747_SHARED_RAM::sendDataToSharedRam(uint32_t ram_address, uint8_t buffer[], uint16_t size)
{
	volatile uint8_t * const transmit_buff_ptr = reinterpret_cast<volatile uint8_t *>(ram_address);
	uint16_t k = 0;

	for(k = 0; k<size; k++)
	{
		*(transmit_buff_ptr + k) = buffer[k];
	}
}



/**
  * @brief 			Get data from the shared ram
  *
  * @param[in]  uint32_t ram_address
  * @param[in]  uint8_t  buffer[]
  * @param[in]  uint16_t size_limit
  *
  * @return 		void	Nothing
  */
void H747_SHARED_RAM::getDataFromSharedRam(uint32_t ram_address, uint8_t buffer[], uint16_t size)
{
	volatile const uint8_t  * const receive_buff_ptr = reinterpret_cast<volatile uint8_t *>(ram_address);
	uint16_t k = 0;

	for(k = 0; k<size;k++)
	{
		buffer[k] = *(receive_buff_ptr + k);
	}
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_SHARED_RAM::H747_SHARED_RAM(const H747_SHARED_RAM& orig)
{
    
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_SHARED_RAM::~H747_SHARED_RAM()
{
    
}



} //End of namespace Ram

} //End of namespace Peripherals



