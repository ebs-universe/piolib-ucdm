
#include <scaffold.h>
#include <ucdm/ucdm.h>

void setUp(void) {
    // setup stuff here
}

void tearDown(void) {
    // clean stuff up here
}

#ifdef PIO_NATIVE
void sysinit(void){
    ;
}
#endif

// One-time initialization. If the initialization is 
// expected to be reentrant or reversible, consider using
// setUp and tearDown instead. 
void libinit(void){
    #ifdef PIO_NATIVE
        ucdm_init();
    #else
    // sysinit would have already done this.
    #endif
}

void init(void){
    sysinit();
    libinit();
}
