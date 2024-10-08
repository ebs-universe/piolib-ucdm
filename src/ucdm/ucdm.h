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
 * @file ucdm.h
 * @brief Interface and usage of the Unified Configuration and Data Manager.
 *
 * This library provides the structures and related functions for implementing 
 * a unified configuration and data manager. This design is based on the 
 * MODBUS data model and is intended to interface seamlessly with MODBUS and 
 * derivative protocols. 
 * 
 * UCDM Storage Model
 * ------------------
 * 
 * The basic register type is uint16_t, and the number of configuration 
 * / data registers supported is configurable from within the application.
 * The necessary variables and containers for this purpose need to be defined 
 * within the application. The tentatively recommended location is 
 * `application/main.c`. 
 * 
 * @see UCDM Configuration and Storage Containers
 * 
 * Each register can be configured to set allowed access type to one or more
 * of the following : 
 * - Read-Only Register                             (modbus: Input Registers)
 * - Read-Write Register                            (modbus: Holding Registers)
 * - 16 Read-Only Discrete Bits                     (modbus: Input Discrete)
 * - 16 Read-Write Discrete Bits                    (modbus: Coils)
 * 
 * Redirection
 * -----------
 * 
 * Though the UCDM library itself maintains storage, it is generally recommended
 * to avoid using UCDM registers for storage and instead opting for one of the 
 * redirection methods. Redirection is configured through the register's access 
 * type configuration (ucdm_acctype). The following sections discuss the various 
 * available options. 
 * 
 * Bit-level Access
 * ----------------
 * 
 * Only basic support of bit read / write is implemented. Bit level access 
 * functions will only operate on normal UCDM registers, ie, registers which
 * are stored directly in UCDM register storage. Specifically :
 *  - Bit read is allowed for all registers where read is allowed. However, 
 *    it may not be appropriate for bits in UCDM registers with redirections 
 *    in place. Bit reads for pointers will read the full register and return 
 *    only the requested bit.  
 *  - Bit write must be enabled if it is to be used, per register. Bit write
 *    should only be used on normal or pointer UCDM registers. 
 * 
 * Bit reads and writes are not supported on registered configured for 
 * redirection using functions. 
 *  
 * @see UCDM Access Type Definitions for Bit Write
 *
 * Register-level Access
 * ---------------------
 * 
 * Register read / write support is more thoroughly supported, allowing a 
 * variety of access types to be used. This implementation of the UCDM treats
 * register read / write as the preferred (and primary) access method.
 * 
 * As in the case of bit read, register read is always allowed. However, 
 * register read can involve one of the following :
 *  - Normal register read, where content of the UCDM storage is returned.
 *  - Pointer register read, where content of the UCDM storage is treated as a 
 *    pointer to where the data is actually stored (equal in size to the 
 *    register),
 *  - Function Pointer register read, where content of the UCDM storage is 
 *    treated as a pointer to a function which, when called with the register 
 *    address, returns the data to be sent back (equal in size to the register).
 *    Such registers MUST be read only registers. 
 * 
 * @see UCDM Access Type Definitions for Register Read
 * 
 * Simlarly, register write allows a variety of write-types. Register write, 
 * though, like bit write, is not allowed by default. Register write must 
 * explicitly be enabled per register, irrespective of what write method is 
 * used. The following write methods are supported : 
 * 
 *  - Normal register write, where content of the UCDM storage is written.
 *  - Pointer register write, where content of the UCDM storage is treated as a 
 *    pointer to where the data should be written (equal in size to the 
 *    register).
 *  - Function Pointer register write, where content of the UCDM storage is 
 *    treated as a pointer to a function which must be called with the data 
 *    to be written (equal in size to the register) and the register address.
 *    Such registers MUST be write only registers.
 * 
 * @see UCDM Access Type Definitions for Register Write
 * 
 * Post-Write Handlers
 * -------------------
 * 
 * For both register and bit write, UCDM can be configured to execute post-write 
 * handler functions after successful write. These handler functions must be set 
 * per-register, and will be called once per call to ucdm_set_register or 
 * ucdm_set_bit. In the case of register write, all types of writes can trigger 
 * the post-write handler function. Bit-write itself is not allowed for registers
 * of the function write type, and post bit-write handlers are not applicable 
 * for such registers. 
 * 
 * In general, bit write handlers are only called in response to a successful 
 * bit write operation. Register write handlers are called in response to a 
 * successful register write operation, as well as a successful bit write 
 * operation for which a bit write handler is not defined, but a register 
 * write handler is.
 * 
 * Application Notes
 * -----------------
 * 
 * Register Access Types
 * =====================
 * 
 * Configuring the appropriate access type definitions should be done by the
 * application code using the provided functions in this library's API. This
 * configuration is expected to occur during initialization, and not during 
 * normal operation. 
 * 
 * Register access type definitions provided can be used to configure the 
 * appropriate access types as needed, but should be avoided in favor of the 
 * configuration functions.  
 * 
 * Note that in most cases, it will be necessary to set both the read and write 
 * operations on registers to the same type. This must be done by application 
 * code utilizing UCDM. 
 * 
 * Post-Write Handlers
 * ===================
 * 
 * When using post-write handlers, remember that the application code using this
 * handler must also provide the necessary data structures to contain and maintain
 * the handler. The functions provided in this libary's API can help you do this, 
 * but they will not be called automatically here. Post-write handlers can also 
 * be pretty expensive, so it is preferable to resort to them only when redirection
 * is insufficient. 
 * 
 * Utilizing Function Pointers
 * ===========================
 * 
 * When using function pointers, either with a function read / write type or 
 * as a post-write handler function, remember that these functions will be 
 * called from within the UCDM context. This means that the call stack is 
 * already pretty deep at this point. Further, communication protocols mounted 
 * atop the UCDM which are time-sensitive may not be able to wait for long 
 * running function calls to complete before sending a response. Keep these 
 * functions short and avoid deep calls. When necessary, just set a global 
 * flag and deal with it in the main loop. 
 * 
 * The Device Map
 * ==============
 * 
 * The mapping between the register address and function, as well as the 
 * mapping between address and access types, are application specific, and 
 * should be defined in the application. The recommended location is 
 * `application/devicemap.h`
 * 
 * Internal Access
 * ===============
 * 
 * The application should internally just set the appropriate variables within
 * the configuration and data buffer directly. The access exposed to the 
 * interface will always be more restrictive than that the application itself 
 * will require, and considerably more expensive. When they are being accessed 
 * via the UCDM, the functions provided by this library ensure the access is 
 * done correctly. 
 * 
 * Casting to larger types
 * =======================
 * 
 * Users should not use UCDM internal storage with void pointers to simulate 
 * stores for larger types. This library does not guarantee that adjacent 
 * registers will be contiguous in memory. Specifically, on platforms where 
 * pointer sizes are larger than 16 bit, the actual data storage will become 
 * discontinuous. To overcome this problem, applications using UCDM to provide 
 * access to variables larger than 16-bit should use the pointer redirection 
 * functionality.
 * 
 * @see ucdm.c
 */

#ifndef UCDM_H
#define UCDM_H

#include <platform/types.h>
#include <ds/avltree.h>
#include <stdint.h>
#include "config.h"

#define UCDM_EXST_KEEPALIVE_REQ         0x01
#define UCDM_EXST_TIMESYNC_REQ          0x02

typedef UCDM_REG_ADDR_TYPE ucdm_addr_t;
typedef UCDM_BIT_ADDR_TYPE ucdm_addrb_t;

typedef void (*ucdm_rw_handler_t)(ucdm_addr_t);
typedef void (*ucdm_bw_handler_t)(ucdm_addr_t, uint16_t);

typedef uint8_t ucdm_acctype_t;

typedef union UCDM_REGISTER_t{
    uint16_t data;
    uint16_t * ptr;
    uint16_t (*rfunc)(ucdm_addr_t);
    void (*wfunc)(ucdm_addr_t, uint16_t);
} ucdm_register_t;

/**
 * @name UCDM Configuration and Storage Containers
 * 
 */
/**@{*/ 

/** \brief Actual storage for UCDM registers. */
extern ucdm_register_t ucdm_register[];

/** \brief Actual storage for UCDM access type settings. */
extern ucdm_acctype_t ucdm_acctype[];

extern uint16_t ucdm_diagnostic_register;

extern  uint8_t ucdm_exception_status;

/**@}*/ 

/**
 * @name UCDM Access Type Definitions for Register and Bit Read
 */
/**@{*/ 
/** Mask for UCDM Access Type, Register Read */
#define UCDM_AT_READ_MASK           0x03
/** Register Read, Not Allowed */
#define UCDM_AT_READ_NONE           0x00
/** Register Read, Normal (from ucdm storage) */
#define UCDM_AT_READ_NORM           0x01
/** Register Read, Pointer (ptr in ucdm storage) */
#define UCDM_AT_READ_PTR            0x02
/** Register Read, Function (function ptr in ucdm storage) */
#define UCDM_AT_READ_FUNC           0x03
/**@}*/ 

/**
 * @name UCDM Access Type Definitions for Bit Write
 */
/**@{*/ 
/** Mask for UCDM Access Type, Bit Write */
#define UCDM_AT_BITW_MASK           0x0C
/** Bit Write, Not Allowed */
#define UCDM_AT_BITW_RO             0x00
/** Bit Write, Write Enabled */
#define UCDM_AT_BITW_WE             0x04
/** Bit Write, Enable Post-Write Handler Function (No effect without WE) */
#define UCDM_AT_BITW_HF             0x08
/** Bit Write, Write Enabled, Enable Post-Write Handler Function */
#define UCDM_AT_BITW_WE_HF          0x0C
/**@}*/ 

/**
 * @name UCDM Access Type Definitions for Register Write
 */
/**@{*/ 
/** Mask for UCDM Access Type, Register Write */
#define UCDM_AT_REGW_MASK           0xF0
/** Register Write, Enable Post-Write Handler Function (No effect without WE) */
#define UCDM_AT_REGW_HF             0x80
/** Register Write, Enable Pre-Write Validation Function (Does not respect WE) **/
#define UCDM_AT_REGW_VALIDATE       0x40
/** Mask for UCDM Access Type, Register Write Type */
#define UCDM_AT_REGW_TYPE_MASK      0x30
/** Register Write, Not Allowed */
#define UCDM_AT_REGW_TYPE_RO        0x00    // 00
/** Register Write, Write Enabled, Normal (to ucdm storage) */
#define UCDM_AT_REGW_TYPE_NORMAL    0x10    // 01
/** Register Write, Write Enabled, Pointer (ptr in ucdm storage) */
#define UCDM_AT_REGW_TYPE_PTR       0x20    // 10
/** Register Write, Write Enabled, Function (function ptr in ucdm storage) */
#define UCDM_AT_REGW_TYPE_FUNC      0x30    // 11
/**@}*/ 

/**
 * @name UCDM Common Setup Functions
 */
/**@{*/

/** 
  * \brief Intitialize the UCDM subsystem.
  */
void ucdm_init(void);
/**@}*/

/**
 * @name UCDM Register Configuration Functions for Register Read
 */
/**@{*/

/** 
 * \brief Disable UCDM register register access on register. 
 *
 * @param addr Address/identifier of the register.
 */
HAL_BASE_t ucdm_disable_regr(ucdm_addr_t addr);

/** 
 * \brief Enable UCDM register register access on register. 
 *
 * @param addr Address/identifier of the register.
 */
HAL_BASE_t ucdm_enable_regr(ucdm_addr_t addr);

/** 
 * \brief Configure UCDM register read access on this register to redirect to a pointer. 
 *
 * Note that in most instances, applications should also call ucdm_redirect_regw_ptr.
 * 
 * @param addr Address/identifier of the register.
 * @param target Pointer to the address where the reads should be redirected to.
 */
HAL_BASE_t ucdm_redirect_regr_ptr(ucdm_addr_t addr, uint16_t * target);

/** 
 * \brief Configure UCDM register read access on this register to redirect to a function pointer. 
 * 
 * This disables writes to this register if enabled. Note, however, that applications 
 * could enable write to this register after calling this function. While there is no 
 * good reason to ever do so, we do nothing to prevent this from happening. 
 * 
 * @param addr Address/identifier of the register.
 * @param target Pointer to the funcition where the reads should be redirected to.
 */
HAL_BASE_t ucdm_redirect_regr_func(ucdm_addr_t addr, uint16_t target(ucdm_addr_t));

/**@}*/ 

/**
 * @name UCDM Register Configuration Functions for Bit Write
 */
/**@{*/ 
/** 
 * \brief Enable UCDM bit write access on register.
 * 
 * @param addr Address/identifier of the register.
 */
HAL_BASE_t ucdm_enable_bitw(ucdm_addr_t addr);

/** 
 * \brief Disable UCDM bit write access on register.
 * 
 * @param addr Address/identifier of the register.
 */
HAL_BASE_t ucdm_disable_bitw(ucdm_addr_t addr);

/**@}*/ 

/**
 * @name UCDM Register Configuration Functions for Register Write
 */
/**@{*/ 

/** 
 * \brief Disable UCDM register write access on a register. 
 * 
 * The write type will be reset by this function. 
 * 
 * @param addr Address/identifier of the register.
 */
HAL_BASE_t ucdm_disable_regw(ucdm_addr_t addr);

/** 
 * \brief Enable UCDM register write access on a normal register. 
 * 
 * @param addr Address/identifier of the register.
 */
HAL_BASE_t ucdm_enable_regw(ucdm_addr_t addr);

/** 
 * \brief Configure UCDM register write access on this register to redirect to a pointer. 
 * 
 * Write enable is also set implicitly. 
 * Note that in most instances, applications should also call ucdm_redirect_regr_ptr.
 *
 * @param addr Address/identifier of the register.
 * @param target Pointer to the address where the writes should be redirected to.
 */
HAL_BASE_t ucdm_redirect_regw_ptr(ucdm_addr_t addr, uint16_t * target);

/** 
 * \brief Configure UCDM register write access on this register to redirect to a function pointer. 
 * 
 * Write enable is also set implicitly. 
 * 
 * This disables reads from this register if enabled. Note, however, that applications 
 * could enable read on this register after calling this function. While there is no good 
 * reason to ever do so, we do nothing to prevent this from happening. .
 * 
 * @param addr Address/identifier of the register.
 * @param target Pointer to the funcition where the writes should be redirected to.
 */
HAL_BASE_t ucdm_redirect_regw_func(ucdm_addr_t addr, void target(ucdm_addr_t, uint16_t));

/**@}*/ 

/**
 * @name UCDM Register Configuration Functions for Post-Write Handler Functions
 */
/**@{*/ 
/** 
 * \brief Install a Register Write Handler for a UCDM register.
 * 
 * \warning This will overwrite any handler previously installed.
 * 
 * @param addr Address/identifier of the register.
 * @param rwh_node Handler tree node container to use. This should be allocated 
 *                 and provided by the application.
 * @param handler Pointer to the handler function.
 */
HAL_BASE_t ucdm_install_regw_handler(ucdm_addr_t addr, 
                               avlt_node_t * rwh_node, 
                               ucdm_rw_handler_t handler);

/** 
 * \brief Install a Bit Write Handler for a UCDM register.
 * 
 * \warning This will overwrite any handler previously installed.
 * 
 * @param addr Address/identifier of the register.
 * @param bwh_node Handler tree node container to use. This should be allocated 
 *                 and provided by the application.
 * @param handler Pointer to the handler function.
 */
HAL_BASE_t ucdm_install_bitw_handler(ucdm_addr_t addr, 
                               avlt_node_t * bwh_node, 
                               ucdm_bw_handler_t handler);
/**@}*/ 

/**
 * @name UCDM Register Access Functions
 */
/**@{*/ 
/** 
  * \brief Set the value of a UCDM register from protocol.
  * 
  * @param addr Address/identifier of the register
  * @param value The value to be set
  * @return 0 for register set, 1 for register out of range, 2 for access error.
  */
HAL_BASE_t ucdm_set_register(ucdm_addr_t addr, uint16_t value);

/** 
  * \brief Get the value of a UCDM register from protocol.
  * 
  * @param addr Address/identifier of the register
  * @return Value of the register, or 0xFFFF if address is invalid.
  */
uint16_t ucdm_get_register(ucdm_addr_t addr);
/**@}*/ 


/**
 * @name UCDM Bit Access Functions
 */
/**@{*/ 
/** 
  * \brief Get the address of a UCDM bit from the identifier
  * 
  * @param addrb Identifier of the bit
  * @param addr Pointer to where the register address should be stored
  * @param mask Pointer to where the bitmask should be stored
  */
static inline void ucdm_get_bit_addr(ucdm_addrb_t addrb, ucdm_addr_t * addr, uint16_t * mask);

static inline void ucdm_get_bit_addr(ucdm_addrb_t addrb, 
                                     ucdm_addr_t * addrp, 
                                     uint16_t * maskp){
    *addrp = addrb >> 4;
    *maskp = (1 << (addrb & 15));
}

/** 
  * \brief Set a UCDM bit from protocol.
  * 
  * @param addrb Address/identifier of the bit
  * @return 1 for bit set, 0 for access error.
  */
HAL_BASE_t ucdm_set_bit(ucdm_addrb_t addrb);

/** 
  * \brief Clear a UCDM bit from protocol.
  * 
  * @param addrb Address/identifier of the bit.
  * @return 1 for bit cleared, 0 for access error.
  */
HAL_BASE_t ucdm_clear_bit(ucdm_addrb_t addrb);

/** 
  * \brief Get the value of a UCDM bit from protocol.
  * 
  * @param addrb Address/identifier of the bit
  * @return Non-zero for bit is set, 0 for bit is cleared, 2 for address invalid
  */
uint8_t ucdm_get_bit(ucdm_addrb_t addrb);
/**@}*/ 

#endif
