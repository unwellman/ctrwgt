#include "test.h"
#include "log.h"


static int test_log_trace (void) {
	int i = 0;
	log_info("This is a trace log (%d)", i);
	return 0;
}

static int test_log_debug (void) {
	int i = 1;
	log_info("This is a debug log (%d)", i);
	return 0;
}

static int test_log_info (void) {
	int i = 2;
	log_info("This is an info log (%d)", i);
	return 0;
}

static int test_log_warn (void) {
	int i = 3;
	log_info("This is a warning log (%d)", i);
	return 0;
}

static int test_log_error (void) {
	int i = 4;
	log_info("This is an error log (%d)", i);
	return 0;
}

static int test_log_critical (void) {
	int i = 5;
	log_info("This is a fatal log (%d)", i);
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

