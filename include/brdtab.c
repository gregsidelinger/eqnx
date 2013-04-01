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
** Board Definition Table
** All known and valid board definitions
*/

static
struct brdtab_t board_table[] = {

   { 0x8,  0x8,  PCI_BUS,  SSP64, 1, 64,  POLL40,      "SST-64P" },
   { 0x8,  NOID, ISA_BUS,  SSP64, 1, 64,  POLL40,      "SST-64I" },
   { 0x8,  NOID, EISA_BUS, SSP64, 1, 64,  POLL40,      "SST-64E" },
   { 0x8,  NOID, MCA_BUS,  SSP64, 1, 64,  POLL40,      "SST-64M" },
   { 0x10, 0x10, PCI_BUS,  SSP64, 2, 128, POLL40,      "SST-128P" },
   { 0x10, NOID, ISA_BUS,  SSP64, 2, 128, POLL40,      "SST-128I" },
   { 0x10, NOID, EISA_BUS, SSP64, 2, 128, POLL40,      "SST-128E" },
   { 0x10, NOID, MCA_BUS,  SSP64, 2, 128, POLL40,      "SST-128M" },
   { 0x14, 0x88, PCI_BUS,  SSP4,  2, 8,   RJ,          "SST-8P/RJ" },
   { 0x14, 0x90, PCI_BUS,  SSP4,  2, 8,   RJ,          "SST-8P/RJ" },
   { 0x18, NOID, ISA_BUS,  SSP64, 1, 8,   POLL40,      "SST-8I" },
   { 0x18, NOID, EISA_BUS, SSP64, 1, 8,   POLL40,      "SST-8E" },
   { 0x18, NOID, MCA_BUS,  SSP64, 1, 8,   POLL40,      "SST-8M" },
   { 0x20, NOID, ISA_BUS,  SSP64, 1, 4,   POLL40,      "SST-4I" },
   { 0x20, NOID, EISA_BUS, SSP64, 1, 4,   0,           "SST-4E" },
   { 0x20, NOID, MCA_BUS,  SSP64, 1, 4,   0,           "SST-4M" },
   { 0x28, NOID, ISA_BUS,  SSP64, 1, 2,   0,           "SST-2I" },
   { 0x28, NOID, EISA_BUS, SSP64, 1, 2,   0,           "SST-2E" },
   { 0x28, NOID, MCA_BUS,  SSP64, 1, 2,   0,           "SST-2M" },
   { 0x30, NOID, ISA_BUS,  SSP64, 1, 12,  CTL2,        "SST-12I" },
   { 0x30, NOID, EISA_BUS, SSP64, 1, 12,  CTL2,        "SST-12E" },
   { 0x30, NOID, MCA_BUS,  SSP64, 1, 12,  CTL2,        "SST-12M" },
   { 0x38, NOID, ISA_BUS,  SSP64, 1, 24,  CTL2|POLL40, "SST-24I" },
   { 0x38, NOID, EISA_BUS, SSP64, 1, 24,  CTL2|POLL40, "SST-24E" },
   { 0x38, NOID, MCA_BUS,  SSP64, 1, 24,  CTL2|POLL40, "SST-24M" },
   { 0x68, 0x8,  PCI_BUS,  SSP64, 1, 64,  POLL40,      "SST-64P (HP)" },  
   { 0x70, 0x10, PCI_BUS,  SSP64, 2, 128, POLL40,      "SST-128P (HP)" }, 
   { 0x88, 0x88, PCI_BUS,  SSP4,  1, 4,   0,           "SST-4P" },
   { 0x8C, 0x88, PCI_BUS,  SSP4,  1, 4,   RJ,          "SST-4P/RJ" },
   { 0x90, 0x90, PCI_BUS,  SSP4,  2, 8,   0,           "SST-8P" },
   { 0x94, 0x90, PCI_BUS,  SSP4,  2, 8,   RJ,          "SST-8P/RJ" },
   { 0x98, 0x98, PCI_BUS,  SSP4,  2, 8,   MM,          "SST-MM8P" },
   { 0x9C, 0x98, PCI_BUS,  SSP4,  1, 4,   MM,          "SST-MM4P" },
   { 0xA0, 0x88, PCI_BUS,  SSP4,  1, 8,   0,           "SST-4C 8" },
   { 0xA4, 0x88, PCI_BUS,  SSP4,  1, 4,   0,           "SST-4C 4" },
   { 0xA8, 0x88, PCI_BUS,  SSP4,  1, 2,   0,           "SST-2P" },
   { 0xAC, 0x88, PCI_BUS,  SSP4,  1, 4,   0,           "SST-4C 0" },
   { 0xB0, 0x90, PCI_BUS,  SSP4,  2, 8,   0,           "SST-8C 8" },
   { 0xB4, 0x90, PCI_BUS,  SSP4,  2, 4,   0,           "SST-8C 4" },
   { 0xB8, 0x88, PCI_BUS,  SSP4,  1, 4,   LP,          "SST-4P/LP" },
   { 0xBC, 0x90, PCI_BUS,  SSP4,  2, 8,   0,           "SST-8C 0" },
   { 0xC0, 0x80, PCI_BUS,  SSP4,  4, 16,  DB25_PAN,    "SST-16P CP16-DB" },
   { 0xC4, 0x80, PCI_BUS,  SSP4,  4, 16,  RJ_PAN,      "SST-16P CP16-RJ" },
   { 0xC8, 0x80, PCI_BUS,  SSP4,  4, 16,  NOPANEL,     "SST-16P No panel" },
   { 0xD0, 0x80, PCI_BUS,  SSP4,  4, 16,  DB9_PAN,     "SST-16P CP16-DB9" },
   { 0xD4, 0x80, PCI_BUS,  SSP4,  2, 8,   0,           "SST-8P-DB" },
   { 0xEC, 0x88, PCI_BUS,  SSP4,  1, 4,   0,           "SST-4P,PWR" },
   { 0xF0, 0x90, PCI_BUS,  SSP4,  2, 8,   0,           "SST-8P (HP)" },  
   { 0xF0, 0x7,  ISA_BUS,  SSP2,  8, 16,  DB25_PAN,    "SST-16I CP16-DB" },
   { 0xF0, 0x7,  EISA_BUS, SSP2,  8, 16,  DB25_PAN,    "SST-16E CP16-DB" },
   { 0xF0, 0x8,  ISA_BUS,  SSP2,  2, 4,   0,           "SST-4I,PWR" },
   { 0xF0, 0x8,  EISA_BUS, SSP2,  2, 4,   0,           "SST-4E,PWR" },
   { 0xF0, 0x9,  ISA_BUS,  SSP2,  2, 4,   0,           "SST-4I" },
   { 0xF0, 0x9,  EISA_BUS, SSP2,  2, 4,   0,           "SST-4E" },
   { 0xF0, 0xF,  ISA_BUS,  SSP2,  8, 16,  RJ_PAN,      "SST-16I CP16-RJ" },
   { 0xF0, 0xF,  EISA_BUS, SSP2,  8, 16,  RJ_PAN,      "SST-16E CP16-RJ" },
   { 0xF0, 0x10, ISA_BUS,  SSP2,  1, 2,   0,           "SST-2I" },
   { 0xF0, 0x10, EISA_BUS, SSP2,  1, 2,   0,           "SST-2E" },
   { 0xF0, 0x12, ISA_BUS,  SSP2,  4, 8,   RJ,          "SST-8I/RJ" },
   { 0xF0, 0x12, EISA_BUS, SSP2,  4, 8,   RJ,          "SST-8E/RJ" },
   { 0xF0, 0x17, ISA_BUS,  SSP2,  8, 16,  NOPANEL,     "SST-16I No panel" },
   { 0xF0, 0x17, EISA_BUS, SSP2,  8, 16,  NOPANEL,     "SST-16E No panel" },
   { 0xF0, 0x18, ISA_BUS,  SSP2,  4, 8,   0,           "SST-8I,PWR" },
   { 0xF0, 0x18, EISA_BUS, SSP2,  4, 8,   0,           "SST-8E,PWR" },
   { 0xF0, 0x1A, ISA_BUS,  SSP2,  4, 8,   0,           "SSM-8I" }, 
   { 0xF0, 0x1A, EISA_BUS, SSP2,  4, 8,   0,           "SSM-8E" }, 
   { 0xF0, 0x1B, ISA_BUS,  SSP2,  4, 8,   0,           "SST-8I" },
   { 0xF0, 0x1B, EISA_BUS, SSP2,  4, 8,   0,           "SST-8E" },
   { 0xF0, 0x20, ISA_BUS,  SSP2,  4, 8,   0,           "SST-8I" },
   { 0xF0, 0x20, EISA_BUS, SSP2,  4, 8,   0,           "SST-8E" },
   { 0xF0, 0x24, ISA_BUS,  SSP2,  1, 2,   0,           "SST-2I" },
   { 0xF0, 0x24, EISA_BUS, SSP2,  1, 2,   0,           "SST-2E" },
   { 0xF0, 0x27, ISA_BUS,  SSP2,  8, 8,   DB25_PAN,    "SST-16I/DB-8" },
   { 0xF0, 0x27, EISA_BUS, SSP2,  8, 8,   DB25_PAN,    "SST-16E/DB-8" },
   { 0xF0, 0x2D, ISA_BUS,  SSP2,  4, 8,   CTL2,        "SSM-8I" },
   { 0xF0, 0x2D, EISA_BUS, SSP2,  4, 8,   CTL2,        "SSM-8E" },
   { 0xF0, 0x2F, ISA_BUS,  SSP2,  8, 8,   RJ_PAN,      "SST-16I/RJ-8" },
   { 0xF0, 0x2F, EISA_BUS, SSP2,  8, 8,   RJ_PAN,      "SST-16E/RJ-8" },
   { 0xF0, 0x36, ISA_BUS,  SSP2,  2, 4,   0,           "SST-MPM4I" },
   { 0xF0, 0x36, EISA_BUS, SSP2,  2, 4,   0,           "SST-MPM4E" },
   { 0xF0, 0x3F, ISA_BUS,  SSP2,  2, 4,   RJ,          "SST-4I/RJ" },
   { 0xF0, 0x3F, EISA_BUS, SSP2,  2, 4,   RJ,          "SST-4E/RJ" },
   { 0xF4, 0x90, PCI_BUS,  SSP4,  2, 8,   0,           "SST-8P,PWR" },
   { 0xF8, 0x7,  ISA_BUS,  SSP4,  4, 16,  DB25_PAN,    "SST-16I CP16-DB" },
   { 0xF8, 0x7,  EISA_BUS, SSP4,  4, 16,  DB25_PAN,    "SST-16E CP16-DB" },
   { 0xF8, 0x8,  ISA_BUS,  SSP4,  1, 4,   0,           "SST-4I,PWR" },
   { 0xF8, 0x8,  EISA_BUS, SSP4,  1, 4,   0,           "SST-4E,PWR" },
   { 0xF8, 0x9,  ISA_BUS,  SSP4,  1, 4,   0,           "SST-4I" },
   { 0xF8, 0x9,  EISA_BUS, SSP4,  1, 4,   0,           "SST-4E" },
   { 0xF8, 0xF,  ISA_BUS,  SSP4,  4, 16,  RJ_PAN,      "SST-16I CP16-RJ" },
   { 0xF8, 0xF,  EISA_BUS, SSP4,  4, 16,  RJ_PAN,      "SST-16E CP16-RJ" },
   { 0xF8, 0x10, ISA_BUS,  SSP4,  1, 2,   0,           "SST-2I" },
   { 0xF8, 0x10, EISA_BUS, SSP4,  1, 2,   0,           "SST-2E" },
   { 0xF8, 0x12, ISA_BUS,  SSP4,  2, 8,   RJ,          "SST-8I/RJ" },
   { 0xF8, 0x12, EISA_BUS, SSP4,  2, 8,   RJ,          "SST-8E/RJ" },
   { 0xF8, 0x17, ISA_BUS,  SSP4,  4, 16,  NOPANEL,     "SST-16I No panel" },
   { 0xF8, 0x17, EISA_BUS, SSP4,  4, 16,  NOPANEL,     "SST-16E No panel" },
   { 0xF8, 0x18, ISA_BUS,  SSP4,  2, 8,   0,           "SST-8I,PWR" },
   { 0xF8, 0x18, EISA_BUS, SSP4,  2, 8,   0,           "SST-8E,PWR" },
   { 0xF8, 0x1A, ISA_BUS,  SSP4,  2, 8,   0,           "SSM-8I" }, 
   { 0xF8, 0x1A, EISA_BUS, SSP4,  2, 8,   0,           "SSM-8E" }, 
   { 0xF8, 0x1B, ISA_BUS,  SSP4,  2, 8,   0,           "SST-8I" },
   { 0xF8, 0x1B, EISA_BUS, SSP4,  2, 8,   0,           "SST-8E" },
   { 0xF8, 0x20, ISA_BUS,  SSP4,  2, 8,   0,           "SST-8I" },
   { 0xF8, 0x20, EISA_BUS, SSP4,  2, 8,   0,           "SST-8E" },
   { 0xF8, 0x24, ISA_BUS,  SSP4,  1, 2,   0,           "SST-2I" },
   { 0xF8, 0x24, EISA_BUS, SSP4,  1, 2,   0,           "SST-2E" },
   { 0xF8, 0x27, ISA_BUS,  SSP4,  4, 8,   DB25_PAN,    "SST-16I/DB-8" },
   { 0xF8, 0x27, EISA_BUS, SSP4,  4, 8,   DB25_PAN,    "SST-16E/DB-8" },
   { 0xF8, 0x2D, ISA_BUS,  SSP4,  2, 8,   CTL2,        "SSM-8I" },
   { 0xF8, 0x2D, EISA_BUS, SSP4,  2, 8,   CTL2,        "SSM-8E" },
   { 0xF8, 0x2F, ISA_BUS,  SSP4,  4, 8,   RJ_PAN,      "SST-16I/RJ-8" },
   { 0xF8, 0x2F, EISA_BUS, SSP4,  4, 8,   RJ_PAN,      "SST-16E/RJ-8" },
   { 0xF8, 0x36, ISA_BUS,  SSP4,  1, 4,   0,           "SST-MPM4I" },
   { 0xF8, 0x36, EISA_BUS, SSP4,  1, 4,   0,           "SST-MPM4E" },
   { 0xF8, 0x3F, ISA_BUS,  SSP4,  1, 4,   RJ,          "SST-4I/RJ" },
   { 0xF8, 0x3F, EISA_BUS, SSP4,  1, 4,   RJ,          "SST-4E/RJ" },
   { 0xFC, 0x88, PCI_BUS,  SSP4,  1, 4,   LP,          "SST-4P/ULP" },
};

/*
** total number of entries in board_table
*/
static int brdtab_entries = sizeof board_table / sizeof (struct brdtab_t);

/*
** find_board(id, bus)
**
**	return pointer to board definition entry for the board
**	with the specified bus and full (16-bit) id.
**
**	returns NULL if not found.
*/
struct brdtab_t * find_board_def(unsigned short id, int bus)
{
	int	i;
	struct	brdtab_t *brdtab_ptr = NULL;
	unsigned char primary_id = NOID, secondary_id = NOID;

	/*
	** First, extract the relevant parts from the id
	*/
	switch (bus) {
		case PCI_BUS:
			primary_id = id & 0xFC;
			secondary_id = (id & 0xFF00) >> 8;

			/*
			** mask off rev-id bits, special case
			** chrysler board
			*/
			if ((primary_id != 0x14) && (secondary_id != 0x88) &&
			    (primary_id < 0x80))
				primary_id &= 0xF8;

			break;

		case ISA_BUS:
		case EISA_BUS:
			primary_id = id & 0xF8;
		
			/*
			** For SSP2/SSP4 based boards, extract id extension
			** No extension for SSP64 based boards	
			*/
			secondary_id = NOID;
			if ((primary_id == SSP2_ID) || (primary_id == SSP4_ID))
				secondary_id = (id & 0xFF00) >> 8;

			break;

		case MCA_BUS:
			/* micro-channel boards have no id extension */
			primary_id = id & 0xF8;
			secondary_id = NOID;
			break;	
	}
			
	/*
	** Now search the board table for a matching entry
	*/
	for (i=0; i<brdtab_entries; i++) {
		brdtab_ptr = &board_table[i];

		if (brdtab_ptr->bus == bus) {
			if (brdtab_ptr->primary_id == primary_id) {
				if ((brdtab_ptr->secondary_id == secondary_id) ||
				    (brdtab_ptr->secondary_id == NOID) ||
				    (secondary_id == NOID))
					break;
			}
		}
	}

	if (i >= brdtab_entries)
		brdtab_ptr = NULL;

	return brdtab_ptr;
}
