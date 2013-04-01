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

/* include file - devstat.h */

/* host adapter static table structure */

#define DEVNAME_LEN     32
#define DEVSIGNATURE    0x7871

#define HA_WIN16        0x0001          /* supports one 16K window */
#define HA_FLAT         0x0002          /* supports flat mapping */

struct ha_list_struct
{
  char  hal_str[DEVNAME_LEN];           /* text name */
  int   hal_valid;                      /* signature present if valid */
  int   hal_id;                         /* id returned */
  int   hal_icp_count;                  /* number of icps */
  int   hal_lmx_count;                  /* lmx's supported locally */
  int   hal_bus_data_size;              /* maximum data bus width (in bits) */
  int   hal_bus_addr_size;              /* maximum addr bus width (in bits) */
  int   hal_ram_size;                   /* 1k units of dram (less tags) */
  int   hal_ram_prop;                   /* characteristics of the ram */
};  /* ha_list_struct */

struct ha_list_struct ha_list0[] = { "ICP2 Protoboard", DEVSIGNATURE, 0,
                                      1, 4, 16,
                                      24, 32, HA_FLAT,
                                      "", -1 /* end of table */ };

/* local device table structure - one lmx per struct */
/* note - a local device may be a bridge! */

struct ldv_list_struct
{
  char  ldvl_str[DEVNAME_LEN];          /* text name */
  int   ldvl_valid;                     /* signature present if valid */
  int   ldvl_id;                        /* id returned */
  int   ldvl_chnl_count;                /* RS232/422 devices this lmx */
  int   ldvl_lmx_speed;                 /* lmx tdm speed it should return */
  int   ldvl_rdv_support;               /* support a remote mux? */
};  /* ldv_list_struct */

struct ldv_list_struct ldv_list0[] = { "Local Panel-16", DEVSIGNATURE, 0,
                                        16, 0, false,
                                        "", -1 /* end of table */ };

/* remote device table structure */

struct rdv_list_struct
{
  char  rdvl_str[DEVNAME_LEN];          /* text name */
  int   rdvl_valid;                     /* signature present if valid */
  int   rdvl_id;                        /* id returned */
  int   rdvl_chnl_count;                /* RS232/422 devices this lmx */
};  /* rdv_list_struct */

struct rdv_list_struct rdv_list0[] = { "", -1 /* end of table */ };
