/**
 ******************************************************************************
 * @file		: ds_telemetry_ins.hpp
 * @brief	  : INS Telemetry Class
 * @author	: Gurtac Kadem
 * 					  This file contains ins telemetry class
 * @date		: 04.06.2020
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

#ifndef DS_TELEMETRY_INS_HPP
#define DS_TELEMETRY_INS_HPP

#include "ds_telemetry_gcs.hpp"

/*
 * Begin of Macro Definitions
 */
const uint16_t INS_TX_BUFFER_SIZE = 15;
const uint16_t INS_RX_BUFFER_SIZE = 96;
const uint16_t INS_RX_EXT_BUFFER_SIZE = 115;

//End of Macro Definitions

namespace Telemetry
{

/*
 * Begin of Enum, Union and Struct Definitions
 */
enum class ins_receive_headers_type : uint8_t
{
	RX_INS = 1,
	RX_INS_EXTENDED = 3,
};
enum class ins_transmit_headers_type : uint8_t
{
	TX_INS = 2,
};

#pragma pack(1)
union ins_receive_type
{
		struct
		{
				uint8_t id = 0;
				uint8_t product_id = 0;
				uint8_t software_ver = 0;
				uint32_t imu_timestemp = 0;
				uint32_t runtime = 0;
				uint32_t gnss_utc = 0;
				uint32_t gnss_time_of_week = 0;
				float roll_deg = 0.0;
				float pitch_deg = 0.0;
				float yaw_deg = 0.0;
				float gyro_x_dps = 0.0;
				float gyro_y_dps = 0.0;
				float gyro_z_dps = 0.0;
				float free_acc_n_ms2 = 0.0;
				float free_acc_e_ms2 = 0.0;
				float free_acc_u_ms2 = 0.0;
				float temperature_deg = 0.0;
				float altitude_m = 0.0;
				int32_t gnss_latitude = 0;
				int32_t gnss_longitude = 0;
				float vel_n_ms = 0.0;
				float vel_e_ms = 0.0;
				float vel_d_ms = 0.0;
				int32_t gnss_heading_deg = 0;
				uint8_t gnss_sat_num = 0;
				uint32_t status_flag = 0;
				uint32_t status_flag_2 = 0;

		} data;

		uint8_t buffer[INS_RX_BUFFER_SIZE] = { 0 };

		ins_receive_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union ins_transmit_type
{
		struct
		{
				uint8_t id = 0;
				uint32_t timestamp = 0;
				uint8_t master_id = 0;
				uint32_t command_flag = 0;
				uint8_t flight_mode = 0;
				uint32_t reserved = 0;

		} data;

		uint8_t buffer[INS_TX_BUFFER_SIZE] = { 0 };

		ins_transmit_type()
		{
		}
};
#pragma pack()

#pragma pack(1)
union ins_ext_receive_type
{
		struct
		{
				float gyro_x_bias_dps = 0.0;
				float gyro_y_bias_dps = 0.0;
				float gyro_z_bias_dps = 0.0;
				float acc_x_ms2 = 0.0;
				float acc_y_ms2 = 0.0;
				float acc_z_ms2 = 0.0;
				float free_acc_x_bias_ms2 = 0.0;
				float free_acc_y_bias_ms2 = 0.0;
				float free_acc_z_bias_ms2 = 0.0;
				float mag_x_gauss = 0.0;
				float mag_y_gauss = 0.0;
				float mag_z_gauss = 0.0;
				float barometer_presure_kpa = 0.0;
				float barometer_altitude_m = 0.0;
				float ex_barometer_altitude_m = 0.0;
				float gnss_altitude_m = 0.0;
				int32_t gnss_latitude = 0;
				int32_t gnss_longitude = 0;
				uint16_t gnss_pdop = 0;
				uint32_t gnss_hdop = 0;
				uint32_t gnss_vdop = 0;
				int32_t gnss_vel_n_mms = 0;
				int32_t gnss_vel_e_mms = 0;
				int32_t gnss_vel_d_mms = 0;
				int32_t gnss_g_speed_mms = 0;
				uint32_t gnss_speed_acc_mms = 0;
				int32_t gnss_head_mot_deg = 0;
				uint32_t gnss_head_mot_acc_deg = 0;
				int32_t gnss_heading_deg = 0;
				uint8_t gnss_fix_status_flag = 0;

		} data;

		uint8_t buffer[INS_RX_EXT_BUFFER_SIZE] = { 0 };

		ins_ext_receive_type()
		{
		}
};
#pragma pack()

// End of Enum, Union and Struct Definitions

/*
 * Begin of INS_TELEMETRY Class Definition
 */

class INS_TELEMETRY: public TELEMETRY
{
	public:
		INS_TELEMETRY(Peripherals::Uart::H747_UART *uart_module, telemetry_id_type source_id, telemetry_id_type destination_id);

		virtual ~INS_TELEMETRY();
		INS_TELEMETRY(const INS_TELEMETRY &other);

	protected:
		void processReceivedPacket(payload_packet_type const &payload_packet) override;

	private:
		const uint8_t INS_VERSION = 0;

		void sendMessage(void);
};

} /* End of namespace Telemetry */

/*
 * External Linkages
 */
extern Telemetry::INS_TELEMETRY ins_telemetry;
// End of External Linkages

#endif /* DS_TELEMETRY_INS_HPP */
