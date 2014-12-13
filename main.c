#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "Integer.h"

const uint64_t MAX_INTEGER_VAL = 0xffffffffffffffff;

void print_status(char *var, Integer *a)
{
	long long int i;
	for (i = a->array_size - 1; i >= 0; i--)
	{
		printf("%s.values[%llu] = %lu\n", var, i, a->values[i]);
	}
	printf("%s.assigned = %zu\n%s.array_size = %zu\n\n", var, a->assigned, var, a->array_size);
}

int main()
{
	Integer a, b, twoTo64;
	integer_initialize(&a);
	integer_initialize(&b);
	integer_initialize(&twoTo64);
	
	integer_assign_from_int(&a, MAX_INTEGER_VAL);
	printf("After assigning a 0xffffffffffffffff:\n");
	print_status("a", &a);

	integer_assign_from_integer(&b, &a);
	printf("After assigning b from a:\n");
	print_status("b", &b);

	integer_add_int(&a, &a, 1);
	printf("After adding 1 to a:\n");
	print_status("a", &a);
	integer_assign_from_integer(&twoTo64, &a);

	integer_add_int(&a, &a, MAX_INTEGER_VAL);
	printf("After adding 0xffffffffffffffff to a:\n");
	print_status("a", &a);
	
	integer_add_int(&b, &b, 2);
	printf("After adding 2 to b:\n");
	print_status("b", &b);
	
	integer_add_integer(&a, &a, &b);
	printf("After adding b to a:\n");
	print_status("a", &a);

	printf("After subtracting 0xffffffffffffffff from a:\n");
	integer_subtract_int(&a, &a, MAX_INTEGER_VAL);
	print_status("a", &a);

	printf("After assigning b from a and subracting 1:\n");
	integer_assign_from_integer(&b, &a);
	integer_subtract_int(&b, &b, 1);
	print_status("b", &b);

	integer_subtract_integer(&a, &a, &b);
	printf("After subtracting b from a:\n");
	print_status("a", &a);

	integer_assign_from_integer(&b, &a);
	integer_subtract_int(&b, &b, 1);
	printf("Assigned a to b and subtracted 1.\n");
	print_status("b", &b);

	integer_subtract_integer(&a, &a, &b);
	printf("Subtracted b from a:\n");
	print_status("a", &a);

	printf("Subtracting (1 0 0)_2^64 - 1 from (1 0 0)_2^64:\n");
	integer_multiply_integer(&a, &twoTo64, &twoTo64);
	integer_subtract_int(&b, &a, 1);
	integer_subtract_integer(&a, &a, &b);
	print_status("a", &a);

	integer_assign_from_int(&a, 2);
	integer_multiply_int(&a, &a, 2);
	printf("Multiplying a = 2 by 2:\n");
	print_status("a", &a);

	integer_assign_from_int(&a, MAX_INTEGER_VAL);
	integer_multiply_int(&a, &a, 2);
	printf("Multiplying a = 2 ^ 64 - 1 by 2:\n");
	print_status("a", &a);

	integer_assign_from_int(&a, MAX_INTEGER_VAL);
	integer_assign_from_int(&b, 2);

	print_status("a", &a);
	print_status("b", &b);

	integer_multiply_integer(&a, &a, &b);
	printf("Multiplied a by b:\n");
	print_status("a", &a);

	integer_assign_from_int(&b, MAX_INTEGER_VAL);
	integer_multiply_integer(&a, &a, &b);
	printf("Multiplying a by 0xffffffffffffffff in b:\n");
	print_status("a", &a);

	integer_assign_from_int(&a, MAX_INTEGER_VAL);
	integer_assign_from_int(&b, 2);
	karatsuba(&a, &a, &b);
	printf("After applying karatsuba to a and b:\n");
	print_status("a", &a);

	integer_assign_from_int(&b, 0xffffffff);
	karatsuba(&a, &a, &b);
	printf("Multiplied a by 0xffffffff:\n");
	print_status("a", &a);

	integer_assign_from_int(&b, MAX_INTEGER_VAL);
	integer_add_int(&b, &b, 1);
	karatsuba(&a, &a, &b);
	printf("Multipled a by b = (1 0)_2^64:\n");
	print_status("a", &a);

	printf("Multipying b = (1 0)_2^64 by 0xfabcd872:\n");
	integer_multiply_int(&b, &b, 0xfabcd872);
	print_status("b", &b);

	printf("Doing a = a * b:\n");
	integer_multiply_integer(&a, &a, &b);
	print_status("a", &a);

	printf("Doing a = (2^32)^3:\n");
	integer_assign_from_int(&a, (uint64_t) 0xffffffff + 1);
	integer_power_int(&a, &a, 3);
	print_status("a", &a);

	integer_assign_from_integer(&b, &a);

	printf("Doing a = a^2 = (2^96)^2:\n");
	integer_power_int(&a, &a, 2);
	print_status("a", &a);

	printf("Doing a = a^5 = (2^192)^5:\n");
	integer_power_int(&a, &a, 5);
	print_status("a", &a);

	print_status("b", &b);
	printf("Doing a = b^10 = (2^96)^10:\n");
	power_by_squaring(&a, &b, 10);	
	print_status("a", &a);

	integer_uninitialize(&a);
	integer_uninitialize(&b);
	integer_uninitialize(&twoTo64);
	return 0;
}
