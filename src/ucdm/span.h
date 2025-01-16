/* 
   Copyright (c)
     (c) 2018 Chintalagiri Shashank
   
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
 * @file span.h
 * @brief Support for types spanning multiple registers
 * 
 */

#ifndef UCDM_SPAN_H
#define UCDM_SPAN_H

#include "ucdm.h"

#if UCDM_SPAN_ENABLE

#include <ds/hashmap.h>

void _ucdm_span_init(void);

HAL_BASE_t ucdm_redirect_spanr_buf(ucdm_addr_t saddr, void * target, uint8_t len);

HAL_BASE_t ucdm_redirect_spanw_func(ucdm_addr_t saddr, uint8_t len, void target(ucdm_addr_t, void * param));

#endif
#endif
