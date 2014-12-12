#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Integer.h"

#define INTEGER_MAX_WORD_VALUE 0xffffffffffffffff

void print_parts(char *name, Integer *a)
{
	size_t i;
	for (i = a->assigned - 1; i != 0xffffffffffffffff; i--)
	{
		printf("%s.values[%zu] = %lu\n", name, i, a->values[i]);
	}
}

void integer_set_assigned(Integer *a);

uint8_t integer_resize_if_necessary(Integer *a, size_t needed_size);

uint8_t integer_add_cycle(Integer *result, uint64_t value, uint8_t carry, size_t index);
void integer_propagate_final_carry(Integer *result, uint8_t carry, size_t carry_index);

uint8_t integer_sub_cycle(Integer *result, uint64_t value, uint8_t borrow, size_t index, size_t *cons_zeros);
void integer_propagate_final_borrow(Integer *result, uint8_t borrow, size_t borrow_index, size_t cons_zeros);

void integer_set_assigned(Integer *a)
{
	if (a->assigned == 0)
	{
		return;
	}

	size_t i = a->assigned - 1;
	while (a->values[i] == 0)
	{
		i--;
	}
	a->assigned = i + 1;
}

uint8_t integer_initialize(Integer *a)
{
	a->values = calloc(1, sizeof *a->values);
	if (a->values == NULL)
	{
		return 0;
	}
	a->assigned = 0;
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
	dest->assigned = 1;
	return *dest;
}

Integer integer_assign_from_integer(Integer *dest, Integer *source)
{
	integer_resize_if_necessary(dest, source->assigned);
	memcpy(dest->values, source->values, source->assigned * sizeof *source->values);
	dest->assigned = source->assigned;
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
	for (i = 0; i < b->assigned; i++)
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

	if (carry_index > result->assigned)
	{
		result->assigned = carry_index;
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
	for (i = 0; i < b->assigned; i++)
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

	result->assigned -= cons_zeros;
}

Integer integer_multiply_int(Integer *result, Integer *a, uint64_t b)
{
	Integer r;
	integer_initialize(&r);

	//handle multiplication by zero immediately
	if ((a->assigned == 0) || (b == 0))
	{
		integer_assign_from_integer(result, &r);
		return *result;
	}

	Integer *intermediates = malloc(a->assigned * sizeof *intermediates);
	size_t i;
	//linear operation
	for (i = 0; i < a->assigned; i++)
	{
		integer_initialize(&intermediates[i]);
		
		__uint128_t result128 = (__uint128_t) a->values[i] * b;	
		uint64_t upper64 = result128 >> BITS_IN_VALUE;
		if (upper64 > 0)
		{
			integer_resize_if_necessary(&intermediates[i], i + 1);
			intermediates[i].values[i] = result128;
			intermediates[i].values[i + 1] = upper64;
			intermediates[i].assigned = i + 2;
		}
		else
		{
			integer_resize_if_necessary(&intermediates[i], i);
			intermediates[i].values[i] = result128;
			intermediates[i].assigned = i + 1;
		}
	}

	//n * n operation
	for (i = 0; i < a->assigned; i++)
	{
		//linear operation
		integer_add_integer(&r, &r, &intermediates[i]);
	}
	free(intermediates);

	integer_assign_from_integer(result, &r);
	return *result;
}

Integer integer_multiply_integer(Integer *result, Integer *a, Integer *b)
{
	Integer r;
	integer_initialize(&r);

	Integer resultInteger;
	resultInteger.values = calloc(2, sizeof *resultInteger.values);
	resultInteger.array_size = 2;
	resultInteger.assigned = 0;

	Integer run_sum;
	integer_initialize(&run_sum);

	size_t n = a->assigned;
	size_t i;
	for (i = 0; i < 2 * n; i++)
	{
		size_t stop = i < n - 1 ? i : n - 1;
		int64_t tmp = (int64_t) i + 1 - n;
		size_t j = tmp > 0 ? tmp : 0;
		for (; j <= stop; j++)
		{
			size_t k = i - j;
			__uint128_t result128 = (__uint128_t) a->values[j] * b->values[k];
			uint64_t upper64 = result128 >> BITS_IN_VALUE;
			resultInteger.values[1] = upper64;
			resultInteger.values[0] = result128;
			resultInteger.assigned = (resultInteger.values[0] > 0) + (resultInteger.values[1] > 0);

			integer_add_integer(&run_sum, &run_sum, &resultInteger);
		}

		integer_resize_if_necessary(&r, i + 1);
		r.values[i] = run_sum.values[0];
		if (r.values[i] > 0)
		{
			r.assigned = i + 1;
		}
		
		if (run_sum.assigned > 0)
		{
			run_sum.assigned--;
			memmove(run_sum.values, run_sum.values + 1, run_sum.assigned * sizeof *run_sum.values);
		}
		run_sum.values[run_sum.assigned] = 0;
		
	}
	integer_assign_from_integer(result, &r);
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
