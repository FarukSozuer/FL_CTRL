/**
 ******************************************************************************
  * @file		: ds_sbus2.hpp
  * @brief	: Futaba SBUS & SBUS2 Telemetry Class
  *				  	This file contains SBUS communication driver
  *						https://github.com/BrushlessPower/SBUS2-Telemetry
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

#ifndef DS_SBUS2_HPP
#define	DS_SBUS2_HPP



/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



namespace Receiver
{

namespace Futaba
{


/*
 * Begin of Macro Definitions
 */
const uint8_t FRAME_START 						= 0x0F;
const uint8_t FRAME_END_NON_TELEMETRY = 0x00;
const uint8_t FRAME_END_SLOT_0_7			= 0x04;
const uint8_t FRAME_END_SLOT_8_15			= 0x14;
const uint8_t FRAME_END_SLOT_16_23		= 0x24;
const uint8_t FRAME_END_SLOT_24_31		= 0x34;
const uint8_t FRAME_LENGTH						=   24;
const uint8_t SLOT_LENGHT							=    3;
const uint8_t TOTAL_SLOT_NUMBER				=   32;
const uint8_t FRAME_SLOT_NUMBER				=    8;
const uint16_t FRAME_DELAY_US					= 2000;
const	uint16_t SLOT_DELAY_US					=  660;
const	uint8_t TOTAL_CHANNEL_NUMBER		=   16;
const uint16_t CHANNEL_RAW_MIN_LIMIT  =   48;
const uint16_t CHANNEL_RAW_MAX_LIMIT  = 2000;
const uint16_t CHANNEL_MIN_US					= 1110;
const uint16_t CHANNEL_MAX_US					= 1930;
const uint16_t CHANNEL_HALF_TRAVEL_US =  410;
// End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */


/**
  * @brief Received Frame Status
  */
enum class frame_status_type : uint8_t
{
	NOT_INITIALIZED				= 0,
	FRAME_NOT_RECOGNIZED	= 1,
	SBUS_NON_TELEMETRY		= 2,
	SBUS2_TELEMETRY				= 3,
};


/**
  * @brief Receiver Physical & Link Status
  */
enum class receiver_status_type : uint8_t
{
	RECEIVER_NOT_PLUGGED_IN	= 0,
	SBUS_ACTIVE  						= 1,
	FAILSAFE_ENABLED				= 2,
	RECEIVER_REMOVED				= 3,
};


/**
  * @brief Receive State Machine States
  */
enum class receive_handler_state_type : uint8_t
{
	HEADER	= 0,
	PAYLOAD	= 1,
	FOOTER	= 2,
};


/**
  * @brief Timer State Machine States
  */
enum class timer_handler_state_type : uint8_t
{
	SLOT_O		= 0,
	SLOT_1_7	= 1,
	SLOT_END	= 2,
};


/**
  * @brief Telemetry Slot Container Struct
  */
struct slot_type
{
	uint8_t data[TOTAL_SLOT_NUMBER][SLOT_LENGHT];
	bool		in_use[TOTAL_SLOT_NUMBER];

	slot_type():
	data(),
	in_use()
	{ }
};


/**
  * @brief SBUS Status Flags Union
  */
#pragma pack(1)
union sbus_flags_type
{
	struct
	{
		uint8_t channel_17			:1;	//0
		uint8_t channel_18			:1;	//1
		uint8_t frame_drop			:1;	//2
		uint8_t failsafe_active	:1;	//3
		uint8_t reserved				:1;	//4-7
	}bits;

	uint8_t u8;

	sbus_flags_type():
	u8(0)
	{ }
};
#pragma pack()


/**
  * @brief Received Channels Container Struct
  */
struct channel_type
{
	uint16_t raw[TOTAL_CHANNEL_NUMBER];
	uint16_t us[TOTAL_CHANNEL_NUMBER];

	float	aileron_roll;
	float elevator_pitch;
	float throttle;
	float rudder_yaw;

	uint8_t switch_G;
	uint8_t switch_E;
	uint8_t switch_A;
	uint8_t switch_B;
	uint8_t switch_C;
	uint8_t switch_D;

	bool failsafe_active;

	channel_type():
	raw(),
	us(),
	aileron_roll(0),
	elevator_pitch(0),
	throttle(0),
	rudder_yaw(0),
	switch_G(0),
	switch_E(0),
	switch_A(0),
	switch_B(0),
	switch_C(0),
	switch_D(0),
	failsafe_active(false)
	{ }
};

// End of Enum, Union and Struct Definitions



/*
 * Begin of SBUS2_TELEMETRY Class Definition
 */
class SBUS2_TELEMETRY
{
	public:
		SBUS2_TELEMETRY();
		
		channel_type channel;

		void initialize(void);
		void scheduler(void);

		void receiveInterruptHandler(uint8_t data);
		void timerInterruptHandler(void);

		SBUS2_TELEMETRY(const SBUS2_TELEMETRY& orig);
		virtual ~SBUS2_TELEMETRY();

	protected:

	private:
		bool			timer_enabled;
		bool			telemetry_initialized;
		bool			new_data_available;
		uint8_t		payload[FRAME_LENGTH];
		uint8_t		active_slot_multiplier;
		uint8_t 	rssi;
		uint16_t  packet_loss_counter;
		uint16_t  unprocessed_packet_counter;
		uint16_t 	out_of_range_packet_counter;
		frame_status_type	frame_status;
		receiver_status_type receiver_status;
		slot_type slot;

		void updateTelemetry(void);
		void parseReceivedData(void);
		bool isRawValuesInLimits(void);
		void mapValuesToChannels(void);

		void sensorTemp125(uint8_t slot_number, int16_t value);
		void sensorRpmSbs01Rmo(uint8_t slot_number, uint32_t value);
		void sensorTempSbs01T(uint8_t slot_number, int16_t value);
		void sensorVoltageSbs01Vslot2(uint8_t slot_number, float battery_voltage_v, float external_voltage_v);
		void sensorCurr1678slot3(uint8_t slot_number, float current_a, float voltage_v, uint16_t capacity_mah);
		void sensorVario1712slot2(uint8_t slot_number, int16_t altitude_m, float climb_rate_mps);
		void sensorVario1672slot2(uint8_t slot_number, int16_t altitude_m, float climb_rate_mps);
		void sensorGps1675slot8(uint8_t slot_number, float ground_speed_mps, int16_t altitude_m, float climb_rate_mps, int32_t latitude, int32_t longitude);

		void writeToSlot(uint8_t slot_number, uint16_t data);
		void sendData(uint8_t buffer[]);
		void enableTimer(void);
		void changeTimer(uint16_t delay_time_us);
		void disableTimer(void);
};
// End of of SBUS2_TELEMETRY Class Definition


} //End of namespace Futaba

} //End of namespace Receiver



/*
 * External Linkages
 */
extern Receiver::Futaba::SBUS2_TELEMETRY sbus;
// End of External Linkages


#endif	/* DS_SBUS2_HPP */

