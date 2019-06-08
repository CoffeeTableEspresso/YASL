#include "whiletest.h"
#include "yats.h"

SETUP_YATS();

static void test_while() {
	unsigned char expected[] = {
		0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		BCONST_T,
		BRF_8,
		0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		BCONST_T,
		PRINT,
		BR_8,
		0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		HALT
	};
	ASSERT_GEN_BC_EQ(expected, "while true { echo true; };");
}

static void test_continue() {
	unsigned char expected[] = {
		0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		'i',
		ICONST_0,
		GSTORE_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		GLOAD_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		ICONST,
		0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		GE, NOT,
		BRF_8,
		0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		GLOAD_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		ICONST_5,
		EQ,
		BRF_8,
		0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		BR_8,
		0xB6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		GLOAD_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		PRINT,
		BR_8,
		0x9B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		HALT
	};
	ASSERT_GEN_BC_EQ(expected, "let i = 0; while i < 10 { if i == 5 { continue; }; echo i; };");
}

static void test_break() {
	unsigned char expected[] = {
		0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		'i',
		ICONST_0,
		GSTORE_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		GLOAD_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		ICONST,
		0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		GE, NOT,
		BRF_8,
		0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		GLOAD_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		ICONST_5,
		EQ,
		BRF_8,
		0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		BCONST_F,
		BR_8,
		0xD1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		GLOAD_8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		PRINT,
		BR_8,
		0x9A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		HALT
	};
	ASSERT_GEN_BC_EQ(expected, "let i = 0; while i < 10 { if i == 5 { break; }; echo i; };");
}

int whiletest(void) {
	test_while();
	test_continue();
	test_break();
	return __YASL_TESTS_FAILED__;
}
