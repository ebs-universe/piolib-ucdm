#include <unity.h>
#include <string.h>
#include <ucdm/ucdm.h>
#include <scaffold.h>

#define RWH_ON_ADDR     0x12
#define BWH_ON_ADDR     0x13
#define BOTH_ON_ADDR    0x16
#define RWHN_ON_ADDR    0x18
#define BWHN_ON_ADDR    0x1C

typedef struct SIGNALS_t{
    uint8_t rwh1;
    uint8_t rwh2;
    uint8_t bwh1;
    uint8_t bwh2;
    uint16_t bwh1_mask;
    uint16_t bwh2_mask;
} signals_t;

signals_t signals;

void reset_signals (void){
    memset(&signals, 0, sizeof(signals));
}

void check_signals(signals_t * expected) {
    TEST_ASSERT_EQUAL_UINT8(expected->rwh1, signals.rwh1);
    TEST_ASSERT_EQUAL_UINT8(expected->rwh2, signals.rwh2);
    TEST_ASSERT_EQUAL_UINT8(expected->bwh1, signals.bwh1);
    TEST_ASSERT_EQUAL_UINT8(expected->bwh2, signals.bwh2);
    TEST_ASSERT_EQUAL_UINT16(expected->bwh1_mask, signals.bwh1_mask);
    TEST_ASSERT_EQUAL_UINT16(expected->bwh2_mask, signals.bwh2_mask);
}

void rwh_1(ucdm_addr_t addr){
    signals.rwh1 = addr;
}

void rwh_2(ucdm_addr_t addr){
    signals.rwh2 = addr;
}

void bwh_1(ucdm_addr_t addr, uint16_t mask){
    signals.bwh1 = addr;
    signals.bwh1_mask = mask;
}

void bwh_2(ucdm_addr_t addr, uint16_t mask){
    signals.bwh2 = addr;
    signals.bwh2_mask = mask;
}

avlt_node_t rwh1_node, rwh2_node, rwh3_node;
avlt_node_t bwh1_node, bwh2_node, bwh3_node;

void setup(void){
    ucdm_enable_regw(RWH_ON_ADDR);
    ucdm_enable_regw(BWH_ON_ADDR);
    ucdm_enable_regw(BOTH_ON_ADDR);
    ucdm_enable_regw(RWHN_ON_ADDR);
    ucdm_enable_regw(BWHN_ON_ADDR);

    ucdm_enable_bitw(RWH_ON_ADDR);
    ucdm_enable_bitw(BWH_ON_ADDR);
    ucdm_enable_bitw(BOTH_ON_ADDR);
    ucdm_enable_bitw(BWHN_ON_ADDR);
    ucdm_enable_bitw(RWHN_ON_ADDR);

    ucdm_install_regw_handler(RWH_ON_ADDR, &rwh1_node, rwh_1);
    ucdm_install_regw_handler(BOTH_ON_ADDR, &rwh2_node, rwh_2);
    ucdm_install_regw_handler(RWHN_ON_ADDR, &rwh3_node, NULL);

    ucdm_install_bitw_handler(BWH_ON_ADDR, &bwh1_node, bwh_1);
    ucdm_install_bitw_handler(BOTH_ON_ADDR, &bwh2_node, bwh_2);
    ucdm_install_bitw_handler(BWHN_ON_ADDR, &bwh3_node, NULL);
}

void test_rwh_rw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_register(RWH_ON_ADDR, 0x1010);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = RWH_ON_ADDR,
        .rwh2 = 0,
        .bwh1 = 0,
        .bwh2 = 0,
        .bwh1_mask = 0,
        .bwh2_mask = 0
    };

    check_signals(&expected);
}


void test_rwh_bw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_bit(RWH_ON_ADDR << 4 | 2);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = RWH_ON_ADDR,
        .rwh2 = 0,
        .bwh1 = 0,
        .bwh2 = 0,
        .bwh1_mask = 0,
        .bwh2_mask = 0
    };

    check_signals(&expected);
}


void test_bwh_rw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_register(BWH_ON_ADDR, 0x0012);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = 0,
        .rwh2 = 0,
        .bwh1 = 0,
        .bwh2 = 0,
        .bwh1_mask = 0,
        .bwh2_mask = 0
    };

    check_signals(&expected);
}

void test_bwh_bw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_bit(BWH_ON_ADDR << 4 | 3);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = 0,
        .rwh2 = 0,
        .bwh1 = BWH_ON_ADDR,
        .bwh2 = 0,
        .bwh1_mask = (1<<3),
        .bwh2_mask = 0
    };

    check_signals(&expected);
}

void test_both_rw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_register(BOTH_ON_ADDR, 0x1212);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = 0,
        .rwh2 = BOTH_ON_ADDR,
        .bwh1 = 0,
        .bwh2 = 0,
        .bwh1_mask = 0,
        .bwh2_mask = 0
    };

    check_signals(&expected);
}

void test_both_bw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_bit(BOTH_ON_ADDR << 4 | 4);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = 0,
        .rwh2 = 0,
        .bwh1 = 0,
        .bwh2 = BOTH_ON_ADDR,
        .bwh1_mask = 0,
        .bwh2_mask = (1<<4)
    };

    check_signals(&expected);
}

void test_rwhn_rw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_register(RWHN_ON_ADDR, 0x1223);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = 0,
        .rwh2 = 0,
        .bwh1 = 0,
        .bwh2 = 0,
        .bwh1_mask = 0,
        .bwh2_mask = 0
    };

    check_signals(&expected);
}


void test_bwhn_bw(void){
    HAL_BASE_t result;
    reset_signals();
    result = ucdm_set_bit(RWHN_ON_ADDR << 4 | 2);
    TEST_ASSERT_EQUAL(0, result);
    signals_t expected = {
        .rwh1 = 0,
        .rwh2 = 0,
        .bwh1 = 0,
        .bwh2 = 0,
        .bwh1_mask = 0,
        .bwh2_mask = 0
    };

    check_signals(&expected);
}


int main(void) {
    init();
    UNITY_BEGIN();
    setup();
    RUN_TEST(test_rwh_rw);
    RUN_TEST(test_rwh_bw);
    RUN_TEST(test_bwh_rw);
    RUN_TEST(test_bwh_bw);
    RUN_TEST(test_both_rw);
    RUN_TEST(test_both_bw);
    RUN_TEST(test_rwhn_rw);
    RUN_TEST(test_bwhn_bw);
    return UNITY_END();
}
