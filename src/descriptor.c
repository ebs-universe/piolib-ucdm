/* 
   Copyright (c)
     (c) 2018 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
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
 * @file descriptor.c
 * @brief Descriptor type definitions.
 *
 */

#include <string.h>
#include <ds/sllist.h>
#include "descriptor.h"

descriptor_custom_t * descriptor_custom_root = NULL;

void descriptor_install(descriptor_custom_t * dptr){
    sllist_install((void *)(&descriptor_custom_root), (void *)dptr);
}


descriptor_custom_t * descriptor_find(uint8_t tag){
    return (descriptor_custom_t *)sllist_find((void *)(&descriptor_custom_root), tag);
}


uint8_t descriptor_read(descriptor_custom_t * dptr, void * target){
    uint8_t len;
    switch (dptr->acctype) {
        case(DESCRIPTOR_ACCTYPE_PTR):
            memcpy(target, dptr->value.ptr, dptr->length);
            return(dptr->length);
        case(DESCRIPTOR_ACCTYPE_RFUNC):
            len = dptr->value.rfunc(240, target);
            return(len);
        default:
            return(0);
    }
}
