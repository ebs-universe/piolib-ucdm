/* 
   Copyright (c)
     (c) 2018 Chintalagiri Shashank, Quazar Technologies Pvt. Ltd.
   
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
 * @file descriptor.h
 * @brief Descriptor type definitions.
 *
 */

#ifndef UCDM_DESCRIPTOR_H
#define UCDM_DESCRIPTOR_H

#include "ucdm.h"


typedef struct DESCRIPTOR_VENDOR_t{
    const char * const name;
    const char * const url;
}descriptor_vendor_t;


typedef struct DESCRIPTOR_HARDWARE_t{
    const char * const name;       
    const char * const variant;
    const char * const revision;
}descriptor_hardware_t;


typedef struct DESCRIPTOR_FIRMWARE_t{
    const char * const name;
    const char * const revision;
}descriptor_firmware_t;


extern const descriptor_vendor_t descriptor_vendor;
extern const descriptor_hardware_t descriptor_hardware;
extern const descriptor_firmware_t descriptor_firmware;


#define DESCRIPTOR_ACCTYPE_PTR      0x01
#define DESCRIPTOR_ACCTYPE_RFUNC    0x02

#define DESCRIPTOR_TAG_SERIALNO     0x00
#define DESCRIPTOR_TAG_LIBVERSION   0x01


typedef union DESCRIPTOR_PTR_t{
    void * const ptr;
    uint8_t (*const rfunc)(uint8_t, void * );
}descriptor_ptr_t;


typedef struct DESCRIPTOR_CUSTOM_t{
    struct DESCRIPTOR_CUSTOM_t * next;
    const uint8_t tag;
    const uint8_t length;
    const uint8_t acctype;
    descriptor_ptr_t value;
}descriptor_custom_t;

extern descriptor_custom_t * descriptor_custom_root;

void descriptor_install(descriptor_custom_t * dptr);

descriptor_custom_t * descriptor_find(uint8_t tag);

uint8_t descriptor_read(descriptor_custom_t * dptr, void * target);

#endif
