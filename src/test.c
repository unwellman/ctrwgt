#include <stdio.h>
#include <assert.h>
#include <SDL3/SDL.h>
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#include "test.h"
#include "log.h"

int run_tests (struct test_batch batch) {
	struct test *tests = batch.ptr;
	for (size_t i = 0; i < batch.count; i++) {
		struct test curr = tests[i];
		printf("Running %zu %s\n", i, curr.name);
		int result = curr.func();
		if (result)
			return result;
	}
	return 0;
}

int main () {
	assert(!run_tests(actor_tests));
	assert(!run_tests(world_tests));
	assert(!run_tests(log_tests));
	assert(!run_tests(state_tests));
	printf("All tests passing\n\n");
	return 0;
}

