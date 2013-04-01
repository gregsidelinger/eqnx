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

/* ----------------------------------------------------------------------*/
/*
Date scope menu
*/
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <malloc.h>
/* ----------------------------------------------------------------------*/
#include <termios.h>

#include <errno.h>
/*-------------*/
#include "twindow.h"
#include "cstream.h"

#define DTRBF 0
#define RTSBF 1
#define DCDBF 2
#define CTSBF 3
#define DSRBF 4
#define RIBF  5

#include "xtend.h"
#include "sizes.h"

typedef unsigned long paddr_t;
#define ulong_t unsigned long
#define	uint_t	unsigned int
#define ushort_t unsigned short
#define uchar_t unsigned char

#include "icp.h"
#include "eqnx_ioctl.h"

extern int errno;

/* other externs */

extern char msk25[];
extern char eqfilename[];
extern char device0[];
extern char device1[];
extern char ddevice[];
extern char regmsk[];
extern char on[];
extern char off[];

extern int ismegadev();
extern int warning();
extern int eqget_chan();
extern int eqget_board();

static void update_fillbar();

extern int data_entry (SWIN *);

/* --------------------------------------------------------------------------*/
/* DATA_SCOPE */

/*int data_scope 

*/

/* --------------------------------------------------------------------------*/
char *monchoice[] = {
	"INPUT ",
	"OUTPUT ",
	"FULL DUPLEX ",
	0
};
char *bufchoice[] = {
	"32K ",
	"64K ",
	"128K " ,
	0
};

unsigned Buftable[] = {
	0x8000,
	0x10000,
	0x20000,
	0
};

char *bfullchoice[] = {
	"Continue ",
	"Stop ",
	0
};
char *displaychoice[] = {
	"Data Only ",
	"Show Detail ",
	0
};

char *scopechoice[] = {
	"Start Scope ",
	"Review Data ",
	0
};

int review_data(); 
int esave_to_file();
static int (*revfuncs[])()={review_data, esave_to_file};

char *revselcs[] = {
	" Review Data ",
	" Save Data ",
	NULL
};


static SMENU review_men[] = {
	{ " Option ", revselcs, revfuncs},
};
/* table to convert nibbles to hex value */
char hexval[] = {
	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' 
};
int start_scope(), start_bbox(), review_data();


static int (*scopefuncs[])()={ start_scope ,review_data};

/* scope submenus */
static SMENU scope_men[] = {
	{ " Scope ", scopechoice, scopefuncs},
};



int validate_megadev();

void helper_monitor();
void helper_buffer();
void helper_bfull();
void helper_display();
void helper_start();
int mon_full_duplex();
int mon_input();
int mon_output();

int Mon_Type = 0;
int Mbuf_Size = 0;
int Mbuf_Full = 0;
int Dis_Type = 1;
int Start_def =0;
static int Review_def = 1;
#define MINPUT 0		/* output of helper_monitor */
#define MOUTPUT 1
#define MBOTH 2

char sctrsig[10][15];  
static char **scopebuf,**tp;

int
data_scope()
{
	char tbuff[50];
	SWIN *regwn;
	struct field *fld;
	int ee, done = 0;	


	scopebuf = tp = NULL;
	/* malloc string pointers */
	scopebuf = (char **)malloc(11 * sizeof(char *));
	if(scopebuf == NULL){	
		sprintf(tbuff,"ERROR MALLOC: errno:%d", errno);
		error_message(tbuff);	
		get_char(); clear_message();
	}	
	tp = scopebuf;
	/* malloc character arrays */
	for(ee = 0; ee < 10; ee++){
		*tp = (char *) malloc(45);
		if(*tp == NULL){	
			sprintf(tbuff,"ERROR MALLOC: errno:%d", errno);
			error_message(tbuff);	
			get_char(); clear_message();
		}	
		tp++;

	}
	*tp = NULL;
	
	/* alloc and display window */
	regwn = establish_window( 14, 7, 10, 38, 0, 0, " Data Scope ");
	field_pass_mode(regwn, 1);

	/* display field names */
	wprompt(regwn, 1, 1, "Device:");
	wprompt(regwn, 1, 2, "Monitor:");
  	wprompt(regwn, 1, 3, "Buffer:");
	wprompt(regwn, 1, 4, "Buffer Full:");	
	wprompt(regwn, 1, 5, "Display:");
	wprompt(regwn, 1, 6, "Start:");
   
	/* create linked list of fields */
	fld = establish_field(regwn, 14, 1, msk25 + 5, scopebuf[0], 'a'); 
	field_window(fld,"pname   ", 30 , 7);	/* set field generic help name */
	field_validate(fld, validate_megadev);	/* set helper function */

	fld = establish_field(regwn, 14, 2, msk25+13, scopebuf[1], 'H'); 
	field_window(fld,"SCOPEDIR", 30, 7); 
	field_help(fld, helper_monitor);

	fld = establish_field(regwn, 14, 3, msk25+20, scopebuf[2], 'H'); 
	field_window(fld,"BUFFSIZE", 30, 7); 
	field_help(fld, helper_buffer);
			
	fld = establish_field(regwn, 14, 4, msk25+15, scopebuf[3], 'H'); 
	field_window(fld,"BUFFULL ", 30, 7); 
	field_help(fld, helper_bfull);
			
	fld = establish_field(regwn, 14, 5, msk25+12, scopebuf[4], 'H'); 
	field_window(fld,"DISPLAY ", 30, 7); 
	field_help(fld, helper_display);
	
	fld = establish_field(regwn, 14, 6, msk25+10, scopebuf[5], 'H'); 
	field_window(fld,"STARTSCO", 30, 7); 
	field_help(fld, helper_start);

	/* set defaults */	
	strcpy( scopebuf[0], eqfilename);
	strcpy( scopebuf[1], monchoice[Mon_Type]);
	strcpy( scopebuf[2], bufchoice[Mbuf_Size]);
	strcpy( scopebuf[3], bfullchoice[Mbuf_Full] );
	strcpy( scopebuf[4], displaychoice[Dis_Type]);
	strcpy( scopebuf[5], scopechoice[Start_def]);

	field_tally(regwn);
	
	while(!done){
		if(data_entry(regwn) == ESC) {
			done = true;
			break;	
		}
	}	
	strcpy(eqfilename, scopebuf[0]);
	delete_window(regwn);

	/* free alloc'ed space */
	tp = scopebuf;
	while(*tp)
		free(*tp++);
	free(scopebuf);

	return(0);
}
/* validate a mega device */
/* called during data entry processing to validate our device names
*/
int validate_megadev( dname)
char *dname;
{
	if(!ismegadev(dname))
			return ERROR;
	strcpy(eqfilename, dname);
	return OK;

}
/* helper_monitor */
/* monitor selection helper routine; called during data entry processing
   to prompt a user of possible monitor selection. Choices are INPUT, OUTPUT
   and FULL DUPLEX.
*/
void helper_monitor( bf )
char *bf;			/* bf contains current entry */
{
	int ret, def;	
	SMENU vmen;
	vmen.mselcs = monchoice;
	vmen.func = NULL;

	def = Mon_Type + 1;
 
	ret = vscroll("", &vmen, def, 29, 10);
	
	Mon_Type = ret -1;	
	strcpy( bf, monchoice[ Mon_Type]);
}

/* buffer_monitor */
/* buffer selection helper routine; called during data entry processing
   to prompt a user of possible buffer size selection. Choices are 32, 
   64 and 128K.
*/
void helper_buffer( bf )
char *bf;			/* bf contains current entry */
{
	int ret, def;	
	SMENU vmen;
	vmen.mselcs = bufchoice;
	vmen.func = NULL;

	def = Mbuf_Size + 1;
 
	ret = vscroll("", &vmen, def, 29, 11);
	
	Mbuf_Size= ret -1;	
	strcpy( bf, bufchoice[ Mbuf_Size]);


}

/* buffer_bfull */
/* buffer full  selection helper routine; called during data entry processing
   to prompt a user of possible buffer size selection. Choices continue;
   in which case we wrap when buffer full or stop.
 
*/
void helper_bfull( bf )
char *bf;			/* bf contains current entry */
{
	int ret, def;	
	SMENU vmen;
	vmen.mselcs = bfullchoice;
	vmen.func = NULL;

	def = Mbuf_Full + 1;
 
	ret = vscroll("", &vmen, def, 29, 12);
	
	Mbuf_Full= ret -1;	
	strcpy( bf, bfullchoice[ Mbuf_Full]);

}

/* helper_display */
/* display selection helper routine; called during data entry processing
   to prompt a user of possible display types. Choices Scopy only, 
   and Show Detail; 
 
*/
void helper_display( bf )
char *bf;			/* bf contains current entry */
{
	int ret, def;	
	SMENU vmen;
	vmen.mselcs = displaychoice;
	vmen.func = NULL;

	def = Dis_Type + 1;
 
	ret = vscroll("", &vmen, def, 29, 13);
	
	Dis_Type = ret -1;	
	strcpy( bf, displaychoice[ Dis_Type]);
}

/* helper_start */
/* start selection helper routine; called during data entry processing
   to prompt a user of possible display types. Choices Start scope, review
   and break_out box; 
 
*/
unsigned int Ibufcount;
unsigned int Obufcount;
unsigned int Gibuf_idx;
unsigned int Gobuf_idx;

unsigned char *Ibuf = NULL;
unsigned char *Obuf = NULL;
#define RDSIZE 1024
#define SCPUT  8
#define ASCBEG 50
#define FASCBEG 53


void helper_start( bf )
char *bf;			/* bf contains current entry */
{
	int def;	

	def = Start_def + 1;
	curs_set(0); 
	vscroll(" Scope ", scope_men, 1,  29, 14); 
	curs_set(1); 
}
char sdetail[5][25];

int start_scope()
{
	SWIN *regwn;
	struct field *fld;
	char tbuff[50];
	char title[50];
	int pfd, fd0, fd1;
	struct eqnchstatus mpstat;
	struct eqnbrdstatus brdstat;
	struct stat sbuf;
	int retval, mchan, mbrd, data_avail;
	int dbsize, height, width, xx;
	unsigned int charin;
	data_avail = 0;
	if((fd0 = open( device0, O_RDONLY | O_NDELAY)) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", device0, errno);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}
	if((fd1 = open( device1, O_RDONLY | O_NDELAY)) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", device1, errno);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}

	if((pfd = open( ddevice, O_RDWR)) < 0){
		sprintf(tbuff,"ERROR OPEN %s: errno:%d", device0, errno);
		error_message(tbuff); get_char(); clear_message();
		return(0);
	}

    mchan = eqget_chan(eqfilename);
    mbrd  = eqget_board(eqfilename);

	/* DEBUG
	sprintf(tbuff, " stat (%s)", 
		eqfilename);
	error_message(tbuff); get_char(); 
	clear_message();
	ENDBUG */

	if(stat(eqfilename, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			eqfilename, errno);
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}
	/* DEBUG
	sprintf(tbuff, " stat (%x)", 
		sbuf.st_rdev);
	error_message(tbuff); get_char(); 
	clear_message();
	ENDEBUG 
	*/
	mchan = sbuf.st_rdev;
    mpstat.ec_nbr = sbuf.st_rdev;
    mpstat.ioctl_version = IOCTL_VERSION;
    brdstat.brd_nbr = mbrd;
    brdstat.ioctl_version = IOCTL_VERSION;

    if(brdstat.brd_nbr < 0){
       sprintf(tbuff, "This device does not exist.");
       error_message(tbuff); get_char();
       clear_message();
       return(0);
     }

     if((retval = ioctl( pfd, EQNCHSTATUS,(char *)&mpstat))){
        sprintf(tbuff,"ERROR EQNCHSTATUS:%d errno:%d", retval, errno);
        error_message(tbuff); get_char();
        clear_message();
        return(0);
     }
     if((retval = ioctl( pfd, EQNBRDSTATUS,(char *)&brdstat))){
        sprintf(tbuff,"ERROR EQNBRDSTATUS:%d errno:%d", retval, errno);
        error_message(tbuff); get_char();
        clear_message();
        return(0);
     }

/* --------------------------------------------------------*/
	if(ioctl( fd0, EQNCHMONSET, &mchan )){
		sprintf(tbuff,"ERROR ioctl: %s, errno:%d", device0, errno);
		error_message(tbuff);	
		get_char(); clear_message();
		return(0);	
	}
	if(ioctl( fd1, EQNCHMONSET, &mchan )){
		sprintf(tbuff,"ERROR ioctl: %s, errno:%d", device0, errno);
		error_message(tbuff);	
		get_char(); clear_message();
		return(0);	
	}

	dbsize = Buftable[Mbuf_Size];

/* --------------------------------------------------------*/
	if(Dis_Type == 1)
		height = 21;
	else
		height = 18;	
	width = 68;	
	if(Mon_Type == MINPUT)
		sprintf(title," Monitoring Receive data from %s", eqfilename);
	if(Mon_Type == MOUTPUT)
		sprintf(title," Monitoring Transmit data to %s", eqfilename);
	if(Mon_Type == MBOTH) {
		sprintf(title," Monitoring %s", eqfilename);
		width = 71;
	}
	/* display the window */	
	regwn = establish_window( 4, 2, height, width,  0,
			(has_colors() ? 1 : 0), title);
/*======================*/
	if(Dis_Type == 1){
/* 422 LD boards have no control sigs */
       if(brdstat.lmx[0][0].lmx_id != 6 &&
             brdstat.lmx[0][0].lmx_id != 7)
       {
	   	  wprompt(regwn, 0, 17, "DCD");
		  wprompt(regwn, 8, 17, "DTR");
	   }

/* 12/24SM and 422  have no RTS/CTS */
       if(brdstat.lmx[0][0].lmx_id != 6
          && brdstat.lmx[0][0].lmx_id != 7
          && brdstat.id != 0x30
          && brdstat.id != 0x38 )
		{
		   wprompt(regwn, 0, 18, "CTS");
		   wprompt(regwn, 8, 18, "RTS");
		}	

		wprompt(regwn, 28, 16, "Buffer Utilization");
		wprompt(regwn, 19, 17, "0%");
		wprompt(regwn, 53, 17, "100%");
		if(Mon_Type != MBOTH) 
			wprompt(regwn, 22, 18, "Press any key exit");


/* 422 LD boards have no control sigs */
       if(brdstat.lmx[0][0].lmx_id != 6 &&
             brdstat.lmx[0][0].lmx_id != 7)
       {
			fld = establish_field(regwn, 4, 17,
				regmsk + 1 , sctrsig[DCDBF], 'a'); 
			fld = establish_field(regwn, 12, 17,
				regmsk + 1 , sctrsig[DTRBF], 'a'); 
		}

/* 12/24SM and 422  have no RTS/CTS */
       if(brdstat.lmx[0][0].lmx_id != 6
          && brdstat.lmx[0][0].lmx_id != 7
          && brdstat.id != 0x30
          && brdstat.id != 0x38 )
		{
			fld = establish_field(regwn, 4, 18,
				regmsk + 1 , sctrsig[CTSBF], 'a'); 
			fld = establish_field(regwn, 12, 18,
				regmsk + 1 , sctrsig[RTSBF], 'a'); 
		}	
	}	
		curs_set(0);
		switch( Mon_Type){
			case MINPUT:
				close(fd1);		/* close output desc */
				Ibufcount = 0;
				if(Ibuf)
					free(Ibuf);
				/* malloc the huge data storage buffers */
				if((Ibuf = (unsigned char *) malloc( dbsize + 2) ) == NULL){
					sprintf(tbuff,"ERROR malloc: errno:%d",  errno);
					error_message(tbuff);	
					get_char(); clear_message();
					return(0);	
				}
				if(Dis_Type == 1)
					wprompt(regwn, 16, 17, "RX");
				data_avail = mon_input(regwn, fd0, pfd, dbsize, mchan);
				break;
			case MOUTPUT:
				Obufcount = 0;
				close(fd0);		/* close input desc */
				if(Obuf)
					free(Obuf);
				/* malloc the huge data storage buffers */
				if((Obuf = (unsigned char *) malloc( dbsize + 2) ) == NULL){
					sprintf(tbuff,"ERROR malloc: errno:%d",  errno);
					error_message(tbuff);	
					get_char(); clear_message();
					return(0);	
				}
				if(Dis_Type == 1)
					wprompt(regwn, 16, 17, "TX");
				data_avail = mon_output(regwn, fd1, pfd, dbsize, mchan);
				break;
			case MBOTH:	
				Obufcount = Ibufcount = 0;
				if(Ibuf)
					free(Ibuf);
				if(Obuf)
					free(Obuf);
				/* malloc the huge data storage buffers */
				if((Ibuf = (unsigned char *) malloc( dbsize + 2) ) == NULL){
					sprintf(tbuff,"ERROR malloc: errno:%d",  errno);
					error_message(tbuff);	
					get_char(); clear_message();
					return(0);	
				}
				if((Obuf = (unsigned char *) malloc( dbsize + 2) ) == NULL){
					sprintf(tbuff,"ERROR malloc: errno:%d",  errno);
					error_message(tbuff);	
					get_char(); clear_message();
					return(0);	
				}
		
				for(xx = 0; xx < 16; xx += 2){
					wprompt(regwn, 0, xx, "RX");
					wprompt(regwn, 0, xx + 1, "TX");
				}
				if(Dis_Type == 1){
					wprompt(regwn, 16, 17, "RX");
					wprompt(regwn, 16, 18, "TX");
					wprompt(regwn, 19, 18, "0%");
					wprompt(regwn, 53, 18, "100%");
				}
				data_avail = mon_full_duplex(regwn, fd0,
					fd1, pfd, dbsize, mchan);
				break;
			default:
				break;
		}
		close(pfd);
		delete_window(regwn);
		nodelay(stdscr, FALSE);		/* set to delayed reads */
		if(data_avail){
		   	regwn = establish_window(13, 10, 6, 49, 0, 1, "");
			wcursor(regwn, 1, 0);
			Wprintf(regwn, " Data has been captured in the buffer. You may "); 
			wcursor(regwn, 1, 1);
			Wprintf(regwn, " review this data or save it for later review. ");
			wcursor(regwn, 1, 2);
			Wprintf(regwn, " Press Enter to proceed. ");
			charin = get_char();
			delete_window(regwn);
			if(charin == '\r'){
				Review_def = vscroll( " Select ", review_men, Review_def,
					29, 12);

				/* error_message("proceeding to review data");	
				get_char(); clear_message(); */
			} 

		}

/*======================*/
	return(0);

}
#define PBARWIDTH 30
/* -------------------------------------------------------*/
int mon_input(regwn, fd0, pfd, dbsize, chan)
SWIN *regwn;
int fd0;
int pfd;
int dbsize;
int chan;
{
	char tbuff[50];
	int ibuf_idx, retv0, retval, bwrapped;
	int iscr_idx, done, junk, ascol, datavail;
	int buf_full, scrcol, scrrow, rsz, ucnt, tt, scrcnt;
	struct stat sbuf;
    struct eqnbufstatus bufstat;
	unsigned int last_time, new_time;
	struct timeb timer;
	unsigned int ii, vv, oldii, oldvv, percent;
	float fibuf_idx, fperc;

	datavail = ii = vv = 0;
	oldii = oldvv = 1;
	scrcnt = 0;
	/* set indexes into data buffers */	
	Gibuf_idx = ibuf_idx =  0; 	/* data read into buffer*/
	iscr_idx =  0;	/* data sent to screen */
	bwrapped = buf_full = done = 0;
	
	ascol = ASCBEG;
	reverse_video(regwn);
	sprintf(sdetail[0], "%d", 0);
	sprintf(sdetail[1], "%d", 0);
	field_tally(regwn);

	for(scrrow = 0; scrrow < 16; scrrow++) {
		ascol = ASCBEG;
		for(scrcol = 0; scrcol < 47; scrcol++) {
				wmputchar(regwn, scrcol, scrrow, ' ');
				wmputchar(regwn, ascol++ , scrrow, ' ');
			if(ascol >= ASCBEG + 16)
				ascol = ASCBEG;
		}
	}
	if(stat(eqfilename, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			eqfilename, errno);
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}
	bufstat.ebf_nbr = sbuf.st_rdev;	
	bufstat.ioctl_version = IOCTL_VERSION;

	if(Dis_Type == 1){
		if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
			sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
			close(fd0);
			error_message(tbuff); get_char(); 
			clear_message();
			return(0);
		}
	}

	ascol = ASCBEG;
	scrcol = scrrow = 0;
	nodelay(stdscr, TRUE);	/* set stdin to nodelay */
	/* get tic count for timing control sig transition */
	ftime(&timer);
	last_time = timer.time * 1000L + (unsigned int) timer.millitm;
	new_time = last_time;
	while(!done){
		rsz = min((RDSIZE * 4), (dbsize - ibuf_idx));
		if(!buf_full)
			if( (retv0 = read(fd0, &Ibuf[ibuf_idx], rsz )) > 0) {
				datavail = 1;
                if((ibuf_idx + retv0) >= dbsize) /* wrapped get an index */
                {
                  Gibuf_idx = (ibuf_idx + retv0) - dbsize; 
                  ibuf_idx = Gibuf_idx; 
				  if(Mbuf_Full == 0){ /* wrap buffer */
					  /* ibuf_idx = 0; */
					  reverse_video(regwn);
					  wprompt(regwn, 58, 17, "Wrapped");
					  reverse_video(regwn);
					  bwrapped = 1;
				 } else {
					  buf_full = 1;
					  close(fd0);
					  reverse_video(regwn);
					  wprompt(regwn, 58, 17, "Full");
					  reverse_video(regwn);
					  /* DEBUG: notify user */
				  }
                }
                else
                {
				  ibuf_idx += retv0; 
                  Gibuf_idx = ibuf_idx; 

                }
                if(bwrapped || buf_full)
                   Ibufcount = dbsize;
                else
				   Ibufcount = max((unsigned int) ibuf_idx, 
						   Ibufcount);
#ifdef FOOOO
				if(ibuf_idx >= dbsize) { /* is the buffer full */
					if(Mbuf_Full == 0){ /* wrap buffer */
						/* ibuf_idx = 0; */
						reverse_video(regwn);
						wprompt(regwn, 58, 17, "Wrapped");
						reverse_video(regwn);
						bwrapped = 1;
					} else {
							buf_full = 1;
							close(fd0);
							reverse_video(regwn);
							wprompt(regwn, 58, 17, "Full");
							reverse_video(regwn);
							/* DEBUG: notify user */
						}
				}
#endif
			}
#define SCRUPDATE 768
		if(iscr_idx == ibuf_idx) { 
			reverse_video(regwn);
			wmputchar(regwn, scrcol, scrrow, ' ');
			wmputchar(regwn, ascol,scrrow , ' '); /* rub out ascii */
			reverse_video(regwn);
			napms(10);  /* take napms 10 mils: force context switch */ 
			ucnt = 0;	
		} else {
			if(ibuf_idx > iscr_idx){
				ucnt = min((ibuf_idx - iscr_idx), SCRUPDATE);
				iscr_idx = ibuf_idx - ucnt;
			} else {
				ucnt = min(((dbsize - iscr_idx) + ibuf_idx), SCRUPDATE); 
				if((dbsize - iscr_idx) < ucnt){
					if(ibuf_idx > SCRUPDATE)
						iscr_idx = ibuf_idx - ucnt; 
					else {
						iscr_idx = ibuf_idx;
						iscr_idx += (ucnt - iscr_idx);
					}
				}
			}
		}	
		scrcnt = 0;
		while(ucnt){
			tt = Ibuf[iscr_idx];
			wmputchar(regwn, scrcol, scrrow, hexval[(tt >> 4) & 0x0f]);
			wmputchar(regwn, scrcol+ 1, scrrow, hexval[(tt & 0x0f)]);
			/* print ascii character */
			if(isprint(Ibuf[iscr_idx])){
				wmputchar(regwn, ascol, scrrow, Ibuf[iscr_idx]);
			} else wmputchar(regwn,ascol,scrrow, '.');
				
			if((scrcnt % 256) == 0)
				wrefresh(stdscr);
			scrcnt++;
			iscr_idx++;
			if(iscr_idx >= dbsize) {
				if(Mbuf_Full == 1)
					break;
				iscr_idx = 0;
			}
			scrcol+= 3;
			if(scrcol >= 48){
				scrrow++;
				scrcol = 0;
			}
			if(scrrow >= 16 )
				scrrow = 0 ;
			ascol++;
			if(ascol >= ASCBEG + 16)
				ascol = ASCBEG;
			ucnt--;
		}

		if(Dis_Type == 1){
			ftime(&timer);
			new_time = timer.time * 1000L + (unsigned int) timer.millitm;
			if((new_time - last_time) > 100){
				if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
					sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d",
						retval, errno);
					close(fd0);		/* close output desc */
					error_message(tbuff); get_char(); 
					clear_message();
					return(0);
				}

				/* ---- get control sigs ------*/
				vv = bufstat.ebf_outsigs; 
				ii = bufstat.ebf_insigs;
				if(vv != oldvv || ii != oldii){
					sprintf(sctrsig[RTSBF],"%s", (vv & 0x02 ? on : off) ); 
					sprintf(sctrsig[DTRBF],"%s", (vv & 0x01 ? on : off) ); 
					sprintf(sctrsig[DCDBF],"%s", (ii & 0x01 ? on : off) ); 
					sprintf(sctrsig[CTSBF],"%s", (ii & 0x02 ? on : off) ); 
					field_tally(regwn);
					oldii = ii;
					oldvv = vv;
				}
				last_time = new_time;
				fibuf_idx = ibuf_idx;
				fperc = fibuf_idx / dbsize;
				percent = fperc * PBARWIDTH;
				if(bwrapped)
					percent = PBARWIDTH;
				update_fillbar(regwn, percent, 0);
			}
		}
		/* did the user hit a key */
		if((junk = wgetch(stdscr)) == ERR)
				continue;		/* if not continue */
		else {				/* else we are finished */
			nodelay(stdscr, FALSE);		/* set to delayed reads */
			flushinp();
			done=1;
			break;
		}

	}
 	if(!buf_full)
		close(fd0);
	
	reverse_video(regwn);
	return(datavail);

}
/* -------------------------------------------------------*/

/* -------------------------------------------------------*/
int mon_output(regwn, fd1, pfd, dbsize, chan)
SWIN *regwn;
int fd1;
int pfd;
int dbsize;
int chan;
{
	char tbuff[50];
	int obuf_idx, retv1, retval, bwrapped;
	int oscr_idx, done, junk, ascol, datavail;
	int buf_full, scrcol, scrrow, rsz, ucnt, tt,scrcnt;
	struct stat sbuf;
    struct eqnbufstatus bufstat;
	unsigned int last_time, new_time;
	struct timeb timer;
	unsigned int vv, ii, oldii, oldvv, percent;
	float fobuf_idx, fperc;

	datavail = bwrapped = ii = vv = 0;
	oldii = oldvv = 1;

	/* set indexes into data buffers */	
	Gobuf_idx = obuf_idx = 0; 	/* data read into buffer*/
	oscr_idx = 0;	/* data sent to screen */
	scrcol = scrrow = 0;
	buf_full = done = 0;
	reverse_video(regwn);
	
	sprintf(sdetail[0], "%d", 0);
	sprintf(sdetail[1], "%d", 0);
	field_tally(regwn);

	/* highlight ouput lines */
	for(scrrow = 0; scrrow < 16; scrrow++) {
		ascol = ASCBEG;
		for(scrcol = 0; scrcol < 47; scrcol++) {
				wmputchar(regwn, scrcol, scrrow, ' ');
				wmputchar(regwn, ascol++ , scrrow, ' ');
			if(ascol >= ASCBEG + 16)
				ascol = ASCBEG;
		}
	}	
	if(stat(eqfilename, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			eqfilename, errno);
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}
	bufstat.ebf_nbr = sbuf.st_rdev;	
	bufstat.ioctl_version = IOCTL_VERSION;
	if(Dis_Type == 1){
		if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
			sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
			close(fd1);		/* close output desc */
			error_message(tbuff); get_char(); 
			clear_message();
			return(0);
		}
	}
	ftime(&timer);
	last_time = timer.time * 1000L + (unsigned int) timer.millitm;
	new_time = last_time;
	ascol = ASCBEG;
	scrcol = scrrow = 0;
	nodelay(stdscr, TRUE);	/* set stdin to nodelay */
	while(!done){
		rsz = min((RDSIZE * 4), (dbsize - obuf_idx));
		if(!buf_full)
			if((retv1 = read(fd1, &Obuf[obuf_idx], rsz )) > 0) {
				datavail = 1;
                if((obuf_idx + retv1) >= dbsize) /* wrapped get an index */
                {
                  Gobuf_idx = (obuf_idx + retv1) - dbsize; 
                  Gobuf_idx = 0;
                  obuf_idx = Gobuf_idx; 
				  if(Mbuf_Full == 0){ /* wrap buffer */
					  /* obuf_idx = 0; */
					  reverse_video(regwn);
					  wprompt(regwn, 58, 17, "Wrapped");
					  reverse_video(regwn);
					  bwrapped = 1;
				 } else {
					  buf_full = 1;
					  close(fd1);
					  reverse_video(regwn);
					  wprompt(regwn, 58, 17, "Full");
					  reverse_video(regwn);
					  /* DEBUG: notify user */
				  }
                }
                else
                {
				  obuf_idx += retv1; 
                  Gobuf_idx = obuf_idx; 

                }
			    if(bwrapped || buf_full)
                   Obufcount = dbsize;
                else
				   Obufcount = max((unsigned int) obuf_idx, 
						   Obufcount);
#ifdef FOOOO
				if(obuf_idx >= dbsize) { /* is the buffer full */
					if(Mbuf_Full == 0){ /* wrap buffer and continue */
						obuf_idx = 0;
						bwrapped = 1;
						reverse_video(regwn);
						wprompt(regwn, 58, 17, "Wrapped");
						reverse_video(regwn);
			 		} else {
							buf_full = 1;
							close(fd1);
							/* DEBUG: notify user */
							reverse_video(regwn);
							wprompt(regwn, 58, 17, "Full");
							reverse_video(regwn);
						}
					}
#endif
			}
		if(oscr_idx == obuf_idx) { 
			reverse_video(regwn);
			wmputchar(regwn, scrcol, scrrow, ' ');
			wmputchar(regwn, ascol,scrrow , ' '); /* rub out ascii */
			reverse_video(regwn);
			napms(10);  /* take napms 10 mils: force context switch */ 
			ucnt = 0;	
		} else {
			if(obuf_idx > oscr_idx){
				ucnt = min((obuf_idx - oscr_idx), SCRUPDATE);
				oscr_idx = obuf_idx - ucnt;
			} else {
				ucnt = min(((dbsize - oscr_idx) + obuf_idx), SCRUPDATE); 
				if((dbsize - oscr_idx) < ucnt){
					if(obuf_idx > SCRUPDATE)
						oscr_idx = obuf_idx - ucnt; 
					else {
						oscr_idx = obuf_idx;
						oscr_idx += (ucnt - oscr_idx);
					}
				}
			}
		}	
		scrcnt = 0;
		while(ucnt){
			tt = Obuf[oscr_idx];
			wmputchar(regwn,scrcol,scrrow ,hexval[(tt >> 4) & 0x0f]);
			wmputchar(regwn,scrcol + 1,
				scrrow ,hexval[(tt & 0x0f)]);
			/* print ascii character */
			if(isprint(Obuf[oscr_idx])){
				wmputchar(regwn, ascol, scrrow , Obuf[oscr_idx]);
			} else wmputchar(regwn,ascol, scrrow , '.');
				
			if((scrcnt % 256) == 0)
				wrefresh(stdscr);
			scrcnt++;
			oscr_idx++;
			if(oscr_idx >= dbsize) {
				if(Mbuf_Full == 1)
					break;
				oscr_idx = 0;
			}
			scrcol+= 3;
			if(scrcol >= 48){
				scrrow++;
				scrcol = 0;
			}
			if(scrrow >= 16 )
				scrrow = 0 ;
			ascol++;
			if(ascol >= ASCBEG + 16)
				ascol = ASCBEG;
			ucnt--;
		}
 
		if(Dis_Type == 1){
			ftime(&timer);
			new_time = timer.time * 1000L + (unsigned int) timer.millitm;
			if((new_time - last_time) > 80){
				if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
					sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d",
						retval, errno);
					close(fd1);		/* close output desc */
					error_message(tbuff); get_char(); 
					clear_message();
					return(0);
				}
				/* ---- get control sigs ------*/
				vv = bufstat.ebf_outsigs; 
				ii = bufstat.ebf_insigs;
				if(vv != oldvv || ii != oldii){
					sprintf(sctrsig[RTSBF],"%s", (vv & 0x02 ? on : off) ); 
					sprintf(sctrsig[DTRBF],"%s", (vv & 0x01 ? on : off) ); 
					sprintf(sctrsig[DCDBF],"%s", (ii & 0x01 ? on : off) ); 
					sprintf(sctrsig[CTSBF],"%s", (ii & 0x02 ? on : off) ); 
					field_tally(regwn);
					oldii = ii;
					oldvv = vv;
				}
				last_time = new_time;
				fobuf_idx = obuf_idx;
				fperc = fobuf_idx / dbsize;
				percent = fperc * PBARWIDTH;
				if(bwrapped)
					percent = PBARWIDTH;
				update_fillbar(regwn, percent, 1);
			}
		}

		/* did the user hit a key */
		if((junk = wgetch(stdscr)) == ERR)
				continue;		/* if not continue */
		else {				/* else we are finished */
			nodelay(stdscr, FALSE);		/* set to delayed reads */
			flushinp();
			done=1;
			break;
		}
	}

	if(!buf_full)
		close(fd1);

	reverse_video(regwn);
	return(datavail);

}
/* -------------------------------------------------------*/

/* -------------------------------------------------------*/
int mon_full_duplex(regwn, fd0, fd1, pfd, dbsize, chan)
SWIN *regwn;
int fd0;
int fd1;
int pfd;
int dbsize;
int chan;
{
	char tbuff[50];
	int ibuf_idx ,obuf_idx, retv0, retv1, retval;
	int iscr_idx ,oscr_idx, done, junk, ascol;
	int ibuf_full, obuf_full, scrcnt, scrcol, scrrow, rsz, iucnt, ucnt, tt;
    struct eqnbufstatus bufstat;
	struct stat sbuf;
	unsigned int last_time, new_time;
	struct timeb timer;
	int obwrapped, ibwrapped, datavail; 
	unsigned int vv,ii, oldii, oldvv, percent;
	float fbuf_idx, fperc;


	datavail = obwrapped = ibwrapped = 0;
	ii = vv = 0;
	oldii = oldvv = 1;
	scrcnt = 0;	
	Gobuf_idx = obuf_idx = 0; 	/* data read into buffer*/
	/* set indexes into data buffers */	
	Gobuf_idx = Gibuf_idx = ibuf_idx = obuf_idx = 0;/* data read into buffer*/
	iscr_idx = oscr_idx = 0;	/* data sent to screen */
	scrcol = scrrow = 0;
	obuf_full = ibuf_full = done = 0;
	reverse_video(regwn);
	sprintf(sdetail[0], "%d", 0);
	sprintf(sdetail[1], "%d", 0);
	field_tally(regwn);
	reverse_video(regwn);
	/* highlight ouput lines */
	for(scrrow = 0; scrrow < 16; scrrow += 2) {
		ascol = FASCBEG;
		for(scrcol = 3; scrcol < 50; scrcol++) {
				reverse_video(regwn);
				wmputchar(regwn, scrcol, scrrow + 1, ' ');
				wmputchar(regwn, ascol++ , scrrow + 1, ' ');
				reverse_video(regwn);
			if(ascol >= FASCBEG + 16)
				ascol = FASCBEG;
		}
	}	
	if(stat(eqfilename, &sbuf) == -1 ){
		sprintf(tbuff, "ERROR: stat %s, errno %d", 
			eqfilename, errno);
		error_message(tbuff); get_char(); 
		clear_message();
		return(0);
	}
	bufstat.ebf_nbr = sbuf.st_rdev;	
	bufstat.ioctl_version = IOCTL_VERSION;
	if(Dis_Type == 1){
		if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
			sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d", retval, errno);
			close(fd1);		/* close output desc */
			close(fd0);		/* close output desc */
			error_message(tbuff); get_char(); 
			clear_message();
			return(0);
		}
	}

	ascol = FASCBEG;
	scrrow = 0;
	scrcol = 3;
	nodelay(stdscr, TRUE);	/* set stdin to nodelay */
	ftime(&timer);
	last_time = timer.time * 1000L + (unsigned int) timer.millitm;
	new_time = last_time;
	while(!done){
		rsz = min((RDSIZE * 8), dbsize - ibuf_idx);
		if(!ibuf_full)
			if( (retv0 = read(fd0, &Ibuf[ibuf_idx], rsz )) > 0) {
				datavail = 1;
                if((ibuf_idx + retv0) >= dbsize) /* wrapped get an index */
                {
                  Gibuf_idx = (ibuf_idx + retv0) - dbsize; 
                  ibuf_idx = Gibuf_idx; 
				  if(Mbuf_Full == 0){ /* wrap buffer */
					  /* ibuf_idx = 0; */
					  /* reverse_video(regwn); */
					  wprompt(regwn, 58, 17, "Wrapped");
					  /* reverse_video(regwn); */
					  ibwrapped = 1;
				 } else {
                      ibuf_idx = dbsize;
					  ibuf_full = 1;
					  close(fd0);
					  /* reverse_video(regwn); */
					  wprompt(regwn, 58, 17, "Full");
					  /* reverse_video(regwn); */
					  /* DEBUG: notify user */
				  }
                }
                else
                {
				  ibuf_idx += retv0; 
                  Gibuf_idx = ibuf_idx; 

                }
				if(ibwrapped || ibuf_full)
                   Ibufcount = dbsize;
                else
				   Ibufcount = max((unsigned int) ibuf_idx, 
						   Ibufcount);
#ifdef FOOOOOI
				if(ibuf_idx >= dbsize) { /* is the buffer full */
					if(Mbuf_Full == 0){ /* wrap buffer */
						ibuf_idx = 0;
						ibwrapped = 1;
						wprompt(regwn, 58, 17, "Wrapped");
					} else {
							ibuf_full = 1;
							close(fd0);
							/* DEBUG: notify user */
							wprompt(regwn, 58, 17, "Full");
						}
				}
#endif
			}
		rsz = min((RDSIZE * 8), dbsize - obuf_idx);
		if(!obuf_full)
			if((retv1 = read(fd1, &Obuf[obuf_idx], rsz )) > 0) {
				datavail = 1;
                if((obuf_idx + retv1) >= dbsize) /* wrapped get an index */
                {
                  Gobuf_idx = (obuf_idx + retv1) - dbsize; 
                  obuf_idx = Gobuf_idx = 0; 
				  if(Mbuf_Full == 0){ /* wrap buffer */
					  /* obuf_idx = 0; */
					  /* reverse_video(regwn); */
					  wprompt(regwn, 58, 18, "Wrapped");
					  /* reverse_video(regwn); */
					  obwrapped = 1;
				 } else {
                      obuf_idx = dbsize;
					  obuf_full = 1;
					  close(fd1);
					  /* reverse_video(regwn); */
					  wprompt(regwn, 58, 18, "Full");
					  /* reverse_video(regwn); */
					  /* DEBUG: notify user */
				  }
                }
                else
                {
				  obuf_idx += retv1; 
                  Gobuf_idx = obuf_idx; 

                }
				if(obwrapped || obuf_full)
				   Obufcount = dbsize;
                else
				   Obufcount = max((unsigned int) obuf_idx, 
						   Obufcount);
#ifdef FOOOOOI
				if(obuf_idx >= dbsize) { /* is the buffer full */
					if(Mbuf_Full == 0){ /* wrap buffer and continue */
						obuf_idx = 0;
						obwrapped = 1;
						wprompt(regwn, 58, 18, "Wrapped");
			 		} else {
							obuf_full = 1;
							close(fd1);
							wprompt(regwn, 58, 18, "Full");
							/* DEBUG: notify user */
				}
			}
#endif
		}
#define	BSCRUPDATE	256
		ucnt = iucnt = 0;
		if(oscr_idx == obuf_idx && iscr_idx == ibuf_idx){ /* DEBUG: which line*/
			wmputchar(regwn, scrcol, scrrow + 1, ' ');
			wmputchar(regwn, ascol,scrrow +1 , ' '); /* rub out ascii */
			napms(10);  /*  take napms 10 mils: force context switch */ 
			ucnt = iucnt = 0;
		} else { 
			if(oscr_idx != obuf_idx && !(obuf_full)){ /* DEBUG:  */
				if(obuf_idx > oscr_idx){
					ucnt = min((obuf_idx - oscr_idx), BSCRUPDATE);
					oscr_idx = obuf_idx - ucnt;
				} else {
					ucnt = min(((dbsize - oscr_idx) + obuf_idx), BSCRUPDATE); 
					if((dbsize - oscr_idx) < ucnt){
						if(obuf_idx > BSCRUPDATE)
							oscr_idx = obuf_idx - ucnt; 
						else {
							oscr_idx = obuf_idx;
							oscr_idx += (ucnt - oscr_idx);
						}
					}
				}
			} 
			if(iscr_idx != ibuf_idx && !(ibuf_full)){ 
				if(ibuf_idx > iscr_idx){
					iucnt = min((ibuf_idx - iscr_idx), BSCRUPDATE);
					iscr_idx = ibuf_idx - iucnt;
				} else {
					iucnt = min(((dbsize - iscr_idx) + ibuf_idx), BSCRUPDATE); 
					if((dbsize - iscr_idx) < iucnt){
						if(ibuf_idx > BSCRUPDATE)
							iscr_idx = ibuf_idx - iucnt; 
						else {
							iscr_idx = ibuf_idx;
							iscr_idx += (iucnt - iscr_idx);
						}
					}
				}
			}
		}

		ucnt = max(ucnt, iucnt);
		scrcnt = 0;
		while(ucnt--){					
				if(iscr_idx != ibuf_idx  ){ /* input data ? */
					tt = Ibuf[iscr_idx];
					wmputchar(regwn, scrcol, scrrow, hexval[(tt >> 4) & 0x0f]);
					wmputchar(regwn, scrcol+ 1, scrrow, hexval[(tt & 0x0f)]);
					/* print ascii character */
					if(isprint(Ibuf[iscr_idx])){
						wmputchar(regwn, ascol, scrrow, Ibuf[iscr_idx]);
					} else wmputchar(regwn,ascol,scrrow, '.');
					scrcnt++;
					
					if(scrcnt % 128 == 0)
						wrefresh(stdscr);
				
					iscr_idx++;
					if(iscr_idx >= dbsize){
						if(Mbuf_Full == 1)
							break;
						iscr_idx = 0;
					}
				} else {	/* if updating output rub out old data*/
					wmputchar(regwn, scrcol, scrrow, ' ');
					wmputchar(regwn, scrcol+ 1, scrrow, ' ');
					wmputchar(regwn, ascol, scrrow, ' '); /* rub out ascii*/
				}                             /* new code */
				if(oscr_idx != obuf_idx ){/* output data  ? */
					reverse_video(regwn);	
					tt = Obuf[oscr_idx];
					wmputchar(regwn,scrcol,scrrow + 1,hexval[(tt >> 4) & 0x0f]);
					wmputchar(regwn,scrcol + 1, scrrow + 1,hexval[(tt & 0x0f)]);
					/* print ascii character */
					if(isprint(Obuf[oscr_idx])){
						wmputchar(regwn, ascol, scrrow + 1, Obuf[oscr_idx]);
					} else wmputchar(regwn,ascol, scrrow + 1, '.');
					
					reverse_video(regwn);	
					scrcnt++;
					oscr_idx++;
					if(oscr_idx >= dbsize) {
						if(Mbuf_Full == 1)
							break;
						oscr_idx = 0;
					}
				} else {  /* rub out */
					reverse_video(regwn);	
					wmputchar(regwn, scrcol, scrrow + 1, ' ');
					wmputchar(regwn, scrcol + 1, scrrow + 1, ' ');
					wmputchar(regwn, ascol,scrrow + 1, ' '); /* rub out ascii */
					reverse_video(regwn);	
				}

			scrcol+= 3;
			if(scrcol >= 51){
				scrrow += 2;
				scrcol = 3;
			}
			if(scrrow >= 16 )
				scrrow = 0 ;
			ascol++;
			if(ascol >= FASCBEG + 16)
				ascol = FASCBEG;
		}
		if(Dis_Type == 1){
			ftime(&timer);
			new_time = timer.time * 1000L + (unsigned int) timer.millitm;
			if((new_time - last_time) > 100){
				if((retval = ioctl( pfd, EQNCHBUFSTAT,(char *)&bufstat))){
					sprintf(tbuff,"ERROR EQNCHBUFSTAT:%d errno:%d",
						retval, errno);
					close(fd1);		/* close output desc */
					close(fd0);		/* close output desc */
					error_message(tbuff); get_char(); 
					clear_message();
					return(0);
				}
				reverse_video(regwn);	
				/* ---- get control sigs ------*/
				vv = bufstat.ebf_outsigs; 
				ii = bufstat.ebf_insigs;
				if(vv != oldvv || ii != oldii){
					sprintf(sctrsig[RTSBF],"%s", (vv & 0x02 ? on : off) ); 
					sprintf(sctrsig[DTRBF],"%s", (vv & 0x01 ? on : off) ); 
					sprintf(sctrsig[DCDBF],"%s", (ii & 0x01 ? on : off) ); 
					sprintf(sctrsig[CTSBF],"%s", (ii & 0x02 ? on : off) ); 
					field_tally(regwn);
					oldii = ii;
					oldvv = vv;
				}
				last_time = new_time;
				/* update input fill bar */
				fbuf_idx = ibuf_idx;
				fperc = fbuf_idx / dbsize;
				percent = fperc * PBARWIDTH;
				if(ibwrapped)
					percent = PBARWIDTH;
				update_fillbar(regwn, percent, 0);
				/* update output fill bar */
				fbuf_idx = obuf_idx;
				fperc = fbuf_idx / dbsize;
				percent = fperc * PBARWIDTH;
				if(obwrapped)
					percent = PBARWIDTH;
				update_fillbar(regwn, percent, 1);
				reverse_video(regwn);
			}
		}
		/* did the user hit a key */
		if((junk = wgetch(stdscr)) == ERR)
				continue;		/* if not continue */
		else {				/* else we are finished */
			nodelay(stdscr, FALSE);		/* set to delayed reads */
			flushinp();
			done=1;
			break;
		}

	}
 	if(!ibuf_full)
		close(fd0);
	if(!obuf_full)
		close(fd1);	

	return(datavail);

}
void update_fillbar(regwn, percent, direct)
SWIN *regwn;
int percent;
int direct;
{
	int xx, max, line;
	static int ilastper = 0;
	static int olastper = 0;

	if(direct == 0){	/* input */
		if(percent == ilastper)		
			return;
		line = 17;
		ilastper = percent;
	} else {			/* output */
		if(percent == olastper)		
			return;
		if(Mon_Type == MOUTPUT)
			line = 17;
		else 
			line = 18; /* both so add one line */
		olastper = percent;
	}
	xx = 22;
	max = percent + xx;
	/* reverse_video(regwn); */

	for(; xx < max; xx++){
		wmputchar(regwn, xx, line, ' ');
	}		
	/* reverse_video(regwn); */

}

/* -------------------------------------------------------*/

char *reviewchoice[] = {
	"INPUT ",
	"OUTPUT ",
	0
};

extern char regmskten[];
int review_data()
{
	char title[35];
	char offbuf[35];
	char totbuf[35];
	struct field *fld;
	SWIN *regwn;
	SMENU rmen;
	unsigned char *dptr;
	unsigned int dctr,maxcnt, soffset;
	int ascol, charin, scrrow, scrcol, revtype, tt, ccnt;
	if(Obufcount == 0 && Ibufcount == 0){
		warning("Data buffers are empty.");	
		return(0);
	}
	
	if(Obufcount > 0 && Ibufcount > 0){
		rmen.mselcs = reviewchoice;	
		rmen.func = NULL;
		revtype = vscroll(" Review ", &rmen, 1, 33, 13);
		revtype--;
	} else revtype = Mon_Type;

	if(revtype == 0){
		dptr = Ibuf;
		sprintf(title, "Reviewing Input data for %s", eqfilename);	
		maxcnt = dctr = Ibufcount;
        if(!(Gibuf_idx == 0))
             dctr = Gibuf_idx;
	} else {
		dptr = Obuf;
		sprintf(title, "Reviewing Output data for %s", eqfilename);	
		maxcnt = dctr = Obufcount;
        if(!(Gobuf_idx == 0))
            dctr = Gobuf_idx;
	}
	
	/* display the window */	
	regwn = establish_window( 4, 2, 21, 68,  0,
#ifndef	XENIX
			(has_colors() ? 1 : 0), title);
#else 
			 0, title);
#endif
	wprompt(regwn, 1, 17, "Offset:");	
	wprompt(regwn, 1, 18, "Total:");	

	wprompt(regwn, 30, 16, "Key Help");	

	wprompt(regwn, 18, 17, "<Page Up>");	
	wprompt(regwn, 18, 18, "<Page Down>");	
	wprompt(regwn, 30, 17, "<UP Arrow>");	
	wprompt(regwn, 30, 18, "<Down Arrow>");	

	wprompt(regwn, 43, 17, "<Home>");	
	wprompt(regwn, 43, 18, "<End>");	

	wprompt(regwn, 51, 17, "<ESC> to exit");	

	fld = establish_field(regwn, 9, 17,
		regmskten + 3, offbuf, 'a');  
	fld = establish_field(regwn, 9, 18,
		regmskten + 3, totbuf, 'a');  
	reverse_video(regwn);
	for(scrrow = 0; scrrow < 16; scrrow++) {
		ascol = ASCBEG;
		for(scrcol = 0; scrcol < 47; scrcol++) {
				wmputchar(regwn, scrcol, scrrow, ' ');
				wmputchar(regwn, ascol++ , scrrow, ' ');
			if(ascol >= ASCBEG + 16)
				ascol = ASCBEG;
		}
	}
	
	ascol = ASCBEG;
	scrcol = scrrow = 0;
	sprintf(totbuf, "%7d",maxcnt);		
	for(;;){
		if(dctr >= 256){	/* point to beginning of a block */
			ccnt = 256;
			dctr = dctr - 256;
		} else {
			ccnt = dctr;	
			dctr = 0;
		}
		ascol = ASCBEG;
		scrcol = scrrow = 0;
		soffset = dctr;		/* get offset of char in upper left corner */
		sprintf(offbuf, "%7d", soffset);		
		/* display the block */
		while(ccnt--){
			tt = dptr[dctr];
            if(revtype == 0)
            {   
                if(Gibuf_idx > 0) {  /* point pervious if possible */
                   if(dctr == (Gibuf_idx - 1)) reverse_video(regwn);
                 } else
                    if(dctr == Gobuf_idx) reverse_video(regwn);
                    
            } else {
                if(Gobuf_idx > 0) {
                    if(dctr == (Gobuf_idx -1)) reverse_video(regwn);
                } else
                    if(dctr == Gobuf_idx) reverse_video(regwn);
            }

			wmputchar(regwn, scrcol, scrrow, hexval[(tt >> 4) & 0x0f]);
			wmputchar(regwn, scrcol+ 1, scrrow, hexval[(tt & 0x0f)]);
			/* print ascii character */
			if(isprint(dptr[dctr])){
				wmputchar(regwn, ascol, scrrow, dptr[dctr]);
			} else wmputchar(regwn,ascol,scrrow, '.');
            if(revtype == 0)
            {   
                if(Gibuf_idx > 0) {
                   if(dctr == (Gibuf_idx - 1)) reverse_video(regwn);
                 } else
                    if(dctr == Gobuf_idx) reverse_video(regwn);
                    
            } else {
                if(Gobuf_idx > 0) {
                    if(dctr == (Gobuf_idx -1)) reverse_video(regwn);
                } else
                    if(dctr == Gobuf_idx) reverse_video(regwn);
            }
			dctr++;
			scrcol+= 3;
			if(scrcol >= 48){
				scrrow++;
				scrcol = 0;
			}
			if(scrrow >= 16 )
				scrrow = 0 ;
			ascol++;
			if(ascol >= ASCBEG + 16)
				ascol = ASCBEG;
			
		}
		field_tally(regwn);
		charin = get_char();
		switch(charin){
			case '\r':
			case '\t':
			case 'j':
			case KEY_ENTER:
			case DN:
			case KEY_DOWN:
			case KEY_RIGHT:
				if(maxcnt > 256){
					if(dctr + 16 <= maxcnt ) {
						dctr += 16;
					} else {
						dctr = maxcnt;					
						
					}
				}
				break;
			case UP:
			case BS:
			case KEY_BACKSPACE:
			case KEY_LEFT:
			case 'k':
			case KEY_UP:
				if(maxcnt > 256){
					if(dctr >= 16 + 256){
						dctr -= 16;
					} else {
						dctr = 256;
					}
				}
			
				break;
			case KEY_NPAGE:
			case 0x0e:
				if(maxcnt > 256){
					if(dctr + 256 <= maxcnt ) {
						dctr += 256;
					} else {
						dctr = maxcnt;					
					}
				}
				
				break;
			case KEY_PPAGE:
			case 0x10:
				if(maxcnt > 256){
					if(dctr >= 512){
						dctr -= 256;
					} else {
						dctr = 256;
					}
				}
				break;
			case 0x0c:
				/* error_message(" CTRL_L");	
				get_char(); clear_message();
				*/
				wrefresh(curscr);
				break;
			case 0x06:
				break;
			case KEY_HOME:
			case 0x02:
				dctr = 256;
				break;
			case 0x05:
			case KEY_END:
				dctr = maxcnt;					
				break;

			default:
				break;

		}
		if(charin == ESC)
			break;
	}
	reverse_video(regwn);
	delete_window(regwn);
	return(0);

}
char *savechoice[] = {
	"Save Input ",
	"Save Output ",
	0
};
int esave_to_file()
{
	SWIN *svwn;
	SMENU smen;
	struct field *fld;
	char tbuff[50];
	char erbuf[50];
	unsigned char *dptr;
	int n, wrote, maxcnt, sfd, idx, revtype = 0;	

	wrote = 0; 
	if(Obufcount == 0 && Ibufcount == 0){
		error_message(" No data avaible");	
		get_char(); clear_message();
		return(0);
	}
	
	if(Obufcount > 0 && Ibufcount > 0){
		smen.mselcs = savechoice;	
		smen.func = NULL;
		revtype = vscroll(" Save ", &smen, 1, 33, 13);
		revtype--;
	} else revtype = Mon_Type;

	svwn = establish_window(15, 12, 3, 45, 0, 0, "");
	field_pass_mode(svwn, 1);
	wprompt(svwn, 5, 0, "Filename:");
	init_template(svwn); 
	fld = establish_field(svwn, 14, 0, msk25, tbuff, 'a'); 
	
	field_window(fld,"pname   ", 19, 7);
	if(revtype == 0)
		strcpy(tbuff, "/tmp/eqnin");
	else 
		strcpy(tbuff, "/tmp/eqnout");	
	field_tally(svwn);
	if(data_entry(svwn) == ESC) {
		delete_window(svwn);
		return(0);	
	}
	for(idx = 0; idx < 25; idx++)
		if(*(tbuff + idx) == ' ' || !tbuff[idx]){
			*(tbuff + idx) = '\0';
		}

	if((sfd = open(tbuff, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1){
		sprintf(erbuf, "ERROR open:%s errno:%d", tbuff, errno);
		error_message(erbuf);	
		get_char(); clear_message();
		delete_window(svwn);
		return(0);
	}

	if(revtype == 0){
		dptr = Ibuf;
		maxcnt = Ibufcount;
        sprintf(tbuff, "Index 0x%x  \n", Gibuf_idx);
	} else {
		dptr = Obuf;
		maxcnt = Obufcount;
        sprintf(tbuff, "Index 0x%x  \n", Gobuf_idx);
	}

	while(wrote < maxcnt){
		if((n = write(sfd,&dptr[wrote], min(BUFSIZE, maxcnt - wrote))) == -1){
			sprintf(erbuf, "ERROR write:%s errno:%d", tbuff, errno);
			error_message(erbuf);	
			get_char(); clear_message();
			delete_window(svwn);
			return(0);
		}
		wrote += n;
	}	
    
    write(sfd, tbuff , strlen(tbuff) );
	close(sfd);	
	delete_window(svwn);

	return(0);

}
int set_trigger()
{
	return(0);
}




