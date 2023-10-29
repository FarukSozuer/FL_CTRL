/**
 ******************************************************************************
  * @file		: ds_gnss.hpp
  * @brief		: GNSS Interface Class
  *				  This file contains vn100 interface class
  * @author		: Faruk Sozuer
  * @date		  : 05.03.2020
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

#ifndef DS_GNSS_HPP_
#define DS_GNSS_HPP_

#include <stdint.h>
#include "ds_uart_h747.hpp"

namespace Sensors
{
namespace Gnss
{

const uint8_t PREAMBLE1				   = 0xB5;
const uint8_t PREAMBLE2 			   = 0x62;
const uint8_t UBX_NAV_CLASS 		   = 0x01;
const uint8_t UBX_NAV_HPPOSLLH 		   = 0x14;
const uint8_t UBX_NAV_PVT 			   = 0x07;
const uint8_t UBX_NAV_RELPOSNED 	   = 0x3C;
const uint8_t UBX_NAV_RELPOSNED_LENGTH = 64;
const uint8_t UBX_NAV_PVT_LENGTH 	   = 92;
const uint8_t UBX_NAV_HPPOSLLH_LENGTH  = 36;



/*
 * @brief Initialize function return message parse state.
 */
enum class zedf9p_parse_status_type : uint8_t
{
	PREAMBLE1   	= 0x00,
	PREAMBLE2    	= 0x01,
	CLASS 			= 0x02,
	MSG_ID		   	= 0x03,
	PAYLOAD_LENGTH 	= 0x04,
	CHECKSUM        = 0x05,
	PARSE			= 0x06,
};

enum class zedf9_parse_message_type:uint8_t
{
	UBX_NAV_HPPOSLLH  = 0x00,
	UBX_NAV_PVT		  = 0x01,
	UBX_NAV_RELPOSNED = 0x02,
	NULL_VALUE  	  = 0x03,
};




/*
 * @brief High precision geodetic position solution UBX-18010854 - R08 Page 128
 */
#pragma pack(1)
	union high_preision_geodic_pos_type
	{
		struct
		{
			uint8_t  version;	  ///< Message version (0x00 for this version)
			uint8_t  reserved;  ///< Reserved
			uint8_t  flags;	    ///< Additional flags
			uint32_t iTOW_3;	    ///< GPS time of week of the navigation epoch.See the section iTOW timestamps in Integration manual for details.
			int32_t  lon;		    ///< Longitude
			int32_t  lat;		    ///< Latitude
			int32_t  height;    ///< Height above ellipsoid.
			int32_t  hMSL;	    ///< Height above mean sea level
			int8_t   lonHp;	    ///< High precision component of longitude. Must be in the range -99..+99. Precise longitude in deg * 1e-7 = lon + (lonHp * 1e-2).
			int8_t   latHp;		  ///< High precision component of latitude. Must be in the range -99..+99. Precise latitude in deg * 1e-7 = lat + (latHp * 1e-2).
			int8_t   heightHp;	///< High precision component of height above ellipsoid. Must be in the range -9..+9. Precise height in mm = height + (heightHp * 0.1).
			int8_t   hMSLHp;		///< High precision component of height above mean sea level. Must be in range -9..+9. Precise height in mm = hMSL + (hMSLHp * 0.1)
			uint32_t hAcc;		  ///< Horizontal accuracy estimate
			uint32_t vAcc;		  ///< Vertical accuracy estimate
		}bits;
		uint8_t buffer[35];
		high_preision_geodic_pos_type() : buffer{} {}
	};
#pragma pack()



/*
 * @brief Navigation position velocity time solution UBX-18010854 - R08 Page 132
 */
#pragma pack(1)
	union navigation_position_velocity_time_type
	{
			struct
			{
					uint32_t iTOW_1; 		    ///< ms GPS time of week of the navigation epoch. See the section iTOW timestamps in Integration manual for details.
					uint16_t year; 		    ///< y Year (UTC)
					uint8_t  month; 		  ///< month Month, range 1..12 (UTC)
					uint8_t  day;   		  ///< d Day of month, range 1..31 (UTC)
					uint8_t  hour;  		  ///< h Hour of day, range 0..23 (UTC)
					uint8_t  min;   		  ///< min Minute of hour, range 0..59 (UTC)
					uint8_t  sec;   		  ///< s Seconds of minute, range 0..60 (UTC)
					uint8_t  valid; 		  ///< Validity flags
 					uint32_t tAcc; 		    ///< ns Time accuracy estimate (UTC)
					int32_t  nano;  	    ///< ns Fraction of second, range -1e9 .. 1e9 (UTC)
					uint8_t  fixType;     ///< GNSSfix Type
					uint8_t  flags;		    ///< Fix status flags
					uint8_t  flags2;		  ///< Additional flags
					uint8_t  numsV;		    ///< Number of satellites used in Nav Solution
					int32_t  lon;			    ///< deg Longitude
					int32_t  lat;			    ///< deg Latitude
					int32_t  height;		  ///< mm Height above ellipsoid
					int32_t  hMSL;			  ///< mm Height above mean sea level
					uint32_t hAcc;		    ///< mm Horizontal accuracy estimate
					uint32_t vAcc;		    ///< mm Vertical accuracy estimate
					int32_t  velN;			  ///< mm/s NED north velocity
					int32_t  velE;			  ///< mm/s NED east velocity
					int32_t  velD;			  ///< mm/s NED down velocity
					int32_t  gSpeed;		  ///< mm/s Ground Speed (2-D)
					int32_t  headMot;	    ///< deg Heading of motion (2-D)
					uint32_t sAcc;        ///< mm/s Speed accuracy estimate
					uint32_t headAcc;     ///< deg Heading accuracy estimate (both motion and vehicle)
					uint16_t pDOP;	      ///< Position DOP
					uint8_t  flags3;	  	///< Additional flags
					uint8_t  reserved0;   ///<
					int32_t  headVeh;     ///< deg Heading of vehicle (2-D), this is only valid when headVehValid is set, otherwise the output is set to the heading of motion
					int16_t  magDec;		  ///< deg Magnetic declination. Only supported in ADR 4.10 and later.
					uint16_t magAcc;	    ///< deg Magnetic declination accuracy. Only supported in ADR 4.10 and later.
			}bits;
			uint8_t buffer[88];
			navigation_position_velocity_time_type(): buffer{} {}
	};



/*
 * @brief Navigation position velocity time solution UBX-18010854 - R08 Page 134
 */
#pragma pack(1)
	union relative_pos_information_type
	{
			struct
			{
						uint8_t  version; 			 ///< Message version (0x01 for this version)
						uint8_t  reserved;			 ///<
						uint16_t refStationId;   ///< Reference station ID. Must be in the range 0..4095.
						uint32_t iTOW_2;				   ///< ms GPS time of week of the navigation epoch. See the section iTOW timestamps in Integration manual for details.
						int32_t  relPosN;			   ///< cm North component of relative position vector
						int32_t  relPosE;			   ///< cm East component of relative position vector
						int32_t  relPosD;			   ///< cm Down component of relative position vector
						int32_t  relPosLength;	 ///< cm Length of the relative position vector
						int32_t  relPosHeading;  ///< deg Heading of the relative position vector
						uint32_t reserved1;		   ///<
						int8_t   relPosHPN;			 ///< mm High-precision North component of relative position vector. Must be in the range -99 to +99.
						int8_t   relPosHPE;			 ///< mm High-precision East component of relative position vector. Must be in the range -99 to +99.
						int8_t   relPosHPD;			 ///< mm High-precision Down component of relative position vector. Must be in the range -99 to +99.
						int8_t   relPosHPLength; ///< mm High-precision component of the length of the relative position vector. Must be in the range -99 to +99.
						uint32_t accN;				   ///< mm Accuracy of relative position North component
						uint32_t accE;				   ///< mm Accuracy of relative position East component
						uint32_t accD;				   ///< mm Accuracy of relative position Down component
						uint32_t accLength;		   ///< mm Accuracy of length of the relative position vector
						uint32_t accHeading;	   ///< deg Accuracy of heading of the relative position vector
						uint32_t reserved2;		   ///<
						uint32_t flags;				   ///<
			}bits;
			uint8_t buffer[64];
			relative_pos_information_type(): buffer{} {}
	};


class ZEDF9P
{
	public:

	 explicit ZEDF9P(Peripherals::Uart::H747_UART *uart_module);

	 Peripherals::Uart::H747_UART *uart = {0};

		void getByte(void);

		ZEDF9P(const ZEDF9P& orig);
		virtual ~ZEDF9P();

		high_preision_geodic_pos_type 					high_preision_geodic_pos;
		navigation_position_velocity_time_type  navigation_position_velocity_time;
		relative_pos_information_type 					relative_pos_information;

	private:

		void getData(uint8_t buffer[],uint16_t size);
		uint16_t calculateCRC(uint8_t data[],uint16_t length);

		uint16_t calculated_crc 			= 0;	///< The variable where the crc value of the related package is kept.
		uint16_t incoming_crc 				= 0;	///< CRC information from the sensor.
		zedf9_parse_message_type parse_message_type = zedf9_parse_message_type::NULL_VALUE;
		uint8_t payload_length = 0;
};
}
}

extern Sensors::Gnss::ZEDF9P gnss;

#endif /* DS_GNSS_HPP_ */
