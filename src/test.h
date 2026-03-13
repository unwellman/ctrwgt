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

int run_tests (struct test_batch tests);

extern struct test_batch actor_tests;
extern struct test_batch world_tests;

// Good enough for government work
#define is_close(a,b) ( ((a) - (b))*((a) - (b)) <= DBL_EPSILON )

#endif

