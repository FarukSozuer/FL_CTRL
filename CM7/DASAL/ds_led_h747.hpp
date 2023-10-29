/**
 ******************************************************************************
  * @file			: ds_led_h747.hpp
  * @brief		: H747I-DISCO Board LED Library
  *				  		This file contains led class
  * @author		: Onur Yildirim
  * @date			: 18.02.2020
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

#ifndef DS_LED_H747_HPP
#define	DS_LED_H747_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



namespace Peripherals
{

namespace Led
{


/*
 * Begin of Macro Definitions
 */

 //End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class led_control_type : uint8_t
{
	OFF 		= 0,
	ON  		= 1,
	TOGGLE	= 2,
};
// End of Enum, Union and Struct Definitions



/*
 * Begin of H747_LED Class Definition
 */
class H747_LED
{
    public:
		H747_LED();

		void initialize(void);
		void scheduler (void);

		H747_LED(const H747_LED& orig);
        virtual ~H747_LED();

    protected:

    private:
        void changeState(uint8_t led_number, led_control_type state);
};
// End of H747_LED Class Definition


} //End of namespace Led

} //End of namespace Peripherals



/*
 * External Linkages
 */
extern Peripherals::Led::H747_LED led;
// End of External Linkages


#endif	/* DS_LED_H747_HPP */

