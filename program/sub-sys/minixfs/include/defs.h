#ifndef __DEFS_H__
#define __DEFS_H__

typedef unsigned char   uint8_t;
typedef char            int8_t;
typedef unsigned short  uint16_t;
typedef short           int16_t;
typedef unsigned long   uint32_t;
typedef long            int32_t;
typedef unsigned long long uint64_t;
typedef long long       int64_t;

typedef uint32_t        size_t;

typedef long            time_t;
typedef int             error_t;
#define NO_ERROR    (0)

#define BYTE1(val) ((val) & 0xFF)
#define BYTE2(val) ((val) >> 8 & 0xFF)
#define BYTE3(val) ((val) >> 16 & 0xFF)
#define BYTE4(val) ((val) >> 24 & 0xFF)

#define NULL    0

#endif // __DEFS_H__