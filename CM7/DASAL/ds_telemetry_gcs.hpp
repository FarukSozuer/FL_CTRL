/**
 ******************************************************************************
 * @file		: ds_telemetry_gcs.hpp
 * @brief	: Telemetry and Telemetry GCS Class
 * @author	: Gurtac Kadem
 * 					This file contains telemetry and telemetry gcs class
 * @date		: 02.06.2020
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

#ifndef DS_TELEMETRY_GCS_HPP
#define DS_TELEMETRY_GCS_HPP

/*
 * Begin of Includes
 */
#include <stdint.h>
#include "ds_telemetry.hpp"
#include "ds_uart_h747.hpp"
// End of Includes

namespace Telemetry
{

/*
 * Begin of Macro Definitions
 */
const uint16_t HEARTBEAT_TX_BUFFER_SIZE = 14;
const uint16_t VEHICLE_TX_BUFFER_SIZE = 4;
const uint16_t PAYLOAD_TX_BUFFER_SIZE = 4;
const uint16_t WAYPOINT_TX_BUFFER_SIZE = 4;
const uint16_t TELEMETRY_5HZ_TX_BUFFER_SIZE = 313;
const uint16_t TELEMETRY_1HZ_TX_BUFFER_SIZE = 68;
const uint16_t CALIBRATION_TX_BUFFER_SIZE = 4;
const uint16_t CONTROL_1_TX_BUFFER_SIZE = 4;
const uint16_t CONTROL_2_TX_BUFFER_SIZE = 4;
const uint16_t CONTROL_3_TX_BUFFER_SIZE = 4;
const uint16_t CONTROL_4_TX_BUFFER_SIZE = 4;
const uint16_t CONFIG_TX_BUFFER_SIZE = 4;
const uint16_t CHP_TX_BUFFER_SIZE = 4;
const uint16_t NFZ_TX_BUFFER_SIZE = 4;
const uint16_t BOOT_TX_BUFFER_SIZE = 4;

const uint16_t HEARTBEAT_RX_BUFFER_SIZE = 8;
const uint16_t VEHICLE_RX_BUFFER_SIZE = 4;
const uint16_t PAYLOAD_RX_BUFFER_SIZE = 4;
const uint16_t WAYPOINT_RX_BUFFER_SIZE = 4;
const uint16_t CALIBRATION_RX_BUFFER_SIZE = 4;
const uint16_t CONTROL_1_RX_BUFFER_SIZE = 4;
const uint16_t CONTROL_2_RX_BUFFER_SIZE = 4;
const uint16_t CONTROL_3_RX_BUFFER_SIZE = 4;
const uint16_t CONTROL_4_RX_BUFFER_SIZE = 4;
const uint16_t CONFIG_RX_BUFFER_SIZE = 4;
const uint16_t CHP_RX_BUFFER_SIZE = 4;
const uint16_t NFZ_RX_BUFFER_SIZE = 4;
const uint16_t BOOT_RX_BUFFER_SIZE = 4;
//End of Macro Definitions

/*
 * Begin of Enum, Union and Struct Definitions
 */

enum class gcs_receive_headers_type : uint8_t
{
	RX_HEARTBEAT = 1,
	RX_VEHICLE = 3,
	RX_PAYLOAD = 5,
	RX_WAYPOINT = 7,
	RX_CALIBRATION = 13,
	RX_CONTROL_1 = 15,
	RX_CONTROL_2 = 17,
	RX_CONTROL_3 = 19,
	RX_CONTROL_4 = 21,
	RX_CONFIG = 23,
	RX_CHANGE_HOME = 25,
	RX_NO_FLY_ZONE = 27,
	RX_BOOT = 29
};

enum class gcs_transmit_headers_type : uint8_t
{
	TX_HEARTBEAT = 2,
	TX_VEHICLE = 4,
	TX_PAYLOAD = 6,
	TX_WAYPOINT = 8,
	TX_TELEMETRY_5HZ = 10,
	TX_TELEMETRY_1HZ = 12,
	TX_CALIBRATION = 14,
	TX_CONTROL_1 = 16,
	TX_CONTROL_2 = 18,
	TX_CONTROL_3 = 20,
	TX_CONTROL_4 = 22,
	TX_CONFIG = 24,
	TX_CHANGE_HOME = 26,
	TX_NO_FLY_ZONE = 28,
	TX_BOOT = 30,
};

enum class control_message_type : uint8_t
{
	TYPE_1 = 1,
	TYPE_2 = 2,
	TYPE_3 = 3,
	TYPE_4 = 4,
};

enum class telemetry_id_type : uint8_t
{
	GCS = 0x00,
	FLIGHT_CONTROLLER = 0x01,
	INS = 0x02,
	MISSION_COMPUTER = 0x03,
	RADAR = 0x04,
	PDB = 0x05,
};

#pragma pack(1)
union heartbeat_receive_type
{
		struct
		{
				uint32_t fc_timestamp_feedback = 0;
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[HEARTBEAT_RX_BUFFER_SIZE] = { 0 };

		heartbeat_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union heartbeat_transmit_type
{
		struct
		{
				uint32_t timestamp = 0;
				uint32_t communication_delay = 0;
				uint8_t ap_major = 0;
				uint8_t ap_minor = 0;
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[HEARTBEAT_TX_BUFFER_SIZE] = { 0 };

		heartbeat_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union vehicle_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[VEHICLE_RX_BUFFER_SIZE] = { 0 };

		vehicle_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union vehicle_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[VEHICLE_TX_BUFFER_SIZE] = { 0 };

		vehicle_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union payload_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[PAYLOAD_RX_BUFFER_SIZE] = { 0 };

		payload_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union payload_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[PAYLOAD_TX_BUFFER_SIZE] = { 0 };

		payload_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union waypoint_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[WAYPOINT_RX_BUFFER_SIZE] = { 0 };

		waypoint_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union waypoint_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[WAYPOINT_TX_BUFFER_SIZE] = { 0 };

		waypoint_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union telemetry_5hz_type
{
		struct
		{
				int16_t p_ins_pitch_deg = 0;
				int16_t p_ins_yaw_deg = 0;
				int16_t p_ins_gyro_x_dps = 0;
				int16_t p_ins_gyro_y_dps = 0;
				int16_t p_ins_gyro_z_dps = 0;
				int16_t p_ins_free_acc_n_ms2 = 0;
				int16_t p_ins_free_acc_e_ms2 = 0;
				int16_t p_ins_free_acc_u_ms2 = 0;
				int16_t p_ins_temperature_deg = 0;
				int16_t p_ins_altitude_m = 0;
				int16_t p_ins_vel_n_ms = 0;
				int16_t p_ins_vel_e_ms = 0;
				int16_t p_ins_vel_d_ms = 0;
				uint32_t p_ins_status_flag = 0;
				uint32_t p_ins_status_flag_2 = 0;
				float p_ins_gyro_x_bias_dps = 0.0;
				float p_ins_gyro_y_bias_dps = 0.0;
				float p_ins_gyro_z_bias_dps = 0.0;
				float p_ins_acc_x_ms2 = 0.0;
				float p_ins_acc_y_ms2 = 0.0;
				float p_ins_acc_z_ms2 = 0.0;
				float p_ins_free_acc_x_bias_ms2 = 0.0;
				float p_ins_free_acc_y_bias_ms2 = 0.0;
				float p_ins_free_acc_z_bias_ms2 = 0.0;
				float p_ins_mag_x_gauss = 0.0;
				float p_ins_mag_y_gauss = 0.0;
				float p_ins_mag_z_gauss = 0.0;
				float p_ins_barometer_presure_kpa = 0.0;
				float p_ins_barometer_altitude_m = 0.0;
				float p_ins_ex_barometer_altitude_m = 0.0;
				float p_ins_gnss_altitude_m = 0.0;
				int32_t p_ins_gnss_latitude = 0;
				int32_t p_ins_gnss_longitude = 0;
				uint16_t p_ins_gnss_pdop = 0;
				uint32_t p_ins_gnss_hdop = 0;
				uint32_t p_ins_gnss_vdop = 0;
				int32_t p_ins_gnss_vel_n_mms = 0;
				int32_t p_ins_gnss_vel_e_mms = 0;
				int32_t p_ins_gnss_vel_d_mms = 0;
				int32_t p_ins_gnss_g_speed_mms = 0;
				uint32_t p_ins_gnss_speed_acc_mms = 0;
				int32_t p_ins_gnss_head_mot_deg = 0;
				uint32_t p_ins_gnss_head_mot_acc_deg = 0;
				int32_t p_ins_gnss_heading_deg = 0;
				uint8_t p_ins_gnss_fix_status_flag = 0;
				uint8_t mc_id = 0;
				uint8_t mc_product_id = 0;
				uint8_t mc_software_ver = 0;
				uint32_t mc_timestamp = 0;
				float mc_distance_down = 0.0;
				float mc_distance_front = 0.0;
				float mc_distance_left = 0.0;
				float mc_distance_right = 0.0;
				uint32_t mc_status_flag = 0;
				uint32_t ap_runtime_ms = 0;
				int16_t ap_roll_ddeg = 0;
				int16_t ap_pitch_ddeg = 0;
				int16_t ap_yaw_ddeg = 0;
				uint8_t ap_flight_mode = 0;
				uint8_t ap_pid_flight_mode = 0;
				int32_t ap_agl_cm = 0;
				int32_t ap_agl_cmd_cm = 0;
				int32_t target_latitude_ddeg = 0;
				int32_t target_longitude_ddeg = 0;
				int32_t distance_to_home_cm = 0;
				int32_t distance_to_target_cm = 0;
				int16_t ap_vertical_speed_ms = 0;
				int16_t ap_ground_vel_n_ms = 0;
				int16_t ap_ground_vel_e_ms = 0;
				uint8_t wind_speed_ms = 0;
				uint8_t wind_diretion_deg = 0;
				uint16_t average_consumption_dw = 0;
				uint16_t total_consumption_dwh = 0;
				uint8_t m0_pwm_us = 0;
				uint8_t m1_pwm_us = 0;
				uint8_t m2_pwm_us = 0;
				uint8_t m3_pwm_us = 0;
				uint8_t m4_pwm_us = 0;
				uint8_t m5_pwm_us = 0;
				uint8_t m6_pwm_us = 0;
				uint8_t m7_pwm_us = 0;
				uint8_t cpu_load_pct = 0;
				uint8_t loop_freq_100_hz = 0;
				uint16_t max_rth_alt_m = 0;
				uint8_t reduced_speed_ms = 0;
				uint32_t task_counter = 0;
				uint32_t flight_critical_flags = 0;
				uint32_t warning_flags = 0;
				uint32_t system_flags = 0;
				uint32_t system_flags_2 = 0;
				float p_radar_x_distance_m = 0.0;
				float p_radar_y_distance_m = 0.0;
				float p_radar_z_distance_m = 0.0;
				float p_radar_x_vel_mps = 0.0;
				float p_radar_y_vel_mps = 0.0;
				float p_radar_z_vel_mps = 0.0;
				uint32_t reserved = 0;

		} data;

		uint8_t buffer[TELEMETRY_5HZ_TX_BUFFER_SIZE] = { 0 };

		telemetry_5hz_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union telemetry_1hz_type
{
		struct
		{
				uint8_t p_ins_id = 0;
				uint16_t bms_current_ma = 0;
				uint8_t bms_voltage_mv = 0;
				uint8_t bms_soc_pct = 0;
				uint8_t bms_soh_pct = 0;
				uint16_t bms_rem_capacity_mah = 0;
				uint16_t bms_full_charge_capacity_mah = 0;
				uint8_t bms_rem_time_s = 0;
				uint16_t battery_rem_time_s = 0;
				uint16_t rtl_rem_time_s = 0;
				int32_t home_latitude_ddeg = 0;
				int32_t home_longitude_ddeg = 0;
				int16_t ap_take_off_alt_dm = 0;
				int16_t ap_rth_alt_dm = 0;
				uint16_t gcs_take_off_alt_dm = 0;
				uint16_t gcs_rth_alt_dm = 0;
				int16_t bms_pcb_temp_ddeg = 0;
				int16_t bms_battery_temp_ddeg = 0;
				int16_t p_ins_temperature_deg = 0;
				int16_t s_ins_temperature_deg = 0;
				uint8_t ins_gnss_num_sat = 0;
				uint16_t reset_counter = 0;
				uint16_t flight_time = 0;
				uint16_t total_flight_time = 0;
				uint8_t bms_error_cntr = 0;
				uint8_t ins_error_cntr = 0;
				uint8_t rc_receiver_error_cntr = 0;
				uint8_t radar_error_cntr = 0;
				uint8_t max_nav_speed_dmps = 0;
				uint8_t max_ver_speed_dmps = 0;
				uint16_t bms_cycle = 0;
				uint16_t bms_id = 0;
				uint16_t boot_ver = 0;
				uint32_t reserved = 0;

		} data;

		uint8_t buffer[TELEMETRY_1HZ_TX_BUFFER_SIZE] = { 0 };

		telemetry_1hz_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union calibration_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CALIBRATION_RX_BUFFER_SIZE] = { 0 };

		calibration_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union calibration_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CALIBRATION_TX_BUFFER_SIZE] { 0 };

		calibration_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_1_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_1_RX_BUFFER_SIZE] = { 0 };

		control_1_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_1_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_1_TX_BUFFER_SIZE] = { 0 };

		control_1_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_2_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_2_RX_BUFFER_SIZE] = { 0 };

		control_2_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_2_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_2_TX_BUFFER_SIZE] = { 0 };

		control_2_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_3_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_3_RX_BUFFER_SIZE] = { 0 };

		control_3_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_3_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_3_TX_BUFFER_SIZE] = { 0 };

		control_3_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_4_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_4_RX_BUFFER_SIZE] = { 0 };

		control_4_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union control_4_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONTROL_4_TX_BUFFER_SIZE] = { 0 };

		control_4_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union config_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONFIG_RX_BUFFER_SIZE] = { 0 };

		config_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union config_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CONFIG_TX_BUFFER_SIZE] = { 0 };

		config_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union chp_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CHP_RX_BUFFER_SIZE] = { 0 };

		chp_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union chp_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[CHP_TX_BUFFER_SIZE] = { 0 };

		chp_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union nfz_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[NFZ_RX_BUFFER_SIZE] = { 0 };

		nfz_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union nfz_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[NFZ_TX_BUFFER_SIZE] = { 0 };

		nfz_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union boot_receive_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[BOOT_RX_BUFFER_SIZE] = { 0 };

		boot_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union boot_transmit_type
{
		struct
		{
				uint32_t reserved = 0;
		} data;

		uint8_t buffer[BOOT_TX_BUFFER_SIZE] = { 0 };

		boot_transmit_type()
		{
		}
};
#pragma pack()

struct payload_packet_type
{
		uint8_t header = 0;
		uint16_t data_length = 0;
		uint8_t version = 0;
		uint8_t changing_byte = 0;
		uint8_t data[1024] = { 0 };
		uint8_t footer = 0;
};

// End of Enum, Union and Struct Definitions

/*
 * Begin of TELEMETRY Class Definition
 */
class TELEMETRY: public BASE
{
	public:
		explicit TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id, telemetry_id_type destination_id);

		void scheduler(void) override;
		void parseReceivedData(void) override;
		void sendPacket(uint8_t source_id, uint8_t destination_id, const uint8_t data[], uint16_t length);
		void sendPacket(uint16_t command, uint8_t data[], uint16_t length) override;
		void sendPeriodicPacket(void) override;

		TELEMETRY(const TELEMETRY &orig);
		virtual ~TELEMETRY();

	protected:
		void processReceivedPacket(uint16_t command, uint8_t data[], uint16_t length) override;
		void processPayloadPacket(const uint8_t data[], uint16_t size);

		virtual void processReceivedPacket(payload_packet_type const &payload_packet) = 0;
		void preparePayload(uint8_t src_id, uint8_t dest_id, payload_packet_type const &payload_packet);
		const uint8_t HEADER_FOOTER_DIFF = 126;

	private:
		Peripherals::Uart::H747_UART *uart_module = nullptr;
		telemetry_id_type source_id;
		telemetry_id_type destination_id;
		uint16_t parse_error_counter = 0;
		uint16_t payload_error_counter = 0;

};
// End of of TELEMETRY Definition

/*
 * Begin of GCS_TELEMETRY Class Definition
 */
class GCS_TELEMETRY: public TELEMETRY
{
	public:
		explicit GCS_TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id, telemetry_id_type destination_id);

		GCS_TELEMETRY(const GCS_TELEMETRY &orig);
		virtual ~GCS_TELEMETRY();

	protected:
		void processReceivedPacket(payload_packet_type const &payload_packet) override;

	private:

		const uint8_t HEARTBEAT_VERSION = 0;
		const uint8_t VEHICLE_VERSION = 0;
		const uint8_t PAYLOAD_VERSION = 0;
		const uint8_t WAYPOINT_VERSION = 0;
		const uint8_t TELEMETRY_5HZ_VERSION = 0;
		const uint8_t TELEMETRY_1HZ_VERSION = 0;
		const uint8_t CALIBRATION_VERSION = 0;
		const uint8_t CONTROL_VERSION = 0;
		const uint8_t CONFIG_VERSION = 0;
		const uint8_t CHP_VERSION = 0;
		const uint8_t NFZ_VERSION = 0;
		const uint8_t BOOT_VERSION = 0;

		void processHeartBeatMessage(payload_packet_type const &payload_packet);
		void processVehicleMessage(payload_packet_type const &payload_packet);
		void processPayloadMessage(payload_packet_type const &payload_packet);
		void processWaypointMessage(payload_packet_type const &payload_packet);
		void processCalibrationMessage(payload_packet_type const &payload_packet);
		void processControlMessage(payload_packet_type const &payload_packet, control_message_type type);
		void processConfigMessage(payload_packet_type const &payload_packet);
		void processCHPMessage(payload_packet_type const &payload_packet);
		void processNFZMessage(payload_packet_type const &payload_packet);
		void processBootMessage(payload_packet_type const &payload_packet);

		void sendHeartBeatMessage(void);
		void sendVehicleMessage(void);
		void sendPayloadMessage(void);
		void sendWaypointMessage(void);
		void sendTelemetry5HzMessage(void);
		void sendTelemetry1HzMessage(void);
		void sendCalibrationMessage(void);
		void sendControlMessage(control_message_type type);
		void sendConfigMessage(void);
		void sendCHPMessage(void);
		void sendNFZMessage(void);
		void sendBootMessage(void);

};
// End of of GCS_TELEMETRY Definition

} /* End of namespace Telemetry */

/*
 * External Linkages
 */
extern Telemetry::GCS_TELEMETRY gcs_telemetry;
// End of External Linkages

#endif /* DS_TELEMETRY_GCS_HPP */
