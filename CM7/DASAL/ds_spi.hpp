/**
 ******************************************************************************
  * @file		: ds_spi.hpp
  * @brief		: Spi Interface Class
  *				  This file contains i2c interface class library
  * @author		: Yunus As
  *
  * @date		: 11.04.2020
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
#ifndef DS_SPI_HPP_
#define DS_SPI_HPP_

/*
 * Begin of Includes
 */
#include <stdint.h>
#include <spi.h>

// End of includes

namespace Peripherals
{
 namespace Spi // if i2c library must be changed , it prevents us to change the inner of class defin.
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
 * Begin of BASE abstract class
 */

class Base
{
public:
	Base();

	virtual  bool 	 initialize			(void)const										=	0;
//	virtual void 	 sendData			(const uint8_t buffer[],const uint16_t size)	= 	0;
//	virtual bool 	 transmitToTarget	(void)											=	0;
//	virtual void 	 receiveByte		(const uint8_t buffer)							=	0;
//	virtual uint16_t getDataFromBuffer	(uint8_t buffer[]) 								=	0;

	Base(const Base& orig); //TODO :  Move ctor and Move assignment
	virtual ~Base(); // no need pure virtual dtor against any objects of BASE class



};
// end of  BASE Class Definition

}// end of i2c namespace


}// end of Peripherals namespace



/*
* External Linkages
*/

// End of External Linkages





#endif /* DS_SPI_HPP_ */
