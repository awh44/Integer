#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Integer.h"

const uint64_t INTEGER_MAX_WORD_VALUE = 0xffffffffffffffff;

//----------------------------------------DEBUG FUNCTIONS-----------------------------------------//
void print_parts(char *name, Integer *a)
{
	size_t i;
	for (i = a->array_size - 1; i != INTEGER_MAX_WORD_VALUE; i--)
	{
		printf("%s.values[%zu] = %lu\n", name, i, a->values[i]);
	}
	printf("%s.assigned = %zu\n%s.array_size = %zu\n", name, a->assigned, name, a->array_size);
}
//------------------------------------------------------------------------------------------------//


//---------------------------------GENERAL HELPER FUNCTIONS---------------------------------------//
/*
	assures that a has at least enough space in its values array to fit needed_size elements (or
	returns false if that is not possible)
	@param a the Integer whose array will be made the correct size; a is salvaged upon failure
	@param needed_size the size the array needs to be, at the least
	@return returns true if the array could be realloc'd, false if realloc returns a NULL pointer
*/
uint8_t integer_resize_if_necessary(Integer *a, size_t needed_size);
/*
	given an Integer a, determines how much of the space allocated for its array is actually
	currently being used to store numbers and sets a's assigned property accordingly;currently used
	in integer_propagate_final_borrow only but has potential for other uses
	@param a the Integer to set the size 
*/
void integer_set_assigned(Integer *a);
//------------------------------------------------------------------------------------------------//


//--------------------------------ADDITION HELPER FUNCTIONS---------------------------------------//
/*
	given result, some value, a carry in, and an index, adds the value and the carry to result at
	the current index and returns the next carry
	@param result pointer to the Integer object to which value and carry will be added
	@param value some value to be added to result at the current index
	@param carry the carry in bit from any previous additions to be added at the current index
	@param index the current index of result's value array being added to
	@return the next carry bit
*/
uint8_t integer_add_cycle(Integer *result, uint64_t value, uint8_t carry, size_t index);
/*
	after exhausting values to be added to the result in addition, this is used to propagate the
	final carray bit as far as it needs to go
	@param result pointer to the Integer object on which the carry is being propagated
	@param carry the carry in at the current carry_index
	@param carry_index the index at which the carry is being added
*/
void integer_propagate_final_carry(Integer *result, uint8_t carry, size_t carry_index);
//------------------------------------------------------------------------------------------------//

//---------------------------------SUBTRACTION HELPER FUNCTIONS-----------------------------------//
/*
	equivalent to integer_add_cycle for addition, given a result, a value, a borrow, and an index,
	this function subtracts the valu and borrow at the index and returns whether there needs to be a
	borrow from the next array position
	@param result pointer to the Integer object being subtracted from
	@param value the value being subtracted from result
	@param borrow the "borrow in," so to speak; 1 if previous array position borrowed, 0 otherwise
	@return 1 if a borrow had to occur in the current subtraction, 0 otherwise
*/

uint8_t integer_sub_cycle(Integer *result, uint64_t value, uint8_t borrow, size_t index);
/*
	equivalent to integer_propagate_final_carry for addition, given a result, a borrow, and an
	index, this function propagates the borrow as far as it needs to go
	@param result pointer to the Integer object on which the borrowing is being propagated
	@param borrow the "borrow in" at the borrow_index
	@param 
*/
void integer_propagate_final_borrow(Integer *result, uint8_t borrow, size_t borrow_index);
//------------------------------------------------------------------------------------------------//

//-----------------------------------KARATSUBA HELPER FUNCTIONS-----------------------------------//
/*
	does all the actual recursive work of the karatsuba function - prevents returning unecessary
	intermediate Integers
	@param result pointer to the Integer at which the result of the multiplication will be stored
	@param a pointer to the first Integer being multiplied
	@param b pointer to the second integer being multiplied
*/
void karatsuba_worker(Integer *result, Integer *a, Integer *b);
/*
	given an Integer i of magnitude i_mag and low and high Integers, splits i so that low contains
	the lower m elements of i and high the upper; note that each does not actually copy from i's
	array but rather maintain references to its array (and so should not be modified)
	@param i pointer to the Integer being split
	@param low pointer to the Integer that will reference the lower half
	@param high pointer to the Integer that will reference the upper half
	@param i_mag the number of elements which i contains
	@param 
*/
void karatsuba_init_low_high(Integer *i, Integer *low, Integer *high, size_t m);
//------------------------------------------------------------------------------------------------//

//-----------------------------------POWERING HELPER FUNCTIONS------------------------------------//
/*
	analogous to the karatsuba_worker function, this one does all the actual recursive work of
	computing the exponentation for power_by_squaring (preventing the unused returns of Integers,
	et.c)
	@param result pointer to the Integer where the result of the calculation is stored
	@param base the base in the exponentation
	@param power the power to which the base is to be raised
*/
void power_worker(Integer *result, Integer *base, uint64_t power);
//-----------------------------------POWERING HELPER FUNCTIONS------------------------------------//


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
	memset(dest->values, 0, dest->array_size * sizeof *dest->values);
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
	integer_uninitialize(&r);
	return *result;
}

Integer integer_add_integer(Integer *result, Integer *a, Integer *b)
{
/*
	if (a->sign != b->sign)
	{
		if (a->sign == '-')
		{
			return integer_subtract_integer(result, b, a);
		}
		else
		{
			return integer_subtract_integer(result, a, b);
		}
	}
*/
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
	result->sign = a->sign;
	integer_uninitialize(&r);
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

	uint8_t borrow = integer_sub_cycle(&r, b, 0, 0);
	integer_propagate_final_borrow(&r, borrow, 1);

	integer_assign_from_integer(result, &r);
	integer_uninitialize(&r);
	return *result;	
}

Integer integer_subtract_integer(Integer *result, Integer *a, Integer *b)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_integer(&r, a);

	uint8_t borrow = 0;
	size_t i;
	for (i = 0; i < b->assigned; i++)
	{
		borrow = integer_sub_cycle(&r, b->values[i], borrow, i);
	}
	integer_propagate_final_borrow(&r, borrow, i);

	integer_assign_from_integer(result, &r);
	integer_uninitialize(&r);
	return *result;
}

uint8_t integer_sub_cycle(Integer *result, uint64_t value, uint8_t borrow, size_t index)
{
	uint64_t orig_value = result->values[index];
	result->values[index] -= borrow;

	uint8_t next_borrow = 0;
	if (result->values[index] > orig_value)
	{
		next_borrow = 1;
	}

	orig_value = result->values[index];
	result->values[index] -= value;
	if (result->values[index] > orig_value)
	{
		next_borrow++;
	}	

	return next_borrow;
}

void integer_propagate_final_borrow(Integer *result, uint8_t borrow, size_t borrow_index)
{
	while (borrow > 0)
	{
		result->values[borrow_index] -= borrow;
		borrow = result->values[borrow_index] == INTEGER_MAX_WORD_VALUE;
		borrow_index++;
	}

	integer_set_assigned(result);
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
		integer_initialize(intermediates + i);
		
		__uint128_t result128 = (__uint128_t) a->values[i] * b;	
		uint64_t upper64 = result128 >> BITS_IN_VALUE;
		if (upper64 > 0)
		{
			integer_resize_if_necessary(intermediates + i, i + 1);
			intermediates[i].values[i] = result128;
			intermediates[i].values[i + 1] = upper64;
			intermediates[i].assigned = i + 2;
		}
		else
		{
			integer_resize_if_necessary(intermediates + i, i);
			intermediates[i].values[i] = result128;
			intermediates[i].assigned = i + 1;
		}
	}

	//n * n operation
	for (i = 0; i < a->assigned; i++)
	{
		//linear operation
		integer_add_integer(&r, &r, intermediates + i);
		integer_uninitialize(intermediates + i);
	}
	free(intermediates);

	integer_assign_from_integer(result, &r);
	integer_uninitialize(&r);
	return *result;
}

Integer integer_multiply_integer(Integer *result, Integer *a, Integer *b)
{
	//handle the zero/identity elements immediately
	if ((a->assigned == 0) || (b->assigned == 0))
	{
		integer_assign_from_int(result, 0);
		return *result;
	}

	if ((a->assigned == 1) && (a->values[0] == 1))
	{
		integer_assign_from_integer(result, b);
		return *result;
	}

	if ((b->assigned == 1) && (b->values[0] == 1))
	{
		integer_assign_from_integer(result, a);
		return *result;
	}

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
			uint64_t b_val = k >= b->assigned ? 0 : b->values[k];
			__uint128_t result128 = (__uint128_t) a->values[j] * b_val;
			uint64_t upper64 = result128 >> BITS_IN_VALUE;
			resultInteger.values[1] = upper64;
			resultInteger.values[0] = result128;
			//0, 1, or 2 assigned values
			if (resultInteger.values[1] > 0)
			{
				resultInteger.assigned = 2;
			}
			else if (resultInteger.values[0] > 0)
			{
				resultInteger.assigned = 1;
			}
			else
			{
				resultInteger.assigned = 0;
			}
			integer_add_integer(&run_sum, &run_sum, &resultInteger);
		}

		integer_resize_if_necessary(&r, i + 1);
		r.values[i] = run_sum.values[0];
		if (run_sum.values[0] > 0)
		{
			r.assigned = i + 1;
		}

		if (run_sum.assigned > 0)
		{
			run_sum.assigned--;
			//integer division by 2^64
			memmove(run_sum.values, run_sum.values + 1, run_sum.assigned * sizeof *run_sum.values);
		}
		run_sum.values[run_sum.assigned] = 0;	
	}
	integer_uninitialize(&resultInteger);
	integer_uninitialize(&run_sum);
	
	integer_assign_from_integer(result, &r);
	integer_uninitialize(&r);
	return *result;
}

Integer karatsuba(Integer *result, Integer *a, Integer *b)
{
	if (a->array_size > b->array_size)
	{
		integer_resize_if_necessary(b, a->array_size);
	}
	else if (b->array_size > a->array_size)
	{
		integer_resize_if_necessary(a, b->array_size);
	}
	karatsuba_worker(result, a, b);
	return *result;
}

void karatsuba_worker(Integer *result, Integer *a, Integer *b)
{
	if (a->assigned < 2)
	{
		integer_multiply_int(result, b, a->values[0]);
		return;
	}
	else if (b->assigned < 2)
	{
		integer_multiply_int(result, a, b->values[0]);
		return;
	}
	size_t a_mag = a->assigned;
	size_t b_mag = b->assigned;

	size_t m = (a_mag > b_mag ? a_mag : b_mag) / 2;

	Integer low1, high1;
	karatsuba_init_low_high(a, &low1, &high1, m);
	Integer low2, high2;
	karatsuba_init_low_high(b, &low2, &high2, m);

	Integer low1_plus_high1;
	integer_initialize(&low1_plus_high1);
	integer_add_integer(&low1_plus_high1, &low1, &high1);
	
	Integer low2_plus_high2;
	integer_initialize(&low2_plus_high2);
	integer_add_integer(&low2_plus_high2, &low2, &high2);

	Integer z0;
	integer_initialize(&z0);
	karatsuba_worker(&z0, &low1, &low2);
	
	Integer z1;
	integer_initialize(&z1);
	karatsuba_worker(&z1, &low1_plus_high1, &low2_plus_high2);

	Integer z2;
	integer_initialize(&z2);
	karatsuba_worker(&z2, &high1, &high2);


	//return z2 * (2^64)^(2 * m) + (z1 - z2 - z0) * (2^64)^m + z0
	//(z1 - z2 - z0) * (2^64)^m - subtract and then use memmove to shift the array, multiplying by
	//factor of 2^(64m)
	integer_subtract_integer(&z1, &z1, &z2);
	integer_subtract_integer(&z1, &z1, &z0);
	integer_resize_if_necessary(&z1, z1.assigned + m);
	memmove(z1.values + m, z1.values, z1.assigned * sizeof *z1.values);
	memset(z1.values, 0, m * sizeof *z1.values);
	z1.assigned += m;

	//z2 * (2^64)^(2 * m) - use the same technique as above to shift the array, having the effect of
	//multiplying by 2^(128m)
	size_t twoM = 2 * m;
	integer_resize_if_necessary(&z2, z2.assigned + twoM);
	memmove(z2.values + twoM, z2.values, z2.assigned * sizeof *z2.values);
	memset(z2.values, 0, twoM * sizeof *z2.values);
	z2.assigned += twoM;
	
	//add all results into z2
	integer_add_integer(&z2, &z2, &z1);
	integer_add_integer(&z2, &z2, &z0);
	//then return the result by assigning it to result
	integer_assign_from_integer(result, &z2);
	
	integer_uninitialize(&z0);
	integer_uninitialize(&z1);
	integer_uninitialize(&z2);
	integer_uninitialize(&low1_plus_high1);
	integer_uninitialize(&low2_plus_high2);
}

void karatsuba_init_low_high(Integer *i, Integer *low, Integer *high, size_t m)
{
	low->assigned = low->array_size = m;
	low->values = i->values;
	high->assigned = high->array_size = i->assigned - m;
	high->values = low->values + low->assigned;
}

//Even Maple can't handle 2^(2^64), which would be the smallest possible advantage if a version with
//an Integer power was implemented, so I didn't bother with implementing it.
Integer integer_power_int(Integer *result, Integer *base, uint64_t power)
{
	Integer r;
	integer_initialize(&r);
	integer_assign_from_int(&r, 1);

	size_t i;
	for (i = 1; i <= power; i++)
	{
		integer_multiply_integer(&r, &r, base);
	}

	integer_assign_from_integer(result, &r);
	integer_uninitialize(&r);
	return *result;
}

Integer power_by_squaring(Integer *result, Integer *base, uint64_t power)
{
	power_worker(result, base, power);
	return *result;
}

void power_worker(Integer *result, Integer *base, uint64_t power)
{
	if (power == 0)
	{
		integer_assign_from_int(result, 1);
		return;
	}
	
	if (power == 1)
	{
		integer_assign_from_integer(result, base);
		return;
	}

	Integer x;
	integer_initialize(&x);
	power_worker(&x, base, power / 2);
	integer_multiply_integer(result, &x, &x);
	if (power % 2 == 1)
	{
		integer_multiply_integer(result, result, base);
	}
	integer_uninitialize(&x);
}

char *integer_to_string(Integer *a)
{
	return NULL;
}

uint8_t integer_resize_if_necessary(Integer *a, size_t needed_size)
{
	if (a->array_size < needed_size)
	{
		size_t orig_size = a->array_size; 
		do
		{
			a->array_size *= 2;
		} while ((a->array_size < needed_size) && (a->array_size > orig_size));
		if (a->array_size <= orig_size)
		{
			a->array_size = orig_size;
			return 0;
		}

		uint64_t *tmp = realloc(a->values, a->array_size * sizeof *a->values);
		if (tmp == NULL)
		{
			a->array_size = orig_size;
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

void integer_set_assigned(Integer *a)
{
	size_t i = a->array_size - 1;
	while (i != INTEGER_MAX_WORD_VALUE && a->values[i] == 0)
	{
		i--;
	}
	a->assigned = i + 1;
}

