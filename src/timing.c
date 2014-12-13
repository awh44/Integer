#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include <string.h>
#include "Integer.h"

const size_t MINN = 10;
const size_t MAXN = 10000;
const size_t NUMTESTS = 10;
const long double MICRO_FACTOR = 10e9L;
const uint64_t MAX_INTEGER_VALUE = 0xffffffffffffffff;

extern void integer_resize_if_necessary(Integer *, size_t);

typedef struct timeval timeval;
typedef Integer (*integer_func)(Integer*, Integer*, Integer*);

void print_integer(Integer *a)
{
	size_t i;
	for (i = a->array_size - 1; i != 0xffffffffffffffff; i--)
	{
		printf("a.values[%zu] = %lu\n", i, a->values[i]);
	}
}

long double perform_test(integer_func func, Integer *result, Integer *a, Integer *b)
{
	timeval start, end;
	gettimeofday(&start, NULL);
	(*func)(result, a, b);
	gettimeofday(&end, NULL);
	long double whole = (long double) end.tv_sec - start.tv_sec;
	long double micro = ((long double) end.tv_usec - (long double) start.tv_usec) / MICRO_FACTOR;
	if (whole > 0)
	{
		return whole + micro;
	}
	else
	{
		return micro;
	}
}

int main()
{
	srand(time(NULL));
	Integer num1, num2, numresult;
	integer_initialize(&num1);
	integer_initialize(&num2);
	integer_initialize(&numresult);
	integer_resize_if_necessary(&num1, MAXN);
	integer_resize_if_necessary(&num2, MAXN);
	memset(num1.values, MAX_INTEGER_VALUE, MAXN * sizeof *num1.values);
	memset(num2.values, MAX_INTEGER_VALUE, MAXN * sizeof *num2.values);
	num1.assigned = MAXN;
	num2.assigned = MAXN;
	integer_multiply_integer(&numresult, &num1, &num2);
	return 0;
	size_t num_size;
	for (num_size = MINN; num_size <= MAXN; num_size *= 10)
	{ 

		long double running_add = 0.0;
		long double running_sub = 0.0;
		long double running_mult = 0.0;
		long double running_karat = 0.0;
		long double running_pow = 0.0;
		long double running_pow_square = 0.0;
		size_t j;
		for (j = 0; j < NUMTESTS; j++)
		{
			Integer a, b, result;
			integer_initialize(&a);
			integer_initialize(&b);
			integer_initialize(&result);

			integer_resize_if_necessary(&a, num_size);
			integer_resize_if_necessary(&b, num_size);
			memset(a.values, MAX_INTEGER_VALUE, num_size * sizeof *a.values);
			a.assigned = num_size;
			memset(b.values, MAX_INTEGER_VALUE, num_size * sizeof *b.values);
			b.assigned = num_size;
			//integer_random(&a, num_size);
			//integer_random(&b, num_size);
			
			timeval start, end;
			running_add += perform_test(integer_add_integer, &result, &a, &b);
			running_sub += perform_test(integer_subtract_integer, &result, &a, &b);
			running_mult += perform_test(integer_multiply_integer, &result, &a, &b);
			running_karat += perform_test(karatsuba, &result, &a, &b);
			//running_pow += perform_test(integer_power_integer, &result, &a, &b);
			integer_uninitialize(&a);
			integer_uninitialize(&b);
			integer_uninitialize(&result);
		}
		printf("%zu,%.30Le,%.30Le,%.30Le,%.30Le,%.30Le,%.30Le\n", num_size, running_add, running_sub, running_mult, running_karat, running_pow, running_pow_square);
	}
	
	return 0;
}
