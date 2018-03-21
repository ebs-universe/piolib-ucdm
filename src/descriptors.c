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
 * @file descriptors.c
 * @brief Descriptor type definitions.
 *
 */

#include <string.h>
#include "descriptors.h"


descriptor_custom_t * descriptor_custom_root = NULL;


void descriptor_install(descriptor_custom_t * dptr){
    descriptor_custom_t * wptr;
    wptr = descriptor_custom_root;
    if (wptr == NULL){
        descriptor_custom_root = dptr;
        return;
    }
    while(wptr->next){
        if (wptr->next->tag > dptr->tag){
            dptr->next = wptr->next;
            break;
        }
    }
    wptr->next = dptr;
}


descriptor_custom_t * descriptor_find(uint8_t tag){
    descriptor_custom_t * dptr;
    dptr = descriptor_custom_root;
    while (dptr){
        if (dptr->tag == tag){
            return(dptr);
        }
        dptr = dptr->next;
    }
    return(NULL);
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
