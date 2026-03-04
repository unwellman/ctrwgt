#include <stdio.h>
#include <assert.h>

#include "test.h"

int main () {
	assert(!test_actor());
	printf("All tests passing\n\n");
	return 0;
}

