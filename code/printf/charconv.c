#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "charconv.h"
#include "report_error.h"

int int_to_string(int32_t value, char* buffer) {
	assert(buffer);

	const bool has_sign = value < 0;
	const uint32_t absolute = value*(has_sign?(-1):(1));

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

	const size_t chars_to_write = num_of_digits + (int)has_sign;
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
	

	float integer_f;
	const int32_t fraction = abs((int32_t)(modff(value, &integer_f)*powf(10, FLT_DIG)));
	const int32_t integer = (int32_t)(integer_f);

	int chars_written = 0;

	int result = int_to_string(integer, buffer);
	if (!result) {
		REPORT_ERROR("Failed to convert integer part to string");
		return 0;
	}

	chars_written += result;
	buffer += result;
	*buffer = '.';
	buffer++;
	chars_written++;

	assert(chars_written + MAX_INT32_LENGTH <= MAX_F32_LENGTH);
	if (chars_written + MAX_INT32_LENGTH > MAX_F32_LENGTH) {
		REPORT_ERROR("chars_written > MAX_F32_LENGTH");
		return 0;
	}

	result = int_to_string(fraction, buffer);
	if (!result) {
		REPORT_ERROR("Failed to convert fraction part to string");
		return 0;
	}

	chars_written += result;
	return chars_written;
}