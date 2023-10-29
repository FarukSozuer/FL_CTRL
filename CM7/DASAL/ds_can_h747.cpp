/**
 ******************************************************************************
  * @file		: ds_can_h747.cpp
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



/*
 * Begin of Includes
 */
#include "ds_can_h747.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Peripherals::Can::H747_CAN can1(Peripherals::Can::can_module_type::H747_FDCAN1);
// End of Object Definitions



namespace Peripherals
{

namespace Can
{


/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_CAN::H747_CAN(can_module_type can_number):
BASE(),
can_number(can_number)
{ }



/**
  * @brief 		TBD
  *
  * @param[in]  void	Nothing
  *
  * @return 	void	Nothing
  */
void H747_CAN::initialize(void)
{

}



/**
  * @brief 		TBD
  *
  * @param[in]  uint8_t buffer[]: TBD
  * @param[in]  uint16_t size   : TBD
  *
  * @return 	void	Nothing
  */
void H747_CAN::sendData(uint8_t buffer[], uint16_t size)
{

}



/**
  * @brief 		TBD
  *
  * @param[out]  uint8_t buffer[] : TBD
  * @param[out]  uint16_t &size   : TBD
  *
  * @return 	void	Nothing
  */
uint16_t H747_CAN::getDataFromBuffer(uint8_t buffer[])
{
	return 0;
}




/**
  * @brief 		TBD
  *
  * @param[in]  uint8_t temp_byte : TBD
  *
  * @return 	void	Nothing
  */
void H747_CAN::receiveByte(uint8_t temp_byte)
{

}



/**
  * @brief 		TBD
  *
  * @param[in]  uint8_t temp_byte : TBD
  *
  * @return 	bool result
  */
bool H747_CAN::transmit(void)
{
	return true;
}




/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_CAN::H747_CAN(const H747_CAN& orig)
{
    
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_CAN::~H747_CAN()
{
    
}



} //End of namespace Can

} //End of namespace Peripherals

