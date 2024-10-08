

#include <application.h>

#ifndef UCDM_CONFIG_H
#define UCDM_CONFIG_H

#define UCDM_VERSION       "0.2.0"

#ifndef APP_ENABLE_UCDM
    #define APP_ENABLE_UCDM     1
#endif

#if (!APP_ENABLE_UCDM)
    #error "EBS applications generally require UCDM to be enabled"
#endif

#ifdef APP_UCDM_MAX_REGISTERS
    #define UCDM_MAX_REGISTERS      APP_UCDM_MAX_REGISTERS
#else
    #define UCDM_MAX_REGISTERS      10
#endif

#define UCDM_MAX_BITS               (UCDM_MAX_REGISTERS * 16)

#if UCDM_MAX_REGISTERS < 256
    #define UCDM_REG_ADDR_TYPE      uint8_t
#else
    #define UCDM_REG_ADDR_TYPE      uint16_t
#endif

#if UCDM_MAX_BITS < 256
    #define UCDM_BIT_ADDR_TYPE      uint8_t
#elif UCDM_MAX_BITS < 65536
    #define UCDM_BIT_ADDR_TYPE      uint16_t
#else
    #define UCDM_BIT_ADDR_TYPE      uint32_t
#endif

#ifdef APP_ENABLE_UCDM_HANDLERS
    #define UCDM_ENABLE_HANDLERS        APP_ENABLE_UCDM_HANDLERS
#else
    #define UCDM_ENABLE_HANDLERS        1
#endif

#ifdef APP_ENABLE_UCDM_DESCRIPTORS
    #define UCDM_ENABLE_DESCRIPTORS     APP_ENABLE_UCDM_DESCRIPTORS
#else
    #define UCDM_ENABLE_DESCRIPTORS     1
#endif

#endif