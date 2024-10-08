#include <unity.h>
#include <ucdm/ucdm.h>
#include "../scaffold/common.c"

int main(void) {
    libinit();
    UNITY_BEGIN();
    // multiple register read, normal
    // multiple register read, pointer to stable buffer
    // multiple register read, pointer to ephemeral buffer
    
    // multiple register write, normal
    // multiple register write, pointer to stable buffer
    // multiple register write, pointer to ephermeral buffer
    return UNITY_END();
}
