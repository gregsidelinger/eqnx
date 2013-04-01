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

/* ------------------ char arrays for regs ---------------- */

#define IDXMSK	0xFFF
#define STS	mpstat.mp_icpchan

/* hack to match .h files */
#ifndef	BUS_ISA
#define	BUS_ISA	1
#endif
#ifndef	BUS_EISA
#define BUS_EISA	2
#endif
#ifndef	BUS_MCA
#define	BUS_MCA		3
#endif

#ifndef	DEV_GOOD
#define DEV_GOOD	1
#endif
#ifndef DEV_BAD
#define DEV_BAD		0
#endif
#ifndef	DEV_VIRGIN	
#define	DEV_VIRGIN	-1
#endif

#define SS8_I   0x18
#define SSM_24_I        0x38

struct strdisp {
	char	sp_state[5];
	char	sp_iflag[5];
	char	sp_oflag[5];
	char	sp_lflag[5];
	char	sp_cflag[5];
	char	sp_flags[5];
	char	sp_dev[5];
	char	sp_rawqcc[5];
	char	sp_outqcc[5];
	char	sp_delct[5];
	char	sp_param[5];
	char	sp_txbase[5];
	char	sp_rxbase[5];
	char	sp_rxcs[5];
	char	sp_txcs[5];
	char	sp_txidx[5];
	char	sp_txend[5];
	char	sp_rxidx[5];
	char	sp_rxend[5];
	char	sp_rxbaud[5];
	char	sp_txbaud[5];
	char	sp_modem[5];
	char	sp_dtr[5];
	char	sp_txcsr[5];
	char 	sp_rxcsr[5];
	char	sp_cie[5];
	char	sp_cis[5];
	char	sp_sample[5];
	char	sp_mie[5];
	char	sp_rxtdm[5];
	char	sp_txtdm[5];
	char	sp_page[5];
	char	sp_ldisp[5];
	char	sp_rmode[5];
	char	sp_wmode[5];
	char	sp_start[5];
	char	sp_stop[5];
	char	sp_esctim[5];
	char	sp_htky1[5];
	char	sp_htky2[5];
	char	sp_add16[5];
	char	sp_type16[5];
	char	sp_selbus[5];
	char	sp_linkst[5];
	char	sp_linkno[5];
	char	sp_cmxtyp[5];
	char	sp_cmxid[5];
	char	sp_sync[5];
	char	sp_equlz[5];
	char	sp_eqmin[5];
	char	sp_eqmax[5];
	char	sp_output[15];
	char	sp_input[15];
};

struct btypes {
	int	type;
	int bsize;
	int csize;
	char *desc;
	char *pnum;
};
struct btypes at_brds [] = {
	{ 3, 8, 8, "MEGAPORT 24CS", "950083" },
	{ 5, 64, 8, "MEGAPORT 24CS", "950083-2" },
	{ 6, 64, 8, "MEGAPORT 12CS", "950101-1" },
	{ 7, 64, 8, "MEGAPORT 12CS", "950101-2" },
	{ 9, 64, 8, "MEGAPORT 12LD", "950110" },
	{ 16, 8, 8, "MEGAPORT 24CS", "950149" },
	{ 17, 8, 8, "MEGAPORT 12CS", "950148" },
	{ 18, 8, 8, "MEGAPORT 12LD", "950167" },
	{ 19, 8, 8, "MEGAPORT 24LD", "950164" },
	{ 22, 8, 8, "MEGAPORT 8CS", "950147" },
	{ 23, 8, 8, "MEGAPORT 12FMC", "" },
	{ 24, 8, 8, "MEGAPORT XP", "" },
	{ 64, 8, 8, "MEGAPLEX 96", "950134" },
	{ 0,	0,	0, NULL, NULL },
};
struct btypes mca_brds [] = {
	{ 0, 8, 8, "MEGAPORT/2 24CS", "950180" },
	{ 1, 8, 8, "MEGAPORT/2 12CS", "950179" },
	{ 2, 8, 8, "MEGAPORT/2 24LD", "950182" },
	{ 3, 8, 8, "MEGAPORT/2 12LD", "950181" },
	{ 4, 8, 8, "MEGAPORT/2 12LD+12CS", "950183" },
	{ 5, 8, 8, "MEGAPORT/2 12FMC", "" },
	{ 6, 8, 8, "MEGAPORT/2 XP", "" },
	{ 64, 8, 8, "MEGAPLEX 96", "950184" },
	{ 0,	0,	0, NULL, NULL },
};
static struct tio_flags {
	char *fname;
	int val; 
} t_iflag [] = {
	{ "IGNBRK",  0000001 },  /* Ignores break condition */
	{ "BRKINT",  0000002 },  /* Signals interrupt on break */
	{ "IGNPAR",  0000004 },  /* Ignores characters with parity errors */
        { "PARMRK",  0000010 },  /* Marks parity errors */
        { "INPCK",   0000020 },  /* Enables input parity check */
        { "ISTRIP",  0000040 },  /* Strips character */
        { "INLCR",   0000100 },  /* Maps NL to CR on input */
        { "IGNCR",   0000200 },  /* Ignores CR */
        { "ICRNL",   0000400 },  /* Maps CR to NL on input */
	{ "IUCLC",   0001000 },  /* Maps uppercase to lowercase on input */
        { "IXON",    0002000 },  /* Enables start/stop output control */
        { "IXANY",   0004000 },  /* Enables any character to restart output */
        { "IXOFF",   0010000 },  /* Enables start/stop input control */
	{ "IMAXBEL", 0020000 },  
	{ NULL,      0 },
};
struct tio_flags t_oflag [] = {
        { "OPOST",   0000001 },  /* Postprocesses output */
        { "OLCUC",   0000002 },  /* Maps lowercase to uppercase on output */
        { "ONLCR",   0000004 },  /* Maps NL to CR-NL on output */
        { "OCRNL",   0000010 },  /* Maps CR to NL on output */
        { "ONOCR",   0000020 },  /* No CR output at column 0 */
        { "ONLRET",  0000040 },  /* NL performs CR function */
        { "OFILL",   0000100 },  /* Uses fill characters for delay */
        { "OFDEL",   0000200 },  /* Fills is DEL, else NUL */
        { "NLDLY",   0000400 },  /* Selects newline delays: */
        { "CRDLY",   0003000 },  /* Selects carriage return delays: */
        { "CR1",     0001000 },
        { "CR2",     0002000 },
        { "CR3",     0003000 },
        { "TAB1",    0004000 },
        { "TAB2",    0010000 },
        { "TAB3",    0014000 },  /* Expands tabs to spaces */
        { "BSDLY",   0020000 },  /* Selects backspace delays: */
        { "VTDLY",   0040000 },  /* Selects vertical tab delays: */
        { "FFDLY",   0100000 },  /* Selects form feed delays: */
	{ NULL,	0 },
};
struct tio_flags t_cflag [] = {
        { "CSTOPB",  0000100 },  /* Sends two stop bits, else one */
        { "CREAD",   0000200 },  /* Enables receiver */
        { "PARENB",  0000400 },  /* Parity enable */
        { "PARODD",  0001000 },  /* Odd parity, else even */
        { "HUPCL",   0002000 },  /* Hangs up on last close */
        { "CLOCAL",  0004000 },  /* Local line, else dial-up */
        { "CBAUDEX", 0010000 },  /* Block layer output */
        { "RTSCTS", 020000000000 },  /* Enables RTS/CTS protocol 
					for a modem line */
	{ NULL,	0 },
};
struct tio_flags t_lflag [] = {
        { "ISIG",    0000001 },  /* Enable signals */
        { "ICANON",  0000002 },  /* Canonical input (erase and kill processing)*/ 
        { "XCASE",   0000004 },  /* Canonical upper/lower presentation */
        { "ECHO",    0000010 },  /* Enables echo */
        { "ECHOE",   0000020 },  /* Echoes erase character as BS-SP-BS */
        { "ECHOK",   0000040 },  /* Echoes NL after kill character */
        { "ECHONL",  0000100 },  /* Echoes NL */
        { "NOFLSH",  0000200 },  /* Disables flush after interrupt or quit */
          /*"XCLUDE",  0100000,*/  /* Exclusive use of the line */
	{ NULL, 0 },
};
