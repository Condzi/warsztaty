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
	const char* format_it = format;
	while (*format_it) {
		if (*format_it != '%') {
			format_it++;
			continue;
		}

		if (is_format(format_it, 'c') || 
			is_format(format_it, 'd') || 
			is_format(format_it, 'f') ) {
			counted_args++;
			format_it += 2;
			continue;
		}

		// Ignore '%%'
		if (is_format(format_it, '%')) {
			format_it += 2;
			continue;
		}

		// Case when format looks like "Misplaced %"
		if (is_format(format_it, '\0')) {
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
	const size_t format_string_length = strlen(format);
	if (!format_string_length) {
		REPORT_ERROR("format string has length 0");
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

	const char* format_it = format;
	size_t current_arg_idx = 0;
	size_t length_of_all_strings_used_for_agruments = 0; // length of all strings used for arguments

	while (*format_it) {
		if (*format_it != '%') {
			format_it++;
			continue;
		} 

		char* arg_string = formated_arguments[current_arg_idx].string;
		// We ignore %% here as well
		if (is_format(format_it, 'c')) {
			char arg_raw = (char)va_arg(args, int);

			arg_string[0] = arg_raw;
			arg_string[1] = '\0';

			length_of_all_strings_used_for_agruments += 1;
		} else if (is_format(format_it, 'd')) {
			int arg_raw = va_arg(args, int);

			int written = int_to_string(arg_raw, arg_string);
			if (written == 0) {
				strcpy_s(arg_string, MAX_FORMATED_ARG_LENGTH, "<error>");
				written = 7; 
			}

			length_of_all_strings_used_for_agruments += written;
		} else if (is_format(format_it, 'f')) {
			float arg_raw = (float)va_arg(args, double);

			int written = float_to_string(arg_raw, arg_string);
			if (written == 0) {
				strcpy_s(arg_string, MAX_FORMATED_ARG_LENGTH, "<error>");
				written = 7; 
			}

			length_of_all_strings_used_for_agruments += written;
		}
		
		formated_arguments[current_arg_idx].offset_in_format = (size_t)format_it - (size_t)format;
		current_arg_idx++;
		format_it += 2;
	}

	va_end(args);

/*
	printf("\n -> Formated %llu args.\n", num_va_args);
	for (int i = 0; i < num_va_args; i++) {
		printf("%d. Position: %llu. \"%s\"\n", i + 1, formated_arguments[i].offset_in_format, formated_arguments[i].string);
	}
*/

	const size_t final_string_length = format_string_length - (num_va_args * 2) + 
									   length_of_all_strings_used_for_agruments +
									   1; // \0

#ifdef _MSC_VER
	char* final_string = _alloca(final_string_length);
#else
	char final_string[final_string_length];
#endif

	char* position_in_final_string = final_string;

	size_t last_position_in_format = 0;
	for (int i = 0; i < num_va_args; i++) {
		const size_t position_in_format = formated_arguments[i].offset_in_format;
		const size_t length_to_copy = position_in_format - last_position_in_format;

		// @ToDo: check for failure
		if (last_position_in_format != position_in_format) {
			memcpy(position_in_final_string, format + last_position_in_format, length_to_copy);
		}

		position_in_final_string += length_to_copy;

		char* arg_string = formated_arguments[i].string;
		const size_t arg_string_length = strlen(arg_string);
		memcpy(position_in_final_string, arg_string, arg_string_length);
		position_in_final_string += arg_string_length;

		// +1 to skip two chars instead of one in '%d' for example
		last_position_in_format = position_in_format + 2; 
	}

	if (last_position_in_format != format_string_length) {
		const size_t length_to_copy = format_string_length - last_position_in_format;
		memcpy(position_in_final_string, format + last_position_in_format, length_to_copy);
	}

	final_string[final_string_length - 1] = '\0';

#ifdef _MSC_VER
	puts(final_string);
#else
	// @ToDo: Test me
	write(1, final_string);
#endif

	return final_string_length;
}

// position of types in format string -> convert va_args to string and store format_it -> insert in correct position in format string
// 
// How to store the converted value?
// %c - just one char string
// %d - -2 147 483 648 to 2 147 483 647 ==> 11 chars at most
// %f - FLT_DECIMAL_DIG + FLT_DIG + 2 ==> sign + number of digits + comma + number of digits after comma
// %s - just store the pointer
//
// Then placing format_it is just as simple as strcpy'ing format_it to correct place