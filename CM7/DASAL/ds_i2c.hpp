/**
 ******************************************************************************
  * @file		: ds_i2c.hpp
  * @brief		: i2c Interface Class
  *				  This file contains i2c interface class library
  * @author		: Yunus As
  *
  * @date		: 13.03.2020
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

#ifndef DS_I2C_HPP_
#define DS_I2C_HPP_


/*
 * Begin of Includes
 */
#include <stdint.h>
// End of includes

namespace Peripherals
{

 namespace I2c // if i2c library must be changed , it prevents us to change the inner of class defin.
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

class BASE
{
public:
	BASE();

	virtual void 	 initialize					(void) 					=	0;
	virtual bool 	 transmitFromBuffer			(void)	=	0;
	virtual void 	 receiveByteToBuffer		(const uint8_t buffer)=0;
	virtual uint16_t getDataFromBuffer			(uint8_t buffer[]) =0;

	BASE(const BASE& orig); //TODO :  Move ctor and Move assignment
	virtual ~BASE(); // no need pure virtual dtor against any objects of BASE class



};
// end of  BASE Class Definition

}// end of i2c namespace


}// end of Peripherals namespace



/*
* External Linkages
*/

// End of External Linkages



#endif /* DS_I2C_HPP_ */
