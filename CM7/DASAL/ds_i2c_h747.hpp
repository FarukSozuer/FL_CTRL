/**
 ******************************************************************************
  * @file		: ds_i2c_h747.cpp
  * @brief		: i2c Interface Class
  *				  This file contains i2c interface class
  * @author		: Yunus As
  *
  * @date		: 13.03.2020
  * @version	: 0.1.0
  * @summary	: The first version of i2c iterface class
  * 			  Class routines and variables were created
  * 			  All interrupts are binded with user interface
  * 			  Compass interconnection was created.
  *
  * @date		: 10.08.2020
  * @version	: 0.1.0
  * 			  The convention and code style rules applied in accordance with CPP_CS_GUIDE_v2
  *
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


#ifndef DS_I2C_H747_HPP_
#define DS_I2C_H747_HPP_

#include <stdint.h>
#include <i2c.h>
#include <ds_i2c.hpp>





namespace Peripherals
{

namespace I2c  // if i2c library must be changed , it prevents us to change the inner of class defin.
{

/*
 * Begin of Macro and Literals Definitions
 */

 //End of Macro and Literals Definitions


/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class i2c_module_type : uint8_t
{
	H747_I2C1 = 0, /*!<I2C1 handle*/
	H747_I2C2 = 1, /*!<I2C2 handle*/
	H747_I2C3 = 2, /*!<I2C3 handle*/
	H747_I2C4 = 3, /*!<I2C4 handle*/

};

enum class i2c_state_type : uint8_t
{
	I2C_STATE_RESET 	    = 0x00, /*!< Peripheral not Initialized                          */
	I2C_STATE_READY 	    = 0x01, /*!< Peripheral Initialized and ready for use            */
	I2C_STATE_BUSY 		    = 0x02, /*!< an internal process is ongoing                      */
	I2C_STATE_BUSY_TX 	  = 0x03, /*!< Data Transmission process is ongoing                */
	I2C_STATE_BUSY_RX 	  = 0x04, /*!< Data Reception process is ongoing                   */
	I2C_STATE_ERROR 	    = 0x05, /*!<I2C  error state                                     */
	I2C_STATE_ABORT 	    = 0x06 /*!< I2C abort is ongoing                                 */

};
 // End of Enum, Union and Struct Definitions

/*
 * Begin of Derived class
 */

class H747_I2C : public  BASE
{
public:

	H747_I2C(i2c_module_type number, I2C_HandleTypeDef *handle);
	void initialize() override;

	 /* reset: reset the related i2c perip.*/
	void reset ();
	 /* transmitToBuffer: write data to i2c TXDR registers */
	bool transmitFromBuffer(void) override;
	/* receiveByteToBuffer: copy a byte value to receive buffer*/
	void receiveByteToBuffer(const uint8_t buffer) override;
	 /* transmit: for i2c devices which is not compatible with register memory conventions */
	bool transmit(const uint16_t devAddress,const  uint8_t *p_Data,const int16_t data_Size);
	 /* transmit: for i2c devices which  follow register memory conventions */
	bool transmit(const uint16_t devAddress, const uint16_t memAddress,const uint8_t memAddSize, const uint8_t *p_Data, uint16_t data_Size);
	/* readRegister: Read  bytes from a device which is not compatiable with register memory convention */
	bool readRegister (const uint16_t devAddress, uint8_t *p_Data,const uint16_t data_Size); //TODO will be applied later
	/* readRegister: Read  bytes from register of memory */
	bool readRegister (const uint16_t devAddress,const uint16_t memAddress,const uint8_t memAddSize, uint8_t *p_Data,uint16_t data_Size);
	/* setHighSpeed: change the i2c speed*/
	bool setHighSpeed	(const bool active);
	H747_I2C(const H747_I2C &orig);
	virtual ~H747_I2C(); // Indeed if the derived class is chosen as polymorphic class, it is useful

	i2c_state_type 			i2c_state;
	uint16_t						i2c_reset_count;
	uint8_t 			    	i2c_read_error;
	uint8_t 			    	i2c_write_error;
	uint8_t 				  	i2c_timeout_error;
	bool 				    		i2c_transfer_completed;
	bool 				    		i2c_receive_completed;

private:

	/* configTransfer: config the CR2 register of i2c*/
	void configTransfer(uint16_t devAddress, uint8_t size,
			uint32_t mode, uint32_t request);

	/* registerRequestRead: start write operation with the register adress*/
	bool registerRequestWrite(const uint16_t DevAddress,
			const uint16_t MemAddress, const uint16_t MemAddSize,
			const uint32_t Tickstart, const uint32_t Timeout);

	/* registerRequestRead: start read operation with the register adress*/
	bool registerRequestRead(const uint16_t DevAddress,
			const uint16_t MemAddress, const uint16_t MemAddSize,
			const uint32_t Tickstart,  const uint32_t Timeout);

	/* sendMultiByte: for i2c devices which send multiple bytes to buffer*/
	void sendMultiByte(const uint8_t buffer[], const uint16_t size);
	/* sendSinlgeByte: for i2c devices which send only single byte buffer*/
	void sendSinlgeByte(const uint8_t data);

	/* getDataFromBuffer: get value from receive buffer*/
	uint16_t getDataFromBuffer(uint8_t buffer[]) override;

	static constexpr uint16_t I2C_BUFFER_SIZE 			= 256; // Configure later according to sensor data rate values
	static constexpr uint16_t I2C_BUFFER_SIZE_MASK 	= I2C_BUFFER_SIZE - 1;


	i2c_module_type 			i2c_number;
	uint16_t 							i2c_device_adress;
	I2C_HandleTypeDef 		*i2c_handle_ptr;

	uint8_t 				receive_buffer[I2C_BUFFER_SIZE] = { 0 };
	uint16_t 				receive_head 							= 0;
	uint16_t 				receive_tail 							= 0;
	bool 					receive_buffer_overrun 			= false;

	uint8_t 				transmit_buffer[I2C_BUFFER_SIZE] = { 0 };
	uint16_t 				transmit_head 				 	 = 0;
	uint16_t 				transmit_tail 				 	 = 0;
};





} // END of  i2c namespace

}// END of  peripherial namespace


/*
* External Linkages
*/
  /* TODO: After activate all i2c peripherial , uncomment all */

//extern Peripherals::i2c::H747_I2C i2c1;
//extern Peripherals::i2c::H747_I2C i2c2;
//extern Peripherals::i2c::H747_I2C i2c3;
 extern Peripherals::I2c::H747_I2C i2c4;

// End of External Linkages




#endif /* DS_I2C_H747_HPP_ */
