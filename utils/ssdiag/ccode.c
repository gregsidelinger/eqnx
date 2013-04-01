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
 * Equinox SuperSerial MultiModem country code utility
 */

/*Include Files */
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

#include <curses.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

typedef unsigned long paddr_t;
#define ulong_t unsigned long
#define	uint_t	unsigned int
#define ushort_t unsigned short
#define uchar_t unsigned char

#include "twindow.h"
#include "cstream.h"
#include "icp.h"
#include "eqnx_ioctl.h"
#include "brdtab.h"

#define SSMAXBRD 8
#define SSDBASE "/etc/eqnx/database"

/*-----------------------------------------*/

/* Qualify an eqn device */
/*-----------------------------------------*/

char dvstr[]="/dev/ttyQ";

char *country_name[] = {"AUSTRIA", "BELGIUM", "DENMARK", "FINLAND", "FRANCE",\
	"GERMANY", "GREECE", "ICELAND", "IRELAND", "ITALY", "LUXEMBOURG",\
	"NETHERLANDS", "NORWAY", "PORTUGAL", "SPAIN", "SWEDEN", "SWITZERLAND",\
	"UK", "US"};
char *country_code[] = {"0A", "0F", "31", "3C", "3D", "42", "46", "52", "57",\
	"59", "69", "7B", "82", "8B", "A0", "A5", "A6", "B4", "B5"};

/* char def_country[] = "B5"; */
char ddevice[]="/dev/eqn";
int phys_log[SSMAXBRD];

chtype SHADATT;

int flow_ctrl=0;
int baud =B38400;

struct mmboard {
	int boardnum;
	int modem_count;
} mmb[SSMAXBRD];

int build_brd_list();
int set_country_code();
int fill_board_struct( struct eqnbrdstatus *bsptr, int brd);
int phys_to_log(int brd);
int prompt_input(int total, int index);

int main() 
{
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
		attron(COLOR_PAIR(1));
		}
	}

	build_brd_list();
	set_country_code();

	erase();
	move(23,0);
	refresh();
	endwin();
	resetty();
	nl();echo();
	eqnx_exit(0);
}




int set_country_code()
{
	struct eqnbrdstatus brdstat;

	int board, bid, i, j;
	int mb_idx=0, modem_total=0;
	int maxboard, ccode = -1;
	char portname[13];
	char errbuff[80], tbuff[80];
	char cmnd[9], inbuf[15];
	char get_cmnd[]="AT+GCI?\r";
	char set_cmnd[]="AT+GCI=";
	FILE *mport = NULL;
	int  mport_fno;
	struct termios ttys;

#define AT_COMMAND_SIZE 7

	if(! fill_board_struct(&brdstat, 1))
		return(0);
	maxboard = brdstat.gbl_neqnbrds;
	for(board=1; board < maxboard + 1; board++) {

		if(! fill_board_struct(&brdstat, board))
			return(0);

		if(! brdstat.alive)
			break;
        	bid = (brdstat.id & 0xFC); 

		if((bid != 0x98) && (bid != 0x9C)) {
			continue;	
		} else {
			mmb[mb_idx].boardnum = phys_to_log(brdstat.brd_nbr);
			if(bid == 0x98) {
				mmb[mb_idx].modem_count = 8;
				modem_total += 8;
			} else {
				mmb[mb_idx].modem_count = 4;
				modem_total += 4;
			}
			mb_idx++;
		}

	}
	if(! mb_idx) {
	 	sprintf(errbuff,"No Multi-Modem boards found");
		error_message(errbuff); get_char(); clear_message();
		return(0);

	}  /* (modem_total) modems found on (mb_idx) MM boards */

	/*   Get modem code input from user 
	     Uses curses interface in screen.c */

	ccode = prompt_input(modem_total, mb_idx);

	if( ccode == -1)
		return(0);
	/* loop each modem board */
	for( i=0; i < mb_idx; i++) {

		/* open each port on board; send country code (+GCI) */
		for( j=0; j < mmb[i].modem_count; j++) {
			sprintf(portname, "%s%da%d", dvstr, mmb[i].boardnum, j + 1);
			mport = fopen(portname, "r+");
			if(mport == NULL) {
				sprintf(errbuff,"ERROR:fopen, %s ", portname);
				error_message(errbuff); get_char(); clear_message();
				continue;
			}
			mport_fno = fileno(mport);
			if (tcgetattr (mport_fno, &ttys)) {
				perror("tcgetattr: ");
			}
			ttys.c_lflag &= ~(ECHO|ECHONL|ECHOK);
			if (tcsetattr (mport_fno, TCSANOW, &ttys)) {
				perror("tcsetattr: ");
			}

			/* write AT+GCI and cpuntry code out to modem */
/*	
			sprintf(errbuff,"Sending AT command to %s", portname);
			error_message(errbuff); get_char(); clear_message();
*/	
			sprintf(cmnd,"%s%s\r\n", set_cmnd, country_code[ccode]);
			fputs(cmnd, mport);
/*
			sprintf(errbuff,"Sent AT command to %s", portname);
			error_message(errbuff); get_char(); clear_message();
*/
			fscanf(mport, "%s", inbuf); 
			if(!strncmp(inbuf, cmnd, AT_COMMAND_SIZE)) {

				/* modem echo is enabled, get next string */
				fscanf(mport, "%s", inbuf); 
			}
			if(strcmp(inbuf, "OK")) {
				sprintf(errbuff,"%s modem command failed:%s ", portname, inbuf);
				error_message(errbuff); get_char(); clear_message();
				if (fclose(mport) !=0 ) {
					sprintf(errbuff,"Error closing port ");
					error_message(errbuff); get_char(); clear_message();
				}
				continue;
			}	


			/* read back code and verify */
	/*
			sprintf(errbuff,"Reading AT command from %s", portname);
			error_message(errbuff); get_char(); clear_message();
	*/
			fclose(mport);
			if((mport = fopen(portname, "r+")) == NULL) {
				sprintf(errbuff,"ERROR:fopen, %s ", portname);
				error_message(errbuff); get_char(); clear_message();
				return(0);
			}
			mport_fno = fileno(mport);
			if (tcgetattr (mport_fno, &ttys)) {
				perror("tcgetattr: ");
			}
			ttys.c_lflag &= ~(ECHO|ECHONL|ECHOK);
			if (tcsetattr (mport_fno, TCSANOW, &ttys)) {
				perror("tcsetattr: ");
			}
			fputs(get_cmnd, mport);
			fscanf(mport, "%s%s", tbuff, inbuf); 
			if(!strncmp(tbuff, get_cmnd, AT_COMMAND_SIZE)) {

				/* modem echo is enabled, get next string */
				fscanf(mport, "%s", inbuf); 
			}
			if(strcmp(inbuf, country_code[ccode])) {
			   sprintf(errbuff,"%s country code not set ", portname);
			   error_message(errbuff); get_char(); clear_message();
			}	
			
			fclose(mport);
		} /* End Modem loop */
	} /* End Board loop */
return(0);
}


int fill_board_struct( struct eqnbrdstatus *bsptr, int brd)
{
	int retval, pfd;
	char tbuff[80];

    pfd = open( ddevice, O_WRONLY | O_NDELAY | O_NONBLOCK | O_NOCTTY );
	if ( pfd < 0 ) {
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

int build_brd_list()
{
	struct eqnbrdstatus brdstat;
	char tbuff[80];
	char datain[80];
	int cnt, log, phys;
	FILE *datab=NULL;

#ifndef	UNISYS
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

		/*
		sprintf(tbuff,"Phys %d Log %d\n", phys, log);
		error_message(tbuff); get_char(); 
		clear_message();
		*/
	}
	fclose(datab);
#else
	/* install linear board list UNISYS */
	for(cnt = 1; cnt < 9; cnt++)
		phys_log[cnt - 1] = cnt;
#endif
	return(0);
}

int prompt_input(int total, int index)
{
	int i, row=0;
	int select, key;
	char modem_str[45], mmname_str[55];
	char esc_str[40];
	SWIN *mmwn, *ccwn, *okwn;


	standend();
	/* --------------- Open a window ----------------*/
	mmwn = establish_window( 10, 6, 8 + index, 46,
		 0,1, " MultiModem Boards ");
	wcursor(mmwn, 1, row++);

	/*----- print modem and board totals.----------*/
	sprintf(modem_str,"Found %d modems on the following %d boards:", \
		total, index);
	Wprintf(mmwn, modem_str);

	wcursor(mmwn, 1, row++);

	/*----- print Multimodem boards and their logical number.--------*/
	for(i =0; i< index; i++) {
		wcursor(mmwn, 10, row++);
		sprintf(mmname_str,"MM%dP, ports %da1 - %da%d", \
			mmb[i].modem_count, mmb[i].boardnum, \
			mmb[i].boardnum, mmb[i].modem_count);
		Wprintf(mmwn, mmname_str);
	}
	wcursor(mmwn, 4, 3 + index);
	sprintf(esc_str,"Use up/down arrows to choose country");
	Wprintf(mmwn, esc_str);
	wcursor(mmwn, 12, 5 + index);
	sprintf(esc_str,"Press ESC to Cancel");
	Wprintf(mmwn, esc_str);

	/* make sure we have a visible cursor */
	curs_set(0);

	/* display country names */
	ccwn = establish_window( 57, 1, 21, 16, 0, 1, " Countries ");
	for (i=0; i<19; i++) {
		wcursor(ccwn, 1, i);
		sprintf(esc_str,"%s", country_name[i]);
		if(!i)
			reverse_video(ccwn);
		Wprintf(ccwn, esc_str);
		if(!i)
			reverse_video(ccwn);
	}
	select = key = 0;
	while ( key != ESC && key != '\r' ) {
		key = get_char();
		switch (key) {
			case KEY_UP:
				wcursor(ccwn, 1, select);
				sprintf(esc_str,"%s", country_name[select]);
				Wprintf(ccwn, esc_str);
				if (select > 0)
					--select;
				else
					select=18;
				reverse_video(ccwn);
				wcursor(ccwn, 1, select);
				sprintf(esc_str,"%s", country_name[select]);
				Wprintf(ccwn, esc_str);
				reverse_video(ccwn);
				break;
			case KEY_DOWN:
				wcursor(ccwn, 1, select);
				sprintf(esc_str,"%s", country_name[select]);
				Wprintf(ccwn, esc_str);
				if (select < 18)
					++select;
				else
					select=0;
				reverse_video(ccwn);
				wcursor(ccwn, 1, select);
				sprintf(esc_str,"%s", country_name[select]);
				Wprintf(ccwn, esc_str);
				reverse_video(ccwn);
				break;
			default:
				beep();
				break;
		}
	}
	if(key == ESC) {
		delete_window(mmwn);	/* close window */
		delete_window(ccwn);	/* close window */
		return(-1);	
	}
	okwn = establish_window( 3, 20, 3, 20, 0, 1, "");
	wcursor(okwn, 2, 0);
	sprintf(esc_str,"Setting modems");
	Wprintf(okwn, esc_str);
	return(select);
}

