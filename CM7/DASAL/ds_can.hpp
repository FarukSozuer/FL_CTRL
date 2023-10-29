/**
 ******************************************************************************
  * @file		: ds_can.hpp
  * @brief		: CAN Interface Class
  *				  This file contains can interface class
  * @author		: Onur Yildirim
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

#ifndef DS_CAN_HPP
#define	DS_CAN_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
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



/*
 * Begin of BASE Abstract Class Definition
 */
class BASE
{
    public:
		BASE();

		virtual void 	 initialize			(void) = 0;
		virtual void 	 sendData			(uint8_t buffer[], uint16_t size) = 0;
		virtual uint16_t getDataFromBuffer	(uint8_t buffer[]) = 0;

		virtual void receiveByte	(uint8_t byte)=0;
		virtual bool transmit		(void)=0;

		BASE(const BASE& orig);
        virtual ~BASE();

    protected:

    private:

};
// End of BASE Class Definition


} //End of namespace Can

} //End of namespace Peripherals



/*
 * External Linkages
 */

// End of External Linkages


#endif	/* DS_CAN_HPP */

