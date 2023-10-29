/**
 ******************************************************************************
  * @file		: ds_sbus2.cpp
  * @brief	: Futaba SBUS & SBUS2 Telemetry Class
  *				  	This file contains SBUS communication driver
  *				  	https://github.com/BrushlessPower/SBUS2-Telemetry
  *				  	http://sbustelemetrysensors.blogspot.com/
  *				  	https://github.com/uzh-rpg/rpg_quadrotor_control/wiki/SBUS-Protocol
  *				  	https://forum.arduino.cc/index.php?topic=99708.0
  * @author	: Onur Yildirim
  * @date		: 12.05.2020
  * @version: 0.1.0
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
#include "ds_sbus2.hpp"
#include "tim.h"
#include "ds_uart_h747.hpp"
#include <cstring>
#include <cmath>
#include "ds_debug_tools.hpp"
// End of Includes



/*
 * Begin of Object Definitions
 */
Receiver::Futaba::SBUS2_TELEMETRY sbus;
// End of Object Definitions



namespace Receiver
{

namespace Futaba
{


/*
 * Begin of Local Constant Definitions
 */


/**
  * @brief Slot ID Matrix
  */
static const uint8_t slot_id[32] =
{
		0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
		0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
		0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
		0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB
};

//End of Local Constant Definitions



/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
SBUS2_TELEMETRY::SBUS2_TELEMETRY():
timer_enabled(false),
telemetry_initialized(false),
new_data_available(false),
payload(),
active_slot_multiplier(0),
rssi(0),
packet_loss_counter(0),
unprocessed_packet_counter(0),
out_of_range_packet_counter(0),
frame_status(frame_status_type::NOT_INITIALIZED),
receiver_status(receiver_status_type::RECEIVER_NOT_PLUGGED_IN)
{ }



/**
  * @brief 			Initialize the telemetry slots for transmit
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::initialize(void)
{
	updateTelemetry();
	telemetry_initialized = true;
}



/**
  * @brief 			Scheduler (Minimum calling frequency:100Hz)
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::scheduler(void)
{
	parseReceivedData();
	updateTelemetry();
}



/**
  * @brief 			Uart receive interrupt handler
  *
  * @param[in]	uint8_t data : Received Byte
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::receiveInterruptHandler(uint8_t data)
{
	static receive_handler_state_type state = receive_handler_state_type::HEADER;
	static uint16_t counter = 0;
	static uint8_t	buffer[FRAME_LENGTH] = {0};

	if(timer_enabled == false)
	{
		switch(state)
		{
			case receive_handler_state_type::HEADER:
				if(data == FRAME_START)
				{
					counter = 0;
					state = receive_handler_state_type::PAYLOAD;
				}
				break;


			case receive_handler_state_type::PAYLOAD:
				buffer[counter++] = data;
				if(counter >= (FRAME_LENGTH-1))
				{
					state = receive_handler_state_type::FOOTER;
				}
				break;


			case receive_handler_state_type::FOOTER :
				switch(data)
				{
					case FRAME_END_SLOT_0_7:
					case FRAME_END_SLOT_8_15:
					case FRAME_END_SLOT_16_23:
					case FRAME_END_SLOT_24_31:
						active_slot_multiplier = (data & 0xF0) >> 4;
						if(new_data_available == false)
						{
							std::memcpy(payload, buffer, FRAME_LENGTH);
							new_data_available = true;
						}
						else
						{
							unprocessed_packet_counter++;
						}
						frame_status = frame_status_type::SBUS2_TELEMETRY;
						state = receive_handler_state_type::HEADER;
						enableTimer();
						break;

					case FRAME_END_NON_TELEMETRY:
						if(buffer[21] != 0)
						{
							if(new_data_available == false)
							{
								std::memcpy(payload, buffer, FRAME_LENGTH);
								new_data_available = true;
							}
							else
							{
								unprocessed_packet_counter++;
							}
							frame_status = frame_status_type::SBUS_NON_TELEMETRY;
						}
						else
						{
							buffer[FRAME_LENGTH-1] = data;
							frame_status = frame_status_type::FRAME_NOT_RECOGNIZED;
							packet_loss_counter++;
						}
						state = receive_handler_state_type::HEADER;
						break;

					default :
						buffer[FRAME_LENGTH-1] = data;
						frame_status = frame_status_type::FRAME_NOT_RECOGNIZED;
						packet_loss_counter++;
						state = receive_handler_state_type::HEADER;
						break;
				}
				break;


			default :
				state = receive_handler_state_type::HEADER;
				break;
		}
	}
}



/**
  * @brief 			Timer interrupt handler for telemetry time frame
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::timerInterruptHandler(void)
{
	static timer_handler_state_type state = timer_handler_state_type::SLOT_O;
	static uint16_t counter = 0;

	if(telemetry_initialized == true)
	{
		switch(state)
		{
			case timer_handler_state_type::SLOT_O:
				counter = 0;
				changeTimer(SLOT_DELAY_US);
				if(active_slot_multiplier != 0)
				{
					if(slot.in_use[active_slot_multiplier*FRAME_SLOT_NUMBER + counter] == true)
					{
						sendData(&slot.data[active_slot_multiplier*FRAME_SLOT_NUMBER + counter][0]);
					}
				}
				counter++;
				state = timer_handler_state_type::SLOT_1_7;
				break;


			case timer_handler_state_type::SLOT_1_7:
				if(slot.in_use[active_slot_multiplier*FRAME_SLOT_NUMBER + counter] == true)
				{
					sendData(&slot.data[active_slot_multiplier*FRAME_SLOT_NUMBER + counter][0]);
				}
				counter++;
				if(counter >= FRAME_SLOT_NUMBER)
				{
					state = timer_handler_state_type::SLOT_END;
				}
				break;


			case timer_handler_state_type::SLOT_END:
				counter = 0;
				disableTimer();
				state = timer_handler_state_type::SLOT_O;
				break;


			default:
				counter = 0;
				disableTimer();
				state = timer_handler_state_type::SLOT_O;
				break;
		}
	}
	else
	{
		disableTimer();
	}
}



/**
  * @brief 			Update telemetry values
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::updateTelemetry(void)
{

	//Slot 1 - 7
	sensorTemp125(1, rssi);
	sensorTemp125(2, static_cast<int16_t>(monitor.cpu_load_active));
	sensorRpmSbs01Rmo(3, monitor.getMillis()/1000);
	sensorTempSbs01T(4, 51);
	sensorCurr1678slot3(5, 28.2, 25.2, 14500);

	//Slot 8 - 15
	sensorVoltageSbs01Vslot2(8, 25.1, 24.9);
	sensorVario1672slot2(10, 135, 2.4);
	sensorVoltageSbs01Vslot2(12, 24.2, 22.8);
	sensorVario1712slot2(14, 156, 3.6);

	//Slot 16 - 23



	//Slot 24 - 31
	sensorGps1675slot8(24, 12.0, 183, 4.7, 410995649, 293185987);
}



/**
  * @brief 			Received packet parser
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::parseReceivedData(void)
{
	const  uint8_t DATA_WAIT_LIMIT	 = 25;
	static uint8_t data_wait_counter =  0;
	sbus_flags_type flags;
	uint8_t k = 0;

	if(new_data_available == true)
	{
		data_wait_counter = 0;
		receiver_status = receiver_status_type::SBUS_ACTIVE;
		channel.raw[ 0] = ( (static_cast<uint16_t>(payload[ 0])   ) | (static_cast<uint16_t>(payload[ 1])<<8) ) & 0x07FF;
		channel.raw[ 1] = ( (static_cast<uint16_t>(payload[ 1])>>3) | (static_cast<uint16_t>(payload[ 2])<<5) ) & 0x07FF;
		channel.raw[ 2] = ( (static_cast<uint16_t>(payload[ 2])>>6) | (static_cast<uint16_t>(payload[ 3])<<2) | (static_cast<uint16_t>(payload[ 4])<<10) ) & 0x07FF;
		channel.raw[ 3] = ( (static_cast<uint16_t>(payload[ 4])>>1) | (static_cast<uint16_t>(payload[ 5])<<7) ) & 0x07FF;
		channel.raw[ 4] = ( (static_cast<uint16_t>(payload[ 5])>>4) | (static_cast<uint16_t>(payload[ 6])<<4) ) & 0x07FF;
		channel.raw[ 5] = ( (static_cast<uint16_t>(payload[ 6])>>7) | (static_cast<uint16_t>(payload[ 7])<<1) | (static_cast<uint16_t>(payload[ 8])<<9 ) ) & 0x07FF;
		channel.raw[ 6] = ( (static_cast<uint16_t>(payload[ 8])>>2) | (static_cast<uint16_t>(payload[ 9])<<6) ) & 0x07FF;
		channel.raw[ 7] = ( (static_cast<uint16_t>(payload[ 9])>>5) | (static_cast<uint16_t>(payload[10])<<3) ) & 0x07FF;
		channel.raw[ 8] = ( (static_cast<uint16_t>(payload[11])   ) | (static_cast<uint16_t>(payload[12])<<8) ) & 0x07FF;
		channel.raw[ 9] = ( (static_cast<uint16_t>(payload[12])>>3) | (static_cast<uint16_t>(payload[13])<<5) ) & 0x07FF;
		channel.raw[10] = ( (static_cast<uint16_t>(payload[13])>>6) | (static_cast<uint16_t>(payload[14])<<2) | (static_cast<uint16_t>(payload[15])<<10) ) & 0x07FF;
		channel.raw[11] = ( (static_cast<uint16_t>(payload[15])>>1) | (static_cast<uint16_t>(payload[16])<<7) ) & 0x07FF;
		channel.raw[12] = ( (static_cast<uint16_t>(payload[16])>>4) | (static_cast<uint16_t>(payload[17])<<4) ) & 0x07FF;
		channel.raw[13] = ( (static_cast<uint16_t>(payload[17])>>7) | (static_cast<uint16_t>(payload[18])<<1) | (static_cast<uint16_t>(payload[19])<<9 ) ) & 0x07FF;
		channel.raw[14] = ( (static_cast<uint16_t>(payload[19])>>2) | (static_cast<uint16_t>(payload[20])<<6) ) & 0x07FF;
		channel.raw[15] = ( (static_cast<uint16_t>(payload[20])>>5) | (static_cast<uint16_t>(payload[21])<<3) ) & 0x07FF;
		flags.u8 = payload[22];
		if(flags.bits.failsafe_active == 1)
		{
			rssi = 0;
			receiver_status = receiver_status_type::FAILSAFE_ENABLED;
			channel.failsafe_active = true;
		}
		else
		{
			channel.failsafe_active = false;
			if(flags.bits.frame_drop == 0)
			{
				rssi += 1;
				if(rssi > 100)
				{
					rssi = 100;
				}
			}
			else
			{
				rssi -= 5;
				if(rssi < 5)
				{
					rssi = 0;
				}
			}
		}
		new_data_available = false;
		if(isRawValuesInLimits()==true)
		{
			for(k=0;k<TOTAL_CHANNEL_NUMBER;k++)
			{
				channel.us[k] = static_cast<uint16_t>((static_cast<float>(channel.raw[k]) -192.0)*0.625 + 1000.0);
			}
			mapValuesToChannels();
		}
	}
	else
	{
		if(receiver_status != receiver_status_type::RECEIVER_NOT_PLUGGED_IN)
		{
			data_wait_counter++;
		}
	}

	if(data_wait_counter >= DATA_WAIT_LIMIT)
	{
		data_wait_counter = DATA_WAIT_LIMIT;
		receiver_status = receiver_status_type::RECEIVER_REMOVED;
		rssi = 0;
	}
}



/**
  * @brief 			Checks the raw values for packet integrity
  *
  * @param[in]	void
  *
  * @return 		bool result
  */
bool SBUS2_TELEMETRY::isRawValuesInLimits(void)
{
	bool result = true;
	uint8_t k = 0;

	for(k=0;k<TOTAL_CHANNEL_NUMBER;k++)
	{
	 if( (channel.raw[k] < CHANNEL_RAW_MIN_LIMIT) || (channel.raw[k] > CHANNEL_RAW_MAX_LIMIT) )
	 {
		 result = false;
		 out_of_range_packet_counter++;
		 break;
	 }
	}
	return result;
}



/**
  * @brief 			Maps the us values to the channels
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::mapValuesToChannels(void)
{
	const float STICK_MIN_VALUE = -1.0;
	const float STICK_MAX_VALUE =  1.0;
	const float STICK_DEADBAND  =  0.1;

	float temp_stick[4] = {0};
	uint8_t temp_switch[6] = {0};
	uint8_t k = 0;

	for(k=0;k<4;k++)
	{
		temp_stick[k] = (static_cast<float>(channel.us[k] - CHANNEL_MIN_US))/(static_cast<float>(CHANNEL_HALF_TRAVEL_US)) - 1.0;
		if(temp_stick[k] < STICK_MIN_VALUE)
		{
			temp_stick[k] = STICK_MIN_VALUE;
		}
		else if(temp_stick[k] > STICK_MAX_VALUE)
		{
			temp_stick[k] = STICK_MAX_VALUE;
		}
		//http://www.mimirgames.com/articles/games/joystick-input-and-using-deadbands/
		if(std::abs(temp_stick[k]) < STICK_DEADBAND)
		{
			temp_stick[k] = 0;
		}
		else
		{
			temp_stick[k] = (temp_stick[k] - (std::abs(temp_stick[k])/temp_stick[k])*STICK_DEADBAND)/(STICK_MAX_VALUE - STICK_DEADBAND);
		}
	}
	channel.aileron_roll 		= temp_stick[0];
	channel.elevator_pitch 	= temp_stick[1]*(-1.0);	//reversed on the transmitter
	channel.throttle 				= temp_stick[2];
	channel.rudder_yaw 			= temp_stick[3];

	for(k=0;k<6;k++)
	{
		if(channel.us[k+4]<(CHANNEL_MIN_US+(CHANNEL_HALF_TRAVEL_US/2)))
		{
			temp_switch[k] = 0;
		}
		else if(channel.us[k+4]>(CHANNEL_MAX_US-(CHANNEL_HALF_TRAVEL_US/2)))
		{
			temp_switch[k] = 2;
		}
		else
		{
			temp_switch[k] = 1;
		}
	}

	if(temp_switch[0] == 0)	//This block added for maintaining compability with Serce Transmitter Settings
	{
		channel.switch_G = 2;
	}
	else if(temp_switch[0] == 2)
	{
		channel.switch_G = 0;
	}
	else
	{
		channel.switch_G = 1;
	}
	channel.switch_E = temp_switch[1];
	channel.switch_A = temp_switch[2];
	channel.switch_B = temp_switch[3];
	channel.switch_C = temp_switch[4];
	channel.switch_D = temp_switch[5];
}



/**
  * @brief 			TEMP125 Temperature Sensor | Compatible also: F1713, SBS-01-TE
  *
  * @param[in]	uint8_t slot_number : Free Slot to be transmitted: 1 - 31
  * @param[in]	int16_t value 		  : Range: -16384 to 16383 degC | Resolution : 1 degC
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorTemp125(uint8_t slot_number, int16_t value)
{
	value |= 0x4000;
	writeToSlot(slot_number, static_cast<uint16_t>(value));
}



/**
  * @brief 			SBS-01-RMO RPM Sensor | Compatible also: SBS-01RB, SBS-01RM, SBS-01RO, SBS-01R
  *
  * @param[in]	uint8_t  slot_number : Free Slot to be transmitted: 1 - 31
  * @param[in]	uint32_t value 		   : Range: 0 to 196600 rpm | Resolution : 3 rpm
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorRpmSbs01Rmo(uint8_t slot_number, uint32_t value)
{
	uint16_t data = 0;

	value /= 3;
	if(value > 0xFFFF)
	{
		value = 0xFFFF;
	}
	data = static_cast<uint16_t>((value & 0x0000FF00) >> 8) | static_cast<uint16_t>((value & 0x000000FF) << 8);
	writeToSlot(slot_number, data);
}



/**
  * @brief 			SBS-01T Temperature Sensor
  *
  * @param[in]	uint8_t  slot_number : Free Slot to be transmitted: 1 - 31
  * @param[in]	int16_t value 			 : Range: -100 to 32667 degC | Resolution : 1 degC
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorTempSbs01T(uint8_t slot_number, int16_t value)
{
	uint16_t data = 0;

	value |= 0x8000;
	value += 100;

	data = static_cast<uint16_t>((value & 0xFF00) >> 8) | static_cast<uint16_t>((value & 0x00FF) << 8);
	writeToSlot(slot_number, data);
}



/**
  * @brief 			SBS-01V Voltage Sensor | Takes Up 2 Slots
  *
  * @param[in]	uint8_t slot_number 		: Free Slot to be transmitted: 1 - 6, 8 - 14, 16 - 22, 24 - 30
  * @param[in]	float	battery_voltage_v : Range: 0 to 819.2V | Resolution : 0.1V
  * @param[in]	float	external_voltage_v: Range: 0 to 819.2V | Resolution : 0.1V
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorVoltageSbs01Vslot2(uint8_t slot_number, float battery_voltage_v, float external_voltage_v)
{
	uint16_t voltage_1 = static_cast<uint16_t>(battery_voltage_v*10);
	uint16_t voltage_2 = static_cast<uint16_t>(external_voltage_v*10);

	voltage_1 |= 0x8000;
	if(voltage_1 > 0x9FFF)
	{
		voltage_1 = 0x9FFF;
	}
	else if(voltage_1 < 0x8000)
	{
		voltage_1 = 0x8000;
	}

	if(voltage_2 > 0x1FFF)
	{
		voltage_2 = 0x1FFF;
	}

	writeToSlot(slot_number,   voltage_1);
	writeToSlot(slot_number+1, voltage_2);
}



/**
  * @brief 			CURR-1678 Current Sensor | Takes Up 3 Slots | Compatible also: F1678
  *
  * @param[in]	uint8_t  slot_number : Free Slot to be transmitted: 1 - 5, 8 - 13, 16 - 21, 24 - 29
  * @param[in]	float 	 current_a	 : Range: 0 to 163A 		| Resolution : 0.1A
  * @param[in]	float 	 current_a	 : Range: 0 to 655V 		| Resolution : 0.1V
  * @param[in]	uint16_t capacity_mah: Range: 0 to 32767mAh | Resolution : 1mAh
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorCurr1678slot3(uint8_t slot_number, float current_a, float voltage_v, uint16_t capacity_mah)
{
	uint16_t current = static_cast<uint16_t>(current_a*100);
	uint16_t voltage = static_cast<uint16_t>(voltage_v*100);


	if(current > 0x3FFF)
	{
		current = 0x3FFF;
	}
	current |= 0x4000;
	current &= 0x7FFF;

	writeToSlot(slot_number,   current);
	writeToSlot(slot_number+1, voltage);
	writeToSlot(slot_number+2, capacity_mah);
}



/**
  * @brief 			VARIO-1712 Variometer Sensor | Takes Up 2 Slots | Altitude Values Are Relative To Initial Value
  *
  * @param[in]	uint8_t  slot_number : Free Slot to be transmitted: 1 - 6, 8 - 14, 16 - 22, 24 - 30
  * @param[in]	int16_t  altitude_m	 : Range: -16384 to 16383 m 		| Resolution : 1 m
  * @param[in]	float climb_rate_mps : Range: -3276.8 to 3276.8 m/s | Resolution : 1 m/s
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorVario1712slot2(uint8_t slot_number, int16_t altitude_m, float climb_rate_mps)
{
	int16_t vertical_speed = static_cast<int16_t>(climb_rate_mps*10);

	altitude_m |= 0x4000;

	writeToSlot(slot_number,   static_cast<uint16_t>(vertical_speed));
	writeToSlot(slot_number+1, static_cast<uint16_t>(altitude_m));
}



/**
  * @brief 			VARIO-1672 Variometer Sensor | Takes Up 2 Slots | Altitude Values Are Relative To Initial Value
  *
  * @param[in]	uint8_t  slot_number : Free Slot to be transmitted: 1 - 6, 8 - 14, 16 - 22, 24 - 30
  * @param[in]	int16_t  altitude_m	 : Range: -16384 to 16383 m 	| Resolution : 1 m
  * @param[in]	float climb_rate_mps : Range: -327.6 to 327.6 m/s | Resolution : 1 m/s
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorVario1672slot2(uint8_t slot_number, int16_t altitude_m, float climb_rate_mps)	 //-16384m - 16383m | -327.6m/s - 327.6m/s  	//takes up 2 slots	//Altitude values are relative
{
	int16_t vertical_speed = static_cast<int16_t>(climb_rate_mps*100);

	altitude_m |= 0x4000;

	writeToSlot(slot_number,   static_cast<uint16_t>(vertical_speed));
	writeToSlot(slot_number+1, static_cast<uint16_t>(altitude_m));
}



/**
  * @brief 			GPS-1675 GPS Sensor | Takes Up 8 Slots | Altitude Values Are Relative To Initial Value
  *
  * @param[in]	uint8_t  slot_number 	: Free Slot to be transmitted: 8, 16, 24
  * @param[in]	float ground_speed_mps: Range: 0 to 277.5 m/s 			| Resolution : 0.3 m/s
  * @param[in]	int16_t altitude_m		: Range: -16384 to 16383m 		| Resolution : 1 m
  * @param[in]	float climb_rate_mps	: Range: -3276.8 to 3276.8m/s | Resolution : 1 m/s
  * @param[in]	int32_t latitude			: Range: -90e+7 to 90e+7 			| Resolution : e-7
  * @param[in]	int32_t longitude			: Range: -180e+7 to 180e+7 		| Resolution : e-7
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sensorGps1675slot8(uint8_t slot_number, float ground_speed_mps, int16_t altitude_m, float climb_rate_mps, int32_t latitude, int32_t longitude)
{
	uint16_t ground_speed_kmph = static_cast<uint16_t>(ground_speed_mps*3.6);
	int16_t  climb_rate = static_cast<int16_t>(climb_rate_mps*10);
	uint16_t time_sec	= 43200;
	uint32_t latitude_1 = 0;
	uint32_t longitude_1 = 0;


	ground_speed_kmph |= 0x4000;
	if(ground_speed_kmph > 0x43E7)
	{
		ground_speed_kmph = 0x43E7;
	}
	else if(ground_speed_kmph < 0x4000)
	{
		ground_speed_kmph = 0x4000;
	}


	altitude_m |= 0x4000;
	if(altitude_m > 0x7FFF)
	{
		altitude_m = 0x7FFF;
	}


	if(latitude <= 0)
	{
		latitude *= -1;
		latitude_1  |= static_cast<uint32_t>(0x10) << 16;
	}
	latitude_1 |= static_cast<uint32_t>(latitude/10000000) << 24;
	latitude_1 |= static_cast<uint32_t>(static_cast<double>(static_cast<uint32_t>(latitude%10000000))*0.06);


	if(longitude <= 0)
	{
		longitude *= -1;
		longitude_1  |= static_cast<uint32_t>(0x10) << 16;
	}
	longitude_1 |= static_cast<uint32_t>(longitude/10000000) << 24;
	longitude_1 |= static_cast<uint32_t>(static_cast<double>(static_cast<uint32_t>(longitude%10000000))*0.06);


	writeToSlot(slot_number  , ground_speed_kmph);
	writeToSlot(slot_number+1, static_cast<uint16_t>(altitude_m));
	writeToSlot(slot_number+2, time_sec);
	writeToSlot(slot_number+3, static_cast<uint16_t>(climb_rate));
	writeToSlot(slot_number+4, static_cast<uint16_t>((latitude_1 & 0xFFFF0000) >> 16));
	writeToSlot(slot_number+5, static_cast<uint16_t>((latitude_1 & 0x0000FFFF) ));
	writeToSlot(slot_number+6, static_cast<uint16_t>((longitude_1 & 0xFFFF0000) >> 16));
	writeToSlot(slot_number+7, static_cast<uint16_t>((longitude_1 & 0x0000FFFF) ));
}



/**
  * @brief 			Fill the slots with telemetry data
  *
  * @param[in]	uint8_t  slot_number : Slot to be transmitted: 1 - 31
  * @param[in]	uint16_t data				 : Data to be transmitted
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::writeToSlot(uint8_t slot_number, uint16_t data)
{
	if(slot.in_use[slot_number] == false)
	{
		slot.in_use[slot_number] = true;
	}
	slot.data[slot_number][0] = slot_id[slot_number];
	slot.data[slot_number][1] = static_cast<uint8_t>((data & 0xFF00) >> 8);
	slot.data[slot_number][2] = static_cast<uint8_t>( data & 0x00FF);
}



/**
  * @brief 			Sends data over uart
  *
  * @param[in]	uint8_t buffer[]
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::sendData(uint8_t buffer[])
{
	uart8.sendData(buffer, SLOT_LENGHT);
}



/**
  * @brief 			Enable timer for telemetry time frame
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::enableTimer(void)
{
	TIM13->ARR = FRAME_DELAY_US - 1;
	HAL_TIM_Base_Start_IT(&htim13);
	timer_enabled = true;
}



/**
  * @brief 			Change timer value
  *
  * @param[in]	uint16_t delay_time_us
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::changeTimer(uint16_t delay_time_us)
{
	TIM13->ARR = delay_time_us - 1;
}



/**
  * @brief 			End telemetry time frame
  *
  * @param[in]	void
  *
  * @return 		void
  */
void SBUS2_TELEMETRY::disableTimer(void)
{
	HAL_TIM_Base_Stop_IT(&htim13);
	timer_enabled = false;
}



/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
SBUS2_TELEMETRY::SBUS2_TELEMETRY(const SBUS2_TELEMETRY& orig)
{

}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
SBUS2_TELEMETRY::~SBUS2_TELEMETRY()
{

}



} //End of namespace Futaba

} //End of namespace Receiver

