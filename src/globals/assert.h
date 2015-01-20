#ifndef GLOBALS_ASSERT_H
#define GLOBALS_ASSERT_H

// Using Assert Library by Gregory Pakosz
// https://github.com/gpakosz/Assert

#define PPK_ASSERT_ENABLED 1

#define PPK_ASSERT_DEFAULT_LEVEL Error

#include "pempek_assert.h"

// custom names
#define assert_warning 			PPK_ASSERT_WARNING
#define assert_debug 			PPK_ASSERT_DEBUG
#define assert_error 			PPK_ASSERT_ERROR
#define assert_fatal 			PPK_ASSERT_FATAL

// not using
// #define ASSERT 					PPK_ASSERT
// #define ASSERT_CUSTOM 			PPK_ASSERT_CUSTOM
// #define ASSERT_USED 			PPK_ASSERT_USED
// #define ASSERT_USED_WARNING 	PPK_ASSERT_USED_WARNING
// #define ASSERT_USED_DEBUG 		PPK_ASSERT_USED_DEBUG
// #define ASSERT_USED_ERROR 		PPK_ASSERT_USED_ERROR
// #define ASSERT_USED_FATAL 		PPK_ASSERT_USED_FATAL
// #define ASSERT_USED_CUSTOM 		PPK_ASSERT_USED_CUSTOM

void initAssertHandler();

#endif // GLOBALS_ASSERT_H

