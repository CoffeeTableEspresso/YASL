#include "whiletest.h"
#include "yats.h"

SETUP_YATS();

static void test_while() {
    unsigned char expected[] = {
            0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            BCONST_T,
            BRF_8,
            0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            BCONST_T,
            POP,
            BR_8,
            0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            HALT
    };
    ASSERT_GEN_BC_EQ(expected,"while true { true; };");
}

static void test_continue() {
    unsigned char expected[] = {
            0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            ICONST,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            GSTORE_1, 0x00,
            GLOAD_1, 0x00,
            ICONST,
            0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            GE, NOT,
            BRF_8,
            0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            GLOAD_1, 0x00,
            ICONST,
            0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            EQ,
            BRF_8,
            0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            BR_8,
            0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            GLOAD_1, 0x00,
            PRINT,
            BR_8,
            0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            HALT
    };
    ASSERT_GEN_BC_EQ(expected,"let i = 0; while i < 10 { if i == 5 { continue; }; echo i; };");
}

static void test_break() {
    unsigned char expected[] = {
            0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            ICONST,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            GSTORE_1, 0x00,
            GLOAD_1, 0x00,
            ICONST,
            0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            GE, NOT,
            BRF_8,
            0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            GLOAD_1, 0x00,
            ICONST,
            0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            EQ,
            BRF_8,
            0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            BCONST_F,
            BR_8,
            0xD8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            GLOAD_1, 0x00,
            PRINT,
            BR_8,
            0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            HALT
    };
    ASSERT_GEN_BC_EQ(expected,"let i = 0; while i < 10 { if i == 5 { break; }; echo i; };");
}

int whiletest(void) {
    test_while();
    test_continue();
    test_break();
    return __YASL_TESTS_FAILED__;
}