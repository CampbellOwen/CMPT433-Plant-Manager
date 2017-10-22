#include "device_array_test.h"
#include "device_manager_test.h"
#include <stdio.h>

int main( void )
{
	device_array_test();

	device_manager_test();

	printf("ALL TESTS PASSED\n");

	return 0;
}
