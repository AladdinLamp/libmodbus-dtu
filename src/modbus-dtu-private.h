/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#ifndef MODBUS_DTU_PRIVATE_H
#define MODBUS_DTU_PRIVATE_H

#ifndef _MSC_VER
#include <stdint.h>
#else
#include "stdint.h"
#endif

#define _MODBUS_DTU_HEADER_LENGTH      1
#define _MODBUS_DTU_PRESET_REQ_LENGTH  6
#define _MODBUS_DTU_PRESET_RSP_LENGTH  2

#define _MODBUS_DTU_CHECKSUM_LENGTH    2


typedef struct _modbus_dtu {
#if HAVE_DECL_TIOCM_RTS
    int rts;
    int rts_delay;
    int onebyte_time;
    void (*set_rts) (modbus_t *ctx, int on);
#endif
    /* To handle many slaves on the same link */
    int confirmation_to_ignore;
} modbus_dtu_t;

#endif /* MODBUS_DTU_PRIVATE_H */
