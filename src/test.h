#ifndef TEST_H
#define TEST_H
#include <stdio.h>

struct test {
	int (*func)(void);
	char name[256];
};

int test_actor (void);

#endif

