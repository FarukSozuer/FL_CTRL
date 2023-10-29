/**
 ******************************************************************************
  * @file			: ds_shared_ram_h747.hpp
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

#ifndef DS_SHARED_RAM_H747_HPP
#define	DS_SHARED_RAM_H747_HPP



/*
 * Begin of Includes
 */
#include <cstdint>
// End of Includes



namespace Peripherals
{

namespace Ram
{


/*
 * Begin of Macro Definitions
 */
const uint16_t MAXIMUM_BUFFER_SIZE	= 512;
 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */




/*
 * Begin of H747_SHARED_RAM Class Definition
 */
class H747_SHARED_RAM
{
	public:
		H747_SHARED_RAM();

		void	 	 initialize				( void );
		void	 	 scheduler				( void );
		uint16_t sendData					( uint8_t buffer[],
															  uint16_t size);
		uint16_t getDataFromBuffer( uint8_t buffer[],
																uint16_t size_limit = MAXIMUM_BUFFER_SIZE);

		void     sendDataToSharedRam ( uint32_t ram_address,
						 	 	 	 	 	 	 	 	 		 	 uint8_t buffer[],
																	 uint16_t size);
		void     getDataFromSharedRam( uint32_t ram_address,
																	 uint8_t buffer[],
																	 uint16_t size);

		H747_SHARED_RAM(const H747_SHARED_RAM& orig);
		virtual ~H747_SHARED_RAM();

	protected:

	private:
		bool	synchronization_complete;
		bool	isOtherCoreActive	( void );

};
// End of H747_SHARED_RAM Class Definition


} //End of namespace Ram

} //End of namespace Peripherals



/*
 * External Linkages
 */
extern Peripherals::Ram::H747_SHARED_RAM inter_core;
// End of External Linkages


#endif	/* DS_SHARED_RAM_H747_HPP */

