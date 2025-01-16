/* 
   Copyright (c)
     (c) 2015-2016 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
   This file is part of
   Embedded bootstraps : ucdm library
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/**
 * @file ucdm.c
 * @brief Implementation the Unified Configuration and Data Manager.
 *
 * See ucdm.h for usage documentation. Implementation 
 * documentation should be added to this file as some point. 
 * 
 * @see ucdm.h
 */

#include <string.h>
#include "ucdm.h"
#include "span.h"
#include "descriptor.h"


uint16_t ucdm_diagnostic_register;
uint8_t  ucdm_exception_status;

#if UCDM_ENABLE_HANDLERS
avlt_t   ucdm_rwht;
avlt_t   ucdm_bwht;
#endif

ucdm_register_t ucdm_register[UCDM_MAX_REGISTERS];
ucdm_acctype_t  ucdm_acctype[UCDM_MAX_REGISTERS];

static inline void _ucdm_registers_init(void);
static inline void _ucdm_acctype_init(void);
static inline void _ucdm_handlers_init(void);

static inline void _ucdm_registers_init(void){
    memset(&ucdm_register, 0, sizeof(ucdm_register_t) * UCDM_MAX_REGISTERS);
}

static inline void _ucdm_acctype_init(void){
    memset(&ucdm_acctype, 0, sizeof(uint8_t) * UCDM_MAX_REGISTERS);
}

#if UCDM_LIBVERSION_DESCRIPTOR

static descriptor_custom_t ucdm_descriptor = {NULL, DESCRIPTOR_TAG_LIBVERSION,
    sizeof(UCDM_VERSION), DESCRIPTOR_ACCTYPE_PTR, {UCDM_VERSION}};

    
static void _ucdm_install_descriptor(void)
{
    descriptor_install(&ucdm_descriptor);
}    

#endif


static inline void _ucdm_handlers_init(void){
    #if UCDM_ENABLE_HANDLERS
    ucdm_bwht.root = NULL;
    ucdm_rwht.root = NULL;
    #endif
}

void ucdm_init(void){
    _ucdm_registers_init();
    _ucdm_acctype_init();
    _ucdm_handlers_init();
    #if UCDM_LIBVERSION_DESCRIPTOR
    _ucdm_install_descriptor();
    #endif
    #if UCDM_SPAN_ENABLE
    _ucdm_span_init();
    #endif
    return;
}

HAL_BASE_t ucdm_disable_regr(ucdm_addr_t addr){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] &= ~UCDM_AT_READ_MASK;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_enable_regr(ucdm_addr_t addr){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] = (ucdm_acctype[addr] & ~UCDM_AT_READ_MASK) | UCDM_AT_READ_NORM;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_redirect_regr_ptr(ucdm_addr_t addr, uint16_t * target){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] = (ucdm_acctype[addr] & ~UCDM_AT_READ_MASK) | UCDM_AT_READ_PTR;
        ucdm_register[addr].ptr = target;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_redirect_regr_func(ucdm_addr_t addr, uint16_t target(ucdm_addr_t)){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] = (ucdm_acctype[addr] & ~UCDM_AT_READ_MASK) | UCDM_AT_READ_FUNC;
        ucdm_register[addr].rfunc = target;
        ucdm_disable_regw(addr);
        return 0;
    } else {
        return 1;
    }
}

uint16_t ucdm_get_register(ucdm_addr_t addr){
    if (addr >= UCDM_MAX_REGISTERS){
        return 0xFFFF;
    }
    
    uint8_t regr_type = ucdm_acctype[addr] & UCDM_AT_READ_MASK;
    switch (regr_type){
        case UCDM_AT_READ_NORM:
            return ucdm_register[addr].data;
            break;
        case UCDM_AT_READ_PTR:
            if (ucdm_register[addr].ptr){
                return *(ucdm_register[addr].ptr);
            } else {
                return 0xFFFF;
            }
            break;
        case UCDM_AT_READ_FUNC:
            if (ucdm_register[addr].rfunc){
                return (ucdm_register[addr].rfunc)(addr);
            } else {
                return 0xFFFF;
            }
            break;
        case UCDM_AT_READ_NONE:
        default:
            return 0xFFFF;
            break;
    }
}

HAL_BASE_t ucdm_disable_regw(ucdm_addr_t addr){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] &= ~UCDM_AT_REGW_TYPE_MASK;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_enable_regw(ucdm_addr_t addr){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] = (ucdm_acctype[addr] & ~UCDM_AT_REGW_TYPE_MASK) | UCDM_AT_REGW_TYPE_NORMAL;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_redirect_regw_ptr(ucdm_addr_t addr, uint16_t * target){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] = (ucdm_acctype[addr] & ~UCDM_AT_REGW_TYPE_MASK) | UCDM_AT_REGW_TYPE_PTR;
        ucdm_register[addr].ptr = target;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_redirect_regw_func(ucdm_addr_t addr, void target(ucdm_addr_t, uint16_t)){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] = (ucdm_acctype[addr] & ~UCDM_AT_REGW_TYPE_MASK) | UCDM_AT_REGW_TYPE_FUNC;
        ucdm_register[addr].wfunc = target;
        ucdm_disable_regr(addr);
        return 0;
    } else {
        return 1;
    }
}


HAL_BASE_t ucdm_set_register(ucdm_addr_t addr, uint16_t value){
    if (addr >= UCDM_MAX_REGISTERS){
        return 1;
    }
    
    uint8_t regw_type = ucdm_acctype[addr] & UCDM_AT_REGW_TYPE_MASK;
    switch (regw_type){
        case UCDM_AT_REGW_TYPE_NORMAL:
            ucdm_register[addr].data = value;
            break;
        case UCDM_AT_REGW_TYPE_PTR:
            if (ucdm_register[addr].ptr){
                *(ucdm_register[addr].ptr) = value;
            } else {
                return 3;
            }
            break;
        case UCDM_AT_REGW_TYPE_FUNC:
            if (ucdm_register[addr].wfunc){
                (ucdm_register[addr].wfunc)(addr, value);
            } else {
                return 3;
            }
            break;
        case UCDM_AT_REGW_TYPE_RO:
        default:
            return 2;
            break;
    }
   
    #if UCDM_ENABLE_HANDLERS
    if (ucdm_acctype[addr] & UCDM_AT_REGW_HF){
        avlt_node_t * hfnode;
        hfnode = avlt_find_node(&ucdm_rwht, addr);
        if (hfnode && hfnode->content){
            ((ucdm_rw_handler_t)(hfnode->content))(addr);
        }
    }
    #endif
    return 0;
}

HAL_BASE_t ucdm_enable_bitw(ucdm_addr_t addr){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] |= UCDM_AT_BITW_WE;
        return 0;
    } else {
        return 1;
    }
}

HAL_BASE_t ucdm_disable_bitw(ucdm_addr_t addr){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] &= ~UCDM_AT_BITW_WE;
        return 0;
    } else {
        return 1;
    }
}

static inline void _ucdm_wfunc_bitset(uint16_t * target, uint16_t mask);
static inline void _ucdm_wfunc_bitclear(uint16_t * target, uint16_t mask);
static uint8_t _ucdm_generic_wop_bit(ucdm_addrb_t addrb, void wfunc(uint16_t *, uint16_t));

#if UCDM_ENABLE_HANDLERS

static void _ucdm_exec_bit_handler(ucdm_addr_t addr, uint16_t mask);

static void _ucdm_exec_bit_handler(ucdm_addr_t addr, uint16_t mask){
    avlt_node_t * hfnode;
    if (ucdm_acctype[addr] & UCDM_AT_BITW_HF){
        hfnode = avlt_find_node(&ucdm_bwht, addr);
        if (hfnode && hfnode->content){
            ((ucdm_bw_handler_t)(hfnode->content))(addr, mask);
        }
    }
    else if (ucdm_acctype[addr] & UCDM_AT_REGW_HF){
        hfnode = avlt_find_node(&ucdm_rwht, addr);
        if (hfnode && hfnode->content){
            ((ucdm_rw_handler_t)(hfnode->content))(addr);
        }
    }
    return;
}

#endif 

static inline void _ucdm_wfunc_bitset(uint16_t * target, uint16_t mask){
    *target |= mask;
    return;
}

static inline void _ucdm_wfunc_bitclear(uint16_t * target, uint16_t mask){
    *target &= ~mask;
    return;
}

static uint8_t _ucdm_generic_wop_bit(ucdm_addrb_t addrb, void wfunc(uint16_t *, uint16_t)){
    if (addrb >= UCDM_MAX_BITS){
        return 1;
    }
    ucdm_addr_t addr;
    uint16_t mask;
    ucdm_acctype_t reg_at;
    ucdm_get_bit_addr(addrb, &addr, &mask);

    reg_at = ucdm_acctype[addr];

    if (!(reg_at & UCDM_AT_BITW_WE)){
        return 2;
    }
    
    switch (reg_at & UCDM_AT_REGW_TYPE_MASK){
        case UCDM_AT_REGW_TYPE_NORMAL:
            wfunc(&(ucdm_register[addr].data), mask);
            break;
        case UCDM_AT_REGW_TYPE_PTR:
            if (ucdm_register[addr].ptr){
                wfunc(ucdm_register[addr].ptr, mask);
            } else {
                return 4;
            }
            break;
        case UCDM_AT_READ_FUNC:
        case UCDM_AT_READ_NONE:
        default:
            return 3;
    }
    #if UCDM_ENABLE_HANDLERS
    _ucdm_exec_bit_handler(addr, mask);
    #endif
    return 0;
}

HAL_BASE_t ucdm_set_bit(ucdm_addrb_t addrb){
    return _ucdm_generic_wop_bit(addrb, _ucdm_wfunc_bitset);
}

HAL_BASE_t ucdm_clear_bit(ucdm_addrb_t addrb){
    return _ucdm_generic_wop_bit(addrb, _ucdm_wfunc_bitclear);
}

uint8_t ucdm_get_bit(ucdm_addrb_t addrb){
    if (addrb >= UCDM_MAX_BITS){
        return 1;
    }
    ucdm_addr_t addr;
    uint16_t mask;
    ucdm_acctype_t reg_at;
    ucdm_get_bit_addr(addrb, &addr, &mask);
    reg_at = ucdm_acctype[addr] & UCDM_AT_READ_MASK;
    switch (reg_at){
        case UCDM_AT_READ_NORM:
            if (ucdm_register[addr].data & mask){
                return 0xFF;
            }
            else{
                return 0x00;
            }
        case UCDM_AT_READ_PTR:
            if (!ucdm_register[addr].ptr) {
                return 3;
            }
            if (*(ucdm_register[addr].ptr) & mask){
                return 0xFF;
            }
            else{
                return 0x00;
            }
        case UCDM_AT_READ_FUNC:
        case UCDM_AT_READ_NONE:
        default:
            return 2;
    }
}

#if UCDM_ENABLE_HANDLERS

static void _prepare_regw_handler(avlt_node_t * node, ucdm_addr_t addr, 
                                  ucdm_rw_handler_t handler);

static void _prepare_regw_handler(avlt_node_t * node, ucdm_addr_t addr, 
                                  ucdm_rw_handler_t handler){
    node->content = (void *)handler;
    node->key = addr;
    return;
}

HAL_BASE_t ucdm_install_regw_handler(ucdm_addr_t addr, 
                               avlt_node_t * rwh_node, 
                               ucdm_rw_handler_t handler){
    if (addr < UCDM_MAX_REGISTERS) {    
        ucdm_acctype[addr] |= UCDM_AT_REGW_HF;
        _prepare_regw_handler(rwh_node, addr, handler);
        avlt_insert_node(&ucdm_rwht, rwh_node);
        return 0;
    } else {
        return 1;
    }
}

static void _prepare_bitw_handler(avlt_node_t * node, ucdm_addr_t addr, 
                                  ucdm_bw_handler_t handler);

static void _prepare_bitw_handler(avlt_node_t * node, ucdm_addr_t addr, 
                                  ucdm_bw_handler_t handler){
    node->content = (void *)handler;
    node->key = addr;
    return;
}

HAL_BASE_t ucdm_install_bitw_handler(ucdm_addr_t addr, 
                               avlt_node_t * bwh_node, 
                               ucdm_bw_handler_t handler){
    if (addr < UCDM_MAX_REGISTERS) {
        ucdm_acctype[addr] |= UCDM_AT_BITW_HF;
        _prepare_bitw_handler(bwh_node, addr, handler);
        avlt_insert_node(&ucdm_bwht, bwh_node);
        return 0;
    } else {
        return 1;
    }
}

#endif