/**
 ******************************************************************************
  * @file			: ds_venus.hpp
  * @brief		: Task scheduler header file
  *				  		This file contains task structure of the application
  * @author		: Gurtac Kadem
  * 						Alperen Emiroglu
  * @date			: 18.11.2020
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


#ifndef DS_VENUS_HPP_
#define DS_VENUS_HPP_
/*
 * Begin of Includes
 */
#include <stdint.h>
#include <ds_uart.hpp>
#include <ds_uart_h747.hpp>
// End of Includes


/*
 * Begin of Macro Definitions
 */
//End of Macro Definitions



/*
 * Begin of Enum, Union and Struct Definitions
 */

typedef union
{
    struct
    {
        uint16_t unused_1                            :1;
        uint16_t unused_2                            :1;
        uint16_t unused_3                            :1;
        uint16_t unused_4                            :1;
        uint16_t unused_5                            :1;
        uint16_t unused_6                            :1;
        uint16_t unused_7                            :1;
        uint16_t unused_8                            :1;
        uint16_t unused_9                            :1;
        uint16_t unused_10                           :1;
        uint16_t unused_11                           :1;
        uint16_t geolock_active                      :1; //1
        uint16_t reserved                            :4; //12-16
    }bits;
    uint16_t u16;
}status_flag_t;

typedef union
{
		struct
		{
				uint8_t motor_enable_active    					:1; //1
				uint8_t motor_disable_active   					:1; //2
				uint8_t gimbal_reset_active    					:1; //3
				uint8_t execute_command_active 					:1; //4
				uint8_t control_command_active 					:1; //5
				uint8_t raw_encoder_command_active 			:1; //6
				uint8_t reserved  											:1; //7-8

		}bits;
		uint8_t u8;
}confirm_flag_t;

typedef enum
{
  	MOTOR_ENABLE             = 0x4D,
		MOTOR_DISABLE            = 0x6D,
		BOARD_INFORMATION        = 0x56,
		EXTRA_BOARD_INFORMATION  = 0x14,
		GIMBAL_RESET             = 0x72,
		ANGLE_INFORMATION        = 0x49,
		EXECUTE_COMMAND          = 0x45,
		STATUS                   = 0x17,
		BROAD_STATUS             = 0x19,
		CONTROL_COMMAND          = 0x43,
		CONFIRM                  = 0x43,
		RAW_ENCODER_DATA         = 0x55,
		RAW_ENCODER_BROAD_STATUS = 0x58,
}MessageId_t;

enum class ahit_venus_state_t : uint8_t
{
  	START         = 0,
  	RTL           = 1,
		LOOK_DOWN     = 2,
		LOOK_UP       = 3,
};

enum class message_id_t : uint8_t
{
  	MOTOR_ENABLE             = 0x4D,
  	MOTOR_DISABLE            = 0x6D,
		BOARD_INFORMATION        = 0x56,
		EXTRA_BOARD_INFORMATION  = 0x14,
		GIMBAL_RESET             = 0x72,
		ANGLE_INFORMATION        = 0x49,
		EXECUTE_INFORMATION      = 0x45,
		STATUS                   = 0x17,
		BROAD_STATUS             = 0x19,
};

enum class venus_packet_state_t : uint8_t
{
    START_BYTE        = 0x00,
    PACKET_ID         = 0x01,
    DATA_SIZE         = 0x02,
    HEADER_CHECKSUM   = 0x03,
    RESPONSE          = 0x04,
    PAYLOAD_CHECKSUM  = 0x05,
};

enum class raw_encoder_packet_state_t : uint8_t
{
    START_BYTE        = 0x00,
    PACKET_ID         = 0x01,
    DATA_SIZE         = 0x02,
    HEADER_CHECKSUM   = 0x03,
    RESPONSE          = 0x04,
    PAYLOAD_CHECKSUM  = 0x05,
};

enum class axis_t : uint8_t{
    ROLL   = 0x00,
    PITCH  = 0x01,
    YAW    = 0x02,
};

enum class mode_command_t : uint8_t
{
    NO_CONTROL           = 0x00,
    SPEED_MODE           = 0x01,
    ANGLE_MODE           = 0x02,
    SPEED_ANGLE_MODE     = 0x03,
    MODE_ANGLE_REL_FRAME = 0x05,
};

enum class command_t : uint8_t
{
    NORMAL_MODE    = 0x00,
    BREAK_MODE     = 0x01,
    SAFE_MODE      = 0x02,
    MOTOR_ON       = 0x0B,
    MOTOR_OFF      = 0x0C,
    LOOK_DOWN      = 0x11,
    HOME_POSITION  = 0x12,
    CENTER_YAW     = 0x23,
    ROTATE_YAW     = 0x28,
    MOTOR_RESET    = 0x07,
    IMU_RESET      = 0x3E
};

struct board_info_pkt_t{
    uint8_t  card_version;
    uint16_t software_version;
    uint8_t  reserved[14];
};

struct extra_board_info_pkt_t{
    uint8_t  device_id[9];
    uint8_t  mcu_id[12];
    uint32_t eeprom_size;
    uint8_t  reserved[44];
};

struct angle_info_pkt_t{
    int16_t  roll_imu;
    float    roll_angle_deg;
    int16_t  roll_target_imu;
    int16_t  roll_target_speed;
    int16_t  pitch_imu;
    float    pitch_angle_deg;
    int16_t  pitch_target_imu;
    int16_t  pitch_target_speed;
    int16_t  yaw_imu;
    float    yaw_angle_deg;
    int16_t  yaw_target_imu;
    int16_t  yaw_target_speed;
    int16_t  yaw_imu_command;
    int32_t  encoder_yaw_angle_command_deg;
};

struct status_pkt_t{

    int16_t  acc_data;
    int16_t  gyro_data;
    uint16_t serial_err_cnt;
    uint16_t system_error;
    uint8_t  system_sub_error;
    uint8_t  reserved1[3];
    int8_t   reserved2[6];
    int16_t  reserved3;
    uint8_t  reserved4[4];
    uint16_t imu_angle[3];
    uint16_t frame_imu_angle[3];
    uint16_t target_angle[3];
    uint16_t cycle_time;
    uint16_t i2c_error_count;
    uint8_t  error_code;
    uint16_t bat_level;
    uint8_t  rt_data_flags;
    uint8_t  cur_imu;
    uint8_t  reserved5;
    uint8_t  motor_power[3];
    uint8_t  reserved6[8];
};

struct broad_status_pkt_t{
    uint8_t  reserved1[13];
    uint16_t current;
    uint8_t  reserved2[6];
    int8_t   imu_temperature;
    int8_t   frame_imu_temperature;
    uint8_t  reserved3[101];
};

struct encoder_status_pkt_t{
    uint16_t  timestamp_ms;
    int32_t   encoder_roll;
    int32_t   encoder_pitch;
    int32_t   encoder_yaw;
    float     encoder_yaw_angle_deg;
    float     first_data_deg 			= -27.0;
    float     true_north_deg;
};

struct control_command_pkt_t{
    uint8_t   control_mode_roll 	= 0;
    uint8_t   control_mode_pitch 	= 0;
    uint8_t   control_mode_yaw 		= 0;
    int16_t   speed_roll				 	= 0;
    int16_t   imu_roll 						= 0;
    int16_t   speed_pitch 				= 0;
    int16_t   imu_pitch 					= 0;
    int16_t   speed_yaw 					= 0;
    int16_t   imu_yaw 						= 0;
};

struct encoder_command_pkt_t{
    uint8_t   id 									= 0x58;
    uint16_t  interval_ms 				= 0x0064;
    int8_t    config[8] 					= {0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t   reserved[10] 				= {0};
};
// End of Enum, Union and Struct Definitions

/*
 * Begin of MATRIX_ORBITAL Class Definition
 */
class DS_VENUS
{
    public:
				DS_VENUS(void);

				venus_packet_state_t        packet_state = venus_packet_state_t::START_BYTE;
				confirm_flag_t              confirm_flag;
				status_flag_t 							status_flag;

		    uint8_t zoom_info 							= 0;
		    uint8_t camera_info 						= 0;
		    int32_t target_latitude_uddeg 	= 0;
        int32_t target_longitude_uddeg 	= 0;

        bool pilot_mode_command_send 		= false;
        bool get_target_imu_unit_deg 		= true;
        bool first_pilot_mode_command 	= false;
        bool encoder_zero_calculated 		= false;

        float pitch_command_deg 				= 0.0;
        float yaw_command_deg		 				= 0.0;

        float pitch_imu_command 				= 0.0;

        float geolock_latitude_uddeg 		= 0.0;
        float geolock_longitude_uddeg 	= 0.0;

				int32_t trim_ref = 2000;
				int32_t trim_command = 0;

        void readPackage( void );
		    void sendControlCommand(control_command_pkt_t ref_control_command);
		    void sendEncoderRawDataCommand(encoder_command_pkt_t ref_encoderRawData_command);
		    void sendMotorEnable(void);
		    void sendMotorDisable(command_t command);
	      void sendBoardInformation(void);
	      void sendExtraBoardInformation(void);
	      void sendReset(void);
	      void sendAngleInformation(void);
	      void sendExecuteCommand(command_t command);
	      void sendStatus(void);
	      void sendBroadStatus(void);
	      void sendCommand(message_id_t message_id, command_t command);
        void sendRequest(message_id_t message_id);
	      void sendReference(mode_command_t mode_command, axis_t axis , int16_t speed_ref_dps, float angle_ref_deg);
	      void sendPilotMode(void);
        void send2DCommand(float command_pitch_deg, float command_yaw_deg);
        void checkLimits(int16_t& command_unit_deg, int16_t const minimum_deg, int16_t const maximum_deg);
		    void checkLimitsDeg(float& command_deg,  float minimum_deg, float maximum_deg);
		    void scheduler(void);
		    void lookUp(void);
		    void lookDown(void);
		    void send2DAngleCommand(float command_pitch_deg, float command_yaw_deg);
		    void sendYawAngleCommand(float command_yaw_deg);
        void checkDrift(void);
        void activateGeolock(void);
        void deactivateGeolock(void);
        void lookHereLimits(float pitch_command_deg, float yaw_command_deg);
        float zoomScale(void);
        void sendErrorCorrection(float angle_ref_deg);

		    board_info_pkt_t             board_info_pkt;
		    extra_board_info_pkt_t       extra_board_info_pkt;
		    angle_info_pkt_t             angle_info_pkt;                                //GIMBAL -> PC
		    status_pkt_t                 status_pkt;
		    broad_status_pkt_t           broad_status_pkt;
		    encoder_status_pkt_t         encoder_status_pkt;
		    encoder_command_pkt_t        ref_encoder_command;

		    int16_t speed_reference_dps      			= 0x0029;

		    const uint8_t PACKET_START_BYTE       = 0x3E;
				const uint8_t DS_VENUS_UART_BYTE_SIZE = 255;

		    const int16_t ROLL_MINIMUM_ANGLE_DEG  = -25;
		    const int16_t ROLL_MAXIMUM_ANGLE_DEG  =  25;
		    const int16_t PITCH_MINIMUM_ANGLE_DEG = -60;
		    const int16_t PITCH_MAXIMUM_ANGLE_DEG =  25;
		    const int16_t YAW_MAXIMUM_ANGLE_DEG   =  165;
		    const int16_t YAW_MINIMUM_ANGLE_DEG   = -175;

		    const float LOOK_UP_ANGLE_DEG    			=  25.0;
		    const float LOOK_DOWN_ANGLE_DEG  			= -65.0;
		    const float ZERO_POS_ANGLE_DEG   			=  15.0;

		    const float   MAXIMUM_ANGLE_DEG 			= 180.0;
		    const float   MINIMUM_ANGLE_DEG 			= -180.0;
    protected:

    private:

        void grabPackage    ( uint8_t *coming_bytes, uint16_t bytes_number );
        void parsePackage   ( void );
        void sendAngleReference(mode_command_t roll_mode, mode_command_t pitch_mode, mode_command_t yaw_mode, float command_roll_deg, float command_pitch_deg, float command_yaw_deg);

        uint8_t  pkt_id             	= 0;
        uint16_t  pkt_size           	= 0;
        uint8_t  pkt_header_chksm   	= 0;
        uint8_t  calc_header_chksm 		= 0;
        uint8_t  pkt_data[255]      	= {0};
        uint16_t pkt_data_idx      		= 0;
        uint8_t  pkt_payload_chksm  	= 0;
        uint8_t  calc_payload_chksm 	= 0;
        //uint8_t  uart_no_u8 = UART_NUMBER_4;

        const float   DEG_TO_IMU 									= 45.511111;                        // 1.0 / IMU_TO_DEG;
        const float   IMU_TO_DEG 									= 0.02197265625;
        const int16_t YAW_DRIFT_IMU_CHECK_LIMIT 	= 30;                               // 2 * DEG_TO_IMU
        const float   YAW_DRIFT_TRUE_NORTH_LIMIT 	= 2.0;
        const float   M_TO_DEG  									= 89.83152098;
        const float   RAD_TO_DEG 									= 180.0 / 3.1415;
        const float   DEG_TO_RAD 									= 3.1415 / 180.0;
        const float   DEG_TO_M 										= 1.113195 * 0.01;

};
// End of MATRIX_ORBITAL Class Definition

/*
 * External Linkages
 */
extern DS_VENUS venus;
// End of External Linkages





#endif /* DS_VENUS_HPP_ */
