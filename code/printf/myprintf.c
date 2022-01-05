#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include "charconv.h"
#include "myprintf.h"
#include "report_error.h"

bool is_format(const char* restrict format, char c) {
	assert(format);
	if (format[0] != '%') {
		return false;
	}

	return format[0] == '%' && format[1] == c;
}

size_t count_number_of_arguments(const char* format) {
	assert(format);

	size_t counted_args = 0;
	const char* it = format;
	while (*it) {
		if (*it != '%') {
			it++;
			continue;
		}

		// Ignore '%%'
		if (is_format(it, 'c') || 
			is_format(it, 'd') || 
			is_format(it, 'f') ||
			is_format(it, 's') ) {
			counted_args++;
			it += 2;
			continue;
		}

		// Case when format looks like "Misplaced %"
		if (is_format(it, '\0')) {
			REPORT_ERROR("misplaced %% at the end of the format string");
			return -1;
		}
	}

	return counted_args;
}

enum {
	MAX_FORMATED_ARG_LENGTH = (MAX_INT32_LENGTH > MAX_F32_LENGTH)?MAX_INT32_LENGTH:MAX_F32_LENGTH
};

typedef struct Formated_Argument_ {
	size_t offset_in_format;
	union {
		char dummy[MAX_FORMATED_ARG_LENGTH];
		const char* string;
	} result;

} Formated_Argument;

int my_printf(const char* restrict format, size_t num_va_args, ...) {
	assert(format);

	if (strlen(format) == 0) {
		REPORT_ERROR("strlen(format) == 0");
		return -1;
	}
	// @ToDo: handle case when num_va_args == 0

	// 1. Count arguments

	if (count_number_of_arguments(format) != num_va_args) {
		REPORT_ERROR("counted_args != num_va_args");
		return -1;
	}

	Formated_Argument formated_arguments[num_va_args];

	va_list args;
	va_start(args, num_va_args);

	va_end(args);

	// 1. Iterate over entire format
	// 2. Confirm that num of % = num of varargs
	// 3. Generate varargs
	// 4. Create the string

	return 0;
}

// position of types in format string -> convert va_args to string and store it -> insert in correct position in format string
// 
// How to store the converted value?
// %c - just one char string
// %d - -2 147 483 648 to 2 147 483 647 ==> 11 chars at most
// %f - FLT_DECIMAL_DIG + FLT_DIG + 2 ==> sign + number of digits + comma + number of digits after comma
// %s - just store the pointer
//
// Then placing it is just as simple as strcpy'ing it to correct place