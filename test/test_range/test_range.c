

#include <unity.h>
#include <ucdm/ucdm.h>
#include "../scaffold/common.c"


void test_regr_conf_maxrange(void) {
    HAL_BASE_t result;
    result = ucdm_enable_regr(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "disable");
    result = ucdm_disable_regr(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "normal");
    result = ucdm_redirect_regr_ptr(UCDM_MAX_REGISTERS, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "pointer");
    result = ucdm_redirect_regr_func(UCDM_MAX_REGISTERS, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "function");
}

void test_bitw_conf_maxrange(void) {
    HAL_BASE_t result;
    result = ucdm_enable_bitw(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "en");
    result = ucdm_disable_bitw(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "dis");
}

void test_regw_conf_maxrange(void) {
    HAL_BASE_t result;
    result = ucdm_disable_regw(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "disable");
    result = ucdm_enable_regw(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "normal");
    result = ucdm_redirect_regw_ptr(UCDM_MAX_REGISTERS, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "pointer");
    result = ucdm_redirect_regw_func(UCDM_MAX_REGISTERS, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "function");
}

void test_regr_maxrange(void) {
    uint16_t result = ucdm_get_register(UCDM_MAX_REGISTERS);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, result);
}

void test_regw_maxrange(void) {
    uint16_t result = ucdm_set_register(UCDM_MAX_REGISTERS, 0x0101);
    TEST_ASSERT_EQUAL_UINT16(1, result);
}

void test_bitr_maxrange(void) {
    uint8_t result = ucdm_get_bit(UCDM_MAX_BITS);
    TEST_ASSERT_EQUAL_UINT8(1, result);
}

void test_bitw_maxrange(void) {
    uint8_t result; 
    result = ucdm_set_bit(UCDM_MAX_BITS);
    TEST_ASSERT_EQUAL_UINT8(1, result);
    result = ucdm_clear_bit(UCDM_MAX_BITS);
    TEST_ASSERT_EQUAL_UINT8(1, result);
}

void test_wh_maxrange(void){
    HAL_BASE_t result;
    result = ucdm_install_regw_handler(UCDM_MAX_REGISTERS, NULL, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "rwh");
    result = ucdm_install_bitw_handler(UCDM_MAX_REGISTERS, NULL, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(1, result, "bwh");
}

int main( int argc, char **argv) {
    libinit();
    UNITY_BEGIN();
    RUN_TEST(test_regr_conf_maxrange);
    RUN_TEST(test_bitw_conf_maxrange);
    RUN_TEST(test_regw_conf_maxrange);
    RUN_TEST(test_regr_maxrange);
    RUN_TEST(test_regw_maxrange);
    RUN_TEST(test_bitr_maxrange);
    RUN_TEST(test_bitw_maxrange);
    RUN_TEST(test_wh_maxrange);
    UNITY_END();
}