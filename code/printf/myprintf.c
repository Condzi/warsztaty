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

/*
		Internal stuff declaration
*/

bool is_format(const char* restrict format, char c);
size_t count_number_of_arguments(const char* format);
size_t count_number_of_double_percents(const char* format);

enum {
	// + 1 for \0
	MAX_FORMATED_ARG_LENGTH = (MAX_INT32_LENGTH > MAX_F32_LENGTH)?MAX_INT32_LENGTH:MAX_F32_LENGTH + 1
};

typedef struct Formated_Argument_ {
	size_t offset_in_format;
	char string[MAX_FORMATED_ARG_LENGTH];
} Formated_Argument;

/*
		Public functions definition
*/

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
	
	if (num_va_args == 0) {
#ifdef _MSC_VER
		if (fputs(format, stdout) == EOF) {
			REPORT_ERROR("call to fputs() faield");
			return -1;
		}
#else
		if (write(1, format, format_string_length) != format_string_length) {
			REPORT_ERROR("call to write() faield");
			return -1;
		}
#endif
		return format_string_length;
	}

	if (count_number_of_arguments(format) != num_va_args) {
		REPORT_ERROR("counted_args != num_va_args");
		return -1;
	}

	const size_t total_arguments_to_parse = num_va_args + count_number_of_double_percents(format);

	// msvc doesn't support VLA because reasons
#ifdef _MSC_VER
	Formated_Argument* formated_arguments = _alloca(total_arguments_to_parse*sizeof(Formated_Argument));
#else
	Formated_Argument formated_arguments[total_arguments_to_parse];
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
			const char arg_raw = (char)va_arg(args, int);

			arg_string[0] = arg_raw;
			arg_string[1] = '\0';

			length_of_all_strings_used_for_agruments += 1;
		} else if (is_format(format_it, 'd')) {
			const int arg_raw = va_arg(args, int);

			int written = int_to_string(arg_raw, arg_string);
			if (written == 0) {
				strcpy(arg_string, "<error>");
				written = 7; 
			}

			length_of_all_strings_used_for_agruments += written;
		} else if (is_format(format_it, 'f')) {
			const float arg_raw = (float)va_arg(args, double);

			int written = float_to_string(arg_raw, arg_string);
			if (written == 0) {
				strcpy(arg_string, "<error>");
				written = 7; 
			}

			length_of_all_strings_used_for_agruments += written;
		} else if (is_format(format_it, '%')) {
			arg_string[0] = '%';
			arg_string[1] = '\0';
			length_of_all_strings_used_for_agruments++;
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

	const size_t final_string_length = format_string_length - (total_arguments_to_parse * 2) + 
									   length_of_all_strings_used_for_agruments +
									   1; // \0

#ifdef _MSC_VER
	char* final_string = _alloca(final_string_length);
#else
	char final_string[final_string_length];
#endif

	char* position_in_final_string = final_string;

	size_t last_position_in_format = 0;
	for (int i = 0; i < total_arguments_to_parse; i++) {
		const size_t position_in_format = formated_arguments[i].offset_in_format;
		const size_t length_to_copy = position_in_format - last_position_in_format;

		// @ToDo: check for memcpy failure
		if (last_position_in_format != position_in_format) {
			memcpy(position_in_final_string, format + last_position_in_format, length_to_copy);
		}

		position_in_final_string += length_to_copy;

		const char* arg_string = formated_arguments[i].string;
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
	if (fputs(final_string, stdout) == EOF) {
		REPORT_ERROR("call to fputs() faield");
		return -1;
	}
#else
	// @ToDo: Test me
	if (write(1, final_string, final_string_length) != final_string_length) {
		REPORT_ERROR("call to write() failed");
		return -1;
	}
#endif

	return final_string_length;
}

/*
		Internal functions definition
*/

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

		// If none of the above, it's just an unknown format
		REPORT_ERROR("unknown format argument");
		format_it += 2;
	}

	return counted_args;
}

size_t count_number_of_double_percents(const char* format) {
	assert(format);
	size_t count = 0;

	const char* format_it = format;
	while (*format_it) {
		if (is_format(format_it, '%')) {
			count++;
			format_it += 2;
		} else {
			format_it++;
		}
	}

	return count;
}