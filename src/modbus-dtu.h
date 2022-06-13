/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#ifndef MODBUS_DTU_H
#define MODBUS_DTU_H

#include "modbus.h"

MODBUS_BEGIN_DECLS

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
#define MODBUS_DTU_MAX_ADU_LENGTH  256

MODBUS_API modbus_t* modbus_new_dtu(int *fd, modbus_dtu_send dtusend, 
         modbus_dtu_recv dturecv, modbus_dtu_close dtuclose,
         modbus_dtu_flush dtuflush, modbus_dtu_connect dtuconnect, 
         modbus_dtu_select dtuselect);

#define MODBUS_DTU_RS232 0
#define MODBUS_DTU_RS485 1

#define MODBUS_DTU_RTS_NONE   0
#define MODBUS_DTU_RTS_UP     1
#define MODBUS_DTU_RTS_DOWN   2

MODBUS_API int modbus_dtu_set_rts(modbus_t *ctx, int mode);
MODBUS_API int modbus_dtu_get_rts(modbus_t *ctx);

MODBUS_API int modbus_dtu_set_custom_rts(modbus_t *ctx, void (*set_rts) (modbus_t *ctx, int on));

MODBUS_API int modbus_dtu_set_rts_delay(modbus_t *ctx, int us);
MODBUS_API int modbus_dtu_get_rts_delay(modbus_t *ctx);

MODBUS_END_DECLS

#endif /* MODBUS_DTU_H */
