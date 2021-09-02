#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

typedef   signed char i8;
typedef unsigned char u8;

typedef   signed short i16;
typedef unsigned short u16;

typedef   signed int i32;
typedef unsigned int u32;

typedef   signed long long i64;
typedef unsigned long long u64;

typedef unsigned long long  ptr_t;

typedef const char *string_t;

typedef unsigned long int color_t;

// has to be an int
#ifndef JGSETTING_UNIT_INT_TYPE
#    define JGSETTING_UNIT_INT_TYPE int
#endif // JGSETTING_UNIT_INT_TYPE

typedef JGSETTING_UNIT_INT_TYPE unit_t;

// has to be a float (float/double/long double)
#ifndef JGSETTING_UNIT_FLOAT_TYPE
#    define JGSETTING_UNIT_FLOAT_TYPE float
#endif // JGSETTING_UNIT_FLOAT_TYPE

typedef JGSETTING_UNIT_FLOAT_TYPE unitf_t;

#define CLP(a) const a*

#endif // __TYPEDEFS_H__
