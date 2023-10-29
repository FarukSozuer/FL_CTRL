/**
******************************************************************************
  * @file		: ds_parse.cpp
  * @brief		: PARSE MESSAGE Interface Class
  *				  This file contains telemetry interface class
  * @author		: Faruk Sozuer
  * @date		: 10.02.2020
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

#include "ds_parse.hpp"

#define _LITTLE_ENDIAN_

AppLayer::Parse::PACKET     packet;

namespace AppLayer
{
namespace Parse
{

/**
  * @brief Default constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PACKET::PACKET()
{

}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_uint8(const uint8_t  *value_u8,  uint8_t *buffer, uint16_t *index)
{
	buffer[ *index ] = ( (uint8_t *) value_u8 )[ 0 ];
	*index = *index + 1;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_int8  (const int8_t   *value_s8,  uint8_t *buffer, uint16_t *index)
{
	buffer[ *index ] = ( (uint8_t *) value_s8 )[ 0 ];
	*index = *index + 1;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_uint16(const uint16_t *value_u16, uint8_t *buffer, uint16_t  *index)
{
		#ifdef _LITTLE_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_u16 )[ 0 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_u16 )[ 1 ];
		#endif
		#ifdef _BIG_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_u16 )[ 1 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_u16 )[ 0 ];
		#endif

		*index = *index + 2;
}


/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_int16 (const int16_t  *value_s16, uint8_t *buffer, uint16_t  *index)
{
		#ifdef _LITTLE_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_s16 )[ 0 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_s16 )[ 1 ];
		#endif
		#ifdef _BIG_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_s16 )[ 1 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_s16 )[ 0 ];
		#endif

		*index = *index + 2;
}


/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_uint32(const uint32_t *value_u32, uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_u32 )[ 0 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_u32 )[ 1 ];
		buffer[ *index + 2 ] = ( (uint8_t *) value_u32 )[ 2 ];
		buffer[ *index + 3 ] = ( (uint8_t *) value_u32 )[ 3 ];
		#endif
		#ifdef _BIG_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_u32 )[ 3 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_u32 )[ 2 ];
		buffer[ *index + 2 ] = ( (uint8_t *) value_u32 )[ 1 ];
		buffer[ *index + 3 ] = ( (uint8_t *) value_u32 )[ 0 ];
		#endif

		*index = *index + 4;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_int32(const int32_t  *value_u32, uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_u32 )[ 0 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_u32 )[ 1 ];
		buffer[ *index + 2 ] = ( (uint8_t *) value_u32 )[ 2 ];
		buffer[ *index + 3 ] = ( (uint8_t *) value_u32 )[ 3 ];
		#endif
		#ifdef _BIG_ENDIAN_
		veri[ *index     ] = ( (uint8_t *) value_u32 )[ 3 ];
		veri[ *index + 1 ] = ( (uint8_t *) value_u32 )[ 2 ];
		veri[ *index + 2 ] = ( (uint8_t *) value_u32 )[ 1 ];
		veri[ *index + 3 ] = ( (uint8_t *) value_u32 )[ 0 ];
		#endif

		*index = *index + 4;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[in]
  * @param[out]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::parse_float (const float    *value_f32, uint8_t *buffer, uint16_t  *index)
{
		#ifdef _LITTLE_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_f32 )[ 0 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_f32 )[ 1 ];
		buffer[ *index + 2 ] = ( (uint8_t *) value_f32 )[ 2 ];
		buffer[ *index + 3 ] = ( (uint8_t *) value_f32 )[ 3 ];
		#endif
		#ifdef _BIG_ENDIAN_
		buffer[ *index     ] = ( (uint8_t *) value_f32 )[ 3 ];
		buffer[ *index + 1 ] = ( (uint8_t *) value_f32 )[ 2 ];
		buffer[ *index + 2 ] = ( (uint8_t *) value_f32 )[ 1 ];
		buffer[ *index + 3 ] = ( (uint8_t *) value_f32 )[ 0 ];
		#endif

		*index = *index + 4;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_uint8 (uint8_t  *value_u8,  const uint8_t *buffer, uint16_t *index)
{
	( (uint8_t *) value_u8 )[ 0 ] = buffer[*index];
	*index = *index + 1;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_int8  (int8_t   *value_s8,  const uint8_t *buffer, uint16_t *index)
{
	( (uint8_t *) value_s8 )[ 0 ] = buffer[ *index ];
	*index = *index + 1;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_uint16(uint16_t *value_u16, const uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		( (uint8_t *) value_u16 )[ 0 ] = buffer[ *index     ];
		( (uint8_t *) value_u16 )[ 1 ] = buffer[ *index + 1 ];
		#endif
		#ifdef _BIG_ENDIAN_
		( (uint8_t *) value_u16 )[ 1 ] = buffer[ *index     ];
		( (uint8_t *) value_u16 )[ 0 ] = buffer[ *index + 1 ];
		#endif

		*index = *index + 2;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_int16 (int16_t  *value_s16, const uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		( (uint8_t *) value_s16 )[ 0 ] = buffer[ *index     ];
		( (uint8_t *) value_s16 )[ 1 ] = buffer[ *index + 1 ];
		#endif
		#ifdef _BIG_ENDIAN_
		( (uint8_t *) value_s16 )[ 1 ] = buffer[ *index     ];
		( (uint8_t *) value_s16 )[ 0 ] = buffer[ *index + 1 ];
		#endif

		*index = *index + 2;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void add_int24( int32_t *value_s32, const uint8_t *buffer, uint16_t *index )
{
	#ifdef _LITTLE_ENDIAN_
	( (uint8_t *) value_s32 )[ 0 ] = buffer[ *index     ];
	( (uint8_t *) value_s32 )[ 1 ] = buffer[ *index + 1 ];
  ( (uint8_t *) value_s32 )[ 2 ] = buffer[ *index + 2 ];
	#endif
	#ifdef _BIG_ENDIAN_
  ( (uint8_t *) value_s32 )[ 2 ] = buffer[ *index     ];
	( (uint8_t *) value_s32 )[ 1 ] = buffer[ *index + 1 ];
	( (uint8_t *) value_s32 )[ 0 ] = buffer[ *index + 2 ];
	#endif

	*index = *index + 3;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_uint32(uint32_t *value_u32, const uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		( (uint8_t *) value_u32 )[ 0 ] = buffer[ *index     ];
		( (uint8_t *) value_u32 )[ 1 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_u32 )[ 2 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_u32 )[ 3 ] = buffer[ *index + 3 ];
		#endif
		#ifdef _BIG_ENDIAN_
		( (uint8_t *) value_u32 )[ 3 ] = buffer[ *index     ];
		( (uint8_t *) value_u32 )[ 2 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_u32 )[ 1 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_u32 )[ 0 ] = buffer[ *index + 3 ];
		#endif

		*index = *index + 4;
}

/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_int32 (int32_t  *value_s32, const uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		( (uint8_t *) value_s32 )[ 0 ] = buffer[ *index     ];
		( (uint8_t *) value_s32 )[ 1 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_s32 )[ 2 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_s32 )[ 3 ] = buffer[ *index + 3 ];
		#endif
		#ifdef _BIG_ENDIAN_
		( (uint8_t *) value_s32 )[ 3 ] = buffer[ *index     ];
		( (uint8_t *) value_s32 )[ 2 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_s32 )[ 1 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_s32 )[ 0 ] = buffer[ *index + 3 ];
		#endif

		*index = *index + 4;
}

void PACKET::add_uint64 (uint64_t *value_u64,const uint8_t *buffer,uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		( (uint8_t *) value_u64 )[ 0 ] = buffer[ *index     ];
		( (uint8_t *) value_u64 )[ 1 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_u64 )[ 2 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_u64 )[ 3 ] = buffer[ *index + 3 ];
		( (uint8_t *) value_u64 )[ 4 ] = buffer[ *index + 4 ];
		( (uint8_t *) value_u64 )[ 5 ] = buffer[ *index + 5 ];
		( (uint8_t *) value_u64 )[ 6 ] = buffer[ *index + 6 ];
		( (uint8_t *) value_u64 )[ 7 ] = buffer[ *index + 7 ];
		#endif
		#ifdef _BIG_ENDIAN_
		( (uint8_t *) value_u64 )[ 7 ] = buffer[ *index     ];
		( (uint8_t *) value_u64 )[ 6 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_u64 )[ 5 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_u64 )[ 4 ] = buffer[ *index + 3 ];
		( (uint8_t *) value_u64 )[ 3 ] = buffer[ *index + 4 ];
		( (uint8_t *) value_u64 )[ 2 ] = buffer[ *index + 5 ];
		( (uint8_t *) value_u64 )[ 1 ] = buffer[ *index + 6 ];
		( (uint8_t *) value_u64 )[ 0] =  buffer[ *index + 7 ];
		#endif

		*index = *index + 8;
}
/**
  * @brief 		Function for the installation of the corresponding uart line.
  * @param[out]
  * @param[in]
  * @param[in]
  * @return 	void	Nothing
  * Example:
  * @code
  * uart1.initialize();
  * @endcode
  */
void PACKET::add_float (float    *value_f32, const uint8_t *buffer, uint16_t *index)
{
		#ifdef _LITTLE_ENDIAN_
		( (uint8_t *) value_f32 )[ 0 ] = buffer[ *index     ];
		( (uint8_t *) value_f32 )[ 1 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_f32 )[ 2 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_f32 )[ 3 ] = buffer[ *index + 3 ];
		#endif
		#ifdef _BIG_ENDIAN_
		( (uint8_t *) value_f32 )[ 3 ] = buffer[ *index     ];
		( (uint8_t *) value_f32 )[ 2 ] = buffer[ *index + 1 ];
		( (uint8_t *) value_f32 )[ 1 ] = buffer[ *index + 2 ];
		( (uint8_t *) value_f32 )[ 0 ] = buffer[ *index + 3 ];
		#endif

		*index = *index + 4;
}

/**
  * @brief Default copy constructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PACKET::PACKET(const PACKET& orig)
{

}



/**
  * @brief Default destructor
  *
  * @param[in]  void
  *
  * @return 	void
  */
PACKET::~PACKET()
{

}


}
}

