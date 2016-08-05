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

static inline void _ucdm_registers_init(void);
static inline void _ucdm_acctype_init(void);
static inline void _ucdm_handlers_init(void);

static inline void _ucdm_registers_init(void)
{
    memset(&ucdm_register, 0, 2 * DMAP_MAXREGS);
}

static inline void _ucdm_acctype_init(void)
{
    memset(&ucdm_acctype, 0, DMAP_MAXREGS);
}

static inline void _ucdm_handlers_init(void)
{
    ;
}

void ucdm_init(void)
{
    DMAP_MAXBITS = DMAP_MAXREGS * 16;
    _ucdm_registers_init();
    _ucdm_acctype_init();
    _ucdm_handlers_init();
}

void ucdm_install_regw_handler(uint8_t addr, void rw_handler(uint8_t)){
    if (!(ucdm_acctype[addr] & (UCDM_AT_REGW))){
        // Not allowed!
        return;
    }
    ucdm_rw_handler[addr] = rw_handler;
    ucdm_acctype[addr] |= UCDM_AT_REGW_HF;
}

void ucdm_install_bitw_handler(uint8_t addr, void bw_handler(uint8_t, uint16_t)){
    if (!(ucdm_acctype[addr] & (UCDM_AT_BITW))){
        // Not allowed!
        return;
    }
    ucdm_bw_handler[addr] = bw_handler;
    ucdm_acctype[addr] |= UCDM_AT_BITW_HF;
}

void ucdm_enable_regw(uint8_t addr){
    if (ucdm_acctype[addr] & (UCDM_AT_PTR)){
        // Not allowed!
        return;
    }
    ucdm_acctype[addr] |= UCDM_AT_REGW;
}

void ucdm_enable_bitw(uint8_t addr){
    if (ucdm_acctype[addr] & (UCDM_AT_PTR)){
        // Not allowed!
        return;
    }
    ucdm_acctype[addr] |= UCDM_AT_BITW;
}

void ucdm_redirect_read(uint8_t addr, uint16_t * target){
    if (ucdm_acctype[addr] & (UCDM_AT_BITW | UCDM_AT_REGW)){
        // Not allowed!
        return;
    }
    ucdm_register[addr] = (uint16_t)target;
    ucdm_acctype[addr] |= UCDM_AT_PTR;
}


uint8_t ucdm_set_register(uint8_t addr, uint16_t value){
    if (addr > DMAP_MAXREGS){
        return 1;
    }
    if (ucdm_acctype[addr] & UCDM_AT_REGW){
        ucdm_register[addr] = value;
        if (ucdm_acctype[addr] & UCDM_AT_REGW_HF){
            (*ucdm_rw_handler[addr])(addr);
        }
        return 0;
    }
    return 2;
}

uint16_t ucdm_get_register(uint8_t addr){
    if (addr > DMAP_MAXREGS){
        return 0xFFFF;
    }
    if (ucdm_acctype[addr] & UCDM_AT_PTR){
        return *(uint16_t *)(void *)(ucdm_register[addr]);
    }
    else{
        return ucdm_register[addr];
    }
}


static void _ucdm_exec_bit_handler(uint8_t addr, uint16_t mask);

static void _ucdm_exec_bit_handler(uint8_t addr, uint16_t mask){
    if (ucdm_acctype[addr] & UCDM_AT_BITW_HF){
        (*ucdm_bw_handler[addr])(addr, mask);
    }
    else if (ucdm_acctype[addr] & UCDM_AT_REGW_HF){
        (*ucdm_rw_handler[addr])(addr);
    }
}

uint8_t ucdm_set_bit(uint16_t addrb){
    if (addrb > DMAP_MAXBITS){
        return 1;
    }
    uint8_t addr;
    uint16_t mask;
    ucdm_get_bit_addr(addrb, &addr, &mask);
    if (ucdm_acctype[addr] & UCDM_AT_BITW){
        ucdm_register[addr] |= mask;
        _ucdm_exec_bit_handler(addr, mask);
        return 0;
    }
    return 1;
}

uint8_t ucdm_clear_bit(uint16_t addrb){
    if (addrb > DMAP_MAXBITS){
        return 1;
    }
    uint8_t addr;
    uint16_t mask;
    ucdm_get_bit_addr(addrb, &addr, &mask);
    if (ucdm_acctype[addr] & UCDM_AT_BITW){
        ucdm_register[addr] &= ~mask;
        _ucdm_exec_bit_handler(addr, mask);
        return 0;
    }
    return 1;
}

uint8_t ucdm_get_bit(uint16_t addrb){
    if (addrb > DMAP_MAXBITS){
        return 1;
    }
    uint8_t addr;
    uint16_t mask;
    ucdm_get_bit_addr(addrb, &addr, &mask);
    if (ucdm_acctype[addr] & UCDM_AT_PTR){
        return (*(uint16_t *)(void *)(ucdm_register[addr])) & mask;
    }
    return (ucdm_register[addr] & mask); 
}
