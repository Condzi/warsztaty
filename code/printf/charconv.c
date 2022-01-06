#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "charconv.h"
#include "report_error.h"

int int_to_string(int32_t value, char* buffer) {
	assert(buffer);

	const bool has_sign = value < 0;

	// To handle INT_MIN
	const uint32_t absolute = (uint32_t)llabs((int64_t)value);

	size_t num_of_digits = 0;
	uint32_t temp = absolute;
	do {
		temp /= 10;
		num_of_digits++;
	} while (temp != 0);

	// sanity check
	assert(num_of_digits + (int)has_sign <= MAX_INT32_LENGTH);
	if (num_of_digits + (int)has_sign > MAX_INT32_LENGTH) {
		REPORT_ERROR("num_of_digits + (int)has_sign > MAX_INT32_LENGTH");
		return 0;
	}

	size_t chars_to_write = num_of_digits + (int)has_sign;
	temp = absolute;
	for (size_t i = chars_to_write - 1; i < chars_to_write; i--) {
		buffer[i] = temp%10 + '0';
		temp /= 10;
	}

	if (has_sign) {
		buffer[0] = '-';
	}

	buffer[chars_to_write] = '\0';

	return chars_to_write;
}

int float_to_string(float value, char* buffer) {
	assert(buffer);
	
	const int32_t exponent = (int32_t)value;
	const int32_t mantissa = abs((int32_t)((value - (float)exponent)*powf(10, FLT_DIG)));

	int chars_written = 0;

	int result = int_to_string(exponent, buffer);
	if (!result) {
		REPORT_ERROR("Failed to convert exponent to string");
		return 0;
	}

	chars_written += result;
	buffer += result;
	*buffer = '.';
	buffer++;
	chars_written++;

	result = int_to_string(mantissa, buffer);
	if (!result) {
		REPORT_ERROR("Failed to convert mantissa to string");
		return 0;
	}

	chars_written += result;
	assert(chars_written <= MAX_F32_LENGTH);
	if (chars_written > MAX_F32_LENGTH) {
		REPORT_ERROR("chars_written > MAX_F32_LENGTH");
		return 0;
	}

	return chars_written;
}