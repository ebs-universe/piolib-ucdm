/* 
   Copyright (c)
     (c) 2025 Chintalagiri Shashank
   
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
 * @file span.c
 * @brief Support for types spanning multiple registers
 *
 */

#include "span.h"

#if UCDM_SPAN_ENABLE

uint16_t ucdm_span_buffer[UCDM_SPAN_MAX_LENGTH];
hashmap_t ucdm_span_map = {0};
hashmap_entry_t ucdm_span_table[UCDM_SPAN_MAX_COUNT];

void _ucdm_span_init(void){
    hashmap_init(&ucdm_span_map, &ucdm_span_table[0], UCDM_SPAN_MAX_COUNT);
}

uint16_t _ucdm_span_read_prep(ucdm_addr_t addr){
    // Prepare the ephemeral buffer. This function is called when the first 
    // 16-bit word is read. It returns the first word, and prepares the rest of 
    // the data in the ephemeral buffer. 
    hashmap_entry_t * entry = hashmap_get(&ucdm_span_map, addr);
    if (entry == NULL){
        return 0xFFFF;
    }
    memcpy(
        &ucdm_span_buffer[1], 
        &entry->ptr + 2, 
        entry->len - 2
    );
    return *(uint16_t *)(entry->ptr) & 0xFFFF;
}

HAL_BASE_t ucdm_redirect_spanr_buf(ucdm_addr_t saddr, void * target, uint8_t len){
    if (len < 4 || len % 2 || len > UCDM_SPAN_MAX_LENGTH){
        return 2;
    }
    if (saddr + len/2 < UCDM_MAX_REGISTERS){
        hashmap_insert(&ucdm_span_map, saddr, target, len);
        ucdm_redirect_regr_func(saddr, &_ucdm_span_read_prep);
        for (size_t i=1; i < len/2 ; i++){
            ucdm_redirect_regr_ptr(saddr + i, &ucdm_span_buffer[i]);
        }
        return 0;
    } else {
        return 1;
    }
}

void _ucdm_span_write_finish(ucdm_addr_t addr, uint16_t value){
    // Handle the data in the ephemeral buffer. 
    // This function is called when the last 16-bit word is read. It should 
    // finish the assembly of the datatype and do whatever it wants with this 
    // data before returning.
    hashmap_entry_t * entry = hashmap_get(&ucdm_span_map, addr);
    if (entry == NULL){
        return;
    }
    
    ((void (*)(ucdm_addr_t, void *))entry->ptr)(
        addr - entry->len/2, 
        (void *)&ucdm_span_buffer[0]
    );
    return;
}


HAL_BASE_t ucdm_redirect_spanw_func(ucdm_addr_t saddr, uint8_t len, void target(ucdm_addr_t, void * param)){
    if (len < 4 || len % 2 || len > UCDM_SPAN_MAX_LENGTH){
        return 2;
    }
    if (saddr + len/2 < UCDM_MAX_REGISTERS){
        hashmap_insert(&ucdm_span_map, saddr, target, len);
        for (uint8_t i=0; i < (len/2) - 1 ; i++){
            ucdm_redirect_regw_ptr(
                saddr + i, 
                &ucdm_span_buffer[i]
            );
        }
        ucdm_redirect_regw_func(
            saddr + len/2, 
            _ucdm_span_write_finish
        );
        return 0;
    } else {
        return 1;
    }
}


#endif
