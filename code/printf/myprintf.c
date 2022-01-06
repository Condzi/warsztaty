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

		if (is_format(it, 'c') || 
			is_format(it, 'd') || 
			is_format(it, 'f') ) {
			counted_args++;
			it += 2;
			continue;
		}

		// Ignore '%%'
		if (is_format(it, '%')) {
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
	// + 1 for \0
	MAX_FORMATED_ARG_LENGTH = (MAX_INT32_LENGTH > MAX_F32_LENGTH)?MAX_INT32_LENGTH:MAX_F32_LENGTH + 1
};

typedef struct Formated_Argument_ {
	size_t offset_in_format;
	char string[MAX_FORMATED_ARG_LENGTH];
} Formated_Argument;

// 1. Iterate over entire format
// 2. Confirm that num of % = num of varargs
// 3. Parse varargs
// 4. Create the string
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

	// msvc doesn't support VLA because reasons
#ifdef _MSC_VER
	Formated_Argument* formated_arguments = _alloca(num_va_args*sizeof(Formated_Argument));
#else
	Formated_Argument formated_arguments[num_va_args];
#endif

	va_list args;
	va_start(args, num_va_args);

	const char* it = format;
	size_t current_arg_idx = 0;

	while(*it) {
		if (*it != '%') {
			it++;
			continue;
		} 

		char* arg_string = formated_arguments[current_arg_idx].string;
		// We ignore %% here as well
		if (is_format(it, 'c')) {
			char arg_raw = (char)va_arg(args, int);

			arg_string[0] = arg_raw;
			arg_string[1] = '\0';
		} else if (is_format(it, 'd')) {
			int arg_raw = va_arg(args, int);

			int written = int_to_string(arg_raw, arg_string);
			if (written == 0) {
				strcpy_s(arg_string, MAX_FORMATED_ARG_LENGTH,"<error>");
			}
		} else if (is_format(it, 'f')) {
			float arg_raw = (float)va_arg(args, double);

			int written = float_to_string(arg_raw, arg_string);
			if (written == 0) {
				strcpy_s(arg_string, MAX_FORMATED_ARG_LENGTH, "<error>");
			}
		}
		
		formated_arguments[current_arg_idx].offset_in_format = (size_t)it - (size_t)format;
		current_arg_idx++;
		it += 2;
	}

	va_end(args);

	printf("\n -> Formated %llu args.\n", num_va_args);
	for (int i = 0; i < num_va_args; i++) {
		printf("%d. Position: %llu. \"%s\"\n", i + 1, formated_arguments[i].offset_in_format, formated_arguments[i].string);
	}

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