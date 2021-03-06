
#ifndef UTILS_H_
#define UTILS_H_

/*****************************************************************************
 * quack_utils.h:
 *  Some utilities for a better code.
 * 
*****************************************************************************/

#include "quack_config.h"

#ifndef ARDUINO_ENABLED
#define pgm_read_byte(address_short) *(address_short)
#endif

#define DEBUGGING_PRINTF(...) Serial.printf(__VA_ARGS__)
#define DEBUGGING_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUGGING_PRINTSTR(str, len) for(u16 i = 0; i < (len); i++) Serial.printf("%d ", (str)[i]);

#define PACKED __attribute__ ((packed))

#define DESERIALIZE_U16(ptr) ((((ptr)[0] << 8)) | ((ptr)[1]))

#define STR_TO_U16(str) *((u16*) (str))
#define STR_TO_I16(str) *((i16*) (str))
#define STR_TO_U32(str) *((u32*) (str))

typedef __INT8_TYPE__  i8;
typedef __INT16_TYPE__ i16;
typedef __INT32_TYPE__ i32;
typedef __INT64_TYPE__ i64;

typedef __UINT8_TYPE__ u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

#define OR  ||
#define AND &&
#define NOT !

#endif