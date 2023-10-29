/**
 ******************************************************************************
  * @file		: ds_spi_h747.hpp
  * @brief		: spi Interface Class
  *				  This file contains spi interface class
  * @author		: Yunus As
  *
  * @date		: 14.04.2020
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


#ifndef DS_SPI_H747_HPP_
#define DS_SPI_H747_HPP_

#include <stdint.h>
#include <ds_spi.hpp>
#include "spi.h"





namespace Peripherals
{

namespace Spi  // if spi library must be changed , it prevents us to change the inner of class defin.
{

/*
 * Begin of Macro and Literals Definitions
 */

//#define SPI_BUFFER_ENABLED

 //End of Macro and Literals Definitions


/*
 * Begin of Enum, Union and Struct Definitions
 */



/**
 * @brief The number of available SPI peripheral in H747
 */

enum class spi_module_type : uint8_t {
	H747_SPI1 = 0x00 , /*!<SPI_1 handle*/
	H747_SPI2 = 0x01 , /*!<SPI_2 handle*/
	H747_SPI3 = 0x02 , /*!<SPI_3 handle*/
	H747_SPI4 = 0x03 , /*!<SPI_4 handle*/
	H747_SPI5 = 0x04  /*!<SPI_5 handle*/
};

/**
 * @brief The transfer size of SPI peripheral in H747
 */
enum class spi_transaction_size_type : uint8_t {
	SPI_8_BIT 	= 0x00 , /*!<SPI_1 handle*/
	SPI_16_BIT  = 0x01 , /*!<SPI_2 handle*/
	SPI_32_BIT  = 0x02 , /*!<SPI_3 handle*/

};

/**
 * @brief The status of ongoing SPI process which is related to upper layers
 */
enum class spi_module_status_type : uint8_t {
	SPI_DEVICE_READY 		= 0x00,
	SPI_CONFIG_ON_PROCESS 	= 0x01,
	SPI_TRANSMIT_ON_PROCESS = 0x02,
	SPI_RECEIVE_ON_PROCESS 	= 0x03,
	SPI_NONE 				= 0x04,
	SPI_ERROR 				= 0x05
};

/**
 * @brief The status of ongoing SPI process which is related to lower level
 */
enum class spi_state_type : uint8_t {
	SPI_STATE_RESET 	    = 0x00, /*!< Peripheral not Initialized                         */
	SPI_STATE_READY 	    = 0x01, /*!< Peripheral Initialized and ready for use           */
	SPI_STATE_BUSY 		    = 0x02, /*!< an internal process is ongoing                     */
	SPI_STATE_BUSY_TX 	    = 0x03, /*!< Data Transmission process is ongoing               */
	SPI_STATE_BUSY_RX 	    = 0x04, /*!< Data Reception process is ongoing                  */
	SPI_STATE_BUSY_TX_RX    = 0x05, /*!< Data Transmission and Reception process is ongoing */
	SPI_STATE_ERROR 	    = 0x06, /*!< SPI error state                                    */
	SPI_STATE_ABORT 	    = 0x07 /*!< SPI abort is ongoing                               */
};

enum class spi_speed_type : uint8_t {
     SPEED_HIGH,
     SPEED_LOW,
 };


// End of Enum, Union and Struct Definitions
/*
 * Begin of Derived class
 */

class H747_Spi : public  Base
{
public:

	H747_Spi(spi_module_type number, SPI_HandleTypeDef *handle );
	H747_Spi					(const H747_Spi &orig);
	virtual ~H747_Spi();


	bool         				initialize					()const override;
	bool 				        setHighSpeed				(const bool active);

	bool 					      receiveOnly					(uint8_t * recv,const uint32_t len);
	bool 					      transmitOnly				(const uint8_t *send,const uint32_t len);
	bool 							  transmitReceive 			(const uint8_t *send, const uint32_t send_len,uint8_t *recv, const uint32_t recv_len);

	void 					   	  transactionDisable 			(SPI_HandleTypeDef *hspi);
	bool 					      transactionEndProcedure		(SPI_HandleTypeDef *hspi);
	void 					      transactionErrorHandler	    (SPI_HandleTypeDef *hspi);


	spi_state_type        spi_state;
	uint8_t 			        spi_error_flag;
	uint8_t				        spi_timeout_error;
	const uint8_t          *tx_data_ptr;
	uint8_t 			        trans_size;
	uint8_t 			        trans_count;
	uint8_t 			        receiv_size;
	uint8_t 			        receiv_count;
	spi_transaction_size_type	spi_trans_size;



private:
				/* Initialise CS pin config*/
	void 			initChipSelect(void)const;

				/* Get data from spi line */
	void 			receiveByte   (uint8_t);

				/* Get data from receive buffer*/
	uint16_t 		getDataFromBuffer			(uint8_t buffer[]);

	SPI_HandleTypeDef 			*spi_handle_ptr;
	GPIO_TypeDef 			    	*spi_cs_port;
	uint32_t 				    		spi_cs_pin;
	spi_module_type 	 			spi_number;
	bool 				        		spi_transfer_completed;
	bool 				        		spi_receive_completed;



	const static uint16_t 					SPI_BUFFER_SIZE 								= 255;
	const static uint16_t 					SPI_BUFFER_SIZE_MASK 						= SPI_BUFFER_SIZE -1;
    					 uint8_t  					receive_buffer[SPI_BUFFER_SIZE] = {0};
    					 uint16_t           receive_head 		  							= 0;
    					 uint16_t           receive_tail 		  							= 0;
    					 bool	              receive_buffer_overrun 	  	    = false;


  const static uint8_t	    SPI_TIMEOUT_ERROR_C 			= 20; //ms
  const static uint8_t	    SPI_TIMEOUT_C					= 10; //ms
};





} // END of  Spi namespace

}// END of  Peripherial namespace


/*
* External Linkages
*/
   /* Uncomment global handle objects if necessary*/

//extern  Peripherals::Spi::H747_Spi spi_1;
  extern  Peripherals::Spi::H747_Spi spi_2;
//extern  Peripherals::Spi::H747_SPI spi_3;
//extern  Peripherals::Spi::H747_SPI spi_4;
//extern  Peripherals::Spi::H747_Spi spi_5;

// End of External Linkages





#endif /* DS_SPI_H747_HPP_ */
