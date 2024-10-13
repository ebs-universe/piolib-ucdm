
#include <unity.h>
#include <ucdm/ucdm.h>
#include <scaffold.h>

#define SUCCESS 0
#define INITVAL 0x0100

uint16_t dummy_target;
uint16_t mock_target;

void mock_function(ucdm_addr_t addr, uint16_t value) {
    mock_target = value;
}

void test_ucdm_bitw_ro_disable(void) {
    ucdm_addr_t addr = 0x10;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0xA;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x9;
    HAL_BASE_t result;

    ucdm_register[addr].data = INITVAL;
    
    result = ucdm_disable_regw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_disable_bitw(addr); 
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL(INITVAL, ucdm_register[addr].data);

    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL(INITVAL, ucdm_register[addr].data);
}

void test_ucdm_bitw_ro_enable(void) {
    ucdm_addr_t addr = 0x12;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0xA;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x9;
    HAL_BASE_t result;
    
    ucdm_register[addr].data = INITVAL;

    result = ucdm_disable_regw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_enable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(3, result);
    TEST_ASSERT_EQUAL(INITVAL, ucdm_register[addr].data);

    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(3, result);
    TEST_ASSERT_EQUAL(INITVAL, ucdm_register[addr].data);
}

void test_ucdm_bitw_normal_disable(void) {
    ucdm_addr_t addr = 0x13;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0xA;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x9;

    HAL_BASE_t result;
    ucdm_register[addr].data = INITVAL;
    
    result = ucdm_enable_regw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_disable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL(INITVAL, ucdm_register[addr].data);

    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL(INITVAL, ucdm_register[addr].data);
}

void test_ucdm_bitw_normal_enable(void) {
    ucdm_addr_t addr = 0x14;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;
    
    HAL_BASE_t result;
    ucdm_register[addr].data = INITVAL;
    
    result = ucdm_enable_regw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_enable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x0300, ucdm_register[addr].data);

    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x0200, ucdm_register[addr].data);
}

void test_ucdm_bitw_ptr_disable(void) {
    ucdm_addr_t addr = 0x15;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;    
    
    HAL_BASE_t result;
    dummy_target = INITVAL;

    result = ucdm_redirect_regw_ptr(addr, &dummy_target);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_disable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL(INITVAL, dummy_target);

    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL(INITVAL, dummy_target);
}

void test_ucdm_bitw_ptr_enable(void) {
    ucdm_addr_t addr = 0x16;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;    
    HAL_BASE_t result;
    dummy_target = INITVAL;

    result = ucdm_redirect_regw_ptr(addr, &dummy_target);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_enable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x0300, dummy_target);

    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    TEST_ASSERT_EQUAL(0x0200, dummy_target);
}

void test_ucdm_bitw_ptr_null_enable(void) {
    ucdm_addr_t addr = 0x17;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;    
    
    HAL_BASE_t result;
    
    result = ucdm_redirect_regw_ptr(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_enable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(4, result);
    
    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(4, result);
}

void test_ucdm_bitw_func_disable(void) {
    ucdm_addr_t addr = 0x18;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;    
    HAL_BASE_t result;
    mock_target = INITVAL;

    result = ucdm_redirect_regw_func(addr, mock_function);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_disable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(2, result);
    
    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(2, result);

    TEST_ASSERT_EQUAL(INITVAL, mock_target);
}


void test_ucdm_bitw_func_enable(void) {
    ucdm_addr_t addr = 0x19;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;    
    HAL_BASE_t result;
    mock_target = INITVAL;

    result = ucdm_redirect_regw_func(addr, mock_function);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_enable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(3, result);
    
    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(3, result);

    TEST_ASSERT_EQUAL(INITVAL, mock_target);
}

void test_ucdm_bitw_func_null_enable(void) {
    ucdm_addr_t addr = 0x1A;
    ucdm_addrb_t addrb_ts = addr << 0x04 | 0x9;
    ucdm_addrb_t addrb_tc = addr << 0x04 | 0x8;   
    HAL_BASE_t result;
    
    result = ucdm_redirect_regw_func(addr, NULL);
    TEST_ASSERT_EQUAL(SUCCESS, result);
    
    result = ucdm_enable_bitw(addr);
    TEST_ASSERT_EQUAL(SUCCESS, result);

    result = ucdm_set_bit(addrb_ts);
    TEST_ASSERT_EQUAL(3, result);
    
    result = ucdm_clear_bit(addrb_tc);
    TEST_ASSERT_EQUAL(3, result);
}

int main(void) {
    init();
    UNITY_BEGIN();
    RUN_TEST(test_ucdm_bitw_ro_disable);
    RUN_TEST(test_ucdm_bitw_ro_enable);
    RUN_TEST(test_ucdm_bitw_normal_disable);
    RUN_TEST(test_ucdm_bitw_normal_enable);
    RUN_TEST(test_ucdm_bitw_ptr_disable);
    RUN_TEST(test_ucdm_bitw_ptr_enable);
    RUN_TEST(test_ucdm_bitw_ptr_null_enable);
    RUN_TEST(test_ucdm_bitw_func_disable);
    RUN_TEST(test_ucdm_bitw_func_enable);
    RUN_TEST(test_ucdm_bitw_func_null_enable);
    return UNITY_END();
}
