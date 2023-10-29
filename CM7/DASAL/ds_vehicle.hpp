/**
 ******************************************************************************
 * @file		: ds_vehicle.hpp
 * @brief		: Vehicle Interface Class
 *				  This file contains can interface class
 * @author		: Gurtac Kadem
 * @date		: 01.04.2020
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

#ifndef DS_VEHICLE_HPP
#define DS_VEHICLE_HPP

#include "ds_mode.hpp"

class VEHICLE
{
	public:
		VEHICLE();
		VEHICLE(const VEHICLE &orig);
		virtual ~VEHICLE();

		typedef union
		{
				struct
				{
						uint32_t rc_manual :1; //1
						uint32_t rc_auto :1; //2
						uint32_t rc_failsafe :1; //3
						uint32_t gcs_arm :1; //4
						uint32_t gcs_disarm :1; //5
						uint32_t auto_disarm :1; //6
						uint32_t waypoint :1; //7
						uint32_t poi :1; //8
						uint32_t escape :1; //9
						uint32_t increase_altitude :1; //10
						uint32_t decrease_altitude :1; //11
						uint32_t return_to_land :1; //12
						uint32_t gcs_communication_lost :1; //13
						uint32_t low_battery :1; //14
						uint32_t critical_battery :1; //15
						uint32_t gcs_brake :1; //16
						uint32_t auto_brake :1; //17
						uint32_t continue_t :1; //18
						uint32_t cancel :1; //19
						uint32_t land :1; //20
						uint32_t gnss_denied :1; //21
						uint32_t gnss_denied_land :1; //22
						uint32_t error :1; //23
						uint32_t precision :1; //24
						uint32_t take_off :1; //25
						uint32_t mode_completed :1; //26
						uint32_t unused_27 :1; //27
						uint32_t unused_28 :1; //28
						uint32_t unused_29 :1; //29
						uint32_t unused_30 :1; //30
						uint32_t unused_31 :1; //31
						uint32_t unused_32 :1; //32
				} bits;
				uint32_t u32;
		} autonomy_input_t;

		friend class MODE;
		friend class DISARM;
		friend class MANUAL;
		friend class READY_TO_TAKE_OFF;
		friend class TAKE_OFF;
		friend class USER_INPUT;
		friend class POSITION_HOLD;
		friend class WAYPOINT;
		friend class BRAKE;
		friend class PRECISION;
		friend class RETURN_TO_LAND;
		friend class EMERGENCY_LAND;
		friend class GNSS_DENIED;
		friend class GNSS_DENIED_LAND;
		friend class RC_GPS;
		friend class ESCAPE;

		VEHICLE* getSingleton(void);
		void scheduler(void);
		void modeProcessRequest(void);
		void modeStateMachine(void);

		void setMode(MODE::number_t new_mode_number, MODE::reason_t mode_reason);
		MODE* getMode(void);
		MODE::number_t getModeNumber(void);
		MODE::reason_t getModeReason(void);

	protected:
		void disarmStateMachine(void);
		void manualStateMachine(void);
		void readyToTakeOffStateMachine(void);
		void takeOffStateMachine(void);
		void userInputStateMachine(void);
		void positionHoldStateMachine(void);
		void waypointStateMachine(void);
		void brakeStateMachine(void);
		void precisionStateMachine(void);
		void returnToLandStateMachine(void);
		void emergencyLandStateMachine(void);
		void gnssDeniedStateMachine(void);
		void gnssDeniedLandStateMachine(void);
		void rcGPSStateMachine(void);
		void escapeStateMachine(void);

		void gcsCommandEvent(void);

	private:
		autonomy_input_t input;
		static VEHICLE *singleton;

		MODE *flight_mode;
		MODE::number_t last_flight_mode_number;
		MODE::number_t next_flight_mode_number;

		DISARM mode_disarm;
		MANUAL mode_manual;
		READY_TO_TAKE_OFF mode_ready_to_take_off;
		TAKE_OFF mode_take_off;
		USER_INPUT mode_user_input;
		POSITION_HOLD mode_position_hold;
		WAYPOINT mode_waypoint;
		BRAKE mode_brake;
		PRECISION mode_precision;
		RETURN_TO_LAND mode_return_to_land;
		EMERGENCY_LAND mode_emergency_land;
		GNSS_DENIED mode_gnss_denied;
		GNSS_DENIED_LAND mode_gnss_denied_land;
		RC_GPS mode_rc_gps;
		ESCAPE mode_escape;

};

class DOGAN: public VEHICLE
{
	public:
		DOGAN();
		virtual ~DOGAN();
};

class ALBATROS: public VEHICLE
{
	public:
		ALBATROS();
		virtual ~ALBATROS();
};

#endif /* DS_VEHICLE_HPP */
