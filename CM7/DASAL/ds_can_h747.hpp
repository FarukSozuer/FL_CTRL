/**
 ******************************************************************************
  * @file		: ds_can_h747.hpp
  * @brief		: CAN Interface Class
  *				  This file contains can interface class
  * @author		:
  * @date		: 10.02.2020
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

#ifndef DS_CAN_H747_HPP
#define	DS_CAN_H747_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
#include "ds_can.hpp"
// End of Includes



namespace Peripherals
{

namespace Can
{


/*
 * Begin of Macro Definitions
 */

 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */

// End of Enum, Union and Struct Definitions
enum class can_module_type : uint8_t
{
	H747_FDCAN1 = 0,	/*!<LPUART handle*/
	H747_FDCAN2 = 1,	/*!<USART1 handle*/
};


/*
 * Begin of H747_CAN Class Definition
 */
class H747_CAN: public BASE
{
    public:
		H747_CAN(can_module_type can_number);

		void 	 initialize			(void) override;
		void 	 sendData			(uint8_t buffer[], uint16_t size) override;
		uint16_t getDataFromBuffer	(uint8_t buffer[]) override;

        void receiveByte	(uint8_t byte) override;
        bool transmit		(void) override;

		H747_CAN(const H747_CAN& orig);
        virtual ~H747_CAN();

    protected:

    private:
        can_module_type  can_number	= can_module_type::H747_FDCAN1;

        uint8_t  receive_buffer[2048] = {0};
        uint16_t receive_head 		  =  0;
        uint16_t receive_tail 		  =  0;
        bool	 buffer_overrun 	  = false;

        uint8_t  transmit_buffer[2048] = {0};
        uint16_t transmit_head 		   =  0;
        uint16_t transmit_tail 		   =  0;
};
// End of H747_CAN Class Definition


} //End of namespace Can

} //End of namespace Peripherals



/*
 * External Linkages
 */
extern Peripherals::Can::H747_CAN can1;
// End of External Linkages


#endif	/* DS_CAN_H747_HPP */

