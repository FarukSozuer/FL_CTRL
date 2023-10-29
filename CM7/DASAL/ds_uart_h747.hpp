/**
 ******************************************************************************
  * @file			: ds_uart_h747.hpp
  * @brief		: UART Interface Class
  *				  		This file contains uart interface class
  * @author		: Onur Yildirim
  * 			  		Faruk Sozuer
  * @date			: 10.02.2020
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

#ifndef DS_UART_H747_HPP
#define	DS_UART_H747_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
#include "ds_uart.hpp"
#include "usart.h"
// End of Includes



namespace Peripherals
{

namespace Uart
{


/*
 * Begin of Macro Definitions
 */
const uint16_t UART_BUFFER_SIZE 					= 2048;
const uint16_t UART_BUFFER_SIZE_MASK 			= UART_BUFFER_SIZE -1;
const uint16_t UART_BUFFER_TRANSFER_LIMIT	= 256;
 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */

/**
 * @brief
 */
enum class uart_module_type : uint8_t
{
	H747_LPUART1 = 0,	/*!<LPUART handle*/
	H747_USART1  = 1,	/*!<USART1 handle*/
	H747_USART2  = 2,	/*!<USART2 handle*/
	H747_USART3  = 3,	/*!<USART3 handle*/
	H747_UART4   = 4,	/*!<UART4 handle*/
	H747_UART5   = 5,	/*!<UART5 handle*/
	H747_USART6  = 6,	/*!<USART6 handle*/
	H747_UART7   = 7,	/*!<UART7 handle*/
	H747_UART8   = 8,	/*!<UART8 handle*/
};
// End of Enum, Union and Struct Definitions



/*
 * Begin of BASE Abstract Class Definition
 */
class H747_UART: public BASE
{
	public:
		H747_UART(uart_module_type number, UART_HandleTypeDef *handle);

		void 	 		initialize				(void) override;
		bool 	 		changeBaudRate 		(uint32_t baudrate) override;
		void 	 		sendData					(uint8_t buffer[], uint16_t size) override;
		uint16_t 	getDataFromBuffer	(uint8_t buffer[], uint16_t size_limit = UART_BUFFER_TRANSFER_LIMIT) override;
		float 	 	getBaudRateError	(uint32_t usart_ker_ck_pres);

		void		receiveByte		(uint8_t buffer) override;
		bool 	 	transmit			(void) override;

		H747_UART(const H747_UART& orig);
		virtual ~H747_UART();

	protected:

	private:
		uart_module_type    uart_number;
		UART_HandleTypeDef *uart_handle;

		uint8_t  receive_buffer[UART_BUFFER_SIZE]	= {0};
		uint16_t receive_head 		  			   			=  0;
		uint16_t receive_tail 		  			   			=  0;
		bool	 receive_buffer_overrun 	  	   		= false;

		uint8_t  transmit_buffer[UART_BUFFER_SIZE]	= {0};
		uint16_t transmit_head 		  			   				=  0;
		uint16_t transmit_tail 		   			   				=  0;
};
// End of H747_UART Class Definition


} //End of namespace Uart

} //End of namespace Peripherals



/*
 * External Linkages
 */
extern Peripherals::Uart::H747_UART lpuart1;
extern Peripherals::Uart::H747_UART uart1;
extern Peripherals::Uart::H747_UART uart2;
extern Peripherals::Uart::H747_UART uart3;
extern Peripherals::Uart::H747_UART uart4;
extern Peripherals::Uart::H747_UART uart5;
extern Peripherals::Uart::H747_UART uart6;
extern Peripherals::Uart::H747_UART uart7;
extern Peripherals::Uart::H747_UART uart8;
// End of External Linkages


#endif	/* DS_UART_H747_HPP */

