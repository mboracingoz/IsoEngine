#ifndef KHR_KHRPLATFORM_H_
#define KHR_KHRPLATFORM_H_

#include <stdint.h>

typedef int8_t khronos_int8_t;
typedef uint8_t khronos_uint8_t;
typedef int16_t khronos_int16_t;
typedef uint16_t khronos_uint16_t;
typedef int32_t khronos_int32_t;
typedef uint32_t khronos_uint32_t;
typedef int64_t khronos_int64_t;
typedef uint64_t khronos_uint64_t;
typedef intptr_t khronos_intptr_t;
typedef uintptr_t khronos_uintptr_t;
typedef intptr_t khronos_ssize_t;
typedef uintptr_t khronos_usize_t;
typedef float khronos_float_t;
typedef uint64_t khronos_utime_nanoseconds_t;
typedef int64_t khronos_stime_nanoseconds_t;
typedef uint64_t khronos_time_ns_t;
typedef unsigned int khronos_boolean_enum_t;

#define KHRONOS_FALSE 0
#define KHRONOS_TRUE 1
#define KHRONOS_SUPPORT_INT64 1
#define KHRONOS_SUPPORT_FLOAT 1

#endif
