#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Integer.h"

#define INTEGER_MAX_WORD_VALUE 0xffffffffffffffff


uint8_t integer_resize_if_necessary(Integer *a, size_t needed_size);

uint8_t integer_add_cycle(Integer *result, uint64_t value, uint8_t carry, size_t index);
void integer_propagate_final_carry(Integer *result, uint8_t carry, size_t carry_index);

uint8_t integer_sub_cycle(Integer *result, uint64_t value, uint8_t borrow, size_t index, size_t *cons_zeros);
void integer_propagate_final_borrow(Integer *result, uint8_t borrow, size_t borrow_index, size_t cons_zeros);

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

Integer integer_assign_from_int(Integer *dest, uint64_t source)
{
	memset(dest->values, 0, dest->array_size * sizeof *dest->values);
	dest->values[0] = source;
	dest->assigned_values = 1;
	return *dest;
}

Integer integer_assign_from_integer(Integer *dest, Integer *source)
{
	integer_resize_if_necessary(dest, source->assigned_values);
	memcpy(dest->values, source->values, source->assigned_values * sizeof *source->values);
	dest->assigned_values = source->assigned_values;
	return *dest;
}

Integer integer_add_int(Integer *result, Integer *a, uint64_t b)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_integer(&r, a);

	uint8_t carry = integer_add_cycle(&r, b, 0, 0);
	integer_propagate_final_carry(&r, carry, 1);
	
	integer_assign_from_integer(result, &r);
	return *result;
}

Integer integer_add_integer(Integer *result, Integer *a, Integer *b)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_integer(&r, a);

	uint8_t carry = 0;
	size_t i;
	for (i = 0; i < b->assigned_values; i++)
	{
		carry = integer_add_cycle(&r, b->values[i], carry, i);
	}
	integer_propagate_final_carry(&r, carry, i);	

	integer_assign_from_integer(result, &r);
	return *result;
}

uint8_t integer_add_cycle(Integer *result, uint64_t value, uint8_t carry, size_t index)
{
	integer_resize_if_necessary(result, index + 1);
	result->values[index] += value;

	uint8_t next_carry = 0;
	if (result->values[index] < value)
	{
		next_carry = 1;
	}
	
	result->values[index] += carry;
	if (result->values[index] < carry)
	{
		next_carry = 1;
	}

	return next_carry;
}

void integer_propagate_final_carry(Integer *result, uint8_t carry, size_t carry_index)
{
	while (carry > 0)
	{
		integer_resize_if_necessary(result, carry_index + 1);
		result->values[carry_index] += carry;
		carry = result->values[carry_index] < carry; //if overflow happened again, set carry to 1 
		carry_index++;
	}

	if (carry_index > result->assigned_values)
	{
		result->assigned_values = carry_index;
	}
}

Integer integer_subtract_int(Integer *result, Integer *a, uint64_t b)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_integer(&r, a);

	size_t consecutive_zeros = 0;
	uint8_t borrow = integer_sub_cycle(&r, b, 0, 0, &consecutive_zeros);
	integer_propagate_final_borrow(&r, borrow, 1, consecutive_zeros);
	
	integer_assign_from_integer(result, &r);
	return *result;	
}

Integer integer_subtract_integer(Integer *result, Integer *a, Integer *b)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_integer(&r, a);

	uint8_t borrow = 0;
	size_t consecutive_zeros = 0;
	size_t i;
	for (i = 0; i < b->assigned_values; i++)
	{
		borrow = integer_sub_cycle(&r, b->values[i], borrow, i, &consecutive_zeros);
	}
	integer_propagate_final_borrow(&r, borrow, i, consecutive_zeros);

	integer_assign_from_integer(result, &r);
	return *result;
}

uint8_t integer_sub_cycle(Integer *result, uint64_t value, uint8_t borrow, size_t index, size_t *cons_zeros)
{
	uint64_t orig_value = result->values[index];
	result->values[index] -= value;

	uint8_t next_borrow = 0;
	if (result->values[index] > orig_value)
	{
		next_borrow = 1;
	}

	orig_value = result->values[index];
	result->values[index] -= borrow;
	if (result->values[index] > orig_value)
	{
		next_borrow = 1;
	}

	*cons_zeros = result->values[index] == 0 ? *cons_zeros + 1 : 0;

	return next_borrow;
}

void integer_propagate_final_borrow(Integer *result, uint8_t borrow, size_t borrow_index, size_t cons_zeros)
{
	while (borrow > 0)
	{
		result->values[borrow_index] -= borrow;
		borrow = result->values[borrow_index] == INTEGER_MAX_WORD_VALUE;
		cons_zeros = result->values[borrow_index] == 0 ? cons_zeros + 1 : 0;
		borrow_index++;
	}

	result->assigned_values -= cons_zeros;
}

void integer_mult_cycle(Integer *result, uint64_t value, size_t index)
{
/*
	     1111 = a
	   x 0110 = b  Note that the 'overflow' is a - r. 
	---------
	0101 1010 = r
*/
}

Integer integer_multiply_int(Integer *result, Integer *a, uint64_t b)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_integer(&r, a);

	return *result;
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
		uint64_t *tmp = realloc(a->values, a->array_size * sizeof *a->values);
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
