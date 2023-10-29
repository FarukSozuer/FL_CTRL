/**
 ******************************************************************************
 * @file		: ds_spi_h747.cpp
 * @brief		: spi Interface Class
 *				  This file contains uart interface class
 * @author		: Yunus As
 *
 * @date		: 20.05.2020
 * @version	: 0.2.1
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
#include "ds_spi_h747.hpp"

// End of Includes



using namespace Peripherals::Spi;

/*
 * Begin of Object Definitions
 */

/* Create a object of spi to use in C code*/
	/* Uncomment objects if necessary*/
//Peripherals::Spi::H747_Spi spi_1(Peripherals::Spi::spi_module_type::H747_SPI1, &hspi1);
  Peripherals::Spi::H747_Spi spi_2(Peripherals::Spi::spi_module_type::H747_SPI2, &hspi2);
//Peripherals::Spi::H747_Spi spi_3(Peripherals::Spi::spi_module_type::H747_SPI3, &hspi3);
//Peripherals::Spi::H747_Spi spi_4(Peripherals::Spi::spi_module_type::H747_SPI4, &hspi4);
//Peripherals::Spi::H747_Spi spi_5(Peripherals::Spi::spi_module_type::H747_SPI5, &hspi5);


  // End of Object Definitions

/*
 * Begin of Private Interrupt Callback Functions
 */
#ifdef __cplusplus
extern "C"
{
#endif


/**
  * @brief 		SPI1 Receive and Transmit Interrupt Routine Handler: 	HAL_SPI_IRQHandler->    hspi->TxISR(hspi)
  * 																							hspi->RxISR(hspi)
  * @param[in]  SPI_HandleTypeDef *hspi: SPI_HandleTypeDef Instance
  *
  * @return 	void	Nothing
  */
static void dsSpi1InterruptCallback(SPI_HandleTypeDef *hspi_handle)
{

}


/**
  * @brief 		SPI2 Receive and Transmit Interrupt Routine Handler: 	HAL_SPI_IRQHandler->    hspi->TxISR(hspi)
  * 																							hspi->RxISR(hspi)
  * @param[in]  SPI_HandleTypeDef *hspi: SPI_HandleTypeDef Instance
  *
  * @return 	void	Nothing
  */
static void dsSpi2InterruptCallback(SPI_HandleTypeDef *hspi_handle)
{

	uint32_t it_source 	     	= hspi_handle->Instance->IER;
	uint32_t it_flag 	     		= hspi_handle->Instance->SR;
	uint32_t trigger 	     		= it_source & it_flag;

	bool handled 			 		 		= false;

	/* SPI in mode Transmitter and Receiver ------------------------------------*/
	if (HAL_IS_BIT_CLR(trigger,
			SPI_FLAG_OVR) && HAL_IS_BIT_CLR(trigger, SPI_FLAG_UDR) && HAL_IS_BIT_SET(trigger, SPI_FLAG_DXP))
	{
		/* Receive data in 8 Bit mode */
		if (spi_2.spi_trans_size == spi_transaction_size_type::SPI_8_BIT)
		{

			/*Get the data from RXDR and buffer it*/
#ifdef 	SPI_BUFFER_ENABLED
			uint8_t received_data_u8 = (*(__IO uint8_t*) &hspi_handle->Instance->RXDR);
			spi_2.receiveByte(received_data_u8);

#else
			*((uint8_t*) hspi_handle->pRxBuffPtr) = (*(__IO uint8_t*) &hspi_handle->Instance->RXDR); //TODO: Reinterpreter_cast and Const_cast both  can be applied
			hspi_handle->pRxBuffPtr 			 += sizeof(uint8_t);
#endif

			*(__IO uint8_t*) &hspi_handle->Instance->TXDR = *(spi_2.tx_data_ptr);
			spi_2.tx_data_ptr							 += sizeof(uint8_t);

			if (spi_2.trans_count != 0)
			{
				spi_2.trans_count--;
			}
			else
			{
				// Disable interrupt SPI_IT_TXP flags
				__HAL_SPI_DISABLE_IT(hspi_handle, SPI_IT_TXP);
			}
			if (spi_2.receiv_count != 0)
			{
				spi_2.receiv_count--;
			}
			else
			{
				// Disable interrupt SPI_IT_RXP flags
				__HAL_SPI_DISABLE_IT(hspi_handle, SPI_IT_RXP);
			}
		}
		/* Receive data in 16 Bit mode */
		else if (spi_2.spi_trans_size == spi_transaction_size_type::SPI_16_BIT)
		{
			//TODO: Implementation will be done later!
		}

		/* Receive data in 32 Bit mode */
		else
		{

		}
		handled = true;

	}
	/* SPI in mode Receiver ----------------------------------------------------*/
	if (HAL_IS_BIT_CLR(trigger,
			SPI_FLAG_OVR) && HAL_IS_BIT_SET(trigger, SPI_FLAG_RXP) && HAL_IS_BIT_CLR(trigger, SPI_FLAG_DXP))
	{
		/* Receive data in 8 Bit mode */
		if (spi_2.spi_trans_size == spi_transaction_size_type::SPI_8_BIT)
		{
#ifdef SPI_BUFFER_ENABLED

			received_data_u8 = (*(__IO uint8_t*) &hspi_handle->Instance->RXDR);
			spi_2.receiveByte(received_data_u8);
#else
			*((uint8_t*) hspi_handle->pRxBuffPtr) = (*(__IO uint8_t*) &hspi_handle->Instance->RXDR);
			hspi_handle->pRxBuffPtr 			 += sizeof(uint8_t);
#endif
			if (spi_2.receiv_count != 0)
			{
				spi_2.receiv_count--;
			}
			else
			{
				// Disable interrupt SPI_IT_RXP flags
				__HAL_SPI_DISABLE_IT(hspi_handle, SPI_IT_RXP);
			}

		}

		/* Receive data in 16 Bit mode */
		else if (spi_2.spi_trans_size == spi_transaction_size_type::SPI_16_BIT)
		{
			//TODO: 16 bit transaction will be applied here
		}

		/* Receive data in 32 Bit mode */
		else
		{

		}
		handled = true;
	}
	/* SPI in mode Transmitter -------------------------------------------------*/
	if (HAL_IS_BIT_CLR(trigger,
			SPI_FLAG_UDR) && HAL_IS_BIT_SET(trigger, SPI_FLAG_TXP) && HAL_IS_BIT_CLR(trigger, SPI_FLAG_DXP))
	{
		/* Receive data in 8 Bit mode */
		if (spi_2.spi_trans_size == spi_transaction_size_type::SPI_8_BIT)
		{
			*(__IO uint8_t*) &hspi_handle->Instance->TXDR = *(spi_2.tx_data_ptr);
			spi_2.tx_data_ptr							 += sizeof(uint8_t);
			if (spi_2.trans_count != 0)
			{
				spi_2.trans_count--;
			}
			else
			{
				// Disable interrupt SPI_IT_TXP flags
				__HAL_SPI_DISABLE_IT(hspi_handle, SPI_IT_TXP);
			}

		}
		/* Receive data in 16 Bit mode */
		else if (spi_2.spi_trans_size == spi_transaction_size_type::SPI_16_BIT)
		{
			//TODO: 16 bit transaction will be applied here
		}

		/* Receive data in 32 Bit mode */
		else
		{

		}

		handled = true;

	}
	if (handled != false)
	{
		if (spi_2.trans_count == 0 && spi_2.receiv_count == 0)
		{
			if(spi_2.transactionEndProcedure(hspi_handle) !=true)
			{
				spi_2.transactionDisable(hspi_handle); // TODO: will be tested later
			}
		}
		return;
	}

	/* End of Transfer Process */
	spi_2.transactionEndProcedure(hspi_handle);
	/* Handler of Spi Transfer Errors */
	spi_2.transactionErrorHandler(hspi_handle);
}


/**
  * @brief 		SPI3 Receive and Transmit Interrupt Routine Handler: 	HAL_SPI_IRQHandler->    hspi->TxISR(hspi)
  * 																							hspi->RxISR(hspi)
  * @param[in]  SPI_HandleTypeDef *hspi: SPI_HandleTypeDef Instance
  *
  * @return 	void	Nothing
  */
static void dsSpi3InterruptCallback(SPI_HandleTypeDef *hspi_handle)
{
}


/**
  * @brief 		SPI4 Receive and Transmit Interrupt Routine Handler: 	HAL_SPI_IRQHandler->    hspi->TxISR(hspi)
  * 																							hspi->RxISR(hspi)
  * @param[in]  SPI_HandleTypeDef *hspi: SPI_HandleTypeDef Instance
  *
  * @return 	void	Nothing
  */
static void dsSpi4InterruptCallback(SPI_HandleTypeDef *hspi_handle)
{

}


/**
  * @brief 		SPI5 Receive and Transmit Interrupt Routine Handler: 	HAL_SPI_IRQHandler->    hspi->TxISR(hspi)
  * 																							hspi->RxISR(hspi)
  * @param[in]  SPI_HandleTypeDef *hspi: SPI_HandleTypeDef Instance
  *
  * @return 	void	Nothing
  */
static void dsSpi5InterruptCallback(SPI_HandleTypeDef *hspi_handle)
{

}


#ifdef __cplusplus
}
#endif

// End of Private Interrupt Callback Functions
namespace Peripherals
{

namespace Spi
{

/**
 *  @brief User defined constructor
 *
 *  @param[in]	number : Selected Module
 *
 * 				spi_module_type:@arg @ref	H747_SPI1
								@arg @ref	H747_SPI2
								@arg @ref	H747_SPI3
								@arg @ref	H747_SPI4
 *                              @arg @ref	H747_SPI5
 *
 *  @param[in]	handle 		: SPI_HandleTypeDef *hspi
 *
 * Example:
 * @code
 * Peripherals::Spi::H747_Spi spi_1(Peripherals::Spi::spi_module_type::H747_SPI1, &hspi1);
 * @endcode
 */
H747_Spi::H747_Spi(spi_module_type number,  SPI_HandleTypeDef *handle) :
		spi_state(spi_state_type::SPI_STATE_READY),
		spi_error_flag(0),
		spi_timeout_error(0),
		tx_data_ptr(nullptr),
		trans_size(0),
		trans_count(0),
		receiv_size(0),
		receiv_count(0),
		spi_trans_size(spi_transaction_size_type::SPI_8_BIT),
		spi_handle_ptr(handle),
		spi_cs_port(nullptr),
		spi_cs_pin(0),
		spi_number(number),
		spi_transfer_completed(false),
		spi_receive_completed(false)
{
	switch (number)
	{
	case spi_module_type::H747_SPI1:

		// The configuration will be set later
		break;
	case spi_module_type::H747_SPI2:
		spi_cs_port = GPIOA;
		spi_cs_pin = GPIO_PIN_11;
		break;
	case spi_module_type::H747_SPI3:
		// TODO:Check the datasheet and pin configuration then writed down here!
		break;
	default:
		spi_cs_port = GPIOA;
		spi_cs_pin = GPIO_PIN_11;
		return;
	}

	if (handle->Init.DataSize > SPI_DATASIZE_16BIT) // 32 bit mode
	{
		spi_trans_size = spi_transaction_size_type::SPI_32_BIT;

	}
	else if (handle->Init.DataSize > SPI_DATASIZE_8BIT
			&& handle->Init.DataSize <= SPI_DATASIZE_16BIT) //16 bit mode
	{
		spi_trans_size = spi_transaction_size_type::SPI_16_BIT;
	}
	else
	{
		spi_trans_size = spi_transaction_size_type::SPI_8_BIT; // 8 bit mode
	}

	/*Initialize the chip select */
	initChipSelect();

}

/**
 * @brief Default destructor
 *
 * @param[in]  void
 *
 * @return 	void
 */
H747_Spi::~H747_Spi()
{

}

/**
 * @brief Function for the installation of the corresponding spi line.
 *
 * @return 	void	Nothing
 *
 * Example:
 * @code
 * spi_2.initialize();
 * @endcode
 */
bool H747_Spi::initialize()const
{

	if (spi_handle_ptr == NULL || !(spi_state == spi_state_type::SPI_STATE_READY))
	{
		return false;
	}
	spi_handle_ptr->ErrorCode = HAL_I2C_ERROR_NONE;

	/* Initialization of Callbacks of Spi */
	switch (spi_number)
	{
	case Peripherals::Spi::spi_module_type::H747_SPI1:

		spi_handle_ptr->TxISR = dsSpi1InterruptCallback;
		spi_handle_ptr->RxISR = dsSpi1InterruptCallback;

		break;
	case Peripherals::Spi::spi_module_type::H747_SPI2:

		spi_handle_ptr->TxISR = dsSpi2InterruptCallback;
		spi_handle_ptr->RxISR = dsSpi2InterruptCallback;

		break;
	case Peripherals::Spi::spi_module_type::H747_SPI3:

		spi_handle_ptr->TxISR = dsSpi3InterruptCallback;
		spi_handle_ptr->RxISR = dsSpi3InterruptCallback;

		break;
	case Peripherals::Spi::spi_module_type::H747_SPI4:

		spi_handle_ptr->TxISR = dsSpi4InterruptCallback;
		spi_handle_ptr->RxISR = dsSpi4InterruptCallback;

		break;
	case Peripherals::Spi::spi_module_type::H747_SPI5:

		spi_handle_ptr->TxISR = dsSpi5InterruptCallback;
		spi_handle_ptr->RxISR = dsSpi5InterruptCallback;

		break;
	default:
		break;
	}

	return true;
}

/**
  * @brief 		A public  function that handle the spi transaction error cases and clear error interrupt flags
  *				It has been used in InterruptCallback functions
  *
  * @param[in]  SPI_HandleTypeDef *hspi_handle: SPI_HandleTypeDef Instance
  *
  * @return 	void
  *
  */
void H747_Spi::transactionErrorHandler(SPI_HandleTypeDef *hspi_handle)
{
	if (hspi_handle == nullptr)
	{
		return;
	}
	uint32_t it_source 	= hspi_handle->Instance->IER;
	uint32_t it_flag 	= hspi_handle->Instance->SR;
	uint32_t trigger 	= it_source & it_flag;

	if (HAL_IS_BIT_SET(it_flag,SPI_FLAG_SUSP) && HAL_IS_BIT_SET(it_source, SPI_FLAG_EOT))
	{
		/* Abort on going, clear SUSP flag to avoid infinite looping */
		__HAL_SPI_CLEAR_SUSPFLAG(hspi_handle);
		return;
	}

	/* SPI in Error Treatment --------------------------------------------------*/
	if ((trigger & (SPI_FLAG_MODF | SPI_FLAG_OVR | SPI_FLAG_FRE | SPI_FLAG_UDR))
			!= 0UL)
	{
		/* SPI Overrun error interrupt occurred ----------------------------------*/
		if ((trigger & SPI_FLAG_OVR) != 0UL)
		{
			SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_OVR);
			__HAL_SPI_CLEAR_OVRFLAG(hspi_handle);
		}

		/* SPI Mode Fault error interrupt occurred -------------------------------*/
		if ((trigger & SPI_FLAG_MODF) != 0UL)
		{
			SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_MODF);
			__HAL_SPI_CLEAR_MODFFLAG(hspi_handle);
		}

		/* SPI Frame error interrupt occurred ------------------------------------*/
		if ((trigger & SPI_FLAG_FRE) != 0UL)
		{
			SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_FRE);
			__HAL_SPI_CLEAR_FREFLAG(hspi_handle);
		}

		/* SPI Underrun error interrupt occurred ------------------------------------*/
		if ((trigger & SPI_FLAG_UDR) != 0UL)
		{
			SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_UDR);
			__HAL_SPI_CLEAR_UDRFLAG(hspi_handle);
		}

		if (hspi_handle->ErrorCode != HAL_SPI_ERROR_NONE)
		{
			/* Disable SPI peripheral */
			__HAL_SPI_DISABLE(hspi_handle);

			/* Disable all interrupts */
			__HAL_SPI_DISABLE_IT(hspi_handle,
					SPI_IT_EOT | SPI_IT_RXP | SPI_IT_TXP | SPI_IT_MODF| SPI_IT_OVR | SPI_IT_FRE | SPI_IT_UDR);

			/* Disable the SPI DMA requests  removed from here  */
			//TODO: if DMA later is considered to implement, code chunk will be added here

			spi_state = spi_state_type::SPI_STATE_READY;
			/* Increment the error flag */
			spi_error_flag++;
		}
		return;
	}

}


/**
  * @brief 		A public  function that disable the transaction  and clear error flags
  *
  * @param[in]  SPI_HandleTypeDef *hspi_handle: SPI_HandleTypeDef Instance
  *
  * @return void
  */

void H747_Spi::transactionDisable(SPI_HandleTypeDef *hspi_handle)
{
	if (hspi_handle == nullptr)
	{
		return;
	}
	uint32_t it_flag 			= hspi_handle->Instance->SR;
	spi_state_type dummy_state 	= spi_state;

	/*Clear EOT and TXT flags */
	__HAL_SPI_CLEAR_EOTFLAG(hspi_handle);
	__HAL_SPI_CLEAR_TXTFFLAG(hspi_handle);

	/* Disable SPI peripheral */
	__HAL_SPI_DISABLE(hspi_handle);

	/* Disable ITs */
	__HAL_SPI_DISABLE_IT(hspi_handle,(SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

	if (dummy_state != spi_state_type::SPI_STATE_BUSY_RX)
	{
		if ((it_flag & SPI_FLAG_UDR) != 0UL)
		{
			SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_UDR);
			__HAL_SPI_CLEAR_UDRFLAG(hspi_handle);
		}
	}
	/* Report OverRun error for non TX Only communication */
	if (dummy_state != spi_state_type::SPI_STATE_BUSY_TX)
	{
		if ((it_flag & SPI_FLAG_OVR) != 0UL)
		{
			SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_OVR);
			__HAL_SPI_CLEAR_OVRFLAG(hspi_handle);
		}
	}
	if ((it_flag & SPI_FLAG_MODF) != 0UL)
	{
		SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_MODF);
		__HAL_SPI_CLEAR_MODFFLAG(hspi_handle);
	}

	/* SPI Frame error interrupt occurred ------------------------------------*/
	if ((it_flag & SPI_FLAG_FRE) != 0UL)
	{
		SET_BIT(hspi_handle->ErrorCode, HAL_SPI_ERROR_FRE);
		__HAL_SPI_CLEAR_FREFLAG(hspi_handle);
	}

	/* clear count of transaction*/
	receiv_count 	= (uint8_t) 0;
	trans_count 	= (uint8_t) 0;
}

/**
  * @brief 		A public  function that handle the disable procdeure of  spi transaction
  *
  * @param[in]  void	Nothing.
  *
  * @return 	bool	true  : success
  * 					false : failure
  */

bool H747_Spi::transactionEndProcedure(SPI_HandleTypeDef *hspi_handle)
{
	if (hspi_handle == nullptr)
	{
		return false;
	}

	spi_state_type state_temp 	= spi_2.spi_state;
	uint32_t it_source 			= hspi_handle->Instance->IER;
	uint32_t it_flag 			= hspi_handle->Instance->SR;
	uint32_t trigger 			= it_source & it_flag;


	/* SPI End Of Transfer: DMA or IT based transfer */
	if (HAL_IS_BIT_SET(trigger, SPI_FLAG_EOT))
	{
		/* Clear EOT/TXTF/SUSP flag */
		__HAL_SPI_CLEAR_EOTFLAG(hspi_handle);
		__HAL_SPI_CLEAR_TXTFFLAG(hspi_handle);
		__HAL_SPI_CLEAR_SUSPFLAG(hspi_handle);

		/* Disable EOT interrupt */
		__HAL_SPI_DISABLE_IT(hspi_handle, SPI_IT_EOT);

		/* Pooling remaining data */
		while (receiv_count != 0)
		{

#ifdef SPI_BUFFER_ENABLED

			uint8_t	received_data_u8 = (*(__IO uint8_t*) &hspi_handle->Instance->RXDR);
			receiveByte(received_data_u8);

#else
			*((uint8_t*) hspi_handle->pRxBuffPtr) 	= (*(__IO uint8_t*) &hspi_handle->Instance->RXDR);
			hspi_handle->pRxBuffPtr 				+= sizeof(uint8_t);
#endif
			receiv_count--;
		}

		/* Call SPI Standard close procedure */
		transactionDisable(hspi_handle);

		spi_state = spi_state_type::SPI_STATE_READY;

		/* Call appropriate user callback */
		if (state_temp == spi_state_type::SPI_STATE_BUSY_TX_RX)
		{
			spi_transfer_completed 	= true;
			spi_receive_completed 	= true;

		}
		else if (state_temp == spi_state_type::SPI_STATE_BUSY_RX)
		{
			spi_receive_completed = true;

		}
		else if (state_temp == spi_state_type::SPI_STATE_BUSY_TX)
		{
			spi_transfer_completed = true;

		}
		else
		{
			/* end of the appropriate call */
		}
		return true;
	}
	return false;
}

/**
  * @brief 		A function that read and write bytes to  probe
  *
  * @param[in]  const uint8_t  *send	: TXDR data to send to related spi port
  *
  * @param[in]  const uint32_t send_len : Length of data to send to spi line
  *
  * @param[in]  uint8_t *recv 			: RXDR data read from the related spi port is kept in this variable
  *
  * @param[in]  const uint32_t recv_len : Length of data to read from spi line
  *
  *
  * @return 	bool	true  : success
  * 					false : failure
  */
bool H747_Spi::transmitReceive(const uint8_t *send, const uint32_t send_len,uint8_t *recv, const uint32_t recv_len)
{
	/* In case of any other transmission  request break transmission */
	if (!(spi_state == spi_state_type::SPI_STATE_READY))
	{
		return false;
	}

	if (spi_handle_ptr->Init.NSS == SPI_NSS_SOFT) // Software Chip Select
	{
		if (spi_cs_port) // cs for low
		{
			spi_cs_port->BSRR = spi_cs_pin;
		}
	}
	else
	{
		//Hardware Chip Select

	}

	if (recv_len == 0)
	{
		spi_state = spi_state_type::SPI_STATE_BUSY_TX;
		spi_transfer_completed = false;
		spi_receive_completed  = true;
	}
	if (send_len == 0)
	{
		spi_state = spi_state_type::SPI_STATE_BUSY_RX;
		spi_transfer_completed = true;
		spi_receive_completed  = false;
	}

	if (recv_len > 0 && send_len > 0)
	{
		spi_state = spi_state_type::SPI_STATE_BUSY_TX_RX;
		spi_transfer_completed = false;
		spi_receive_completed  = false;
	}

	trans_size  = (uint8_t) send_len;
	receiv_size = (uint8_t) recv_len;
	receiv_count= (uint8_t) recv_len;
	trans_count = (uint8_t) send_len;

	tx_data_ptr 		       = send;
	spi_handle_ptr->pRxBuffPtr = (uint8_t*) recv;

	/* Tsize value programmed to 0 for unknown transfer size */
	MODIFY_REG(spi_handle_ptr->Instance->CR2, SPI_CR2_TSIZE,(uint32_t )recv_len);

	/* Enable SPI peripheral */
	__HAL_SPI_ENABLE(spi_handle_ptr);

	/* Enable EOT, RXP, TXP, DXP, UDR, OVR, FRE, MODF and TSERF interrupts */
	__HAL_SPI_ENABLE_IT(spi_handle_ptr,
			(SPI_IT_EOT | SPI_IT_RXP | SPI_IT_TXP | SPI_IT_DXP | SPI_IT_UDR| SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF | SPI_IT_TSERF));

	/*  Master start the transaction*/
	SET_BIT(spi_handle_ptr->Instance->CR1, SPI_CR1_CSTART);

	volatile uint32_t tick_start = HAL_GetTick();

	while (spi_receive_completed == false && spi_transfer_completed == false)
	{

		if (((HAL_GetTick() - tick_start) > SPI_TIMEOUT_C))
		{

			spi_timeout_error++;
			if (spi_timeout_error == SPI_TIMEOUT_ERROR_C)
			{
				//TODO: disable spi class sources if it is necessary or then close and open spi for a moment.
			}
			break;  //  wait for 20ms than break the loop
		}
	}
#ifdef SPI_BUFFER_ENABLED
	getDataFromBuffer(recv);
#else
	// Pointer semantics work
#endif

	if (spi_handle_ptr->Init.NSS == SPI_NSS_SOFT)
	{

		if (spi_cs_port) //cs for high
		{
			spi_cs_port->BSRR = (uint32_t) spi_cs_pin << 16; //TODO:Test if the pin gets high
		}

	}
	return true;

}

/**
  * @brief 		A function that only write bytes to  probe. It is derived from @transmitReceive function
  *
  * @param[in]  const uint8_t  *send	: TXDR data to send to related spi port
  *
  * @param[in]  const uint32_t send_len : Length of data to send to spi line
  *
  * @return 	bool	true  : success
  * 					false : failure
  */
bool H747_Spi::transmitOnly(const uint8_t *send, const uint32_t send_len)
{
	return transmitReceive(send, send_len,nullptr, 0);
}

/**
  * @brief 		A function that only read   bytes from  probe
  *
  * @param[in]  uint8_t *recv 			: RXDR data read from the related spi port is kept in this variable
  *
  * @param[in]  const uint32_t recv_len : Length of data to read from spi line
  *
  *
  * @return 	bool	true  : success
  * 					false : failure
  */
bool H747_Spi::receiveOnly( uint8_t *recv, const uint32_t recv_len)
{


	transmitReceive(nullptr, 0, recv, recv_len);
	return true;



}

/**
  * @brief 		A function that reads one byte of data and keep it in a receive_buffer
  *
  * @param[in]  uint8_t buffer : RXDThe data read from the related spi port is kept in this variable.
  *
  * @return 	void	Nothing
  */
void H747_Spi::receiveByte(uint8_t buffer)
{
	receive_buffer[ (receive_head++) & SPI_BUFFER_SIZE_MASK ] = buffer;
	if(receive_head == receive_tail)
	{
		receive_buffer_overrun = true;
	}

}

/**
  * @brief 		A function that get the data from spi buffer.
  *
  * @param[in]  uint8_t buffer[] : related buffer that keeps the data
  *
  * @return 	uint16_t		 : size of data to be read
  */
uint16_t  H747_Spi::getDataFromBuffer	(uint8_t buffer[])
{
		uint16_t size = 0;
		while( (receive_tail & SPI_BUFFER_SIZE_MASK) != (receive_head & SPI_BUFFER_SIZE_MASK) )
		{
			buffer[size++] = receive_buffer[(receive_tail++) & SPI_BUFFER_SIZE_MASK ];
		}
		return size;
}


/**
  * @brief 		A private  function that intialize chip select pin for SPI peripherial.
  *
  * @param[in]  void	Nothing.
  *
  * @return 	void	Nothing
  */
void H747_Spi::initChipSelect()const
{
	if (spi_cs_port) { // general chip select configuration
		// Enable clock
		if (spi_cs_port == GPIOA)
			__HAL_RCC_GPIOA_CLK_ENABLE();
		else if (spi_cs_port == GPIOB)
			__HAL_RCC_GPIOB_CLK_ENABLE();
		else if (spi_cs_port == GPIOC)
			__HAL_RCC_GPIOC_CLK_ENABLE();
		else if (spi_cs_port == GPIOD)
			__HAL_RCC_GPIOD_CLK_ENABLE();
		else if (spi_cs_port == GPIOE)
			__HAL_RCC_GPIOE_CLK_ENABLE();
		else if (spi_cs_port == GPIOF)
			__HAL_RCC_GPIOF_CLK_ENABLE();
		else if (spi_cs_port == GPIOG)
			__HAL_RCC_GPIOG_CLK_ENABLE();
		else if (spi_cs_port == GPIOH)
			__HAL_RCC_GPIOH_CLK_ENABLE();

		HAL_GPIO_WritePin(spi_cs_port, spi_cs_pin, GPIO_PIN_SET); // set pin high after configuration = chip is NOT selected

		GPIO_InitTypeDef GPIO_InitStruct 	= { 0 };
		GPIO_InitStruct.Pin 				= spi_cs_pin;
		GPIO_InitStruct.Mode 				= GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull 				= GPIO_NOPULL;
		GPIO_InitStruct.Speed 				= GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(spi_cs_port, &GPIO_InitStruct);

	}

	else {

		//Hardware Chip Select Configuration TODO: Later if needed will be implemented

	}


}

}//End of namespace Spi

} //End of namespace Peripherals

