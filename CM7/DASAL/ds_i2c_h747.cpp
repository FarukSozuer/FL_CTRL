/**
 ******************************************************************************
 * @file		: ds_i2c_h747.cpp
 * @brief		: i2c Interface Class
 *				  This file contains uart interface class
 * @author		: Yunus As
 *
 * @date		: 12.03.2020
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
// Begin of Includes
#include "ds_i2c_h747.hpp"

// End of Includes

using namespace Peripherals::I2c;
// Peripherals::i2c::H747_I2C i2c1(Peripherals::i2c::i2c_module_type::H747_I2C1,   &hi2c1);
// Peripherals::i2c::H747_I2C i2c2(Peripherals::i2c::i2c_module_type::H747_I2C2,   &hi2c2);
// Peripherals::i2c::H747_I2C i2c3(Peripherals::i2c::i2c_module_type::H747_I2C3,   &hi2c3);
   Peripherals::I2c::H747_I2C i2c4(Peripherals::I2c::i2c_module_type::H747_I2C4,   &hi2c4);

/*
 * Begin of Private Interrupt Callback Functions
 */
#ifdef __cplusplus
extern "C"
{
#endif

//TODO : Uncomment them when all i2c peripherial will be activated
/**
 * @brief 		I2C1 Receive and Transmit Interrupt Rountine
 *
 * @param[in]  __I2C_HandleTypeDef *hi2c : i2C Instance
 * @param[in]  uint32_t it_flags			 : Interrupt flags bit status(ISR register)
 * @param[in]  uint32_t it_sources		 : Flags of Source register (CR1 register)
 *
 * @return		HAL_StatusTypeDef
 */
static HAL_StatusTypeDef DS_I2C1__InterruptCallback(I2C_HandleTypeDef *hi2c_ptr,
		uint32_t it_flags, uint32_t it_sources)
{
	return HAL_OK;
}
/**
 * @brief 		I2C2 Receive and Transmit Interrupt Rountine
 *
 * @param[in]  __I2C_HandleTypeDef *hi2c : i2C Instance
 * @param[in]  uint32_t it_flags			 : Interrupt flags bit status(ISR register)
 * @param[in]  uint32_t it_sources		 : Flags of Source register (CR1 register)
 *
 * @return		HAL_StatusTypeDef
 */
static HAL_StatusTypeDef DS_I2C2__InterruptCallback(I2C_HandleTypeDef *hi2c_ptr,
		uint32_t it_flags, uint32_t it_sources)
{
	return HAL_OK;
}
/**
 * @brief 		I2C3 Receive and Transmit Interrupt Rountine
 *
 * @param[in]  __I2C_HandleTypeDef *hi2c : i2C Instance
 * @param[in]  uint32_t it_flags			 : Interrupt flags bit status(ISR register)
 * @param[in]  uint32_t it_sources		 : Flags of Source register (CR1 register)
 *
 * @return		HAL_StatusTypeDef
 */
static HAL_StatusTypeDef DS_I2C3__InterruptCallback(I2C_HandleTypeDef *hi2c_ptr,
		uint32_t it_flags, uint32_t it_sources)
{
	return HAL_OK;

}
/**
 * @brief 		I2C4 Receive and Transmit Interrupt Rountine
 *
 * @param[in]  __I2C_HandleTypeDef *hi2c : i2C Instance
 * @param[in]  uint32_t it_flags			 : Interrupt flags bit status(ISR register)
 * @param[in]  uint32_t it_sources		 : Flags of Source register (CR1 register)
 *
 * @return 		HAL_StatusTypeDef
 */
static HAL_StatusTypeDef DS_I2C4__InterruptCallback(I2C_HandleTypeDef *hi2c_ptr,
		uint32_t it_flags, uint32_t it_sources)
{

	uint32_t tmpit_flags = it_flags;
	/* I2C events treatment -------------------------------------*/

	if ((I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_AF) != RESET)
			&& (I2C_CHECK_IT_SOURCE(it_sources, I2C_IT_NACKI) != RESET))
	{
		/* Clear NACK Flag */
		__HAL_I2C_CLEAR_FLAG(hi2c_ptr, I2C_FLAG_AF);

		if (__HAL_I2C_GET_FLAG(hi2c_ptr, I2C_FLAG_TXIS) != RESET)
		{
			hi2c_ptr->Instance->TXDR = 0x00U;  // TO SET TXIS=0
		}
		__HAL_I2C_CLEAR_FLAG(hi2c_ptr, I2C_FLAG_TXE);
	}
	else if ((I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_RXNE) != RESET)
			&& (I2C_CHECK_IT_SOURCE(it_sources, I2C_IT_RXI) != RESET))
	{

		/* Remove RXNE flag on temporary variable as read done */
		tmpit_flags &= ~I2C_FLAG_RXNE;

		/* Buffer Configuration to read data from RXDR */
		uint8_t received_data = (uint8_t) hi2c_ptr->Instance->RXDR;
		i2c4.receiveByteToBuffer(received_data);

	}
	else if ((I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_TXIS) != RESET)
			&& (I2C_CHECK_IT_SOURCE(it_sources, I2C_IT_TXI) != RESET))
	{
		/* Write data to TXDR */
		i2c4.transmitFromBuffer();
	}
	else if ((I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_TCR) != RESET)
			&& (I2C_CHECK_IT_SOURCE(it_sources, I2C_IT_TCI) != RESET))
	{
		if ((hi2c_ptr->XferCount != 0U) && (hi2c_ptr->XferSize == 0U)) // TODO: Remove it unless it necessary
		{

		}
		else
		{
			// nothing
		}
	}
	else if ((I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_TC) != RESET)
			&& (I2C_CHECK_IT_SOURCE(it_sources, I2C_IT_TCI) != RESET))
	{
		if (hi2c_ptr->XferCount == 0U)
		{
			if (I2C_GET_STOP_MODE(hi2c_ptr) != I2C_AUTOEND_MODE)
			{
				/* Generate a stop condition in case of no transfer option */
				/* Generate Stop */
				hi2c_ptr->Instance->CR2 |= I2C_CR2_STOP;
			}
		}
		else
		{
			/* Wrong size Status regarding TC flag event */
		}
	}

	else
	{
		/* Nothing to do */
	}

			/* Stop and clear flags process */
	if ((I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_STOPF) != RESET)
			&& (I2C_CHECK_IT_SOURCE(it_sources, I2C_IT_STOPI) != RESET))
	{
		/* Call I2C Master complete process */

		uint32_t tmpit_flags = it_flags;

		/* Clear STOP Flag */
		__HAL_I2C_CLEAR_FLAG(hi2c_ptr, I2C_FLAG_STOPF);

		/* Disable Interrupts and Store Previous state */
		if (i2c4.i2c_state == i2c_state_type::I2C_STATE_BUSY_TX)
		{
			uint32_t tmpisr_t = 0;
			tmpisr_t = 0;
			tmpisr_t |= I2C_IT_TCI | I2C_IT_TXI;
			tmpisr_t |= I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;

			__HAL_I2C_DISABLE_IT(hi2c_ptr, tmpisr_t);

		}
		else if (i2c4.i2c_state == i2c_state_type::I2C_STATE_BUSY_RX)
		{
			uint32_t tmpisr_t = 0;
			tmpisr_t |= I2C_IT_TCI | I2C_IT_RXI;
			tmpisr_t |= I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;

			__HAL_I2C_DISABLE_IT(hi2c_ptr, tmpisr_t);
		}
		else
		{
			/* Do nothing */
		}

		/* Clear Configuration Register 2 */
		I2C_RESET_CR2(hi2c_ptr);

		/* Reset handle parameters */

		if (I2C_CHECK_FLAG(tmpit_flags, I2C_FLAG_AF) != RESET)
		{
			/* Clear NACK Flag */
			__HAL_I2C_CLEAR_FLAG(hi2c_ptr, I2C_FLAG_AF);
			/* Set acknowledge error code */

		}

		/* Flush TX register */
		/* If a pending TXIS flag is set */
		/* Write a dummy data in TXDR to clear it */
		if (__HAL_I2C_GET_FLAG(hi2c_ptr, I2C_FLAG_TXIS) != RESET)
		{
			hi2c_ptr->Instance->TXDR = 0x00U;
		}

		/* Flush TX register if not empty */
		if (__HAL_I2C_GET_FLAG(hi2c_ptr, I2C_FLAG_TXE) == RESET)
		{
			__HAL_I2C_CLEAR_FLAG(hi2c_ptr, I2C_FLAG_TXE);
		}

		if (hi2c_ptr->State == HAL_I2C_STATE_BUSY_TX
				|| i2c4.i2c_state == i2c_state_type::I2C_STATE_BUSY_TX)
		{
			i2c4.i2c_state 				= i2c_state_type::I2C_STATE_READY;
			i2c4.i2c_transfer_completed = true;

		}

		else if (i2c4.i2c_state == i2c_state_type::I2C_STATE_BUSY_RX)
		{
			i2c4.i2c_state 				= i2c_state_type::I2C_STATE_READY;
			i2c4.i2c_receive_completed 	= true;
		}
		else
		{
			/* Nothing to do */
		}
	}

	/* Process Unlocked */
	__HAL_UNLOCK(hi2c_ptr);

	return HAL_OK;

}

#ifdef __cplusplus
}
#endif

// End of Private Interrupt Callback Functions
namespace Peripherals
{

namespace I2c
{

/**
 *  @brief User defined constructor
 *
 * @param[in]	number : Selected Module
 *
 * i2c_module_type:@arg @ref	H747_I2C1
 	 	 	 	   @arg @ref	H747_I2C2
 	 	 	 	   @arg @ref	H747_I2C3
 	 	 	 	   @arg @ref	H747_I2C4
 *
 *
 * @param[in]	handle
 *
 */
H747_I2C::H747_I2C(i2c_module_type number, I2C_HandleTypeDef *handle) :
		BASE(),
		i2c_state(i2c_state_type::I2C_STATE_RESET),
		i2c_read_error(0),
		i2c_write_error(0),
		i2c_timeout_error(0),
		i2c_transfer_completed(false),
		i2c_receive_completed(false),
		i2c_number(number),
		i2c_device_adress(0x00),
		i2c_handle_ptr(handle)
{

}

/**
 * @brief Default destructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
H747_I2C::~H747_I2C()
{

}

/**
 * @brief 		Function for the installation of the corresponding i2c line.
 *
 * @return 	void	Nothing
 *
 * Example:
 * @code
 * i2c1.initialize();
 * @endcode
 */
void H747_I2C::initialize()


{
	static uint8_t dummy 	= 0;
	i2c_read_error				=	0;
	i2c_timeout_error			=	0;
	i2c_write_error				=	0;

	i2c_handle_ptr->Mode 				= HAL_I2C_MODE_MEM;// TODO: MUST BE MOVED TO CLASS SCOPE
	i2c_handle_ptr->ErrorCode 	= HAL_I2C_ERROR_NONE; // TODO: MUST BE MOVED TO CLASS SCOPE
	i2c_handle_ptr->pBuffPtr 		= &dummy;

	i2c_state					= i2c_state_type::I2C_STATE_READY;
	/* update CR2 register */

	switch (i2c_number)
	{
	case Peripherals::I2c::i2c_module_type::H747_I2C1:

		i2c_handle_ptr->XferISR = DS_I2C1__InterruptCallback;

		break;
	case Peripherals::I2c::i2c_module_type::H747_I2C2:

		i2c_handle_ptr->XferISR = DS_I2C2__InterruptCallback;

		break;
	case Peripherals::I2c::i2c_module_type::H747_I2C3:

		i2c_handle_ptr->XferISR = DS_I2C3__InterruptCallback;

		break;
	case Peripherals::I2c::i2c_module_type::H747_I2C4:

		i2c_handle_ptr->XferISR = DS_I2C4__InterruptCallback;

		break;
	default:
		break;
	}

}

/**
 * @brief 		Function reset the corresponding i2c line.
 *
 * @return 	void	Nothing
 *
 * Example:
 * @code
 * i2c1.reset();
 * @endcode
 */
void H747_I2C::reset()
{
	i2c_reset_count++;
	i2c_state = i2c_state_type::I2C_STATE_BUSY;
	__HAL_I2C_DISABLE(i2c_handle_ptr);
	HAL_I2C_Init(i2c_handle_ptr);
	initialize();

}

/**
 * @brief 		Transfers one byte from the corresponding i2c line.
 *
 * @param[in]  const uint8_t data: One byte to send buffer t.
 *
 * @return 	void	Nothing
 *
 * Example:
 * @code
 * i2c4.sendData(data);
 * @endcode
 */
void H747_I2C::sendSinlgeByte(const uint8_t data)
{
	transmit_buffer[(transmit_head++) & I2C_BUFFER_SIZE_MASK] = data;

}

/**
 * @brief 		A function that transfers a buffer from the corresponding serial line.
 *
 * @param[in]  uint8_t buffer[]: The buffer entry to be sent.
 * @param[in]  uint16_t size   : The buffer size you want to send.
 *
 * @return 	void	Nothing
 *
 * Example:
 * @code
 * i2c4.sendData(buffer, size);
 * @endcode
 */
void H747_I2C::sendMultiByte(const uint8_t buffer[], const uint16_t size)
{
	uint16_t k = 0U;
	for (k = 0; k < size; k++)
	{
		transmit_buffer[(transmit_head++) & I2C_BUFFER_SIZE_MASK] = buffer[k];
	}

}

/**
 * @brief 		A function that transmit one byte to i2c line .
 *
 * @return 	bool transmit_in_progress
 */
bool H747_I2C::transmitFromBuffer(void) //TODO: Fix it later
{

	bool transmit_in_progress = false;

	if ((transmit_tail & I2C_BUFFER_SIZE_MASK)!= (transmit_head & I2C_BUFFER_SIZE_MASK))
	{
		i2c_handle_ptr->Instance->TXDR = transmit_buffer[(transmit_tail++) & I2C_BUFFER_SIZE_MASK];
		transmit_in_progress = true;

	}
	else
	{
		transmit_in_progress = false;
	}

	return transmit_in_progress;

}

/**
 * @brief 		A function that reads one byte of data and send to buffer.
 *
 * @param[in]  const uint8_t buffer : RXDT  the related i2c port is kept in this variable.
 *
 * @return 	void	Nothing
 */
void H747_I2C::receiveByteToBuffer(const uint8_t buffer)
{
	receive_buffer[ (receive_head++) & I2C_BUFFER_SIZE_MASK ] = buffer;
	if(receive_head == receive_tail)
	{
		receive_buffer_overrun = true;
	}

}

/**
 * @brief 		A private function that reads one byte of data.
 *
 * @param[out]  uint8_t buffer[] : The data read from the related i2c port is kept in this variable.
 *
 * @return 	uint16_t size	  : Information on how many bytes are read.
 *
 * Example:
 * @code
 * uint32_t size =  i2c4.getDataFromBuffer(buffer);
 * @endcode
 */
uint16_t H747_I2C::getDataFromBuffer(uint8_t buffer[])
{
	uint16_t size = 0;

	while ((receive_tail & I2C_BUFFER_SIZE_MASK)!= (receive_head & I2C_BUFFER_SIZE_MASK))
	{
		buffer[size++] = receive_buffer[(receive_tail++) & I2C_BUFFER_SIZE_MASK];
	}

	return size;
}

/**
 * @brief 		A function that change the speed of i2c module .
 *
 * @param [in]  const bool active : True	:High speed		=	400khz
 * 								 	False	:Normal speed 	=	100khz
 * @return 	uint16_t size	  : Information on how many bytes are read.
 *
 *	@note
 * Example:
 * @code
 *  i2c4.set_High_Speed(true); // 400khz speed
 * @endcode
 */
bool H747_I2C::setHighSpeed(const bool active)
{
	uint8_t counter_Set_High_Speed = 0;
	bool change_Speed_Progress = true;

	if (active) //400khz
	{
		i2c_handle_ptr->Init.Timing = 0x00B03FDB; // settings without rise and fall time
	}

	else
	{
		i2c_handle_ptr->Init.Timing = 0x307075B1; // settings without rise and fall time
	}

	i2c_state = i2c_state_type::I2C_STATE_RESET; // in case of any other process violation

	CLEAR_BIT((i2c_handle_ptr)->Instance->CR1,
			(I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_TXIE));
	CLEAR_BIT((i2c_handle_ptr)->Instance->CR1, I2C_CR1_PE); // Clear the Peripherial enable

	//Reconfigure the peripherial

	i2c_handle_ptr->Init.OwnAddress1 		= 0;
	i2c_handle_ptr->Init.AddressingMode 	= I2C_ADDRESSINGMODE_7BIT;
	i2c_handle_ptr->Init.DualAddressMode 	= I2C_DUALADDRESS_DISABLE;
	i2c_handle_ptr->Init.OwnAddress2 		= 0;
	i2c_handle_ptr->Init.OwnAddress2Masks 	= I2C_OA2_NOMASK;
	i2c_handle_ptr->Init.GeneralCallMode 	= I2C_GENERALCALL_DISABLE;
	i2c_handle_ptr->Init.NoStretchMode 		= I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(i2c_handle_ptr) != HAL_OK)
	{
		counter_Set_High_Speed++;
	}
	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(i2c_handle_ptr, I2C_ANALOGFILTER_ENABLE)
			!= HAL_OK)
	{
		counter_Set_High_Speed++;
	}
	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(i2c_handle_ptr, 0) != HAL_OK)
	{
		counter_Set_High_Speed++;
	}

	if (counter_Set_High_Speed > 1)
	{
		change_Speed_Progress = false;
	}
	else
	{
		change_Speed_Progress = true;
	}

	this->initialize(); // re-initialize the related peripherial

	return change_Speed_Progress;
}

/**
 * @brief 		A function for the target devices which do not have register communication
 *
 * @param [in]  uint16_t :Target Device 7 bits address value
 * @param [in]  uint8_t  :p_Data
 * @param [in]  uint16_t :data_Size
 *
 * @return
 */
bool H747_I2C::transmit(const uint16_t devAddress, const uint8_t *p_Data,const int16_t data_Size)
{
	// Will be implemented in case of non registered sensor available
	return true;
}

/**
 * @brief The Function transmit to slave devices which have multi-register addresses
 *
 * @param [in] devAddress  	: Target Device Address	The device 7 bits address value
 * @param [in] memAddress	: Target Device Register Address
 * @param [in] memAddSize	: Size of Device Register Address
 * @param [in] p_Data		: The transmit data which will be sent to Slave Device
 * @param [in] data_Size		: The data size of transmit data
 *
 * @return
 */
bool H747_I2C::transmit(const uint16_t dev_address, const uint16_t mem_address,const uint8_t mem_add_size, const uint8_t *p_data_ptr, uint16_t data_size)
{

	if (i2c_state != i2c_state_type::I2C_STATE_READY)
	{
		++i2c_write_error;
		if (i2c_write_error > 50U) // Do not get stuck in!
		{
			i2c_write_error = 0;
			reset();
		}
		return false;
	}

	bool state_transmit_bool 					= false;
			 i2c_transfer_completed 			= false;
	volatile uint32_t tick_start    	= 0;

	if (__HAL_I2C_GET_FLAG(i2c_handle_ptr, I2C_FLAG_BUSY) == SET)
	{
		i2c_handle_ptr->State = HAL_I2C_STATE_BUSY;
		return false;
	}

		tick_start 	= HAL_GetTick();
		i2c_state 	= i2c_state_type::I2C_STATE_BUSY_TX;

		if (data_size > 1)
		{
			sendMultiByte(p_data_ptr, data_size);
		}

		else
		{
			sendSinlgeByte(*p_data_ptr);
		}

		if (registerRequestWrite(dev_address, mem_address, mem_add_size,tick_start, 25U) != true)
		{
			return false;
		}


		/* Config for write action*/
		configTransfer(dev_address, (uint8_t) data_size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);
		/*  Error, Transfer Completed,Stop, Acknowledge and Tx interrupt enabled*/
		SET_BIT(i2c_handle_ptr->Instance->CR1,(I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_TXIE));

		state_transmit_bool = true;


		while (i2c_transfer_completed == false)
			{
				if (((HAL_GetTick() - tick_start) > 25U))
				{
					return false;  // In case wait for more than 25ms, return error
				}
			}


	return state_transmit_bool;

}

/**
 * @brief A  private function which configure CR2 register
 *
 * @param [in] devAddress	: Target 7-bit device adress
 * @param [in] size	: Size Specifies the number of bytes  to be programmed.
 * 				  	This parameter must be between 0-255
 * @param [in] mode		:  Mode New state of the I2C START condition generation.
 * 					Mode types in the following @arg:
 * 					@arg @ref I2C_RELOAD_MODE Enable Reload mode .
 *     				@arg @ref I2C_AUTOEND_MODE Enable Automatic end mode.
 *     				@arg @ref I2C_SOFTEND_MODE Enable Software end mode.
 *
 * @param [in] request		: Request New state of the I2C START condition generation.
 *
 *				    @arg @ref I2C_NO_STARTSTOP Don't Generate stop and start condition.
 *		 		    @arg @ref I2C_GENERATE_STOP Generate stop condition (Size should be set to 0).
 *		 		    @arg @ref I2C_GENERATE_START_READ Generate Restart for read request.
 *				    @arg @ref I2C_GENERATE_START_WRITE Generate Restart for write request.
 */
void H747_I2C::configTransfer(uint16_t devAddress,uint8_t size, uint32_t mode, uint32_t request)
{

	MODIFY_REG(i2c_handle_ptr->Instance->CR2,
			((I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | (I2C_CR2_RD_WRN & (uint32_t)(request >> (31U - I2C_CR2_RD_WRN_Pos))) | I2C_CR2_START | I2C_CR2_STOP)),
			(uint32_t)(((uint32_t)devAddress & I2C_CR2_SADD) | (((uint32_t)size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES) | (uint32_t)mode | (uint32_t)request));
}

/**
 * @brief The private function that send the addresses and START-STOP communication for WRITE operations
 *
 *
 * @param [in] DevAddress	:Target 7-bit device address
 * @param [in] MemAddress	:Target device register address
 * @param [in] MemAddSize :Size of device register adress

 * @return [out]  Process complete condition
 */
bool H747_I2C::registerRequestWrite(const uint16_t DevAddress,
		const uint16_t MemAddress, const uint16_t MemAddSize,
		const uint32_t Tickstart, const uint32_t Timeout)
{
	configTransfer(DevAddress, (uint8_t) MemAddSize,
	I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);
	/* NACK is not checked*/
	/* Wait for Transmit Interrupt Register to be SET and Transmit Register ready to sent*/
	while (__HAL_I2C_GET_FLAG(i2c_handle_ptr, I2C_ISR_TXIS) == RESET)
	{
		if (((HAL_GetTick() - Tickstart) > Timeout))
		{

			return false;  // In case wait for more than 25ms, return error
		}
	}

	/* If Memory address size is 8Bit */
	if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
	{
		/* Memory address size sent to the Register */
		i2c_handle_ptr->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);

		while (__HAL_I2C_GET_FLAG(i2c_handle_ptr, I2C_ISR_TCR) == RESET)
		{
			if (((HAL_GetTick() - Tickstart) > Timeout))
			{
				++i2c_timeout_error;
				return false;
			}
		}
	}
	else
	{
		//Nothing
		//NOTE: 16bit memory adress is not used yet
	}
	return true;
}

/**
 * @brief The private function send the address registers and start the communication for READ operations
 *
 * @param DevAddress[in]    :   Target Device Address	The device 7 bits address value
 * @param MemAddress[in]    :   Target Device Register Address
 * @param MemAddSize[in]    :   Size of Device Register Address
 * @param Tickstart [in]    :   The start tick of timer when read request started
 * @param Timeout   [in]    :   The time interval which request must be established before it
 * @return
 */
bool H747_I2C::registerRequestRead(const uint16_t DevAddress,
		const uint16_t MemAddress, const uint16_t MemAddSize,
		const uint32_t Tickstart, const uint32_t Timeout)
{
	configTransfer(DevAddress, MemAddSize, I2C_SOFTEND_MODE,I2C_GENERATE_START_WRITE);
	/* NACK is not checked*/
	/* Wait for Transmit Interrupt Register to be SET and Transmit Register ready to sent*/
	while (__HAL_I2C_GET_FLAG(i2c_handle_ptr, I2C_ISR_TXIS) == RESET)
	{
		if (((HAL_GetTick() - Tickstart) > Timeout))
		{
			i2c_timeout_error++;
			return false;  // In case wait for more than 25ms, return error
		}

	}

	/* If Memory address size is 8Bit */
	if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
	{
		/* Memory address size sent to the Register */
		i2c_handle_ptr->Instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);

		while (__HAL_I2C_GET_FLAG(i2c_handle_ptr, I2C_ISR_TC) == RESET)
		{
			if (((HAL_GetTick() - Tickstart) > Timeout))
			{
				i2c_timeout_error++;
				return false;
			}
		}
	}
	else
	{
		//Nothing
		//NOTE: 16bit memory adress is not used yet
	}
	return true;

}

/**
 * @brief The  function that read the address registers  and get values
 *
 * @param   devAddress[in]    :   Target Device Address	The device 7 bits address value
 * @param   memAddress[in]    :   Target Device Register Address
 * @param   memAddSize[in]    :   Size of Device Register Address
 * @param   p_Data    [in]    :   Data to read
 * @param   data_Size [in]    :   The time interval which request must be established before it
 *
 * @return	bool			  :  success: true
 * 								 fail 	: false
 */
bool H747_I2C::readRegister(const uint16_t devAddress,
		const uint16_t memAddress, const uint8_t memAddSize, uint8_t *p_Data,
		uint16_t data_size)
{

	if (i2c_state != i2c_state_type::I2C_STATE_READY)
	{
		i2c_read_error++;

		if (i2c_read_error > 50U) // Do not get stuck in!
		{
			i2c_read_error 	= 0;
			reset();
			i2c_state 			= i2c_state_type::I2C_STATE_READY;
		}
		return false;
	}
	volatile uint32_t tick_start 	= HAL_GetTick();
	bool state_device_start 		= false;
	i2c_read_error 					= 0U;
	i2c_receive_completed 			= false;
//	i2c_handle_ptr->pBuffPtr 		= p_Data;
	i2c_state 						= i2c_state_type::I2C_STATE_BUSY_RX;

	if (registerRequestRead(devAddress, memAddress, memAddSize, tick_start, 25U) != true)
	{
		return false;
	}
	/* Config for read action*/
	configTransfer(devAddress, (uint8_t) data_size, I2C_AUTOEND_MODE,I2C_GENERATE_START_READ);

	/*  Error, Transfer Completed,Stop, Acknowledge and Rx interrupt enabled*/
	SET_BIT(i2c_handle_ptr->Instance->CR1,(I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_RXI));

	state_device_start = true;
	tick_start = HAL_GetTick();

	while (i2c_receive_completed == false)
	{
		if (((HAL_GetTick() - tick_start) > 25U))
		{
			i2c_timeout_error++;
			return false;  // In case wait for more than 25ms, return error
		}
	}

	if(data_size != getDataFromBuffer(p_Data))
	{

		++i2c_read_error;
		// Error case!
	}
	return state_device_start;

}


} //End of namespace i2c

} //End of namespace Peripherals

