#include <stdio.h>
#include <assert.h>

#include "test.h"

int run_tests (struct test_batch batch) {
	size_t count = batch.count;
	struct test *tests = batch.ptr;
	for (size_t i = 0; i < count; i++) {
		struct test curr = tests[i];
		printf("Running %s\n", curr.name);
		int result = curr.func();
		if (result)
			return result;
	}
	return 0;
}

int main () {
	assert(!run_tests(actor_tests));
	assert(!run_tests(world_tests));
	printf("All tests passing\n\n");
	return 0;
}

