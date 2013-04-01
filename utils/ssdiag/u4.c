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

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/errno.h>

#include "twindow.h"
#include "cstream.h"

typedef unsigned long paddr_t;
#define ulong_t unsigned long
#define	uint_t	unsigned int
#define ushort_t unsigned short
#define uchar_t unsigned char

#include "xtend.h"
#include "sizes.h"
#include "icp.h"
#include "brdtab.h"
#include "eqnx_ioctl.h"

#define SSMAXBRD 8
int phys_log[SSMAXBRD];
char eqfilename[50];

#include "misc.h"

char msk43[]="___________________________________________";
char msk60[]="____________________________________________________________";
char msk25[]="_________________________";
char wti[]="Port         Xmit         Rcv    Errors   Rate ";
char eightyspaces[]="%s                                                                               ";

extern int errno;
int getbaud(char *s);

/*-------------------------------------------*/
char *proc;
int baud;
int flow_ctrl=0;
int baud =B38400;
int Internal_loop=1;
int Filespec = 0;
int Board_Selected = 1;
int Group_Selected = 1;
int break_err = 0;
chtype SHADATT;

/* phsyical board count */
int Phys_Brd = 0;
int LOOPBRD = 0;
char device0[]="/dev/eqn0";
char device1[]="/dev/eqn1";
/* ----------------vertical scroll strings -------------------------------*/
char *baudstrs[] = {
	"50",   /* 0000001 */
	"75",   /* 0000002 */
	"110",  /* 0000003 */
	"134",  /* 0000004 */
	"150",  /* 0000005 */
	"200",  /* 0000006 */
	"300",  /* 0000007 */
	"600",  /* 0000010 */
	"1200", /* 0000011 */
	"1800", /* 0000012 */
	"2400", /* 0000013 */
	"4800",	/* 0000014 */
	"9600",	/* 0000015 */
	"19200",/* 0000016 */
	"38400",/* 0000017 */
	"57600",/* extended baud will wrap to 50 baud */
	"115200",/* "	"	" 	"   75 baud */
	0
};
char *baudstrs8c[] = {
	"50",   /* 0000001 */
	"75",   /* 0000002 */
	"110",  /* 0000003 */
	"134",  /* 0000004 */
	"150",  /* 0000005 */
	"200",  /* 0000006 */
	"300",  /* 0000007 */
	"600",  /* 0000010 */
	"1200", /* 0000011 */
	"1800", /* 0000012 */
	"2400", /* 0000013 */
	"4800",	/* 0000014 */
	"9600",	/* 0000015 */
	"19200",/* 0000016 */
	"38400",/* 0000017 */
	"57600",/* extended baud will wrap to 50 baud */
	"115200",/* "	"	" 	"   75 baud */
	"230400", /* "	"	" 	"   110 baud */
	0
};
char *baudstrs4c[] = {
	"50",   /* 0000001 */
	"75",   /* 0000002 */
	"110",  /* 0000003 */
	"134",  /* 0000004 */
	"150",  /* 0000005 */
	"200",  /* 0000006 */
	"300",  /* 0000007 */
	"600",  /* 0000010 */
	"1200", /* 0000011 */
	"1800", /* 0000012 */
	"2400", /* 0000013 */
	"4800",	/* 0000014 */
	"9600",	/* 0000015 */
	"19200",/* 0000016 */
	"38400",/* 0000017 */
	"57600",/* extended baud will wrap to 50 baud */
	"115200",/* "	"	" 	"   75 baud */
	"230400", /* "	"	" 	"   110 baud */
	"460800",/*  "	"	" 	"   134 baud */
	0
};
char *baudstrs2c[] = {
	"50",   /* 0000001 */
	"75",   /* 0000002 */
	"110",  /* 0000003 */
	"134",  /* 0000004 */
	"150",  /* 0000005 */
	"200",  /* 0000006 */
	"300",  /* 0000007 */
	"600",  /* 0000010 */
	"1200", /* 0000011 */
	"1800", /* 0000012 */
	"2400", /* 0000013 */
	"4800",	/* 0000014 */
	"9600",	/* 0000015 */
	"19200",/* 0000016 */
	"38400",/* 0000017 */
	"57600",/* extended baud will wrap to 50 baud */
	"115200",/* "	"	" 	"   75 baud */
	"230400", /* "	"	" 	"   110 baud */
	"460800",/*  "	"	" 	"   134 baud */
	/*"921600",*//*  "	"	" 	"   150 baud */
	0
};
char *Csize[] = {
	"CS5",
	"CS6",
	"CS7",
	"CS8",
	0
};
/*----------------- barbor pole string ------------------*/
char bpole[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890~!@#$%^&*()}{[];:?";


char *brkerrstrs[] = {
	" Break on Errors",
	"Continue on Errors",
	0
};
char *verifystrs[] = {
	"Verify Input",
	"Dont Verify",
	0
};
char *looptstrs[] = {
	"External Loopback",
	"Internal Loopback",
	0
};

/* ------------ Array of ICP queue Sizes --------------- */
unsigned int mqsize[] = {
	256,
	512,
	1024,
	2048,
	4096,
	0
};
/* -------------- Index pointer mask ---------------------*/
unsigned int idxmsk[] = {
	0xFF,
	0x1FF,
	0x3FF,
	0x7FF,
	0xFFF,
	0
};

char fmat[]="   %10u  %10u   %7u  %5u";

#define DTRBF 0
#define RTSBF 1
#define DCDBF 2
#define CTSBF 3
#define DSRBF 4
#define RIBF  5

#define NP 24
int NPORTS = 1;
char txbuf[NP][55];
char pbase[40];
int upd=1;
CSTREAM *r[NP];

#define MAXRD 1	
int maxread=MAXRD;
int check_err = TRUE;


/* ----------------- function declarations ----------------- */
void top_banner();
void clear_ttyname();
int get_version(); 
int stat_port();
int stat_reg_raw();
int stat_reset();
int ismegadev();

int loop_back_test(), single();
int s_speed(),  s_verify(), s_update(), s_brkerr();
int diag_config(), diag_run(), loopback(), sendbp();
int set_exflag();
int get_exflag();
int get_ttyname(), warning();
int stat_termio();
int get_board_stat();
int log_to_phys();
int phys_to_log();

int get_xfercnts(), get_topo();
int diag_file_select();
int diag_group_select();
int quit_now();
int port_config();

extern int data_scope();
#ifdef SYSMON
extern  int show_cpu_usage();
#endif

int redraw_panel();
int navigate_brd();
int navigate_ports();
int navigate_mega();
/* helper routines */
void helper_s_speed();
void helper_s_verify();
void helper_s_update();
void helper_s_brkerr();
void helper_s_loop_type();
void helper_diag_run();
int run_loop_test(void);

int sspstub();
int fill_board_struct();
int eget_chan_count();
int eqget_chan();
int eqget_board();
int reset_board_ctrs();

void set_xmit_buf(int);
int disp_xfercnts(int);
int gen_dev_name(char *, int, int);
extern void display_notice (char *);
extern void clear_notice();
extern int data_entry (SWIN *);
extern pid_t wait(int *);
struct brdtab_t * get_board_def(struct eqnbrdstatus *);

/* ---------- menu selection strings  --------- */
char *rselcs[] = {
	"Single   ",
	"Group    ",
	"File list",
	NULL
};
char *cselcs[] = {
	" Speed ",
	" Verify ",
	" Update ",
	" Errors ",
	NULL
};
char *dselcs[] = {
	" Configure ",
	" Run       ",
	NULL
};

char *statselcs[] = {
	"Breakout Box",
	"Data Scope",
	"Termio",
	"Register Dump",
	"Reset Channel",
	NULL
}; 
char *diagselcs[] = {
	"Loopback",
	"Send",
	NULL
}; 
char *dvrselcs[] = {
	"Driver Status",
	"Board Status",
	"Counters",
	"Topology",
	NULL
}; 
char *admselcs[] = {
	"Sysadm ports",
	"Readme",
	"Unix Shell",
	NULL
}; 
char *quitselc[] = {
	"Quit",
	NULL
}; 
#ifdef SYSMON
char *mon_selcs[]={
	" CPU Usage ",
	0
};
static int(*monfuncs[])()={show_cpu_usage};
#endif

/* ------------------ function tables ---------------------*/
static int (*rfuncs[])()={single, diag_group_select, diag_file_select};
static int (*cfuncs[])()={s_speed, s_verify, s_update, s_brkerr};
static int (*diagfuncs[])()={ loopback, sendbp};
static int (*dvrfuncs[])()={get_version, get_board_stat,get_xfercnts, get_topo};
static int (*statfuncs[])()={stat_port,data_scope,stat_termio,
			stat_reg_raw, stat_reset};

static int (*quitfunc[])()={ quit_now};


/* ---------- main menu table  ---------------- */

static SMENU defaultmn [] = {           /* default */
        {" Driver Status ", dvrselcs, dvrfuncs},
        {" Port Status ", statselcs, statfuncs},
	{" Diagnostics ", diagselcs, diagfuncs},
#ifdef SYSMON
	{" Monitor ", mon_selcs, monfuncs}, 
#endif

	{" Quit ", quitselc, quitfunc},
        {NULL,NULL,NULL}
};

static SMENU *tmn = defaultmn;

char ddevice[]="/dev/eqn";

#include "brdtab.c"

/* --------------------------------------------------------------------*/
/* QUIT_NOW
 this routine is called when the user selects quit from the 
   horizontal menu */

int quit_now(void)
{
		return(1);
}
/* --------------------------------------------------------------------*/
/*  PORT_CONFIG
	this routine is call during menu processing it simply
	invokes sscfg
*/
/* --------------------------------------------------------------------*/
int port_config(void)
{
	struct stat sbuf;
	char tbuff[35];
	int ret;

	/* should stat file first */
	if(stat("/etc/sscfg", &sbuf) == -1 ){
		if(errno == 2){
			sprintf(tbuff, "File %s does not exist", 
				"/etc/sscfg");
		} else {
			sprintf(tbuff, "ERROR: stat %s, errno %d", 
				"/etc/sscfg", errno);
		}
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}

	ret = system("/etc/sscfg");
	/* refresh the screen */
	wrefresh(curscr); 
	return(0);
}	
/* ------------------------------------------------------------------------
int GET_VERSION   

- This routine is called indirectly from horizontal menu processing. It
  displays the device driver status information 
- Calls 
   fill_board_struct
---------------------------------------------------------------------------*/

int
get_version()
{
	int row;
	struct eqnbrdstatus brdstat;
	char tbuff[80];
	SWIN *vwn;

	row = 0;
	if(! fill_board_struct(&brdstat, 1))
		return(0);

	/* window size for get_version is fixed at 58. May need to check lenth of		of version string to verify window is wide enough ( as shown above ); */

	/* --------------- Open a window ----------------*/
	vwn = establish_window( 4, 6, 5 , 72,
		 0,1, " Driver Status ");
	wcursor(vwn, 1, row++);
	/*----- print the driver version string.----------*/
	Wprintf(vwn, brdstat.gbl_version_str);

	wcursor(vwn, 1, row++);
	/*----- print the driver copyrignt string.--------*/
	Wprintf(vwn, brdstat.gbl_copyright_str);

	/* --------------- boards found	------------------*/
	wcursor(vwn, 1, row++);
	sprintf(tbuff,"Number of boards found: %d", brdstat.gbl_neqnbrds);
	Wprintf(vwn, tbuff);

	get_char();
	delete_window(vwn);	/* close window */
	return(0);
}
/* --------------------------------------------------------------------*/
/* GET_BOARD_STAT
	show board statistics including: I/O , slot, memory, and
	size */
/* --------------------------------------------------------------------*/

int
get_board_stat()
{
	int row, x;
	struct eqnbrdstatus brdstat;
	char tbuff[80];
	SWIN *vwn;
	struct brdtab_t *brd_def;
	int oldbrd = 0;
	int io_diff = 0;
	row = 0;

	oldbrd = Board_Selected;

	if(! fill_board_struct(&brdstat, 1))
		return(0);
	if(brdstat.gbl_neqnbrds < 1) {
		sprintf(tbuff,"No boards found.");
		error_message(tbuff); get_char(); clear_message();
		return(0);

	}

	/* window size for get_version is fixed at 58. May need to check lenth of		of version string to verify window is wide enough ( as shown above ); */

	/* --------------- Open a window ----------------*/
	vwn = establish_window( 12, 6, 3 + brdstat.gbl_neqnbrds , 55,
		 0,1, " Board Status ");

	wcursor(vwn, 1, row++);

	Wprintf(vwn, "Board Type        Address       Size      I/O");


	for(x=0; x < (int)brdstat.gbl_neqnbrds; x++){

		if(! fill_board_struct(&brdstat, x + 1))
			break;
		Board_Selected = x + 1;

		wcursor(vwn, 1, row++);

		brd_def = get_board_def(&brdstat);

		/* is this computed correctly ? mikes */
		io_diff = 2;
		if (((brd_def->bus == ISA_BUS) ||
		     (brd_def->bus == EISA_BUS)) &&
		     (brd_def->asic != SSP64)) 
			io_diff = 8;

		sprintf(tbuff,"%-16.16s  0x%-10lX  0x%-7X 0x%-6X", 
			brd_def->name,
			(unsigned long)brdstat.pmem, 
			brdstat.addrsz, brdstat.pgctrl - io_diff); 	
		Wprintf(vwn, tbuff);
 
	} 
	Board_Selected = oldbrd;
	
	get_char();
	delete_window(vwn);	/* close window */

	return(0);

}

extern int Vertical_sel;
/* Vertical_sel contains the last vertical selection made in vscoll 
	use this variable to determine user responses. If a user selects
	the first entry on a vertical menu Vertical_sel is set to 1 */


/*-------------------------------------------------------------------*/
/*	FILL_BOARD_STRUCT
	fills in board structure.

*/
/*-------------------------------------------------------------------*/

int fill_board_struct( struct eqnbrdstatus *bsptr, int brd)
{
	int retval, pfd;
	char tbuff[80];

	if((pfd = open( ddevice, O_RDWR )) < 0){
		nodelay(stdscr, FALSE);
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}
	bsptr->brd_nbr = brd;
	bsptr->ioctl_version = IOCTL_VERSION;
	if((retval = ioctl( pfd, EQNBRDSTATUS, bsptr))){
		sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d" ,retval, errno);
		error_message(tbuff); get_char(); clear_message();
		close(pfd);
		return(0);
	}
	close(pfd);
	return(1);
}
/*-------------------------------------------------------------------*/
/*	FILL_CHAN_STRUCT
	fills in channel structure.

*/
/*-------------------------------------------------------------------*/
int fill_chan_struct(cptr,  channel)
struct eqnchstatus *cptr;
int channel;
{
		struct stat sbuf;
		char tbuff[50];
		char dname[35];
		int pfd, retval;

	if((pfd = open( ddevice, O_RDWR )) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
		nodelay(stdscr, FALSE);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}
	gen_dev_name(dname, phys_to_log(Board_Selected), channel);

	if(stat(dname, &sbuf) == -1 ){
			sprintf(tbuff, "f_c_s ERROR: stat %s, errno %d", 
				dname, errno);
			nodelay(stdscr, FALSE);
			error_message(tbuff); get_char(); 
			clear_message();
			return(0);
		}
	cptr->ioctl_version = IOCTL_VERSION;
	if((retval = ioctl( pfd, EQNCHSTATUS, cptr))){
		sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d" ,retval, errno);
		nodelay(stdscr, FALSE);
		error_message(tbuff); get_char(); clear_message();
		close(pfd);
		return(0);
	}

	close(pfd);
	return(1);
}
/*-------------------------------------------------------------------*/
#define	G_LDV_ALIVE (brdstat->lmx[icp][chnl / 16].lmx_active)
#define	G_LDV_ID (brdstat->lmx[icp][chnl / 16].lmx_id)
#define	LDVCCOUNT (brdstat->lmx[icp][chnl / 16].lmx_chan)
#define	G_RDV_ALIVE (brdstat->lmx[icp][chnl / 16].lmx_rmt_active)
#define	RDVCCOUNT (brdstat->lmx[icp][chnl / 16].lmx_chan)
#define	G_RDV_ID (brdstat->lmx[icp][chnl / 16].lmx_rmt_id)
/*-------------------------------------------------------------------
 
 get channel count for a panel, mux or board 
 EGET_CHAN_COUNT( brdstat, egroup)
-------------------------------------------------------------------*/
int eget_chan_count( brdstat, egroup)
struct eqnbrdstatus *brdstat;
int egroup;
{
	int chnl, icp;
	int count = 0;
	struct brdtab_t *brd_def;

	icp = 0;

	/* calculate chan number */
	/* calculate icp number for */
	/* SVR3.2 icp index */
	icp = (egroup - 1) / 4;
	if(egroup < 5)
		chnl = (egroup - 1) * 16;
	else {
		egroup -= 5;
		chnl = egroup  * 16;
	}

	if(! fill_board_struct(brdstat, brdstat->brd_nbr))
		return(0);

	brd_def = get_board_def(brdstat);

/* 	DEBUG
	sprintf(tbuff, "Channel Info on port: %d", chnl);
	error_message(tbuff); get_char(); clear_message();
	
	sprintf(tbuff, "G_LDV_ALIVE:%d", G_LDV_ALIVE);
	error_message(tbuff); get_char(); clear_message();

	sprintf(tbuff, "G_LDV_ID:%X", G_LDV_ID);
	error_message(tbuff); get_char(); clear_message();

	sprintf(tbuff, "LDVCCOUNT:%d",LDVCCOUNT);
	error_message(tbuff); get_char(); clear_message();

	sprintf(tbuff, "RDVCCOUNT:%d",RDVCCOUNT);
	error_message(tbuff); get_char(); clear_message();
	
	sprintf(tbuff, "G_RDV_ALIVE:%d", G_RDV_ALIVE);
	error_message(tbuff); get_char(); clear_message();

	sprintf(tbuff, "G_RDV_ID:%d", G_RDV_ID);
	error_message(tbuff); get_char(); clear_message();
   ENDEBUG */

	switch( G_LDV_ALIVE ){
		case DEV_GOOD: {
			unsigned int brdid = G_LDV_ID;
			if (brdid & 0xFF) brdid = brdid & 0xFF;
			switch(brdid){
				case 0:		/* PM16 */
				case 4:	
				case 7:		/* PM16 422 */
					count = LDVCCOUNT;
					break;
				case 1:		/* bridge */
					if(G_RDV_ALIVE == DEV_GOOD 
					    && G_RDV_ID == 2) {	/* mux ? */
						count = RDVCCOUNT;
					}	
					break;
				case 3:
						/* (on board lmx)  */
					count = brd_def->number_of_ports;
					break;
				case 5:		/* SSM12 and 24 */
					if(brd_def->number_of_ports == 24)
						count = 24;
						break;
					count = LDVCCOUNT; /* set to 12 by driver */
					break;
				case 6:		/* SST-8E/422 */
					count = 8;
					break;
				case 0x0008:	/* RAMP with 1 engine */
			 	case 0x0009:	/* RAMP with 2 engines */
				case 0x000B:	/* RAMP with 4 engines */
					count = 16;
					break;
/* GDC */
				case 0x0010:	/* GDC */
					count = 16;
					break;
				case 0xA8:
       	        			count = 2;
					break;
				case 0xF0:	/* SSP2 port asic */
				case 0xF8:	/* SSP4 port asic */
					count = brd_def->number_of_ports;
					break;
				case 0xD0:	/* SST-16P DB9 */
				case 0xC0:	/* SST-16P DB */
				case 0xC4:	/* SST-16P RJ */
				case 0xC8:	/* SST-16P no panel */
					count = 16;
					break;
				case 0x90:	/* SSP4 port asic */
				case 0x94:	/* SSP4 port asic */
				case 0xF4:
				case 0x98:
       	        			count = 8; 
					break;
				case 0x88:
				case 0xB8:
				case 0x8C:
				case 0xEC:
				case 0x9C:
				case 0xFC:
       	        			count = 4; 
					break;
				default:
					count = 0;
					break;
			}
			break;	
		}
		case DEV_VIRGIN:	
			count = 0;	
			break;
		case DEV_BAD:
			count = 0;	
			break;
		default:
			count = 0;	
			break;
		}
	return(count);
}

/*-------------------------------------------------------------------*/
/*	RESET_BOARD_CTRS
	reset a boards statistics counters
	
*/

int
reset_board_ctrs( brd)
int brd;
{
	int retval, pfd;

	char tbuff[80];

	if((pfd = open( ddevice, O_RDWR )) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}
	if((retval = ioctl( pfd, EQNBRDSTATCLR, &brd))){
		sprintf(tbuff,"ERROR EQNBRDSTATCLR:%d errno:%d", retval, errno);
		error_message(tbuff); get_char(); clear_message();
		close(pfd);
		return(0);
	}

	close(pfd);
	return(1);
}

/* --------------------------------------------------------------------------*/
/*int stat_port

	this routine displays port status information, and graphical
	breakout box
*/

/* --------------------------------------------------------------------------*/
char regmsk[] = "____";
char regmsk2[] = "________";
char regmskten[] = "__________";

int get_phys_char();
char offl[]  = "OFFLINE";
char onl[] = " ONLINE";
char off[]  = "OFF";
char on[] = " ON";
char statbuf[18][15];
char ctrsig[10][15];
char quebuf[3][10];
#define RDX (mpstat.mp_mpc.mpc_rxq.q_ptr & rcmsk)
#define TDX (STS.icp_txnext & txmsk)

int
stat_port()
{
	SWIN *regwn;
	struct field *fld;
	unsigned int last_time, new_time;
	time_t oldt, newt;
	int  done, ssp4asic, oldupd;
	int chnl, pfd, retval;
	unsigned int junk,rxold,txold, rxcnt, txcnt, oldrx, old_cflag, oldtx;
	unsigned long  rrate, trate;
	unsigned int t_save, r_save;
	struct timeb timer;
	struct stat sbuf;
	struct eqnchstatus mpstat;
	struct eqnbrdstatus brdstat;
	struct eqnbufstatus bufstat;
	struct strdisp sdp;
	unsigned char vv;
	char tbuff[60];
	char dname[35];	
	struct brdtab_t *brd_def;

	oldupd = upd;
	upd = 1;
	done=0;
	while(TRUE){
		if(!get_ttyname(tbuff))			/* get a device name */
			break;
		if(!ismegadev(tbuff)){ 			/* is it one of ours */
			char _temp_str[100];
			sprintf(_temp_str,"%s is not a valid device", tbuff);
			error_message(_temp_str);
			/*error_message("This is not a valid device");*/
			get_char();
			clear_message();
			continue; 
		}	
		chnl = eqget_chan(tbuff);
		brdstat.brd_nbr = eqget_board(tbuff);	
		brdstat.ioctl_version = IOCTL_VERSION;
		brd_def = get_board_def(&brdstat);

		strcpy(dname, tbuff);	
		
		if(brdstat.brd_nbr < 0){
			sprintf(tbuff, "This device does not exist.");
			error_message(tbuff); get_char(); 
			clear_message();
			continue;
		}
		/*
		sprintf(tbuff, "Logical board is %d", 
			brdstat.brd_nbr);	
		error_message(tbuff); get_char(); 
		clear_message();
		*/	

		if(stat(dname, &sbuf) == -1 ){
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", 
					dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
			error_message(tbuff); get_char(); 
			clear_message();
			continue;
		}
		mpstat.ec_nbr = sbuf.st_rdev;
		mpstat.ioctl_version = IOCTL_VERSION;
   
    		if((pfd = open( ddevice, O_RDWR )) <= 0){
				sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
				error_message(tbuff); get_char(); clear_message();
				done=ESC;
				continue;
				
			}

			if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
				sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
				sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}

			/* alloc and display window */
	    		regwn = establish_window( 14, 7, 13, 52, 0,1, dname);

			/* display fields names */
			wprompt(regwn, 5, 1, "OUTPUT");			
			wprompt(regwn, 32, 1, "INPUT");			
	    		wprompt(regwn, 1, 2, "TD");
/* 422 LD boards have no control sigs */
			if(brdstat.lmx[0][0].lmx_id != 6 &&
				brdstat.lmx[0][0].lmx_id != 7)
			{
	    		    wprompt(regwn, 1, 3, "DTR");
	    		    wprompt(regwn, 28, 3, "DCD");
			}
/* 12/24SM and 422  have no RTS/CTS */	
			if(brdstat.lmx[0][0].lmx_id == 0x90F8)
                		ssp4asic = true;

			if(brdstat.lmx[0][0].lmx_id != 6 
				&& brdstat.lmx[0][0].lmx_id != 7
				&& !(brd_def->flags & CTL2) )

			{
	    		    wprompt(regwn, 1, 4, "RTS");
	    		    wprompt(regwn, 28, 4, "CTS");
	    		    wprompt(regwn, 28, 5, "DSR");
	    		    wprompt(regwn, 28, 6, "RI");
	    		}
	
			wprompt(regwn, 1, 5, "OUTFLOW");	
	    		wprompt(regwn, 1, 6, "Status");
			wprompt(regwn, 1, 8, "CPS");
	    	
			wprompt(regwn, 1, 9, "Transmitted");
	    		wprompt(regwn, 1, 10, "Buffered");

	    		wprompt(regwn, 28, 2, "RD");


			wprompt(regwn, 28, 7, "INFLOW");
	    		wprompt(regwn, 28, 8, "CPS");
			
			wprompt(regwn, 28, 9, "Received");
	    		wprompt(regwn, 28, 10, "Buffered");

			/* create linked list of fields */
			fld = establish_field(regwn, 14, 2, regmsk+1, statbuf[0], 'A'); 
/* 422 LD boards have no control sigs */
			if(brdstat.lmx[0][0].lmx_id != 6 &&
				brdstat.lmx[0][0].lmx_id != 7)

			{
	    		fld = establish_field( regwn,
						 14, 3, regmsk+1, ctrsig[DTRBF], 'A');
	    		fld = establish_field(regwn,
						 38, 3, regmsk+1, ctrsig[DCDBF], 'A'); 
			}

/* 12/24SM and 422  have no RTS/CTS */	
			if(brdstat.lmx[0][0].lmx_id != 6 
				&& brdstat.lmx[0][0].lmx_id != 7
				&& !(brd_def->flags & CTL2) )

			{
	    		fld = establish_field(regwn,
						 14,4,regmsk+1,ctrsig[RTSBF],'A');
	  		fld = establish_field(regwn, 38, 4,regmsk+1,ctrsig[CTSBF], 'A');
	    		fld = establish_field(regwn, 38, 5,regmsk+1,ctrsig[DSRBF], 'A');
	    		fld = establish_field(regwn, 38, 6,regmsk+1,ctrsig[RIBF], 'A');
			}

	    	fld = establish_field(regwn, 14, 5, regmsk+1, statbuf[12], 'Z');
	    	fld = establish_field(regwn, 14, 8, regmsk2+3, statbuf[6], 'Z');
    		fld = establish_field(regwn, 38, 2, regmsk+1, statbuf[3], 'A'); 
	    	fld = establish_field(regwn, 14, 6, regmsk2, statbuf[9], 'A'); 

			/* 	 INFLOW status */  
		  	fld = establish_field(regwn, 38, 7,regmsk+1,statbuf[13], 'A');

	    		fld = establish_field(regwn, 38, 8, regmsk2+3, statbuf[7], 'Z');
		
			fld = establish_field(regwn, 14, 9, regmskten, sdp.sp_output,'X');
			fld = establish_field(regwn, 38, 9, regmskten, sdp.sp_input,'X');
	    	
			fld = establish_field(regwn, 14, 10, regmsk, statbuf[8], 'Z'); 
	    		fld = establish_field(regwn, 38, 10, regmsk, statbuf[10], 'Z'); 
	   
		 
			sprintf(sdp.sp_output,"%10d",0);
			sprintf(sdp.sp_input,"%10d",0);
			sprintf(statbuf[6],"%5d", 0);
	    	sprintf(statbuf[7],"%5d", 0);
			field_tally(regwn);
	    	reverse_video(regwn);
	    	time(&oldt);
	    	ftime(&timer);
	    	last_time = timer.time * 1000L + (unsigned int) timer.millitm;	

			bufstat.ebf_nbr = sbuf.st_rdev;
			bufstat.ioctl_version = IOCTL_VERSION;
			if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
				sprintf(tbuff,"ERROR EQNCHBUFSTATUS:%d errno:%d",retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				delete_window(regwn);
				done=ESC; 
				break;
			}

	    	sprintf(quebuf[0],"%4d",2048);
	    	sprintf(quebuf[1],"%4d",2048);
			
			/* macros to calc absolute chars xmitted and rcved */ 
#define MCURRCV (brdstat.brd_cst[chnl].cst_in + bufstat.ebf_incount)
#define MCURXMT (brdstat.brd_cst[chnl].cst_out - bufstat.ebf_outcount)
		   	
			
	    	txcnt = oldtx = 0L;
	    	rxcnt = oldrx = 0L;
			ftime(&timer);
			last_time = timer.time * 1000L + (unsigned int) timer.millitm;
			/* last_time = brdstat.gbl_lbolt * 10L;	 */
			r_save = oldrx = rxold = MCURRCV;
		   	t_save = oldtx = txold = MCURXMT;

#if	(LINUX_VERSION_CODE < 132608)
			/* 2.2 and 2.4 kernels */
			if((mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
				|| (mpstat.serial_flags & ASYNC_NORMAL_ACTIVE)
				|| (mpstat.ec_openwaitcnt)){
#else
			/* 2.6+ kernels */
			if ((mpstat.serial_flags & ASYNC_NORMAL_ACTIVE)
				|| (mpstat.ec_openwaitcnt)){
#endif
 	    		reverse_video(regwn);
				get_phys_char(&mpstat, tbuff);
				wprompt(regwn, 1, 0, tbuff);			
	    		reverse_video(regwn);
			}	
			old_cflag = mpstat.c_cflag; 
#ifdef MONTY /* code removed for release 3.30 */
            if(ssp4asic == true)
                if(chnl >= 4)
                   chnl =  ((chnl - 4) + 64); 

#endif /* MONTY */
			while(1){
				if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
					sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d",
						retval, errno);
					error_message(tbuff); get_char(); 
					clear_message();
					delete_window(regwn);
					done=ESC; 
					break;
				}
				if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
					sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d",retval,errno);
					error_message(tbuff); get_char(); 
					clear_message();
					delete_window(regwn);
					done=ESC; 
					break;
				}
			bufstat.ebf_nbr = sbuf.st_rdev;
			bufstat.ioctl_version = IOCTL_VERSION;
			if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
				sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				delete_window(regwn);
				done=ESC; 
				break;
			}

				if((int) (time(&newt) - oldt) >= upd) {
#if	(LINUX_VERSION_CODE < 132608)
					/* 2.2 and 2.4 kernels */
					if(((mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
						|| (mpstat.serial_flags & ASYNC_NORMAL_ACTIVE)
						|| (mpstat.ec_openwaitcnt))
						&& (old_cflag != mpstat.c_cflag ))
#else
					/* 2.6+ kernels */
					if (((mpstat.serial_flags & ASYNC_NORMAL_ACTIVE)
						|| (mpstat.ec_openwaitcnt))
						&& (old_cflag != mpstat.c_cflag ))
#endif
					{ 
	    				reverse_video(regwn);
						if( old_cflag != mpstat.c_cflag )
							wprompt(regwn, 1, 0, eightyspaces + 28);
						old_cflag = mpstat.c_cflag; 
						get_phys_char(&mpstat, tbuff);
						wprompt(regwn, 1, 0, tbuff);			
	    				reverse_video(regwn);
					}	else {
#if	(LINUX_VERSION_CODE < 132608)
							/* 2.2 and 2.4 kernels */
							if((!(mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
							&& !(mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
							&& !(mpstat.ec_openwaitcnt)){
#else
							/* 2.6 kernels */
							if ((!(mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
							&& !(mpstat.ec_openwaitcnt)){
#endif
	    					reverse_video(regwn);
							wprompt(regwn, 1, 0, eightyspaces + 28);
	    					reverse_video(regwn);
						}
					}	
					/* how much data has been sent */
					if(rxold <= MCURRCV)  
						rxcnt = MCURRCV - rxold;	
					else 		/* cst_in has wrapped at 4 giga */
						rxcnt = (0xffffffff - rxold) + MCURRCV;
					if(txold <= MCURXMT)  
						txcnt = MCURXMT - txold;	
					else 
				  		txcnt = (0xffffffff - txold) + MCURXMT;
				
					/* ----------------- get mills --------*/
					ftime(&timer);
					new_time = timer.time * 1000L + (unsigned int) timer.millitm;
					/* new_time = brdstat.gbl_lbolt * 10L;  */
					trate=(txcnt  * 1000L) /(new_time - last_time);
					rrate=(rxcnt  * 1000L) /(new_time - last_time);
					oldrx = rxcnt;
					oldtx = txcnt;
				
					oldt = newt;

					last_time = new_time; 
		   			sprintf(statbuf[6],"%5d", (unsigned int)trate);
		   			sprintf(statbuf[7],"%5d", (unsigned int)rrate);
					rxold = MCURRCV;
					txold = MCURXMT;
	
				}
				/* are we transmitting or recieving */
				sprintf(statbuf[0],"%s", (MCURXMT == t_save) ? off : on);
				sprintf(statbuf[3],"%s", (MCURRCV == r_save) ? off : on); 
				r_save = MCURRCV;
				t_save = MCURXMT;
				/*  flow control */	
				sprintf(statbuf[12], "%s",
					(bufstat.ebf_outflow_state) ? on : off);
				sprintf(statbuf[13], "%s",
					(bufstat.ebf_inflow_state) ? on : off);

				/* ---------- get out bound control sigs ----------*/
				/* in all cases control sigs are written to buffers.
				   these buffers may not be associated with a field */
				vv = bufstat.ebf_outsigs; 
				sprintf(ctrsig[RTSBF],"%s", (vv & 0x02 ? on : off) );
				sprintf(ctrsig[DTRBF],"%s", (vv & 0x01 ? on : off) );

				vv = bufstat.ebf_insigs;
				sprintf(ctrsig[DCDBF],"%s", (vv & 0x01 ? on : off) );
				sprintf(ctrsig[CTSBF],"%s", (vv & 0x02 ? on : off) );
				sprintf(ctrsig[DSRBF],"%s", (vv & 0x04 ? on : off) );
				sprintf(ctrsig[RIBF],"%s",  (vv & 0x08 ? on : off) );

               
		    	sprintf(sdp.sp_input,"%10u",brdstat.brd_cst[chnl].cst_in);
		    	sprintf(sdp.sp_output,"%10u",brdstat.brd_cst[chnl].cst_out);
				/* get buffered data counts */	
				sprintf(statbuf[8],"%4d", bufstat.ebf_outcount );
				sprintf(statbuf[10],"%4d", bufstat.ebf_incount);
				
#if	(LINUX_VERSION_CODE < 132608)
				/* 2.2 and 2.4 kernels */
				if((!(mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
					&& !(mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
					&& !(mpstat.ec_openwaitcnt))
					sprintf(statbuf[9],"%s", "CLOSED");
				else {
					if((mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
						|| (mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
						sprintf(statbuf[9],"%s", "  OPEN");
					if (mpstat.ec_openwaitcnt)
						sprintf(statbuf[9],"%s", "CDWAIT");
#else
				/* 2.6+ kernels */
				if ((!(mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
					&& !(mpstat.ec_openwaitcnt))
					sprintf(statbuf[9],"%s", "CLOSED");
				else {
					if ((mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
						sprintf(statbuf[9],"%s", "  OPEN");
					if (mpstat.ec_openwaitcnt)
						sprintf(statbuf[9],"%s", "CDWAIT");
#endif
				}	

				field_tally(regwn);
				napms(60);
				nodelay(stdscr, TRUE);
				if((junk = wgetch(stdscr)) == ERR)
					continue;
				else {
					nodelay(stdscr, FALSE);
					flushinp();
					delete_window(regwn);
					/* done=ESC; */
					break;
				}
	    	}
		close(pfd);
		if(done == ESC)
			break;
	}	
	clear_ttyname();
	upd=oldupd;
	return(0);
}	/* stat_port */

/* Get the physical characteristics for a channel */

int get_phys_char(mpstat, psetup)
struct eqnchstatus *mpstat;
char *psetup;
{
	char parstr[25];
	char tempbaud[25];
	unsigned int hold;
	unsigned speed;

	hold=mpstat->c_cflag;

	if(hold == 0){
		sprintf(psetup,"%s", "BAUD DATA and STOP bits set to ZERO");
		return(0);
	}
	if(hold & PARENB) {
		if(hold & PARODD)
			strcpy(parstr, "ODD");	
		else	
			strcpy(parstr, "EVEN");	
	} else
		strcpy(parstr, "NONE");	
	
	speed = hold & CBAUD;
  	if (speed & CBAUDEX){
		speed &= ~CBAUDEX;
		if ((speed >= 1) || (speed <= 4))
			speed += 15;
  	}
	strcpy(tempbaud,speed?baudstrs[speed - 1]:"0");
	sprintf(psetup,"BAUD RATE=%s DATA/STOP BITS=%d/%d PARITY=%s",
		 tempbaud,
		((((hold & CSIZE) >> 3)/2 ) + 5), 
		(CSTOPB & hold ? 2 : 1 ),
		parstr); 

	return(0);
}


#define IFLAG 0
#define OFLAG 1
#define LFLAG 2
#define CFLAG 3
#define BITS 32
/* ---------------------------------------------------------------------*/
/* STAT_TERMIO
	get and display the all termio (stty) flags that are turned on

*/
/* ---------------------------------------------------------------------*/

int
stat_termio()
{
	int hold=0,i,xx, ret;	
	int pfd, retval;
	SMENU smen;
	char tbuff[60];
	struct stat sbuf;
	struct tio_flags *iflags, *flst=NULL;
	struct eqnchstatus mpstat;
	int done=0;
	char **tps;
	char **fl;
	char dname[35];
	smen.func = NULL;

	fl = (char **)malloc(38 * sizeof(char *));
	if(fl == NULL){
			printf("malloc error\n");
			eqnx_exit(1);
		}
   	if((pfd = open( ddevice, O_RDWR )) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}
	while(TRUE){
		if(!get_ttyname(tbuff))
			break;
			tps=fl;

		if(!ismegadev(tbuff)){
			char _temp_str[100];
			sprintf(_temp_str,"%s is not a valid device", tbuff);
			error_message(_temp_str);
			get_char();
			clear_message();
			continue;
		}		

		strcpy(dname, tbuff);	
		if(stat(dname, &sbuf) == -1 ){
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", 
					dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
			error_message(tbuff); get_char(); 
			clear_message();
			continue;
		}
		mpstat.ec_nbr = sbuf.st_rdev;
		mpstat.ioctl_version = IOCTL_VERSION;

		if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
			sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d", retval, errno);
			error_message(tbuff); get_char(); 
			clear_message();
			done=ESC; close(pfd);
			continue;
		}
		/* if not open and not waiting in open */
#if	(LINUX_VERSION_CODE < 132608)
		/* 2.2 and 2.4 kernels */
		if((!(mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
				&& !(mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
				&& !(mpstat.ec_openwaitcnt)){
#else
		/* 2.6+ kernels */
		if ((!(mpstat.serial_flags & ASYNC_NORMAL_ACTIVE))
				&& !(mpstat.ec_openwaitcnt)){
#endif
			warning("Device closed: Termio flags are not applicable");
			continue;
		}

		for(xx=0; xx < CFLAG + 1; xx++){
			unsigned speed;
			i=0;	
			switch(xx) {
				case IFLAG:	hold=mpstat.c_iflag;
						flst=t_iflag;
						break;
				case OFLAG:	hold=mpstat.c_oflag;
						flst=t_oflag;
						break;
				case CFLAG:	hold=mpstat.c_cflag;
						flst=t_cflag;
						if(hold != 0L){
							*tps = (char *)malloc(25);
							if(*tps == NULL){
								printf("malloc error\n");
								eqnx_exit(1);
							}
								
						speed = hold & CBAUD;
  						if (speed & CBAUDEX){
							speed &= ~CBAUDEX;
							if ((speed >= 1) 
							   || (speed <= 4))
								speed += 15;
  						}
		 				   	strcpy(*tps,speed?
						        baudstrs[speed - 1]:"0");
							tps++;
							*tps = (char *)malloc(25);
							if(*tps == NULL){
								printf("malloc error\n");
								eqnx_exit(1);
							}
							strcpy(*tps, Csize[((hold & CSIZE) >> 3)/2]);
							tps++;
						}
						break;
				case LFLAG:	hold=mpstat.c_lflag;
							flst=t_lflag;
							break;
				default:
					break;
				}
			for(; ; i++){
				iflags = flst + i;
				if(iflags->val == 0)
					break;
				if(hold & iflags->val){
					*tps = (char *)malloc(strlen(iflags->fname) + 1);
					if(*tps == NULL){
						printf("malloc error\n");
						eqnx_exit(1);
					}
					strcpy(*tps, iflags->fname);
					tps++;
				}
			}
		}
		if(tps == fl){
			*tps = (char *)malloc(strlen(" NONE ") + 1);
			if(*tps == NULL){
				printf("malloc error\n");
				eqnx_exit(1);
			}
			strcpy(*tps, " NONE " );
			tps++;
		}

		*tps=NULL;
		smen.mselcs = fl;	
		ret = vscroll(" Flags ", &smen, 1, 32, 7);
		tps=fl;
		while(*tps)
			free(*tps++);
	}
	clear_ttyname();	/* clear tty window if any */
	close(pfd);
	free(fl);
	return(0);
}	/* stat_termio */

/*--------------------------------------------------------------------*/
/* 	STAT_REG_RAW()
		Display ICP registers in raw form
		ec_cin
		ec_cout
*/
/*--------------------------------------------------------------------*/
int stat_reg_raw()
{

    SWIN *regwn=NULL, *svwn;
	struct field *fld;
	char erbuf[60];	
	char dname[30];	
    int done= FALSE, once;
    int xx, sfd, idx, n, keyin=0,pfd,retval;
    unsigned short *r;
        struct registers regs;
	struct stat sbuf;
    char tbuff[60];


        while(TRUE){
        done=0;
        if(!get_ttyname(tbuff))	/* get device name */
            break;
        if(!ismegadev(tbuff)){	/* is this a valid equinox device */
			char _temp_str[100];
			sprintf(_temp_str,"%s is not a valid device", tbuff);
			error_message(_temp_str);
			get_char();
			clear_message();
            continue; 
		}
		strcpy(dname, tbuff);
		if(stat(dname, &sbuf) == -1 ){
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", 
					dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
			error_message(tbuff); get_char(); 
			clear_message();
			continue;
		}
		regs.dev = sbuf.st_rdev;
		
        while(done != ESC){					/* open diag device */
    		if((pfd = open( ddevice, O_RDWR )) < 0){
				sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
				error_message(tbuff); get_char(); clear_message();
				done=ESC;
				continue;
			}
			sprintf(tbuff," Reg. dump for %s ", eqfilename);
			regwn = establish_window( 16, 3, 20, 51, 0, 0, tbuff);
			wprompt(regwn, 0, 17, "Press <F3> to write to file.");
			while(1){
				if((retval = ioctl( pfd, EQNREGISTERS,(char *)&regs))){
					sprintf(tbuff,"ERROR EQNREGISTERS:%d errno:%d",retval,errno);
					nodelay(stdscr, FALSE);		/* set to delayed reads */
					error_message(tbuff); get_char(); 
					clear_message();
					done=ESC; close(pfd);
					break;
				}
				once=0;
				for( xx = 0; xx < 16; xx ++){
					r = (unsigned short *) &regs.ec_cin;	
					r = r +  (xx * 8);
					sprintf(erbuf,
					"%4.4hx  %4.4hx  %4.4hx  %4.4hx   %4.4hx  %4.4hx  \
%4.4hx  %4.4hx",

					*r, *(r+1), *(r+2), *(r+3), *(r+4), *(r+5), *(r+6),*(r+7));
					if(xx == 8)
						once++;
					wcursor(regwn, 1, xx + once );
					Wprintf(regwn, erbuf);
				}
				napms(160);			/* sleep for 160 millisecs */
				nodelay(stdscr, TRUE);	/* set stdin to nodelay */
									/* did the user hit a key */
				if((keyin = wgetch(stdscr)) == ERR)
					continue;		/* if not continue */
				else {				/* else we are finished */
					nodelay(stdscr, FALSE);		/* set to delayed reads */
					flushinp();
					done=ESC;
					break;
				}

			}
			if(keyin == KEY_F3){
				svwn = establish_window(15, 11, 3, 45, 0, 0, "");
				field_pass_mode(svwn, 1);
				wprompt(svwn, 5, 0, "Filename:");
				init_template(svwn); 
				fld = establish_field(svwn, 14, 0, msk25, tbuff, 'a'); 
				field_window(fld,"pname   ", 19, 7);
				strcpy(tbuff, "/tmp/eqnprofile");
				field_tally(svwn);
				if(data_entry(svwn) == ESC) {
					delete_window(svwn);
					break;	
				}
				for(idx = 0; idx < 25; idx++)
				if(*(tbuff + idx) == ' ' || !tbuff[idx]){
					*(tbuff + idx) = '\0';
				}
				if((sfd = open(tbuff,
					 O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1){
					sprintf(erbuf, "ERROR open:%s errno:%d", tbuff, errno);
					error_message(erbuf);	
					get_char(); clear_message();
					delete_window(svwn);
					return(0);
				}
				/* ----------------------- */
				for( xx = 0; xx < 16; xx ++){
					r = (unsigned short *) &regs.ec_cin;	
					r = r +  (xx * 8);
					sprintf(erbuf,
					"%4.4hx  %4.4hx  %4.4hx  %4.4hx   %4.4hx  %4.4hx  \
%4.4hx  %4.4hx\n",

					*r, *(r+1), *(r+2), *(r+3), *(r+4), *(r+5), *(r+6),*(r+7));
				/* ----------------------- */
					if(xx == 8)
						write(sfd,"\n", 1);
					if((n = write(sfd,erbuf,
						 strlen(erbuf))) == -1){
						sprintf(erbuf, "ERROR write:%s errno:%d", tbuff, errno);
						error_message(erbuf);	
						get_char(); clear_message();
						delete_window(svwn);
						return(0);
					}
				}	
				close(sfd);	
				delete_window(svwn);
			}
			close(pfd);
			}
			delete_window(regwn);
		}
	clear_ttyname();
	return(0);	

} 	/* stat_raw_regs */

int stat_reset()
{

	char dname[30];	
	int done= FALSE;
	int pfd,retval;
	struct eqnchstatus mpstat;
	struct stat sbuf;
	char tbuff[60];

	while(TRUE){
		done=0;
		if(!get_ttyname(tbuff))	/* get device name */
			break;
		if(!ismegadev(tbuff)){	/* is this a valid equinox device */
			error_message("This is not a valid device");
			get_char();
			clear_message();
			continue; 
		}
		strcpy(dname, tbuff);
		if(stat(dname, &sbuf) == -1 ){
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
			error_message(tbuff); get_char(); 
			clear_message();
			continue;
		}
		mpstat.ec_nbr = sbuf.st_rdev;
		
		if((pfd = open( ddevice, O_RDWR )) < 0){
			sprintf(tbuff,"ERROR OPEN %s: errno:%d",ddevice,errno);
			error_message(tbuff); get_char(); clear_message();
			done=ESC;
			continue;
		}

		if((retval = ioctl( pfd, EQNCHRESET,(char *)&mpstat))){
			sprintf(tbuff,"ERROR EQNCHRESET:%d errno:%d",
				retval,errno);
			nodelay(stdscr, FALSE);	/* set to delayed reads */
			error_message(tbuff); get_char(); 
			clear_message();
			done=ESC; close(pfd);
			break;
		}
		close(pfd);
		break;
	}
	clear_ttyname();
	return(0);	

} 	/* stat_reset */

#ifdef	STRDRVR
#define BRDLOCAL 10
#define EMODLOCAL	11	
#define CHANLOCAL	12	
#endif
#ifdef RS422
#define BRDLOCAL 0
#define EMODLOCAL	1
#define CHANLOCAL	2
#else
#define BRDLOCAL 9
#define EMODLOCAL	10
#define CHANLOCAL	11
#endif

/*-----------------------------------------*/

/* Qualify an eqn device */
/*-----------------------------------------*/
char dvstr[]="/dev/ttyQ";

int ismegadev(st)
char *st;
{
	int module;
	int chanlocal, modlocal, boardlocal;
	
	if((strncmp(st, "/dev/ttyQ", strlen("/dev/ttyQ")) != 0) 
	&& (strncmp(st, "/dev/cuq", strlen("/dev/cuq")) != 0)) {
		return(FALSE);
	}
	module = 0;
	if (strncmp(st,"/dev/ttyQ", strlen("/dev/ttyQ")) == 0){
		boardlocal = 9;
		modlocal = 10;
		chanlocal = 11;
	}
	else{
		boardlocal = 8;
		modlocal = 9;
		chanlocal = 10;
	}
	if(isalpha(st[modlocal])){		/* calculate module */
		module = tolower(st[modlocal]);
		module = module - 'a';
		module++;
	}

	if((st[boardlocal] < '1') ||  (st[boardlocal] > '8')		/* board */
		|| (module < 1 || module > 8) 			/* module */
			|| (atoi(&st[chanlocal]) < 1 )
				||  (atoi( &st[chanlocal]) > 16)) {

		return(FALSE);
	}
	return(TRUE);

}
/*-----------------------------------------*/
/* 
int	EQGET_CHAN(char * );
	- return the logical number 
*/

int eqget_chan(name)
char *name;
{
	int chan, module;
	int chanlocal, modlocal;
	char *nm = name;
	int tmp;
	tmp = chan = 0;
	if (strncmp(name,"/dev/ttyQ", strlen("/dev/ttyQ")) == 0){
		modlocal = 10;
		chanlocal = 11;
	}
	else{
		modlocal = 9;
		chanlocal = 10;
	}


	/* add module */
	module = tolower(nm[modlocal]);
	module = module - 'a';

	/* DEBUG: error chan is lost test fix with multiple brds */
	/* could fix problem with erronios data in port status(counts) */
	tmp = module * 16;
	chan = tmp + chan;
	chan += atoi(&nm[chanlocal]);

	return(chan - 1);
}
/* 
	int EQGET_BOARD(char *);
	- return the logical board number 
*/
int eqget_board(name)
char *name;
{
	int board;
	int brdlocal;
	char *nm = name;
	if (strncmp(name,"/dev/ttyQ", strlen("/dev/ttyQ")) == 0)
		brdlocal = 9;
	else
		brdlocal = 8;
	board = (int) (nm[brdlocal] - '1');

	return(phys_log[board]);
}
/* this routine is really returning physical board */
int log_to_phys(brd)
int brd;
{
	int xx;

	for(xx = 0; xx < SSMAXBRD; xx++){
		if(phys_log[xx] == brd)
			break;
	}
	if(xx == SSMAXBRD)
		return(0);
	return(xx + 1);
}
int phys_to_log(brd)
{
	int xx;

	for(xx = 0; xx < SSMAXBRD; xx++){
		if(phys_log[xx] == brd)
			break;
	}
	if(xx == SSMAXBRD){
		error_message("hit max");
	get_char();
	clear_message();
		return(0);
		}
	return(xx + 1);

}
/*
	get_ttyname - read in a ttyname

*/
/*-----------------------------------------*/
SWIN *ttywn=NULL;
int get_ttyname(bf)
char *bf;
{

	struct field *fld;

	int idx = 0;

	if(ttywn == NULL)
		ttywn = establish_window(21, 9, 3, 45, 0, 0, "");
	field_pass_mode(ttywn, 1);
	wprompt(ttywn, 5, 0, "device:");
	init_template(ttywn); 
	fld = establish_field(ttywn, 13, 0, msk25, bf, 'a'); 
	
	field_window(fld,"pname   ", 19, 7);
	strcpy(bf, eqfilename);
	
	field_tally(ttywn);
	/* make sure we have a visible cursor */
	curs_set(1);
	if(data_entry(ttywn) == ESC) {
		return(0);	
	}
	for(idx = 0; idx < 25; idx++)
		if(*(bf + idx) == ' ' || !bf[idx]){
			*(bf + idx) = '\0';
		}

	strcpy(eqfilename,bf);
	return(1);	
             
}


void clear_ttyname()
{
    if (ttywn){
        delete_window(ttywn);
	}
    ttywn = NULL;
}

/* -Board status---> Counters tree------------------------------------------*/

/*----------------  Transfer counts tree -----------------------------*/
int disp_xcnts();
static int (*bxfer_dummy[])()={ disp_xcnts, disp_xcnts, disp_xcnts,
		disp_xcnts,disp_xcnts,disp_xcnts, disp_xcnts, disp_xcnts,
		disp_xcnts, disp_xcnts};
/* Entry point to display read write counters */
/* Generate board list and indirectly call disp_xcnts */

int
get_xfercnts()
{
	int x;
	struct eqnbrdstatus brdstat;
	char tbuff[80];
	char **fl=NULL;	
	char **tps;	
	SMENU bmen;
	bmen.func = bxfer_dummy;
	if(! fill_board_struct(&brdstat, 1))
		return(0);
	if(brdstat.gbl_neqnbrds < 1) {
		sprintf(tbuff,"No boards found.");
		error_message(tbuff); get_char(); clear_message();
		return(0);

	}

	/* First get the number of boards found */
	if(brdstat.gbl_neqnbrds > 1) {
		fl = (char **)malloc((brdstat.gbl_neqnbrds + 2) * sizeof(char *));
		if(fl == NULL){
			printf("malloc error\n");
			eqnx_exit(1);
		}
		tps=fl;

		for(x=0; x < brdstat.gbl_neqnbrds; x++){
				if(log_to_phys(x + 1) == 0){
					sprintf(tbuff,"Board %d not configured.", x + 1);
					error_message(tbuff); get_char(); clear_message();
					continue;
				}
				*tps = (char *)malloc(35);
				if(*tps == NULL){
					printf("malloc error\n");
					eqnx_exit(1);
				}
				sprintf(*tps,"%s%d", "   Board ",log_to_phys(x + 1));
				tps++;
		}
		if(tps == fl){
			sprintf(tbuff,"No boards configured.");
			error_message(tbuff); get_char(); clear_message();
			free(fl); 
			return(0);
		}
		*tps = NULL;
		bmen.mselcs = fl;
	}
	
	if(brdstat.gbl_neqnbrds == 1){
		Vertical_sel = Board_Selected = 1;	
		disp_xcnts();
	} else {
		Board_Selected = vscroll(" Select Board ",&bmen,Board_Selected,13, 5);

	}
	if(brdstat.gbl_neqnbrds != 1){
		tps = fl;
		while(*tps)
			free(*tps++);
		free(fl); 
	}
	return(0);

}

char *wishtoreset[] = {
	"      Continue      ",
	" Reset I/O Counters ",
	0
};


/* Like disp_info this routine call indirectly; when it is run
  Vertical_sel contains the physical board selected */ 

int
disp_xcnts()
{
	int board;
	struct eqnbrdstatus brdstat;
	SMENU bmen;

	Board_Selected = board = Vertical_sel; 
	if(! fill_board_struct(&brdstat, board ))
		return(0);
	
	disp_xfercnts(board);
	bmen.func = NULL;
	bmen.mselcs = wishtoreset;
	switch(vscroll("", &bmen, 1, 22, 6)){
		case 1:
			break;		
		case 2:
			reset_board_ctrs(board);
			break;	
		default:
			break;	
	}	
	return(0);

}
char xfer_tit[]="Port      Transmit      Receive ";
char xfer_fmt[] = "    %10u    %10u";
char xfercnt[NP][60];
char *pgroups1[] = {
	"Ports a1 through a16",
	"Ports b1 through b16",
	"Ports c1 through c16",
	"Ports d1 through d16",
	0
};
char *pgroups2[] = {
	"Ports a1 through a16",
	"Ports b1 through b16",
	"Ports c1 through c16",
	"Ports d1 through d16",
	"Ports e1 through e16",
	"Ports f1 through f16",
	"Ports g1 through g16",
	"Ports h1 through h16",
	0
};


int show_cnts();
static int (*show_xrcnts[])()={ show_cnts, show_cnts, show_cnts,
		show_cnts, show_cnts, show_cnts, show_cnts, show_cnts};

/*--------------------------*/

int
disp_xfercnts(int bnum)
{
	int board, line, logicalb;
	struct eqnbrdstatus brdstat;
	SMENU pmen;
	struct brdtab_t *brd_def;

	line = 0;
	logicalb = board = bnum; 
	if(! fill_board_struct(&brdstat, board ))
		return(0);
	
	brd_def = get_board_def(&brdstat);
	
	/*  now query for which group the 16 ports the user wants */
	pmen.func = show_xrcnts;
	 
	if(brdstat.nicps == 1)
		pmen.mselcs = pgroups1;			/* one ICP */
	else	
		pmen.mselcs = pgroups2;			/* two ICP's */

	if (brd_def->number_of_ports < 64)
	{
		Board_Selected = logicalb;
		Vertical_sel = Group_Selected = 1;	
		show_cnts();
	} else {
		Board_Selected = logicalb;
		Group_Selected = vscroll(" Select Group ",&pmen, Group_Selected, 20, 6);
	}	
	return(0);	
	
}

int
show_cnts()
{
	int vr, org_group, group, tmpb, pcnt;
	struct eqnbrdstatus brdstat;
	struct field *fld;	
	char tbuff[80];
	SWIN *vwn;

	group = Group_Selected = Vertical_sel;
	group--;
	org_group = group;
	/* get the number of channels */
	brdstat.brd_nbr = Board_Selected;

	if((pcnt = eget_chan_count(&brdstat, Group_Selected)) == 0){
		error_message("No channels available.");
		get_char();
		clear_message();
		return(0);
	}
	
	vwn = establish_window(10, 5, (pcnt == 24) ? 25 : (pcnt + 2), 38,
		(pcnt == 24) ? 1 : 2, 0, xfer_tit);

	init_template(vwn);
	tmpb = 0;

	for(vr= 0; vr < pcnt ; vr ++){
		sprintf(tbuff, "%d%c%d", log_to_phys(Board_Selected) ,
				('a' + group), tmpb + 1);
		tmpb++;
		if(pcnt == 24 && vr == 11){
			group++;
			tmpb=0;	
		}
		wprompt(vwn, 1, vr, tbuff);
	}
	for(vr=0; vr < pcnt ; vr ++){
		sprintf(xfercnt[vr], xfer_fmt,  /* preset count buffers*/
				0 , 0 
		       );
		fld = establish_field(vwn, 6, vr, msk60, xfercnt[vr], 'a');
	}	
	while(TRUE){
		/* not the most efficient way*/
		if(! fill_board_struct(&brdstat, Board_Selected))
			break;
		tmpb = 0;
		group = org_group;
		for(vr= 0; vr < pcnt; vr++){
			if(pcnt == 24 && vr == 12){ /* skip gap in SSM 24 */
				if(group > 0 )
					break;
				group++;
				vr = 0;
			}


			sprintf(xfercnt[tmpb++], xfer_fmt,
					brdstat.brd_cst[vr + (group * 16)].cst_out,
					brdstat.brd_cst[vr + ( group * 16)].cst_in
			       );
		}
		field_tally(vwn);
		napms(160);		/* take a napms 160 mils */
		nodelay(stdscr, TRUE);
		if((wgetch(stdscr)) == ERR)
			continue;
		else {
			nodelay(stdscr, FALSE);
			flushinp();
			break;
		}
	}
	delete_window(vwn);
	return(0);
}

/*end -Board status---> Counters tree---------------------------------------*/

/* -Board status---> Topology tree------------------------------------------*/

int disp_topo();
static int (*topo_dummy[])()={ disp_topo, disp_topo, disp_topo,
		disp_topo,disp_topo,disp_topo, disp_topo, disp_topo,
		disp_topo, disp_topo};
/* Entry point to get and display Topology */
/* Generate board list and indirectly call disp_topo */

int
get_topo()
{
	int x;
	struct eqnbrdstatus brdstat;
	char tbuff[80];
	char **fl=NULL;	
	char **tps;	
	SMENU bmen;

	bmen.func = topo_dummy;
	if(! fill_board_struct(&brdstat, 1))
		return(0);
	if(brdstat.gbl_neqnbrds < 1) {
		sprintf(tbuff,"No boards found.");
		error_message(tbuff); get_char(); clear_message();
		return(0);

	}

	/* First get the number of boards found */
	if(brdstat.gbl_neqnbrds > 1) {
		fl = (char **)malloc((brdstat.gbl_neqnbrds + 2) * sizeof(char *));
		if(fl == NULL){
			printf("malloc error\n");
			eqnx_exit(1);
		}
		tps=fl;

		for(x=0; x < brdstat.gbl_neqnbrds; x++){
				if(log_to_phys(x + 1) == 0){
					sprintf(tbuff,"Board %d not configured.", x + 1);
					error_message(tbuff); get_char(); clear_message();
					continue;
				}
				*tps = (char *)malloc(35);
				if(*tps == NULL){
					printf("malloc error\n");
					eqnx_exit(1);
				}
				sprintf(*tps,"%s%d", "   Board ",log_to_phys(x + 1));
				tps++;
		}
		*tps = NULL;
		bmen.mselcs = fl;
	}
	
	if(brdstat.gbl_neqnbrds == 1){
		Vertical_sel = Board_Selected = 1;	
		disp_topo();
	} else {
		Board_Selected = vscroll(" Select Board ",&bmen,Board_Selected,13, 5);

	}
	if(brdstat.gbl_neqnbrds != 1){
		tps = fl;
		while(*tps)
			free(*tps++);
		free(fl); 
	}
	return(0);

} /* end get_topo() */

/*----------------------------------------------------------------*/
/* disp_topo
	display topology
*/
char *Fss12[] = {
"#--------------------------------$   #-$",
"|                                &-$ |a|   1  2  3  4  5  6  7  8  9 10 11 12",
"|   #---$                        | | &-^",
"|   |   |         #--$           | | |",
"|   %---^         %--^           | &-^",
"|                                | |",
"|                                | |",
"|                                &-^",
"%--------------=--------------=--^",  
"               %--------------^   ",
NULL	
};
char *Fss24[] = {
"#--------------------------------$   #-$",
"|       (Labeled 13-24 on cable) &-$ |b|   1  2  3  4  5  6  7  8  9 10 11 12",
"|   #---$         #--$           | | &-^",
"|   |   |         %--^           | | |",
"|   %---^         #--$           | &-*",
"|                 %--^           | | |",
"|                                | | &-$",
"|       (Labeled 1-12 on cable)  &-^ |a|   1  2  3  4  5  6  7  8  9 10 11 12",
"%--------------=--------------=--^   %-^",
"               %--------------^   ",
NULL	
};

char *Fss2_asic[] = {
	"#-----------------------------------$",
	"|                                   |",
	"|                                   |",
	"|         #-----$                   |",
	"|         |     |                   &---",
	"|         |     |                   |",
	"|         %-----^                   |",
	"|                                   |",
	"|                                   &---",
	"%-----------------=--------------=--^",
	"                  %--------------^   ",
	NULL	
};
char *Fss4_asic[] = {
	"#-----------------------------------$",
	"|                                   |",
	"|                                   &---",
	"|         #-----$                   |",
	"|         |     |                   &---",
	"|         |     |                   |",
	"|         %-----^                   &---",
	"|                                   |",
	"|                                   &---",
	"%-----------------=--------------=--^",
	"                  %--------------^   ",
	NULL	
};
char *Fss4[] = {
	"#-----------------------------------$",
	"|                                   |",
	"|                                   &---",
	"|    #---$                          |",
	"|    |   |           #--$           &---",
	"|    %---^           %--^           |",
	"|                                   &---",
	"|                                   |",
	"|                                   &---",
	"%-----------------=--------------=--^",
	"                  %--------------^   ",
	NULL	
};
char *Fss4LP[] = {
	"#-----------------------------------$",
	"|            #-----$                |",
	"|    #-----$ |     |                |", 
	"|    |     | |     |                &---",
	"|    |     | %-----^                &---",
	"|    %-----^                        &---",
	"|                                   &---",
	"%-----------------=--------------=--^",
	"                  %--------------^   ",
	NULL	
};
char *Fss16I_asic[] = {
	"#--------------------------------$",
	"|                                |",
	"|                                |",
	"|   #---$  #---$  #---$  #---$   |",
	"|   |   |  |   |  |   |  |   |   &-$",
	"|   %---^  %---^  %---^  %---^   | |",
	"|                                &-^",
	"%-----------=--------------=-----^",
	"            %--------------^      ",
	NULL	
};
char *Fss16P_asic[] = {
	"    #----------------------------$",
	"    |     #---$  #---$  #---$    |",
	"    |     |   |  |   |  |   |    |",
	"    |     %---^  %---^  %---^    |",
	"    |                            &-$",
	"    |        #---$  #---$        | |",
	"    |        |   |  |   |        &-^",
	"    |        %---^  %---^        |",
	"    %-------=--------------=-----^",
	"            %--------------^      ",
	NULL	
};
char *Fcp16db9[] = {
	"#----------$",
	"* CP16-DB9 |",
	"%----------^",
	NULL
};
char *Fcp16db[] = {
	"#---------$",
	"* CP16-DB |",
	"%---------^",
	NULL
};
char *Fcp16rj[] = {
	"#---------$",
	"* CP16-RJ |",
	"%---------^",
	NULL
};
char *Fcp16no[] = {
	"  No Port  ",
	"   Panel   ",
	" Connected ",
	NULL
};
char *Fss8_asic[] = {
	"#-----------------------------------$",
	"|                                   &---",
	"|                                   &---",
	"|    #-----$                        &---",
	"|    |     |                        &---",
	"|    %-----^                        &---",
	"|    #-----$                        &---",
	"|    |     |                        &---",
	"|    %-----^                        &---",
	"%-----------------=--------------=--^",
	"                  %--------------^   ",
	NULL	
};
char *Fss8[] = {
	"#-----------------------------------$",
	"|                                   &---",
	"|                                   &---",
	"|    #---$                          &---",
	"|    |   |           #--$           &---",
	"|    %---^           %--^           &---",
	"|                                   &---",
	"|                                   &---",
	"|                                   &---",
	"%-----------------=--------------=--^",
	"                  %--------------^   ",
	NULL	
};

char *Fss64[] = {
	"#--------------------------------$",
	"|                                |",
	"|       #---$                    |",
	"|       |   |                    |",
	"|       %---^           #--$     &-$",
	"|                       %--^     | |",
	"|                                &-^",
	"%--------------=--------------=--^",
	"               %--------------^   ",
	NULL
};
char *Fss128[] = {
	"#--------------------------------$",
	"|                                &-$",
	"|       #---$     #---$       e-h| |",
	"|       |   |     |   |          &-^",
	"|       %---^     %---^  #--$    &-$",
	"|                        %--^ a-d| |",
	"|                                &-^",
	"%--------------=--------------=--^",
	"               %--------------^   ",
	NULL
};

char *Fpm16422[] = {
	"#--------$",
	"*PM16-422|",
	"%--------^",
	NULL
};
char *Fpm16[] = {
	"#--------$",
	"*  PM16  |",
	"%--------^",
	NULL
};
char *Fbackpack32[] = {
	"#------------------$",
	"*       BP32       |",
	"%------------------^",
	NULL
};
char *Fpm8[] = {
	"#--------$",
	"*  PM8   |",
	"%--------^",
	NULL
};
char *mim1[] = {
	"#--------$",
	"*  MIM1  |",
	"%--------^",
	NULL
};
char *Framp1[] = {
	"#--------$",
	"*  MP16  |",
	"%--------^",
	NULL
};
char *Framp2[] = {
	"#--------$",
	"*  MP 2E |",
	"%--------^",
	NULL
};
char *Framp4[] = {
	"#--------$",
	"*  MP 4E |",
	"%--------^",
	NULL
};
char *null_display[] = {
	NULL
};
char *punk[] = {
	"#--------$",
	"*UNKNOWN |",
	"%--------^",
	NULL
};
char *cmx16[] = {
	"#--------$",
	"*  MIM1  |",
	"%---=----^",
/* 	"    |    ", */
	"#---+----$",
	"| CMX16  |",
	"%--------^",
	NULL
};
char *hcmx16[] = {
	"#--------$",
	"| CMX16  |",
	"%---=----^",
/* 	"    |    ", */
	"#---+----$",
	"*  MIM1  |",
	"%--------^",
	NULL
};
#define STCOL	36
#define STROW	6
#define MWIDTH	10

#define LDVFOUND 1
#define RDVFOUND 2
#define MUXFOUND 3
#define LDV8FOUND 4
#define LDVFOUND422 5
#define RAMP1FOUND 6
#define RAMP2FOUND 7
#define RAMP4FOUND 8
#define BPKFOUND 9
#define DBFOUND 10
#define RJFOUND 11
#define NOFOUND 12
#define DB9FOUND 13
#define NONE 0 

int
disp_topo()
{
	int tmp, junk, board, chnl, ii, ee, tidx;
	int pfd, dummy, retval, crow, ccol;	
	int phold, cur_pan, pan_high, connect_a, last_was_bp; 
	struct eqnbrdstatus brdstat;
	struct eqnchstatus mpstat;
	int topo[16];
	int topold[16];
	char dname[35];
	struct stat sbuf;
	char tbuff[80];
	char *figline, **boardsrc=NULL, **tmpsrc;
	SWIN *vwn;
	struct brdtab_t *brd_def;

	Board_Selected = board = Vertical_sel; 
	if(! fill_board_struct(&brdstat, board))
		return(0);

	brd_def = get_board_def(&brdstat);

#define	EBUSTYPE	(brdstat.brd_dev.mpd_bus)

	if (brd_def->primary_id == NOID) {
		sprintf(tbuff,"Unknown board ID: %X",brdstat.id);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}

	for(ii = 0; ii < 8 ; ii++){
		topo[ii] = 0;		/* set topo to nothing */
		topold[ii]= 1;		/* set old to any thing else */
	}

	cur_pan = tidx = chnl = 0;
	/* create the first device name for this board */
	
	gen_dev_name(dname, phys_to_log(Board_Selected), 0);
	if(stat(dname, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			dname, errno);
		nodelay(stdscr, FALSE);
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}
	mpstat.ec_nbr = sbuf.st_rdev;
	brdstat.brd_nbr = Board_Selected;

	dummy = 1;
   	if((pfd = open( ddevice, O_RDWR )) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
         	nodelay(stdscr, FALSE);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}

	gen_dev_name(dname, phys_to_log(Board_Selected), 0);
		nodelay(stdscr, FALSE);
	if(stat(dname, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			dname, errno);
		nodelay(stdscr, FALSE);
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}
	vwn = establish_window(12, 4, 15, 80, 0, 1, "TOPOLOGY");

	/* special case panels on 16I and 16P boards */
	if (brd_def->number_of_ports == 16) {
		if (brd_def->flags & DB9_PAN)
			topo[0] = DB9FOUND;
		if (brd_def->flags & DB25_PAN)
			topo[0] = DBFOUND;
		if (brd_def->flags & RJ_PAN)
			topo[0] = RJFOUND;
		if (brd_def->flags & NOPANEL)
			topo[0] = NOFOUND;
	}

	if (brd_def->asic == SSP2)
		boardsrc = Fss2_asic;
	else if (brd_def->asic == SSP4) {
		if ((brd_def->number_of_ports == 16) &&
		    (brd_def->bus == PCI_BUS))
			boardsrc = Fss16P_asic;		/* SST-16P */

		if ((brd_def->number_of_ports == 16) &&
		    (brd_def->bus != PCI_BUS))
			boardsrc = Fss16I_asic;		/* SST-16I */

		if ((brd_def->number_of_ports == 8) &&
		    (brd_def->bus == PCI_BUS))
			boardsrc = Fss8;		/* SST-8P */

		if ((brd_def->number_of_ports == 8) &&
		    (brd_def->bus != PCI_BUS))
			boardsrc = Fss8_asic;		/* SST-8I */

		if ((brd_def->number_of_ports == 4) &&
		    (brd_def->bus == PCI_BUS)) {
			if (brd_def->flags & LP)
				boardsrc = Fss4LP;	/* SST-4P/LP */
			else	boardsrc = Fss4;	/* SST-4P */
		}

		if ((brd_def->number_of_ports == 4) &&
		    (brd_def->bus != PCI_BUS))
			boardsrc = Fss4_asic;		/* SST-4I */

		if (brd_def->number_of_ports == 2)
			boardsrc = Fss2_asic;		/* SST-2 */
		
	} else {

		switch (brd_def->number_of_ports) {
			case 2:
			 	boardsrc = Fss2_asic;	/* SST-2 */
				break;
			case 4:
				boardsrc = Fss4;	/* SST-4 */
				break;
			case 8:
				boardsrc = Fss8;	/* SST-8 */
				break;
			case 12:
				boardsrc = Fss12;	/* SST-12 */
				break;
			case 24:
				boardsrc = Fss24;	/* SST-24 */
				break;
			case 64:
				boardsrc = Fss64;	/* SST-64 */
				break;
			case 128:
				boardsrc = Fss128;	/* SST-128 */
				break;
		}
	}
	
	figline = *boardsrc;
	crow =2;
	while((figline = *boardsrc) != NULL){
	ccol =0;
	ee = 0;
		while (*figline != '\0'){
			wcursor(vwn, ccol, crow );
			switch(*figline){
			case '#':			/* upper lefthand corner */	
					wputacs(vwn, ACS_ULCORNER); break;
			case '$':			/* upper right corner */
					wputacs(vwn, ACS_URCORNER); break;
			case '%':			/* lower left corner */		
					wputacs(vwn, ACS_LLCORNER); break;
			case '^':			/* lower right corner */
					wputacs(vwn, ACS_LRCORNER); break;
			case '&':			/* right tee */
					wputacs(vwn, ACS_LTEE); break;
			case '*':			/* left tee */
					wputacs(vwn, ACS_RTEE); break;
			case '+':			/* Bottom tee */
					wputacs(vwn, ACS_BTEE); break;
			case '=':			/* Top tee */
					wputacs(vwn, ACS_TTEE); break;
			case '-':			/* horizontal line */
					wputacs(vwn, ACS_HLINE); break;
			case '|':		/* vertical line */
					wputacs(vwn, ACS_VLINE); break;
			default:
					wputchar(vwn, *figline); /* ascii */
					break;
			}
		ccol++;	
		ee++;
		figline++;
		}
		crow++;
		boardsrc++;
	}
	sprintf(tbuff, "%s", brd_def->name);
	wprompt(vwn, 2, 0, tbuff);

	ee = 0;
	ccol = 2;
	crow = 12;
	wrefresh(stdscr);
/* if ISA board show switch */

/* ------------------------ */
#define STCOL	36
#define STROW	6
#define MWIDTH	10
	curs_set(0);

	nodelay(stdscr, TRUE);
	while(1){
		if((junk = wgetch(stdscr)) == ERR){
			napms(50); 
		} else {
			nodelay(stdscr, FALSE);
			flushinp();
			delete_window(vwn);
			break;
		}
 		if((brd_def->number_of_ports == 12) ||
		   (brd_def->number_of_ports == 24))
        {
			nodelay(stdscr, FALSE);

            tmpsrc = boardsrc;
           
            connect_a = false;
            if(brd_def->number_of_ports == 24)
            {
               boardsrc += 5;  /* point to side 'a' */
               connect_a = true;
            }
        sspstub(0);

        junk = 0;
        do 
        {
            reverse_video(vwn);
            if(connect_a)
            { 
               boardsrc = tmpsrc + 5;  /* point to side 'a' */
               crow = 7;      
               connect_a = true;
            }
            else
            {
               boardsrc = tmpsrc;  /* point to side 'b' */
               crow = 2;
               connect_a = false;
            }

	        while((figline = *boardsrc) != NULL){
        
	        ccol = 37;
            figline = figline + 37;

	          ee = 0;
		        while (*figline != '\0'){
			        wcursor(vwn, ccol, crow );
			        switch(*figline){
			        case '#':			/* upper lefthand corner */	
					        wputacs(vwn, ACS_ULCORNER); break;
			        case '$':			/* upper right corner */
					        wputacs(vwn, ACS_URCORNER); break;
			        case '%':			/* lower left corner */		
					        wputacs(vwn, ACS_LLCORNER); break;
			        case '^':			/* lower right corner */
					        wputacs(vwn, ACS_LRCORNER); break;
			        case '&':			/* right tee */
					        wputacs(vwn, ACS_LTEE); break;
			        case '*':			/* left tee */
					        wputacs(vwn, ACS_RTEE); break;
			        case '+':			/* Bottom tee */
					        wputacs(vwn, ACS_BTEE); break;
			        case '=':			/* Top tee */
					        wputacs(vwn, ACS_TTEE); break;
			        case '-':			/* horizontal line */
					        wputacs(vwn, ACS_HLINE); break;
			        case '|':		/* vertical line */
					        wputacs(vwn, ACS_VLINE); break;
			        default:
					        wputchar(vwn, *figline); /* ascii */
					        break;
			        }
		        ccol++;	
		        ee++;
		        figline++;
                if(ee == 3)
                   break;
		        }
		        crow++;
                if(connect_a)
                {
                   if(crow == 11)  /* stop at this row */
                       break;
                } 
                else
                {
                   if(crow == 6)  /* stop at this row */
                       break;

                }
		        boardsrc++;
	        }
            sspstub(0);
            junk = get_char();

            if( junk == '\r' || junk == '\n' 
                || junk == KEY_ENTER )
                navigate_mega(Board_Selected, 
                        (brd_def->number_of_ports == 24) ? connect_a : true);

                
            if(junk != ESC)
            {     /* remove highlight */
               reverse_video(vwn);
               if(connect_a)
               { 
                  boardsrc = tmpsrc + 5;  /* point to side 'a' */
                  crow = 7;      
                  connect_a = true;
               }
               else
               {
                  boardsrc = tmpsrc;  /* point to side 'b' */
                  crow = 2;
                  connect_a = false;
               }
   
	           while((figline = *boardsrc) != NULL){
           
	           ccol = 37;
               figline = figline + 37;
   
	             ee = 0;
		           while (*figline != '\0'){
			           wcursor(vwn, ccol, crow );
			           switch(*figline){
			           case '#':			/* upper lefthand corner */	
					           wputacs(vwn, ACS_ULCORNER); break;
			           case '$':			/* upper right corner */
					           wputacs(vwn, ACS_URCORNER); break;
			           case '%':			/* lower left corner */		
					           wputacs(vwn, ACS_LLCORNER); break;
			           case '^':			/* lower right corner */
					           wputacs(vwn, ACS_LRCORNER); break;
			           case '&':			/* right tee */
					           wputacs(vwn, ACS_LTEE); break;
			           case '*':			/* left tee */
					           wputacs(vwn, ACS_RTEE); break;
			           case '+':			/* Bottom tee */
					           wputacs(vwn, ACS_BTEE); break;
			           case '=':			/* Top tee */
					           wputacs(vwn, ACS_TTEE); break;
			           case '-':			/* horizontal line */
					           wputacs(vwn, ACS_HLINE); break;
			           case '|':		/* vertical line */
					           wputacs(vwn, ACS_VLINE); break;
			           default:
					           wputchar(vwn, *figline); /* ascii */
					           break;
			           }
		           ccol++;	
		           ee++;
		           figline++;
                   if(ee == 3)
                      break;
		           }
		           crow++;
                   if(connect_a)
                   {
                      if(crow == 11)  /* stop at this row */
                          break;
                   } 
                   else
                   {
                      if(crow == 6)  /* stop at this row */
                          break;
   
                   }
   
		           boardsrc++;
	           }
               if(!(junk == '\r' || junk == KEY_ENTER || junk == '\n') 
                       && (brd_def->number_of_ports == 24) )
               {
                   if(connect_a)
                      connect_a = false;
                   else
                      connect_a = true;
               }
	            wrefresh(stdscr);
            }

            if(junk == ESC) 
               break;

        } while(true);


           reverse_video(vwn);
	       /* DEBUG : not needed */ wrefresh(stdscr);
#ifdef FOOOOOI
           if(junk != ESC)
              navigate_mega(Board_Selected, connect_a);
#endif
           if(junk == ESC)
           {
			  nodelay(stdscr, FALSE);
			  flushinp();
			  delete_window(vwn);
			  break;
		   }
      }

 
	tmp = brd_def->number_of_ports;

	/* account for SST-16 boards with no panel */
	if (tmp == 16 && (brd_def->flags & NOPANEL))
		tmp = 0;

	if (brd_def->number_of_ports < 64) {

          if (tmp == 16 || tmp == 0) {  /* SST-16I or SST-16P */
             nodelay(stdscr, FALSE);
             redraw_panel(vwn, &brdstat, topo, 0, 0);
             junk = 0;
             do 
             {
               junk = get_char();


               if( junk == '\r' || junk == '\n' 
                || junk == KEY_ENTER )
                 navigate_brd(Board_Selected, cur_pan, 
                              tmp,(topo[ cur_pan ] == RAMP1FOUND),LDVFOUND);
               if(junk == ESC) 
                  break;
             } while(true);
             if(junk == ESC)
             {
			       nodelay(stdscr, FALSE);
			       flushinp();
			       delete_window(vwn);
			       break;
	      }
          } /* if (tmp == 16) */
         else { 
		
           if(tmp != 0) {
              wprompt(vwn, 40  , 0, "ON");

              reverse_video(vwn);

              wprompt(vwn, 44  , 0, "[  ]");
              reverse_video(vwn);

              wprompt(vwn, 50  , 0, "OFF");
              wprompt(vwn, 55  , 0, "[  ]");
              for(ii = 0; ii < tmp; ii++)
              {
                 sprintf(tbuff, "%d", ii + 1);
                 wprompt(vwn, 42 + (ii * 5)  , 2, tbuff);
              }
           }

           navigate_ports(Board_Selected, tmp);
        } /* end else */
		if (tmp != 0)
		   delete_window(vwn);
		break;
	}	   
		tidx = chnl = 0;
#define EICPCOUNT (brdstat.nicps)
#define	S_LDV_ALIVE (brdstat.lmx[ee][(chnl + 1) / 16].lmx_active)
#define	S_LDV_ID (brdstat.lmx[ee][(chnl + 1) / 16].lmx_id)
#define	S_RDV_ALIVE (brdstat.lmx[ee][(chnl + 1) / 16].lmx_rmt_active)
#define	S_RDV_ID (brdstat.lmx[ee][(chnl + 1) / 16].lmx_rmt_id)

				if(! fill_board_struct(&brdstat, brdstat.brd_nbr))
				{
					delete_window(vwn);
					nodelay(stdscr, FALSE);
					return(0);
				}
		for(ee = 0; ee < EICPCOUNT; ee++)	{
			for(chnl = 0; chnl < 64; chnl+= 16)
			{

               gen_dev_name(dname, phys_to_log(Board_Selected), chnl );
               if(stat(dname, &sbuf) == -1 ){
                   sprintf(tbuff, "f_c_s ERROR: board(%d) chnl(%d) stat %s, errno %d",
                    Board_Selected,chnl,dname, errno);
                 nodelay(stdscr, FALSE);
                 error_message(tbuff); get_char();
                 clear_message();
			        delete_window(vwn);
			        return(0);
              }
            mpstat.ec_nbr = sbuf.st_rdev;
	    mpstat.ioctl_version = IOCTL_VERSION;

				if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
					sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d",retval,errno);
					nodelay(stdscr, FALSE);		/* set to delayed reads */
					error_message(tbuff); get_char(); 
					clear_message();
			      delete_window(vwn);
					close(pfd);
					return(0);
				}
				switch( S_LDV_ALIVE ){
					case DEV_GOOD:
						if(S_LDV_ID == 0x10){   /* GDC */
							topo[tidx++] = BPKFOUND;
							break;
						}	
						if(S_LDV_ID == 0){
							topo[tidx++] = LDVFOUND;
							break;
						}	
						if(S_LDV_ID == 7){
							topo[tidx++] = LDVFOUND422;
							break;
						}	
						if(S_LDV_ID == 1) {  /* bridge */
							if(S_RDV_ALIVE == DEV_GOOD 
								&& S_RDV_ID == 2) {	/* mux ? */
								topo[tidx++] = MUXFOUND;
							} else {
								topo[tidx++] = RDVFOUND;
							}	
							break;
						}
						if(S_LDV_ID == 4 ){
							topo[tidx++] = LDV8FOUND;
							break;
						}	
						if(S_LDV_ID == 8 ){
							topo[tidx++] = RAMP1FOUND;
							break;
						}	
						if(S_LDV_ID == 9 ){
							topo[tidx++] = RAMP2FOUND;
							break;
						}	
						if(S_LDV_ID == 0xB ){
							topo[tidx++] = RAMP4FOUND;
							break;
						}	
						topo[tidx++] = NONE;
						break;
					case DEV_VIRGIN:	
						topo[tidx++] = NONE;
						break;
					case DEV_BAD:
						topo[tidx++] = NONE;
						break;
					default:
						topo[tidx++] = NONE;
						break;
				}
			}
		}
		/* if topology changed? Clear old map */
		if(topo[0] != topold[0] || topo[1] != topold[1] ||
			topo[2] != topold[2] || topo[3] != topold[3] ||
			topo[4] != topold[4] || topo[5] != topold[5] ||
			topo[6] != topold[6] || topo[7] != topold[7] ){
			for(ee = 0 ; ee <  12; ee++){		/* clear window */
				for(ii = STCOL; ii < 78; ii++){
					wcursor(vwn, ii, ee );
					wputchar(vwn, ' ');
				}
			}
		} else {		/* continue polling for key entry */
 
           if(cur_pan == 0)
              if(topo[cur_pan] == NONE)
                  cur_pan = 4;
           if(cur_pan == 4)
              if(topo[cur_pan] == NONE)
                  cur_pan = 0;
               
           junk = 0;
           pan_high = FALSE;
           if(topo[cur_pan] != NONE)
		       while(junk != ESC){
           		if(cur_pan == 0)
              		if(topo[cur_pan] == NONE)
                  		cur_pan = 4;
           		if(cur_pan == 4)
              		if(topo[cur_pan] == NONE)
                  		cur_pan = 0;

			if ((brd_def->number_of_ports == 64) ||
			    (brd_def->number_of_ports == 128)) {
                           sspstub(2);
                           if(!pan_high)
                           {
                              redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                              pan_high = TRUE;
                           }
	               }
				  nodelay(stdscr, TRUE);
	              if((junk = wgetch(stdscr)) == ERR)
				         ;
	              else
                   switch(junk)
                   {
			           case BS:
			           case KEY_BACKSPACE:
			           case KEY_LEFT:
                           last_was_bp =
                               redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                           pan_high = FALSE;
                           
                            if(cur_pan == 0)
                            { 
                               tmp = 7;
                               
                               for(tmp = 7; tmp > 0; tmp--)
                               if(topo[tmp] == BPKFOUND)
                                  tmp--;
                                  if(topo[tmp] != NONE)
                                     break;
                               cur_pan = tmp;
                            } 
                            else
                            {
                               for(tmp = cur_pan - 1; tmp > 0; tmp--)
                                  if(topo[tmp] != NONE)
                                     break;
                               if(topo[tmp] == BPKFOUND)
                                  tmp--;
                               cur_pan = tmp;
                            }
                            break;
			           case '\t':
			           case KEY_RIGHT:
                            last_was_bp = 
                               redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                            pan_high = FALSE;
                            if(cur_pan == 7)
                            { 
                               for(tmp = 0; tmp < 8; tmp++)
                                  if(topo[tmp] != NONE)
                                     break;
                               cur_pan = tmp;
                            } 
                            else 
                            {
                               if(last_was_bp)  /* Backpack */
                                  tmp = cur_pan + 2;
                               else
                                  tmp = cur_pan + 1;
                               for(; tmp < 8; tmp++)
                                 if(topo[tmp] != NONE)
                                    break;
                               if(tmp > 7)
                                  cur_pan = 0;
                               else
                                  cur_pan = tmp;
                            }
                            break;
                       case ERR:
                            napms(10); 
                            break;
			           case DN:
			           case KEY_DOWN:
                            redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                            pan_high = FALSE;
                            if(cur_pan > 3 )
                            { 
                               phold = cur_pan;
                               cur_pan -= 4;
                               for(tmp = cur_pan; tmp > 0; tmp--)
                                  if(topo[tmp] != NONE)
                                     break;
                               if((topo[tmp] == BPKFOUND) && (tmp != 0))
                               {
                                  /* no way to tell if 1st or 2nd lmx on */
                                  if((tmp == 2) && (topo[0] == BPKFOUND)
                                       && (topo[1] == BPKFOUND))
                                     ; /* 2 backpacks on a ring */
                                  else if((tmp == 1) && (topo[0] != BPKFOUND))
                                     ;
                                  else
                                      tmp--;
                               }
                               cur_pan = tmp;
                               break;
                            } 
                            if(cur_pan < 4 )
                            { 
                               phold = cur_pan;
                               cur_pan += 4;
                               for(tmp = cur_pan; tmp > 4; tmp--)
                                  if(topo[tmp] != NONE)
                                     break;
                               if(topo[tmp] == BPKFOUND && (tmp != 4))
                               {
                                  /* no way to tell if 1st or 2nd lmx on */
                                  if((tmp == 6) && (topo[4] == BPKFOUND)
                                       && (topo[5] == BPKFOUND))
                                     ; /* 2 backpacks on a ring */
                                  else if((tmp == 5) && (topo[4] != BPKFOUND))
                                     ;
                                  else
                                     tmp--;
                               }
                               cur_pan = tmp;
                               if(topo[cur_pan] == NONE)
                                  cur_pan = 0;
                            } 
                            break;
                      case ESC:

                            break;
			           case '\r':
			           case 'j':
			           case KEY_ENTER:
                           redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                           if(topo[ cur_pan ] == LDV8FOUND)
                              tmp = 8;
                           else
                              tmp = 16;
                              junk = navigate_brd(Board_Selected, cur_pan, 
                                          tmp,(topo[ cur_pan ] == RAMP1FOUND),
                                            topo[cur_pan]);
                                     /* ----- done for pairing */
                           redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                           break;

			           case KEY_UP:
			           case UP:
                            redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                            pan_high = FALSE;
                            if(cur_pan < 4 )
                            { 
                               phold = cur_pan;
                               cur_pan += 4;
                               for(tmp = cur_pan; tmp > 0; tmp--)
                                  if(topo[tmp] != NONE)
                                     break;
                               if(topo[tmp] == BPKFOUND && (tmp != 4))
                               {
                                  /* no way to tell if 1st or 2nd lmx on */
                                  if((tmp == 6) && (topo[4] == BPKFOUND)
                                       && (topo[5] == BPKFOUND))
                                     ; /* 2 backpacks on a ring */
                                  else if((tmp == 5) && (topo[4] != BPKFOUND))
                                     ;
                                  else
                                     tmp--;
                               }
                               cur_pan = tmp;
                            } 
                            else
                            { 
                               phold = cur_pan;
                               cur_pan -= 4;
                               for(tmp = cur_pan; tmp > 4; tmp--)
                                  if(topo[tmp] != NONE)
                                     break;
                               if((topo[tmp] == BPKFOUND) && (tmp != 0))
                               {
                                  /* no way to tell if 1st or 2nd lmx on */
                                  if((tmp == 2) && (topo[0] == BPKFOUND)
                                       && (topo[1] == BPKFOUND))
                                     ; /* 2 backpacks on a ring */
                                  else if((tmp == 1) && (topo[0] != BPKFOUND))
                                     ;
                                  else
                                       tmp--;
                                }
                               cur_pan = tmp;
                               if(topo[cur_pan] == NONE)
                                  cur_pan = 0;
                            } 
                            break;

                       default:
                           redraw_panel(vwn, &brdstat, topo, cur_pan, 1);
                           pan_high = FALSE;
#ifdef FOOOOI
						   nodelay(stdscr, FALSE);
                           sprintf(tbuff, "Input is default 0x%X", junk);
						   error_message(tbuff); get_char(); clear_message();
						     nodelay(stdscr, TRUE);
#endif
                            break;
                   }
                   if(junk == ESC)
                      break;

                   if(junk == ERR)
                   {
                      tidx = chnl = 0;
/*==========================*/
				      if(! fill_board_struct(&brdstat, brdstat.brd_nbr))
				      {
					      delete_window(vwn);
					      nodelay(stdscr, FALSE);
					      return(0);
				      }
		              for(ee = 0; ee < EICPCOUNT; ee++)	{
			              for(chnl = 0; chnl < 64; chnl+= 16)
			              {
				              switch( S_LDV_ALIVE ){
					              case DEV_GOOD:
						              if(S_LDV_ID == 0x10){   /* GDC */
							              topo[tidx++] = BPKFOUND;
							              break;
						              }	
						              if(S_LDV_ID == 0){
							              topo[tidx++] = LDVFOUND;
							              break;
						              }	
						              if(S_LDV_ID == 7){
							              topo[tidx++] = LDVFOUND422;
							              break;
						              }	
						              if(S_LDV_ID == 1) {  /* bridge */
							              if(S_RDV_ALIVE == DEV_GOOD 
								              && S_RDV_ID == 2) {	/* mux ? */
								              topo[tidx++] = MUXFOUND;
							              } else {
								              topo[tidx++] = RDVFOUND;
							              }	
							              break;
						              }
						              if(S_LDV_ID == 4 ){
							              topo[tidx++] = LDV8FOUND;
							              break;
						              }	
						              if(S_LDV_ID == 8 ){
							              topo[tidx++] = RAMP1FOUND;
							              break;
						              }	
						              if(S_LDV_ID == 9 ){
							              topo[tidx++] = RAMP2FOUND;
							              break;
						              }	
						              if(S_LDV_ID == 0xB ){
							              topo[tidx++] = RAMP4FOUND;
							              break;
						              }	
						              topo[tidx++] = NONE;
						              break;
					              case DEV_VIRGIN:	
					              case DEV_BAD:
					              default:
						              topo[tidx++] = NONE;
						              break;
				              }
			              }
		              }

		              if(topo[0] != topold[0] || topo[1] != topold[1] ||
			              topo[2] != topold[2] || topo[3] != topold[3] ||
			              topo[4] != topold[4] || topo[5] != topold[5] ||
			              topo[6] != topold[6] || topo[7] != topold[7] ){
                           reverse_video(vwn);
			               for(ee = 0 ; ee <  12; ee++){ /* clear window */
				               for(ii = STCOL; ii < 78; ii++){
					               wcursor(vwn, ii, ee );
					               wputchar(vwn, ' ');
				               }
			               }
                           break;
                      }
                   }
                   napms(100);
               }
		}

		if(topo[0] != NONE){ 	/* print fig for lower connector*/
			wcursor(vwn, 35, STROW + 1);
			wputacs(vwn, ACS_LTEE);
		} else {
			wcursor(vwn, 35, STROW + 1);
			wputacs(vwn, ACS_VLINE);
		}

		if(brd_def->number_of_ports == 128) { 
			/* print fig for upper connector */
			if(topo[4] != NONE){ 
				wcursor(vwn, 35, STROW - 2);
				wputacs(vwn, ACS_LTEE);
			} else {
				wcursor(vwn, 35, STROW - 2);
				wputacs(vwn, ACS_VLINE);
			}
		}
		ii = 0;
/****************/
     if(junk != ESC )
		for(ee = 0; ee < EICPCOUNT; ee++){
            last_was_bp = false;
			for(; ii < (ee + 1) * 4 ;ii++){
                if(!last_was_bp)
                { 
                  if(!(topo[ii] == NONE))
                    last_was_bp = redraw_panel(vwn, &brdstat, topo, ii, 0);
                }
                else
                   last_was_bp = false;
			}
		}
		/* save old setup */
		for(ii = 0; ii < 8; ii++)
			topold[ii] = topo[ii];

		if(junk == ESC){
			nodelay(stdscr, FALSE);
			flushinp();
			delete_window(vwn);
			break;
		}
	}

/* end polling */
	close(pfd);
	curs_set(1);	
	return(0);

}
int redraw_panel( vwn, brdstat, topo,  cur_pan, state)
SWIN *vwn;
struct eqnbrdstatus *brdstat;
int *topo;
int cur_pan;
int state;
{

   int ii, ee, vv, tt, crow, ccol;
   char *figline, **boardsrc;
   struct brdtab_t *brd_def;

   brd_def = get_board_def(brdstat);
   
   if(state)
       reverse_video( vwn );
    else
    {
		if(topo[0] != NONE){ 	/* print fig for lower connector*/
			wcursor(vwn, 35, STROW + 1);
				wputacs(vwn, ACS_LTEE);
		} else {
			wcursor(vwn, 35, STROW + 1);
				wputacs(vwn, ACS_VLINE);
		}
		if(brd_def->number_of_ports == 128) { 
			/* print fig for upper connector */
			if(topo[4] != NONE){ 
				wcursor(vwn, 35, STROW - 2);
				wputacs(vwn, ACS_LTEE);
			} else {
				wcursor(vwn, 35, STROW - 2);
				wputacs(vwn, ACS_VLINE);
			}
		}
     }
#ifdef FOOOOOOI
     if(cur_pan > 1)
     {
       if(topo[cur_pan] == 0x09 && topo[cur_pan - 1] == 0x09)
          return(1);
     }
#endif
        if(cur_pan <= 3)
          ee =  0;
        else
          ee = 1;
		ii = cur_pan;
		     switch(topo[ii]){		/* get figs for modules */
	                case DB9FOUND:
						boardsrc = Fcp16db9;
						break;
	                case DBFOUND:
						boardsrc = Fcp16db;
						break;
	                case RJFOUND:
						boardsrc = Fcp16rj;
						break;
	                case NOFOUND:
						boardsrc = Fcp16no;
						break;
	                case LDVFOUND:
						boardsrc = Fpm16;
						break;
	                case BPKFOUND:
						boardsrc = Fbackpack32;
						break;
					case LDVFOUND422:
						boardsrc = Fpm16422;
						break;
					case LDV8FOUND:
						boardsrc = Fpm8;
						break;
					case RAMP1FOUND:
						boardsrc = Framp1; 
						break;
					case RAMP2FOUND:
						boardsrc = Framp2; 
						break;
					case RAMP4FOUND:
						boardsrc = Framp4; 
						break;
					case RDVFOUND:
						boardsrc = mim1; 
						break;
					case MUXFOUND:
						if(ii < 4)
							boardsrc = cmx16; 
						else boardsrc = hcmx16;
						break;
					default:
						boardsrc = null_display; 
						break;	

		     }

				/* this doesn't make sense  */
				/* and its not just SST16 - mikes */
				/*-----------------------------*/
                                /* Kludge for SST-16I and SST-16P */
				if (brd_def->asic != SSP64)
      					reverse_video( vwn );

				tt = 0;
				while((figline = *boardsrc) != NULL){
					if( ii <= 3)
						ccol =STCOL + (MWIDTH * ii);
					else	ccol =STCOL + (MWIDTH * (ii - 4));

					vv = 0;
					if(ii > 3 && topo[ii] == MUXFOUND){
						crow = STROW - 3 ;
					} else {
						crow = STROW;
					}
					while (*figline != '\0'){
						if(ii <= 3)
							wcursor(vwn, ccol, crow + tt );
						else wcursor(vwn, ccol, crow + tt  - 3);

						switch(*figline){
						case '#':			/* upper lefthand corner */	
							wputacs(vwn, ACS_ULCORNER); break;
						case '$':			/* upper right corner */
							wputacs(vwn, ACS_URCORNER); break;
						case '%':			/* lower left corner */		
							wputacs(vwn, ACS_LLCORNER); break;
						case '^':			/* lower right corner */
							wputacs(vwn, ACS_LRCORNER); break;
						case '&':			/* right tee */
							wputacs(vwn, ACS_LTEE); break;
						case '*':			/* left tee */
							wputacs(vwn, ACS_RTEE); break;
						case '+':			/* Bottom tee */
							wputacs(vwn, ACS_BTEE); break;
						case '=':			/* Top tee */
							wputacs(vwn, ACS_TTEE); break;
						case '-':			/* horizontal line */
							wputacs(vwn, ACS_HLINE); break;
						case '|':		/* vertical line */
							wputacs(vwn, ACS_VLINE); break;
						default:
							wputchar(vwn, *figline); break;
						}
						ccol++;	
						vv++;
						figline++;
					}
					boardsrc++;
					tt++;
				}

				if(ii < 3 ){
					if(topo[ii + 1] != NONE){
                      if(topo[ii] != BPKFOUND)
                      {
					     wcursor(vwn, STCOL + ((MWIDTH  * (ii + 1 )) - 1),
						    STROW + 1 +  (ee * 1) );
						 wputacs(vwn, ACS_LTEE);
                      }

                      if((topo[ii] == BPKFOUND) && (topo[ii + 2] != NONE)
                            && (ii != 2))
                      {
						wcursor(vwn, STCOL + ((MWIDTH  * (ii + 1 )) + 9),
						 STROW + 1 +  (ee * 1) );

						wputacs(vwn, ACS_LTEE);
                      }
					} 
				}
			if(ii > 3 && ii < 7 ){
					if(topo[ii + 1] != NONE){
                      if(topo[ii] != BPKFOUND)
                      {
					    wcursor(vwn, STCOL + ((MWIDTH  * ((ii - 4) + 1 )) - 1),
						  STROW - 2);
						 wputacs(vwn, ACS_LTEE);
                      }
                      if((topo[ii] == BPKFOUND) && (topo[ii + 2] != NONE)
                            && (ii != 2))
                      {
						wcursor(vwn, STCOL + ((MWIDTH  * ((ii - 4) + 1 )) + 9),
						 STROW - 2);
						wputacs(vwn, ACS_LTEE);
                      }
					}
				}

				wrefresh(stdscr);

       return((topo[cur_pan] == BPKFOUND) ? true : false );
}


/*--------------------------------------------------------------------------*/
int navigate_brd( board, lmx, pcount, ramp_found, id )
int board;
int lmx;
int pcount;
int ramp_found; /* true if RAMP */
int id;
{
/* ------------------------------------------ */
	SWIN *regwn;
    short int present[16];
	int  done, ports, oldupd;
	int ii, aa, pfd=0, retval, err_ret;
	unsigned int junk;
    char cmod;
	struct stat sbuf;
    key_t key = 0x0;
    key_t key2 = 0x0;
    int shmid;
    int shmid2;
    unsigned int oldrxcnt[32];
    unsigned int oldtxcnt[32];
    SWIN *pwin[32];
    short int child_scan_id[16];
	struct eqnchstatus mpstat;
	struct eqnchstatus *mpstat_arr;
	struct eqnbrdstatus brdstat;
	struct eqnbufstatus bufstat;
	struct eqnbufstatus *bufstat_arr;
    int *quit_now, stat_loc;
    pid_t tchd;
	unsigned char vv;
    unsigned char last_in[32];
    unsigned char last_out[32];
    unsigned char last_rcv[32];
    unsigned char last_xmt[32];
	char tbuff[60];
	char dname[35];	
    int height, row ,width;
	/*unsigned char tempfdstr[100];*/
	oldupd = upd;
	upd = 1;
	err_ret = done = 0;


    shmid = 0;			/* get a shared memory segment */
    shmid = shmget( key, 0x10000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
    shmid2 = 0;			/* get a shared memory segment */
    shmid2 = shmget( key2, 0x10000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
	/* set up varables to share memory */
    mpstat_arr = (struct eqnchstatus *)NULL;
    bufstat_arr = (struct eqnbufstatus *)NULL;

    if(shmid != -1){
      mpstat_arr = (struct eqnchstatus *) shmat( shmid,(char *) 0, SHM_RND);
      
      if((unsigned long)mpstat_arr == -1) 
      {
		  nodelay(stdscr, FALSE);
	      sprintf(tbuff,"ERORR shmat(): mpstat_arr %lX: errno:%d",
                      (unsigned long) mpstat_arr, errno);
	      error_message(tbuff); get_char(); clear_message();
          return(ERR);
       }
    }

    if(shmid2 != -1){
      bufstat_arr = (struct eqnbufstatus *) shmat( shmid2,(char *) 0, SHM_RND);
      if((unsigned long)bufstat_arr == -1) 
      {
		  nodelay(stdscr, FALSE);
	      sprintf(tbuff,"ERROR shmat():bufstat_arr %lX: errno:%d",
                    (unsigned long) bufstat_arr, errno);
	      error_message(tbuff); get_char(); clear_message();
          return(ERR);
      }
    }


    quit_now = (int *)(bufstat_arr + 33); /* used as a semophore */
    *quit_now = 0;                          /* informs child procs to eqnx_exit */

    while(!done)
    {
		nodelay(stdscr, TRUE);
		gen_dev_name(dname, phys_to_log(board), (lmx * 16));
		if(stat(dname, &sbuf) == -1 ){
			nodelay(stdscr, FALSE);
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", 
					dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
			error_message(tbuff); get_char(); 
			clear_message();
		}
		brdstat.brd_nbr = board;	
		brdstat.ioctl_version = IOCTL_VERSION;

		mpstat.ec_nbr = sbuf.st_rdev;
		mpstat.ioctl_version = IOCTL_VERSION;
   
    		if((pfd = open( ddevice, O_RDWR )) <= 0){
				sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
				error_message(tbuff); get_char(); clear_message();
				done=ESC;
				continue;
				
			}

			if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
				sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
				sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			/* macros to calc absolute chars xmitted and rcved */ 
#define NCURRCV (brdstat.brd_cst[(lmx * 16) + aa].cst_in \
                        + (bufstat_arr +  aa)->ebf_incount)
#define NCURXMT (brdstat.brd_cst[(lmx * 16) + aa].cst_out \
                        - (bufstat_arr +  aa)->ebf_outcount)
            width = 80;
            row = 8;
            height = 14;
            if(id == BPKFOUND) /* Backpack */
            {
               row = 1;
               height = 23;
            }

            if(pcount == 8)
            {
               width = 44; 
            }

            cmod = 'a';    /* compute the module */
            cmod = cmod + lmx;
 
            sprintf(tbuff, " Ports /dev/ttyQ%d%c1-%d ",
                   phys_to_log(Board_Selected) , cmod, pcount);

	    	regwn = establish_window( 14, row , height, width, 0,1, tbuff);
            
            row = 11;
            if(id == BPKFOUND)
            {
               row = 20;
            }
            wprompt(regwn, 3  , row, "ON");
            reverse_video(regwn);
            wprompt(regwn, 7  , row, "[  ]");
            reverse_video(regwn);
            wprompt(regwn, 16  , row, "OFF");
            wprompt(regwn, 21  , row, "[  ]");
            if(ramp_found)
            {
               wprompt(regwn, 35  , row - 1, "Power    ON   [  ][  ][  ][  ]");
               wprompt(regwn, 35  , row,     "Settings OFF  [  ][  ][  ][  ]");
            }
		 
		 
			bufstat.ebf_nbr = sbuf.st_rdev;
			bufstat.ioctl_version = IOCTL_VERSION;

			if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
				sprintf(tbuff,"ERROR EQNCHBUFSTATUS:%d errno:%d",retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				delete_window(regwn);
				done=ESC; 
				break;
			}

            if(id != BPKFOUND)
		        for(ii = 0; ii < pcount; ii++)
                {
                   sprintf(tbuff, "%d", ii + 1);
                   if(pcount == 8)
                      wprompt(regwn, 3 + (ii * 5)  , 0, tbuff);
                   else
                      wprompt(regwn, 1 + (ii * 5)  , 0, tbuff);
     
                }
             else
                {
		            for(aa = 0,ii = 0; ii < (pcount * 2); ii += 2 , aa++)
                    {
                      /* top */
                      sprintf(tbuff, "%d", ii + 1);
                      wprompt(regwn, 1 + (aa * 5)  , 0, tbuff);
                      /* bottom */
                      sprintf(tbuff, "%d", (ii + 2));
                      wprompt(regwn, 1 + (aa * 5)  , 10, tbuff); 
                    }
                }

            if(id != BPKFOUND)  
		       for(ii = 0; ii < pcount; ii++)
               {
                 last_in[ii] = last_out[ii] = 0xFF;
                 last_rcv[ii] = last_xmt[ii] = 0;
                  present[ii] = true;
                  /* SWIN *establish_window( x, y, h, w, t, rev, title) */
                  sprintf(tbuff, "%1.1d", ii);
                 if(pcount == 8)
	    	      pwin[ii] = establish_window( 16 + (ii * 5), 10, 8, 4, 0,1,"");
                 else
	    	       pwin[ii] = establish_window( 1 + (ii * 5), 10, 8, 4, 0,1,"");
                }
              else
                {
		          for(aa = 0, ii = 0; ii < (pcount * 2 ); ii += 2, aa++)
                  {
                    last_in[ii] = last_out[ii] = 0xFF;
                    last_in[ii + 1] = last_out[ii + 1] = 0xFF;
                    last_rcv[ii] = last_xmt[ii] = 0;
                    last_rcv[ii + 1] = last_xmt[ii + 1] = 0;
                    present[ii] = true;
                    present[ii + 1] = true;
	    	          pwin[ii] 
                        = establish_window( 1 + (aa * 5), 3, 8, 4, 0,1,"");
	    	          pwin[ii + 1] = 
                        establish_window( 1 + (aa * 5), 13, 8, 4, 0,1,"");

                  }

                }
           ports = pcount;

           for(aa = 0; aa < ports; aa++)
           {
              switch(child_scan_id[aa] = fork()) {
                case -1:
		          nodelay(stdscr, FALSE);
			      sprintf(tbuff,"Error creating read proc.");
			      error_message(tbuff); get_char(); clear_message();
                  break;
                case 0:
		          gen_dev_name(dname, phys_to_log(board), (lmx * 16) + aa);
		          if(stat(dname, &sbuf) == -1 ){
			         if(errno == 2){
				         sprintf(tbuff, "File %s does not exist", 
				          dname);
			          } else {
				          sprintf(tbuff, "ERROR: stat %s, errno %d", 
					           dname, errno);
			          }
			          error_message(tbuff); get_char(); 
			          clear_message();
			          continue;
		         }
		         (mpstat_arr + aa)->ec_nbr = sbuf.st_rdev;
		         (mpstat_arr + aa)->ioctl_version = IOCTL_VERSION;
			 (bufstat_arr + aa)->ebf_nbr = sbuf.st_rdev;
		         (bufstat_arr + aa)->ioctl_version = IOCTL_VERSION;

                 if(id == BPKFOUND) 
                 {
		             gen_dev_name(dname, phys_to_log(board),
                           ((lmx + 1) * 16) + aa);
		             if(stat(dname, &sbuf) == -1 ){
			            if(errno == 2){
				         sprintf(tbuff, "File %s does not exist", 
				          dname);
			          } else {
				          sprintf(tbuff, "ERROR: stat %s, errno %d", 
					           dname, errno);
			          }
		              nodelay(stdscr, FALSE);
			          error_message(tbuff); get_char(); 
			          clear_message();
			          continue;
                    }
                   /* set pointer to upper 16 channels */
		           (mpstat_arr + aa + 16)->ec_nbr = sbuf.st_rdev;
		           (mpstat_arr + aa + 16)->ioctl_version = IOCTL_VERSION;
			   (bufstat_arr + aa + 16)->ebf_nbr = sbuf.st_rdev;
		           (bufstat_arr + aa + 16)->ioctl_version = IOCTL_VERSION;
                }

               while(true)
               {
                 while(!*quit_now)
                 {
				     if((retval = ioctl( pfd,
                             EQNCHSTATUS,(char *)(mpstat_arr + aa)))){
					     sprintf(tbuff,
                            "ERROR EQNCHSTATUS:%d errno:%d",retval,errno);
					     error_message(tbuff); get_char(); 
					     clear_message();
					     delete_window(regwn);
					     done=ESC; 
					     break;
				     }
                 if(id == BPKFOUND)
                 {
				     if((retval = ioctl( pfd,
                             EQNCHSTATUS,(char *)(mpstat_arr + aa + 16)))){
		                 nodelay(stdscr, FALSE);
					     sprintf(tbuff,
                            "ERROR EQNCHSTATUS:%d errno:%d aa %d",retval,
                                   errno, aa);
					     error_message(tbuff); get_char();clear_message();
					     delete_window(regwn); done=ESC; 
					     break;
				     }
                  }

			         if((retval = ioctl( pfd, EQNCHBUFSTAT,
                     (char *)(bufstat_arr + aa)))){
				         sprintf(tbuff,
                            "ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
				         error_message(tbuff); get_char(); 
				         clear_message();
				         delete_window(regwn);
				         done=ESC; 
				         break;
	                  }
                  if(id == BPKFOUND)
                  {
			         if((retval = ioctl( pfd, EQNCHBUFSTAT,
                     (char *)(bufstat_arr + aa + 16)))){
		                 nodelay(stdscr, FALSE);
				         sprintf(tbuff,
                            "ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
				         error_message(tbuff); get_char(); clear_message();
				         delete_window(regwn); done=ESC; 
				         break;
	                  }
                   }
                     if(id == BPKFOUND)
                       napms(30);  
                     else 
                       napms(80);  
                   }
			exit(0); /* Shashi */
                  }
                default:
                   break;
               }
            }

			while(!done){
				if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
					sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d",
						retval, errno);
					error_message(tbuff); get_char(); 
					clear_message();
					delete_window(regwn);
					done=ESC; 
					break;
				}

/* brdstat has alive status */

           ports = pcount;

           if( id == BPKFOUND )
              ports = 32;
           /* start for loop */
           for(aa = 0; aa < ports; aa++)
           {
            if(aa == 0)
            {
               if(brdstat.lmx[lmx/4][0].lmx_active 
                  != DEV_GOOD)
               {
		           for(aa = 0; aa < ports; aa++)
		             delete_window(pwin[aa]);
				   delete_window(regwn);
                   err_ret = 1;
                   done = 1;  /* this will force a break from the while */
                   break;    /* break out of for loop */
               }
            }


		    if( ramp_found &&
               !((mpstat_arr + aa)->ec_mpa_stat & MPA_UART_CFG_DONE))
            {
               if(present[aa] == true) 
               { 
                  wprompt(pwin[aa], 0 , 0, "E ");
                  wprompt(pwin[aa], 0 , 1, "M ");
                  wprompt(pwin[aa], 0 , 2, "P ");
                  wprompt(pwin[aa], 0 , 3, "T ");
                  wprompt(pwin[aa], 0 , 4, "Y ");
                  wprompt(pwin[aa], 0 , 5, "  ");
                  present[aa] = false;
                  last_rcv[aa] = last_xmt[aa] = 0;
                  last_in[aa] = last_out[aa] = 0xFF;
               }
            }
            else
            {
                present[aa] = true;
		        /* are we transmitting or recieving */
                if(!(NCURRCV == oldrxcnt[aa]))
                {
                    if(last_rcv[aa] != 1)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 0, "RD");
                        reverse_video(pwin[aa]);
                    }
                    last_rcv[aa] = 1;
                }
                else
                {
                    if(last_rcv[aa] != 2)
                       wprompt(pwin[aa], 0 , 0, "RD");
                    last_rcv[aa] = 2;
                }
                if(!(NCURXMT == oldtxcnt[aa]))
                {
                    if(last_xmt[aa] != 1)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 1, "TD");
                        reverse_video(pwin[aa]);
                    }
                    last_xmt[aa] = 1;
                }
                else
                {
                    if(last_xmt[aa] != 2)
                        wprompt(pwin[aa], 0 , 1, "TD");
                    last_xmt[aa] = 2;
                }

                oldrxcnt[aa] = NCURRCV;
                oldtxcnt[aa] = NCURXMT;

			    /* ---------- get out bound control sigs ----------*/
			    /* in all cases control sigs are written to buffers.
			       these buffers may not be associated with a field */
                 
			    vv = (bufstat_arr + aa)->ebf_insigs; 
                if(last_in[aa] != vv ||       /* did they change */
                        last_in[aa] == 0xFF)  /* or first pass */
                {
                    if( vv & 0x01)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 2, "CD");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 2, "CD");

                    if( vv & 0x02)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 3, "CS");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 3, "CS");
                    last_in[aa] = vv;
                }
			    vv = (bufstat_arr + aa)->ebf_outsigs; 
                if(last_out[aa] != vv ||       /* did they change */
                        last_out[aa] == 0xFF)  /* or first pass */
                {
                    if( vv & 0x01)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 4, "TR");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 4, "TR");
    
                    if( vv & 0x02)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 5, "RS");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 5, "RS");
                    last_out[aa] = vv;
                 }
            }

            /* check switch status */
            if(ramp_found
              && ((aa % 4) == 0))
            {
                /* is it powered off */
                if((mpstat_arr + aa)->ec_power_state == 0x00)  
                {     /* clear ON indicator */
                    wprompt(regwn, 49 + ((aa / 4) * 4) ,
                      row - 1, "[  ]");
 
                    /* set OFF indicator */
                    reverse_video(regwn);
                    wprompt(regwn, 49 + ((aa / 4) * 4) ,
                      row, "[  ]");
                    reverse_video(regwn);
                }
                else
                if((mpstat_arr + aa)->ec_power_state == 0xFF)   
                {         
                    reverse_video(regwn);   /* set the ON indicator */
                    wprompt(regwn, 49 + ((aa / 4) * 4) ,
                      row - 1, "[  ]");
                    reverse_video(regwn);
                                           /* clear the OFF indicator */
                    wprompt(regwn, 49 + ((aa / 4) * 4) ,
                      row, "[  ]");
                }

            }
	     }	/* end for loop */
			napms(80);
			if((junk = wgetch(stdscr)) == ERR)
				continue;
			else {
				flushinp();
		        for(aa = 0; aa < ports; aa++)
		            delete_window(pwin[aa]);
				delete_window(regwn);
				done=1; 
				break;
			}
	    }
    }

    *quit_now = 1; /* inform child procs to eqnx_exit */

    for(aa = 0; aa < pcount; aa++)
    {
       tchd = wait( &stat_loc );
    }

    shmdt((char *)mpstat_arr);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);
    shmdt((char *)bufstat_arr);
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)NULL);

	nodelay(stdscr, FALSE);
    close(pfd);
	clear_ttyname();
	upd=oldupd;

    return(err_ret);

}

/*--------------------------------------------------------------------------*/
int navigate_ports( board, pcount)
int board;
int pcount;
{
/* ------------------------------------------ */
    short int present[16];
	int  done, oldupd;
	int ii, aa, pfd=0, retval;
	unsigned int junk;
	struct stat sbuf;
    key_t key = 0x0;
    key_t key2 = 0x0;
    int shmid;
    int shmid2;
    unsigned int oldrxcnt[16];
    unsigned int oldtxcnt[16];
    SWIN *pwin[16];
    short int child_scan_id[16];
	struct eqnchstatus mpstat;
	struct eqnchstatus *mpstat_arr;
	struct eqnbrdstatus brdstat;
	struct eqnbufstatus bufstat;
	struct eqnbufstatus *bufstat_arr;
    int *quit_now, stat_loc, tchd;
	unsigned char vv;
    unsigned char last_in[16];
    unsigned char last_out[16];
    unsigned char last_rcv[16];
    unsigned char last_xmt[16];
	char tbuff[60];
	char dname[35];	
    int ccol ,width;
	oldupd = upd;
	upd = 1;
	done=0;


    shmid = 0;			/* get a shared memory segment */
    shmid = shmget( key, 0xF000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
    shmid2 = 0;			/* get a shared memory segment */
    shmid2 = shmget( key2, 0xF000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
	/* set up varables to share memory */
    mpstat_arr = (struct eqnchstatus *)NULL;
    bufstat_arr = (struct eqnbufstatus *)NULL;

    if(shmid != -1){
      mpstat_arr = (struct eqnchstatus *) shmat( shmid,(char *) 0, SHM_RND);
      
      if((unsigned long)mpstat_arr == -1) 
      {
		  nodelay(stdscr, FALSE);
	      sprintf(tbuff,"ERORR shmat(): mpstat_arr %lX: errno:%d",
                      (unsigned long) mpstat_arr, errno);
	      error_message(tbuff); get_char(); clear_message();
          return(ERR);
       }
    }

    if(shmid2 != -1){
      bufstat_arr = (struct eqnbufstatus *) shmat( shmid2,(char *) 0, SHM_RND);
      if((unsigned long)bufstat_arr == -1) 
      {
		  nodelay(stdscr, FALSE);
	      sprintf(tbuff,"ERROR shmat():bufstat_arr %lX: errno:%d",
                    (unsigned long) bufstat_arr, errno);
	      error_message(tbuff); get_char(); clear_message();
          return(ERR);
      }
    }


    quit_now = (int *)(bufstat_arr + 25); /* used as a semophore */
    *quit_now = 0;                          /* informs child procs to eqnx_exit */

    while(!done)
    {
		nodelay(stdscr, TRUE);
		gen_dev_name(dname, phys_to_log(board), 0);
		if(stat(dname, &sbuf) == -1 ){
			nodelay(stdscr, FALSE);
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", 
					dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
		    nodelay(stdscr, FALSE);
			error_message(tbuff); get_char(); 
			clear_message();
		}
		brdstat.brd_nbr = board;	
		brdstat.ioctl_version = IOCTL_VERSION;

		mpstat.ec_nbr = sbuf.st_rdev;
		mpstat.ioctl_version = IOCTL_VERSION;
   
    		if((pfd = open( ddevice, O_RDWR )) <= 0){
				sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
		    		nodelay(stdscr, FALSE);
				error_message(tbuff); get_char(); clear_message();
				done=ESC;
				continue;
				
			}

			if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
				sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
				sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d", retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			/* macros to calc absolute chars xmitted and rcved */ 
#define N_CURRCV (brdstat.brd_cst[aa].cst_in \
                        + (bufstat_arr + aa)->ebf_incount)
#define N_CURXMT (brdstat.brd_cst[aa].cst_out \
                        - (bufstat_arr +  aa)->ebf_outcount)

            width = 80;
            ccol = 8;
            if(pcount == 8)
            {
               width = 44; 
            }
		 
			bufstat.ebf_nbr = sbuf.st_rdev;
			bufstat.ioctl_version = IOCTL_VERSION;
			if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
				sprintf(tbuff,"ERROR EQNCHBUFSTATUS:%d errno:%d",retval, errno);
				error_message(tbuff); get_char(); 
				clear_message();
				done=ESC; 
				break;
			}

		    for(ii = 0; ii < pcount; ii++)
            {
               last_in[ii] = last_out[ii] = 0xFF;
               last_rcv[ii] = last_xmt[ii] = 0;
               present[ii] = true;
               /* SWIN *establish_window( x, y, h, w, t, rev, title) */
	    	   pwin[ii] = establish_window( 41 + (ii * 5), 8, 8, 4, 0,1,"");
            }

           for(aa = 0; aa < pcount; aa++)
           {
              switch(child_scan_id[aa] = fork()) {
                case -1:
		          nodelay(stdscr, FALSE);
			      sprintf(tbuff,"Error creating read proc.");
			      error_message(tbuff); get_char(); clear_message();
                  break;
                case 0:
		          gen_dev_name(dname, phys_to_log(board), aa);
		          if(stat(dname, &sbuf) == -1 ){
			         if(errno == 2){
				         sprintf(tbuff, "File %s does not exist", 
				          dname);
			          } else {
				          sprintf(tbuff, "ERROR: stat %s, errno %d", 
					           dname, errno);
			          }
			          error_message(tbuff); get_char(); 
			          clear_message();
			          continue;
		         }
		         (mpstat_arr + aa)->ec_nbr = sbuf.st_rdev;
		         (mpstat_arr + aa)->ioctl_version = IOCTL_VERSION;
			 (bufstat_arr + aa)->ebf_nbr = sbuf.st_rdev;
		         (bufstat_arr + aa)->ioctl_version = IOCTL_VERSION;
   

               while(true)
               {
                 while(!*quit_now)
                 {
				     if((retval = ioctl( pfd,
                             EQNCHSTATUS,(char *)(mpstat_arr + aa)))){
					     sprintf(tbuff,
                            "ERROR EQNCHSTATUS:%d errno:%d port %d name %s",
                              retval,errno, aa, dname);
		                  nodelay(stdscr, FALSE);
					     error_message(tbuff); get_char(); 
					     clear_message();
					     done=ESC; 
					     break;
				     }

			         if((retval = ioctl( pfd, EQNCHBUFSTAT,
                     (char *)(bufstat_arr + aa)))){
				         sprintf(tbuff,
                            "ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
		                  nodelay(stdscr, FALSE);
				         error_message(tbuff); get_char(); 
				         clear_message();
				         done=ESC; 
				         break;
	                  }
                      napms(140);  
                   }
			exit(0); /* Shashi */
                  }
                default:
                   break;
               }
            }

			while(!done){
				if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
					sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d",
						retval, errno);
		                  nodelay(stdscr, FALSE);
					error_message(tbuff); get_char(); 
					clear_message();
					done=ESC; 
					break;
				}
           /* start for loop */
           for(aa = 0; aa < pcount; aa++)
           {
                present[aa] = true;
		        /* are we transmitting or recieving */
                if(!(N_CURRCV == oldrxcnt[aa]))
                {
                    if(last_rcv[aa] != 1)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 0, "RD");
                        reverse_video(pwin[aa]);
                    }
                    last_rcv[aa] = 1;
                }
                else
                {
                    if(last_rcv[aa] != 2)
                       wprompt(pwin[aa], 0 , 0, "RD");
                    last_rcv[aa] = 2;
                }
                if(!(N_CURXMT == oldtxcnt[aa]))
                {
                    if(last_xmt[aa] != 1)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 1, "TD");
                        reverse_video(pwin[aa]);
                    }
                    last_xmt[aa] = 1;
                }
                else
                {
                    if(last_xmt[aa] != 2)
                        wprompt(pwin[aa], 0 , 1, "TD");
                    last_xmt[aa] = 2;
                }

                oldrxcnt[aa] = N_CURRCV;
                oldtxcnt[aa] = N_CURXMT;

			    /* ---------- get out bound control sigs ----------*/
			    /* in all cases control sigs are written to buffers.
			       these buffers may not be associated with a field */
                 
			    vv = (bufstat_arr + aa)->ebf_insigs; 
                if(last_in[aa] != vv ||       /* did they change */
                        last_in[aa] == 0xFF)  /* or first pass */
                {
                    if( vv & 0x01)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 2, "CD");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 2, "CD");

                    if( vv & 0x02)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 3, "CS");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 3, "CS");
                    last_in[aa] = vv;
                }
			    vv = (bufstat_arr + aa)->ebf_outsigs; 
                if(last_out[aa] != vv ||       /* did they change */
                        last_out[aa] == 0xFF)  /* or first pass */
                {
                    if( vv & 0x01)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 4, "TR");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 4, "TR");
    
                    if( vv & 0x02)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 5, "RS");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 5, "RS");
                    last_out[aa] = vv;
                 }
	     }	/* end for loop */
			napms(140);
			if((junk = wgetch(stdscr)) == ERR)
				continue;
			else {
				flushinp();
		        for(aa = 0; aa < pcount; aa++)
		            delete_window(pwin[aa]);
				done=1; 
				break;
			}
	    }
    }

    *quit_now = 1; /* inform child procs to eqnx_exit */

    for(aa = 0; aa < pcount; aa++)
    {
       tchd = wait( &stat_loc );
    }

    shmdt((char *)mpstat_arr);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);
    shmdt((char *)bufstat_arr);
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)NULL);

	nodelay(stdscr, FALSE);
    close(pfd);
	clear_ttyname();
	upd=oldupd;

    return(ERR);

}
/*--------------------------------------------------------------------------*/
int navigate_mega( board, con_a)
int board;
int con_a;
{
/* ------------------------------------------ */
    short int present[16];
	int  done;
	int ii, aa, start, pfd=0, retval;
	unsigned int junk;
	struct stat sbuf;
    key_t key = 0x0;
    key_t key2 = 0x0;
    int shmid;
    int shmid2;
    unsigned int oldrxcnt[16];
    unsigned int oldtxcnt[16];
    SWIN *pwin[16], *wmain=NULL;
    short int child_scan_id[16];
	struct eqnchstatus mpstat;
	struct eqnchstatus *mpstat_arr;
	struct eqnbrdstatus brdstat;
	struct eqnbufstatus bufstat;
	struct eqnbufstatus *bufstat_arr;
    int *quit_now, stat_loc, tchd;
	unsigned char vv;
    unsigned char last_in[16];
    unsigned char last_out[16];
    unsigned char last_rcv[16];
    unsigned char last_xmt[16];
	char tbuff[60];
	char dname[35];	
	done=0;


    shmid = 0;			/* get a shared memory segment */
    shmid = shmget( key, 0xF000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
    shmid2 = 0;			/* get a shared memory segment */
    shmid2 = shmget( key2, 0xF000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
	/* set up varables to share memory */
    mpstat_arr = (struct eqnchstatus *)NULL;
    bufstat_arr = (struct eqnbufstatus *)NULL;

    if(shmid != -1){
      mpstat_arr = (struct eqnchstatus *) shmat( shmid,(char *) 0, SHM_RND);
      
      if((unsigned long)mpstat_arr == -1) 
      {
		  nodelay(stdscr, FALSE);
	      sprintf(tbuff,"ERORR shmat(): mpstat_arr %lX: errno:%d",
                      (unsigned long) mpstat_arr, errno);
	      error_message(tbuff); get_char(); clear_message();
          return(ERR);
       }
    }

    if(shmid2 != -1){
      bufstat_arr = (struct eqnbufstatus *) shmat( shmid2,(char *) 0, SHM_RND);
      if((unsigned long)bufstat_arr == -1) 
      {
		  nodelay(stdscr, FALSE);
	      sprintf(tbuff,"ERROR shmat():bufstat_arr %lX: errno:%d",
                    (unsigned long) bufstat_arr, errno);
	      error_message(tbuff); get_char(); clear_message();
          return(ERR);
      }
    }


    quit_now = (int *)(bufstat_arr + 25); /* used as a semophore */
    *quit_now = 0;                          /* informs child procs to eqnx_exit */


	nodelay(stdscr, FALSE);

    while(!done)
    {
		nodelay(stdscr, TRUE);
		gen_dev_name(dname, phys_to_log(board), 0);
		if(stat(dname, &sbuf) == -1 ){
			nodelay(stdscr, FALSE);
			if(errno == 2){
				sprintf(tbuff, "File %s does not exist", 
					dname);
			} else {
				sprintf(tbuff, "ERROR: stat %s, errno %d", 
					dname, errno);
			}
			error_message(tbuff); get_char(); 
			clear_message();
		}
		brdstat.brd_nbr = board;	
		brdstat.ioctl_version = IOCTL_VERSION;

		mpstat.ec_nbr = sbuf.st_rdev;
		mpstat.ioctl_version = IOCTL_VERSION;
   
    		if((pfd = open( ddevice, O_RDWR )) <= 0){
				sprintf(tbuff,"ERROR OPEN %s: errno:%d", ddevice, errno);
	            nodelay(stdscr, FALSE);
				error_message(tbuff); get_char(); clear_message();
				done=ESC;
				continue;
				
			}

			if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
				sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d", retval, errno);
	            nodelay(stdscr, FALSE);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
				sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d", retval, errno);
	            nodelay(stdscr, FALSE);
				error_message(tbuff); get_char(); 
				clear_message();
				clear_ttyname();
				return(0);
			}
			/* macros to calc absolute chars xmitted and rcved */ 
#define N_CURRCV (brdstat.brd_cst[aa].cst_in \
                        + (bufstat_arr + aa)->ebf_incount)
#define N_CURXMT (brdstat.brd_cst[aa].cst_out \
                        - (bufstat_arr +  aa)->ebf_outcount)

			bufstat.ebf_nbr = sbuf.st_rdev;
			bufstat.ioctl_version = IOCTL_VERSION;
			if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
				sprintf(tbuff,"ERROR EQNCHBUFSTATUS:%d errno:%d",retval, errno);
	            nodelay(stdscr, FALSE);
				error_message(tbuff); get_char(); 
				clear_message();
				done=ESC; 
				break;
			}
            if(con_a == true)
                start = 0;
            else
                start = 16;

            sprintf(tbuff, " Ports /dev/ttyQ%d%c1-12 ", Board_Selected,
                       (con_a == true) ? 'a' : 'b');

	        wmain = establish_window( 6, 12, 11, 63, 0,1, tbuff);
		    for(ii = 0; ii < 12; ii++)
            {
               sprintf(tbuff, "%d", ii + 1);
                  wprompt(wmain, 1 + (ii * 5)  , 0, tbuff);
            }
            wprompt(wmain, 5  , 8, "ON");
            reverse_video(wmain);

            wprompt(wmain, 8  , 8, "[  ]");
            reverse_video(wmain);

            wprompt(wmain, 16  , 8, "OFF");
            wprompt(wmain, 21  , 8, "[  ]");

		    for(ii = 0; ii < 12; ii++)
            {
               last_in[ii] = last_out[ii] = 0xFF;
               last_rcv[ii] = last_xmt[ii] = 0;
               present[ii] = true;
	    	   pwin[ii] = establish_window((7 + (ii * 5)), 14, 6, 4, 0,1,"");
            }

           for(aa = 0; aa < 12; aa++)
           {
              switch(child_scan_id[aa] = fork()) {
                case -1:
		          nodelay(stdscr, FALSE);
			      sprintf(tbuff,"Error creating read proc.");
			      error_message(tbuff); get_char(); clear_message();
                  break;
                case 0:
		          gen_dev_name(dname, phys_to_log(board), aa + start);
		          if(stat(dname, &sbuf) == -1 ){
			         if(errno == 2){
				         sprintf(tbuff, "File %s does not exist", 
				          dname);
			          } else {
				          sprintf(tbuff, "ERROR: stat %s, errno %d", 
					           dname, errno);
			          }
			          error_message(tbuff); get_char(); 
			          clear_message();
			          continue;
		         }
		         (mpstat_arr + aa)->ec_nbr = sbuf.st_rdev;
		         (mpstat_arr + aa)->ioctl_version = IOCTL_VERSION;
			 (bufstat_arr + aa)->ebf_nbr = sbuf.st_rdev;
		         (bufstat_arr + aa)->ioctl_version = IOCTL_VERSION;
   
               while(true)
               {
                 while(!*quit_now)
                 {
				     if((retval = ioctl( pfd,
                             EQNCHSTATUS,(char *)(mpstat_arr + aa)))){
					     sprintf(tbuff,
                            "ERROR EQNCHSTATUS:%d errno:%d",retval,errno);
					     error_message(tbuff); get_char(); 
					     clear_message();
					     done=ESC; 
					     break;
				     }

			         if((retval = ioctl( pfd, EQNCHBUFSTAT,
                     (char *)(bufstat_arr + aa)))){
				         sprintf(tbuff,
                            "ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
				         error_message(tbuff); get_char(); 
				         clear_message();
				         done=ESC; 
				         break;
	                  }
                      napms(140);  
                   }
			exit(0); /* Shashi */
                  }
                default:
                   break;
               }
            }

			while(!done){
				if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
					sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d",
						retval, errno);
					error_message(tbuff); get_char(); 
					clear_message();
					done=ESC; 
					break;
				}
           /* start for loop */
           for(aa = 0; aa < 12; aa++)
           {
                present[aa] = true;
		        /* are we transmitting or recieving */
                if(!(N_CURRCV == oldrxcnt[aa]))
                {
                    if(last_rcv[aa] != 1)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 0, "RD");
                        reverse_video(pwin[aa]);
                    }
                    last_rcv[aa] = 1;
                }
                else
                {
                    if(last_rcv[aa] != 2)
                       wprompt(pwin[aa], 0 , 0, "RD");
                    last_rcv[aa] = 2;
                }
                if(!(N_CURXMT == oldtxcnt[aa]))
                {
                    if(last_xmt[aa] != 1)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 1, "TD");
                        reverse_video(pwin[aa]);
                    }
                    last_xmt[aa] = 1;
                }
                else
                {
                    if(last_xmt[aa] != 2)
                        wprompt(pwin[aa], 0 , 1, "TD");
                    last_xmt[aa] = 2;
                }

                oldrxcnt[aa] = N_CURRCV;
                oldtxcnt[aa] = N_CURXMT;

			    /* ---------- get out bound control sigs ----------*/
			    /* in all cases control sigs are written to buffers.
			       these buffers may not be associated with a field */
                 
			    vv = (bufstat_arr + aa)->ebf_insigs; 
                if(last_in[aa] != vv ||       /* did they change */
                        last_in[aa] == 0xFF)  /* or first pass */
                {
                    if( vv & 0x01)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 2, "CD");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 2, "CD");
                    last_in[aa] = vv;
                }
			    vv = (bufstat_arr + aa)->ebf_outsigs; 
                if(last_out[aa] != vv ||       /* did they change */
                        last_out[aa] == 0xFF)  /* or first pass */
                {
                    if( vv & 0x01)
                    {
                        reverse_video(pwin[aa]);
                        wprompt(pwin[aa], 0 , 3, "TR");
                        reverse_video(pwin[aa]);
                    }
                    else
                       wprompt(pwin[aa], 0, 3, "TR");
                    last_out[aa] = vv;
                 }
	     }	/* end for loop */
			napms(140);
			if((junk = wgetch(stdscr)) == ERR)
				continue;
			else {
				flushinp();
		        for(aa = 0; aa < 12; aa++)
		            delete_window(pwin[aa]);
				done=1; 
				break;
			}
	    }
    }

    *quit_now = 1; /* inform child procs to eqnx_exit */

    for(aa = 0; aa < 12; aa++)
    {
       tchd = wait( &stat_loc );
    }

    shmdt((char *)mpstat_arr);
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);
    shmdt((char *)bufstat_arr);
    shmctl(shmid2, IPC_RMID, (struct shmid_ds *)NULL);

	nodelay(stdscr, FALSE);
    close(pfd);

    delete_window(wmain);
    return(ERR);

}

/*--------------------------------------------------------------------------*/
/*end -Board status---> Topology tree---------------------------------------*/
int Diagrun_def = 1;
int Loopbt_def = 2;

/* loopback main menu and submenus */
static char **loopbuf, **lpb; 
int Loopb_def = 1;

int
loopback()
{
	SWIN *regwn;
	struct field *fld;
	int ekey, done = 0;	
	char tbuff[50];
	
	int ee;

	loopbuf = lpb = NULL;
	/* malloc string pointers */
	loopbuf = (char **)malloc(11 * sizeof(char *));
	if(loopbuf == NULL){	
		sprintf(tbuff,"ERROR MALLOC: errno:%d", errno);
		error_message(tbuff);	
		get_char(); clear_message();
	}	
	lpb = loopbuf;
	/* malloc character arrays */
	for(ee = 0; ee < 10; ee++){
		*lpb = (char *) malloc(45);
		if(*lpb == NULL){	
			sprintf(tbuff,"ERROR MALLOC: errno:%d", errno);
			error_message(tbuff);	
			get_char(); clear_message();
		}	
		lpb++;

	}
	*lpb = NULL;



	/* alloc and display window */
	regwn = establish_window( 24, 5, 11, 38, 0, 0, " Loopback ");
	field_pass_mode(regwn, 1);

	/* display field names */
	wprompt(regwn, 1, 1, "Baud Rate:");
	wprompt(regwn, 1, 2, "Verify:");
  	wprompt(regwn, 1, 3, "Update:");
	wprompt(regwn, 1, 4, "Errors:");	
	wprompt(regwn, 1, 5, "Loop Type:");
	wprompt(regwn, 1, 6, "Mode:");
	wprompt(regwn, 1, 8, "Press <F3> to start.");
   
	/* create linked list of fields */
	fld = establish_field(regwn, 14, 1, msk25 + 19, loopbuf[0], 'H'); 
	field_window(fld, "LBaud Ra", 30, 7);
	field_help(fld, helper_s_speed);	

	fld = establish_field(regwn, 14, 2, msk25+12, loopbuf[1], 'H'); 
	field_window(fld, "LVerify ", 30, 7);
	field_help(fld, helper_s_verify);

	fld = establish_field(regwn, 14, 3, msk25+20, loopbuf[2], 'H'); 
	field_window(fld, "LUpdate ", 30, 7);
	field_help(fld, helper_s_update);
			
	fld = establish_field(regwn, 14, 4, msk25+6, loopbuf[3], 'H'); 
	field_window(fld, "LErrors ", 30, 7);
	field_help(fld, helper_s_brkerr);
			
	fld = establish_field(regwn, 14, 5, msk25+7, loopbuf[4], 'H'); 
	field_window(fld,"LLoop Ty", 30, 7);
	field_help(fld, helper_s_loop_type);
	
	fld = establish_field(regwn, 14, 6, msk25+14, loopbuf[5], 'H'); 
	field_window(fld, "LMode   ", 30, 7);
	field_help(fld, helper_diag_run);

	
	switch(NPORTS){
		case 24:
		case 16:
		case 12:
			strcpy(loopbuf[0], baudstrs[baud - 1]);
			break;
		case 8:
			strcpy(loopbuf[0], baudstrs8c[baud - 1]);
			break;
		case 4:
			strcpy(loopbuf[0], baudstrs4c[baud - 1]);
			break;
		case 2:
			strcpy(loopbuf[0], baudstrs2c[baud - 1]);
			break;
		default:
			strcpy(loopbuf[0], baudstrs[baud - 1]);
			break;
	}

	strcpy( loopbuf[1], verifystrs[(check_err == 1 ? 0 : 1) ]);
	sprintf( loopbuf[2], "%-5d", upd);
	strcpy(loopbuf[3], brkerrstrs[ (break_err == 0 ? 1 : 0)]);
	strcpy( loopbuf[4], looptstrs[Loopbt_def - 1]);
	strcpy( loopbuf[5], rselcs[Diagrun_def - 1]);
	
	Internal_loop = (Loopbt_def == 1) ? FALSE : TRUE;

	field_tally(regwn);
	while(!done){
		if((ekey = data_entry(regwn)) == ESC) {
			done = true;
			break;	
		}
		if(NPORTS == 1 || NPORTS == 4 || NPORTS == 2 ||
			 NPORTS == 8 || NPORTS == 24 ||
			 NPORTS == 12 || NPORTS == 16){
			if(ekey == KEY_F3){
				run_loop_test();
			}
		} else {
			sprintf(tbuff,"No channels found. ");
			error_message(tbuff);
			get_char();
			clear_message(); 

		}
      
	}	
	delete_window(regwn);

	/* free alloc'ed space */
	lpb = loopbuf;
	while(*lpb)
		free(*lpb++);
	free(loopbuf);
	return(0);

}

static SMENU dia_men[] = {
	{ " Config ", cselcs, cfuncs},
};
int Configt_def = 1;

int
diag_config()
{
	Configt_def = vscroll(" Configure ", dia_men, Configt_def,  40, 4);
	return(0);
}

/* ------------------------------------*/
void helper_diag_run(rchc)
char *rchc;
{

	diag_run();
	strcpy(rchc, rselcs[Diagrun_def - 1]);

}

static SMENU sdiag_run[] = {
	{ " Diag_run ", rselcs, rfuncs},
};

int
diag_run()
{
	struct eqnbrdstatus brdstat;

	Diagrun_def = vscroll(" Run ", sdiag_run, Diagrun_def, 39, 11);

	if(Diagrun_def == 2){
		brdstat.brd_nbr = Board_Selected;
		
		NPORTS = eget_chan_count(&brdstat, Group_Selected);
	}

	return(0);
}



char pfile[50];
char logfile[80];
static int first = 0;

int
diag_file_select()
{
	SWIN *fwn;
	struct field *fld;
	int done, idx = 0;


	if(first++ == 0){
		strcpy(pfile, "ports");
	}

	Filespec = TRUE;
	fwn = establish_window(25, 12, 3, 45, 0, 1, "");
	field_pass_mode(fwn, 1);
	wprompt(fwn, 2, 0, "File name: ");
	init_template(fwn); 
	fld = establish_field(fwn, 13, 0, msk25, pfile, 'a'); 
	field_window(fld,"pname   ", 19, 7);
	field_tally(fwn);
	done = data_entry(fwn);
	for(idx = 0; idx < 45; idx++)
		if(*(pfile + idx) == ' ' || !pfile[idx]){
			*(pfile + idx) = '\0';
		}
	delete_window(fwn);
	return(1);

}


/* ------------- diag_group_select tables  ------------- */

int sboard_select();
static int (*boards_dummy[])()={ sboard_select, sboard_select, sboard_select,
		sboard_select , sboard_select, sboard_select, sboard_select,
		sboard_select , sboard_select};

/* ------------- diag_group_select tree ------------- */
int
diag_group_select()
{
	struct eqnbrdstatus brdstat;
	char **tps, **fl=NULL;	
	int x;
	SMENU bmen;
	
	/* first are any boards selected  */
	if(! fill_board_struct(&brdstat, 1))
		return(0);
	/* process a vertical menu */
	bmen.func = boards_dummy;		/* we resume here */


	/* First get the number of boards found */
	if(brdstat.gbl_neqnbrds > 1) {
		fl = (char **)malloc((brdstat.gbl_neqnbrds + 2) * sizeof(char *));

		if(fl == NULL){
			printf("malloc error\n");
			eqnx_exit(1);
		}
		tps=fl;

		for(x=0; x < brdstat.gbl_neqnbrds; x++){
				*tps = (char *)malloc(30);
				if(*tps == NULL){
					printf("malloc error\n");
					eqnx_exit(1);
				}
				sprintf(*tps,"%s%d", "   Board ",log_to_phys(x + 1));
				tps++;
		}
		*tps = NULL;
		bmen.mselcs = fl;
	}

	if(brdstat.gbl_neqnbrds == 1){
		Vertical_sel = Board_Selected = 1;	
		sboard_select();
	} else {
	Board_Selected = vscroll(" Select Board ", &bmen, Board_Selected , 43, 8);

	}

	if(brdstat.gbl_neqnbrds != 1){
		tps=fl;
		while(*tps)
			free(*tps++);
		free(fl); 
	}
	return(1);

}
int sgroup_select();
static int (*groups_dummy[])()={ sgroup_select, sgroup_select, sgroup_select,
		sgroup_select , sgroup_select, sgroup_select, sgroup_select,
		sgroup_select , sgroup_select};

char *pgroups_24[] = {
	"Ports 1 through 12 ",
	0
};

char *pgroups_12[] = {
	"Ports 1a1-1a12 and 1b1-1b12",
	0
};

char *pgroups_sixteen[] = {
	"Ports 1 through 16 ",
	0
};
char *pgroups_eight[] = {
	"Ports 1 through 8 ",
	0
};
char *pgroups_four[] = {
	"Ports 1 through 4 ",
	0
};
char *pgroups_two[] = {
	"Ports 1 through 2 ",
	0
};
char *pgroups_one[] = {
	"Port 1 ",
	0
};

int
sboard_select()
{
	struct eqnbrdstatus brdstat;
	SMENU bmen;
	struct brdtab_t *brd_def;
	bmen.func = groups_dummy;

	
	/* Does the selected board exist */	
	if(! fill_board_struct(&brdstat, Vertical_sel))
		return(0);

	brd_def = get_board_def(&brdstat);

	Board_Selected = Vertical_sel;	/* Vertical_sel holds board selected */
	bmen.func = groups_dummy;

	Group_Selected = 1;	
	Filespec = 0;
	NPORTS = brd_def->number_of_ports;

	if (brd_def->asic == SSP64) {

		if(brdstat.nicps == 1)
			bmen.mselcs = pgroups1;		/* one ICP */
		else
			bmen.mselcs = pgroups2;		/* two ICP's */

	} 

        if (brd_def->number_of_ports < 64) {
		switch (NPORTS) {
			case  2: bmen.mselcs = pgroups_two;
				 break;
			case  4: bmen.mselcs = pgroups_four;
				 break;
			case  8: bmen.mselcs = pgroups_eight;
				 break;
			case 12: bmen.mselcs = pgroups_12;
				 break;
			case 16: bmen.mselcs = pgroups_sixteen;
				 break;
			case 24: bmen.mselcs = pgroups_24;
				 break;
			default: bmen.mselcs = pgroups_one;
		}
	}

	Group_Selected = vscroll(" Select Group ", &bmen, 
		Group_Selected , 43, 10);

	return(1);	
}

/* ------------------------*/

int
sgroup_select()
{
	struct eqnbrdstatus brdstat;
	/* get group value */
	Group_Selected = Vertical_sel;

	brdstat.brd_nbr = Board_Selected;
	NPORTS = eget_chan_count(&brdstat, Group_Selected);

	Filespec = 0;
	return(1);	

}
/* ----------------------------------------------------*/
int
sendbp()
{
	int bplen,junk, wcnt;
	CSTREAM *wr;
	char err[60];
	char errst[60];
	char dbf[35];
	char ct[35];
	int  flag, pfd;
	SWIN *swn;
	struct field *fld;

    	if((pfd = open( ddevice, O_RDWR )) < 0){
			sprintf(errst,"ERROR OPEN %s: errno:%d", ddevice, errno);
			error_message(errst); get_char(); clear_message();
			return(0);
		}

	bplen = strlen(bpole);
	while(TRUE){
		if(!get_ttyname(dbf))
			break;
		flow_ctrl=IXON;	
		s_speed();


		if((wr = Sopen(dbf, "a")) == NULL){
			sprintf(err,"ERROR:open, %s", dbf);
			error_message(err);
			get_char();
			clear_message(); 
			clear_ttyname();
			flow_ctrl=0L;	
			return(0);
		}
		wcnt=0;	
		swn = establish_window(18, 7, 3, 40, 0, 1, "");
		wprompt(swn, 1, 0, "Characters Transmitted:" );
		fld = establish_field(swn, 27, 0, regmskten, ct, 'Z'); 
		reverse_video(swn);
		nodelay(stdscr, TRUE);
		while(1){
			sprintf(ct, "%10d", wcnt);
			field_tally(swn);
			if(write(wr->fd, bpole, bplen) == -1){
				sprintf(err,"ERROR:write, %s,errno = %d ", dbf,errno);
				nodelay(stdscr, FALSE);
				error_message(err);
				get_char();
				clear_message(); 
				nodelay(stdscr, TRUE);
				break;
			}
			/* kludge:BIG time;
			   on some systems port is open with ONDELAY writes return with 0 */
			wcnt = wcnt + bplen;
			
			if((junk = wgetch(stdscr)) == ERR)
				continue;
			else {
				nodelay(stdscr, FALSE);
				flushinp();
				break;
			}
		}
		Sclose(wr);	
		delete_window(swn);
	}
	flow_ctrl=0L;	
	clear_ttyname();
	if(ismegadev(dbf)) {
		flag = get_exflag(dbf, pfd, MPC_HIGHBAUD);
	}
	close(pfd);
	return(0);
}
void helper_s_speed(speed )
char *speed;
{
	s_speed();	
		switch(NPORTS){
			case 24:
			case 16:
			case 12:
				strcpy(speed, baudstrs[baud - 1]);
				break;
			case 8:
				strcpy(speed, baudstrs8c[baud - 1]);
				break;
			case 4:
				strcpy(speed, baudstrs4c[baud - 1]);
				break;
			case 2:
				strcpy(speed, baudstrs2c[baud - 1]);
				break;
			default:
				strcpy(speed, baudstrs[baud - 1]);
				break;
		}

}

int s_speed()
{
	int ret, def;	
	struct eqnbrdstatus brdstat;
	SMENU smen;
	struct brdtab_t *brd_def;
	smen.func = NULL;
	
	brdstat.brd_nbr = Board_Selected;
	brd_def = get_board_def(&brdstat);

	if(Diagrun_def == 2){
		switch(eget_chan_count(&brdstat, Group_Selected)){
			case	12:
			case	16:
				smen.mselcs = baudstrs;	
				break;
			case	8:
				if (brd_def->asic == SSP4)
				       smen.mselcs = baudstrs4c;
                		else
				       smen.mselcs = baudstrs8c;
				break;
			case	4:
				smen.mselcs = baudstrs4c;	
				break;

			case	2:
				smen.mselcs = baudstrs2c;	
				break;
			default:
				smen.mselcs = baudstrs;	
				break;	
		}
	} else smen.mselcs = baudstrs;


	def = baud;
	ret = vscroll(" Speed ", &smen, def, 39, 6);
	baud=ret;
	return(0);
}

/*--------------------------------------------------------------------*/
void helper_s_verify(state)
char *state;
{
	s_verify();

	strcpy(state, verifystrs[( check_err == TRUE ? 0 : 1)]);

}
int s_verify()
{
	int ret, def;	
	SMENU vmen;
	vmen.mselcs = verifystrs;
	vmen.func = NULL;

	def = (check_err == TRUE ? 1 : 2);
 
	ret = vscroll(" Verify ", &vmen, def, 39, 8);
	switch(ret) {
		case 1:
			check_err = TRUE;
			break;		
		case 2:
			check_err = FALSE;
			break;		
		default:
			break;	
	}	
	return(0);
}
/*--------------------------------------------------------------------*/
void helper_s_brkerr(ber)
char  *ber;
{
	s_brkerr();
	strcpy(ber, brkerrstrs[ (break_err == 0 ? 1 : 0)]);
}
int s_brkerr()
{
	int ret, def;	
	SMENU vmen;
	vmen.mselcs = brkerrstrs;
	vmen.func = NULL;

	def = (break_err == 0 ? 2 : 1);
 
	ret = vscroll(" Errors ", &vmen, def, 39, 10);
	switch(ret) {
		case 1:
			break_err = TRUE;
			break;		
		case 2:
			break_err = FALSE;
			break;		
		default:
			break;	
	}	
	return(0);

}
void helper_s_loop_type(ltype)
char *ltype;
{
	SMENU vmen;
	vmen.mselcs = looptstrs;
	vmen.func = NULL;

	Loopbt_def = vscroll(" Loop type ", &vmen, Loopbt_def, 39, 11);
	Internal_loop = (Loopbt_def == 1) ? FALSE : TRUE;
	strcpy(ltype, looptstrs[Loopbt_def - 1]);
}


/*--------------------------------------------------------------------*/
void helper_s_update(uval)
char *uval;
{
	s_update();
	sprintf(uval,"%-5d", upd);
}

char upmsk[]="_____";

int s_update()
{

	SWIN *swn;
	struct field *fld;
	int res;
	char upbuf[10];

	sprintf(upbuf,"%-5d", upd);
	swn = establish_window(39, 9, 3, 20,0, 0, "");
	field_pass_mode(swn, 1);
	wprompt(swn, 3, 0, "Update:");
	init_template(swn); 
		clear_template(swn); 
		fld = establish_field(swn, 10, 0, upmsk, upbuf, 'N'); 
		field_window(fld,"setupdat", 19, 7);
		field_tally(swn);
		data_entry(swn); 
		res = atoi(upbuf);
	upd = res ? res : 1;
	delete_window(swn);
	return(0);	

}

int single()
{
	char dbf[35];

	if(get_ttyname(dbf))
	{
		NPORTS=1;
		Filespec = 0;
	}	
	clear_ttyname();
	return(1);	
             
}



/* --------------------------------------------------------------------------*/

int warning(st)
char *st;
{
	int len, x;
	SWIN *swn;

	len= strlen(st) + 4;
	beep();
	swn = establish_window(15, 5, 4, len, 0, 1, " WARNING ");
	wcursor(swn, 1, 0);
	Wprintf(swn, st);
	wcursor(swn, 1, 1);
	Wprintf(swn, "Press Enter to continue.");
	x=wgetch(stdscr);
	delete_window(swn);
	if(x == '\r')
		return(1);
	return(0);

}


int Gloopon=0;

int f = 0; 
unsigned char spatt[BUFSIZE+1];
unsigned char *patt = NULL;

char firstdev[]="/dev/ttyQ1a1";

int fastread();
int build_brd_list();

int main( int argc, char *argv[])
{
	unsigned char ch = 'a';
	extern char *optarg;
	int  op, tmp, transient = 0;
 
	strcpy(eqfilename, firstdev);
	strcat(pbase,"/dev/ttys");
	pbase[9]=ch;

	/* parse command */
    while ((op = getopt(argc, argv, "vhu:b:g:s:f:")) != -1)
    {
        switch (op)
        {
        case 's':           /* get baud for loop back test */
            baud = getbaud(optarg);
            baud++;

            break;
		case 'b':
			tmp = atoi(optarg);
			if(tmp < 1 || tmp > 8){
				printf("Invalid board; 1 - 8 \n");
				eqnx_exit(0);
			}
			transient = TRUE;
			Board_Selected = atoi(optarg);
			break;
		case 'g':
			tmp = atoi(optarg);
			if(tmp < 1 || tmp > 8){
				printf("Invalid group; 1 - 8 \n");
				eqnx_exit(0);
			}
			transient = TRUE;
			Group_Selected = atoi(optarg);
			break;
        case 'u':
            upd=atoi(optarg); /* set update for loopback test */
            break ;
        case 'v':           /* disable error checking */
            check_err=FALSE;
            break;
		case 'f':
			Filespec = transient = TRUE;
			strcpy(pfile, optarg);
            break;
		case 'h':
        default:
		printf("usage %s -[v][-b board -g num][s baud][u update][f filename]\n", argv[0]);
		printf("-b board -g num   run the loopback test in transient mode\n");
		printf("                  skips all menu processing\n");
		printf("-f filename       run the loopback in transient mode\n");
		printf("                  filename specifies a device file list\n");
		printf("                  skips all menu processing\n");
		printf("-h                print this help screen\n");
		printf("-s baud           set default speed \n");
		printf("-u num            set update frequency \n");
		printf("-v                disable data verification \n");
		exit(0);
        }
    }

/*---------------------------------------------------------*/
	
/*	init curses */	
		initscr();cbreak();
		noecho();nonl();
		intrflush(stdscr, FALSE); 
		keypad(stdscr, TRUE);
		if(has_colors()==TRUE) {
			if(start_color() == OK ) {
			init_pair(1 , COLOR_BLACK, COLOR_CYAN);
			init_pair(2 , COLOR_BLUE, COLOR_RED);
			init_pair(3 , COLOR_WHITE, COLOR_BLACK);
			SHADATT=COLOR_PAIR(3);
#ifdef	NOTUSED
			GLATT=COLOR_PAIR(1);
#endif
			attron(COLOR_PAIR(1));
			}
		}

	if(build_brd_list()){
		erase();	/* no database found */
		move(23,0);	/* get out */
		refresh();
		endwin();
		nl();echo();
		eqnx_exit(1);
	}

/* build the test pattern */
    tmp = 1;
	f=0;
	while(true){
        /* strip out FLOW Control characters */
        if(tmp == 0x11){
           tmp++;
           continue; 
        }

        if(tmp == 0x13){
           tmp++;
           continue; 
        }
		spatt[f++] = tmp++;
        if(tmp == 256){ 
		   if(f > 500) /* write size = 506 */
		   /*if(f > 3840)*/ /* write size = 3840 */
           	   {
             	   	spatt[f++] = 0;
             	   	break;
           	   }
           	   else 
			   tmp=1;
        }
	}
	spatt[f]='\0';


	patt=(unsigned char *)malloc((BUFSIZE) + 2);

	strcpy((char *)patt,(char *)spatt); 

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	if(!transient){	
		top_banner();
		load_help("/usr/lib/sst/ss.hlp");
		executive(tmn);
	} else {
		run_loop_test();
	}
	erase();
	move(23,0);
	refresh();
	endwin();
	resetty();
	nl();echo();
	eqnx_exit(0);
}

#define SSDBASE "/etc/eqnx/database"
int build_brd_list()
{
	struct eqnbrdstatus brdstat;
	char tbuff[80];
	char datain[80];
	int cnt, log, phys;
	FILE *datab=NULL;

	if((datab=fopen(SSDBASE, "r")) == NULL){
		sprintf(tbuff,"error open file %s \n", SSDBASE);
		error_message(tbuff); get_char(); 
		clear_message();
		return(1);
	}

	brdstat.gbl_neqnbrds = 0;
	if(fill_board_struct(&brdstat, 1))
		if(brdstat.gbl_neqnbrds < 1) {
			sprintf(tbuff,"No boards found.");
			error_message(tbuff); get_char(); clear_message();
			return(1);
		}
	cnt = 0;
	while((fgets(datain, 80, datab) != NULL) && (cnt < SSMAXBRD)){
		sscanf(datain ,"%d%d", &phys, &log);
		phys_log[cnt++] = log;	/* add to table */
	}
	fclose(datab);

	return(0);
}

/* ------------------------------------------------------*/
/* fastread 
   read and alternate verify the data
*/
/* ------------------------------------------------------*/
SWIN *shwin, *flack;
char shw_buff[80];
char z_total[50];
int *derror;
unsigned char *data_err_buf;
unsigned long *sh_ptr;

int fastread( register CSTREAM *z)
{
	register unsigned char st;
	unsigned char *r;
	int xx;
	*derror=0;
	switch (z->total = read(z->fd, z->buf, sizeof(z->buf)))	{

		case -1:
			printf("read error with %d for port %d\n",errno, z->pnum);
			return(FALSE);
		case 0:
			return(FALSE);
		default:
			z->rc += z->total; 		/* - inc read count - */
			if(check_err == FALSE)
				return(z->total > 2 ? TRUE : FALSE);
			z->next=0;
			break;
	}
	st=z->patt;			/* -- set pattern -- */
	for(;;) {			 /* while data checks good do nothing */
		while(st && (z->next < z->total) && st++ == z->buf[z->next++]) {
		}
        	if(st == 0x12 || st == 0x14) { /*skip to XON+1  XOFF+1 */
		   	st++;
		   continue; 
        	} 
		if(z->next >= z->total) {     /* if out of data save test str */
			z->patt=(st);	      /* and return */
			return(z->total > 2 ? TRUE : FALSE);
		}
		if(st != 0) {		             /* if test string */
			if(z->next < z->total) {     /* if data still in buf */
				if(break_err == TRUE){
					*derror=1;
                    			sleep(3);

		  			for(xx=0; xx < NPORTS ; xx++) {
						/* sh_ptr points to shared mem*/
			          		sprintf(txbuf[xx],fmat,	
                         			sh_ptr[xx], sh_ptr[xx + 100],
                               			sh_ptr[xx + 200], 
						sh_ptr[xx + 300]);
	 				}
					/* refresh counts window */
					field_tally(flack);  

					sprintf(z_total,"Bytes read: %4.4x Offset: %4.4x st: %4.4x",
					z->total, z->next - 1, st);
					shwin = establish_window( 16, 3, 18, 53, 0, 0, z_total);
					for( xx = 0; xx < 16; xx++){
						r = (unsigned char *) z->buf;	
						r = r +  (xx * 16);
						sprintf(shw_buff,
						"%2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx %2.2hx",
					*r, *(r+1), *(r+2), *(r+3), *(r+4), *(r+5), *(r+6),*(r+7),
				*(r+8), *(r+9), *(r+10), *(r+11), *(r+12), *(r+13), *(r+14),
				*(r+15));
						wcursor(shwin, 1, xx );
						Wprintf(shwin, shw_buff);
					}
					nodelay(stdscr, FALSE);
					get_char();
					delete_window(shwin);
					nodelay(stdscr, TRUE);
					return(FALSE);	
				}

			/* --------------------------*/ 
				z->err++;	     /* record error */
				st = z->buf[z->next];  /* reset test char */
			 	continue;
			} 
	   	} else  {		    /* if test string depleted set to */
			st=1;	     /* and continue */
			continue;
		}
	}
}


int run_loop_test(void)
{

	register int k;
	unsigned int oldcnt[NP];
	unsigned int tcnt[NP];
	unsigned int last_time, new_time;
	unsigned long oldnports=0;
	int wy, wh, wtype;
	int ink =0,kchk, pfd, done;
	int str_len;
	time_t oldt, newt;
	int thigh, readcnt=0;
	SWIN *mscrn, *note;
	int wk, retval = 0;
	unsigned long *xm_ptr=NULL;
/*FORK VERSION*/
    short int childid[24], childid_r;
/*FORK VERSION*/
    key_t key = 0x0;
    int shmid;
    unsigned long *rx_ptr=NULL , *err_ptr=NULL, *rate_ptr=NULL;
	int *stop_write=NULL;
    int napmstime;
	unsigned speed;

	/* save these: later we may want to get lbolt from the driver */
	/* this produces more accurate calculated rates that the time() call*/
	struct field *fld;
	struct timeb timer;
	FILE *prtn = NULL;
	char namebuf[80];
	char errst[50];


	pbase[11]='\0';
	if(Internal_loop == FALSE)
		if(!warning("This Utility writes raw data to selected ports!"))
			return(0);
	kchk=min(baud, 1);
	if(baud > B4800)
		kchk=15;
	flow_ctrl=IXON | IXOFF;	  /*Shashi 01/19/97 */

    	if((pfd = open( ddevice, O_RDWR )) < 0){
			sprintf(errst,"ERROR OPEN %s: errno:%d", ddevice, errno);
			error_message(errst); get_char(); clear_message();
			return(0);
		}
	if(NPORTS != 1 || Filespec)
		display_notice("Setting up test. Please wait."); 
	/* get devices from a file */	
	if(Filespec){
		if((prtn=fopen(pfile, "r")) == NULL){
			clear_notice(); 
			sprintf(errst,"ERROR:fopen, %s ", pfile);
			error_message(errst);
			get_char();
			clear_message(); 
			return(FALSE);
		}
		k=0;
		oldnports = NPORTS;
		NPORTS = 0;
		while(fgets(namebuf, 80, prtn) != NULL){
			if(namebuf[strlen(namebuf) - 1]!= '\0')
				namebuf[strlen(namebuf) - 1 ] = '\0';
			
				thigh = 0;	

				if((r[k] = Sopen(namebuf, "a")) == NULL) {
					sprintf(errst,"ERROR:open, [%s]", namebuf );
					clear_notice(); 
					error_message(errst);
					get_char();
					clear_message(); 
					return(FALSE);
				}
				strcpy(r[k]->fname, namebuf);
				k++; NPORTS++;
		}

	} else {	 /* group or single selected */
		if(NPORTS != 1){ /* group selected */
			set_xmit_buf( Board_Selected ); /* set write size */ 
			k = Group_Selected - 1;
			ink = ((k * 16));
			for(k=0; k< NPORTS ; k++) {	/* group selected */
				/* generate device name */
				gen_dev_name(namebuf, phys_to_log(Board_Selected), ink);
				ink++;
				if(NPORTS == 24 && ink == 12) /* jump over hole in SS24M*/
					ink = 16;
				/* make sure device is closed */
				if (get_exflag(namebuf, pfd, ASYNC_NORMAL_ACTIVE)) { /* if open abort */
					clear_notice(); 
					sprintf(errst,"DEVICE ALREADY OPEN: [%s]", namebuf);
					error_message(errst);
					get_char();
					clear_message(); 
					if(prtn != NULL)
						fclose(prtn);
					if(k > 0){
						for(k=k--; k >= 0; k--){
							Sclose( r[k] );
						}
					}	
					close(pfd);
					return(0);
				}
				thigh = 0;

				/* -------------------------- */
				if((r[k] = Sopen(namebuf, "a")) == NULL) {
					clear_notice(); 
					sprintf(errst,"ERROR:open, [%s]", namebuf);
					error_message(errst);
					get_char();
					clear_message(); 
                    if(k > 1){ /* if any open close all descriptors */
					   for(k=k--; k > 0; k--)
						   Sclose(r[k]); 
                    }
	                nodelay(stdscr, FALSE);
					return(FALSE);
				}
				strcpy(r[k]->fname, namebuf);
			}	
		} else { /* single selected */
			if(ismegadev(eqfilename)) {
				if (get_exflag(eqfilename, pfd, ASYNC_NORMAL_ACTIVE)) { /* if open abort */
					clear_notice(); 
					sprintf(errst,"DEVICE ALREADY OPEN: [%s]", eqfilename);
					error_message(errst);
					get_char();
					clear_message(); 
					close(pfd);
					return(0);
				}
			}
			thigh = 0;
			if((r[0] = Sopen(eqfilename, "a")) == NULL) {
				sprintf(errst,"ERROR:Sopen(%d) [%s]", errno,eqfilename);
				error_message(errst);
				get_char();
				clear_message(); 
				return(FALSE);
			}
			strcpy(r[0]->fname, eqfilename);
		}

	}
	for(k=0; k< NPORTS ; k++) {
		if(ismegadev(r[k]->fname)) {
			if (get_exflag(r[k]->fname, pfd, MPC_LOOPBACK)) /*save flags */
				r[k]->loopback = 1;
			if(Internal_loop == TRUE) /* set loopback */
				set_exflag(r[k]->fname,pfd, EQNCHLOOPON);
		}
		if (tcsetattr(r[k]->fd, TCSANOW, (struct termios *) &(r[k]->cterm))){
			if(errno == EINVAL){
				sprintf(errst,"Baud rate not supported.");
				error_message(errst); get_char(); clear_message();
			} else {
				sprintf(errst,"ERROR ioctl :%d errno:%d", retval, errno);
				error_message(errst); get_char(); clear_message();
			}	
			clear_notice(); 
			close(pfd);
			if(prtn != NULL)
				fclose(prtn);
			for(; k >= 0; k--){
				Sclose( r[k] );
			}
			return(0);
		}
		r[k]->pnum = k;
		oldcnt[k] = 0L;
		tcnt[k]=0L;
	}
	clear_notice();
	note = establish_window( 51, 5, 13, 26, 0,0, " Note ");
	wcursor(note, 2, 1);
	Wprintf(note, "ESC to exit");
	wcursor(note, 2, 3);
	Wprintf(note, "F2  Reset Errors");
	wcursor(note, 2, 5);
	Wprintf(note, "F3  Reset Test");
	wcursor(note, 2, 7);
	Wprintf(note, "F4  Refresh Screen");

	switch(NPORTS) {
		case 1:
		case 8:		wy=5; wtype = 2;
				wh=NPORTS + 2;
				break;
		case 12:	wy=5; wtype = 2;
				wh=NPORTS + 2;
				break;
		case NP:
				wy=0; wtype = 1;
				wh=25;
			break;
		default:
				wy=5; wtype = 2;
				wh=NPORTS + 2;
			break;
	}

	/* display the main window */
	mscrn =	establish_window( 2, wy, wh, 49,wtype,0, wti);
 	for(k=0; k < NPORTS; k++) { /* print port names */
		wprompt(mscrn, 0, k, &r[k]->fname[(strlen(r[k]->fname) -3)]); 
		if (strncmp(r[k]->fname,"/dev/ttyQ", strlen("/dev/ttyQ")) == 0)
			wprompt(mscrn, 0, k, &r[k]->fname[9]); 
		else
			wprompt(mscrn, 0, k, &r[k]->fname[8]); 
	}

    flack = mscrn; /* get a global so child can update screen */
				   /* after child is forked his view of the screen */
				   /* becomes instantly out of date */
	init_template(mscrn);
	for(k=0; k < NPORTS; k++) {		/* set data fields */
		fld = establish_field(mscrn, 4, k, msk43, txbuf[k], 'a');
		sprintf(txbuf[k],fmat, r[k]->xm, r[k]->rc, r[k]->err, 0);
	}
	field_tally(mscrn);
	str_len=strlen((char *)patt);
	time(&oldt);
	
	ftime(&timer);
	last_time = timer.time * 1000L + (unsigned int) timer.millitm;
		
	/* last_time = brdstat.gbl_lbolt * 10; */
	done = readcnt=0;
	nodelay(stdscr, TRUE);

/*-----------------------------*/
    shmid = 0;			/* get a shared memory segment */
    shmid = shmget( key, 0x1000 ,
           IPC_CREAT | SHM_R | SHM_W | 0x06 );                       
   
	speed = baud;
	if (baud > 15)
		speed = (baud - 15) | CBAUDEX;
    switch(speed){			/* set napmstime based on baud */
       case B38400:
                   napmstime = 60;
                   break;
       case B57600: 
                   napmstime = 50; 
                   break;
       case B115200: 
                   napmstime = 30;
                   break;
       case B230400: 
                   napmstime = 10;
                   break;
       case B460800: 
                   napmstime = 1;
                   break;
       default: 
                   napmstime =100;
                   break; 
   }


	/* set up varables to share memory */

    if(shmid != -1){
      xm_ptr = (unsigned long *) shmat( shmid,(char *) 0, SHM_RND);
    }
    rx_ptr = (&(xm_ptr[100])); /* get a pointer to recieve counts */
    err_ptr = (&(xm_ptr[200])); /* get a pointer to error counts */
    rate_ptr = (&(xm_ptr[300])); /* get a pointer to error counts */

    sh_ptr = xm_ptr;	/* sh_ptr is a global used by reader to update 
						   the counters */
/*FORK VERSION*/
    childid[0] = -1;
/*FORK VERSION*/
#define RDCNT_SIM 402
#define RDRST_SIM 401
#define ERRST_SIM 400
#define DERROR_SIM 403
#define CHILD_WAIT 404
#define STOP_WRITE 405

	xm_ptr[CHILD_WAIT] = 0;	/* memory semophors use by parent and reader */
    derror =(int *) (&(xm_ptr[DERROR_SIM]));
    stop_write =(int *) (&(xm_ptr[STOP_WRITE]));
    *stop_write = 0;
    *derror = 0;


    /* fork the reader */
    switch(childid_r = fork()) {
       case -1:
			sprintf(errst,"Error creating read proc.");
			error_message(errst); get_char(); clear_message();
            break;
       case 0:
			while(!done){
				xm_ptr[RDCNT_SIM] = 0; /* read counts not fully updated */
				for(k=0; k < NPORTS; k++){
                   		fastread(r[k]);
			       rx_ptr[k] = r[k]->rc; /* receive count */
			       err_ptr[k] = r[k]->err;  /*  error count */
			       if(*derror){
                       		xm_ptr[CHILD_WAIT] = TRUE;
				       done = TRUE; 
				       break ;
			       } /* stop on error*/
		        }
				xm_ptr[RDCNT_SIM] = 1; /* read counts now fully updated */
                if(xm_ptr[ERRST_SIM]){ /* parent request child to reset errors*/
				   for(k=0; k < NPORTS; k++)
                      			err_ptr[k] = r[k]->err = 0L; 
                   xm_ptr[ERRST_SIM] = 0; 
                }
				/*parent request child to reset read cnt*/
                if(xm_ptr[RDRST_SIM]){
				   for(k=0; k < NPORTS; k++)
                     			rx_ptr[k] = r[k]->rc = 0L; 
                   xm_ptr[RDRST_SIM] = 0; 
                }
            napms(napmstime); 
          }
          while(done){
			 sleep(1); /* wait for parrent to signal me */
          }
          break;
       default:
           break;
    }

/*FORK VERSION*/
    /* fork the writers */
for(wk = 0; wk < NPORTS; wk++) { 
    switch(childid[wk] = fork()) {
/*FORK VERSION*/
       case -1:
			sprintf(errst,"Error creating write proc.");
			error_message(errst); get_char(); clear_message();
            break;
       case 0:
            sleep(3);
            while(true) {

			   if(*derror){
			       break ;
			   } /* stop on error*/
               		if(*stop_write)
                   		break;
            	      if ((retval = write(r[wk]->fd, patt, str_len)) < 0) {
								sprintf(errst,"ERROR write(%d) errno(%d)",retval,errno);
								error_message(errst); get_char(); clear_message();
								*derror = 1;
								*stop_write = 1;
								break;
							} else 
								xm_ptr[wk] += retval;
               		if(*stop_write)
                   		break;

	speed = baud;
	if (baud > 15)
		speed = (baud - 15) | CBAUDEX;
               switch(speed){
                   case B38400:
                               napms(50);
                               break;
       		   case B57600: 
       		   case B115200: 
                               napms(20);
                               break;
       		   case B230400: 
                               napms(10);
                               break;
       		   case B460800: 
                               napms(1);
                               break;
                   default: 
                               napms(napmstime);
                               break; 
               }
            }
            while(true){
				 sleep(1); /* wait for parrent to signal me */
            }
            break;  
       default:
            break; 

    }
} /*FORK VERSION*/
/*---------------------------*/
	nice(25);          
	for( ;done != TRUE; )
	{
       napms(20);

       if(*derror){ /* if child is displaying an error stop and wait */
					/* until child indicates by clearing CHILD_WAIT */
		 nodelay(stdscr, FALSE);
		 break;
       }
	   if((ink = wgetch(stdscr)) == ERR)
				;
	   else
       {
		  nodelay(stdscr, FALSE);
		  switch(ink) {
				case KEY_F2:	/* ---- reset errors --- */
		 		    xm_ptr[ERRST_SIM] = 1; /* tells reader to reset errors*/
					for(k=0; k< NPORTS ; k++) 
						err_ptr[k] = 0L;
					oldt = oldt - upd;
				   	break;
				case KEY_F3:	/* ----- reset test ----- */
		 		    xm_ptr[ERRST_SIM] = 1; /* tells reader to reset errors*/
 					/* tells reader to reset read count*/
		 		    xm_ptr[RDRST_SIM] = 1;
					for(k=0; k< NPORTS ; k++) {
					   oldcnt[k] = 0L;
					   xm_ptr[k] = 0L; /* reset write counts */
					   rx_ptr[k] = 0L; /* reset read counts */
                       err_ptr[k] = 0L;  /* reset errcounts */
					   rate_ptr[k] = 0L;
					}
					oldt = oldt - upd;
				   	break;
				case KEY_F4: 	/* --- refresh screen ---*/
					wrefresh(curscr);
					break;
				default:
					if(ink != ESC)
						beep();
					break;
		   }
 	       nodelay(stdscr, TRUE);
		   if(ink == ESC)
	          break;
	   }
	   if((int)(time(&newt) - oldt) >= upd && !(*derror))
       {
	      if(!xm_ptr[RDCNT_SIM]) /* read counts not fully updated */
             continue;
          xm_ptr[RDCNT_SIM]= 0; /* make sure reader runs before we run again */
          ftime(&timer);
		  new_time = timer.time * 1000L + (unsigned int) timer.millitm;
		  /* make sure some time has ellapsed to prevent divide by zero later*/
		  if((new_time - last_time) == 0)
		     continue;

		  for(k=0; k< NPORTS ; k++) {
			  rate_ptr[k]=
               ((rx_ptr[k] - oldcnt[k]) * 1000) / (new_time - last_time);
			  oldcnt[k] = rx_ptr[k];
			  sprintf(txbuf[k],fmat,
                     xm_ptr[k], rx_ptr[k],
                        err_ptr[k], rate_ptr[k]);
             
			}
			oldt = newt;
			last_time=new_time;
    		if(!*derror)
			   field_tally(mscrn);
            napms(400);
	   }
	}
    *stop_write = 1; /* stop the writer process */
    if(*derror){
		while(!xm_ptr[CHILD_WAIT])
			napms(200);

	}
    nice(19); 
 
	if(NPORTS != 1)
		display_notice("Resetting Ports. Please Wait");

/*FORK VERSION*/
    if(childid[0] != -1){
       for(k = 0; k < NPORTS; k++){
           kill(childid[k], SIGTERM); 
           wait(&ink);  /* put the zombies to rest */
       }
/*FORK VERSION*/
       if(NPORTS > 1) /* allow only the reader to run */
           sleep(1);
       if(childid_r != -1){
           kill(childid_r, SIGTERM); 
           wait(&ink);  /* put the zombies to rest */
       } 
       shmdt((char *)xm_ptr);
       shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);
    } 

	nodelay(stdscr, FALSE);
	flushinp();
	if((tcflush(0 , TCIFLUSH)) == -1){
		error_message("Error TCFLUSH "); get_char(); clear_message();
		eqnx_exit(1);
	}
	flow_ctrl=0;	
	Gloopon=0;
	for(k=0;k < NPORTS; k++){
		tcflush(r[k]->fd , TCIOFLUSH);
		if(ismegadev(r[k]->fname)) {

			if(r[k]->loopback)
				set_exflag(r[k]->fname,pfd, EQNCHLOOPON);
			else
				set_exflag(r[k]->fname,pfd, EQNCHLOOPOFF);
			
		}	
		Sclose( r[k] );
	}
	if(Filespec){
		NPORTS = oldnports;
		fclose(prtn);
	}
	close(pfd);
	clear_notice();
	delete_window(mscrn);
	delete_window(note);
	return(0);
}

/* set extended flags */
int set_exflag(sp,ddev, flag)
char *sp;
int ddev;
unsigned int flag;
{
	int retval;
	struct eqnchstatus chst;
	struct stat sbuf;
	char tbuff[60];

	if(stat(sp, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			sp, errno);
		error_message(tbuff); get_char(); 
		clear_message();
		return(1);
	}
	chst.ec_nbr = sbuf.st_rdev;

	if(Internal_loop){
    		if((retval = ioctl(ddev, flag, &chst ))){
				return(0);	/* bug: not sure what return is */
							/* or what to do about it */
			}
		}
	return (0);

}
/* set extended flags */
int get_exflag(fln,ddev, flag)
char *fln;
int ddev;
unsigned int flag;
{
	int  retval;
	struct eqnchstatus mpstat;
	struct stat sbuf;
	char tbuff[50];
	
	if(stat(fln, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			fln, errno);
		error_message(tbuff); get_char(); 
		clear_message();
		return(1);
	}
	mpstat.ec_nbr = sbuf.st_rdev;
	mpstat.ioctl_version = IOCTL_VERSION;

	if((retval = ioctl( ddev, EQNCHSTATUS, &mpstat))){
		sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d" ,retval, errno);
		error_message(tbuff); get_char(); clear_message();
		return(1);
	}
				/* DEBUG: args may be incompatble */
				/* ISOPEN and MPC_... flags */
	if(flag == ASYNC_NORMAL_ACTIVE)
#if	(LINUX_VERSION_CODE < 132608)
		/* 2.2 and 2.4 kernels */
		if((mpstat.serial_flags & ASYNC_CALLOUT_ACTIVE )
			|| (mpstat.serial_flags & ASYNC_NORMAL_ACTIVE)
			|| (mpstat.ec_openwaitcnt))
#else
		/* 2.6 kernels */
		if ((mpstat.serial_flags & ASYNC_NORMAL_ACTIVE)
			|| (mpstat.ec_openwaitcnt))
#endif
		return(1);
	if(flag == MPC_LOOPBACK)
		if(mpstat.ec_flags & MPC_LOOPBACK)
			return(1);
	return(0);
}
int getbaud(char *s)
{
	char **p = baudstrs;
	int i = 0;
	while (*p)
	{
		if (!strcmp(s, *p++)) return i;
		i++;
	}
	fprintf(stderr, "%s: Unknown baudrate: %s\n", proc, s);
	eqnx_exit(-3);
}
void top_banner()
{
	int x, y;
	chtype tp, attr;
	y = 0;
	if(has_colors() == TRUE)
		attr = COLOR_PAIR(1);
	else {
		 attr =  A_STANDOUT;
	}
	for(x = 0; x < SCRNWIDTH; x++)
		mvwaddch(stdscr, y, x, ' ' | attr);

	y++;
	for(x = 0; x < SCRNWIDTH; x++){
		tp = (mvwinch(stdscr, y, x) | attr);
		mvwaddch(stdscr, y, x, tp );
	}

	move(0,0);
	printw("         ");
	printw("%s %s",
        "SuperSerial Configuration and Diagnostics", VERSNUM); 
	
	wrefresh(stdscr);
	standend();
}
void
set_xmit_buf(int board)
{
	int tmp, ff, max;
	struct eqnbrdstatus brdstat;
	struct brdtab_t *brd_def;

	fill_board_struct(&brdstat, board);
	brd_def = get_board_def(&brdstat);

	/* build the test pattern */
        tmp = 1;
	ff=0;
	if (brd_def->number_of_ports >= 64)
		max = 3840; /* 4096 byte write(minus flow chars & nulls */
	else	max = 1000; /* 1024 bytes minus flow chars */

	while(true){
        	/* strip out FLOW Control characters */
        	if(tmp == 0x11){
           		tmp++;
           		continue; 
        	}

        	if(tmp == 0x13){
           		tmp++;
           		continue; 
        	}

		spatt[ff++] = tmp++;
        	if(tmp == 256){ 
		   if(ff > max) {
             		spatt[ff++] = 0;
             		break;
           		}
		   else 
			 tmp=1;
        	}
	}
	spatt[ff]='\0';

	strcpy((char *)patt,(char *)spatt); 


}

int sspstub(phoney)
int phoney;
{
	int a;

	a = phoney;	

	if(a == 1)
		wrefresh(stdscr);
	return(0);
}
int gen_dev_name(hold, board, chan)
char *hold;
int board;
int chan;
{
	int module;	
	int port;
	char cmod;

	module = chan  / 16;

	if(chan > 15 )
		port = chan % 16;
	else
		port = chan;

	cmod = module  + 'a';
	sprintf(hold,"%s%d%c%d", dvstr,board, cmod, port + 1);

	return(0);
}

struct brdtab_t unknown_board = {
	NOID, NOID, NOBUS, 0, 1, 0, 0, "Unknown" };

struct brdtab_t * get_board_def(struct eqnbrdstatus * brdstat)
{
	struct brdtab_t *brd_def;

	brd_def = find_board_def(brdstat->id,
				 brdstat->bus);
	if (brd_def == NULL)
		brd_def = &unknown_board;

	return (brd_def);
}

/* -------------------------------------------------------------*/
