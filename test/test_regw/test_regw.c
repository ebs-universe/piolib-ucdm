
#include <unity.h>
#include <ucdm/ucdm.h>
#include <scaffold.h>

#define SUCCESS 0
uint16_t dummy_target;
uint16_t mock_target;

void mock_function(ucdm_addr_t addr, uint16_t value) {
    mock_target = value;
}

void test_ucdm_disable_regw(void) {
    ucdm_addr_t addr = 0x11;
    HAL_BASE_t result;
    
    result = ucdm_disable_regw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result); 
    
    result = ucdm_set_register(addr, 0x0101);
    TEST_ASSERT_EQUAL(2, result);
}

void test_ucdm_enable_regw_normal(void) {
    ucdm_addr_t addr = 0x12;
    HAL_BASE_t result;
    
    result = ucdm_enable_regw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    result = ucdm_set_register(addr, 0x0101);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x0101, ucdm_register[addr].data);
}

void test_ucdm_redirect_regw_ptr(void) {
    ucdm_addr_t addr = 0x13;
    HAL_BASE_t result;

    result = ucdm_redirect_regw_ptr(addr, &dummy_target);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_set_register(addr, 0x0123);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x0123, dummy_target);
}

void test_ucdm_redirect_regw_ptr_null(void) {
    ucdm_addr_t addr = 0x14;
    HAL_BASE_t result;
    
    result = ucdm_redirect_regw_ptr(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_set_register(addr, 0x0101);
    TEST_ASSERT_EQUAL(3, result);
}

void test_ucdm_redirect_regw_func(void) {
    ucdm_addr_t addr = 0x15;
    HAL_BASE_t result;

    result = ucdm_redirect_regw_func(addr, mock_function);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_register(addr, 0x5123);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x5123, mock_target);
}

void test_ucdm_redirect_regw_func_null(void) {
    ucdm_addr_t addr = 0x16;
    HAL_BASE_t result;
    
    result = ucdm_redirect_regw_func(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_register(addr, 0x3333);
    TEST_ASSERT_EQUAL(3, result);
}

int main(void) {
    init();
    UNITY_BEGIN();
    RUN_TEST(test_ucdm_disable_regw);
    RUN_TEST(test_ucdm_enable_regw_normal);
    RUN_TEST(test_ucdm_redirect_regw_ptr);
    RUN_TEST(test_ucdm_redirect_regw_ptr_null);
    RUN_TEST(test_ucdm_redirect_regw_func);
    RUN_TEST(test_ucdm_redirect_regw_func_null);
    return UNITY_END();
}
