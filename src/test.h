#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

struct test {
	int (*func)(void);
	char name[256];
};

struct test_batch {
	size_t count;
	struct test *ptr;
};

/* Macro for easier test exporting */
#define TEST(NAME) { NAME, #NAME }
/* Export a literal array of tests as a test batch structure.
 * Note that trailing commas will break the counter.
 * sizeof(test_list) / sizeof(struct test) should give the number of elements
 * in a compile-time constant array---works with initial test batches. */
#define EXPORT(test_list, name) \
	struct test_batch name ## _tests = { \
		.count = sizeof(test_list) / sizeof(struct test), \
		.ptr = test_list \
	}; \

int run_tests (struct test_batch tests);

extern struct test_batch actor_tests;
extern struct test_batch world_tests;
extern struct test_batch log_tests;
extern struct test_batch state_tests;

// Good enough for government work
#define is_close(a,b) ( ((a) - (b))*((a) - (b)) <= DBL_EPSILON )


#endif

