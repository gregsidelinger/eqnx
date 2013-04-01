/* 
 * Equinox SST driver for Linux.
 *
 * Copyright (C) 1999-2007 Equinox Systems Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* include file - v24.h */

#if !HINVDATA
#define TIOC	('T'<<8)
#endif

#define TCGETXCTL       (TIOC|18)
#define TCSETXCTL       (TIOC|17)

#define V24_CSIG_POLL   4       /* centisecs between v24 scans of ctrl sigs */

#define V24_LE_DSRTOUT  1500    /* centisecs dsr timeout for leased lines */
#define V24_SW_DCDTIN   6000    /* centisecs dcd timein for switched lines */
#define V24_SW_DCDTOUT  300     /* centisecs dcd timeout for switched lines */
#define V24_CTSTOUT     7500    /* centisecs cts flow may be off */

#define V24_DTR_TIM_EQN 100     /* centisecs to leave DTR off/on for toggle */

/* v_cflag values - in Unisys serial.h */
#if !HINVDATA
#define V24_ENA         1       /* if set, V.24 modes enabled, else disabled */
#define V24_LEASED      2       /* if set, leased line (default), else swit'd */
#define V24_HDX         4       /* if set, half duplex (default), else full */
#define CTSCD		8	/* if set, 8530 in auto enable mode */
#endif

/* v_state values - each governs a particular state machine */
#define V24_DTRTOGGLE   0x0001  /* hangup toggle */
#define V24_DSRWAIT     0x0002
#define V24_DSRMON      0x0004
#define V24_CTSMON      0x0008
#define V24_DCDMON      0x0010

struct eqn_ser_v24 {
  int v_cflag;
  int v_state;
  int v_dtr_state;
  int v_dtr_timer;
  int v_dsr_state;
  int v_dsr_timer;
  int v_cts_state;
  int v_cts_timer;
  int v_dcd_state;
  int v_dcd_timer;
};  /* eqn_ser_v24 */
  
