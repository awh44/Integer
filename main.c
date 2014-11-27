#include <stdio.h>

#include "Integer.h"

int main()
{
	Integer a, b;
	integer_initialize(&a);
	integer_initialize(&b);
	
	integer_assign_from_int(&a, 0xffffffff);
	printf("After assigning a 0xffffffff:\n");
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_assign_from_integer(&b, &a);
	printf("After assigning b from a:\n");
	printf("b.values[0] = %lu\n", b.values[0]);
	printf("b.assigned_values = %zu\n", b.assigned_values);
	printf("b.array_size = %zu\n\n", b.array_size);

	integer_assign_from_int(&a, 4294967296);
	printf("After assigning a 4294965296 (2^32):\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&a, &a, 1);
	printf("After adding 1 to a:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&a, &a, 4294967296);
	printf("After adding 4294967296 to a:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&a, &a, 4294967295);
	printf("After adding 4294967295 to a:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&a, &a, 4294967296 * 2);
	printf("After adding 2 * 4294967296 to a:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&b, &b, 2);
	printf("After adding 2 to b:\n");
	printf("b.values[1] = %lu\n", b.values[1]);
	printf("b.values[0] = %lu\n", b.values[0]);
	printf("b.assigned_values = %zu\n", b.assigned_values);
	printf("b.array_size = %zu\n\n", b.array_size);

	integer_add_integer(&a, &a, &b);
	printf("After adding b to a:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&a, &a, 0xffffffffffffffff);
	printf("After adding 0xffffffffffffffff to a:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_add_int(&a, &a, 0xffffffffffffffff);
	printf("After adding 0xffffffffffffffff to a again:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	a.values[0] = 0xffffffffffffffff;
	a.values[1] = 0xffffffffffffffff;
	printf("After setting a.values[0] and a.values[1] = 0xffffffffffffffff:\n");
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n\n", a.array_size);

	integer_assign_from_integer(&b, &a);
	printf("After assigning b from a:\n");
	printf("b.values[1] = %lu\n", b.values[1]);
	printf("b.values[0] = %lu\n", b.values[0]);
	printf("b.assigned_values = %zu\n", b.assigned_values);
	printf("b.array_size = %zu\n\n", b.array_size);

	integer_add_integer(&a, &a, &b);
	printf("After adding b to a:\n");
	printf("a.values[2] = %lu\n", a.values[2]);
	printf("a.values[1] = %lu\n", a.values[1]);
	printf("a.values[0] = %lu\n", a.values[0]);
	printf("a.assigned_values = %zu\n", a.assigned_values);
	printf("a.array_size = %zu\n", a.array_size);

	integer_uninitialize(&a);
	integer_uninitialize(&b);
	return 0;
}
