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

/*
** Layout of a board definition - one for each known + valid board type
*/
struct brdtab_t {
	unsigned char	primary_id;		/* primary id */
	unsigned char	secondary_id;		/* id extension */
	unsigned char	bus;			/* bus type */
	unsigned char	asic;			/* asic type */
	unsigned char	number_of_asics;	/* number of asics on board */
	unsigned char	number_of_ports;	/* number of valid ports */
	unsigned int	flags;			/* misc flags */
	char		*name;			/* board name */
};

/*
** special ids
*/
#define	NOID	0

/*
** bus types
*/
#define	NOBUS	0xFF

#undef	EISA_BUS

#define ISA_BUS   1
#define EISA_BUS  2
#define MCA_BUS   3
#define PCI_BUS   4

#define BUS_ISA   ISA_BUS
#define BUS_EISA  EISA_BUS
#define BUS_MCA   MCA_BUS
#define BUS_PCI   PCI_BUS

/*
** ASIC types 
*/
#define	NOASIC	0
#define	SSP64	1
#define	SSP4	2
#define	SSP2	3

/*
** flags
*/
#define	CTL2	0x1		/* board provides 2 wire (limited) control */
#define	MM	0x2		/* multi-modem board */
#define	POLL40	0x4		/* poll board every 40 milliseconds */
#define	RJ	0x8		/* RJ board (limited control signals) */
#define	NOPANEL	0x10		/* no panel (SST16) */
#define	DB9_PAN	0x20		/* DB-9 panel (SST16) */
#define	DB25_PAN 0x40		/* DB-25 panel (SST16) */
#define	RJ_PAN	0x80		/* RJ panel (SST16) */
#define	LP	0x100		/* low-profile board (SST-4P) */

/*
** SSP2/SSP4
*/
#define SSP2_ASIC 0x88		/* Any ID less than this is not SSP2/4 */
#define SSP2_ID   0xF0		/* SSP2 ID */
#define SSP4_ID   0xF8		/* SSP4 ID */
#define SSP4_MASK 0xF8

#define IFNS_SSP	1
#define IFNS_SSP4	2
#define SSP4_PAGE_SEL	((int) 0x08)
#define SSP4_ADDR_LOW	((int) 0x09)
#define SSP4_ADDR_HIGH	((int) 0x0a)

