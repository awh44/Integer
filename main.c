#include <stdio.h>

#include "Integer.h"

int main()
{
	Integer a, b;
	integer_initialize(&a);
	integer_initialize(&b);
	
	integer_assign_from_int(&a, 0xffffffff);
	printf("After assigning a 0xffffffff:\n");
	printf("a.values[0] = %u\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n", a.array_size);

	integer_assign_from_integer(&b, &a);
	printf("\nAfter assigning b from a:\n");
	printf("b.values[0] = %u\n", b.values[0]);
	printf("b.assigned_values = %zu\n", b.assigned_values);
	printf("b.array_size = %zu\n", b.array_size);

	integer_assign_from_int(&a, 4294967296);
	printf("\nAfter assigning a 4294965296 (2^32):\n");
	printf("a.values[1] = %u\n", a.values[1]);
	printf("a.values[0] = %u\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n", a.array_size);

	a = integer_add_int(&a, 1);
	printf("\nAfter adding 1 to a:\n");
	printf("a.values[1] = %u\n", a.values[1]);
	printf("a.values[0] = %u\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n", a.array_size);

	a = integer_add_int(&a, 4294967296);
	printf("\nAfter adding 4294967296 to a:\n");
	printf("a.values[1] = %u\n", a.values[1]);
	printf("a.values[0] = %u\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n", a.array_size);

	integer_uninitialize(&a);
	integer_uninitialize(&b);
	return 0;
}
