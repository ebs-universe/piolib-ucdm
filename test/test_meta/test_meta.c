

#include <unity.h>
#include <ucdm/descriptor.h>
#include <scaffold.h>

void test_descriptor_version(void) {
    // TODO This needs to actually find the descriptor.
    // All libraries use the same tag.
    char readbuffer[20];
    descriptor_custom_t * desc = descriptor_find(DESCRIPTOR_TAG_LIBVERSION);
    TEST_ASSERT_NOT_NULL(desc);
    TEST_ASSERT_EQUAL(11, desc->length);
    descriptor_read(desc, &readbuffer);
    TEST_ASSERT_EQUAL_STRING("ucdm 0.3.0", readbuffer);
    return;
}

int main( int argc, char **argv) {
    init();
    UNITY_BEGIN();
    RUN_TEST(test_descriptor_version);
    UNITY_END();
}
