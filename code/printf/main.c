#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "charconv.h"
#include "myprintf.h"

void test_charconv();
void test_my_printf();

int main(){
	test_charconv();
	test_my_printf();
}

void test_charconv() {
	#define ARRAY_LENGTH(x) sizeof(x)/sizeof(x[0])

	puts("\ntest_charconv: int_to_string");
	{
		char buffer[MAX_INT32_LENGTH + 1];
		const int test_input[] = {1234, -1234, INT32_MAX, INT32_MIN, 0, -0, 987654321};

		for (int i = 0; i < ARRAY_LENGTH(test_input); i++) {
			if (int_to_string(test_input[i], buffer) == 0) {
				strcpy(buffer, "<error>");
			}

			printf(" input: \"%d\", result \"%s\"\n", test_input[i], buffer);
		}
	}

	puts("\n\ntest_charconv: float_to_string");
	{
		char buffer[MAX_F32_LENGTH + 1];
		const float test_input[] = {0, -1, 1, 21.37, -100.123456789, 123.321f};

		for (int i = 0; i < ARRAY_LENGTH(test_input); i++) {
			if (float_to_string(test_input[i], buffer) == 0) {
				strcpy(buffer, "<error>");
			}

			printf(" input: \"%f\", result \"%s\"\n", test_input[i], buffer);
		}
	}
}

void test_my_printf() {
	puts("\ntest_my_printf: simple");
	{
		my_printf(" (the next 5 lines fail on purpose)%s", "\n");
		my_printf(" blah %%\n", 321);
		my_printf(" %\n", 123);
		my_printf("%");
		my_printf(" %c\n", 'X');
		my_printf(" %d\n", 123);
		my_printf(" %f\n", 123.321f);
		my_printf(" %s", "a string\n");
	}

	puts("\ntest_my_printf: combined");
	my_printf(" Test: %d, %f, %c, %d. And finally, it works 100%% of the time %s!\n", 666, 21.37f, 'D', 997, ":)");
}