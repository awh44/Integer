#include <stdio.h>

#include "Integer.h"

int main()
{
	Integer a, b;
	integer_initialize(&a);
	integer_initialize(&b);

	integer_assign_from_int(&a, 5000000);
	integer_assign_from_integer(&b, &a);

	printf("%lu\n", a.values[0]);
	printf("%lu\n", b.values[0]);

	integer_uninitialize(&a);
	integer_uninitialize(&b);
	return 0;
}
