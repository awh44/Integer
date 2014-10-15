#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Integer.h"

uint8_t integer_initialize(Integer *a)
{
	a->values = malloc(sizeof *a->values);
	if (a->values == NULL)
	{
		return 0;
	}
	a->assigned_values = 0;
	a->array_size = 1;
}

void integer_uninitialize(Integer *a)
{
	free(a->values);
}

Integer integer_assign_from_int(Integer *a, uint64_t b)
{
	memset(a->values, 0, a->array_size * sizeof *a->values);
	a->values[0] = b;
	a->assigned_values = 1;
	return *a;
}

Integer integer_assign_from_integer(Integer *a, Integer *b)
{
	resize_if_necessary(a, b->assigned_values);
	memcpy(a->values, b->values, b->assigned_values * sizeof *b->values);
	return *a;
}

char *integer_to_string(Integer *a)
{
}

uint8_t resize_if_necessary(Integer *a, size_t needed_size)
{
    if (a->array_size < needed_size)
    {
		a->array_size *= 2;
		uint64_t *tmp = realloc(a->values, a->array_size * sizeof *a->values);
		if (tmp == NULL)
		{
			a->array_size /= 2;
			return 0;
		}
		a->values = tmp;
		return 1;
	}
}
