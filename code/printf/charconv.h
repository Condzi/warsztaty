#ifndef CHARCONV_H
#define CHARCONV_H

#include <inttypes.h>
#include <float.h>

enum {
	// Maximum length of a string which represents 32-bit integer.
	// -2147483648 => 11 characters
	MAX_INT32_LENGTH = 11,
	// Maximum length of a string which represents 32-bit floating point number.
	// FLT_MAX length
	MAX_F32_LENGTH = 40
};

/*
 * Converts a 32 bit integer to a string.
 *
 * @param[in] value
 * @param[in][out] buffer - buffer where the value shall be written. Must be at least MAX_INT32_LENGTH + 1 (for \0)
 * @return number of characters written to buffer, 0 on error.
*/
int int_to_string(int32_t value, char* buffer);

/*
 * Converts a 32 bit floating point number to a string.
 *
 * @param[in] value - number to convert
 * @param[in][out] buffer - buffer where the value shall be written. Must be at least MAX_F32_LENGTH + 1 (for \0)
 * @return number of characters written to buffer, 0 on error.
*/
int float_to_string(float value, char* buffer);

#endif // CHARCONV_H