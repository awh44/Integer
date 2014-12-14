#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include <string.h>
#include "Integer.h"

const size_t MINN = 10;
const size_t MAXN = 10000;
const size_t STEP = 5;
const size_t NUMTESTS = 10;
const long double MICRO_FACTOR = 10e9L;
const uint64_t MAX_INTEGER_VALUE = 0xffffffffffffffff;
const uint64_t MINPOWER = 0x0002;
const uint64_t MAXPOWER = 0x1000;

extern void integer_resize_if_necessary(Integer *, size_t);

typedef struct timeval timeval;
typedef Integer (*integer_func)(Integer*, Integer*, Integer*);
typedef Integer (*power_func)(Integer*, Integer*, uint64_t);
typedef void (*mpz_func)(mpz_t, const mpz_t, const mpz_t);

void print_integer(Integer *a)
{
	size_t i;
	for (i = a->array_size - 1; i != 0xffffffffffffffff; i--)
	{
		printf("a.values[%zu] = %lu\n", i, a->values[i]);
	}
}

long double perform_mpz_test(mpz_func func, mpz_t result, mpz_t a, mpz_t b)
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

long double perform_power_test(power_func func, Integer *result, Integer *a, uint64_t power)
{
	timeval start, end;
	gettimeofday(&start, NULL);
	(*func)(result, a, power);
	gettimeofday(&end, NULL);
	long double whole = (long double) end.tv_sec - start.tv_sec;
	long double micro = ((long double) end.tv_usec - (long double) start.tv_usec) / MICRO_FACTOR;
	if (whole > 0)
	{
		return whole + micro;
	}
	{
		return micro;
	}
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	if (argc < 2)
	{
		size_t num_size;
		for (num_size = MINN; num_size <= MAXN; num_size *= STEP)
		{ 

			long double running_add = 0.0;
			long double running_sub = 0.0;
			long double running_mult = 0.0;
			long double running_karat = 0.0;
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
				
				running_add += perform_test(integer_add_integer, &result, &a, &b);
				running_sub += perform_test(integer_subtract_integer, &result, &a, &b);
				running_mult += perform_test(integer_multiply_integer, &result, &a, &b);
				running_karat += perform_test(karatsuba, &result, &a, &b);
				//running_pow += perform_test(integer_power_integer, &result, &a, &b);
				integer_uninitialize(&a);
				integer_uninitialize(&b);
				integer_uninitialize(&result);
			}
			printf("%zu,%.30Le,%.30Le,%.30Le,%.30Le\n", num_size, running_add, running_sub, running_mult, running_karat);
		}
	}
	else if (strcmp(argv[1], "1") == 0)
	{
			uint64_t power;
			Integer a;
			integer_initialize(&a);
			integer_assign_from_int(&a, MAX_INTEGER_VALUE);
			integer_add_int(&a, &a, 1);
			for (power = MINPOWER; power <= MAXPOWER; power *= 2)
			{
				long double running_power = 0.0;
				long double running_power_by_squaring = 0.0;
				size_t j;
				for (j = 0; j < NUMTESTS; j++)
				{
					Integer result;
					integer_initialize(&result);
					running_power += perform_power_test(integer_power_int, &result, &a, power);
					running_power_by_squaring += perform_power_test(power_by_squaring, &result, &a, power);
				}
				printf("%zu,%Le,%Le\n", power, running_power, running_power_by_squaring);
			}
	}
	else if (strcmp(argv[1], "2") == 0)
	{
		mpz_t twoTo64, twoTo64Minus1, running_val;
		mpz_init_set_ui(twoTo64, MAX_INTEGER_VALUE);
		mpz_init_set_ui(twoTo64Minus1, MAX_INTEGER_VALUE);
		mpz_init_set(running_val, twoTo64Minus1);
		mpz_add_ui(twoTo64, twoTo64, 1);
		size_t num_size;
		for (num_size = 10; num_size <= MAXN; num_size *= STEP)
		{
			long double running_add = 0.0;
			long double running_sub = 0.0;
			long double running_mul = 0.0;
			size_t j;
			for (j = 0; j < NUMTESTS; j++)
			{
				mpz_t a, b, result;
				mpz_init_set(a, running_val);
				mpz_init_set(b, running_val);
				mpz_sub_ui(b, b, 1);
				mpz_init(result);
				running_add += perform_mpz_test(mpz_add, result, a, b);
				running_sub += perform_mpz_test(mpz_sub, result, a, b);
				running_mul += perform_mpz_test(mpz_mul, result, a, b);	
			}

			size_t i;
			for (i = num_size; i < STEP * num_size; i++)
			{
				mpz_add(running_val, running_val, twoTo64Minus1);
				mpz_mul(running_val, running_val, twoTo64);
			}
			printf("%zu,%.30Le,%.30Le,%.30Le\n", num_size, running_add, running_sub, running_mul);
		}
	}
	else if (strcmp(argv[1], "3") == 0)
	{
		mpz_t a;
		mpz_init_set_ui(a, MAX_INTEGER_VALUE);
		mpz_add_ui(a, a, 1);
		size_t power;
		for (power = MINPOWER; power <= MAXPOWER; power *= 2)
		{
			long double running_power = 0.0;
			size_t j;
			for (j = 0; j < NUMTESTS; j++)
			{
				mpz_t result;
				mpz_init(result);
				timeval start, end;
				gettimeofday(&start, NULL);
				mpz_pow_ui(result, a, power);
				gettimeofday(&end, NULL);
				running_power += ((long double) end.tv_sec - start.tv_sec) + ((long double) end.tv_usec - (long double) start.tv_usec) / MICRO_FACTOR;
			}
			printf("%zu,%.30Le\n", power, running_power);
		}
	}
	
	return 0;
}
