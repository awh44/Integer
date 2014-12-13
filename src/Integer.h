#ifndef _INTEGER_H_
#define _INTEGER_H_

#include <stddef.h>
#include <stdint.h>

#define BITS_IN_VALUE 64

typedef struct
{
	uint64_t *values;
	uint8_t sign;
	size_t assigned;
	size_t array_size;
} Integer;

uint8_t integer_initialize(Integer *a);
void integer_uninitialize(Integer *a);
Integer integer_assign_from_int(Integer *dest, uint64_t source);
Integer integer_assign_from_integer(Integer *dest, Integer *source);
Integer integer_add_int(Integer *result, Integer *a, uint64_t b);
Integer integer_add_integer(Integer *result, Integer *a, Integer *b);
Integer integer_subtract_int(Integer *result, Integer *a, uint64_t b);
Integer integer_subtract_integer(Integer *result, Integer *a, Integer *b);
Integer integer_multiply_int(Integer *result, Integer *a, uint64_t b);
Integer integer_multiply_integer(Integer *result, Integer *a, Integer *b);
Integer integer_power_int(Integer *result, Integer *base, uint64_t power);
char *integer_to_string(Integer *a);
void integer_random(Integer *result, size_t length);

Integer karatsuba(Integer *result, Integer *a, Integer *b);
Integer power_by_squaring(Integer *result, Integer *a, uint64_t b);
#endif
