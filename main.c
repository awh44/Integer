#include <stdio.h>

#include "Integer.h"

void print_status(char *var, Integer *a)
{
	long long int i;
	for (i = a->assigned - 1; i >= 0; i--)
	{
		printf("%s.values[%llu] = %lu\n", var, i, a->values[i]);
	}
	printf("%s.assigned = %zu\n%s.array_size = %zu\n\n", var, a->assigned, var, a->array_size);
}

int main()
{
	Integer a, b;
	integer_initialize(&a);
	integer_initialize(&b);
	
	integer_assign_from_int(&a, 0xffffffff);
	printf("After assigning a 0xffffffff:\n");
	print_status("a", &a);

	integer_assign_from_integer(&b, &a);
	printf("After assigning b from a:\n");
	print_status("b", &b);

	integer_assign_from_int(&a, 4294967296);
	printf("After assigning a 4294965296 (2^32):\n");
	print_status("a", &a);

	integer_add_int(&a, &a, 1);
	printf("After adding 1 to a:\n");
	print_status("a", &a);

	integer_add_int(&a, &a, 4294967296);
	printf("After adding 4294967296 to a:\n");
	print_status("a", &a);

	integer_add_int(&a, &a, 4294967295);
	printf("After adding 4294967295 to a:\n");
	print_status("a", &a);
	
	integer_add_int(&a, &a, 4294967296 * 2);
	printf("After adding 2 * 4294967296 to a:\n");
	print_status("a", &a);

	integer_add_int(&b, &b, 2);
	printf("After adding 2 to b:\n");
	print_status("b", &b);
	
	integer_add_integer(&a, &a, &b);
	printf("After adding b to a:\n");
	print_status("a", &a);
	
	integer_add_int(&a, &a, 0xffffffffffffffff);
	printf("After adding 0xffffffffffffffff to a:\n");
	print_status("a", &a);

	integer_add_int(&a, &a, 0xffffffffffffffff);
	printf("After adding 0xffffffffffffffff to a again:\n");
	print_status("a", &a);
	
	a.values[0] = 0xffffffffffffffff;
	a.values[1] = 0xffffffffffffffff;
	printf("After setting a.values[0] and a.values[1] = 0xffffffffffffffff:\n");
	print_status("a", &a);
	
	integer_assign_from_integer(&b, &a);
	printf("After assigning b from a:\n");
	print_status("b", &b);
	
	integer_add_integer(&a, &a, &b);
	printf("After adding b to a:\n");
	print_status("a", &a);

	integer_subtract_int(&a, &a, 0xffffffffffffffff);
	printf("After subtracting 0xffffffffffffffff from a:\n");
	print_status("a", &a);

	integer_assign_from_integer(&b, &a);
	integer_subtract_int(&b, &b, 1);
	printf("After assigning b from a and subracting 1:\n");
	print_status("b", &b);

	integer_subtract_integer(&a, &a, &b);
	printf("After subtracting b from a:\n");
	print_status("a", &a);

	integer_assign_from_integer(&a, &b);
	a.values[0] = 0;
	a.values[1] = 0;
	printf("Assigned a 1 0 0 (base 64):\n");
	print_status("a", &a);

	integer_assign_from_integer(&b, &a);
	integer_subtract_int(&b, &b, 1);
	printf("Assigned a to b and subtracted 1.\n");
	print_status("b", &b);

	integer_subtract_integer(&a, &a, &b);
	printf("Subtracted b from a:\n");
	print_status("a", &a);

	integer_assign_from_int(&a, 2);
	integer_multiply_int(&a, &a, 2);
	printf("Multiplying a = 2 by 2:\n");
	print_status("a", &a);

	integer_assign_from_int(&a, 0xffffffffffffffff);
	integer_multiply_int(&a, &a, 2);
	printf("Multiplying a = 2 ^ 64 - 1 by 2:\n");
	print_status("a", &a);

	size_t i;
	for (i = 0; i < 2; i++)
	{
		integer_multiply_int(&a, &a, 0xffffffff);
		printf("Multiplying a by 2^32 - 1:\n");
		print_status("a", &a);
	}


			
	integer_uninitialize(&a);
	integer_uninitialize(&b);
	return 0;
}
