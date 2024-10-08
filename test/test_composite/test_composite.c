#include <unity.h>
#include <ucdm/ucdm.h>
#include "../scaffold/common.c"
#include <string.h>

#define ADDR_READ_NORM          2
#define ADDR_READ_STABLE        6
#define ADDR_READ_EPHEMERAL     10
#define ADDR_WRITE_NORM         14
#define ADDR_WRITE_STABLE       18
#define ADDR_WRITE_EPHEMERAL    22

#define EXAMPLE_VALUE   0x5678901234567890
#define EXAMPLE_INPUT   0x9876543210987654
#define EXAMPLE_TYPE    int64_t

int64_t stable_buffer;
int64_t ephermeral_buffer;


void setup_read_normal(void) {
    uint64_t source = EXAMPLE_VALUE;
    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_enable_regr(ADDR_READ_NORM + i);
        ucdm_register[ADDR_READ_NORM + i].data = source & 0xFFFF;
        source = source >> 16;
    }
}

void test_read_multiple_direct(void){
    // multiple register read, direct
    // memcpy can't be used here because we don't guarantee 
    // ucdm_register.data is contiguous. 
    setup_read_normal();
    int64_t target;

    for (uint8_t i = 0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        * ((uint16_t *)&target + i) = ucdm_register[ADDR_READ_NORM + i].data;
    }

    TEST_ASSERT_EQUAL_INT64(EXAMPLE_VALUE, target); 
}

void test_read_multiple_normal(void){
    // multiple register read, normal
    setup_read_normal();
    
    int64_t target;

    for (uint8_t i = 0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        * ((uint16_t *)&target + i) = ucdm_get_register(ADDR_READ_NORM + i);
    }

    TEST_ASSERT_EQUAL_INT64(EXAMPLE_VALUE, target);
}

void setup_read_stable(void) {
    uint8_t i;
    stable_buffer = EXAMPLE_VALUE;
    for (i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_redirect_regr_ptr(
            ADDR_READ_STABLE + i, 
            (uint16_t *)(&stable_buffer) + (i));
    }
}

void test_read_multiple_stable(void){
    // multiple register read, pointer to stable buffer
    setup_read_stable();
    
    int64_t target;

    for (uint8_t i = 0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        * ((uint16_t *)&target + i) = ucdm_get_register(ADDR_READ_STABLE + i);
    }

    TEST_ASSERT_EQUAL_INT64(EXAMPLE_VALUE, target);
}


uint16_t read_ephemeral_prep(ucdm_addr_t addr){
    // Prepare the ephemeral buffer. Generally, the buffer would be globally 
    // provisioned and used for various register collections and data types.
    // This function is called when the first 16-bit word is read. It should 
    // return the first word, and prepare the rest of the data in the 
    // ephemeral buffer. Note that trying to read the datatype from the 
    // middle will not work. This approach is useful when the underlying 
    // storage is unstable, or when the data is assembled from various sources. 
    memcpy(
        (void *)&ephermeral_buffer + 2, 
        (void *)&stable_buffer + 2, 
        sizeof(EXAMPLE_TYPE) - 2
    );
    return (uint16_t)(stable_buffer & 0xFFFF);
}

void setup_read_ephemeral(void) {
    // When the read starts, we want to copy from the stable 
    // buffer into an ephemeral buffer. 

    uint8_t i;
    stable_buffer = EXAMPLE_VALUE;
    ephermeral_buffer = 0;
    ucdm_redirect_regr_func(ADDR_READ_EPHEMERAL, read_ephemeral_prep);
    for (i=1; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_redirect_regr_ptr(
            ADDR_READ_EPHEMERAL + i, 
            (void*)(&ephermeral_buffer) + (i * 2)
        );
    }
}    


void test_read_multiple_ephemeral(void){
    // multiple register read, pointer to ephemeral buffer
    setup_read_ephemeral();

    int64_t target;

    for (uint8_t i = 0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        * ((uint16_t *)&target + i) = ucdm_get_register(ADDR_READ_EPHEMERAL + i);
    }

    TEST_ASSERT_EQUAL_INT64(EXAMPLE_VALUE, target);
}


void setup_write_normal(void) {
    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_enable_regw(ADDR_WRITE_NORM + i);
    }
}

void test_write_multiple_direct(void){
    // multiple register write, direct
    setup_write_normal();
    uint64_t source = EXAMPLE_INPUT;

    // presetting to 0 isn't needed. We're doing this because these
    // registered are shared by both normal and direct. 
    ucdm_register[ADDR_WRITE_NORM + 3].data = 0;
    ucdm_register[ADDR_WRITE_NORM + 2].data = 0;
    ucdm_register[ADDR_WRITE_NORM + 1].data = 0;
    ucdm_register[ADDR_WRITE_NORM + 0].data = 0;
    
    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_register[ADDR_WRITE_NORM + i].data = source & 0xFFFF;
        source = source >> 16;
    }

    TEST_ASSERT_EQUAL_INT16(0x9876, ucdm_register[ADDR_WRITE_NORM + 3].data);
    TEST_ASSERT_EQUAL_INT16(0x5432, ucdm_register[ADDR_WRITE_NORM + 2].data);
    TEST_ASSERT_EQUAL_INT16(0x1098, ucdm_register[ADDR_WRITE_NORM + 1].data);
    TEST_ASSERT_EQUAL_INT16(0x7654, ucdm_register[ADDR_WRITE_NORM + 0].data);
}

void test_write_multiple_normal(void){
    // multiple register write, normal
    setup_write_normal();
    uint64_t source = EXAMPLE_INPUT;

    // presetting to 0 isn't needed. We're doing this because these
    // registered are shared by both normal and direct. 
    ucdm_register[ADDR_WRITE_NORM + 3].data = 0;
    ucdm_register[ADDR_WRITE_NORM + 2].data = 0;
    ucdm_register[ADDR_WRITE_NORM + 1].data = 0;
    ucdm_register[ADDR_WRITE_NORM + 0].data = 0;

    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_set_register(ADDR_WRITE_NORM + i, source & 0xFFFF);
        source = source >> 16;
    }

    TEST_ASSERT_EQUAL_INT16(0x9876, ucdm_register[ADDR_WRITE_NORM + 3].data);
    TEST_ASSERT_EQUAL_INT16(0x5432, ucdm_register[ADDR_WRITE_NORM + 2].data);
    TEST_ASSERT_EQUAL_INT16(0x1098, ucdm_register[ADDR_WRITE_NORM + 1].data);
    TEST_ASSERT_EQUAL_INT16(0x7654, ucdm_register[ADDR_WRITE_NORM + 0].data);
}

void setup_write_stable(void) {
    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_redirect_regw_ptr(ADDR_WRITE_STABLE + i, (uint16_t *)(&stable_buffer) + (i));
    }
}

void test_write_multiple_stable(void){
    // multiple register write, pointer to stable buffer
    setup_write_stable();
    uint64_t source = EXAMPLE_INPUT;

    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_set_register(ADDR_WRITE_STABLE + i, source & 0xFFFF);
        source = source >> 16;
    }

    TEST_ASSERT_EQUAL_INT64(EXAMPLE_INPUT, stable_buffer);
}

void write_ephemeral_finish(ucdm_addr_t addr, uint16_t value){
    // Handle the data in the ephemeral buffer. Generally, the buffer would 
    // be globally provisioned and used for various register collections and 
    // data types.
    // This function is called when the last 16-bit word is read. It should 
    // finish the assembly of the datatype and do whatever it wants with this 
    // data before returning.
    // This can also be achieved with a post write handler instead of a function
    // redirection on the last word. That might be marginally slower, but 
    // significantly easier to reason about.  
    *((uint16_t *)&ephermeral_buffer + sizeof(EXAMPLE_TYPE) / 2 - 1) = value;
    memcpy(
        (void *)&stable_buffer, 
        (void *)&ephermeral_buffer, 
        sizeof(EXAMPLE_TYPE)
    );
    return;
}

void setup_write_ephemeral(void) {
    // When the write finishes, we want to handle the assembled 
    // data in the ephemeral buffer.
    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2) - 1 ; i++){
        ucdm_redirect_regw_ptr(
            ADDR_READ_EPHEMERAL + i, 
            (void*)(&ephermeral_buffer) + (i * 2)
        );
    }
    ucdm_redirect_regw_func(
        ADDR_WRITE_EPHEMERAL + (sizeof(EXAMPLE_TYPE) / 2), 
        write_ephemeral_finish
    );
}    

void test_write_multiple_ephemeral(void){
    // multiple register write, pointer to ephermeral buffer
    setup_write_ephemeral();
    uint64_t source = EXAMPLE_INPUT;
    
    // clear stable_buffer because it already holds the value from the 
    // last test
    stable_buffer = 0;

    for (uint8_t i=0; i < (sizeof(EXAMPLE_TYPE) / 2); i++){
        ucdm_set_register(ADDR_WRITE_STABLE + i, source & 0xFFFF);
        source = source >> 16;
    }

    TEST_ASSERT_EQUAL_INT64(EXAMPLE_INPUT, stable_buffer);
}


int main(void) {
    libinit();
    UNITY_BEGIN();
    RUN_TEST(test_read_multiple_direct);
    RUN_TEST(test_read_multiple_normal);
    RUN_TEST(test_read_multiple_stable);
    RUN_TEST(test_read_multiple_ephemeral);
    
    RUN_TEST(test_write_multiple_direct);
    RUN_TEST(test_write_multiple_normal);
    RUN_TEST(test_write_multiple_stable);
    RUN_TEST(test_write_multiple_ephemeral);
    return UNITY_END();
}
