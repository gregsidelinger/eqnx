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
** ioctl operations - tty device ioctls and backdoor device ioctls
*/

#ifndef uchar_t
#define uchar_t unsigned char
#endif
#ifndef uint_t
#define uint_t unsigned int
#endif
#ifndef ulong_t
#define ulong_t unsigned long
#endif
#ifndef ushort
#define ushort unsigned short
#endif

#define	IOCTL_VERSION	1

/*
** tty device ioctl operations
*/

/*
** raise/lower DTR: ioctl(fd, EQNXDTR, arg)
** arg	= 1	: raise DTR
** otherwise	: lower DTR
*/
#define EQNXDTR	(('M'<<8)|93)	

/*
** overwrite stty setting: ioctl(fd, EQNXSTTY, arg)
** arg	= 1	: turn on IXON 
** arg	= 2	: turn off IXON 
** arg	= 3	: turn on IXANY
** arg	= 4	: turn off IXANY
** arg	= 17	: turn on CRTSCTS
** arg	= 18	: turn off CRTSCTS
** arg	= 21	: turn on internal loopback
** arg	= 22	: turn off internal loopback
** arg	= 23	: discard XON/XOFF
** arg	= 24	: do not discard XON/XOFF
** arg	= 25	: lock port settings
** arg	= 26	: do not lock port settings
*/
#define EQNXSTTY	(('M'<<8)|86)	

/*
** backdoor device (/dev/eqn) ioctl operations
*/

/*
** get board status: ioctl(backdoor_fd, EQNBRDSTATUS, &eqnbrdstatus)
** eqnbrdstatus is defined below.
** on invocation:	brd_nbr = board number (1..n)
**    			ioctl_version = ioctl version number (IOCTL_VERSION)
*/
#define EQNBRDSTATUS    (('q'<<8)|2)   

/*
** clear channel statistics: ioctl(backdoor_fd, EQNBRDSTATCLR, &brd)
** brd	= board number to clear (1..n)
*/
#define EQNBRDSTATCLR   (('q'<<8)|3)   

/*
** get channel status: ioctl(backdoor_fd, EQNCHSTATUS, &eqnchstatus)
** eqnchstatus is defined below.
** on invocation:	ec_nbr = device id
**    			ioctl_version = ioctl version number (IOCTL_VERSION)
*/
#define EQNCHSTATUS     (('q'<<8)|17)   

/*
** get channel buffer stats: ioctl(backdoor_fd, EQNCHBUFSTAT, &eqnbufstatus)
** eqnbufstatus is defined below.
** on invocation:	ebf_nbr = device id
**    			ioctl_version = ioctl version number (IOCTL_VERSION)
*/
#define EQNCHBUFSTAT    (('q'<<8)|18)  

/*
** turn on internal loopback: ioctl(backdoor_fd, EQNCHLOOPON, &devid)
** devid = device id.
*/
#define EQNCHLOOPON     (('q'<<8)|22) 

/*
** turn off internal loopback: ioctl(backdoor_fd, EQNCHLOOPOFF, &devid)
** devid = device id.
*/
#define EQNCHLOOPOFF    (('q'<<8)|23)

/*
** turn on data monitor: ioctl(backdoor_fd, EQNCHMONSET, &devid)
** devid = device id.
*/
#define EQNCHMONSET     (('q'<<8)|32)

/*
** turn off data monitor: ioctl(backdoor_fd, EQNCHMONCLR, &devid)
** devid = device id.
*/
#define EQNCHMONCLR     (('q'<<8)|33)

/*
** return board configuration: ioctl(backdoor_fd, EQNISTCFG, &istcfg)
** istcfg is defined in ist.h
*/
#define EQNISTCFG	(('q'<<8)|34)

/*
** turn on quiet mode for ring configuration: ioctl(backdoor_fd, EQNQUIETON)
*/
#define EQNQUIETON	(('q'<<8)|35)	

/*
** turn off quiet mode for ring configuration: ioctl(backdoor_fd, EQNQUIETOFF)
*/
#define EQNQUIETOFF	(('q'<<8)|36)	

/*
** move ISA board to new address: ioctl(backdoor_fd, EQNMOVEBRD, &movebrd)
** movebrd is defined below.
*/
#define EQNMOVEBRD      (('q'<<8)|37)   

/*
** hard reset: ioctl(backdoor_fd, EQNCHRESET, &devid)
** devid = device id.
*/
#define EQNCHRESET      (('q'<<8)|38)  

/*
** turn on RAMP verbose messages: ioctl(backdoor_fd, EQNTRACEON)
*/
#define EQNTRACEON      (('q'<<8)|39)  

/*
** turn off RAMP verbose messages: ioctl(backdoor_fd, EQNTRACEOFF)
*/
#define EQNTRACEOFF     (('q'<<8)|40)  

/*
** get port status: ioctl(backdoor_fd, EQNSTATUS1, &disp)
** disp is defined below.
** on invocation:	mp_dev = device id
**    			ioctl_version = ioctl version number (IOCTL_VERSION)
*/
#define EQNSTATUS1	(('q'<<8)|50)	

/*
** resume output: ioctl(backdoor_fd, EQNRESUME, &devid)
** devid = device id.
*/
#define EQNRESUME	(('q'<<8)|51)	

/*
** get port settings: ioctl(backdoor_fd, EQNSTATUS2, &SttyStatus)
** SttyStatus is defined below.
** on invocation:	mp_dev = device id
*/
#define EQNSTATUS2	(('q'<<8)|52)	

/*
** get modem signals: ioctl(backdoor_fd, EQNSTATUS3, &ModemStatus)
** ModemStatus is defined below.
** on invocation:	mp_dev = device id
*/
#define EQNSTATUS3	(('q'<<8)|53)	

/*
** get port registers: ioctl(backdoor_fd, EQNREGISTERS, &registers)
** registers is defined below.
** on invocation:	dev = device id
*/
#define EQNREGISTERS	(('q'<<8)|54)	


/* structures passed on EQNBRDSTATUS request */

struct cst_struct 
{
	uint_t cst_in;				/* characters received */
	uint_t cst_in_pferrs;			/* parity/framing errors */
	uint_t cst_in_brks;			/* breaks */
	uint_t cst_out;				/* characters transmitted */
};

struct lmx_info
{
	uint_t lmx_active;			/* lmx state */
	uint_t lmx_id;				/* lmx id */
	uint_t lmx_chan;
	uint_t lmx_rmt_active;			/* lmx ramp state */
	uint_t lmx_rmt_id;			/* lmx ramp id */
};

#define BRDSTAT_STRLEN 82

#ifndef	MAXBRDCHNL
#define	MAXBRDCHNL	128			/* max channels per board */
#endif

#ifndef	MAXICPS
#define	MAXICPS		4			/* max icps per board */
#endif

#ifndef	MAXLMX
#define	MAXLMX		4			/* max lmxs per icps */
#endif

struct  eqnbrdstatus 
{
	int brd_nbr;				/* board number (1..n) */
	int ioctl_version;			/* ioctl version number */
	char gbl_version_str[BRDSTAT_STRLEN];	/* driver version */
	char gbl_copyright_str[BRDSTAT_STRLEN];	/* driver copyright */
	int gbl_neqnbrds;			/* number of boards present */
	int gbl_neqnicps;			/* number of channels present */
	int nicps;				/* number of icps */
	int id;					/* board id */
	int bus;				/* bus type */
	int alive;				/* board present */
	unsigned long pmem;			/* physical memory address */
	int addrsz;				/* physical memory size */
	int pgctrl;				/* I/O port */	
	struct lmx_info lmx[MAXICPS][MAXLMX];	/* lmx info */
	struct cst_struct brd_cst[MAXBRDCHNL];	/* channel info */
};

/* structure passed on EQNCHSTATUS request */

struct  eqnchstatus
{
	int ec_nbr;				/* device id */
	int ioctl_version;			/* ioctl version number */
	unsigned int c_iflag;			/* input mode flags */
	unsigned int c_oflag;			/* output mode flags */
	unsigned int c_cflag;			/* control mode flags */
	unsigned int c_lflag;			/* local mode flags */

	unsigned int ec_flags;			/* channel flags */
#define	MPC_HIGHBAUD	0x800			/* use high speed table */
#define	MPC_LOOPBACK	0x80000			/* internal loopback */

	unsigned int serial_flags;		/* linux serial flags */
#ifndef	ASYNC_NORMAL_ACTIVE
#define	ASYNC_NORMAL_ACTIVE	0x20000000	/* normal device active */
#define	ASYNC_CALLOUT_ACTIVE	0x40000000	/* callout device active */
#define	ASYNC_INITIALIZED	0x80000000	/* serial port initialized */
#endif

	int	ec_openwaitcnt;			/* wait for open */
	int	ec_power_state;			/* ramp power state */
	unsigned int ec_mpa_stat;		/* modem pool status */
#define	MPA_UART_CFG_DONE	0x00000040
	
};  

/* structure passed on EQNBUFSTAT request */

struct  eqnbufstatus
{
	int ebf_nbr;				/* device id */
	int ioctl_version;			/* ioctl version number */
	unsigned ebf_incount;			/* input chars buffered */
	unsigned ebf_outcount;			/* output chars buffered */
	uchar_t ebf_inflow_state;		/* input flow: 1=on */
	uchar_t ebf_outflow_state; 		/* output flow: 1=on */ 
	uchar_t ebf_insigs;			/* input signals: */
						/*b3=ri b2=dsr b1=cts b0=dcd */
	uchar_t ebf_outsigs;			/* output signals: */
						/* b1=rts b0=dtr */
};  

/* structure passed on EQNMOVEBRD request */

struct movebrd
{
	unsigned long addr;            		/* addr to move to */
};

/* structure passed on EQNSTATUS1 request */

struct 	disp {
	int	ioctl_version;			/* ioctl version number */
	int	mp_state;			/* serial flags */
	int	mp_iflag;			/* termios: iflags */
	int	mp_oflag;			/* termios: oflags */
	int	mp_lflag;			/* termios: lflags */
	int	mp_cflag;			/* termios: cflags */
	int	mp_flags;			/* internal flags */
	int mp_dev;				/* device id */
	uchar_t mp_rawqcc;			/* output chars buffered */
	uchar_t mp_outqcc;			/* output write room */
	uint_t	mp_param;			/* overwrite parameters */
	uint_t	mp_input;			/* characters received */
	uint_t	mp_output;			/* characters transmitted */

	uchar_t	mp_tx_base;			/* tx fifo address */
	uchar_t mp_rx_base;			/* rx fifo address */
	uchar_t mp_rx_fmt;			/* cin char format */
	uchar_t mp_tx_fmt;			/* cout char format */
	ushort	mp_tx_q_ptr;			/* tx q ptr */
	ushort	mp_tx_q_cnt;			/* output chars buffered */
	ushort	mp_tx_csr;			/* cout status */
	ushort	mp_txbaud;			/* cout baud */
	uchar_t	mp_tx_flow_cfg;			/* cout flow cfg */
	uchar_t	mp_tx_cpu_reg;			/* cout cpu reg */
	ushort	mp_tx_count;			/* tx pending count */
	ushort	mp_rx_next;			/* cin dma pointer */
	ushort	mp_rx_tail;			/* cin queue tail pointer */
	ushort	mp_rxbaud;			/* cin baud */
	ushort	mp_cin_ctrl;			/* cin signals */
	ushort	mp_cout_ctrl;			/* cout signals */
	ushort	mp_rx_csr;			/* cin flags */
	ushort	mp_cie;				/* cin attn enable */
	ushort	mp_cis;				/* cin attn events */
	ushort	mp_rxtdm;			/* cin tdm */
	ushort	mp_txtdm;			/* cout tdm */
	uchar_t	mp_susp_lmx;			/* susp lmx */
	uchar_t	mp_susp_sig;			/* susp sig */
	ushort	mp_rx_rcv_cnt;			/* input chars buffered */
	uchar_t	mp_vmin;			/* VMIN value */
	uchar_t	mp_vtime;			/* VTIME value */
	uchar_t	mp_fifo_cnt;			/* cin fifo level */
	uchar_t mp_rx_locks;			/* cin locks */
	ushort	mp_cieo;			/* cout attn enable */
	ushort	mp_tx_events;			/* cout events */
	uchar_t	mp_int_flgs;			/* cin int flags */
	uchar_t	mp_ldisp;			/* termios: line disc */
	uchar_t mp_start;			/* xon character */
	uchar_t mp_stop;			/* xoff character */
	uchar_t mp_bus;				/* bus type */
	uchar_t mp_brdno;			/* board number */
	uchar_t mp_icpno;			/* icp number */
	uchar_t mp_lmxno;			/* lmx number */
	uchar_t mp_chan;			/* channel number */
	uchar_t mp_lmx_cmd;			/* cout lmx cmd */
	uchar_t mp_g_attn;			/* attn bits */
	uchar_t mp_g_init;			/* init bits */
	int	mp_g_rx_attn0;			/* attn0 bits */
	int     mp_mpa_stat;			/* ramp status */
	uint_t	mp_parity_err_cnt;		/* parity errs */
	uint_t	mp_framing_err_cnt;		/* framing errs */
	uint_t	mp_break_cnt;			/* break count */
};

/* structure passed on EQNSTATUS2 request */

struct SttyStatus{
	int mp_dev;				/* device id */
	uint_t mpc_param;			/* stty overwrite parms */
};
#define	IXONSET		1			/* forced ixon */
#define	IXANYSIG	2			/* forced ixany */
#define	IOCTCTS		0x40			/* forced CTS mode */
#define	IOCTRTS		0x80			/* forced RTS mode */
#define	IOCTLLB		0x400			/* forced loopback mode */
#define	IOCTXON		0x800			/* forced XON/XOFF mode */
#define	IOCTLCK		0x1000			/* forced lock settings */

/* structure passed on EQNSTATUS3 request */

struct ModemStatus{
	int mp_dev;				/* device id */
	uint_t mpstatus;			/* modem signal status */
};

/* structure passed on EQNREGISTERS request */
struct registers {
	int dev;				/* device id */
	volatile union  input_regs_u ec_cin;
	volatile union  output_regs_u ec_cout;
};
