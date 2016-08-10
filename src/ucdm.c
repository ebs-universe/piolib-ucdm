/* 
   Copyright (c)
     (c) 2015-2016 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
   This file is part of
   Embedded bootstraps : ucdm library
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
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

uint16_t DMAP_MAXBITS;
uint16_t ucdm_diagnostic_register;
uint8_t  ucdm_exception_status;
avlt_t ucdm_rwht;
avlt_t ucdm_bwht;


static inline void _ucdm_registers_init(void);
static inline void _ucdm_acctype_init(void);
static inline void _ucdm_handlers_init(void);

static inline void _ucdm_registers_init(void)
{
    memset(&ucdm_register, 0, sizeof(ucdm_register_t) * DMAP_MAXREGS);
}

static inline void _ucdm_acctype_init(void)
{
    memset(&ucdm_acctype, 0, sizeof(uint8_t) * DMAP_MAXREGS);
}

static inline void _ucdm_handlers_init(void)
{
    ucdm_bwht.root = NULL;
    ucdm_rwht.root = NULL;
}

void ucdm_init(void)
{
    DMAP_MAXBITS = DMAP_MAXREGS * 16;
    _ucdm_registers_init();
    _ucdm_acctype_init();
    _ucdm_handlers_init();
    return;
}

void ucdm_redirect_regr_ptr(uint16_t addr, uint16_t * target){
    ucdm_acctype[addr] |= UCDM_AT_REGR_PTR;
    ucdm_register[addr].ptr = target;
    return;
}

void ucdm_redirect_regr_func(uint16_t addr, uint16_t target(uint16_t)){
    ucdm_acctype[addr] |= UCDM_AT_REGR_FUNC;
    ucdm_register[addr].rfunc = target;
    return;
}

void ucdm_enable_bitw(uint16_t addr){
    ucdm_acctype[addr] |= UCDM_AT_BITW_WE;
    return;
}

void ucdm_disable_bitw(uint16_t addr){
    ucdm_acctype[addr] &= ~UCDM_AT_BITW_WE;
    return;
}

void ucdm_enable_regw(uint16_t addr){
    ucdm_acctype[addr] |= UCDM_AT_REGW_TYPE_NORMAL;
    return;
}

void ucdm_disable_regw(uint16_t addr){
    ucdm_acctype[addr] &= ~UCDM_AT_REGW_TYPE_NORMAL;
    return;
}

void ucdm_redirect_regw_ptr(uint16_t addr, uint16_t * target){
    ucdm_acctype[addr] |= UCDM_AT_REGW_TYPE_PTR;
    ucdm_register[addr].ptr = target;
    return;
}

void ucdm_redirect_regw_func(uint16_t addr, void target(uint16_t, uint16_t)){
    ucdm_acctype[addr] |= UCDM_AT_REGW_TYPE_FUNC;
    ucdm_register[addr].wfunc = target;
    return;
}

static void _prepare_regw_handler(avlt_node_t * node, uint16_t addr, 
                                  ucdm_rw_handler_t handler);

static void _prepare_regw_handler(avlt_node_t * node, uint16_t addr, 
                                  ucdm_rw_handler_t handler){
    node->content = (void *)handler;
    node->key = addr;
    return;
}

void ucdm_install_regw_handler(uint16_t addr, 
                               avlt_node_t * rwh_node, 
                               ucdm_rw_handler_t handler){
    ucdm_acctype[addr] |= UCDM_AT_REGW_HF;
    _prepare_regw_handler(rwh_node, addr, handler);
    avlt_insert_node(&ucdm_rwht, rwh_node);
    return;
}

static void _prepare_bitw_handler(avlt_node_t * node, uint16_t addr, 
                                  ucdm_bw_handler_t handler);

static void _prepare_bitw_handler(avlt_node_t * node, uint16_t addr, 
                                  ucdm_bw_handler_t handler){
    node->content = (void *)handler;
    node->key = addr;
    return;
}


void ucdm_install_bitw_handler(uint16_t addr, 
                               avlt_node_t * bwh_node, 
                               ucdm_bw_handler_t handler){
    ucdm_acctype[addr] |= UCDM_AT_BITW_HF;
    _prepare_bitw_handler(bwh_node, addr, handler);
    avlt_insert_node(&ucdm_bwht, bwh_node);
    return;
}


uint8_t ucdm_set_register(uint16_t addr, uint16_t value){
    if (addr > DMAP_MAXREGS){
        return 1;
    }
    
    uint8_t regw_type = ucdm_acctype[addr] & UCDM_AT_REGW_TYPE_MASK;
    switch (regw_type){
        case UCDM_AT_REGW_TYPE_NORMAL:
            ucdm_register[addr].data = value;
            break;
        case UCDM_AT_REGW_TYPE_PTR:
            *(ucdm_register[addr].ptr) = value;
            break;
        case UCDM_AT_REGW_TYPE_FUNC:
            (ucdm_register[addr].wfunc)(addr, value);
            break;
        case UCDM_AT_REGW_TYPE_RO:
        default:
            return 2;
            break;
    }
   
    if (ucdm_acctype[addr] & UCDM_AT_REGW_HF){
        avlt_node_t * hfnode;
        hfnode = avlt_find_node(&ucdm_rwht, addr);
        ((ucdm_rw_handler_t)(hfnode->content))(addr);
    }
    return 2;
}

uint16_t ucdm_get_register(uint16_t addr){
    if (addr > DMAP_MAXREGS){
        return 0xFFFF;
    }
    
    uint8_t regr_type = ucdm_acctype[addr] & UCDM_AT_REGR_MASK;
    switch (regr_type){
        case UCDM_AT_REGR_NORM:
            return ucdm_register[addr].data;
            break;
        case UCDM_AT_REGR_PTR:
            return *(ucdm_register[addr].ptr);
            break;
        case UCDM_AT_REGR_FUNC:
            return (ucdm_register[addr].rfunc)(addr);
            break;
        case UCDM_AT_REGR_RESV:
        default:
            return 0xFFFF;
            break;
    }
}

static void _ucdm_wfunc_set(uint16_t * target, uint16_t mask);
static void _ucdm_wfunc_clear(uint16_t * target, uint16_t mask);
static uint8_t _ucdm_generic_wop_bit(uint16_t addrb, void wfunc(uint16_t *, uint16_t));
static void _ucdm_exec_bit_handler(uint16_t addr, uint16_t mask);

static void _ucdm_exec_bit_handler(uint16_t addr, uint16_t mask){
    avlt_node_t * hfnode;
    if (ucdm_acctype[addr] & UCDM_AT_BITW_HF){
        hfnode = avlt_find_node(&ucdm_bwht, addr);
        ((ucdm_bw_handler_t)(hfnode->content))(addr, mask);
    }
    else if (ucdm_acctype[addr] & UCDM_AT_REGW_HF){
        hfnode = avlt_find_node(&ucdm_rwht, addr);
        ((ucdm_rw_handler_t)(hfnode->content))(addr);
    }
    return;
}

uint8_t ucdm_set_bit(uint16_t addrb){
    return _ucdm_generic_wop_bit(addrb, _ucdm_wfunc_set);
}

uint8_t ucdm_clear_bit(uint16_t addrb){
    return _ucdm_generic_wop_bit(addrb, _ucdm_wfunc_clear);
}

static void _ucdm_wfunc_set(uint16_t * target, uint16_t mask){
    *target |= mask;
    return;
}

static void _ucdm_wfunc_clear(uint16_t * target, uint16_t mask){
    *target &= ~mask;
    return;
}

static uint8_t _ucdm_generic_wop_bit(uint16_t addrb, void wfunc(uint16_t *, uint16_t)){
    if (addrb > DMAP_MAXBITS){
        return 1;
    }
    uint16_t addr;
    uint16_t mask;
    uint8_t reg_at;
    ucdm_get_bit_addr(addrb, &addr, &mask);
    
    if (!(ucdm_acctype[addr] & UCDM_AT_BITW_WE)){
        return 2;
    }
    
    reg_at = ucdm_acctype[addr] & UCDM_AT_REGW_TYPE_MASK;
    switch (reg_at){
        case UCDM_AT_REGW_TYPE_NORMAL:
            wfunc(&(ucdm_register[addr].data), mask);
            break;
        case UCDM_AT_REGW_TYPE_PTR:
            wfunc(ucdm_register[addr].ptr, mask);
            break;
        case UCDM_AT_REGR_FUNC:
        case UCDM_AT_REGR_RESV:
        default:
            return 3;
    }
    _ucdm_exec_bit_handler(addr, mask);
    return 0;
}

uint8_t ucdm_get_bit(uint16_t addrb){
    if (addrb > DMAP_MAXBITS){
        return 1;
    }
    uint16_t addr;
    uint16_t mask;
    uint8_t reg_at;
    ucdm_get_bit_addr(addrb, &addr, &mask);
    reg_at = ucdm_acctype[addr] & UCDM_AT_REGR_MASK;
    switch (reg_at){
        case UCDM_AT_REGR_NORM:
            return (ucdm_register[addr].data & mask); 
        case UCDM_AT_REGR_PTR:
            return (*(ucdm_register[addr].ptr)) & mask;
        case UCDM_AT_REGR_FUNC:
        case UCDM_AT_REGR_RESV:
        default:
            return 2;
    }
}
