#include <stddef.h>
#include <stdint.h>

typedef struct
{
	uint64_t *values;
	size_t assigned_values;
	size_t array_size;
} Integer;

uint8_t integer_initialize(Integer *a);
void integer_uninitialize(Integer *a);
Integer integer_assign_from_int(Integer *dest, uint64_t source);
Integer integer_assign_from_integer(Integer *dest, Integer *source);
char *integer_to_string(Integer *a);
uint8_t resize_if_necessary(Integer *a, size_t needed_size);
