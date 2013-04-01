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

/* include file - sizes.h */

#define MAXHBA          8               /* max host adapter boards */
#define MAXHBAICP       2               /* max icp per host adapter */
#define MAXICPLDV       4               /* max lmx per icp */
#define MAXLDVRDV       1               /* max remote devices per lmx */
#define MAXLDVMPC       16              /* max RS232/422 channels per lmx */

#if 0  /* Already defined in include/eqnx.h */
#define MAXCHNL         (MAXHBA * MAXHBAICP * MAXICPLDV * MAXLDVMPC) /* 1024 */
#endif
#define MAXICPCHNL      (MAXICPLDV * MAXLDVMPC)                        /* 64 */
#define MAXCHNL_ANX	2
