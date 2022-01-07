#ifndef MYPRINTF_H
#define MYPRINTF_H

#include "vargs_length.h"

/*
 * Formats and writes a string to stdout.
 *
 * @param[in] format - string with following specifiers: %d -- int32_t, %f -- float, %c -- int8_t/char, %s -- NULL-terminated string
 * @param[in] num_va_args - number of arguments specified in "format" and passed `...`
 * @param[in] ... - arguments that match formating specifiers given in "format"
 * @return number of characters transmitted to the output stream or negative value if an output error or an encoding error (for string and character conversion specifiers) occurred
*/
int my_printf(const char* restrict format, size_t num_va_args, ...);

/*
 * Wrapper for `my_printf` which automatically deduces number of variadic arguments
*/
#ifndef MY_PRINTF
	#define MY_PRINTF(format, ...) my_printf(format, VA_ARGS_LENGTH(__VA_ARGS__), __VA_ARGS__)
#endif // MYPRINTF
#endif // MYPRINTF_H