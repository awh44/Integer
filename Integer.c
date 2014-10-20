#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Integer.h"

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

	uint32_t b_bottom = b;
	uint64_t result64 = (uint64_t) a->values[0] + (uint64_t) b_bottom;
	result.values[0] = result64;
	uint64_t remaining = result64 >> BITS_IN_VALUE;
	size_t i = 1;
	while (remaining > 0)
	{
		integer_resize_if_necessary(&result, i + 1);
		result64 = (uint64_t) a->values[i] + remaining;
		result.values[i] = result64;
		remaining = result64 >> BITS_IN_VALUE;
		i++;
	}

	result64 = (uint64_t) a->values[1] + (b >> BITS_IN_VALUE);
	result.values[1] = result64;
	remaining = result64 >> BITS_IN_VALUE;
	i = 2;
	while (remaining > 0)
	{
		integer_resize_if_necessary(&result, i + 1);
		result64 = (uint64_t) a->values[i] + remaining;
		result.values[i] = result64;
		remaining = result64 >> BITS_IN_VALUE;
		i++;
	}

	return result;
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
