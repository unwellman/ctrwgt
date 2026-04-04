#include "test.h"
#include "log.h"


static int test_log_trace (void) {
	return 0;
}

static int test_log_debug (void) {
	return 0;
}

static int test_log_info (void) {
	return 0;
}

static int test_log_warn (void) {
	return 0;
}

static int test_log_error (void) {
	return 0;
}

static int test_log_critical (void) {
	return 0;
}

static struct test TEST_LIST[] = {
	TEST(test_log_trace),
	TEST(test_log_debug),
	TEST(test_log_info),
	TEST(test_log_warn),
	TEST(test_log_error),
	TEST(test_log_critical)
};
EXPORT(TEST_LIST, log)

