/**
 *****************************************************.*************************
  * @file		: ds_vn100.hpp
  * @brief		: VN100 Interface Class
  *				  This file contains vn100 interface class
  * @author		: Faruk Sozuer
  * @date		: 05.03.2020
  * @version	: 0.0.1
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

#ifndef DS_VN100_HPP
#define DS_VN100_HPP

#include <stdint.h>

#include "ds_uart_h747.hpp"


namespace Sensors
{
namespace Imu
{


/*
 * @brief
 */
union system_error_code_type
{
	struct
	{
	  uint32_t init_error				:1;	///< bit0: 	Sensor initialize error.
		uint32_t baudrate_error			:1;	///< bit1:  Baudratee Error.
		uint32_t init_time_out_error 	:1; ///< bit2:  Sensor is not in system.
		uint32_t init_open_serial_error :1; ///< bit3:
		uint32_t init_sync_error        :1; ///< bit4:
		uint32_t init_bitrate_error     :1; ///< bit5:
		uint32_t init_output_group_error:1; ///< bit6:
		uint32_t hard_fault				:1; ///< bit7:  If this error occurs, then the firmware on the VN-100 has experienced a hard fault exception.
		uint32_t serial_buffer_overflow :1; ///< bit8:  The processorâ€™s serial input buffer has experienced an overflow
		uint32_t invalid_checksum		:1; ///< bit9:  The checksum for the received command was invalid.
		uint32_t invalid_command		:1; ///< bit10: The user has requested an invalid command.
		uint32_t not_enough_parameters  :1; ///< bit11: The user did not supply the minimum number of required parameters for the requested command.
		uint32_t too_many_parameters	:1; ///< bit12: The user supplied too many parameters for the requested command.
		uint32_t invalid_parameter		:1; ///< bit13: The user supplied a parameter for the requested command which was invalid.
		uint32_t invalid_register		:1; ///< bit14: An invalid register was specified.
		uint32_t unauthorized_access	:1; ///< bit15: The user does not have permission to write to this register.
		uint32_t watchdog_reset			:1; ///< bit16: A watchdog reset has occurred.
		uint32_t output_buffer_overflow :1; ///< bit17: The output buffer has experienced an overflow.
		uint32_t insufficient_baud_rate :1; ///< bit18: The baud rate is not high enough to support the requested asynchronous data output at the requested data rate.
		uint32_t error_buffer_overflow  :1; ///< bit19: An overflow event has occurred on the system error buffer.
	}bits;
	uint32_t all;
};


/*
 * @brief The Group and Group Field parameters consist of variable length arguments to allow
 * conciseness where possible and expandability where necessary.
 */
union group_header_type
{
	struct
	{
		uint8_t common		:1;	///<bit0:   Where the value of the common bit containing Binary1 is stored.
		uint8_t time  		:1; ///<bit1:	Where the value of the common bit containing Binary2 is stored.
		uint8_t imu			:1; ///<bit2:	Where the value of the common bit containing Binary3 is stored.
		uint8_t attitude	:2; ///<bit3-4:	Where the value of the common bit containing Binary5 is stored.
	}bits;
	uint8_t all;
};


/*
 * @brief Binary group 1 contains a wide assortment of commonly used data required for most applications flags.
 */
union binary_group1_type
{
	struct
	{
		uint16_t time_startup :1; ///<bit0: 	The system time since startup measured in nano seconds flag.
		uint16_t reserved	  :1; ///<bit1:		Reserved. Not used on this product flag.
		uint16_t time_sync_in :1; ///<bit2:		The time since the last SyncIn trigger event expressed in nano seconds flag.
		uint16_t ypr		  :1; ///<bit3:   The estimated attitude Yaw, Pitch, and Roll angles measured in degrees flag.
		uint16_t queternion   :1; ///<bit4:   The estimated attitude quaternion. The last term is the scalar value flag.
		uint16_t angular_rate :1; ///<bit5:		The estimated angular rate measured in rad/s flag.
		uint16_t reserved1    :1; ///<bit6:		Reserved. Not used on this product flag.
		uint16_t reserved2    :1; ///<bit7:		Reserved. Not used on this product flag.
		uint16_t accel        :1; ///<bit8:		The estimated acceleration in the body frame, given in m/s^2 flag.
		uint16_t imu          :1; ///<bit9:		The uncompensated IMU acceleration and angular rate measurements flag.
		uint16_t magpres      :1; ///<bit10:	The compensated magnetic, temperature, and pressure measurements from the IMU flag.
		uint16_t delta_theta  :1; ///<bit11:	The delta time, angle, and velocity measurements flag.
		uint16_t vpe_status   :1; ///<bit12:	The VPE status bitfield flag.
		uint16_t sync_in_cnt  :1; ///<bit13:	The number of SyncIn trigger events that have occurred.
		uint16_t reserved3    :2; ///<bit14-15: Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};


/*
 * @brief Binary group 1 contains a wide assortment of commonly used data required for most applications flags.
 */
union common_group_msg_type
{
	struct
	{
		uint16_t time_startup :1; ///<bit0: 	The system time since startup measured in nano seconds flag.
		uint16_t reserved	  :1; ///<bit1:		Reserved. Not used on this product flag.
		uint16_t time_sync_in :1; ///<bit2:		The time since the last SyncIn trigger event expressed in nano seconds flag.
		uint16_t ypr		  :1; ///<bit3:   The estimated attitude Yaw, Pitch, and Roll angles measured in degrees flag.
		uint16_t queternion   :1; ///<bit4:   The estimated attitude quaternion. The last term is the scalar value flag.
		uint16_t angular_rate :1; ///<bit5:		The estimated angular rate measured in rad/s flag.
		uint16_t reserved1    :1; ///<bit6:		Reserved. Not used on this product flag.
		uint16_t reserved2    :1; ///<bit7:		Reserved. Not used on this product flag.
		uint16_t accel        :1; ///<bit8:		The estimated acceleration in the body frame, given in m/s^2 flag.
		uint16_t imu          :1; ///<bit9:		The uncompensated IMU acceleration and angular rate measurements flag.
		uint16_t magpres      :1; ///<bit10:	The compensated magnetic, temperature, and pressure measurements from the IMU flag.
		uint16_t delta_theta  :1; ///<bit11:	The delta time, angle, and velocity measurements flag.
		uint16_t vpe_status   :1; ///<bit12:	The VPE status bitfield flag.
		uint16_t sync_in_cnt  :1; ///<bit13:	The number of SyncIn trigger events that have occurred.
		uint16_t reserved3    :2; ///<bit14-15: Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};


/*
 * @brief Binary group 2 provides all timing and event counter related outputs flags.
 */
union binary_group2_type
{
	struct
	{
		uint16_t time_startup :1;  ///<bit0: 	   The system time since startup measured in nano seconds flag.
		uint16_t reserved1    :1;  ///<bit1: 	   Reserved. Not used on this product flag.
		uint16_t reserved2    :1;  ///<bit2: 	   Reserved. Not used on this product flag.
		uint16_t reserved3    :1;  ///<bit3: 	   Reserved. Not used on this product flag.
		uint16_t time_sync_in :1;  ///<bit4: 	   The time since the last SyncIn event trigger expressed in nano seconds flag.
		uint16_t reserved4    :1;  ///<bit5: 	 	 Reserved. Not used on this product flag.
		uint16_t reserved5    :1;  ///<bit6: 	   Reserved. Not used on this product flag.
		uint16_t sync_in_cnt  :1;  ///<bit7: 	   The number of SyncIn trigger events that have occurred flag.
		uint16_t sync_out_cnt :1;  ///<bit8: 	 	 The number of SyncOut trigger events that have occurred flag.
		uint16_t time_status  :1;  ///<bit9: 	 	 Nothing information.
		uint16_t reserved6    :6;  ///<bit10-15: Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};

union time_group_msg_type
{
	struct
	{
		uint16_t time_startup :1;  ///<bit0: 	   The system time since startup measured in nano seconds flag.
		uint16_t reserved1    :1;  ///<bit1: 	   Reserved. Not used on this product flag.
		uint16_t reserved2    :1;  ///<bit2: 	   Reserved. Not used on this product flag.
		uint16_t reserved3    :1;  ///<bit3: 	   Reserved. Not used on this product flag.
		uint16_t time_sync_in :1;  ///<bit4: 	   The time since the last SyncIn event trigger expressed in nano seconds flag.
		uint16_t reserved4    :1;  ///<bit5: 	 	 Reserved. Not used on this product flag.
		uint16_t reserved5    :1;  ///<bit6: 	   Reserved. Not used on this product flag.
		uint16_t sync_in_cnt  :1;  ///<bit7: 	   The number of SyncIn trigger events that have occurred flag.
		uint16_t sync_out_cnt :1;  ///<bit8: 	 	 The number of SyncOut trigger events that have occurred flag.
		uint16_t time_status  :1;  ///<bit9: 	 	 Nothing information.
		uint16_t reserved6    :6;  ///<bit10-15: Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};


/*
 * @brief Binary group 3 provides all outputs which are dependent upon the measurements collected
 * from the	onboard IMU, or an external IMU (if enabled).
 */
union binary_group3_type
{
	struct
	{
		uint16_t imu_status	  :1; ///<bit0:	  	Status is reserved for future use. Not currently used in the current code, as such will always report 0.
		uint16_t uncomp_mag   :1; ///<bit1:			The IMU magnetic field measured in units of Gauss, given in the body frame flag.
		uint16_t uncomp_accel :1; ///<bit2:			The IMU acceleration measured in units of m/s^2, given in the body frame flag.
		uint16_t uncomp_gyro  :1; ///<bit3:			The IMU angular rate measured in units of rad/s, given in the body frame flag.
		uint16_t temp		  		:1; ///<bit4:			The IMU temperature measured in units of Celsius flag.
		uint16_t pres 		  	:1; ///<bit5:			The IMU pressure measured in kilopascals flag.
		uint16_t delta_theta  :1; ///<bit6:			The delta time (dtime) is the time interval that the delta angle and velocities are integrated over flag.
		uint16_t delta_v      :1; ///<bit7:			The delta velocity (dvel) is the delta velocity incurred due to motion, since the last time the values were
															///<					output by the device flag.
		uint16_t mag      	  :1; ///<bit8:			The IMU compensated magnetic field measured units of Gauss, and given in the body frame flag.
		uint16_t accel        :1; ///<bit9:			The compensated acceleration measured in units of m/s^2, and given in the body frame flag.
		uint16_t angular_rate :1; ///<bit10:		The compensated angular rate measured in units of rad/s, and given in the body frame flag.
		uint16_t reserved     :5; ///<bit11-15:	Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};

/*
 * @brief Binary group 3 provides all outputs which are dependent upon the measurements collected
 * from the	onboard IMU, or an external IMU (if enabled).
 */
union imu_group_msg_type
{
	struct
	{
		uint16_t imu_status	  :1; ///<bit0:	  	Status is reserved for future use. Not currently used in the current code, as such will always report 0.
		uint16_t uncomp_mag   :1; ///<bit1:			The IMU magnetic field measured in units of Gauss, given in the body frame flag.
		uint16_t uncomp_accel :1; ///<bit2:			The IMU acceleration measured in units of m/s^2, given in the body frame flag.
		uint16_t uncomp_gyro  :1; ///<bit3:			The IMU angular rate measured in units of rad/s, given in the body frame flag.
		uint16_t temp		  		:1; ///<bit4:			The IMU temperature measured in units of Celsius flag.
		uint16_t pres 		  	:1; ///<bit5:			The IMU pressure measured in kilopascals flag.
		uint16_t delta_theta  :1; ///<bit6:			The delta time (dtime) is the time interval that the delta angle and velocities are integrated over flag.
		uint16_t delta_v      :1; ///<bit7:			The delta velocity (dvel) is the delta velocity incurred due to motion, since the last time the values were
															///<					output by the device flag.
		uint16_t mag      	  :1; ///<bit8:			The IMU compensated magnetic field measured units of Gauss, and given in the body frame flag.
		uint16_t accel        :1; ///<bit9:			The compensated acceleration measured in units of m/s^2, and given in the body frame flag.
		uint16_t angular_rate :1; ///<bit10:		The compensated angular rate measured in units of rad/s, and given in the body frame flag.
		uint16_t sensStat     :5; ///<bit11-15:	Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};


/*
 * @brief Binary group 5 provides all estimated outputs which are dependent upon the estimated attitude solution.
 */
union binary_group5_type
{
	struct
	{
		uint16_t vpe_status    			:1; ///<bit0:	The VPE status bitfield.
		uint16_t ypr		   					:1; ///<bit1:	The estimated attitude Yaw, Pitch, and Roll angles measured in degrees flag.
		uint16_t qtn   		   				:1; ///<bit2:	The estimated attitude quaternion. The last term is the scalar value flag.
		uint16_t dcm		   					:1; ///<bit3:	The estimated attitude directional cosine matrix given in column major order flag.
		uint16_t mag_ned       			:1; ///<bit4:	The current estimated magnetic field (Gauss), given in the North East Down (NED) frame flag.
		uint16_t accel_ned					:1; ///<bit5:	The estimated acceleration (with gravity) reported in m/s^2, given in the North East Down (NED) frame flag.
		uint16_t linear_accel_body  :1; ///<bit6:	The estimated linear acceleration (without gravity) reported in m/s^2, and given in the body frame flag.
		uint16_t linear_accel_ned   :1; ///<bit7:	The estimated linear acceleration (without gravity) reported in m/s^2, and given in the NED frame flag.
		uint16_t ypr_u			    		:1; ///<bit8:	The estimated attitude (Yaw, Pitch, Roll) uncertainty (1 Sigma), reported in degrees flag.
		uint16_t reserved						:7; ///<bit9-15:Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};

/*
 * @brief Binary group 5 provides all estimated outputs which are dependent upon the estimated attitude solution.
 */
union attitude_group_msg_type
{
	struct
	{
		uint16_t ahrs_status    			:1; ///<bit0:	The VPE status bitfield.
		uint16_t ypr		   					:1; ///<bit1:	The estimated attitude Yaw, Pitch, and Roll angles measured in degrees flag.
		uint16_t qtn   		   				:1; ///<bit2:	The estimated attitude quaternion. The last term is the scalar value flag.
		uint16_t dcm		   					:1; ///<bit3:	The estimated attitude directional cosine matrix given in column major order flag.
		uint16_t mag_ned       			:1; ///<bit4:	The current estimated magnetic field (Gauss), given in the North East Down (NED) frame flag.
		uint16_t accel_ned					:1; ///<bit5:	The estimated acceleration (with gravity) reported in m/s^2, given in the North East Down (NED) frame flag.
		uint16_t linear_accel_body  :1; ///<bit6:	The estimated linear acceleration (without gravity) reported in m/s^2, and given in the body frame flag.
		uint16_t linear_accel_ned   :1; ///<bit7:	The estimated linear acceleration (without gravity) reported in m/s^2, and given in the NED frame flag.
		uint16_t ypr_u			    		:1; ///<bit8:	The estimated attitude (Yaw, Pitch, Roll) uncertainty (1 Sigma), reported in degrees flag.
		uint16_t heave						:7; ///<bit9-15:Reserved. Not used on this product flag.
	}bits;
	uint16_t all;
};


/*
 * @brief
 */
union vpe_status_type
{
	struct
	{
		uint16_t attitude_quality 				:2;	  ///<bit0-1:  	Provides an indication of the quality of the attitude solution.
		uint16_t gyro_saturation  				:1;	  ///<bit2:	   	At least one gyro axis is currently saturated.
		uint16_t gyro_saturation_recovery :1;	  ///<bit3:    	Filter is in the process of recovering from a gyro saturation event.
		uint16_t mag_disturbance					:2;	  ///<bit4-5:  	A magnetic DC disturbance has been detected.
		uint16_t acc_disturbance 					:2;	  ///<bit6-7:  	A strong acceleration disturbance has been detected.
		uint16_t acc_saturation						:1;	  ///<bit8:    	At least one accelerometer axis is currently saturated.
	  uint16_t reserved									:1;	  ///<bit9:    	Reserved for internal use.
	  uint16_t known_mag_disturbance  	:1;	  ///<bit10:   	A known magnetic disturbance has been reported by the user and the magnetometer is currently tuned out.
	  uint16_t known_accel_disturbance 	:1;	  ///<bit11:   	A known acceleration disturbance has been reported by the user and the accelerometer is currently tuned out.
	  uint16_t reserved2								:4;	  ///<bit12-15: Reserved for future use.
	}bits;
	uint16_t all;
};



/*
 * @brief Parse message controller step.
 */
enum class vn100_parse_status_type : uint8_t
{
	PREAMBLE  		= 0x00,			///<	Check preamble controller.
	GROUP     		= 0x01,			///<	Check group message controller.
	BINARY_GROUP  = 0x02,			///<  Binary group messages added.
	BINARY_GROUP1 = 0x03,			///<	Binary group1 messages added.
	BINARY_GROUP2	= 0x04,			///<	Binary group2 messages added.
	BINARY_GROUP3 = 0x05,			///<	Binary group3 messages added.
	BINARY_GROUP5 = 0x06,			///<	Binary group5 messages added.
	CHECKSUM			= 0x07,			///<	Check checksum controller payload.
};

/*
 * @brief Parse message controller step.
 */
enum class vn100_parser_status_type : uint8_t
{
	PREAMBLE  	   = 0,			///<	Check preamble controller.
	GROUP     	   = 1,			///<	Check group message controller.
	COMMON_GROUP_LOW  = 2,			///<	Binary group1 messages added.
	COMMON_GROUP_HIGH  = 3,			///<	Binary group2 messages added.
	TIME_GROUP_LOW  = 4,			///<	Binary group3 messages added.
	TIME_GROUP_HIGH  = 5,			///<	Binary group5 messages added.
	IMU_GROUP_LOW	   = 6,			///<	Check checksum controller payload.
	IMU_GROUP_HIGH	   = 7,			///<	Check checksum controller payload.
	ATTITUDE_GROUP_LOW = 8,
	ATTITUDE_GROUP_HIGH = 9,
	PAYLOAD_CONTAINER   = 10,
	CRC_LOW				= 11,
	CRC_HIGH  		    = 12,
	CRC_CHECK		    = 13,
};



/*
 * @brief Initialize function return message parse state.
 */
enum class vn100_response_parse_type : uint8_t
{
	PREAMBLE     = 0x00,
	FIRST_INDEX  = 0x01,
	SECOND_INDEX = 0x02,
	PAYLOAD 	 	 = 0x03,
	CHECKSUM     = 0x04,
	PARSE        = 0x05,
};

enum class vn100_scheduler_state_type : uint8_t
{
	FIRST_START = 0x00,
	MEASURE			= 0x01,
};


struct sensor_info_type
{
		uint32_t model;
		uint8_t hardware_rev;
		uint32_t serial_number;
		uint32_t firmware_version;
};


/*
 * @brief
 */
struct output_raw_data_type
{
	uint64_t time_startup; 	  	 ///< This field is equivalent to the TimeStartup field in group 2.
	uint64_t time_sync_in;	 	 ///< This field is equivalent to the TimeSyncIn field in group 2.

	struct
	{
		float yaw;			 	 ///< This field is equivalent to the YPR field in group 5.
		float pitch;		 	 ///< This field is equivalent to the YPR field in group 5.
		float roll;			 	 ///< This field is equivalent to the YPR field in group 5.
	}ypr;				///<

	struct
	{
		float qtn_1; 			 ///< This field is equivalent to the Quaternion field in group 5.
		float qtn_2;		 	 ///< This field is equivalent to the Quaternion field in group 5.
		float qtn_3; 			 ///< This field is equivalent to the Quaternion field in group 5.
		float qtn_4; 			 ///< This field is equivalent to the Quaternion field in group 5.
	}quaternion;		///<

	struct
	{
		float body_x_axis_rs;    	 ///< This field is equivalent to the AngularRate field in group 3.
		float body_y_axis_rs;	  	 ///< This field is equivalent to the AngularRate field in group 3.
		float body_z_axis_rs;   	 ///< This field is equivalent to the AngularRate field in group 3.
	}angular_rate_rs;		///<

	struct
	{
		double body_x_axis_ms;    	 ///< Conversion AngularRate field in group 3.
		double body_y_axis_ms;	  	 ///< Conversion AngularRate field in group 3.
		double body_z_axis_ms;   	 ///< Conversion AngularRate field in group 3.
	}angular_rate_ms;		///<

	struct
	{
		float body_x_axis_ms;		 ///< This field is equivalent to the Accel field in group 3.
		float body_y_axis_ms;		 ///< This field is equivalent to the Accel field in group 3.
		float body_z_axis_ms;		 ///< This field is equivalent to the Accel field in group 3.
	}accel_ms;				///<


	struct
	{
		float body_x_axis_accel; 	///< This field is equivalent to the UncompGyro field in group 3.
		float body_y_axis_accel;	///< This field is equivalent to the UncompGyro field in group 3.
		float body_z_axis_accel; 	///< This field is equivalent to the UncompGyro field in group 3.
		float body_x_axis_rate_rs;  ///< This field is equivalent to the UncompGyro field in group 3.
		float body_y_axis_rate_rs;  ///< This field is equivalent to the UncompGyro field in group 3.
		float body_z_axis_rate_rs;  ///< This field is equivalent to the UncompGyro field in group 3.
	}imu;				///<

	struct
	{
		float body_x_axis_rate_ms;  ///< Conversion UncompGyro field in group 3.
		float body_y_axis_rate_ms;  ///< Conversion UncompGyro field in group 3.
		float body_z_axis_rate_ms;  ///< Conversion UncompGyro field in group 3.
	}imu_ms;				///<

	struct
	{
		float mag_x;			 ///< This field is equivalent to the Mag,Temp and Press fields in group 3.
		float mag_y;			 ///< This field is equivalent to the Mag,Temp and Press fields in group 3.
		float mag_z;			 ///< This field is equivalent to the Mag,Temp and Press fields in group 3.
		float temp;				 ///< This field is equivalent to the Mag,Temp and Press fields in group 3.
		float pres;				 ///< This field is equivalent to the Mag,Temp and Press fields in group 3.
	}mag_pres;			///<

	struct
	{
		float dtime;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
		float dtheta_0;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
		float dtheta_1;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
		float dtheta_2;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
		float dvel_0;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
		float dvel_1;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
		float dvel_2;			 ///< This field is equivalent to the DeltaTheta and DeltaVel fields in group 3.
	}delta_theta_vel;	///<

	uint32_t sync_in_cnt;		 ///< This field is equivalent to the SyncInCnt field in group 2.
	uint32_t sync_out_cnt;		 ///<
	uint16_t imu_status;		 ///<

	struct
	{
		float body_x_axis;		 ///<
		float body_y_axis;		 ///<
		float body_z_axis;		 ///<
	}uncomp_mag;		///<

	struct
	{
		float dcm_1;			 ///<
		float dcm_2;			 ///<
		float dcm_3;			 ///<
		float dcm_4;			 ///<
		float dcm_5;			 ///<
		float dcm_6;			 ///<
		float dcm_7;			 ///<
		float dcm_8;			 ///<
	}dcm; 				///< directional cosine matrix

	struct
	{
		float north_axis;		 ///<
		float east_axis;		 ///<
		float down_axis;		 ///<
	}mag_ned;			///<

	struct
	{
		float north_axis_ms;	 ///<
		float east_axis_ms;		 ///<
		float down_axis_ms;		 ///<
	}accel_ned;			///<

	struct
	{
		float x_ms;			 	 ///<
		float y_ms;				 ///<
		float z_ms;				 ///<
	}linear_accel_body; ///<

	struct
	{
		float north_axis;		 ///<
		float east_axis;		 ///<
		float down_axis;		 ///<
	}linear_accel_ned;  ///<

	struct
	{
		float yaw;				 ///<
		float pitch;			 ///<
		float roll;				 ///<
	}yprU;				///<
};

/*
* @brief Message type value in VN100.
*/
struct heading_type
{
	float magnetic_north_2d;	///< Calculate IMU magnetometer two(x,y) axis value.
	float magnetic_north_3d;  ///< Calculate IMU magnetometer there(x,y,z) axis value.
};


const uint8_t BinaryPacketGroupLengths[4][16] = {
	{ 8, 0, 8, 12, 16, 12, 0, 0, 12, 24, 20, 28, 2, 4, 0,0},
	{ 8, 0, 0, 0, 8, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0},
	{ 2, 12, 12, 12, 4, 4, 16, 12, 12, 12, 12, 0, 0, 0, 0 },
	{ 2, 12, 16, 36, 12, 12, 12, 12, 12, 0, 0, 0, 0, 0, 0,0},
};

const uint8_t SYNC_MESSAGE = 0xFA;

const uint8_t CHANNEL1 = 1;  ///< Selectable output message channel.
const uint8_t CHANNEL2 = 2;	 ///< Selectable output message channel.


const uint8_t HZ800 = 1;  	///< Divider 8 is 800Hz/1 = 800Hz---> VN100 Bitrate
const uint8_t HZ400 = 2;  	///< Divider 2 is 800Hz/2 = 400Hz---> VN100 Bitrate
const uint8_t HZ266 = 3;  	///< Divider 3 is 800Hz/3 = 266Hz---> VN100 Bitrate
const uint8_t HZ200 = 4;  	///< Divider 4 is 800Hz/4 = 200Hz---> VN100 Bitrate
const uint8_t HZ160 = 5;  	///< Divider 5 is 800Hz/5 = 160Hz---> VN100 Bitrate
const uint8_t HZ133 = 6;  	///< Divider 6 is 800Hz/6 = 133Hz---> VN100 Bitrate
const uint8_t HZ114 = 7;  	///< Divider 7 is 800Hz/7 = 114Hz---> VN100 Bitrate
const uint8_t HZ100 = 8;  	///< Divider 8 is 800Hz/8 = 100Hz---> VN100 Bitrate

const uint8_t GROUP_COMMON 												= 0x01;		///<
const uint8_t GROUP_TIME   												= 0x02;		///<
const uint8_t GROUP_IMU    												= 0x04;		///<
const uint8_t GROUP_ATTITUDE  										= 0x10;		///<

const uint16_t BINARY_GROUP_1_NOT_USED	   				= 0x00;		///<
const uint16_t BINARY_GROUP_1_TIME_STARTUP 				= 0x01;		///<
const uint16_t BINARY_GROUP_1_TIME_SYNCIN  				= 0x04;		///<
const uint16_t BINARY_GROUP_1_YPR          				= 0x08;		///<
const uint16_t BINARY_GROUP_1_QUETERNION   				= 0x10;		///<
const uint16_t BINARY_GROUP_1_ANGULAR_RATE 				= 0x20;		///<
const uint16_t BINARY_GROUP_1_ACCEL 	   					= 0x100;	///<
const uint16_t BINARY_GROUP_1_IMU		   						= 0x200;	///<
const uint16_t BINARY_GROUP_1_MAG_PRESS    				= 0x400;	///<
const uint16_t BINARY_GROUP_1_DELTA_THETA 	 			= 0x800;	///<
const uint16_t BINARY_GROUP_1_VPE_STATUS   				= 0x1000;	///<
const uint16_t BINARY_GROUP_1_SYNC_IN_CNT  				= 0x2000;	///<

const uint16_t BINARY_GROUP_2_NOT_USED	   				= 0x00;		///<
const uint16_t BINARY_GROUP_2_TIME_STARTUP 				= 0x01;		///<
const uint16_t BINARY_GROUP_2_TIME_SYNCIN  				= 0x10;		///<
const uint16_t BINARY_GROUP_2_SYNC_CNT     				= 0x80;		///<
const uint16_t BINARY_GROUP_2_SYNC_OUT_CNT 				= 0x100;	///<
const uint16_t BINARY_GROUP_2_TIME_STATUS  				= 0x200;	///<

const uint16_t BINARY_GROUP_3_NOT_USED	   				= 0x00;		///<
const uint16_t BINARY_GROUP_3_IMU_STATUS 					= 0x01;		///<
const uint16_t BINARY_GROUP_3_UNCOMP_MAG 					= 0x02;		///<
const uint16_t BINARY_GROUP_3_UNCOMP_ACCEL 				= 0x04;		///<
const uint16_t BINARY_GROUP_3_UNCOMP_GYRO 				= 0x08;		///<
const uint16_t BINARY_GROUP_3_TEMP 								= 0x10;		///<
const uint16_t BINARY_GROUP_3_PRES 					= 0x20;		///<
const uint16_t BINARY_GROUP_3_DELTA_THETA 			= 0x40;		///<
const uint16_t BINARY_GROUP_3_DELTA_V 				= 0x80;		///<
const uint16_t BINARY_GROUP_3_MAG 					= 0x100;	///<
const uint16_t BINARY_GROUP_3_ACCEL 				= 0x200;	///<
const uint16_t BINARY_GROUP_3_ANGULAR_RATE 			= 0x400;	///<

const uint16_t BINARY_GROUP_5_NOT_USED	   			= 0x00;		///<
const uint16_t BINARY_GROUP_5_VPE_STATUS 			= 0x01;		///<
const uint16_t BINARY_GROUP_5_YPR 					= 0x02;		///<
const uint16_t BINARY_GROUP_5_QTN 					= 0x04;		///<
const uint16_t BINARY_GROUP_5_DCM 				    = 0x08;		///<
const uint16_t BINARY_GROUP_5_MAG_NED 				= 0x10;		///<
const uint16_t BINARY_GROUP_5_ACCEL_NED 			= 0x20;		///<
const uint16_t BINARY_GROUP_5_LINEAR_ACCEL_BODY 	= 0x40;		///<
const uint16_t BINARY_GROUP_5_LINEAR_ACCEL_NED  	= 0x80;		///<
const uint16_t BINARY_GROUP_5_YPRU 					= 0x100;	///<


const uint8_t VN100_RESPONSE_TRUE								= 0x18;
const uint8_t VN100_RESPONSE_FALSE								= 0x00;


const double M_PI_C = 3.1415926535897932384626433832795;

//Buffer container
struct buffer_container_type
{
	uint8_t  data[2048];
	uint16_t length;
	uint16_t counter;
	uint16_t crc_counter;
	uint16_t received_checksum;
	int32_t  checksum  ;

	buffer_container_type() :
	data{},
	length(0),
	counter(0),
	crc_counter(0),
	received_checksum(0),
	checksum(0)
	{ }
};

class VN100
{
public:

	explicit VN100(Peripherals::Uart::H747_UART *uart_module,uint8_t bitRate,uint8_t serialChannel,uint32_t baudRate);
	explicit VN100(Peripherals::Uart::H747_UART *uart_module, uint8_t groupHeader,uint16_t binary_group1,uint16_t binary_group2,uint16_t binary_group3,uint16_t binary_group4);

	output_raw_data_type output_raw_data = {0};


	uint8_t initialize(void);
	void 	scheduler(void);
	void 	getByte(void);
	void 	setGyroBias(void);
	void 	setReferanceFrame(float x1, float x2, float x3, float y1, float y2, float y3, float z1, float z2, float z3);

	void getHeadingCompass(void);
	void closeAsyncOutputConfig(void);
	void baseInitialize(void);
	void conversionRadToMs(void);

	uint8_t getModel(void);
	uint8_t getSerialNumber(void);
	uint8_t getFirmwareVersion(void);
	uint8_t getHardwareRevision(void);
	uint8_t responseController(void);
	void openSerialPort(void);
	void settingBitrate(void);
	void settingOutputGroup(void);
	VN100(const VN100& orig);
	virtual ~VN100();

private:

	void sendData(char buffer[],uint16_t size);
	void sendBaudRate(void);
	void getData(uint8_t buffer[],uint16_t size);
	void parseMessage(void);
	void processReceivedPacket(buffer_container_type &payload);

	uint8_t  		calculateChecksum( char buffer[], uint16_t length);
	uint16_t calculatedPayloadLength(uint16_t value,uint8_t group_index);
	uint16_t 		calculateCRC(uint8_t data[],uint16_t length);

	unsigned short  calculateChar_CRC(char  data[], uint8_t length);

	Peripherals::Uart::H747_UART *uart = {0};

	uint8_t  group_header   			= 0;	///<
	uint8_t  vn100_bitRate 			    = 0;	///<
	uint8_t  vn100_serial_channel 	 	= 0;	///<
	uint8_t  response 					= 0;	///< Imu initialize error response. 0xFF null value.
	uint8_t  sensor_ack 			 	= 0;
	uint8_t  imu_initialize 			= 0;	///< Imu initialize result
	uint16_t binary_group_1 			= 0;	///<
	uint16_t binary_group_2 			= 0;	///<
	uint16_t binary_group_3 			= 0;	///<
	uint16_t binary_group_5 			= 0;	///<
	uint16_t calculated_crc 			= 0;	///< The variable where the crc value of the related package is kept.
	uint16_t incoming_crc 				= 0;	///< CRC information from the sensor. // gereksiz
	uint32_t vn100_baudrate 			= 0;	///<
	uint32_t baudrate_array[9] = {9600,19200,38400,57600,115200,128000,230400,460800,921600}; 	///< Auto baud detection array.

	system_error_code_type  system_error_code;	///<
	group_header_type  		msg_group_header;		///<
	common_group_msg_type   common_group_msg;
	time_group_msg_type     time_group_msg;
	imu_group_msg_type      imu_group_msg;
	attitude_group_msg_type attiude_group_msg;

	binary_group1_type 			msg_binary_group1;	///<
	binary_group2_type 			msg_binary_group2;	///<
	binary_group3_type 			msg_binary_group3;	///<
	binary_group5_type 			msg_binary_group5;	///<

	vpe_status_type 			vpe_status;							///<
	sensor_info_type			sensor_info = {0};
	heading_type				heading = {0};
}; // End of class VN100

} // End of namespace VN100
} // End of namespace IMU
extern Sensors::Imu::VN100 imu;

#endif /* DS_VN100_HPP */
