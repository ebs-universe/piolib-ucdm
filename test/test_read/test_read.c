#include <unity.h>
#include <ucdm/ucdm.h>
#include "../scaffold/common.c"

#define SUCCESS 0
uint16_t dummy_target=0x0211;

uint16_t mock_function(ucdm_addr_t addr) {
    return 0x5678;
}

void test_ucdm_disable_regr(void) {
    ucdm_addr_t addr = 0x01;
    uint16_t read_result;
    ucdm_register[addr].data = 0x0101;

    HAL_BASE_t result = ucdm_disable_regr(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result); 
    
    read_result = ucdm_get_register(0x01);
    TEST_ASSERT_EQUAL(0xFFFF, read_result);
}

void test_ucdm_enable_regr_normal(void) {
    ucdm_addr_t addr = 0x02;
    uint16_t read_result;
    ucdm_register[addr].data = 0x0101;

    HAL_BASE_t result = ucdm_enable_regr(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    read_result = ucdm_get_register(addr);
    TEST_ASSERT_EQUAL(0x0101, read_result);
}

void test_ucdm_redirect_regr_ptr(void) {
    ucdm_addr_t addr = 0x03;
    uint16_t read_result;

    HAL_BASE_t result = ucdm_redirect_regr_ptr(addr, &dummy_target);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    read_result = ucdm_get_register(addr);
    TEST_ASSERT_EQUAL(0x0211, read_result);
}

void test_ucdm_redirect_regr_ptr_null(void) {
    ucdm_addr_t addr = 0x04;
    uint16_t read_result;

    HAL_BASE_t result = ucdm_redirect_regr_ptr(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    read_result = ucdm_get_register(addr);
    TEST_ASSERT_EQUAL(0xFFFF, read_result);
}

void test_ucdm_redirect_regr_func(void) {
    ucdm_addr_t addr = 0x05;
    uint16_t read_result;

    HAL_BASE_t result = ucdm_redirect_regr_func(addr, mock_function);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    read_result = ucdm_get_register(addr);
    TEST_ASSERT_EQUAL(0x5678, read_result);
}

void test_ucdm_redirect_regr_func_null(void) {
    ucdm_addr_t addr = 0x06;
    uint16_t read_result;

    HAL_BASE_t result = ucdm_redirect_regr_func(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    read_result = ucdm_get_register(addr);
    TEST_ASSERT_EQUAL(0xFFFF, read_result);
}

void test_ucdm_get_bit_normal(void) {
    ucdm_addr_t addr = 0x07;
    ucdm_addrb_t addrb_set = addr << 0x04 | 0xC;
    ucdm_addrb_t addrb_clr = addr << 0x04 | 0xD;
    HAL_BASE_t result;
    
    result = ucdm_enable_regr(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    ucdm_register[addr].data = 0x1000;

    result = ucdm_get_bit(addrb_set);
    TEST_ASSERT_EQUAL(0xFF, result);

    result = ucdm_get_bit(addrb_clr);
    TEST_ASSERT_EQUAL(0x00, result);
}

void test_ucdm_get_bit_pointer(void) {
    ucdm_addr_t addr = 0x08;
    ucdm_addrb_t addrb_set = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_clr = addr << 0x04 | 0xD;

    HAL_BASE_t result;
    result = ucdm_redirect_regr_ptr(addr, &dummy_target);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_get_bit(addrb_set);
    TEST_ASSERT_EQUAL(0xFF, result);

    result = ucdm_get_bit(addrb_clr);
    TEST_ASSERT_EQUAL(0x00, result);
}

void test_ucdm_get_bit_noread(void) {
    ucdm_addr_t addr = 0x09;
    ucdm_addrb_t addrb_set = addr << 0x04 | 0xC;
    ucdm_addrb_t addrb_clr = addr << 0x04 | 0xD;
    HAL_BASE_t result;

    result = ucdm_disable_regr(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    ucdm_register[addr].data = 0x1000;

    result = ucdm_get_bit(addrb_set);
    TEST_ASSERT_EQUAL(2, result);

    result = ucdm_get_bit(addrb_clr);
    TEST_ASSERT_EQUAL(2, result);
}

void test_ucdm_get_bit_nullptr(void) {
    ucdm_addr_t addr = 0x0A;
    ucdm_addrb_t addrb_set = addr << 0x04 | 0xC;
    ucdm_addrb_t addrb_clr = addr << 0x04 | 0xD;
    HAL_BASE_t result;

    result = ucdm_redirect_regr_ptr(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_get_bit(addrb_set);
    TEST_ASSERT_EQUAL(3, result);

    result = ucdm_get_bit(addrb_clr);
    TEST_ASSERT_EQUAL(3, result);
}

int main(void) {
    libinit();
    UNITY_BEGIN();
    RUN_TEST(test_ucdm_disable_regr);
    RUN_TEST(test_ucdm_enable_regr_normal);
    RUN_TEST(test_ucdm_redirect_regr_ptr);
    RUN_TEST(test_ucdm_redirect_regr_ptr_null);
    RUN_TEST(test_ucdm_redirect_regr_func);
    RUN_TEST(test_ucdm_redirect_regr_func_null);
    RUN_TEST(test_ucdm_get_bit_normal);
    RUN_TEST(test_ucdm_get_bit_pointer);
    RUN_TEST(test_ucdm_get_bit_noread);
    RUN_TEST(test_ucdm_get_bit_nullptr);
    return UNITY_END();
}
