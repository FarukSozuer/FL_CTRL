/**
 ******************************************************************************
  * @file		: ds_uart_h747.cpp
  * @brief		: UART Interface Class
  *				  This file contains uart interface class
  * @author		: Onur Yildirim
  * 			  Faruk Sozuer
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
#include "ds_uart_h747.hpp"
#include "ds_sbus2.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Peripherals::Uart::H747_UART lpuart1(Peripherals::Uart::uart_module_type::H747_LPUART1, &hlpuart1);
Peripherals::Uart::H747_UART uart1  (Peripherals::Uart::uart_module_type::H747_USART1,    &huart1);
Peripherals::Uart::H747_UART uart2  (Peripherals::Uart::uart_module_type::H747_USART2, 	  &huart2);
Peripherals::Uart::H747_UART uart3  (Peripherals::Uart::uart_module_type::H747_USART3,    &huart3);
Peripherals::Uart::H747_UART uart4  (Peripherals::Uart::uart_module_type::H747_UART4,     &huart4);
Peripherals::Uart::H747_UART uart5  (Peripherals::Uart::uart_module_type::H747_UART5,     &huart5);
Peripherals::Uart::H747_UART uart6  (Peripherals::Uart::uart_module_type::H747_USART6,    &huart6);
Peripherals::Uart::H747_UART uart7  (Peripherals::Uart::uart_module_type::H747_UART7,     &huart7);
Peripherals::Uart::H747_UART uart8  (Peripherals::Uart::uart_module_type::H747_UART8,     &huart8);
// End of Object Definitions



/*
 * Begin of Private Interrupt Callback Functions
 */
#ifdef __cplusplus
extern "C"
{
#endif


/**
  * @brief 		USART1 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART1_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart1.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart1.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

/**
  * @brief 		USART2 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART2_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart2.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart2.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

/**
  * @brief 		USART3 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART3_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart3.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart3.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

/**
  * @brief 		USART4 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART4_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart4.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart4.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

/**
  * @brief 		USART5 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART5_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart5.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart5.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

/**
  * @brief 		USART6 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART6_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart6.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart6.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}


/**
  * @brief 		USART7 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART7_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	uart7.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart7.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

/**
  * @brief 		USART8 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_USART8_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	//uart8.receiveByte(received_data);
    	sbus.receiveInterruptHandler(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = uart8.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}


/**
  * @brief 		LPUSART1 Receive and Transmit Interrupt Rountine
  *
  * @param[in]  UART_HandleTypeDef *huart : Uart Instance
  *
  * @return 	void
  */
static void DS_LPUSART1_InterruptCallback(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   		 	 = READ_REG(huart->Instance->ISR);
	uint32_t cr1its     			 = READ_REG(huart->Instance->CR1);
	uint32_t cr3its     			 = READ_REG(huart->Instance->CR3);
	uint8_t  received_data 			 = 0;

	bool	 is_transmit_in_progress = false;

    if ( ( (isrflags & USART_ISR_RXNE_RXFNE) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
    	   ( (cr3its & USART_CR3_RXFTIE) != 0U)
		 )
	   )
    {
    	received_data = (uint8_t) READ_REG(huart->Instance->RDR);
    	lpuart1.receiveByte(received_data);
    }


    if ( ( (isrflags & USART_ISR_TXE_TXFNF) != 0U)     &&
    	 ( ( (cr1its & USART_CR1_TXEIE_TXFNFIE) != 0U) ||
    	   ( (cr3its & USART_CR3_TXFTIE) != 0U)
		 )
	   )
	{
    	is_transmit_in_progress = lpuart1.transmit();
    	if(is_transmit_in_progress == false)
    	{
  	      CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
    	}
	}
}

#ifdef __cplusplus
}
#endif
// End of Private Interrupt Callback Functions



namespace Peripherals
{

namespace Uart
{


/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_UART::H747_UART(uart_module_type number, UART_HandleTypeDef *handle) :
BASE(),
uart_number(number),
uart_handle(handle)
{ }



/**
  * @brief 		Function for the installation of the corresponding uart line.
  *
  * @return 	void	Nothing
  *
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void H747_UART::initialize(void)
{
	static uint8_t dummy = 0;

	uart_handle->pRxBuffPtr  = &dummy;
	uart_handle->RxXferSize  = 1;
	uart_handle->RxXferCount = 1;
	uart_handle->ErrorCode   = HAL_UART_ERROR_NONE;

    SET_BIT(uart_handle->Instance->CR3, USART_CR3_EIE);
    SET_BIT(uart_handle->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);

	switch(uart_number)
	{
		case uart_module_type::H747_LPUART1 :
			uart_handle->RxISR = DS_LPUSART1_InterruptCallback;	///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_LPUSART1_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_USART1 :
			uart_handle->RxISR = DS_USART1_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART1_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_USART2 :
			uart_handle->RxISR = DS_USART2_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART2_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_USART3 :
			uart_handle->RxISR = DS_USART3_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART3_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_UART4 :
			uart_handle->RxISR = DS_USART4_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART4_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_UART5 :
			uart_handle->RxISR = DS_USART5_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART5_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_USART6 :
			uart_handle->RxISR = DS_USART6_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART6_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_UART7 :
			uart_handle->RxISR = DS_USART7_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART7_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		case uart_module_type::H747_UART8 :
			uart_handle->RxISR = DS_USART8_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			uart_handle->TxISR = DS_USART8_InterruptCallback; ///< While interrupt routines are init, they are handled to ISR registers.
			break;

		default :
			//Error
			break;
	}
}



/**
  * @brief 		TBD
  *
  * @param[in]  uint32_t baudrate  	 	  : Baudrate rate must be entered here.
  *
  * @return 	bool change_baud_progress : Returns information on whether the Uart line is correctly init.
  * Example:
  * @code
  * uart4.changeBaudRate(921600);
  * @endcode
  */
bool H747_UART::changeBaudRate(uint32_t baudrate)
{
	bool 	 change_baud_progress 		  = false;  ///< Returns the status of BaudRate Init.
	uint8_t  change_baud_progress_counter  = 0;		///< It controls the 4 steps required for baudrate initiation.

	/* Clear Interrupt Flag*/
	CLEAR_BIT(uart_handle->Instance->CR3, USART_CR3_EIE);
	CLEAR_BIT(uart_handle->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
	CLEAR_BIT(uart_handle->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);

	HAL_UART_DeInit(uart_handle); ///<Reset Uart Flag, Pin Assign

	/* Re-init according to the new baud value */
	uart_handle->Init.BaudRate = baudrate;
	uart_handle->Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle->Init.StopBits = UART_STOPBITS_1;
	uart_handle->Init.Parity = UART_PARITY_NONE;
	uart_handle->Init.Mode = UART_MODE_TX_RX;
	uart_handle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handle->Init.OverSampling = UART_OVERSAMPLING_16;
	uart_handle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart_handle->Init.ClockPrescaler = UART_PRESCALER_DIV1;
	uart_handle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	 if (HAL_UART_Init(uart_handle) != HAL_OK)
	 {
		 change_baud_progress_counter++;
	 }
	 if (HAL_UARTEx_SetTxFifoThreshold(uart_handle, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	 {
		 change_baud_progress_counter++;
	 }

	 if (HAL_UARTEx_SetRxFifoThreshold(uart_handle, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	 {
		 change_baud_progress_counter++;
	 }

	 if (HAL_UARTEx_DisableFifoMode(uart_handle) != HAL_OK)
	 {
		 change_baud_progress_counter++;
	 }

	 if (change_baud_progress_counter!=0)  ///< If any of the steps required for UartInit is corrupted, change_baud_progress is false.
	 {
		 change_baud_progress = false;
	 }
	 else
	 {
		 change_baud_progress = true;
	 }

	 /*The init function of the related object is running.*/
	 initialize();

	 return change_baud_progress;
}



/**
  * @brief 		Transfers a byte from the corresponding serial line.
  *
  * @param[in]  uint8_t buffer[]: The buffer entry to be sent.
  * @param[in]  uint16_t size   : The buffer size you want to send.
  *
  * @return 	void	Nothing
  *
  * Example:
  * @code
  * uart4.sendData(buffer, size);
  * @endcode
  */
void H747_UART::sendData(uint8_t buffer[], uint16_t size)
{
	uint16_t k = 0;

	for(k=0;k<size;k++)
	{
		transmit_buffer[ (transmit_head++) & UART_BUFFER_SIZE_MASK] = buffer[k];
	}
    SET_BIT(uart_handle->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
}



/**
  * @brief 		A function that reads one byte of data.
  *
  * @param[out]  uint8_t buffer[] : The data read from the related uart port is kept in this variable.
  *
  * @return 	uint16_t size	  : Information on how many bytes are read.
  *
  * Example:
  * @code
  * uint32_t size =  uart1.getDataFromBuffer(buffer);
  * @endcode
  */
uint16_t H747_UART::getDataFromBuffer(uint8_t buffer[], uint16_t size_limit)
{
	uint16_t size = 0;

	while( (receive_tail & UART_BUFFER_SIZE_MASK) != (receive_head & UART_BUFFER_SIZE_MASK) )
	{
		buffer[size++] = receive_buffer[(receive_tail++) & UART_BUFFER_SIZE_MASK ];
		if(size >= size_limit)
		{
			break;
		}
	}

	return size;
}



/**
  * @brief 		A function that reads one byte of data.
  *
  * @param[in]  uint8_t buffer : TBDThe data read from the related uart port is kept in this variable.
  *
  * @return 	void	Nothing
  */
void H747_UART::receiveByte(uint8_t buffer)
{
	receive_buffer[ (receive_head++) & UART_BUFFER_SIZE_MASK ] = buffer;
	if(receive_head == receive_tail)
	{
		receive_buffer_overrun = true;
	}

}



/**
  * @brief 		The function that transmit one byte.
  *
  * @return 	bool transmit_in_progress
  */
bool H747_UART::transmit(void)
{
	bool transmit_in_progress = false;

	if( ( transmit_tail & UART_BUFFER_SIZE_MASK ) != ( transmit_head & UART_BUFFER_SIZE_MASK ) )
	{
		uart_handle->Instance->TDR = transmit_buffer[ (transmit_tail++) & UART_BUFFER_SIZE_MASK ];
		transmit_in_progress = true;
	}
	else
	{
		transmit_in_progress = false;
	}

	return transmit_in_progress;
}



/**
  * @brief 		TBD
  *
  * @param[in]  uint32_t usart_ker_ck_pres : MCU UART Clock
  *
  * @return 	return baud_rate_error	   : MCU baud rate error ratio
  *
  * Example:
  * @code
  * uart4.changeBaudRate(9600);
  * @endcode
  */
float H747_UART::getBaudRateError(uint32_t usart_ker_ck_pres)
{
	float    baud_rate_error = 0;
	uint32_t actual_baud  	 = 0;
	uint16_t usart_div    	 = 0;
	uint32_t baud	   	  	 = 0;

	usart_div 		= static_cast<uint16_t>(uart_handle->Instance->BRR);
	baud 			= uart_handle->Init.BaudRate;

	if(usart_div > 0)
	{
		actual_baud  	= static_cast<uint16_t>(usart_ker_ck_pres/usart_div);
	}
	else
	{
		actual_baud = 1; //error
	}

	if(baud > 0)
	{
		baud_rate_error = (100 - ((static_cast<float>(actual_baud)/static_cast<float>(baud))*100));
	}
	else
	{
		baud_rate_error = 99;
	}

	return baud_rate_error;
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_UART::H747_UART(const H747_UART& orig)
{
    
}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
H747_UART::~H747_UART()
{
    
}



} //End of namespace Uart

} //End of namespace Peripherals



