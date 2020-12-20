#include "literaltest.h"
#include "yats.h"

SETUP_YATS();

static void test_elimination() {
	unsigned char expected[] = {
		0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 1,
		O_LIT, 0x00,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "let x = 1; x; undef; true; 'YASL'; 10; 10.0;");
}

static void test_undef() {
	unsigned char expected[] = {
		0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		O_NCONST,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo undef;");
}

static void test_true() {
	unsigned char expected[] = {
		0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		O_BCONST_T,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo true;");
}

static void test_false() {
	unsigned char expected[] = {
		0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		O_BCONST_F,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo false;");
}

static void test_small_ints() {
	unsigned char expected[] = {
		0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 0xFF,
		C_INT_1, 0,
		C_INT_1, 1,
		C_INT_1, 2,
		C_INT_1, 3,
		C_INT_1, 4,
		C_INT_1, 5,
		O_LIT, 0x00,
		O_ECHO,
		O_LIT, 0x01,
		O_ECHO,
		O_LIT, 0x02,
		O_ECHO,
		O_LIT, 0x03,
		O_ECHO,
		O_LIT, 0x04,
		O_ECHO,
		O_LIT, 0x05,
		O_ECHO,
		O_LIT, 0x06,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo -1; echo 0; echo 1; echo 2; echo 3; echo 4; echo 5;");
}

static void test_bin() {
	unsigned char expected[] = {
		0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 3,
		O_LIT, 0x00,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo 0b11;");
}

static void test_dec() {
	unsigned char expected[] = {
		0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 0x0A,
		O_LIT, 0x00,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo 10;");
}

static void test_hex() {
	unsigned char expected[] = {
		0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 0x10,
		O_LIT, 0x00,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo 0x10;");
}

static void test_small_floats() {
	unsigned char expected[] = {
		0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_FLOAT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_FLOAT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F,
		C_FLOAT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
		O_LIT, 0x00,
		O_ECHO,
		O_LIT, 0x01,
		O_ECHO,
		O_LIT, 0x02,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo 0.0; echo 1.0; echo 2.0;");
}

static void test_float() {
	unsigned char expected[] = {
		0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_FLOAT,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F,
		O_LIT, 0x00,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo 1.5;");
}

static void test_string() {
	unsigned char expected[] = {
		0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_STR,
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		'Y', 'A', 'S', 'L',
		O_LIT, 0x00,
		O_ECHO,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "echo 'YASL';");
}

static void test_list() {
	unsigned char expected[] = {
		0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 0,
		C_INT_1, 1,
		C_INT_1, 2,
		C_INT_1, 3,
		C_INT_1, 4,
		O_END,
		O_LIT,
		0x00,
		O_LIT,
		0x01,
		O_LIT,
		0x02,
		O_LIT,
		0x03,
		O_LIT,
		0x04,
		O_NEWLIST,
		O_POP,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "[0, 1, 2, 3, 4];");
}

static void test_table() {
	unsigned char expected[] = {
		0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		C_INT_1, 0,
		C_STR,
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		'z', 'e', 'r', 'o',
		C_INT_1, 1,
		C_STR,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		'o', 'n', 'e',
		O_END,
		O_LIT, 0x00,
		O_LIT, 0x01,
		O_LIT, 0x02,
		O_LIT, 0x03,
		O_NEWTABLE,
		O_POP,
		O_HALT
	};
	ASSERT_GEN_BC_EQ(expected, "{0:'zero', 1:'one'};");
}

TEST(literaltest) {
	test_undef();
	test_true();
	test_false();
	test_small_ints();
	test_bin();
	test_dec();
	test_hex();
	test_small_floats();
	test_float();
	test_string();
	test_list();
	test_table();
	test_elimination();

	return NUM_FAILED;
}
