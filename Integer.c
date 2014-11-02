#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Integer.h"

void integer_add_cycle(Integer *result, uint32_t value, size_t index);

uint8_t integer_initialize(Integer *a)
{
	a->values = calloc(1, sizeof *a->values);
	if (a->values == NULL)
	{
		return 0;
	}
	a->assigned_values = 0;
	a->array_size = 1;
	return 1;
}

void integer_uninitialize(Integer *a)
{
	free(a->values);
}

Integer integer_assign_from_int(Integer *a, uint64_t b)
{
	memset(a->values, 0, a->array_size * sizeof *a->values);
	uint32_t b_top = b >> BITS_IN_VALUE;
	if (b_top > 0)
	{
		integer_resize_if_necessary(a, 2);
		a->values[0] = b;
		a->values[1] = b_top;
		a->assigned_values = 2;
	}
	else
	{
		a->values[0] = b;
		a->assigned_values = 1;
	}
	return *a;
}

Integer integer_assign_from_integer(Integer *a, Integer *b)
{
	integer_resize_if_necessary(a, b->assigned_values);
	memcpy(a->values, b->values, b->assigned_values * sizeof *b->values);
	a->assigned_values = b->assigned_values;
	return *a;
}

Integer integer_add_int(Integer *a, uint64_t b)
{
	Integer result;
	integer_initialize(&result);
	integer_assign_from_integer(&result, a);

	integer_add_cycle(&result, b, 0);
	integer_add_cycle(&result, b >> BITS_IN_VALUE, 1);

	return result;
}

Integer integer_add_integer(Integer *a, Integer *b)
{
	Integer result;
	integer_initialize(&result);
	integer_assign_from_integer(&result, a);

	size_t i;
	for (i = 0; i < b->assigned_values; i++)
	{
		integer_add_cycle(&result, b->values[i], i);
	}

	return result;
}

void integer_add_cycle(Integer *result, uint32_t value, size_t index)
{
	uint64_t result64 = (uint64_t) value + result->values[index];
	result->values[index] = result64;

	uint64_t remaining = result64 >> BITS_IN_VALUE;
	size_t i = index + 1;
	while (remaining > 0)
	{
		integer_resize_if_necessary(result, i);
		result64 = remaining + result->values[i];

		result->values[i] = result64;
		result->assigned_values = i + 1;

		remaining = result64 >> BITS_IN_VALUE;
		i++;
	}
}

char *integer_to_string(Integer *a)
{
	return NULL;
}

uint8_t integer_resize_if_necessary(Integer *a, size_t needed_size)
{
    if (a->array_size < needed_size)
    {
		a->array_size *= 2;
		uint32_t *tmp = realloc(a->values, a->array_size * sizeof *a->values);
		if (tmp == NULL)
		{
			a->array_size /= 2;
			return 0;
		}
		a->values = tmp;
		int i;
		for (i = a->array_size / 2 ; i < a->array_size; i++)
		{
			a->values[i] = 0;
		}
	}

	return 1;
}
