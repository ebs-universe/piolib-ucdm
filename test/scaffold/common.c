
#include <ucdm/ucdm.h>

void setUp(void) {
    // setup stuff here
}

void tearDown(void) {
    // clean stuff up here
}

// One-time initialization. If the initialization is 
// expected to be reentrant or reversible, consider using
// setUp and tearDown instead. 
void libinit(void){
    ucdm_init();
}
