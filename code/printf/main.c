#include <stdio.h>
#include <assert.h>

#include "charconv.h"
#include "myprintf.h"

void test_charconv();
void test_my_printf();

int main(){
	test_my_printf();
	//test_charconv();
}

void test_charconv() {
	char buffer[MAX_INT32_LENGTH + 1];
	int iarr[] = {1234, -1234, INT32_MAX, INT32_MIN, 0, -0, 123};
	for (int i = 0; i < sizeof(iarr)/sizeof(int); i++) {
		int_to_string(iarr[i], buffer);
		printf("int %d. '%s', expected '%d'\n", i + 1, buffer, iarr[i]);
	}

	printf("\n\n");
	float farr[] = {0, -1, 1, 21.37, -100.123456789, 123.321f};
	char buffer2[MAX_F32_LENGTH + 1];
	for (int i = 0; i < sizeof(farr)/sizeof(float); i++) {
		float_to_string(farr[i], buffer2);
		printf("float %d. '%s', expected '%f'\n", i + 1, buffer2, farr[i]);
	}
}

void test_my_printf() {
	printf("PRINTF TEST -- SIMPLE\n");

	my_printf("blah %%", 1);
	my_printf("%c", 1, 'X');
	my_printf("%d", 1, 123);
	my_printf("%f", 1, 123.321f);

	printf("\nPRINTF TEST -- COMBINED\n");
	my_printf("Test: %d, %f, %c, %d", 4, 666, 21.37f, 'D', 997);
}