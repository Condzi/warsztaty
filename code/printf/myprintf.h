#ifndef MYPRINTF_H
#define MYPRINTF_H

#include "vargs_length.h"

// _Printf_format_string_ 
// https://docs.microsoft.com/en-us/cpp/code-quality/annotating-function-parameters-and-return-values?view=msvc-170#format-string-parameters

/*
 * Formats and writes a string to stdout.
 *
 * @param[in] format - string with following specifiers: %d -- int32_t, %f -- float, %c -- int8_t/char
 * @param[in] num_va_args - number of arguments specified in "format" and passed in va_args
 * @return number of characters transmitted to the output stream or negative value if an output error or an encoding error (for string and character conversion specifiers) occurred
*/
int my_printf(const char* restrict format, size_t num_va_args, ...);

#ifndef MY_PRINTF
	#define MY_PRINTF(format, ...) my_printf(format, VA_ARGS_LENGTH(__VA_ARGS__), __VA_ARGS__)
#endif // MYPRINTF
#endif // MYPRINTF_H