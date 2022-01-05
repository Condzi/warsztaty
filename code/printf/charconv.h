#ifndef CHARCONV_H
#define CHARCONV_H

#include <inttypes.h>
#include <float.h>

enum {
	// -2147483648 => 11 characters
	MAX_INT32_LENGTH = 11,
	// sign + decimal + . + mantissa
	MAX_F32_LENGTH = 1 + FLT_DECIMAL_DIG + 1 + FLT_DIG
};

/*
 * Converts a 32 bit integer to a string.
 *
 * @param[in] value
 * @param[in][out] buffer
 * @return number of characters written to buffer, 0 on error.
*/
int int_to_string(int32_t value, char* buffer);

/*
 * Converts a 32 bit floating point number to a string.
 *
 * @param[in] value
 * @param[in][out] buffer
 * @return number of characters written to buffer, 0 on error.
*/
int float_to_string(float value, char* buffer);

#endif // CHARCONV_H