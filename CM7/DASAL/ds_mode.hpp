/**
 ******************************************************************************
  * @file		: ds_mode.hpp
  * @brief		: Mode Interface Class
  *				  This file contains mode interface class
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


#ifndef DS_MODE_HPP
#define DS_MODE_HPP

#include <stdint.h>

class MODE
{
	public:
		MODE();
		virtual ~MODE();

		enum class number_t : uint8_t
		{
			DISARM = 0,
			MANUAL = 1,
			READY_TO_TAKE_OFF = 2,
			TAKE_OFF = 3,
			USER_INPUT = 4,
			POSITION_HOLD = 5,
			WAYPOINT = 6,
			BRAKE = 7,
			PRECISION = 8,
			RETURN_TO_LAND = 9,
			EMERGENCY_LAND = 10,
			GNSS_DENIED = 11,
			GNSS_DENIED_LAND = 12,
			RC_GPS = 13,
			ESCAPE = 14,
		};

		enum class reason_t : uint8_t
		{
			UNKNOWN = 0,
			AUTONOMY,
			RC_COMMAND,
			RC_FAILSAFE,
			GCS_COMMAND,
			MISSION_END,
			AVOIDANCE,
			CRASH_FAILSAFE,
			FAILSAFE,
			TEST,
		};

		bool slow_down_active = false;

		virtual void init(void) = 0;
		virtual void run(void) = 0;
		virtual void exitMode(void) = 0;
		virtual bool isDisarm(void) { return false; };
		virtual bool isReadyToTakeOff(void) { return false; };
		virtual bool isTakingOff(void) { return false; };
		virtual bool isPositionHold(void) { return false; };
		virtual bool isInFlight(void) { return false; };
		virtual bool isLanding(void) { return false; };
		virtual bool isManual(void) { return false; };
		virtual bool isWaypoint(void) { return false; };
		virtual bool isBrake(void) { return false; };
		virtual bool isGNSSDenied(void) { return false; };
		virtual bool isReturnToLand(void) { return false; };
		virtual bool isEscape(void) { return false; };
		virtual bool isUserInput(void) { return false; };
		virtual bool isModeCompleted() const;

		virtual void landEvent(void);
		void setModeReason(reason_t modeReason);
		number_t getModeNumber(void) const;
		reason_t getModeReason(void) const;

	protected:
		number_t mode_number = number_t::DISARM;
		reason_t mode_reason = reason_t::UNKNOWN;
		bool mode_completed = false;
		bool mode_started = false;
		bool coordinate_target_reached = false;

		void yawCommandEvent(void);
		void altitudeCommandEvent(void);
		bool checkHeadingReference(void);
		void gotoCoordinate(int32_t requested_latitude, int32_t requested_longitude);

};

class DISARM: public MODE
{
	public:
		DISARM();
		virtual ~DISARM();

		void init(void) override;
		void run(void) override;
		void exitMode(void) override;
		bool isDisarm(void) override { return true; };
};

class MANUAL: public MODE
{
	public:
		MANUAL();
		virtual ~MANUAL();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isManual(void) override { return true; };
};

class READY_TO_TAKE_OFF: public MODE
{
	public:
		READY_TO_TAKE_OFF();
		virtual ~READY_TO_TAKE_OFF();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isReadyToTakeOff(void) override { return true; };
};

class TAKE_OFF: public MODE
{
	public:
		TAKE_OFF();
		virtual ~TAKE_OFF();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isTakingOff(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class USER_INPUT: public MODE
{
	public:
		USER_INPUT();
		virtual ~USER_INPUT();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isUserInput(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class POSITION_HOLD: public MODE
{
	public:
		POSITION_HOLD();
		virtual ~POSITION_HOLD();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isPositionHold(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class WAYPOINT: public MODE
{
	public:
		WAYPOINT();
		virtual ~WAYPOINT();

		struct waypoint_t
		{
		    int32_t  latitude;
		    int32_t  longitude;
		    float    altitude_m;
		};

		enum class waypoint_state_t: uint8_t
		{
		    INCREASE_ALTITUDE,
		    WAIT_FOR_HEADING_CHANGE,
		    APPLY_COORDINATE,
		    DECREASE_ALTITUDE,
		    CHANGE_WAYPOINT,
		};

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isWaypoint(void) override { return true; };
		bool isInFlight(void) override { return true; };

	private:
		waypoint_state_t state_waypoint = waypoint_state_t::INCREASE_ALTITUDE;
		uint16_t waypoint_number = 0;
		uint16_t waypoint_index  = 0;
		waypoint_t waypoint_data[128] = {0};

		const float MAX_HORIZONTAL_DISTANCE = 40.0;
		const float MAX_ALTITUDE_DISTANCE = 3.0;

		void increaseAltitudeEvent(void);
		void waitForHeadingEvent(void);
		void applyCoordinateEvent(void);
		void decreaseAltitudeEvent(void);
		void changeWaypointEvent(void);
};

class BRAKE: public MODE
{
	public:
		BRAKE();
		virtual ~BRAKE();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isBrake(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class PRECISION: public MODE
{
	public:
		PRECISION();
		virtual ~PRECISION();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isInFlight(void) override { return true; };
};

class RETURN_TO_LAND: public MODE
{
	public:
		RETURN_TO_LAND();
		virtual ~RETURN_TO_LAND();

		enum class return_to_land_state_t: uint8_t
		{
		    CLIMB,
		    WAIT_FOR_HEADING_CHANGE,
		    APPLY_COORDINATE,
		    WAIT_FOR_HOME_HEADING_CHANGE,
		    LAND,
		};

		void init() override;
		void run() override;
		void exitMode() override;
		bool isReturnToLand(void) override { return true; };
		bool isInFlight(void) override { return true; };
		void landEvent(void) override;

	private:
		return_to_land_state_t rtl_state = return_to_land_state_t::CLIMB;
		float lat_dist = 0.0;
		float lon_dist = 0.0;

		const float RTL_RADIUS_M = 10.0 ;
		const float SPEED_REDUCTION_ALT_DIFF	= 15.0;
		const float SPEED_REDUCTION_DIVIDE_CONSTANT = -6.0;
		const float SPEED_REDUCTION_CONSTANT = 0.6;

		void climbEvent(void);
		void waitForHeadingEvent(void);
		void applyCoordinateEvent(void);
		void waitForHomeHeadingChangeEvent(void);
};

class EMERGENCY_LAND: public MODE
{
	public:
		EMERGENCY_LAND();
		virtual ~EMERGENCY_LAND();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isLanding(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class GNSS_DENIED: public MODE
{
	public:
		GNSS_DENIED();
		virtual ~GNSS_DENIED();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isGNSSDenied(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class GNSS_DENIED_LAND: public MODE
{
	public:
		GNSS_DENIED_LAND();
		virtual ~GNSS_DENIED_LAND();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isGNSSDenied(void) override { return true; };
		bool isLanding(void) override { return true; };
		bool isInFlight(void) override { return true; };
};

class RC_GPS: public MODE
{
	public:
		RC_GPS();
		virtual ~RC_GPS();

		void init() override;
		void run() override;
		void exitMode(void) override;
};

class ESCAPE: public MODE
{
	public:
		ESCAPE();
		virtual ~ESCAPE();

		void init() override;
		void run() override;
		void exitMode(void) override;
		bool isEscape(void) override { return true; };
		bool isInFlight(void) override { return true; };

	private:
		const float ESCAPE_ALTITUDE_M = 100.0;
};


#endif /* DS_MODE_HPP */
