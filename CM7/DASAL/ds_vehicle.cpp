/**
 ******************************************************************************
 * @file		: ds_vehicle.cpp
 * @brief		: Vehicle Interface Class
 *				  This file contains vehicle interface class
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

#include <ds_vehicle.hpp>
#include "ds_parameters.hpp"

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
VEHICLE::VEHICLE()
{
	if (singleton != nullptr)
	{
		//error
	}
	else
	{
		singleton = this;
		input.u32 = 0;
		flight_mode = &mode_disarm;
	}
}

/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
VEHICLE::VEHICLE(const VEHICLE &orig)
{
}

/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
VEHICLE::~VEHICLE()
{
}

VEHICLE *VEHICLE::singleton = nullptr;

/**
  * @brief Setting mode
  *
  * @param[in]  MODE::number_t new_mode_number
  * @param[in]  MODE::reason_t mode_reason
  *
  * @return 	void
  */
void VEHICLE::setMode(MODE::number_t new_mode_number,
		MODE::reason_t mode_reason)
{
	MODE *new_mode = nullptr;

	switch (new_mode_number)
	{
		case MODE::number_t::DISARM:
			new_mode = &mode_disarm;
			break;

		case MODE::number_t::MANUAL:
			new_mode = &mode_manual;
			break;

		case MODE::number_t::READY_TO_TAKE_OFF:
			new_mode = &mode_ready_to_take_off;
			break;

		case MODE::number_t::TAKE_OFF:
			new_mode = &mode_take_off;
			break;

		case MODE::number_t::USER_INPUT:
			new_mode = &mode_user_input;
			break;

		case MODE::number_t::POSITION_HOLD:
			new_mode = &mode_position_hold;
			break;

		case MODE::number_t::WAYPOINT:
			new_mode = &mode_waypoint;
			break;

		case MODE::number_t::BRAKE:
			new_mode = &mode_brake;
			break;

		case MODE::number_t::PRECISION:
			new_mode = &mode_precision;
			break;

		case MODE::number_t::RETURN_TO_LAND:
			new_mode = &mode_return_to_land;
			break;

		case MODE::number_t::EMERGENCY_LAND:
			new_mode = &mode_emergency_land;
			break;

		case MODE::number_t::GNSS_DENIED:
			new_mode = &mode_gnss_denied;
			break;

		case MODE::number_t::GNSS_DENIED_LAND:
			new_mode = &mode_gnss_denied_land;
			break;

		case MODE::number_t::RC_GPS:
			new_mode = &mode_rc_gps;
			break;

		case MODE::number_t::ESCAPE:
			new_mode = &mode_escape;
			break;

		default:
			break;

	}

	flight_mode->exitMode();
	flight_mode = new_mode;
	flight_mode->setModeReason(mode_reason);
	flight_mode->init();
}

/**
  * @brief Getting mode
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE* VEHICLE::getMode(void)
{
	return flight_mode;
}

/**
  * @brief Getting mode number
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE::number_t VEHICLE::getModeNumber(void)
{
	return flight_mode->getModeNumber();
}

/**
  * @brief Getting reason of mode change
  *
  * @param[in]  void
  *
  * @return 	void
  */
MODE::reason_t VEHICLE::getModeReason(void)
{
	return flight_mode->getModeReason();
}

/**
  * @brief Getting singleton
  *
  * @param[in]  void
  *
  * @return 	void
  */
VEHICLE* VEHICLE::getSingleton(void)
{
	return singleton;
}

/**
  * @brief GCS command event handler
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::gcsCommandEvent(void)
{
	if (flight_mode->isGNSSDenied())
	{
		input.bits.gnss_denied_land = parameters.gcs_command.bits.land;
	}

	input.bits.gcs_arm = parameters.gcs_command.bits.arm;
	input.bits.gcs_disarm = parameters.gcs_command.bits.disarm;
	input.bits.increase_altitude = parameters.gcs_command.bits.increase_altitude;
	input.bits.decrease_altitude = parameters.gcs_command.bits.decrease_altitude;
	input.bits.escape = parameters.gcs_command.bits.escape;
	input.bits.cancel = parameters.gcs_command.bits.cancel;
	input.bits.continue_t = parameters.gcs_command.bits.continue_t;
	input.bits.waypoint = parameters.gcs_command.bits.waypoint;
	input.bits.take_off = parameters.gcs_command.bits.take_off;
	input.bits.return_to_land = parameters.gcs_command.bits.return_to_land;
	input.bits.land = parameters.gcs_command.bits.land;
	input.bits.gcs_brake = parameters.gcs_command.bits.brake;
}

/**
  * @brief Process request
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::modeProcessRequest(void)
{
	if(flight_mode->isLanding())
	{
		if(parameters.altitude < parameters.SLOW_DOWN_LIMIT)
		{
			flight_mode->slow_down_active = true;
		}
		else
		{
			flight_mode->slow_down_active = false;
		}
	}

	if (flight_mode->isTakingOff() || flight_mode->isBrake()
			|| flight_mode->isEscape() || flight_mode->isUserInput())
	{
		input.bits.mode_completed = flight_mode->isModeCompleted();
	}
	else if (flight_mode->isLanding())
	{
		input.bits.auto_disarm = flight_mode->isModeCompleted();
	}

	gcsCommandEvent();
}

/**
  * @brief State machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::modeStateMachine(void)
{
	switch (flight_mode->getModeNumber())
	{
		case MODE::number_t::DISARM:
			disarmStateMachine();
			break;

		case MODE::number_t::MANUAL:
			manualStateMachine();
			break;

		case MODE::number_t::READY_TO_TAKE_OFF:
			readyToTakeOffStateMachine();
			break;

		case MODE::number_t::TAKE_OFF:
			takeOffStateMachine();
			break;

		case MODE::number_t::USER_INPUT:
			userInputStateMachine();
			break;

		case MODE::number_t::POSITION_HOLD:
			positionHoldStateMachine();
			break;

		case MODE::number_t::WAYPOINT:
			waypointStateMachine();
			break;

		case MODE::number_t::BRAKE:
			brakeStateMachine();
			break;

		case MODE::number_t::PRECISION:
			precisionStateMachine();
			break;

		case MODE::number_t::RETURN_TO_LAND:
			returnToLandStateMachine();
			break;

		case MODE::number_t::EMERGENCY_LAND:
			emergencyLandStateMachine();
			break;

		case MODE::number_t::GNSS_DENIED:
			gnssDeniedStateMachine();
			break;

		case MODE::number_t::GNSS_DENIED_LAND:
			gnssDeniedLandStateMachine();
			break;

		case MODE::number_t::RC_GPS:
			rcGPSStateMachine();
			break;

		case MODE::number_t::ESCAPE:
			escapeStateMachine();
			break;

		default:
			break;
	}

	input.u32 = 0;
}

void VEHICLE::disarmStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.gcs_arm)
	{
		setMode(MODE::number_t::READY_TO_TAKE_OFF, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Manual mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::manualStateMachine(void)
{
	if (input.bits.rc_auto)
	{
		setMode(MODE::number_t::BRAKE, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.rc_failsafe)
	{
		setMode(MODE::number_t::BRAKE, MODE::reason_t::RC_FAILSAFE);
	}
}

/**
  * @brief Ready to takeoff mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::readyToTakeOffStateMachine(void)
{
	if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.error)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.take_off)
	{
		setMode(MODE::number_t::TAKE_OFF, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Takeoff mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::takeOffStateMachine(void)
{
	last_flight_mode_number = flight_mode->getModeNumber();
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.mode_completed)
	{
		if (input.bits.waypoint)
		{
			setMode(MODE::number_t::WAYPOINT, MODE::reason_t::AUTONOMY);
		}
		else
		{
			setMode(MODE::number_t::POSITION_HOLD, MODE::reason_t::AUTONOMY);
		}
	}
	else if (input.bits.critical_battery)
	{
		setMode(MODE::number_t::EMERGENCY_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		setMode(MODE::number_t::EMERGENCY_LAND, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.cancel || input.bits.return_to_land)
	{
		setMode(MODE::number_t::RETURN_TO_LAND, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gcs_communication_lost || input.bits.low_battery)
	{
		setMode(MODE::number_t::RETURN_TO_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.auto_brake)
	{
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake)
	{
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied_land)
	{
		setMode(MODE::number_t::GNSS_DENIED_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gnss_denied)
	{
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.auto_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::AUTONOMY);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief User input mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::userInputStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.mode_completed)
	{
		setMode(MODE::number_t::POSITION_HOLD, MODE::reason_t::AUTONOMY);
	}
	else if (input.bits.critical_battery)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gcs_communication_lost || input.bits.low_battery)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.return_to_land)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.waypoint)
	{
		next_flight_mode_number = MODE::number_t::WAYPOINT;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.escape)
	{
		next_flight_mode_number = MODE::number_t::ESCAPE;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Position hold mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::positionHoldStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.critical_battery)
	{
		setMode(MODE::number_t::EMERGENCY_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		setMode(MODE::number_t::EMERGENCY_LAND, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.continue_t)
	{
		if (last_flight_mode_number == MODE::number_t::TAKE_OFF)
		{
			setMode(MODE::number_t::TAKE_OFF, MODE::reason_t::GCS_COMMAND);
		}
		else if (last_flight_mode_number == MODE::number_t::RETURN_TO_LAND)
		{
			setMode(MODE::number_t::RETURN_TO_LAND, MODE::reason_t::GCS_COMMAND);
		}
		else if (last_flight_mode_number == MODE::number_t::WAYPOINT)
		{
			setMode(MODE::number_t::WAYPOINT, MODE::reason_t::GCS_COMMAND);
		}
	}
	else if (input.bits.gcs_communication_lost || input.bits.low_battery)
	{
		setMode(MODE::number_t::RETURN_TO_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.return_to_land)
	{
		setMode(MODE::number_t::RETURN_TO_LAND, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.waypoint)
	{
		setMode(MODE::number_t::WAYPOINT, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.escape)
	{
		setMode(MODE::number_t::ESCAPE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.precision)
	{
		setMode(MODE::number_t::PRECISION, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.increase_altitude || input.bits.decrease_altitude)
	{
		setMode(MODE::number_t::USER_INPUT, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Waypoint mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::waypointStateMachine(void)
{
	last_flight_mode_number = flight_mode->getModeNumber();
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.critical_battery)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake || !input.bits.waypoint)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gcs_communication_lost || input.bits.low_battery)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.return_to_land)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.escape)
	{
		next_flight_mode_number = MODE::number_t::ESCAPE;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied)
	{
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Brake mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::brakeStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.mode_completed)
	{
		if (next_flight_mode_number == MODE::number_t::EMERGENCY_LAND)
		{
			setMode(MODE::number_t::EMERGENCY_LAND, flight_mode->getModeReason());
		}
		else if (next_flight_mode_number == MODE::number_t::RETURN_TO_LAND)
		{
			setMode(MODE::number_t::RETURN_TO_LAND, flight_mode->getModeReason());
		}
		else if (next_flight_mode_number == MODE::number_t::WAYPOINT)
		{
			setMode(MODE::number_t::WAYPOINT, flight_mode->getModeReason());
		}
		else if (next_flight_mode_number == MODE::number_t::ESCAPE)
		{
			setMode(MODE::number_t::ESCAPE, flight_mode->getModeReason());
		}
		else if (next_flight_mode_number == MODE::number_t::PRECISION)
		{
			setMode(MODE::number_t::PRECISION, flight_mode->getModeReason());
		}
		else
		{
			setMode(MODE::number_t::POSITION_HOLD, MODE::reason_t::AUTONOMY);
		}
	}
	else if (input.bits.critical_battery)
	{
		setMode(MODE::number_t::EMERGENCY_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		setMode(MODE::number_t::EMERGENCY_LAND, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.increase_altitude || input.bits.decrease_altitude)
	{
		setMode(MODE::number_t::USER_INPUT, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Precision mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::precisionStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.critical_battery)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake || input.bits.cancel)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gcs_communication_lost || input.bits.low_battery)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.return_to_land)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.escape)
	{
		next_flight_mode_number = MODE::number_t::ESCAPE;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.increase_altitude || input.bits.decrease_altitude)
	{
		setMode(MODE::number_t::USER_INPUT, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Return to land mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::returnToLandStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.critical_battery)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.cancel)
	{
		if (last_flight_mode_number == MODE::number_t::TAKE_OFF)
		{
			setMode(MODE::number_t::TAKE_OFF, MODE::reason_t::GCS_COMMAND);
		}
		else if (input.bits.waypoint)
		{
			next_flight_mode_number = MODE::number_t::WAYPOINT;
			setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
		}
		else
		{
			next_flight_mode_number = MODE::number_t::POSITION_HOLD;
			setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
		}
	}
	else if (input.bits.waypoint)
	{
		next_flight_mode_number = MODE::number_t::WAYPOINT;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.escape)
	{
		next_flight_mode_number = MODE::number_t::ESCAPE;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied_land)
	{
		last_flight_mode_number = flight_mode->getModeNumber();
		setMode(MODE::number_t::GNSS_DENIED_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.auto_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::AUTONOMY);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Emergency land mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::emergencyLandStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.cancel)
	{
		if (last_flight_mode_number == MODE::number_t::TAKE_OFF)
		{
			setMode(MODE::number_t::TAKE_OFF, MODE::reason_t::GCS_COMMAND);
		}
		else
		{
			next_flight_mode_number = MODE::number_t::POSITION_HOLD;
			setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
		}
	}
	else if (input.bits.escape)
	{
		next_flight_mode_number = MODE::number_t::ESCAPE;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gnss_denied_land)
	{
		last_flight_mode_number = flight_mode->getModeNumber();
		setMode(MODE::number_t::GNSS_DENIED_LAND, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = flight_mode->getModeNumber();
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.auto_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::AUTONOMY);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Gnns denied mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::gnssDeniedStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (!input.bits.gnss_denied)
	{
		if (last_flight_mode_number == MODE::number_t::RETURN_TO_LAND)
		{
			next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		}
		else
		{
			next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		}
		setMode(MODE::number_t::BRAKE, MODE::reason_t::AUTONOMY);
	}
	else
	{
		if (input.bits.critical_battery)
		{
			setMode(MODE::number_t::GNSS_DENIED_LAND, MODE::reason_t::FAILSAFE);
		}
		else if (input.bits.land)
		{
			setMode(MODE::number_t::GNSS_DENIED_LAND, MODE::reason_t::GCS_COMMAND);
		}
		else if (input.bits.gcs_disarm)
		{
			setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
		}
	}
}

/**
  * @brief Gnss denied land mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::gnssDeniedLandStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (!input.bits.gnss_denied)
	{
		if (last_flight_mode_number == MODE::number_t::RETURN_TO_LAND)
		{
			next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		}
		else
		{
			next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		}
		setMode(MODE::number_t::BRAKE, MODE::reason_t::AUTONOMY);
	}
	else
	{
		if (input.bits.auto_brake)
		{
			setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
		}
		else if (input.bits.gcs_brake || input.bits.cancel)
		{
			setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::GCS_COMMAND);
		}
		else if (input.bits.auto_disarm)
		{
			setMode(MODE::number_t::DISARM, MODE::reason_t::AUTONOMY);
		}
		else if (input.bits.gcs_disarm)
		{
			setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
		}
	}
}

/**
  * @brief RC GPS mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::rcGPSStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Escape mode state machine
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::escapeStateMachine(void)
{
	if (input.bits.rc_manual)
	{
		setMode(MODE::number_t::MANUAL, MODE::reason_t::RC_COMMAND);
	}
	else if (input.bits.critical_battery)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.land)
	{
		next_flight_mode_number = MODE::number_t::EMERGENCY_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.auto_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_brake)
	{
		next_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.gcs_communication_lost || input.bits.low_battery)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.return_to_land)
	{
		next_flight_mode_number = MODE::number_t::RETURN_TO_LAND;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.waypoint)
	{
		next_flight_mode_number = MODE::number_t::WAYPOINT;
		setMode(MODE::number_t::BRAKE, MODE::reason_t::GCS_COMMAND);
	}
	else if (input.bits.mode_completed)
	{
		setMode(MODE::number_t::POSITION_HOLD, MODE::reason_t::MISSION_END);
	}
	else if (input.bits.gnss_denied)
	{
		last_flight_mode_number = MODE::number_t::POSITION_HOLD;
		setMode(MODE::number_t::GNSS_DENIED, MODE::reason_t::FAILSAFE);
	}
	else if (input.bits.gcs_disarm)
	{
		setMode(MODE::number_t::DISARM, MODE::reason_t::GCS_COMMAND);
	}
}

/**
  * @brief Vehicle Scheduler
  *
  * @param[in]  void
  *
  * @return 	void
  */
void VEHICLE::scheduler(void)
{
	modeProcessRequest();
	modeStateMachine();

	flight_mode->run();

	parameters.gcs_command.u16 = 0;
}
